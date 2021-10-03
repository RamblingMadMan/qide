#include <QApplication>
#include <QListView>
#include <QStringListModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QStackedLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QFontDatabase>

#include "QideSettings.hpp"
#include "QideWindow.hpp"

QideSettings::QideSettings(QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("QIDE Settings");
	setWidgetDarkMode(this);

	setAttribute(Qt::WA_ShowModal);

	auto doneBtn = new QPushButton("Done");
	doneBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	connect(doneBtn, &QPushButton::pressed, [this]{ close(); });

	auto btnLay = new QHBoxLayout;
	btnLay->setAlignment(Qt::AlignRight);
	btnLay->addWidget(doneBtn);

	auto categoryList = new QListView(this);

	auto categoryModel = new QStringListModel;

	QStringList categories;
	categories.append("Interface");
	categories.append("Editor");

	categoryModel->setStringList(categories);
	categoryList->setModel(categoryModel);

	auto stackLay = new QStackedLayout;

	auto interfaceWidget = new QWidget(this);
	auto interfaceLay = new QFormLayout(interfaceWidget);

	auto fontsCombo = new QComboBox(this);
	auto fontSizes = new QComboBox(this);

	auto fonts = QFontDatabase().families();

	fontsCombo->addItems(fonts);
	fontsCombo->setCurrentText(QApplication::font().family());

	fontSizes->addItem("8");
	fontSizes->addItem("9");
	fontSizes->addItem("10");
	fontSizes->addItem("11");
	fontSizes->addItem("12");
	fontSizes->addItem("13");
	fontSizes->addItem("14");
	fontSizes->setCurrentText(QString("%1").arg(QApplication::font().pointSize()));

	connect(fontsCombo, &QComboBox::currentTextChanged, [=](const QString &family){
		QApplication::setFont(QFont(family, QApplication::font().pointSize()));
	});

	connect(fontSizes, &QComboBox::currentTextChanged, [=](const QString &ptSize){
		QApplication::setFont(QFont(QApplication::font().family(), ptSize.toInt()));
	});

	interfaceLay->addRow("Font", fontsCombo);
	interfaceLay->addRow("Font Size", fontSizes);

	interfaceWidget->setLayout(interfaceLay);

	auto editorWidget = new QWidget(this);
	auto editorLay = new QFormLayout(editorWidget);
	editorWidget->setLayout(editorLay);

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

	auto mainLay = new QVBoxLayout(this);
	mainLay->addLayout(contentLay);
	mainLay->addLayout(btnLay);
}
