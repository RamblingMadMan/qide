#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QWizard>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

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

	QideWindow window(QDir::currentPath());

	QSettings settings;

	if(!settings.value("quakeDir").isValid() || !settings.value("fteqwDir").isValid()){
		auto wizard = new QideSetup(&window);
		wizard->show();
	}

	window.show();

	return qapp.exec();
}

