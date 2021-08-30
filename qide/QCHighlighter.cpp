#include "fmt/format.h"

#include <QRegularExpression>

#include "QCHighlighter.hpp"
#include "QCLexer.hpp"
#include "QCType.hpp"

QCHighlighter::QCHighlighter(QTextDocument *doc, QCParser *parser)
	: QSyntaxHighlighter(doc)
	, m_parser(parser)
{
	m_tokenFmt[QCToken::Id].setForeground(Qt::magenta);
	m_tokenFmt[QCToken::Type].setForeground(QColor(224, 109, 0));
	m_tokenFmt[QCToken::Keyword].setForeground(QColor(99, 173, 242));
	m_tokenFmt[QCToken::Number].setForeground(QColor(228, 255, 26));
	m_tokenFmt[QCToken::Comment].setForeground(Qt::darkGray);
	m_tokenFmt[QCToken::String].setForeground(Qt::yellow);
	m_tokenFmt[QCToken::GlobalId].setForeground(Qt::cyan);
}

void QCHighlighter::highlightBlock(const QString &text){
	if(currentBlock().blockNumber() == 1){
		m_lexer.reset();
	}

	int startIdx = m_lexer.tokens().size();

	int n = m_lexer.lex(text);

	if(n <= 0) return;

	auto it = &m_lexer.tokens()[startIdx];
	const auto end = m_lexer.tokens().end();

	while(it != end){
		const auto &tok = *it;

		const auto tokStart = std::distance(text.begin(), tok.str().begin());

		/*if(auto expr = m_parser->atLocation(tok.location());){
			highlightParsed(tokStart, expr);
			it = expr->end();
		}
		else */if(tok.kind() < QCToken::count){
			auto tokLen = tok.str().length();
			setFormat(tokStart, tokLen, m_tokenFmt[tok.kind()]);
			++it;
		}
		else{
			break;
		}
	}
}

void QCHighlighter::highlightParsed(int startIdx, const QCExpr *expr){
	QTextCharFormat format;
	format.setForeground(Qt::blue);
	setFormat(startIdx, expr->str().length(), format);
}
