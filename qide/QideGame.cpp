#include "fmt/format.h"

#include <QSettings>
#include <QProcess>

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
	m_proc->reset();

	QSettings settings;
	auto fteqwPath = settings.value("fteqwPath").toString();

	m_proc->setProgram(fteqwPath);
	m_proc->start();
}
