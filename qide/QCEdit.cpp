#include "fmt/format.h"

#include "QCEdit.hpp"

QCEdit::QCEdit(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_parser()
	, m_highlighter(document(), &m_parser)
{
	setDefaultFont();

	QObject::connect(
		this, &QCEdit::textChanged,
		this, &QCEdit::reparse
	);
}

QCEdit::QCEdit(const QString &text, QWidget *parent)
	: QPlainTextEdit(text, parent)
	, m_parser()
	, m_highlighter(document(), &m_parser)
{
	setDefaultFont();

	QObject::connect(
		this, &QCEdit::textChanged,
		this, &QCEdit::reparse
	);
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
	fmt::print("Reparsed '{}'\n", document()->metaInformation(QTextDocument::DocumentTitle).toStdString());

	m_parser.reset();
	QCLexer lexer(toPlainText());
	QVector<QCToken> toks;
	int reserved = 16;
	toks.reserve(reserved);

	while(lexer.hasTokens()){
		toks.push_back(lexer.lex());
		if(toks.size() == reserved){
			reserved *= 2;
			toks.reserve(reserved);
		}
	}

	m_parser.parse(toks.begin(), toks.end());
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
