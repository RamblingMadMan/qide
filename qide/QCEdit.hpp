#ifndef QIDE_QCEDIT_HPP
#define QIDE_QCEDIT_HPP

#include <QDir>
#include <QPlainTextEdit>

#include "QCHighlighter.hpp"
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

	public:
		explicit QCEdit(QWidget *parent = nullptr);

		bool loadFile(const QDir &dir);

		QDir fileDir() const noexcept{ return m_fileDir; }

		void lineNumberAreaPaintEvent(QPaintEvent *event);
		int lineNumberAreaWidth();

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private slots:
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &rect, int dy);

	signals:
		void fileChanged();

	private:
		void reparse();

		LineNumberArea m_lineNumArea;
		QDir m_fileDir;
		QCParser m_parser;
		QCHighlighter m_highlighter;

		void setDefaultFont();
};

#endif // QIDE_QCEDITOR_HPP
