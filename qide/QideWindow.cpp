#include <vector>

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
#include <QUndoStack>

#include "QideEditor.hpp"
#include "QideGame.hpp"
#include "QideCompiler.hpp"
#include "QideWindow.hpp"

QideTabsWidget::QideTabsWidget(QWidget *parent)
	: QWidget(parent)
	, m_codeTab(new QideTab(QIcon::fromTheme("accessories-text-editor"), "Code"))
	, m_playTab(new QideTab(QIcon::fromTheme("applications-games"), "Play"))
	, m_selected(m_codeTab)
{
	auto lay = new QVBoxLayout(this);

	lay->addWidget(m_codeTab);
	lay->addWidget(m_playTab);

	setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
}

QideTab::QideTab(QWidget *parent)
	: QWidget(parent)
	, m_icon(new QLabel)
	, m_text(new QLabel)
	, m_progress(new QProgressBar)
{
	auto labelLay = new QHBoxLayout;
	labelLay->addWidget(m_icon);
	labelLay->addWidget(m_text);

	auto lay = new QVBoxLayout(this);
	lay->addLayout(labelLay);
	lay->addWidget(m_progress);

	m_progress->hide();
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

QideWindow::QideWindow(Ctor, QWidget *parent)
	: QMainWindow(parent)
	, m_tabs(new QideTabsWidget(this))
	, m_editor(new QideEditor(this))
	, m_game(new QideGame(this))
	, m_comp(new QideCompiler(this))
	, m_menuBar(this)
	, m_fileMenu("File", this)
	, m_editMenu("Edit", this)
{
	auto openAction = new QAction(QIcon::fromTheme("document-open"), "Open Project", this);
	auto saveAction = new QAction(QIcon::fromTheme("document-save"), "Save", this);
	auto quitAction = new QAction(QIcon::fromTheme("application-exit"), "Quit", this);
	m_undoAction = new QAction(QIcon::fromTheme("edit-undo"), "Undo", this);
	m_redoAction = new QAction(QIcon::fromTheme("edit-redo"), "Redo", this);
	auto buildAction = new QAction("Build", this);
	auto launchAction = new QAction(QIcon::fromTheme("system-run"), "Launch", this);

	connect(openAction, &QAction::triggered, this, [this]{
		auto newDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
														QSettings().value("projDir").toString(),
														QFileDialog::ShowDirsOnly
														| QFileDialog::DontResolveSymlinks);
		setProjectDir(newDir);
	});

	connect(saveAction, &QAction::triggered, this, [this]{
		writeSettings();
		m_editor->saveCurrent();
	});

	connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
	connect(buildAction, &QAction::triggered, m_comp, &QideCompiler::compile);

	//connect(m_editor->qcEdit(), &QPlainTextEdit::undoAvailable, &m_undoAction, &QAction::setEnabled);
	//connect(m_editor->qcEdit(), &QPlainTextEdit::redoAvailable, &m_redoAction, &QAction::setEnabled);

	connect(
		m_comp, &QideCompiler::compileStarted,
		this, [=]{
			buildAction->setEnabled(false);
			launchAction->setEnabled(false);
		}
	);

	connect(
		m_comp, &QideCompiler::compileFinished,
		this, [=](bool success){
			buildAction->setEnabled(true);
			launchAction->setEnabled(success);
		}
	);

	saveAction->setEnabled(false);
	openAction->setEnabled(true);
	buildAction->setEnabled(true);
	launchAction->setEnabled(false);

	m_undoAction->setEnabled(false);
	m_redoAction->setEnabled(false);

	// file menu
	m_fileMenu.addAction(openAction);
	m_fileMenu.addAction(saveAction);
	m_fileMenu.addSeparator();
	m_fileMenu.addAction(quitAction);

	// edit menu
	m_editMenu.addAction(m_undoAction);
	m_editMenu.addAction(m_redoAction);

	// menu bar
	m_menuBar.addMenu(&m_fileMenu);
	m_menuBar.addMenu(&m_editMenu);

	// tool bar buttons
	auto docToolbar = new QToolBar(this);
	docToolbar->addAction(openAction);
	docToolbar->addAction(saveAction);
	docToolbar->addSeparator();
	docToolbar->addAction(m_undoAction);
	docToolbar->addAction(m_redoAction);

	auto runToolBar = new QToolBar(this);
	runToolBar->addAction(buildAction);
	runToolBar->addAction(launchAction);

	setMenuBar(&m_menuBar);
	addToolBar(Qt::TopToolBarArea, docToolbar);
	addToolBar(Qt::TopToolBarArea, runToolBar);

	//m_codeTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//m_playTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto stack = new QStackedWidget(this);
	int editorIdx = stack->addWidget(m_editor);
	stack->addWidget(m_game);

	stack->setCurrentIndex(editorIdx);

	auto mainWidget = new QWidget(this);
	auto mainLay = new QHBoxLayout(mainWidget);

	mainLay->addWidget(m_tabs);
	mainLay->setAlignment(m_tabs, Qt::AlignTop);

	mainLay->addWidget(stack);

	setCentralWidget(mainWidget);
}

QideWindow::QideWindow(QWidget *parent_)
	: QideWindow(Ctor{}, parent_)
{
	readSettings();
}

QideWindow::QideWindow(QDir projectDir_, QWidget *parent_)
	: QideWindow(Ctor{}, parent_)
{
	QSettings().setValue("projDir", projectDir_.absolutePath());
	readSettings();
}

void QideWindow::setProjectDir(QDir projectDir_){
	QString projPath = projectDir_.path();

	qDebug() << "Opening project:" << projPath;

	auto dirInfo = QFileInfo(projPath);
	if(!dirInfo.exists() || !dirInfo.isDir()){
		throw std::runtime_error("project directory doesn't exist or is not a directory");
	}

	QSettings settings;

	if(m_projectDir.exists()){
		writeSettings();
	}

	m_projectDir.setPath(projectDir_.absolutePath());

	settings.setValue(QString("projDir"), m_projectDir.path());

	readProjSettings();
}

void QideWindow::closeEvent(QCloseEvent *event){
	(void)event;
	writeSettings();
}

void QideWindow::writeSettings(){
	QSettings settings;

	auto projName = m_projectDir.dirName();

	QHash<QString, QVariant> fileBufs;

	for(auto it = m_editor->qcEdit()->fileBuffers().begin(); it != m_editor->qcEdit()->fileBuffers().end(); ++it){
		fileBufs.insert(it.key(), it.value()->toPlainText());
	}

	settings.setValue(QString("projDir"), m_projectDir.absolutePath());
	settings.setValue(QString("%1/geometry").arg(projName), saveGeometry());
	settings.setValue(QString("%1/state").arg(projName), saveState());
	settings.setValue(QString("%1/splitState").arg(projName), m_editor->splitter()->saveState());
	settings.setValue(QString("%1/fileBufs").arg(projName), fileBufs);
	settings.setValue(QString("%1/curFile").arg(projName), m_editor->qcEdit()->fileDir().absolutePath());

	qDebug() << "Wrote settings for project" << projName;
}

void QideWindow::readSettings(){
	QSettings settings;

	auto fteqwPath = settings.value("fteqwPath");
	if(!fteqwPath.isValid() || !QFileInfo(fteqwPath.toString()).exists()){
		m_tabs->playTab()->setEnabled(false);

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

	auto projPath = QDir(settings.value("projDir").toString()).absolutePath();

	setProjectDir(projPath);

	auto projName = m_projectDir.dirName();

	setWindowTitle(QString("QIDE - %1").arg(projName));
}

void QideWindow::readProjSettings(){
	QSettings settings;

	auto projPath = m_projectDir.path();
	auto projName = m_projectDir.dirName();

	QHash<QString, QTextDocument*> projFileBufs;
	QString curFilePath = projPath + "/src/progs.src";
	QString buildPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + projName;

	auto projFileBufsVar = settings.value(QString("%1/fileBufs").arg(projName));
	if(projFileBufsVar.isValid()){
		auto fileBufsVars = projFileBufsVar.value<QHash<QString, QVariant>>();
		projFileBufs.reserve(fileBufsVars.size());

		for(auto it = fileBufsVars.begin(); it != fileBufsVars.end(); ++it){
			auto fileBuf = new QTextDocument;
			fileBuf->setDefaultFont(m_editor->qcEdit()->font());
			fileBuf->setDocumentLayout(new QPlainTextDocumentLayout(fileBuf));
			fileBuf->setPlainText(it.value().toString());
			projFileBufs.insert(it.key(), fileBuf);
		}
	}

	auto projCurFileVar = settings.value(QString("%1/curFile"));
	if(projCurFileVar.isValid()){
		curFilePath = projCurFileVar.toString();
	}
	else{
		settings.setValue(QString("%1/curFile").arg(projName), curFilePath);
	}

	auto projBuildDirVar = settings.value(QString("%1/buildDir"));
	if(projBuildDirVar.isValid()){
		buildPath = projBuildDirVar.toString();
	}
	else{
		settings.setValue(QString("%1/buildDir").arg(projName), buildPath);
	}

	auto savedGeometry = settings.value(QString("%1/geometry").arg(projName));
	if(savedGeometry.isValid()){
		restoreGeometry(savedGeometry.toByteArray());
	}

	auto savedState = settings.value(QString("%1/state").arg(projName));
	if(savedState.isValid()){
		restoreState(savedState.toByteArray());
	}

	auto splitState = settings.value(QString("%1/splitState").arg(projName));
	if(splitState.isValid()){
		m_editor->splitter()->restoreState(splitState.toByteArray());
	}

	m_comp->setSrcPath(projPath);
	m_comp->setBuildPath(buildPath);

	m_editor->setRootDir(projPath);
	m_editor->qcEdit()->setFileBuffers(projFileBufs);

	auto fileIdx = m_editor->fsModel()->index(curFilePath);
	m_editor->treeView()->setCurrentIndex(fileIdx);
	m_editor->qcEdit()->loadFile(curFilePath);
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

	auto playTab = m_tabs->playTab();

	playTab->showProgress();

	auto manager = new QNetworkAccessManager(this);

	auto reply = manager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::downloadProgress, [playTab](qint64 bytesReceived, qint64 bytesTotal){
		double progress = double(bytesReceived) / bytesTotal;

		playTab->setProgress(progress);
	});

	connect(manager, &QNetworkAccessManager::finished, [playTab](QNetworkReply *data){
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

		playTab->hideProgress();
		playTab->setEnabled(true);
	});
}
