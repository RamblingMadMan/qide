#include <QDebug>
#include <QDirIterator>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSizePolicy>
#include <QStandardPaths>

#ifdef _WIN32
#include <dwmapi.h>
#include <versionhelpers.h>
#endif

#include "QideProjectWizard.hpp"
#include "QideWindow.hpp"

QideProjectWizardInfo::QideProjectWizardInfo(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle("Project Settings");
	setSubTitle("Information about your new project.");

	auto templateCombo = new QComboBox;

	auto templateDirIter = QDirIterator(":/templates");
	while(templateDirIter.hasNext()){
		auto templateDir = templateDirIter.next();
		templateCombo->addItem(QFileInfo(templateDir).baseName());
	}

	registerField("template", templateCombo, "currentText");

	auto nameEdit = new QLineEdit;
	nameEdit->setPlaceholderText("My Quake Mod");

	registerField("name*", nameEdit);

	const auto docsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

	auto dirEntry = new QLineEdit;
	dirEntry->setText(QDir(QString("%1/QIDE Projects").arg(docsDir)).path());
	dirEntry->setReadOnly(true);

	registerField("dir", dirEntry);

	auto dirOpen = new QPushButton(QIcon::fromTheme("document-open"), "Browse");
	QObject::connect(dirOpen, &QPushButton::pressed, [dirEntry]{
		auto result = QFileDialog::getExistingDirectory(nullptr, "Select where the mod directory will be created", QDir::currentPath());
		if(!result.isEmpty()){
			dirEntry->setText(result);
		}
	});

	auto lay = new QFormLayout;

	lay->addRow("Template", templateCombo);
	lay->addRow("Name", nameEdit);

	auto dirLay = new QHBoxLayout;
	dirLay->addWidget(dirEntry);
	dirLay->addWidget(dirOpen);

	lay->addRow("Directory", dirLay);

	setLayout(lay);
}

QideProjectWizard::QideProjectWizard(QWidget *parent)
	: QWizard(parent)
{
	setWizardStyle(QWizard::ModernStyle);
	setWidgetDarkMode(this);

	auto infoPage = new QideProjectWizardInfo(this);
	//infoPage->setPalette(palette());

	addPage(infoPage);
}
