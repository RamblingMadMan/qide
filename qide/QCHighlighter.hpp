#ifndef QIDE_QCHIGHLIGHTER_HPP
#define QIDE_QCHIGHLIGHTER_HPP 1

#include <QVector>
#include <QSyntaxHighlighter>

#include "QCLexer.hpp"
#include "QCParser.hpp"

class QCHighlighter: public QSyntaxHighlighter{
	Q_OBJECT

	public:
		QCHighlighter(QTextDocument *doc, QCParser *parser);

		void highlightBlock(const QString &text) override;

	private:
		void highlightParsed(int startIdx, const QCExpr *expr);

		QCParser *m_parser;
		QCLexer m_lexer;
		QCToken::Location m_curLoc;

		QTextCharFormat m_tokenFmt[QCToken::Kind::count];
};

#endif // !QIDE_QCHIGHLIGHTER_HPP
