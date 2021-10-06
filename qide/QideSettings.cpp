#include <QApplication>
#include <QSettings>
#include <QListView>
#include <QStringListModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QStackedLayout>
#include <QAction>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QFontComboBox>
#include <QSlider>
#include <QFontDatabase>

#include "QideSettings.hpp"
#include "QideWindow.hpp"

QideSettings::QideSettings(QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("QIDE Settings");
	setWidgetDarkMode(this);

	setAttribute(Qt::WA_ShowModal);

	auto oldInterfaceFont = QApplication::font();
	auto oldEditorFont = QSettings().value("editor/font").value<QFont>();

	// Categories

	auto categoryList = new QListView(this);
	auto categoryModel = new QStringListModel(categoryList);

	QStringList categories;
	categories.append("Interface");
	categories.append("Editor");

	categoryModel->setStringList(categories);
	categoryList->setModel(categoryModel);

	auto stackLay = new QStackedLayout;

	// Interface settings

	auto interfaceWidget = new QWidget(this);
	auto interfaceLay = new QFormLayout(interfaceWidget);

	auto fontsCombo = new QFontComboBox(this);

	connect(fontsCombo, &QComboBox::currentTextChanged, [=](const QString &family){
		QApplication::setFont(QFont(family, QApplication::font().pointSize()));
	});

	auto sizeSlider = new QSlider(Qt::Horizontal, this);
	sizeSlider->setMinimum(6);
	sizeSlider->setMaximum(24);

	sizeSlider->setValue(QApplication::font().pointSize());

	connect(sizeSlider, &QSlider::valueChanged, [=](int ptSize){
		QApplication::setFont(QFont(QApplication::font().family(), ptSize));
	});

	interfaceLay->addRow("Font Family", fontsCombo);
	interfaceLay->addRow("Font Size", sizeSlider);

	interfaceWidget->setLayout(interfaceLay);

	// Editor settings

	auto editorWidget = new QWidget(this);
	auto editorLay = new QFormLayout(editorWidget);
	editorWidget->setLayout(editorLay);

	fontsCombo = new QFontComboBox(this);
	fontsCombo->setFontFilters(QFontComboBox::MonospacedFonts);
	fontsCombo->setCurrentFont(oldEditorFont);
	//fontsCombo->setWindowModified(false);

	connect(fontsCombo, &QFontComboBox::currentFontChanged, [=](const QFont &font){
		// TODO: set editor font
	});

	sizeSlider = new QSlider(Qt::Horizontal, this);
	sizeSlider->setMinimum(6);
	sizeSlider->setMaximum(24);
	sizeSlider->setValue(oldEditorFont.pointSize());

	editorLay->addRow("Font Family", fontsCombo);
	editorLay->addRow("Font Size", sizeSlider);

	// Category stack

	stackLay->addWidget(interfaceWidget);
	stackLay->addWidget(editorWidget);

	stackLay->setCurrentWidget(interfaceWidget);

	connect(categoryList, &QListView::activated, this, [=](const QModelIndex &idx){
		auto val = categoryModel->data(idx).toString();
		if(val == "Interface"){
			stackLay->setCurrentWidget(interfaceWidget);
		}
		else if(val == "Editor"){
			stackLay->setCurrentWidget(editorWidget);
		}
	});

	auto contentLay = new QHBoxLayout;
	contentLay->addWidget(categoryList);
	contentLay->addLayout(stackLay);

	// Save/Done

	auto cancelAction = new QAction(this);
	cancelAction->setText("Cancel");

	connect(cancelAction, &QAction::triggered, [this, oldInterfaceFont]{
		QApplication::setFont(oldInterfaceFont);
		// TODO: reset editor font
		close();
	});

	auto cancelBtn = new QToolButton(this);
	cancelBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
	cancelBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	cancelBtn->setDefaultAction(cancelAction);

	auto doneAction = new QAction(this);
	doneAction->setText("Done");

	connect(doneAction, &QAction::triggered, [this, fontsCombo, sizeSlider]{
		auto curFont = QApplication::font();

		auto editorFnt = fontsCombo->currentFont();
		editorFnt.setPointSize(sizeSlider->value());

		QSettings config;

		config.setValue("interface/font", curFont);
		config.setValue("editor/font", editorFnt);

		close();
	});

	auto doneBtn = new QToolButton(this);
	doneBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
	doneBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	doneBtn->setDefaultAction(doneAction);

	auto btnLay = new QHBoxLayout;
	btnLay->setAlignment(Qt::AlignRight);
	btnLay->addWidget(cancelBtn);
	btnLay->addWidget(doneBtn);

	auto mainLay = new QVBoxLayout(this);
	mainLay->addLayout(contentLay);
	mainLay->addLayout(btnLay);
}
