#include <QSettings>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QListView>
#include <QStringListModel>

#include "QideSearchBar.hpp"
#include "QideVMDock.hpp"

#include "QuakeColors.hpp"

QideVMDock::QideVMDock(QCVM *vm_, QWidget *parent)
	: QWidget(parent), m_vm(vm_)
{
	auto grid = new QGridLayout(this);
	grid->setSpacing(0);
	grid->setMargin(0);

	// Results

	auto resultsWidget = new QWidget(this);
	resultsWidget->setContentsMargins(0, 0, 0, 0);
	//resultsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	auto resultsLay = new QVBoxLayout(resultsWidget);
	resultsLay->setSpacing(0);
	resultsLay->setMargin(0);

	auto searchBar = new QideSearchBar(this);

	auto resultsFilter = new QComboBox(this);
	auto resultsView = new QListView(this);
	auto resultsModel = new QStringListModel(resultsView);

	resultsFilter->setStyleSheet(
		QString("border-bottom: 1px solid %1;").arg(quakeDarkGrey.darker(200).name(QColor::HexRgb))
	);

	resultsView->setModel(resultsModel);

	resultsLay->addWidget(resultsFilter);
	resultsLay->addWidget(resultsView);

	// Selected name and type

	auto selectedLabel = new QLabel(QString("<i>name</i> : <i>param</i> -> <i>result<i>"), this);
	selectedLabel->setContentsMargins(15, 0, 0, 0);

	auto editorFnt = QSettings().value("editor/font").value<QFont>();

	selectedLabel->setFont(editorFnt);

	// Metadata view

	auto dataWidget = new QWidget(this);
	dataWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto dataLay = new QGridLayout(dataWidget);

	auto typeLabel = new QLabel("Type");
	auto typeValue = new QLabel("<i>null</i>");

	dataLay->addWidget(typeLabel, 0, 0);
	dataLay->addWidget(typeValue, 0, 1);

	dataLay->setAlignment(typeLabel, Qt::AlignLeft);
	dataLay->setAlignment(typeValue, Qt::AlignLeft);

	dataLay->setColumnMinimumWidth(0, 86);
	dataLay->setColumnMinimumWidth(1, 64);
	dataLay->setColumnStretch(0, 0);

	// Execution

	auto execInLabel = new QLabel("Input(s)", this);
	auto execInEntry = new QLineEdit(this);

	auto execWidget = new QWidget(this);
	execWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto execLay = new QGridLayout(execWidget);
	execLay->setColumnMinimumWidth(0, 16);
	execLay->addWidget(execInLabel, 0, 0, Qt::AlignLeft);
	execLay->addWidget(execInEntry, 0, 1, Qt::AlignLeft);

	// lay[v] searchBar (selectionLay[h] resultsLay (innerLay[v] (selectedLabel (selectedLay[h] (...))))

	auto selectedWidget = new QWidget(this);
	selectedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	auto selectedLay = new QHBoxLayout(selectedWidget);
	selectedLay->addWidget(dataWidget);
	selectedLay->addWidget(execWidget);

	selectedLay->setAlignment(dataWidget, Qt::AlignTop);
	selectedLay->setAlignment(execWidget, Qt::AlignTop);

	grid->addWidget(searchBar, 0, 0);
	grid->addWidget(resultsWidget, 1, 0);

	grid->addWidget(selectedLabel, 0, 1);
	grid->addWidget(selectedWidget, 1, 1);

	grid->setAlignment(selectedWidget, Qt::AlignTop);

	setLayout(grid);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}
