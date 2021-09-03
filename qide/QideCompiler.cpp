#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

#include "gmqcc.h"
#include "gmqcc/parser.h"

// required by gmqcc (^:',
const oper_info *operators = nullptr;
size_t operator_count = 0;

#include "QideCompiler.hpp"

QideCompiler::QideCompiler(QObject *parent)
	: QObject(parent)
	, m_srcPath(QDir::currentPath())
	, m_buildPath(m_srcPath)
{
	con_init();
	opts_init("progs.dat", COMPILER_QCC, (1024 << 3));

	util_seed(time(0));

	operators = qcc_operators;
	operator_count = GMQCC_ARRAY_COUNT(qcc_operators);

	opts_set(opts.flags, ADJUST_VECTOR_FIELDS,  false);
	opts_set(opts.flags, ASSIGN_FUNCTION_TYPES, true);
	opts_setflag("LNO", true);
	opts_setoptimlevel(2);

	OPTS_OPTION_U32(OPTION_STANDARD) = COMPILER_QCC;
	OPTS_OPTION_BOOL(OPTION_DEBUG) = true;
	OPTS_OPTION_BOOL(OPTION_G) = true;
	OPTS_OPTION_U32(OPTION_O) = 2;
}

QideCompiler::~QideCompiler()
{
	con_close();
}

void QideCompiler::setSrcPath(const QString &path){
	auto dir = QDir(path);
	dir.makeAbsolute();

	if(dir == QDir(m_srcPath)) return;

	m_srcPath = dir.path();
	emit srcPathChanged();
}

void QideCompiler::setBuildPath(const QString &path){
	auto dir = QDir(path);
	dir.makeAbsolute();

	if(dir == QDir(m_buildPath)) return;

	if(!dir.exists() && !QDir().mkdir(dir.path())){
		return;
	}

	m_buildPath = dir.path();
	emit buildPathChanged();
}

void QideCompiler::compile()
{
	QVector<QString> m_progsSrcs;

	QDirIterator dirIt(m_srcPath, QDirIterator::Subdirectories);

	while(dirIt.hasNext()){
		dirIt.next();

		QFileInfo fileInfo(dirIt.filePath());
		if(fileInfo.isFile() && fileInfo.fileName() == "progs.src"){
			auto progsSrcPath = dirIt.filePath();
			m_progsSrcs.push_back(progsSrcPath);
		}
	}

	emit compileStarted();

	bool allCompiled = true;

	for(const auto &progsSrcPath : m_progsSrcs){
		qDebug() << "Compiling" << progsSrcPath;

		auto file = QFile(progsSrcPath);
		if(!file.open(QFile::ReadOnly)){
			allCompiled = false;
			qDebug() << "Could not open file" << progsSrcPath;
			emit progsSrcCompiled(progsSrcPath, false);
			continue;
		}

		auto progsSrcInfo = QFileInfo(progsSrcPath);

		auto progsSrcDir = progsSrcInfo.absoluteDir().path().mid(m_srcPath.length() + 1);

		qDebug() << "progs.src dir:" << progsSrcDir;

		auto buildOutPath = m_buildPath;
		if(!progsSrcDir.isEmpty()){
			buildOutPath += "/" + progsSrcDir;
		}

		qDebug() << "Build path:" << buildOutPath;

		opts_setoptimlevel(2);
		opts_setflag("LNO", true);

		QString outputFile;
		bool hasOutputLine = false;

		QVector<QString> files;

		files.reserve(16);

		QTextStream in(&file);

		while(!in.atEnd()){
			auto line = in.readLine();

			if(line.isEmpty() || (line[0] == '\0') || (line[0] == '/' && line[1] == '/')){
				continue;
			}

			int nonAlphaSlash = -1;
			for(int i = 0; i < line.length(); i++){
				if(!line[i].isLetterOrNumber() && line[i] != '/' && line[i] != '.'){
					nonAlphaSlash = i;
					break;
				}
			}

			if(nonAlphaSlash != -1){
				line.remove(nonAlphaSlash, line.length() - nonAlphaSlash);
			}

			if(hasOutputLine){
				line = QDir::cleanPath(progsSrcInfo.path() + "/" + line);
				qDebug() << "Source file:" << line;
				files.push_back(line);
			}
			else{
				outputFile = QDir::cleanPath(buildOutPath + "/" + line);
				qDebug() << "Output file:" << outputFile;
				hasOutputLine = true;
			}
		}

		if(!hasOutputLine){
			qDebug() << "No output file in" << progsSrcPath;
			allCompiled = false;
			emit progsSrcCompiled(progsSrcPath, false);
			continue;
		}

		auto outputFileStr = outputFile.toStdString();

		OPTS_OPTION_STR(OPTION_OUTPUT) = outputFileStr.c_str();

		auto ftepp = ftepp_create();
		auto parser = parser_create();

		bool hasError = false;

		for(const auto &path : files){
			QFileInfo pathInfo(path);

			auto fileName = pathInfo.fileName().toStdString();
			auto filePath = path.toStdString();

			if(!ftepp_preprocess_file(ftepp, filePath.c_str())){
				hasError = true;
				qDebug() << "Could not preprocess file" << path;
				break;
			}

			auto data = ftepp_get(ftepp);
			if(vec_size(data)){
				if(!parser_compile_string(parser, fileName.c_str(), data, vec_size(data))){
					hasError = true;
					qDebug() << "Could not parse file" << path;
					break;
				}
			}

			ftepp_flush(ftepp);
		}

		if(hasError){
			parser_finish(parser, nullptr);
			ftepp_finish(ftepp);
			allCompiled = false;
			emit progsSrcCompiled(progsSrcPath, false);
			continue;
		}

		bool success = parser_finish(parser, outputFileStr.c_str());

		ftepp_finish(ftepp);

		if(!success){
			qDebug() << "Error compiling" << outputFile;
			allCompiled = false;
		}

		emit progsSrcCompiled(progsSrcPath, success);
	}

	emit compileFinished(allCompiled);
}
