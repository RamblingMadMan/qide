#ifndef QIDE_QIDEGAME_HPP
#define QIDE_QIDEGAME_HPP 1

#include <QWidget>

class QProcess;

class QideGame: public QWidget{
	Q_OBJECT

	public:
		explicit QideGame(QWidget *parent = nullptr);

		void launch();

	private:
		QProcess *m_proc;

};

#endif // !QIDE_QIDEGAME_HPP
