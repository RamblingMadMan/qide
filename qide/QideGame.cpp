#include "fmt/format.h"

#include <QDebug>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QSplitter>

#include "QideGame.hpp"

QideGame::QideGame(QWidget *parent)
	: QWidget(parent)
	, m_proc(new QProcess(this))
{
	auto gameWidget = new QWidget;

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	connect(m_proc, &QProcess::started, [this, gameWidget]{
		auto left = 0;
		auto top = 0;
		auto width = this->width();
		auto height = this->height();

		auto vidCmd = fmt::format("vid_recenter {} {} {} {} {}", left, top, width, height, gameWidget->winId());

		m_proc->write(vidCmd.c_str(), vidCmd.size());

		emit launched();
	});

	auto stdoutWidget = new QTextEdit;

	QFont hackFont("Hack", 10);
	hackFont.setFixedPitch(true);
	hackFont.setStyleHint(QFont::Monospace);

	stdoutWidget->setFont(hackFont);

	connect(m_proc, &QProcess::readyReadStandardOutput, [this, stdoutWidget]{
		stdoutWidget->append(m_proc->readAllStandardOutput());
	});

	connect(m_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onExited()));

	auto split = new QSplitter;
	split->setOrientation(Qt::Vertical);
	split->addWidget(gameWidget);
	split->addWidget(stdoutWidget);

	auto lay = new QHBoxLayout;
	lay->addWidget(split);

	setLayout(lay);

	//split->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void QideGame::onExited(int status, QProcess::ExitStatus exitStatus){
	(void)exitStatus;
	emit exited(status);
}

QString QideGame::workDir(){
	return m_proc->workingDirectory();
}

void QideGame::setWorkDir(const QString &dir){
	m_proc->setWorkingDirectory(dir);
	emit workDirChanged();
}

void QideGame::launch(){
	m_proc->terminate();

	QSettings settings;
	auto projDir = settings.value("projDir").toString();
	auto projName = QDir(projDir).dirName();
	auto fteqwPath = settings.value("fteqwPath").toString();

	qDebug() << "Launching" << fteqwPath;

	m_proc->setProgram(fteqwPath);

	QStringList argList;
	argList.push_back("-window");
	argList.push_back("-game");
	argList.push_back(projName);

	m_proc->setArguments(argList);
	m_proc->start();
}

bool QideGame::isRunning() const{ return m_proc->state() == QProcess::Running; }
