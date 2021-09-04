#ifndef QIDE_QIDEGAME_HPP
#define QIDE_QIDEGAME_HPP 1

#include <QWidget>
#include <QProcess>

class QideGame: public QWidget{
	Q_OBJECT

	public:
		explicit QideGame(QWidget *parent = nullptr);

		QString workDir();

		void setWorkDir(const QString &dir);

		void launch();

	signals:
		void workDirChanged();

		void launched();
		void exited(int status);

	private slots:
		void onExited(int status, QProcess::ExitStatus exitStatus);

	private:
		QProcess *m_proc;

};

#endif // !QIDE_QIDEGAME_HPP
