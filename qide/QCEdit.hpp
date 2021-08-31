#ifndef QIDE_QCEDIT_HPP
#define QIDE_QCEDIT_HPP

#include <QDir>
#include <QPlainTextEdit>

#include "QCHighlighter.hpp"
#include "QCCompleter.hpp"
#include "QCParser.hpp"

class QCEdit;

class LineNumberArea : public QWidget{
	Q_OBJECT

	public:
		LineNumberArea(QCEdit *plainEdit_);

		QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent *event);

	private:
		QCEdit *m_plainEdit;
};

class QCEdit: public QPlainTextEdit{
	Q_OBJECT

	Q_PROPERTY(QDir fileDir READ fileDir NOTIFY fileDirChanged)
	Q_PROPERTY(QCLexer lexer READ lexer)
	Q_PROPERTY(QCParser parser READ parser)

	public:
		explicit QCEdit(QWidget *parent = nullptr);

		bool loadFile(const QDir &dir);

		const QDir &fileDir() const noexcept{ return m_fileDir; }
		const QCLexer *lexer() const noexcept{ return &m_lexer; }
		const QCParser *parser() const noexcept{ return &m_parser; }

		void lineNumberAreaPaintEvent(QPaintEvent *event);
		int lineNumberAreaWidth();

	signals:
		void fileDirChanged();

	private slots:
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &rect, int dy);

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private:
		void reparse();

		QDir m_fileDir;
		QString m_plainStr;
		QCLexer m_lexer;
		QCParser m_parser;
		QCHighlighter m_highlighter;
		QCCompleter m_completer;
		LineNumberArea m_lineNumArea;

		void setDefaultFont();
};

#endif // QIDE_QCEDITOR_HPP
