#ifndef QIDE_QIDEFILEWIZARD_HPP
#define QIDE_QIDEFILEWIZARD_HPP 1

#include <QWizard>

class QideFileWizardInfo: public QWizardPage{
	Q_OBJECT

	public:
		explicit QideFileWizardInfo(QWidget *parent_ = nullptr);
};

class QideFileWizard: public QWizard{
	Q_OBJECT

	public:
		explicit QideFileWizard(QWidget *parent_ = nullptr);
};

#endif // !QIDE_QIDEFILEWIZARD_HPP
