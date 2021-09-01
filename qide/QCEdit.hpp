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
	Q_PROPERTY(QHash<QString, QString> fileBuffers READ fileBuffers WRITE setFileBuffers NOTIFY fileBuffersChanged)

	public:
		explicit QCEdit(QWidget *parent = nullptr);

		bool loadFile(const QDir &dir);

		const QDir &fileDir() const noexcept{ return m_fileDir; }
		const QCLexer *lexer() const noexcept{ return &m_lexer; }
		const QCParser *parser() const noexcept{ return &m_parser; }

		const QHash<QString, QString> &fileBuffers() const noexcept{ return m_fileBufs; }

		void lineNumberAreaPaintEvent(QPaintEvent *event);
		int lineNumberAreaWidth();

		void setFileBuffers(const QHash<QString, QString> &bufs){
			m_fileBufs = bufs;
			emit fileBuffersChanged();
		}

	signals:
		void fileDirChanged();
		void fileBuffersChanged();

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
		QHash<QString, QString> m_fileBufs;

		void setDefaultFont();
};

#endif // QIDE_QCEDITOR_HPP
