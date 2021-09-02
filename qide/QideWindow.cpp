#include "fmt/core.h"

#include <QtMath>
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

#include "QideEditor.hpp"
#include "QideGame.hpp"
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
	, m_menuBar(this)
	, m_fileMenu("File", this)
	, m_editMenu("Edit", this)
	, m_codeTab(new QideTab(QIcon::fromTheme("accessories-text-editor"), "Code", this))
	, m_playTab(new QideTab(QIcon::fromTheme("applications-games"), "Play", this))
	, m_openAction(QIcon::fromTheme("document-open"), "Open", this)
	, m_saveAction(QIcon::fromTheme("document-save"), "Save", this)
	, m_quitAction(QIcon::fromTheme("application-exit"), "Quit", this)
	, m_undoAction(QIcon::fromTheme("edit-undo"), "Undo", this)
	, m_redoAction(QIcon::fromTheme("edit-redo"), "Redo", this)
	, m_buildAction(QIcon::fromTheme(""), "Build", this)
	, m_launchAction(QIcon::fromTheme("system-run"), "Launch", this)
{
	connect(&m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
	connect(&m_undoAction, &QAction::triggered, m_editor->qcEdit(), &QPlainTextEdit::undo);
	connect(&m_redoAction, &QAction::triggered, m_editor->qcEdit(), &QPlainTextEdit::redo);

	connect(m_editor->qcEdit(), &QPlainTextEdit::undoAvailable, &m_undoAction, &QAction::setEnabled);
	connect(m_editor->qcEdit(), &QPlainTextEdit::redoAvailable, &m_redoAction, &QAction::setEnabled);

	m_saveAction.setDisabled(true);
	m_openAction.setDisabled(true);
	m_undoAction.setDisabled(true);
	m_redoAction.setDisabled(true);

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
	QSettings settings;

	settings.setValue("workDir", projectDir_.path());

	setProjectDir(projectDir_);
}

void QideWindow::setProjectDir(QDir projectDir_){
	QString projPath = projectDir_.path();

	auto dirInfo = QFileInfo(projPath);
	if(!dirInfo.exists() || !dirInfo.isDir()){
		throw std::runtime_error("project directory doesn't exist or is not a directory");
	}

	QSettings settings;

	auto currentFilePath = projPath + "/src/defs.qc";

	settings.setValue("workDir", projPath);
	settings.setValue("currentFile", currentFilePath);

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

		m_playTab->hideProgress();
		m_playTab->setEnabled(true);
	});
}

void QideWindow::readSettings(){
	QSettings settings;

	auto fteqwPath = settings.value("fteqwPath");
	if(!fteqwPath.isValid() || !QDir(fteqwPath.toString()).exists()){
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

	setWindowTitle(settings.value("title", "QIDE").toString());

	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());

	m_editor->splitter()->restoreState(settings.value("splitState").toByteArray());
	m_editor->qcEdit()->setFileBuffers(settings.value("fileBufs").value<QHash<QString, QString>>());

	setProjectDir(settings.value("workDir").toString());
}
