#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QFile>

#include "gmqcc.h"
#include "gmqcc/parser.h"

// required by gmqcc (^:',
const oper_info *operators = nullptr;
size_t operator_count = 0;

#include "QideCompiler.hpp"

QideCompiler::QideCompiler(QObject *parent)
	: QObject(parent)
	, m_watcher(new QFileSystemWatcher(this))
{
	auto buildDir = QSettings().value("buildDir").toString();
	auto projDir = QSettings().value("projDir").toString();

	m_watcher->addPath(projDir);

	connect(
		m_watcher, &QFileSystemWatcher::fileChanged,
		this, [buildDir, this](const QString &path){
			QFileInfo fileInfo(path);
			if(fileInfo.exists() && fileInfo.isFile()){
				QDir fileDir(path);
				if(QDir(path).dirName() == "progs.src"){
					m_progsSrcPath = path;
					setCanCompile(true);
				}
			}
			else if(path == m_progsSrcPath){
				m_progsSrcPath.clear();
				setCanCompile(false);
			}
		}
	);

	con_init();
	opts_init("progs.dat", COMPILER_QCC, (1024 << 3));

	util_seed(time(0));

	operators = qcc_operators;
	operator_count = GMQCC_ARRAY_COUNT(qcc_operators);
}

QideCompiler::~QideCompiler()
{
	con_close();
}

void QideCompiler::compile()
{
	if(!m_canCompile) return;

	auto file = QFile(m_progsSrcPath);
	if(!file.open(QFile::ReadOnly | QFile::Text)){
		setCanCompile(false);
		return;
	}

	emit compileStarted();

	QVector<QString> files;

	files.reserve(16);

	auto buildDir = QSettings().value("buildDir").toString();
	auto srcPath = QDir(m_progsSrcPath).rootPath();
	auto outputFile = (QSettings().value("buildDir").toString() + "/progs.dat").toStdString();

	OPTS_OPTION_BOOL(OPTION_DEBUG) = true;
	OPTS_OPTION_BOOL(OPTION_G) = true;
	OPTS_OPTION_U32(OPTION_O) = 2;
	OPTS_OPTION_STR(OPTION_OUTPUT) = outputFile.c_str();

	opts_setoptimlevel(2);
	opts_setflag("LNO", true);

	while(file.canReadLine()){
		auto line = file.readLine();
		if(!line[0] || (line[0] == '/' && line[1] == '/')){
			continue;
		}

		files.push_back(srcPath + "/" + line);
	}

	auto ftepp = ftepp_create();
	auto parser = parser_create();

	for(const auto &path : files){
		auto fileName = QDir(path).dirName().toStdString();
		auto filePath = path.toStdString();
		if(!ftepp_preprocess_file(ftepp, filePath.c_str())){
			parser_finish(parser, nullptr);
			ftepp_finish(ftepp);
			emit compileFinished(false);
			return;
		}

		auto data = ftepp_get(ftepp);
		if(vec_size(data)){
			if(!parser_compile_string(parser, fileName.c_str(), data, vec_size(data))){
				parser_finish(parser, nullptr);
				ftepp_finish(ftepp);
				emit compileFinished(false);
				return;
			}
		}

		ftepp_flush(ftepp);
	}

	bool success = parser_finish(parser, outputFile.c_str());

	ftepp_finish(ftepp);

	emit compileFinished(success);
}

void QideCompiler::setCanCompile(bool canCompile_)
{
	if(m_canCompile == canCompile_) return;

	m_canCompile = canCompile_;
	emit canCompileChanged();
}
