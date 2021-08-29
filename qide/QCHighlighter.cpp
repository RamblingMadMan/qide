#include "fmt/format.h"

#include <QRegularExpression>

#include "QCHighlighter.hpp"
#include "QCLexer.hpp"
#include "QCType.hpp"

QCHighlighter::QCHighlighter(QTextDocument *doc, QCParser *parser)
	: QSyntaxHighlighter(doc)
	, m_parser(parser)
{
	QObject::connect(parser, &QCParser::resultsChanged, this, &QCHighlighter::rehighlight);
}

void QCHighlighter::highlightBlock(const QString &text){
	QTextCharFormat idFormat;
	idFormat.setForeground(Qt::white);

	QTextCharFormat tyFormat;
	tyFormat.setFontWeight(QFont::Bold);
	tyFormat.setForeground(QColor(224, 109, 6));

	QTextCharFormat kwFormat;
	kwFormat.setForeground(QColor(99, 173, 242));

	QTextCharFormat numFormat;
	numFormat.setForeground(QColor(228, 255, 26));

	QTextCharFormat commentFormat;
	commentFormat.setForeground(Qt::darkGray);

	m_lexer.setSrc(text);

	while(m_lexer.hasTokens()){
		auto tok = m_lexer.lex();

		auto srcLoc = QCToken::Location{ currentBlock().lineCount(), tok.location().col };

		auto expr = m_parser->atLocation(srcLoc);
		if(expr){
			highlightParsed(tok.location(), expr);
		}
		else{
			QTextCharFormat *fmt = [&]() -> QTextCharFormat*{
				switch(tok.kind()){
					case QCToken::Id: return qcKeywords.contains(tok.str().toString()) ? &kwFormat : &idFormat;
					case QCToken::Type: return &tyFormat;
					case QCToken::Number: return &numFormat;
					case QCToken::Comment: return &commentFormat;
					default: return nullptr;
				}
			}();

			if(fmt){
				auto tokStart = std::distance(text.begin(), tok.str().begin());
				auto tokLen = tok.str().length();
				setFormat(tokStart, tokLen, *fmt);
			}
		}
	}
}

void QCHighlighter::highlightParsed(QCToken::Location blockLoc, const QCExpr *expr){

}
