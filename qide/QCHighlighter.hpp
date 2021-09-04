#ifndef QIDE_QCHIGHLIGHTER_HPP
#define QIDE_QCHIGHLIGHTER_HPP 1

#include <QVector>
#include <QSyntaxHighlighter>

#include "QCToken.hpp"
#include "QCExpr.hpp"

class QCLexer;
class QCParser;

class QCHighlighter: public QSyntaxHighlighter{
	Q_OBJECT

	public:
		explicit QCHighlighter(QObject *parent = nullptr);
		explicit QCHighlighter(QTextDocument *parent);

		void highlightBlock(const QString &text) override;

	private:
		void highlightParsed(int startIdx, const QCExpr *expr);

		QCLexer *m_lexer;
		QCParser *m_parser;

		QTextCharFormat m_tokenFmt[QCToken::Kind::count];
};

#endif // !QIDE_QCHIGHLIGHTER_HPP
