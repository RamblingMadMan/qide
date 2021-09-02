#include <QtWidgets>

#include "fmt/format.h"

#include "QCEdit.hpp"
#include "QideWindow.hpp"

void downloadFteqw(){
	const auto url = QUrl("https://www.fteqw.org/dl/fteqw-sdl2-linux64.zip");
}

int main(int argc, char *argv[]){
	fmt::print(
		"┌─────────────┐\n"
		"│ QIDE v0.0.1 │\n"
		"└─────────────┘\n"
	);

	QApplication::setOrganizationDomain("hamsmith.dev");
	QApplication::setOrganizationName("Hamsmith");
	QApplication::setApplicationName("QIDE");

	QApplication qapp(argc, argv);

	QideWindow window(QDir::currentPath());

	window.show();

	return qapp.exec();
}

