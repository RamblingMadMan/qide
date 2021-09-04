#include <QtMath>

#include <QSettings>
#include <QDebug>
#include <QLabel>
#include <QDir>
#include <QLineEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>

#include "QideSetup.hpp"

QideSetupIntro::QideSetupIntro(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle("Welcome to QIDE!");
	setSubTitle("QIDE - The Quake Mod IDE");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLabel *iconLabel = new QLabel;
	iconLabel->setPixmap(QIcon(":/icon-256.png").pixmap(192, 192));

	QLabel *label = new QLabel("This wizard will set up QIDE");
	label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	auto skipIntroCheck = new QCheckBox("Skip this page");
	skipIntroCheck->setChecked(false);

	connect(skipIntroCheck, &QCheckBox::toggled, [](bool skip){
		QSettings().setValue("skipSetupIntro", skip);
	});

	QVBoxLayout *lay = new QVBoxLayout;

	lay->addWidget(iconLabel);
	lay->addWidget(label);
	lay->addWidget(skipIntroCheck);

	lay->setAlignment(iconLabel, Qt::AlignTop | Qt::AlignHCenter);
	lay->setAlignment(label, Qt::AlignHCenter);
	lay->setAlignment(skipIntroCheck, Qt::AlignBottom | Qt::AlignRight);

	setLayout(lay);
}

QideSetupQuake::QideSetupQuake(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle("Quake Installation Directory");
	setSubTitle("A retail or shareware copy of Quake is needed for running mods.");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLabel *label = new QLabel("If the directory auto-filled is not correct, enter the correct one.");
	label->setWordWrap(true);

	QVector<QString> searchDirs;
#ifdef __linux__
	searchDirs.push_back(QDir::homePath() + "/.steam/steam/steamapps/common/Quake");
#elif defined(_WIN32)
	qDebug() << "Windows is untested";
	searchDirs.push_back("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Quake");
#else
#error "Unsupported platform"
#endif

	QString quakePath;

	for(const auto &dir : searchDirs){
		qDebug() << "Searching for Quake in" << dir;
		if(QDir(dir + "/id1re").exists() || QDir(dir + "/Id1").exists() || QDir(dir + "/id1").exists()){
			quakePath = dir;
			break;
		}
	}

	auto pathEntry = new QLineEdit;
	pathEntry->setReadOnly(true);

	connect(pathEntry, &QLineEdit::textChanged, [this, pathEntry]{
		auto path = pathEntry->text();
		auto pathExists = QDir(path).exists();

		auto isValid = pathExists;

		if(pathExists){
			qDebug() << "Searching for Quake in" << path;
			if(!QDir(path + "/id1re").exists() && !QDir(path + "/Id1").exists() && !QDir(path + "/id1").exists()){
				isValid = false;
			}
		}

		auto oldIsValid = m_dirIsValid;
		m_dirIsValid = isValid;
		if(oldIsValid != m_dirIsValid){
			emit completeChanged();
		}
	});

	auto browseButton = new QPushButton(QIcon::fromTheme("document-open"), "Browse");
	QObject::connect(browseButton, &QPushButton::pressed, [pathEntry]{
		auto result = QFileDialog::getExistingDirectory(nullptr, "Select the Quake Installation Folder", QDir::currentPath());
		if(!result.isEmpty()){
			pathEntry->setText(result);
		}
	});

	auto pathLay = new QHBoxLayout;
	pathLay->addWidget(pathEntry);
	pathLay->addWidget(browseButton);

	auto lay = new QVBoxLayout;
	lay->addWidget(label);
	lay->addLayout(pathLay);

	connect(this, &QWizardPage::completeChanged, [this, pathEntry]{
		if(!isComplete()) return;
		QSettings().setValue("quakeDir", pathEntry->text());
	});

	registerField("quakeDir", pathEntry);

	if(!quakePath.isEmpty()){
		pathEntry->setText(quakePath);
	}

	setLayout(lay);
}

bool QideSetupQuake::isComplete() const{ return m_dirIsValid; }

QideSetupFTEQW::QideSetupFTEQW(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle("FTEQW Download");
	setSubTitle("FTEQW is required for launching mods in the editor.");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	auto label = new QLabel("Click download to get the latest version of FTEQW from <a href=\"https://fte.triptohell.info/\">https://fte.triptohell.info/</a>, "
							"Otherwise click Browse to locate the FTEQW executable.");

	label->setWordWrap(true);

	auto dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if(!QDir(dataPath).exists()){
		QDir().mkdir(dataPath);
	}

#ifdef _WIN32
	static const auto execName = "fteqw64.exe";
#elif defined(__linux__)
	static const auto execName = "fteqw64";
#else
#error "Unsupported platform"
#endif

	QVector<QString> searchDirs;
	searchDirs.reserve(3);

	searchDirs.push_back(dataPath + "/" + QString(execName));

#ifdef __linux__
	searchDirs.push_back(field("quakeDir").toString() + "/fteqw64");
	searchDirs.push_back(QDir::currentPath() + "/fteqw64");
#elif defined(_WIN32)
	searchDirs.push_back(field("quakeDir").toString() + "/fteqw64.exe");
	searchDirs.push_back(QDir::currentPath() + "/fteqw64.exe");
#else
#error "Unsupported platform"
#endif

	QString fteqwPath;

	for(const auto &dir : searchDirs){
		qDebug() << "Searching for FTEQW at" << dir;
		if(QFileInfo(dir).exists() && QFileInfo(dir).isFile()){
			fteqwPath = dir;
			break;
		}
	}

	auto pathEntry = new QLineEdit;
	pathEntry->setReadOnly(true);

	connect(pathEntry, &QLineEdit::textChanged, [this, pathEntry]{
		auto pathInfo = QFileInfo(pathEntry->text());
		auto isValid = pathInfo.exists() && pathInfo.isFile() && (pathInfo.fileName() == "fteqw64");
		auto oldIsValid = m_pathIsValid;
		m_pathIsValid = isValid;
		if(oldIsValid != m_pathIsValid){
			emit completeChanged();
		}
	});

	auto browseBtn = new QPushButton(QIcon::fromTheme("document-open"), "Browse");
	browseBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QObject::connect(browseBtn, &QPushButton::pressed, [pathEntry]{
		auto result = QFileDialog::getOpenFileName(nullptr, "Select the FTEQW Executable", QDir::currentPath());
		if(!result.isEmpty()){
			pathEntry->setText(result);
		}
	});

	auto dlBtn = new QPushButton(QIcon::fromTheme("emblem-downloads"), "Download");
	dlBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto dlProgress = new QProgressBar;
	dlProgress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(dlBtn, &QPushButton::pressed, [this, dlProgress, pathEntry, browseBtn]{
		dlProgress->setEnabled(true);
		browseBtn->setEnabled(false);

		#ifdef _WIN32
			const auto url = QUrl("https://fte.triptohell.info/moodles/win64/fteqw64.exe");
		#elif defined(__linux__)
			const auto url = QUrl("https://fte.triptohell.info/moodles/linux_amd64/fteqw64");
		#else
		#error "Unsupported platform"
		#endif

		auto manager = new QNetworkAccessManager(this);

		auto reply = manager->get(QNetworkRequest(url));

		connect(reply, &QNetworkReply::downloadProgress, [dlProgress](qint64 bytesReceived, qint64 bytesTotal){
			qreal progress = qreal(bytesReceived) / bytesTotal;
			dlProgress->setValue(qFloor(progress * 100.0));
		});

		connect(manager, &QNetworkAccessManager::finished, [dlProgress, pathEntry, browseBtn](QNetworkReply *data){
			auto fteqwBinary = data->readAll();

			auto dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
			if(!QDir(dataPath).exists()){
				QDir().mkdir(dataPath);
			}

			auto fteqwPath = dataPath + "/" + QString(execName);

			QFile outFile(fteqwPath);
			outFile.open(QFile::WriteOnly);
			outFile.write(fteqwBinary);

			dlProgress->setValue(100);
			dlProgress->setEnabled(false);

			browseBtn->setEnabled(true);

			pathEntry->setText(fteqwPath);
		});
	});

	auto browseLay = new QHBoxLayout;
	browseLay->addWidget(pathEntry);
	browseLay->addWidget(browseBtn);

	auto dlLay = new QHBoxLayout;
	dlLay->addWidget(dlProgress);
	dlLay->addWidget(dlBtn);

	auto lay = new QVBoxLayout;
	lay->addWidget(label);
	lay->addLayout(browseLay);
	lay->addLayout(dlLay);

	lay->setAlignment(browseLay, Qt::AlignBottom | Qt::AlignRight);
	lay->setAlignment(dlLay, Qt::AlignBottom | Qt::AlignRight);

	connect(this, &QWizardPage::completeChanged, [this, pathEntry]{
		if(!m_pathIsValid) return;
		QSettings().setValue("fteqwPath", pathEntry->text());
	});

	if(!fteqwPath.isEmpty()){
		pathEntry->setText(fteqwPath);
	}

	setLayout(lay);
}

bool QideSetupFTEQW::isComplete() const{ return m_pathIsValid; }

QideSetup::QideSetup(QWidget *parent)
	: QWizard(parent)
{
	auto skipIntroVar = QSettings().value("skipSetupIntro");
	const bool skipIntro = skipIntroVar.isValid() && skipIntroVar.toBool();

	if(!skipIntro){
		addPage(new QideSetupIntro);
	}

	addPage(new QideSetupQuake);
	addPage(new QideSetupFTEQW);
}
