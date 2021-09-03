#ifndef QIDE_QIDECOMPILER_HPP
#define QIDE_QIDECOMPILER_HPP 1

#include <QObject>

class QFileSystemWatcher;

class QideCompiler: public QObject{
	Q_OBJECT

	Q_PROPERTY(bool canCompile READ canCompile NOTIFY canCompileChanged)

	public:
		explicit QideCompiler(QObject *parent = nullptr);

		~QideCompiler();

		bool canCompile() const noexcept{ return m_canCompile; }

	public slots:
		void compile();

	signals:
		void canCompileChanged();
		void compileStarted();
		void compileFinished(bool success);

	protected:
		void setCanCompile(bool canCompile_);

	private:
		QFileSystemWatcher *m_watcher;
		QString m_progsSrcPath;
		struct ftepp_t *m_ftepp;
		bool m_canCompile = false;
};

#endif // !QIDE_QIDECOMPILER_HPP
