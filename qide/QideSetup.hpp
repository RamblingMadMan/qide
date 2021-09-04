#ifndef QIDE_QIDESETUP_HPP
#define QIDE_QIDESETUP_HPP 1

#include <QWizardPage>

class QLineEdit;

class QideSetupIntro: public QWizardPage{
	Q_OBJECT

	public:
		explicit QideSetupIntro(QWidget *parent = nullptr);
};

class QideSetupQuake: public QWizardPage{
	Q_OBJECT

	public:
		explicit QideSetupQuake(QWidget *parent = nullptr);
};

class QideSetupFTEQW: public QWizardPage{
	Q_OBJECT

	public:
		explicit QideSetupFTEQW(QWidget *parent = nullptr);

		bool isComplete() const override;

	private:
		bool m_pathIsValid = false;
		bool m_downloading = false;
};

class QideSetup: public QWizard{
	Q_OBJECT

	public:
		explicit QideSetup(QWidget *parent = nullptr);
};

#endif // !QIDE_QIDESETUP_HPP
