#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

#include "QideWindow.hpp"
#include "QideFileWizard.hpp"

QideFileWizardInfo::QideFileWizardInfo(QWidget *parent_)
	: QWizardPage(parent_)
{
	setTitle("New File");
	setSubTitle("Choose a name and whether to add the new file to progs.src");

	auto templateCombo = new QComboBox;
	templateCombo->addItem("Blank");
	registerField("template", templateCombo, "currentText");

	auto nameInput = new QLineEdit;
	registerField("name*", nameInput);

	auto progsCheck = new QCheckBox;
	registerField("progsAdd", progsCheck);

	auto lay = new QFormLayout(this);
	lay->addRow("Template", templateCombo);
	lay->addRow("Name", nameInput);
	lay->addRow("Add to progs", progsCheck);
	lay->setAlignment(templateCombo, Qt::AlignVCenter);
	lay->setAlignment(nameInput, Qt::AlignVCenter);
	lay->setAlignment(progsCheck, Qt::AlignVCenter);
}

QideFileWizard::QideFileWizard(QWidget *parent_)
	: QWizard(parent_)
{
	setWizardStyle(QWizard::ModernStyle);
	setWidgetDarkMode(this);

	auto infoPage = new QideFileWizardInfo(this);

	addPage(infoPage);
}
