#ifndef QIDE_QIDECOMPILER_HPP
#define QIDE_QIDECOMPILER_HPP 1

#include <QObject>

class QFileSystemWatcher;

class QideCompiler: public QObject{
	Q_OBJECT

	Q_PROPERTY(QString srcPath READ srcPath WRITE setSrcPath NOTIFY srcPathChanged)
	Q_PROPERTY(QString buildPath READ buildPath WRITE setBuildPath NOTIFY buildPathChanged)

	public:
		explicit QideCompiler(QObject *parent = nullptr);

		~QideCompiler();

		const QString &srcPath() const noexcept{ return m_srcPath; }
		const QString &buildPath() const noexcept{ return m_buildPath; }

		void setSrcPath(const QString &path);
		void setBuildPath(const QString &path);

	public slots:
		void compile();

	signals:
		void srcPathChanged();
		void buildPathChanged();

		void progsSrcCompiled(const QString &progsSrcPath, bool success);

		void compileStarted();
		void compileFinished(bool success);

	private:
		QString m_srcPath;
		QString m_buildPath;
};

#endif // !QIDE_QIDECOMPILER_HPP
