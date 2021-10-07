#ifndef QIDE_QCEDIT_HPP
#define QIDE_QCEDIT_HPP

#include <QDir>
#include <QPlainTextEdit>
#include <QDateTime>

class QUndoStack;
class QShortcut;

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

	Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
	Q_PROPERTY(QCLexer lexer READ lexer)
	Q_PROPERTY(QCParser parser READ parser)
	Q_PROPERTY(QHash<QString, QTextDocument*> fileBuffers READ fileBuffers WRITE setFileBuffers NOTIFY fileBuffersChanged)
	Q_PROPERTY(bool hasChanges READ hasChanges NOTIFY hasChangesChanged)

	public:
		explicit QCEdit(QWidget *parent = nullptr);

		bool loadFile(const QString &path);

		bool saveFile();

		const QString &filePath() const noexcept{ return m_filePath; }
		const QCLexer *lexer() const noexcept{ return m_lexer; }
		const QCParser *parser() const noexcept{ return m_parser; }
		QCCompleter *completer() noexcept{ return m_completer; }
		const QHash<QString, QTextDocument*> &fileBuffers() const noexcept{ return m_fileBufs; }
		bool hasChanges() const noexcept{ return m_hasChanges; }
		int lineNumberAreaWidth();

		void setFileBuffers(const QHash<QString, QTextDocument*> &bufs){
			m_fileBufs = bufs;
			emit fileBuffersChanged();
		}

		//QVariant saveState();
		//bool restoreState(const QVariant &state);

	public slots:
		void showCompleter();
		void hideCompleter();
		void reparse();

	signals:
		void filePathChanged();
		void fileBufferChanged(const QString &filePath);
		void fileBuffersChanged();
		void fileSaveTimesChanged();

		void hasChangesChanged();

		void parseStarted();
		void parseFinished(bool success);

	private slots:
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &rect, int dy);

	protected:
		void lineNumberAreaPaintEvent(QPaintEvent *event);
		void resizeEvent(QResizeEvent *event) override;

	private:
		QCLexer *m_lexer;
		QCParser *m_parser;
		QCHighlighter *m_highlighter;
		QCCompleter *m_completer;
		QString m_filePath;
		LineNumberArea m_lineNumArea;
		QHash<QString, QTextDocument*> m_fileBufs;
		bool m_hasChanges = false;

		void setDefaultFont();

		friend class LineNumberArea;
};

#endif // QIDE_QCEDITOR_HPP
