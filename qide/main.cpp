#include <QApplication>
#include <QDebug>
#include <QResource>

#include "fmt/format.h"

#include "QideWindow.hpp"

int main(int argc, char *argv[]){
	qInfo() <<
		"┌─────────────┐\n"
		"│ QIDE v0.0.1 │\n"
		"└─────────────┘\n";

	QApplication::setOrganizationDomain("hamsmith.dev");
	QApplication::setOrganizationName("Hamsmith");
	QApplication::setApplicationName("QIDE");

	QApplication qapp(argc, argv);

	//QResource::registerResource("icon.qrc");

	QIcon appIcon(":/icon-32.png");
	qapp.setWindowIcon(appIcon);

	QFileInfo iconInfo(":/icon-32.png");
	if(!iconInfo.exists()){
		qDebug() << "Could not find app icon in resources";
	}

	QideWindow window(QDir::currentPath());

	window.setWindowIcon(appIcon);

	window.show();

	return qapp.exec();
}

