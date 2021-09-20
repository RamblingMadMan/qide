#include "fmt/core.h"

#include "rapidfuzz/utils.hpp"

#include <QApplication>
#include <QMultiMap>

#include "QCLexer.hpp"
#include "QCCompleter.hpp"
#include "QCEdit.hpp"

QCCompleter::QCCompleter(QCEdit *qcEdit, QObject *parent)
	: QObject(parent)
	, m_qcEdit(nullptr)
	, m_popup()
	, m_listView(&m_popup)
{
	m_popup.hide();
	m_popup.setContentsMargins(0, 0, 0, 0);
	//m_popup.setWindowFlags(Qt::Popup);

	m_listView.setContentsMargins(0, 0, 0, 0);
	m_listView.setModel(&m_model);

	setQcEdit(qcEdit);
}

QStringList QCCompleter::complete(const QString &tok){
	QMultiMap<double, QStringView> results;

	const auto tokStr = tok.toStdString();
	const auto scorer = rapidfuzz::fuzz::CachedRatio<std::string>(tokStr);

	constexpr double scoreCutoff = 50.0;

	for(const auto &choice : m_choices){
		auto score = scorer.ratio(choice.toStdString(), scoreCutoff);

		if(score >= scoreCutoff){
			//fmt::print("score '{}' <=> '{}': {}\n", tok.toStdString(), choice.toStdString(), score);
			results.insert(score, choice);
		}
	}

	QStringList ret;

	std::reverse(results.begin(), results.end());

	for(auto &&result : results){
		ret.append(result.toString());
	}

	return ret;
}

void QCCompleter::setQcEdit(class QCEdit *qcEdit_){
	if(m_qcEdit){
		disconnect(m_qcEdit, &QPlainTextEdit::textChanged, this, &QCCompleter::completeAtCursor);
		//disconnect(m_qcEdit, &QPlainTextEdit::cursorPositionChanged, this, &QCCompleter::completeAtCursor);
	}

	m_qcEdit = qcEdit_;

	m_popup.setParent(m_qcEdit);

	if(qcEdit_){
		m_listView.setFont(qcEdit_->font());
		connect(qcEdit_, &QPlainTextEdit::textChanged, this, &QCCompleter::completeAtCursor);
		//connect(qcEdit, &QPlainTextEdit::cursorPositionChanged, this, &QCCompleter::completeAtCursor);
	}

	if(!parent()){
		setParent(qcEdit_);
	}

	emit qcEditChanged();
}

void QCCompleter::closePopup(){
	m_popup.hide();
}

void QCCompleter::completeAtCursor(){
	m_popup.hide();

	if(!m_qcEdit){
		return;
	}

	auto textCursor = m_qcEdit->textCursor();
	auto cursorRect = m_qcEdit->cursorRect(textCursor);

	//fmt::print("completing at cursor {{ {}, {} }}\n", textCursor.blockNumber(), textCursor.positionInBlock());
	//std::fflush(stdout);

	auto tok = m_qcEdit->lexer()->closest(QCToken::Location{ textCursor.blockNumber(), textCursor.columnNumber() });

	if(!tok || (tok->kind() != QCToken::Id && tok->kind() != QCToken::Keyword)){
		return;
	}

	auto matches = complete(tok->str().toString()).mid(0, 6);

	if(matches.empty()){
		return;
	}

	m_model.setStringList(matches);

	//fmt::print("Matches for '{}': {{ \"{}\"", tok->str().toString().toStdString(), matches[0].toStdString());

	int maxLen = matches[0].length();

	for(auto it = matches.cbegin() + 1; it != matches.cend(); ++it){
		//fmt::print(", \"{}\"", it->toStdString());
		maxLen = qMax(maxLen, it->length());
	}

	//fmt::print(" }}\n");
	//std::fflush(stdout);

	QFontMetricsF fntMetrics(m_qcEdit->font());

	int sbWidth = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

	qreal charWidth = fntMetrics.horizontalAdvance(QLatin1Char('9'));
	int xSize = sbWidth + (charWidth * (maxLen + 1));
	int ySize = sbWidth + (fntMetrics.height() * (matches.size() + 1));

	int xOff = m_qcEdit->lineNumberAreaWidth() + ((tok->location().col + 0.25) * charWidth);
	int yOff = cursorRect.y() + (fntMetrics.height() * 1.25);

	// TODO: handle tabs

	m_popup.setGeometry(xOff, yOff, xSize, ySize);
	m_listView.setGeometry(0, 0, xSize, ySize);
	m_listView.setCurrentIndex(m_model.index(0, 0));

	m_popup.show();
}
