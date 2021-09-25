#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

#include "gmqcc.h"
#include "gmqcc/parser.h"

// required by gmqcc (^:',
// might try cobbling something together with fteqcc...
const oper_info *operators = nullptr;
size_t operator_count = 0;

#include "QCLexer.hpp"
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

namespace {
	using TokIter = QVector<QCToken>::ConstIterator;

	inline QStringList parseProgsSrc(TokIter beg, TokIter end){
		QStringList files;

		files.reserve(8);

		auto lineBeg = beg;

		while(lineBeg != end){
			while(lineBeg != end && lineBeg->kind() == QCToken::Space){
				++lineBeg;
			}

			if(lineBeg == end) break;

			auto it = lineBeg;

			do{
				switch(it->kind()){
					case QCToken::Comment:
					case QCToken::NewLine:{
						auto fileBeg = lineBeg;
						auto fileEnd = it;
						lineBeg = ++it;

						if(std::distance(fileBeg, fileEnd) == 0){
							break;
						}

						QString fileName;

						auto fileIt = fileBeg;
						while(fileIt != fileEnd){
							fileName.append(fileIt->str().toString());
							++fileIt;
						}

						fileName = fileName.trimmed();

						if(fileName.isEmpty()){
							continue;
						}

						files.append(fileName);

						break;
					}

					default:{
						++it;
						break;
					}
				}
			} while(it > lineBeg && it != end);
		}

		return files;
	}
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

		auto progsSrcDir = progsSrcInfo.absoluteDir();
		auto progsSrcProjDir = progsSrcDir.path().mid(m_srcPath.length() + 1);

		qDebug() << "progs.src dir:" << progsSrcProjDir;

		auto buildOutPath = m_buildPath;
		if(!progsSrcProjDir.isEmpty()){
			buildOutPath += "/" + progsSrcProjDir;
		}

		qDebug() << "Build path:" << buildOutPath;

		opts_setoptimlevel(2);
		opts_setflag("LNO", true);

		QCLexer lexer;
		lexer.setSkipSpaces(false);
		lexer.setSkipNewLines(false);

		auto progsSrcContents = QString::fromUtf8(file.readAll());
		lexer.lex(progsSrcContents);

		auto &&toks = lexer.tokens();
		auto files = parseProgsSrc(toks.begin(), toks.end());

		if(files.empty()){
			qDebug() << "No files listed";
			emit progsSrcCompiled(progsSrcPath, false);
			continue;
		}

		QString outputFile = files[0];

		auto outputFileStr = outputFile.toStdString();

		files.erase(files.begin());

		OPTS_OPTION_STR(OPTION_OUTPUT) = outputFileStr.c_str();

		auto ftepp = ftepp_create();
		auto parser = parser_create();

		bool hasError = false;

		foreach(auto &&file, files){
			qDebug() << "Including file" << file;

			QFileInfo fileInfo(progsSrcDir.absolutePath() + "/" + file);

			auto filePath = fileInfo.absoluteFilePath();
			auto fileName = fileInfo.fileName();
			auto fileDir = fileInfo.dir();

			if(!fileInfo.exists()){
				qDebug() << "File" << filePath << "does not exist";

				const auto guesses = QVector<QString>{
					fileName.toLower(), fileName.toUpper()
				};

				QString existingPath;

				foreach(auto &&guess, guesses){
					auto guessDir = fileDir;
					auto guessPath = guessDir.absolutePath() + "/" + guess;
					qDebug() << "Guessing" << guessPath;
					QFileInfo guessInfo(guessPath);
					if(guessInfo.exists()){
						existingPath = guessPath;
						break;
					}
				}

				if(existingPath.isEmpty()){
					qDebug() << "Could not guess file path :^(";
					continue;
				}

				fileInfo.setFile(existingPath);

				filePath = fileInfo.absoluteFilePath();
				fileName = fileInfo.fileName();
			}

			auto filePathStr = filePath.toStdString();

			if(!ftepp_preprocess_file(ftepp, filePathStr.c_str())){
				qDebug() << "Could not preprocess file" << filePath;
				continue;
			}

			auto data = ftepp_get(ftepp);
			if(vec_size(data)){
				if(!parser_compile_string(parser, filePathStr.c_str(), data, vec_size(data))){
					hasError = true;
					qDebug() << "Could not parse file" << filePath;
					ftepp_flush(ftepp);
					continue;
				}
			}

			ftepp_flush(ftepp);
		}

		(void)hasError;

		/*
		if(hasError){
			//parser_finish(parser, nullptr);
			ftepp_finish(ftepp);
			allCompiled = false;
			emit progsSrcCompiled(progsSrcPath, false);
			continue;
		}
		*/

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
