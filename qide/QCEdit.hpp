#ifndef QIDE_QCEDIT_HPP
#define QIDE_QCEDIT_HPP

#include <QDir>
#include <QPlainTextEdit>

class QUndoStack;
class QCHighlighter;
class QCCompleter;
class QCLexer;
class QCParser;

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

class QCFileBuffer: public QTextDocument{
	Q_OBJECT

	public:
		explicit QCFileBuffer(QObject *parent = nullptr);
		explicit QCFileBuffer(const QString &contents_, QObject *parent = nullptr);

		QVariant saveState();
		bool restoreState(const QVariant &state);
};

class QCEdit: public QPlainTextEdit{
	Q_OBJECT

	Q_PROPERTY(QDir fileDir READ fileDir NOTIFY fileDirChanged)
	Q_PROPERTY(QCLexer lexer READ lexer)
	Q_PROPERTY(QCParser parser READ parser)
	Q_PROPERTY(QHash<QString, QTextDocument*> fileBuffers READ fileBuffers WRITE setFileBuffers NOTIFY fileBuffersChanged)

	public:
		explicit QCEdit(QWidget *parent = nullptr);

		bool loadFile(const QDir &dir);

		const QDir &fileDir() const noexcept{ return m_fileDir; }
		const QCLexer *lexer() const noexcept{ return m_lexer; }
		const QCParser *parser() const noexcept{ return m_parser; }

		const QHash<QString, QTextDocument*> &fileBuffers() const noexcept{ return m_fileBufs; }

		void lineNumberAreaPaintEvent(QPaintEvent *event);
		int lineNumberAreaWidth();

		void setFileBuffers(const QHash<QString, QTextDocument*> &bufs){
			m_fileBufs = bufs;
			emit fileBuffersChanged();
		}

		//QVariant saveState();
		//bool restoreState(const QVariant &state);

	signals:
		void fileDirChanged();
		void fileBufferChanged(const QString &filePath);
		void fileBuffersChanged();

		void undoStackChanged();

		void parseStarted();
		void parseFinished(bool success);

	private slots:
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &rect, int dy);

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private:
		void reparse();

		QCLexer *m_lexer;
		QCParser *m_parser;
		QCHighlighter *m_highlighter;
		QCCompleter *m_completer;
		QDir m_fileDir;
		LineNumberArea m_lineNumArea;
		QHash<QString, QTextDocument*> m_fileBufs;

		void setDefaultFont();
};

#endif // QIDE_QCEDITOR_HPP
