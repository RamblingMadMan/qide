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
	});
}

void QideGame::launch(){
	m_proc->reset();

	QSettings settings;
	auto fteqwPath = settings.value("fteqwPath").toString();

	m_proc->setProgram(fteqwPath);
	m_proc->start();
}
