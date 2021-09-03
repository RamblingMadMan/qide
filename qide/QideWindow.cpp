#include "fmt/core.h"

#include "zip.h"

#include <QtMath>
#include <QDebug>
#include <QStringLiteral>
#include <QApplication>
#include <QStandardPaths>
#include <QSettings>
#include <QProxyStyle>
#include <QHeaderView>
#include <QSplitter>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QStackedWidget>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>

#include "QideEditor.hpp"
#include "QideGame.hpp"
#include "QideCompiler.hpp"
#include "QideWindow.hpp"

class QideTabBarStyle : public QProxyStyle{
	public:
		QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size, const QWidget* widget) const{
			QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);

			if(type == QStyle::CT_TabBarTab){
				s.transpose();
			}

			return s;
		}

		void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const{
			if(element == CE_TabBarTabLabel){
				if(const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option)){
					QStyleOptionTab opt(*tab);
					opt.shape = QTabBar::RoundedNorth;
					QProxyStyle::drawControl(element, &opt, painter, widget);
					return;
				}
			}

			QProxyStyle::drawControl(element, option, painter, widget);
		}
};

QideTab::QideTab(QWidget *parent)
	: QWidget(parent)
	, m_icon(new QLabel(this))
	, m_text(new QLabel(this))
	, m_progress(new QProgressBar(this))
{
	auto labelLay = new QHBoxLayout(this);
	labelLay->addWidget(m_icon);
	labelLay->addWidget(m_text);

	auto lay = new QVBoxLayout(this);
	lay->addLayout(labelLay);
	lay->addWidget(m_progress);

	m_progress->hide();

	setLayout(lay);
}

void QideTab::setIcon(const QIcon &icon){
	m_icon->setPixmap(icon.pixmap(32, 32));
}

void QideTab::setText(const QString &str){
	m_text->setText(str);
}

void QideTab::setProgress(qreal p){ m_progress->setValue(qFloor(p * 100)); }
void QideTab::showProgress(){ m_progress->show(); }
void QideTab::hideProgress(){ m_progress->hide(); }

QideWindow::QideWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_editor(new QideEditor(this))
	, m_game(new QideGame(this))
	, m_comp(new QideCompiler(this))
	, m_menuBar(this)
	, m_fileMenu("File", this)
	, m_editMenu("Edit", this)
	, m_codeTab(new QideTab(QIcon::fromTheme("accessories-text-editor"), "Code", this))
	, m_playTab(new QideTab(QIcon::fromTheme("applications-games"), "Play", this))
	, m_openAction(QIcon::fromTheme("document-open"), "Open Project", this)
	, m_saveAction(QIcon::fromTheme("document-save"), "Save", this)
	, m_quitAction(QIcon::fromTheme("application-exit"), "Quit", this)
	, m_undoAction(QIcon::fromTheme("edit-undo"), "Undo", this)
	, m_redoAction(QIcon::fromTheme("edit-redo"), "Redo", this)
	, m_buildAction(/*QIcon::fromTheme(""), */"Build", this)
	, m_launchAction(QIcon::fromTheme("system-run"), "Launch", this)
{
	connect(&m_openAction, &QAction::triggered, this, [this]{
		auto newDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
														QSettings().value("projDir").toString(),
														QFileDialog::ShowDirsOnly
														| QFileDialog::DontResolveSymlinks);
		setProjectDir(newDir);
	});

	connect(&m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
	connect(&m_undoAction, &QAction::triggered, m_editor->qcEdit(), &QPlainTextEdit::undo);
	connect(&m_redoAction, &QAction::triggered, m_editor->qcEdit(), &QPlainTextEdit::redo);
	connect(&m_buildAction, &QAction::triggered, m_comp, &QideCompiler::compile);

	connect(m_editor->qcEdit(), &QPlainTextEdit::undoAvailable, &m_undoAction, &QAction::setEnabled);
	connect(m_editor->qcEdit(), &QPlainTextEdit::redoAvailable, &m_redoAction, &QAction::setEnabled);

	connect(
		m_comp, &QideCompiler::compileStarted,
		this, [this]{
			m_buildAction.setEnabled(false);
			m_launchAction.setEnabled(false);
		}
	);

	connect(
		m_comp, &QideCompiler::compileFinished,
		this, [this](bool success){
			m_buildAction.setEnabled(true);
			m_launchAction.setEnabled(success);
		}
	);

	m_saveAction.setEnabled(false);
	m_openAction.setEnabled(true);
	m_undoAction.setEnabled(false);
	m_redoAction.setEnabled(false);
	m_buildAction.setEnabled(true);
	m_launchAction.setEnabled(false);

	// file menu
	m_fileMenu.addAction(&m_openAction);
	m_fileMenu.addAction(&m_saveAction);
	m_fileMenu.addSeparator();
	m_fileMenu.addAction(&m_quitAction);

	// edit menu
	m_editMenu.addAction(&m_undoAction);
	m_editMenu.addAction(&m_redoAction);

	// menu bar
	m_menuBar.addMenu(&m_fileMenu);
	m_menuBar.addMenu(&m_editMenu);

	// tool bar buttons
	auto docToolbar = new QToolBar(this);
	docToolbar->addAction(&m_openAction);
	docToolbar->addAction(&m_saveAction);
	docToolbar->addSeparator();
	docToolbar->addAction(&m_undoAction);
	docToolbar->addAction(&m_redoAction);

	auto runToolBar = new QToolBar(this);
	runToolBar->addAction(&m_buildAction);
	runToolBar->addAction(&m_launchAction);

	setMenuBar(&m_menuBar);
	addToolBar(Qt::TopToolBarArea, docToolbar);
	addToolBar(Qt::TopToolBarArea, runToolBar);

	auto mainWidget = new QWidget(this);
	auto mainLay = new QHBoxLayout(this);

	auto tabLay = new QVBoxLayout(this);
	m_codeTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_playTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	tabLay->setAlignment(Qt::AlignTop);
	tabLay->addWidget(m_codeTab);
	tabLay->addWidget(m_playTab);

	auto stack = new QStackedWidget(this);
	int editorIdx = stack->addWidget(m_editor);
	stack->addWidget(m_game);

	stack->setCurrentIndex(editorIdx);

	mainLay->addLayout(tabLay);
	mainLay->addWidget(stack);

	mainWidget->setLayout(mainLay);

	setCentralWidget(mainWidget);

	readSettings();
}

QideWindow::QideWindow(QDir projectDir_, QWidget *parent_)
	: QideWindow(parent_)
{
	setProjectDir(projectDir_);
}

void QideWindow::setProjectDir(QDir projectDir_){
	QString projPath = projectDir_.path();

	qDebug() << QString::fromStdString(fmt::format("Opening project '{}'\n", projPath.toStdString()));

	auto dirInfo = QFileInfo(projPath);
	if(!dirInfo.exists() || !dirInfo.isDir()){
		throw std::runtime_error("project directory doesn't exist or is not a directory");
	}

	QSettings settings;

	auto currentFilePath = projPath + "/src/defs.qc";

	settings.setValue("projDir", projPath);
	settings.setValue("currentFile", currentFilePath);

	auto projSubDir = dirInfo.fileName();
	auto buildPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + projSubDir;

	m_comp->setSrcPath(projectDir_.path());
	m_comp->setBuildPath(buildPath);
	m_editor->setRootDir(projectDir_);
}

void QideWindow::closeEvent(QCloseEvent *event){
	(void)event;

	QSettings settings;

	settings.setValue("title", windowTitle());
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.setValue("splitState", m_editor->splitter()->saveState());
	settings.setValue("fileBufs", QVariant::fromValue(m_editor->fileBuffers()));
	settings.setValue("currentFile", m_editor->qcEdit()->fileDir().path());
}

void QideWindow::downloadFTEQW(){
	QSettings settings;
	auto fteqwPath = settings.value("fteqwPath");
	if(!fteqwPath.isValid()){
		settings.setValue("fteqwPath", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/fteqw");
	}

#ifdef _WIN32
	const auto url = QUrl("https://www.fteqw.org/dl/fteqw_win64.zip");
#elif defined(__linux__)
	const auto url = QUrl("https://www.fteqw.org/dl/fteqw-sdl2-linux64.zip");
#else
#error "Unsupported platform"
#endif

	m_playTab->showProgress();

	auto manager = new QNetworkAccessManager(this);

	auto reply = manager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::downloadProgress, [this](qint64 bytesReceived, qint64 bytesTotal){
		double progress = double(bytesReceived) / bytesTotal;

		m_playTab->setProgress(progress);
	});

	connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply *data){
		auto fteqwArchive = data->readAll();

		auto zipSource = zip_source_buffer_create(fteqwArchive.data(), fteqwArchive.size(), 0, nullptr);
		auto zip = zip_open_from_source(zipSource, ZIP_RDONLY, nullptr);

#ifdef _WIN32
		const auto execName = "fteglqw64.exe";
#elif defined(__linux__)
		const auto execName = "fteqw-sdl2";
#else
#error "Unsupported platform"
#endif

		auto execIdx = zip_name_locate(zip, execName, ZIP_FL_NOCASE);

		zip_stat_t execStat;
		zip_stat_index(zip, execIdx, ZIP_FL_NOCASE, &execStat);

		QByteArray execBytes;
		execBytes.resize(execStat.size);

		auto execFile = zip_fopen(zip, execName, ZIP_RDONLY);

		zip_fseek(execFile, 0, 0);
		zip_fread(execFile, execBytes.data(), execStat.size);

		zip_fclose(execFile);

		zip_close(zip);
		zip_source_close(zipSource);

		auto dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
		if(!QDir(dataPath).exists()){
			QDir().mkdir(dataPath);
		}

		auto fteqwPath = dataPath + "/" + QString(execName);

		QFile outFile(fteqwPath);
		outFile.open(QFile::WriteOnly);
		outFile.write(execBytes);

		QSettings().setValue("fteqwPath", fteqwPath);

		m_playTab->hideProgress();
		m_playTab->setEnabled(true);
	});
}

void QideWindow::readSettings(){
	QSettings settings;

	setProjectDir(settings.value("projDir").toString());

	auto buildDir = settings.value("buildDir", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + m_projectDir.dirName());

	auto fteqwPath = settings.value("fteqwPath");
	if(!fteqwPath.isValid() || !QFileInfo(fteqwPath.toString()).exists()){
		m_playTab->setEnabled(false);

		auto downloadDialog = new QMessageBox(
			QMessageBox::Icon::Question,
			"Download FTEQW?",
			"To test your mod you will need FTEQW.\nDo you want to download it?",
			QMessageBox::Yes | QMessageBox::No,
			this
		);

		downloadDialog->setDefaultButton(QMessageBox::Yes);

		int choice = downloadDialog->exec();
		if(choice == QMessageBox::Yes){
			downloadFTEQW();
		}
	}
	else{
	}

	setWindowTitle(settings.value("title", "QIDE").toString());

	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());

	m_editor->splitter()->restoreState(settings.value("splitState").toByteArray());
	m_editor->qcEdit()->setFileBuffers(settings.value("fileBufs").value<QHash<QString, QString>>());
}
