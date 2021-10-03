#include <stdexcept>

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
#include <QStringListModel>
#include <QListView>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QFileDialog>
#include <QMenuBar>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QToolBar>
#include <QStackedLayout>

#include "QCVM.hpp"
#include "QCByteCode.hpp"
#include "QCEdit.hpp"
#include "QidePakExplorer.hpp"
#include "QideEditor.hpp"
#include "QideMapEditor.hpp"
#include "QideGame.hpp"
#include "QideCompiler.hpp"
#include "QideWindow.hpp"
#include "QideProjectWizard.hpp"
#include "QideFileWizard.hpp"
#include "QideSettings.hpp"

#include "QuakeColors.hpp"

#ifdef _WIN32
#include <dwmapi.h>
#include <versionhelpers.h>
#endif

void setWidgetDarkMode(QWidget *widget){
#ifdef _WIN32
	if(IsWindows10OrGreater()){
		const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
		const int useDarkMode = 1;
		DwmSetWindowAttribute((HWND)widget->winId(), DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
	}
#endif
}

// TODO: make this class more generic (add/remove tabs)
QideTabsWidget::QideTabsWidget(QWidget *parent)
	: QWidget(parent)
	, m_codeTab(new QPushButton("Code"))
	, m_mapTab(new QPushButton("Map"))
	, m_playTab(new QPushButton("Play"))
	, m_selected(m_codeTab)
{	
	setContentsMargins(0, 0, 0, 0);

	const auto quakeDarkGreyHex = quakeDarkGrey.name(QColor::HexRgb);
	const auto quakeOrangeHex = quakeOrange.name(QColor::HexRgb);
	const auto quakeDarkBrownHex = quakeDarkBrown.name(QColor::HexRgb);

	setStyleSheet(QString(
		"QPushButton {"
			"margin-right: 0;"
			"padding: 10px 10px 10px 10px;"
			"background-color: %1;"
			"border-radius: 0;"
		"}"

		"QPushButton:hover {"
			"background-color: %2;"
		"}"

		"QPushButton:pressed {"
			"background-color: %3;"
		"}"
	).arg(quakeDarkGreyHex, quakeOrangeHex, quakeDarkBrownHex));

	auto lay = new QVBoxLayout(this);

	lay->setContentsMargins(0, 0, 0, 0);

	QPushButton *btns[] = { m_codeTab, m_mapTab, m_playTab };
	QImage imgs[] = { QImage(":/img/ui/code.svg"), QImage(":/img/ui/map.svg"), QImage(":/img/ui/play-circle.svg") };

	for(std::size_t i = 0; i < std::size(btns); i++){
		auto btn = btns[i];
		auto img = &imgs[i];
		img->invertPixels();
		btn->setIcon(QIcon(QPixmap::fromImage(*img)));
		btn->setContentsMargins(0, 0, 0, 0);
		lay->addWidget(btn);
	}

	lay->addWidget(m_codeTab);
	lay->addWidget(m_mapTab);
	lay->addWidget(m_playTab);

	setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
}

QideVMDock::QideVMDock(QWidget *parent)
	: QDockWidget("QuakeC VM", parent)
	, m_vm(nullptr)
{
	auto fnView = new QListView;
	m_model = new QStringListModel;

	fnView->setContentsMargins(0, 0, 0, 0);
	fnView->setModel(m_model);

	setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	setWidget(fnView);
}

QStringList QideVMDock::fns() const{
	return m_model->stringList();
}

void QideVMDock::setVm(QCVM *vm_){
	if(m_vm == vm_) return;

	connect(vm_, &QCVM::byteCodeChanged, this, &QideVMDock::updateFnList);
	vm_->setParent(this);

	disconnect(m_vm, &QCVM::byteCodeChanged, this, &QideVMDock::updateFnList);
	delete m_vm;
	m_vm = vm_;

	emit vmChanged();

	updateFnList();
}

void QideVMDock::updateFnList(){
	qDebug() << "Updating function list";

	auto fnList = m_vm->fns();

	QStringList strs;
	strs.reserve(fnList.size());

	for(auto &&fn : fnList){
		strs.push_back(fn->name());
	}

	m_model->setHeaderData(0, Qt::Orientation::Horizontal, "Functions");
	m_model->setStringList(strs);

	emit fnsChanged();
}

static void recurseAddDir(QDir d, QStringList& list) {
	QStringList qsl = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

	foreach(QString file, qsl){
		QFileInfo finfo(QString("%1/%2").arg(d.path(), file));

		if(finfo.isSymLink())
			return;

		if(finfo.isDir()){
			//QString dirname = finfo.fileName();
			QDir sd(finfo.filePath());

			recurseAddDir(sd, list);

		}
		else{
			list << QDir::toNativeSeparators(finfo.filePath());
		}
	}
}

QideWindow::QideWindow(Ctor, QWidget *parent)
	: QMainWindow(parent)
	, m_tabs(new QideTabsWidget(this))
	, m_vmDock(new QideVMDock(new QCVM))
	, m_editor(new QideEditor(this))
	, m_mapEditor(new QideMapEditor(this))
	, m_game(new QideGame(this))
	, m_comp(new QideCompiler(this))
	, m_settings(nullptr)
{
	setContentsMargins(0, 0, 0, 0);

	auto menuBar = new QMenuBar(this);
	auto fileMenu = new QMenu("File", this);
	auto editMenu = new QMenu("Edit", this);
	auto helpMenu = new QMenu("Help", this);

	auto plusImg = QImage(":/img/ui/plus.svg");
	auto folderImg = QImage(":/img/ui/folder.svg");
	auto newFileImg = QImage(":/img/ui/file-add.svg");
	auto saveImg = QImage(":/img/ui/save.svg");
	auto quitImg = QImage(":/img/ui/close.svg");
	auto undoImg = QImage(":/img/ui/undo.svg");
	auto redoImg = QImage(":/img/ui/undo.svg").mirrored(true, false);
	auto playImg = QImage(":/img/ui/play-circle.svg");
	auto buildImg = QImage(":/img/ui/code-download.svg");
	auto settingsImg = QImage(":/img/ui/settings.svg");
	plusImg.invertPixels();
	folderImg.invertPixels();
	newFileImg.invertPixels();
	saveImg.invertPixels();
	quitImg.invertPixels();
	undoImg.invertPixels();
	redoImg.invertPixels();
	playImg.invertPixels();
	buildImg.invertPixels();
	settingsImg.invertPixels();

	auto newProjAction = new QAction(QIcon(QPixmap::fromImage(plusImg)), "New Project", this);
	auto openProjAction = new QAction(QIcon(QPixmap::fromImage(folderImg)), "Open Project", this);
	auto newFileAction = new QAction(QIcon(QPixmap::fromImage(newFileImg)), "New file", this);
	auto saveAction = new QAction(QIcon(QPixmap::fromImage(saveImg)), "Save", this);
	saveAction->setShortcut(Qt::CTRL + Qt::Key_S);

	auto saveAllAction = new QAction(QIcon(QPixmap::fromImage(saveImg)), "Save All", this);
	saveAllAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);

	auto settingsAction = new QAction(QIcon(QPixmap::fromImage(settingsImg)), "Settings", this);

	connect(settingsAction, &QAction::triggered, this, [this]{
		delete m_settings;

		m_settings = new QideSettings;
		m_settings->show();
	});

	auto quitAction = new QAction(QIcon(QPixmap::fromImage(quitImg)), "Quit", this);

	m_undoAction = new QAction(QIcon(QPixmap::fromImage(undoImg)), "Undo", this);
	m_redoAction = new QAction(QIcon(QPixmap::fromImage(redoImg)), "Redo", this);

	auto buildAction = new QAction(QIcon(QPixmap::fromImage(buildImg)), "Build", this);
	auto launchAction = new QAction(QIcon(QPixmap::fromImage(playImg)), "Launch", this);

	connect(newProjAction, &QAction::triggered, this, [this]{
		auto wizard = new QideProjectWizard(this);
		wizard->show();

		connect(wizard, &QWizard::finished, this, [=](int result){
			if(result == QDialog::Rejected) return;

			auto templ = wizard->field("template").toString();
			QDir baseDir = wizard->field("dir").toString();
			auto name = wizard->field("name").toString();

			baseDir.mkdir(name);

			QDir projDir = QString("%1/%2").arg(baseDir.path(), name);

			auto templateDir = QDir(QString(":/templates/%1").arg(templ));

			QStringList templateFiles;
			recurseAddDir(templateDir, templateFiles);

			auto templatePath = templateDir.path();

			foreach(auto &str, templateFiles){

				auto relPath = str.midRef(templatePath.size() + 1);
				auto newPath = QString("%1/%2").arg(projDir.path()).arg(relPath);
				QDir().mkpath(QFileInfo(newPath).dir().path());
				QFile::copy(str, newPath);
				qDebug() << "Template file:" << relPath;
			}

			setProjectDir(projDir);
		});

		// TODO: connect onFinished
	});

	connect(openProjAction, &QAction::triggered, this, [this]{
		auto newDir = QFileDialog::getExistingDirectory(this, tr("Open Project Directory"), QSettings().value("projDir").toString());
		setProjectDir(newDir);
	});

	connect(newFileAction, &QAction::triggered, this, [this]{
		auto wizard = new QideFileWizard(this);
		wizard->show();
	});

	connect(saveAction, &QAction::triggered, this, [this]{
		writeSettings();
		m_editor->saveCurrent();
	});

	connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
	connect(buildAction, &QAction::triggered, m_comp, &QideCompiler::compile);

	//connect(m_editor->qcEdit(), &QPlainTextEdit::undoAvailable, &m_undoAction, &QAction::setEnabled);
	//connect(m_editor->qcEdit(), &QPlainTextEdit::redoAvailable, &m_redoAction, &QAction::setEnabled);

	connect(m_editor->qcEdit(), &QCEdit::hasChangesChanged, saveAction, [this, saveAction]{ saveAction->setEnabled(m_editor->qcEdit()->hasChanges()); });

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

			if(success){
				QFile file(m_comp->buildPath() + "/progs.dat");
				if(file.open(QFile::ReadOnly)){
					auto bc = file.readAll();
					m_vmDock->vm()->byteCode()->setByteCode(bc);
					m_vmDock->updateFnList();
				}
			}
		}
	);

	saveAction->setEnabled(false);
	openProjAction->setEnabled(true);
	buildAction->setEnabled(true);
	launchAction->setEnabled(false);

	m_undoAction->setEnabled(false);
	m_redoAction->setEnabled(false);

	// file menu
	fileMenu->addAction(newProjAction);
	fileMenu->addAction(openProjAction);
	fileMenu->addSeparator();
	fileMenu->addAction(newFileAction);
	fileMenu->addAction(saveAction);
	fileMenu->addAction(saveAllAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	// edit menu
	editMenu->addAction(m_undoAction);
	editMenu->addAction(m_redoAction);
	editMenu->addSeparator();
	editMenu->addAction(settingsAction);

	// menu bar
	menuBar->addMenu(fileMenu);
	menuBar->addMenu(editMenu);
	menuBar->addMenu(helpMenu);

	// tool bar buttons
	auto docToolbar = new QToolBar(this);
	docToolbar->addAction(openProjAction);
	docToolbar->addSeparator();
	docToolbar->addAction(newFileAction);
	docToolbar->addAction(saveAction);
	docToolbar->addAction(saveAllAction);
	docToolbar->addSeparator();
	docToolbar->addAction(m_undoAction);
	docToolbar->addAction(m_redoAction);

	auto runToolBar = new QToolBar(this);
	runToolBar->addAction(buildAction);
	runToolBar->addAction(launchAction);

	setMenuBar(menuBar);
	addToolBar(Qt::TopToolBarArea, docToolbar);
	addToolBar(Qt::TopToolBarArea, runToolBar);

	auto pakDock = new QDockWidget(this);
	auto pakExplorer = new QidePakExplorer(pakDock);

	pakExplorer->setContentsMargins(0, 0, 0, 0);

	pakDock->setTitleBarWidget(new QLabel("Pak Explorer"));
	pakDock->setWidget(pakExplorer);
	pakDock->setContentsMargins(0, 0, 0, 0);

	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, pakDock);
	addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, m_vmDock);

	//m_codeTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//m_playTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	//m_mapEditor->setEnabled(false);

	auto stackLay = new QStackedLayout;
	stackLay->setStackingMode(QStackedLayout::StackAll);
	stackLay->setContentsMargins(0, 0, 0, 0);

	stackLay->insertWidget(2, m_editor);
	stackLay->insertWidget(1, m_mapEditor);
	stackLay->insertWidget(0, m_game);

	m_game->hide();
	m_mapEditor->hide();

	stackLay->setCurrentIndex(0);

	connect(m_tabs->codeTab(), &QPushButton::pressed, [=]{
		bool showMap = m_mapEditor->isVisible();

		m_editor->show();
		m_editor->raise();

		m_game->hide();

		stackLay->setCurrentWidget(m_editor);

		if(showMap){
			m_mapEditor->show();
			m_editor->setOpacity(0.925);
		}
		else{
			m_editor->setOpacity(1.0);
		}
	});

	connect(m_tabs->mapTab(), &QPushButton::pressed, [=]{
		m_editor->hide();
		m_game->hide();
		m_mapEditor->show();
		m_mapEditor->raise();

		stackLay->setCurrentWidget(m_mapEditor);
	});

	connect(m_tabs->playTab(), &QPushButton::pressed, [=]{
		m_mapEditor->hide();
		m_editor->hide();
		m_game->show();

		stackLay->setCurrentWidget(m_game);

		if(!m_game->isRunning()){
			m_game->launch();
		}
	});

	auto mainWidget = new QWidget(this);
	mainWidget->setContentsMargins(0, 0, 0, 0);

	auto mainLay = new QHBoxLayout(mainWidget);
	mainLay->setContentsMargins(0, 0, 0, 0);
	mainLay->setAlignment(m_tabs, Qt::AlignTop);

	mainLay->addWidget(m_tabs);
	mainLay->addLayout(stackLay);

	setCentralWidget(mainWidget);
}

QideWindow::QideWindow(QWidget *parent_)
	: QideWindow(Ctor{}, parent_)
{
	readSettings();
	setWidgetDarkMode(this);
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
	writeSettings();
	QMainWindow::closeEvent(event);
}

void QideWindow::writeSettings(){
	QSettings settings;

	auto projName = m_projectDir.dirName();

	QHash<QString, QVariant> fileBufs;

	for(auto it = m_editor->qcEdit()->fileBuffers().begin(); it != m_editor->qcEdit()->fileBuffers().end(); ++it){
		fileBufs.insert(it.key(), it.value()->toPlainText());
	}

	settings.setValue(QString("projDir"), m_projectDir.path());
	settings.setValue(QString("%1/geometry").arg(projName), saveGeometry());
	settings.setValue(QString("%1/state").arg(projName), saveState());
	settings.setValue(QString("%1/splitState").arg(projName), m_editor->splitter()->saveState());
	settings.setValue(QString("%1/fileBufs").arg(projName), fileBufs);
	settings.setValue(QString("%1/curFile").arg(projName), m_editor->qcEdit()->filePath());

	qDebug() << "Wrote settings for project" << projName;
}

void QideWindow::readSettings(){
	QSettings settings;

	auto fteqwPath = settings.value("fteqwPath");
	m_tabs->playTab()->setEnabled(fteqwPath.isValid() && QFileInfo::exists(fteqwPath.toString()));

	auto projPath = QDir(settings.value("projDir").toString());

	setProjectDir(projPath);

	auto projName = m_projectDir.dirName();

	setWindowTitle(QString("QIDE - %1").arg(projName));
}

void QideWindow::readProjSettings(){
	QSettings settings;

	auto projPath = m_projectDir.absolutePath();
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
