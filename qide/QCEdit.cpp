#include "fmt/format.h"

#include <QDebug>
#include <QPainter>
#include <QUndoStack>
#include <QPlainTextDocumentLayout>

#include "QCLexer.hpp"
#include "QCParser.hpp"
#include "QCHighlighter.hpp"
#include "QCCompleter.hpp"
#include "QCEdit.hpp"

LineNumberArea::LineNumberArea(QCEdit *plainEdit_)
	: QWidget(plainEdit_)
	, m_plainEdit(plainEdit_){}

QSize LineNumberArea::sizeHint() const{
	return QSize(m_plainEdit->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event){
	m_plainEdit->lineNumberAreaPaintEvent(event);
}

QCFileBuffer::QCFileBuffer(QObject *parent)
	: QTextDocument(parent)
{
	setDocumentLayout(new QPlainTextDocumentLayout(this));
}

QCFileBuffer::QCFileBuffer(const QString &contents_, QObject *parent)
	: QCFileBuffer(parent)
{
	setPlainText(contents_);
}

QCEdit::QCEdit(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_lexer(new QCLexer(this))
	, m_parser(new QCParser(this))
	, m_highlighter(new QCHighlighter(this))
	, m_completer(new QCCompleter(nullptr, this))
	, m_fileDir()
	, m_lineNumArea(this)
{
	auto newPalette = palette();
	newPalette.setColor(QPalette::Window, Qt::darkGray);
	newPalette.setColor(QPalette::Text, Qt::lightGray);

	setPalette(newPalette);

	setDefaultFont();

	connect(this, &QCEdit::blockCountChanged, this, &QCEdit::updateLineNumberAreaWidth);
	connect(this, &QCEdit::updateRequest, this, &QCEdit::updateLineNumberArea);
	connect(this, &QCEdit::cursorPositionChanged, this, &QCEdit::highlightCurrentLine);

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();

	QStringList completerChoices;
	completerChoices << qcKeywords;
	completerChoices << qcBasicTypes;

	m_completer->setQcEdit(this);
	m_completer->setChoices(completerChoices);
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
	if(dy){
		m_lineNumArea.scroll(0, dy);
	}
	else{
		m_lineNumArea.update(0, rect.y(), m_lineNumArea.width(), rect.height());
	}

	if(rect.contains(viewport()->rect())){
		updateLineNumberAreaWidth(0);
	}
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
	int blockNum = block.blockNumber();
	int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + qRound(blockBoundingRect(block).height());

	while(block.isValid() && top <= event->rect().bottom()){
		if(block.isVisible() && bottom >= event->rect().top()){
			QString numStr = QString::number(blockNum + 1);
			painter.setPen(QColor(Qt::white).darker(175));
			painter.drawText(
				-3, top, m_lineNumArea.width(), fontMetrics().height(),
				Qt::AlignRight, numStr
			);
		}

		block = block.next();
		top = bottom;
		bottom = top + qRound(blockBoundingRect(block).height());
		++blockNum;
	}
}

bool QCEdit::loadFile(const QDir &dir){
	auto filePath = dir.absolutePath();

	QTextDocument *fileBuf = nullptr;

	auto fileBufIt = m_fileBufs.find(filePath);
	if(fileBufIt != m_fileBufs.end()){
		qDebug() << "Loaded buffered file" << fileBufIt.key();
		fileBuf = fileBufIt.value();
	}
	else{
		qDebug() << "Loaded new file" << filePath;
		QFile file(filePath);
		if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
			return false;
		}

		auto fileContents = file.readAll();

		fileBuf = new QTextDocument(fileContents);
		fileBuf->setDefaultFont(font());
		fileBuf->setDocumentLayout(new QPlainTextDocumentLayout(fileBuf));
		fileBuf->setMetaInformation(QTextDocument::DocumentTitle, filePath);

		m_fileBufs.insert(filePath, fileBuf);
	}

	m_fileDir = dir;
	m_fileDir.makeAbsolute();

	emit fileDirChanged();

	m_highlighter->setDocument(fileBuf);
	m_highlighter->rehighlight();

	setDocument(fileBuf);
	updateLineNumberAreaWidth(0);

	m_parser->setTitle(filePath);
	reparse();

	return true;
}

QVariant saveState(){
	QVector<QVariant> vars;

	return QVariant::fromValue(vars);
}

bool restoreState(const QVariant &state){
	if(!state.isValid()) return false;

	auto vars = state.value<QVector<QVariant>>();

	return true;
}

void QCEdit::reparse(){
	emit parseStarted();

	auto plainStr = toPlainText();

	m_lexer->reset();
	auto numToks = m_lexer->lex(plainStr);

	if(numToks < 0){
		emit parseFinished(false);
		return; // lexing error
	}

	m_parser->reset();
	m_parser->parse(m_lexer->tokens().begin(), m_lexer->tokens().end());

	emit parseFinished(true);
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
