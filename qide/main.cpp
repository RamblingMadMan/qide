#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QFontDatabase>

#include "fmt/format.h"

#include "QideWindow.hpp"
#include "QideSetup.hpp"

int main(int argc, char *argv[]){
	qInfo() <<
		"┌─────────────┐\n"
		"│ QIDE v0.0.1 │\n"
		"└─────────────┘\n";

	QApplication::setOrganizationDomain("hamsmith.dev");
	QApplication::setOrganizationName("Hamsmith");
	QApplication::setApplicationName("QIDE");

	QApplication qapp(argc, argv);

	QFileInfo iconInfo(":/icon-32.png");
	if(!iconInfo.exists()){
		qDebug() << "Could not find app icon in resources";
	}
	else{
		QIcon appIcon(":/icon-32.png");
		qapp.setWindowIcon(appIcon);
	}

	QFontDatabase::addApplicationFont(":/Hack-Regular.ttf");
	QFontDatabase::addApplicationFont(":/Hack-Bold.ttf");
	QFontDatabase::addApplicationFont(":/Hack-Italic.ttf");
	QFontDatabase::addApplicationFont(":/Hack-BoldItalic.ttf");

	QideWindow *window = nullptr;

	QSettings settings;

	const auto id1Dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/id1";

	const auto pak0Path = id1Dir + "/pak0.pak";

	if(!QFileInfo(pak0Path).exists() || !QFileInfo(pak0Path).isFile() || !settings.value("fteqwPath").isValid()){
		auto wizard = new QideSetup;
		wizard->show();

		QObject::connect(wizard, &QideSetup::finished, [&]{
			window = new QideWindow(QDir::currentPath());
			window->show();
			wizard->setParent(window);
		});
	}
	else{
		window = new QideWindow(QDir::currentPath());
		window->show();
	}

	return qapp.exec();
}

