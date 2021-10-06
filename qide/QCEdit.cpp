#include "fmt/format.h"

#include <QDebug>
#include <QSettings>
#include <QPainter>
#include <QPalette>
#include <QShortcut>
#include <QDateTime>
#include <QTemporaryFile>

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
	, m_filePath()
	, m_lineNumArea(this)
{
	setDefaultFont();

	connect(this, &QCEdit::blockCountChanged, this, &QCEdit::updateLineNumberAreaWidth);
	connect(this, &QCEdit::updateRequest, this, &QCEdit::updateLineNumberArea);
	connect(this, &QCEdit::cursorPositionChanged, this, &QCEdit::highlightCurrentLine);
	connect(this, &QPlainTextEdit::textChanged, this, [this]{ reparse(); m_hasChanges = true; }); // TODO: check if undos available instead of text changes
	connect(m_parser, &QCParser::resultsChanged, this, [this]{
		QStringList completerChoices;
		completerChoices.append(qcKeywords);
		completerChoices.append(qcBasicTypes);
		completerChoices.append(m_parser->globalVarNames());
		m_completer->setChoices(completerChoices);
	});

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();

	QStringList completerChoices;
	completerChoices.append(qcKeywords);
	completerChoices.append(qcBasicTypes);
	m_completer->setChoices(completerChoices);

	m_completer->setQcEdit(this);
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

void QCEdit::showCompleter(){
	m_completer->completeAtCursor(true);
}

void QCEdit::hideCompleter(){
	m_completer->hide();
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
		lineColor.setAlpha(128);

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

bool QCEdit::loadFile(const QString &path){
	auto fileInfo = QFileInfo(path);
	auto filePath = fileInfo.absoluteFilePath();

	if(filePath == m_filePath){
		return true;
	}

	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		return false;
	}

	auto fileContents = file.readAll();

	QTextDocument *fileBuf = nullptr;

	auto fileBufIt = m_fileBufs.find(filePath);
	if(fileBufIt != m_fileBufs.end()){
		qDebug() << "Loaded buffered file" << fileBufIt.key();
		fileBuf = fileBufIt.value();

		if(fileBuf->toPlainText() != fileContents){
			m_hasChanges = true;
			emit hasChangesChanged();
		}
	}
	else{
		qDebug() << "Loaded new file" << filePath;

		fileBuf = new QTextDocument(fileContents);
		fileBuf->setDefaultFont(font());
		fileBuf->setDocumentLayout(new QPlainTextDocumentLayout(fileBuf));
		fileBuf->setMetaInformation(QTextDocument::DocumentTitle, filePath);

		m_fileBufs.insert(filePath, fileBuf);

		if(m_hasChanges){
			m_hasChanges = false;
			emit hasChangesChanged();
		}
	}

	m_filePath = fileInfo.absoluteFilePath();

	emit filePathChanged();

	m_highlighter->setDocument(fileBuf);
	m_highlighter->rehighlight();

	setDocument(fileBuf);

	setDefaultFont();

	updateLineNumberAreaWidth(0);

	m_parser->setTitle(filePath);
	reparse();

	return true;
}

bool QCEdit::saveFile(){
	//if(!hasChanges()) return true;

	qDebug() << "Saving" << m_filePath;

	QTemporaryFile oldFileTmp("qide-oldXXXXXX");

	if(QFileInfo::exists(m_filePath)){
		if(!oldFileTmp.open()){
			qDebug() << "Could not open temporary file";
			return false;
		}

		QFile oldFile(m_filePath);
		if(!oldFile.open(QFile::ReadOnly)){
			qDebug() << "Error opening" << m_filePath << "for read";
			return false;
		}

		oldFileTmp.write(oldFile.readAll());

		if(!QFile::remove(m_filePath)){
			qDebug() << "Could not remove" << m_filePath;
			return false;
		}
	}

	auto src = toPlainText().toUtf8();

	QFile outFile(m_filePath);
	if(!outFile.open(QFile::WriteOnly)){
		qDebug() << "Error opening" << m_filePath << "for write";

		if(oldFileTmp.isOpen()){
			oldFileTmp.copy(m_filePath);
		}

		return false;
	}

	if(outFile.write(src) != src.size()){
		qDebug() << "Failed to write" << m_filePath;

		if(oldFileTmp.isOpen()){
			oldFileTmp.copy(m_filePath);
		}

		return false;
	}

	m_hasChanges = false;
	emit hasChangesChanged();

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

	static QString plainStr;

	plainStr = toPlainText();

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
	QSettings config;

	auto fnt = config.value("editor/font").value<QFont>();

	QFontMetricsF fntMetrics(fnt);
	auto stopWidth = 4 * fntMetrics.width(' ');

	setTabStopDistance(stopWidth);
	setFont(fnt);
}
