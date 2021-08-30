#include "fmt/format.h"

#include <QPainter>

#include "QCEdit.hpp"

LineNumberArea::LineNumberArea(QCEdit *plainEdit_)
	: QWidget(plainEdit_)
	, m_plainEdit(plainEdit_){}

QSize LineNumberArea::sizeHint() const {
	return QSize(m_plainEdit->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
	m_plainEdit->lineNumberAreaPaintEvent(event);
}

QCEdit::QCEdit(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_lineNumArea(this)
	, m_parser()
	, m_highlighter(document(), &m_parser)
{
	auto newPalette = palette();
	newPalette.setColor(QPalette::Window, Qt::darkGray);
	newPalette.setColor(QPalette::Text, Qt::lightGray);

	setPalette(newPalette);

	setDefaultFont();

	connect(this, &QCEdit::textChanged, this, &QCEdit::reparse);

	connect(this, &QCEdit::blockCountChanged, this, &QCEdit::updateLineNumberAreaWidth);
	connect(this, &QCEdit::updateRequest, this, &QCEdit::updateLineNumberArea);
	connect(this, &QCEdit::cursorPositionChanged, this, &QCEdit::highlightCurrentLine);

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

int QCEdit::lineNumberAreaWidth(){
	int digits = 1;
	int max = qMax(1, blockCount());
	while(max >= 10){
		max /= 10;
		++digits;
	}

	int space = 6 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

	return space;
}

void QCEdit::updateLineNumberAreaWidth(int /* newBlockCount */){
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void QCEdit::updateLineNumberArea(const QRect &rect, int dy){
	if(dy)
		m_lineNumArea.scroll(0, dy);
	else
		m_lineNumArea.update(0, rect.y(), m_lineNumArea.width(), rect.height());

	if(rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void QCEdit::resizeEvent(QResizeEvent *e){
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumArea.setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void QCEdit::highlightCurrentLine(){
	QList<QTextEdit::ExtraSelection> extraSelections;

	if(!isReadOnly()){
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::darkGray).lighter(20);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void QCEdit::lineNumberAreaPaintEvent(QPaintEvent *event){
	QPainter painter(&m_lineNumArea);
	painter.fillRect(event->rect(), QColor(Qt::darkGray).lighter(40));

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + qRound(blockBoundingRect(block).height());

	while(block.isValid() && top <= event->rect().bottom()){
		if(block.isVisible() && bottom >= event->rect().top()){
			QString number = QString::number(blockNumber + 1);
			painter.setPen(QColor(Qt::white).darker(175));
			painter.drawText(
				-3, top, m_lineNumArea.width(), fontMetrics().height(),
				Qt::AlignRight, number
			);
		}

		block = block.next();
		top = bottom;
		bottom = top + qRound(blockBoundingRect(block).height());
		++blockNumber;
	}
}

bool QCEdit::loadFile(const QDir &dir){
	QFile file(dir.path());
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		return false;
	}

	setPlainText(file.readAll());
	m_fileDir = dir;

	document()->setMetaInformation(QTextDocument::DocumentTitle, dir.path());

	emit fileChanged();

	return true;
}

void QCEdit::reparse(){
	m_parser.reset();
	m_parser.setTitle(m_fileDir.path());

	const auto plainStr = toPlainText();

	QCLexer lexer;
	lexer.lex(plainStr);

	m_parser.parse(lexer.tokens().begin(), lexer.tokens().end());
}

void QCEdit::setDefaultFont(){
	QFont fontHack;
	fontHack.setFamily("Hack");
	fontHack.setStyleHint(QFont::Monospace);
	fontHack.setFixedPitch(true);
	fontHack.setPointSize(10);

	QFontMetricsF fntMetrics(fontHack);
	auto stopWidth = 4 * fntMetrics.width(' ');

	setTabStopDistance(stopWidth);
	setFont(fontHack);
}
