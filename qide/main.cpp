#include <QColor>
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QStyleFactory>

#include "fmt/format.h"

#include "QideWindow.hpp"
#include "QideSetup.hpp"

#include "QuakeColors.hpp"

int main(int argc, char *argv[]){
	qInfo() <<
		"┌─────────────┐\n"
		"│ QIDE v0.0.1 │\n"
		"└─────────────┘\n";

	QApplication::setOrganizationDomain("hamsmith.dev");
	QApplication::setOrganizationName("Hamsmith");
	QApplication::setApplicationName("QIDE");
	QApplication::setApplicationVersion("0.0.0");

	QApplication qapp(argc, argv);

	QSurfaceFormat surfaceFmt;
	surfaceFmt.setVersion(4, 3);
	surfaceFmt.setProfile(QSurfaceFormat::CoreProfile);
#ifndef NDEBUG
	surfaceFmt.setOption(QSurfaceFormat::FormatOption::DebugContext);
#endif
	surfaceFmt.setRedBufferSize(8);
	surfaceFmt.setGreenBufferSize(8);
	surfaceFmt.setBlueBufferSize(8);
	surfaceFmt.setAlphaBufferSize(8);
	surfaceFmt.setDepthBufferSize(24);
	surfaceFmt.setStencilBufferSize(8);
	surfaceFmt.setSwapBehavior(QSurfaceFormat::SwapBehavior::TripleBuffer);
	surfaceFmt.setSwapInterval(1);

	QSurfaceFormat::setDefaultFormat(surfaceFmt);

	QFileInfo iconInfo(":/icon-32.png");
	if(!iconInfo.exists()){
		qDebug() << "Could not find app icon in resources";
	}
	else{
		qapp.setWindowIcon(QIcon(":/icon-32.png"));
	}

	qapp.setStyle(QStyleFactory::create("fusion"));

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, quakeDarkGrey.darker());
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127,127,127));
	darkPalette.setColor(QPalette::Base, QColor(42,42,42));
	darkPalette.setColor(QPalette::AlternateBase, QColor(66,66,66));
	darkPalette.setColor(QPalette::ToolTipBase, quakeDarkGrey.darker());
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127,127,127));
	darkPalette.setColor(QPalette::Dark, QColor(35,35,35));
	darkPalette.setColor(QPalette::Shadow, QColor(20,20,20));
	darkPalette.setColor(QPalette::Button, QColor(53,53,53));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127,127,127));
	darkPalette.setColor(QPalette::BrightText, quakeYello);
	darkPalette.setColor(QPalette::Link, quakeOrange);
	darkPalette.setColor(QPalette::Highlight, QColor(127, 83, 63));
	darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80,80,80));
	darkPalette.setColor(QPalette::HighlightedText, Qt::white);
	darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127,127,127));

	qapp.setPalette(darkPalette);

	// Hack font
	QFontDatabase::addApplicationFont(":ttf/Hack-Regular.ttf");
	QFontDatabase::addApplicationFont(":ttf/Hack-Bold.ttf");
	QFontDatabase::addApplicationFont(":ttf/Hack-Italic.ttf");
	QFontDatabase::addApplicationFont(":ttf/Hack-BoldItalic.ttf");

	// Monoid font
	QFontDatabase::addApplicationFont(":ttf/Monoid-Regular.ttf");
	QFontDatabase::addApplicationFont(":ttf/Monoid-Bold.ttf");
	QFontDatabase::addApplicationFont(":ttf/Monoid-Italic.ttf");
	QFontDatabase::addApplicationFont(":ttf/Monoid-Retina.ttf");

	// Source sans font
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-Black.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-BlackIt.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-Bold.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-BoldIt.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-ExtraLight.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-ExtraLightIt.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-It.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-Light.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-LightIt.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-Regular.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-Semibold.ttf");
	QFontDatabase::addApplicationFont(":ttf/SourceSans3-SemiboldIt.ttf");

	QFont fontHack("Source Sans 3", 11);

	QApplication::setFont(fontHack);

	QideWindow *window = nullptr;

	QSettings settings;

	const auto id1Dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/id1";

	const auto pak0Path = id1Dir + "/pak0.pak";

	if(!QFileInfo::exists(pak0Path) || !QFileInfo(pak0Path).isFile() || !settings.value("fteqwPath").isValid()){
		auto wizard = new QideSetup;
		wizard->show();

		QObject::connect(wizard, &QideSetup::finished, [wizard](int result){
			if(result != QDialog::Accepted){
				qApp->quit();
				return;
			}

			auto window = new QideWindow;
			window->show();
			wizard->setParent(window);
		});
	}
	else{
		auto window = new QideWindow;
		window->show();
	}

	return qapp.exec();
}

