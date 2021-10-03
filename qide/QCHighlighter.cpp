#include "fmt/format.h"

#include <QDebug>
#include <QTextDocument>
#include <QRegularExpression>

#include "QCHighlighter.hpp"
#include "QCLexer.hpp"
#include "QCParser.hpp"
#include "QCType.hpp"

#include "QuakeColors.hpp"

QCHighlighter::QCHighlighter(QObject *parent)
	: QSyntaxHighlighter(parent)
	, m_lexer(new QCLexer(this))
	, m_parser(new QCParser(this))
{
	m_tokenFmt[QCToken::Id].setForeground(QColor(Qt::white).darker(66));
	m_tokenFmt[QCToken::Op].setForeground(quakeYello.darker(66));
	m_tokenFmt[QCToken::Type].setForeground(QColor(224, 109, 0));
	m_tokenFmt[QCToken::Keyword].setForeground(QColor(99, 173, 242));
	m_tokenFmt[QCToken::Number].setForeground(QColor(228, 255, 26));
	m_tokenFmt[QCToken::Comment].setForeground(Qt::darkGray);
	m_tokenFmt[QCToken::String].setForeground(Qt::yellow);
	m_tokenFmt[QCToken::GlobalId].setForeground(Qt::cyan);
}

QCHighlighter::QCHighlighter(QTextDocument *doc)
	: QCHighlighter((QObject*)doc)
{
	setDocument(doc);
}

void QCHighlighter::highlightBlock(const QString &text){
	if(currentBlock().blockNumber() == 0){
		m_lexer->reset();
	}

	int startIdx = m_lexer->tokens().size();

	auto textView = QStringView(text);

	int n = m_lexer->lex(textView);

	if(n <= 0) return;

	auto it = &m_lexer->tokens()[startIdx];
	const auto end = m_lexer->tokens().end();

	while(it != end){
		const auto &tok = *it;

		const auto tokStart = std::distance(textView.begin(), tok.str().begin());

		const QSignalBlocker block{document()};

		if(auto expr = m_parser->atLocation(tok.location())){
			highlightParsed(tokStart, expr);
			it = expr->end();
		}
		else if(tok.kind() < QCToken::count){
			const auto tokLen = tok.str().length();
			setFormat(tokStart, tokLen, m_tokenFmt[tok.kind()]);
			++it;
		}
		else{
			break;
		}
	}
}

void QCHighlighter::highlightParsed(int startIdx, const QCExpr *expr){
	const QSignalBlocker block{document()};
	QTextCharFormat format;
	format.setForeground(Qt::blue);
	setFormat(startIdx, expr->str().length(), format);
}
