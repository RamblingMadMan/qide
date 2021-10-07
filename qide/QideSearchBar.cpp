#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QStyleOption>
#include <QPainter>

#include "QideSearchBar.hpp"

#include "QuakeColors.hpp"

QideSearchBar::QideSearchBar(QWidget *parent)
	: QWidget(parent)
{
	setStyleSheet(QString(
		"padding: 2px 0 2px 0;"
		"background-color: %1;"
		"border-radius: 0;"
	).arg(quakeDarkGrey.darker(150).name(QColor::HexRgb)));

	//setAutoFillBackground(true);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	auto image = QImage(":/img/ui/search.svg");
	image.invertPixels();

	auto pixmap = QPixmap::fromImage(image).scaledToWidth(16, Qt::SmoothTransformation);

	auto label = new QLabel(this);
	label->setPixmap(pixmap);

	label->setStyleSheet(
		"margin: 0 0 0 5px;"
		"border-radius: 0;"
	);

	m_entry = new QLineEdit(this);
	m_entry->setPlaceholderText("Search...");

	m_entry->setStyleSheet(
		"padding: 2px 0 2px 0;"
		"border-radius: 0;"
		"background-color: #00000000;"
	);

	connect(m_entry, &QLineEdit::textChanged, this, &QideSearchBar::textChanged);
	connect(m_entry, &QLineEdit::returnPressed, this, &QideSearchBar::triggered);

	//m_dropDown = new QListView(this);

	auto lay = new QHBoxLayout(this);

	lay->setContentsMargins(0, 0, 0, 0);

	lay->addWidget(label);
	lay->addWidget(m_entry);

	setLayout(lay);
}

QString QideSearchBar::text() const{ return m_entry->text(); }

void QideSearchBar::setText(const QString &text_){
	m_entry->setText(text_);
	emit textChanged();
}

void QideSearchBar::setResults(const QStringList &results_){
	m_results = results_;
	emit resultsChanged();
}

void QideSearchBar::showResults(){}

void QideSearchBar::hideResults(){}

void QideSearchBar::paintEvent(QPaintEvent* event){
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

	QWidget::paintEvent(event);
}
