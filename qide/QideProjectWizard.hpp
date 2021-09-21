#ifndef QIDE_QIDEPROJECTWIZARD_HPP
#define QIDE_QIDEPROJECTWIZARD_HPP 1

#include <QWizardPage>

class QideProjectWizardInfo: public QWizardPage{
	Q_OBJECT

	public:
		explicit QideProjectWizardInfo(QWidget *parent = nullptr);
};

class QideProjectWizard: public QWizard{
	Q_OBJECT

	public:
		explicit QideProjectWizard(QWidget *parent = nullptr);
};

#endif // !QIDE_QIDEPROJECTWIZARD_HPP
