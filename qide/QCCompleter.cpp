#include "fmt/core.h"

#include "rapidfuzz/utils.hpp"

#include <QApplication>
#include <QDebug>
#include <QMultiMap>
#include <QKeyEvent>

#include "QCLexer.hpp"
#include "QCCompleter.hpp"
#include "QCEdit.hpp"

QCCompleter::QCCompleter(QCEdit *qcEdit, QWidget *parent)
	: QWidget(parent)
	, m_qcEdit(nullptr)
	, m_listView(this)
{
	hide();

	setContentsMargins(0, 0, 0, 0);
	//setWindowFlags(Qt::Popup);

	m_listView.setContentsMargins(0, 0, 0, 0);
	m_listView.setModel(&m_model);

	setQcEdit(qcEdit);
}

QStringList QCCompleter::complete(const QString &tok){
	QMultiMap<double, QStringView> results;

	const auto tokStr = tok.toStdString();
	const auto scorer = rapidfuzz::fuzz::CachedRatio<std::string>(tokStr);

	constexpr double scoreCutoff = 50.0;

	foreach(const auto &choice, m_choices){
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

void QCCompleter::setQcEdit(QCEdit *qcEdit_){
	if(m_qcEdit){
		disconnect(m_qcEdit, &QTextEdit::textChanged, this, nullptr);
		disconnect(m_qcEdit, SIGNAL(cursorPositionChanged()), this, SLOT(completeAtCursor()));
	}

	auto oldEdit = m_qcEdit;
	m_qcEdit = qcEdit_;

	if(qcEdit_){
		m_listView.setFont(qcEdit_->font());
		connect(qcEdit_, &QTextEdit::textChanged, this, [this]{ completeAtCursor(true); });
		connect(qcEdit_, SIGNAL(cursorPositionChanged()), this, SLOT(completeAtCursor()));
	}

	if(!parent() || parent() == oldEdit){
		setParent(qcEdit_);
	}

	emit qcEditChanged();
}

void QCCompleter::closePopup(){
	hide();
}

void QCCompleter::completeAtCursor(bool forceShow){
	if(isHidden() && !forceShow){
		return;
	}

	hide();

	if(!m_qcEdit){
		return;
	}

	auto textCursor = m_qcEdit->textCursor();
	auto beforeCursor = textCursor;
	beforeCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);

	if(beforeCursor.selectedText() == " "){
		return;
	}

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

	auto cursorRect = m_qcEdit->cursorRect(textCursor);
	int xOff = m_qcEdit->lineNumberAreaWidth() + ((tok->location().col + 0.25) * charWidth);
	int yOff = cursorRect.y() + (fntMetrics.height() * 1.25);

	// TODO: handle tabs

	setGeometry(xOff, yOff, xSize, ySize);
	m_listView.setGeometry(0, 0, xSize, ySize);
	m_listView.setCurrentIndex(m_model.index(0));

	show();
	setFocus(Qt::PopupFocusReason);
}

void QCCompleter::keyPressEvent(QKeyEvent *ev){
	if(ev->key() >= Qt::Key_A && ev->key() <= Qt::Key_Z){
		ev->ignore();
		completeAtCursor(true);
		return;
	}
	else if(isHidden()){
		ev->ignore();
		return;
	}
	else if(ev->key() >= Qt::Key_0 && ev->key() <= Qt::Key_9){
		ev->ignore();
		completeAtCursor(true);
		return;
	}

	int idx = m_listView.currentIndex().row();

	switch(ev->key()){
		case Qt::Key_Up:{
			ev->accept();
			idx = std::max(0, idx - 1);
			m_listView.setCurrentIndex(m_model.index(idx));
			break;
		}

		case Qt::Key_Down:{
			ev->accept();
			idx = std::min(m_model.rowCount() - 1, idx + 1);
			m_listView.setCurrentIndex(m_model.index(idx));
			break;
		}

		case Qt::Key_Escape:{
			ev->accept();
			hide();
			break;
		}

		case Qt::Key_Semicolon:
		case Qt::Key_Space:{
			ev->ignore();
			hide();
			break;
		}

		case Qt::Key_Return:
		case Qt::Key_Enter:{
			ev->accept();
			auto val = m_model.data(m_model.index(idx)).toString();

			auto textCursor = m_qcEdit->textCursor();

			auto tok = m_qcEdit->lexer()->closest(QCToken::Location{ textCursor.blockNumber(), textCursor.columnNumber() });

			qDebug() << "Cursor pos:" << textCursor.columnNumber() << "Token pos:" << tok->location().col;

			textCursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
			textCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveAnchor, tok->location().col);
			textCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::KeepAnchor, tok->str().size());

			textCursor.removeSelectedText();
			textCursor.insertText(val);

			m_qcEdit->reparse();

			hide();
			break;
		}

		default: ev->ignore(); break;
	}
}
