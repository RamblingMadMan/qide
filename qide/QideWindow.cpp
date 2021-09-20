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
#include <QToolBar>

#include "QCVM.hpp"
#include "QCByteCode.hpp"
#include "QCEdit.hpp"
#include "QideEditor.hpp"
#include "QideMapEditor.hpp"
#include "QideGame.hpp"
#include "QideCompiler.hpp"
#include "QideWindow.hpp"

#ifdef _WIN32
#include <dwmapi.h>
#include <VersionHelpers.h>
#endif

QideTabsWidget::QideTabsWidget(QWidget *parent)
	: QWidget(parent)
	, m_codeTab(new QPushButton(QIcon::fromTheme("accessories-text-editor"), "Code"))
	, m_mapTab(new QPushButton(QIcon::fromTheme("applications-internet"), "Map"))
	, m_playTab(new QPushButton(QIcon::fromTheme("applications-games"), "Play"))
	, m_selected(m_codeTab)
{
	auto lay = new QVBoxLayout(this);

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

QideWindow::QideWindow(Ctor, QWidget *parent)
	: QMainWindow(parent)
	, m_tabs(new QideTabsWidget(this))
	, m_vmDock(new QideVMDock(new QCVM))
	, m_editor(new QideEditor(this))
	, m_mapEditor(new QideMapEditor(this))
	, m_game(new QideGame(this))
	, m_comp(new QideCompiler(this))
{
	auto menuBar = new QMenuBar(this);
	auto fileMenu = new QMenu("File", this);
	auto editMenu = new QMenu("Edit", this);

	auto openAction = new QAction(QIcon::fromTheme("document-open"), "Open Project", this);
	auto saveAction = new QAction(QIcon::fromTheme("document-save"), "Save", this);
	auto quitAction = new QAction(QIcon::fromTheme("application-exit"), "Quit", this);
	m_undoAction = new QAction(QIcon::fromTheme("edit-undo"), "Undo", this);
	m_redoAction = new QAction(QIcon::fromTheme("edit-redo"), "Redo", this);
	auto buildAction = new QAction("Build", this);
	auto launchAction = new QAction(QIcon::fromTheme("system-run"), "Launch", this);

	connect(openAction, &QAction::triggered, this, [this]{
		auto newDir = QFileDialog::getExistingDirectory(this, tr("Open Project Directory"), QSettings().value("projDir").toString());
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
		this, [=, this](bool success){
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
	openAction->setEnabled(true);
	buildAction->setEnabled(true);
	launchAction->setEnabled(false);

	m_undoAction->setEnabled(false);
	m_redoAction->setEnabled(false);

	// file menu
	fileMenu->addAction(openAction);
	fileMenu->addAction(saveAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	// edit menu
	editMenu->addAction(m_undoAction);
	editMenu->addAction(m_redoAction);

	// menu bar
	menuBar->addMenu(fileMenu);
	menuBar->addMenu(editMenu);

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

	setMenuBar(menuBar);
	addToolBar(Qt::TopToolBarArea, docToolbar);
	addToolBar(Qt::TopToolBarArea, runToolBar);

	addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, m_vmDock);

	//m_codeTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//m_playTab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto stack = new QStackedWidget(this);

	// added in reverse order for correct stacking
	int gameIdx = stack->addWidget(m_game);
	int mapEditorIdx = stack->addWidget(m_mapEditor);
	int editorIdx = stack->addWidget(m_editor);

	m_mapEditor->setEnabled(false);

	stack->setCurrentIndex(editorIdx);

	connect(m_tabs->codeTab(), &QPushButton::pressed, [=, this]{
		if(m_mapEditor->isEnabled()){
			m_mapEditor->setEnabled(false);
			m_editor->setOpacity(0.25);
		}

		stack->setCurrentIndex(editorIdx);
	});

	connect(m_tabs->mapTab(), &QPushButton::pressed, [=, this]{
		m_mapEditor->setEnabled(true);
		stack->setCurrentIndex(mapEditorIdx);
	});

	connect(m_tabs->playTab(), &QPushButton::pressed, [=, this]{
		if(!m_game->isRunning()){
			m_game->launch();
		}

		stack->setCurrentIndex(gameIdx);
	});

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

#ifdef _WIN32
	if(IsWindows10OrGreater()){
		const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
		const int useDarkMode = 1;
		DwmSetWindowAttribute((HWND)winId(), DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
	}
#endif
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
	settings.setValue(QString("%1/curFile").arg(projName), m_editor->qcEdit()->fileDir().absolutePath());

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
