#ifndef QIDE_QCHIGHLIGHTER_HPP
#define QIDE_QCHIGHLIGHTER_HPP 1

#include <QVector>
#include <QSyntaxHighlighter>

#include "QCLexer.hpp"
#include "QCParser.hpp"

inline const QVector<QString> qcKeywords = {
	"var", "return", "typedef", "for", "if", "else"
};

class QCHighlighter: public QSyntaxHighlighter{
	Q_OBJECT

	public:
		QCHighlighter(QTextDocument *doc, QCParser *parser);

		void highlightBlock(const QString &text) override;

	private:
		void highlightParsed(QCToken::Location blockLoc, const QCExpr *expr);

		QCParser *m_parser;
		QCLexer m_lexer;
		QCToken::Location m_curLoc;
};

#endif // !QIDE_QCHIGHLIGHTER_HPP
