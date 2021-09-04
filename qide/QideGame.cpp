#include "fmt/format.h"

#include <QDebug>
#include <QSettings>
#include <QProcess>
#include <QDir>

#include "QideGame.hpp"

QideGame::QideGame(QWidget *parent)
	: QWidget(parent)
	, m_proc(new QProcess(this))
{
	connect(m_proc, &QProcess::started, [this]{
		auto left = 0;
		auto top = 0;
		auto width = this->width();
		auto height = this->height();

		auto vidCmd = fmt::format("vid_recenter {} {} {} {} {}", left, top, width, height, winId());

		m_proc->write(vidCmd.c_str(), vidCmd.size());

		emit launched();
	});

	connect(m_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onExited));
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
