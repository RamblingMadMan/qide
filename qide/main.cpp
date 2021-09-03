#include <QApplication>
#include <QDebug>

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

	QideWindow window(QDir::currentPath());

	window.show();

	return qapp.exec();
}

