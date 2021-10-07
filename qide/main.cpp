#include <QApplication>
#include <QLibrary>
#include <QColor>
#include <QDebug>
#include <QSettings>
#include <QDirIterator>
#include <QStandardPaths>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QStyleFactory>

#include "physfs.h"

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

	QDir execDir = QFileInfo(argv[0]).absoluteDir();

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

	qapp.setStyleSheet(
		"QToolBar{"
			"border-radius: 0;"
		"}"
		"QPushButton{"
			"border-radius: 0;"
		"}"
		"QToolButton{"
			"border-radius: 0;"
		"}"
		"QComboBox{"
			"border-radius: 0;"
		"}"
		"QFontComboBox::down-arrow{"
			"border-radius: 0;"
		"}"
		"QPlainTextEdit{"
			"border-radius: 0;"
		"}"
		"QLineEdit{"
			"border-radius: 0;"
		"}"
		"QListView{"
			"border-radius: 0;"
		"}"
	);

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

	auto tryAddFont = [](const QString &name){
		if(QFontDatabase::addApplicationFont(QString(":ttf/%1").arg(name)) == -1){
			qDebug() << "Could not load font" << QString("%1").arg(name);
		}
	};

	// Hack font
	tryAddFont("Hack-Regular.ttf");
	tryAddFont("Hack-Bold.ttf");
	tryAddFont("Hack-Italic.ttf");
	tryAddFont("Hack-BoldItalic.ttf");

	// Monoid font
	tryAddFont("Monoid-Regular.ttf");
	tryAddFont("Monoid-Bold.ttf");
	tryAddFont("Monoid-Italic.ttf");
	tryAddFont("Monoid-Retina.ttf");

	// Quake font
	tryAddFont("dpquake.ttf");
	tryAddFont("Quake.ttf");
	tryAddFont("Quake1.ttf");
	tryAddFont("Quake2.ttf");

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

	QSettings config;

	auto curFont = config.value("interface/font");
	auto codeFont = config.value("editor/font");

	if(curFont.isValid()){
		QApplication::setFont(curFont.value<QFont>());
	}
	else{
		QFont fontSans("Source Sans 3", 11);
		config.setValue("interface/font", fontSans);
		QApplication::setFont(fontSans);
	}

	if(!codeFont.isValid()){
		QFont fontHack("Hack", 10);
		fontHack.setStyleHint(QFont::Monospace);
		fontHack.setFixedPitch(true);
		config.setValue("editor/font", fontHack);
	}

	QideWindow *window = nullptr;

	const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	const auto appDataPathStr = appDataPath.toUtf8().toStdString();

	QVector<QString> gameSearchPaths = {
		execDir.path() + "/games",
		appDataPath + "/games"
	};

	QList<QLibrary*> games;

	foreach(const QString &path, gameSearchPaths){
		QDirIterator dirIt{QDir(path)};
		while(dirIt.hasNext()){
			auto entry = dirIt.next();
			auto info = QFileInfo(entry);

			if(!info.isFile()) continue;

			qDebug() << "Game:" << entry;

			games.append(new QLibrary(entry, &qapp));
		}
	}

	PHYSFS_init(argv[0]);
	std::atexit([]{ PHYSFS_deinit(); });

	PHYSFS_setWriteDir(appDataPathStr.c_str());
	PHYSFS_mkdir("/id1");

	const auto id1Dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/id1";

	const auto pak0Path = id1Dir + "/pak0.pak";
	const auto pak0PathStr = pak0Path.toUtf8().toStdString();

	if(!QFileInfo::exists(pak0Path) || !QFileInfo(pak0Path).isFile() || !config.value("fteqwPath").isValid()){
		auto wizard = new QideSetup;
		wizard->show();

		QObject::connect(wizard, &QideSetup::finished, [wizard, pak0PathStr](int result){
			if(result != QDialog::Accepted){
				qApp->quit();
				return;
			}

			PHYSFS_mount(pak0PathStr.c_str(), "/id1/pak0", 1);

			auto window = new QideWindow;
			window->show();
			wizard->setParent(window);
		});
	}
	else{
		PHYSFS_mount(pak0PathStr.c_str(), "/id1/pak0", 1);
		auto window = new QideWindow;
		window->show();
	}

	return qapp.exec();
}

