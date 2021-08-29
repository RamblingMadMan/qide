#include <filesystem>

namespace fs = std::filesystem;

#include <QtWidgets>
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTreeView>

#include "fmt/format.h"

#include "QCEdit.hpp"

inline std::vector<fs::path> getSourceFiles(const fs::path &root){
	std::vector<fs::path> paths;

	for(const auto &entry : fs::recursive_directory_iterator(root)){
		if(entry.is_directory()) continue;

		const auto p = entry.path();

		if(p.extension() == ".qc" || p.extension() == ".h"){
			paths.emplace_back(p);
		}
	}

	return paths;
}

int main(int argc, char *argv[]){
	fmt::print(
		"-----------\n"
		"QIDE v0.0.1\n"
		"-----------\n"
	);

	auto workDir = fs::current_path();
	auto srcDir = workDir / "src";

	fmt::print("Working dir: {}\n", workDir.c_str());

	auto sources = getSourceFiles(srcDir);

	QApplication qapp(argc, argv);
	qapp.setApplicationName("QIDE");
	qapp.setApplicationDisplayName("QIDE");
	qapp.setApplicationVersion("0.0.1");

	QFileSystemModel fsModel;
	fsModel.setRootPath("");

	QModelIndex fsModelIdx = fsModel.index(srcDir.c_str());

	QWidget window;
	window.resize(1280, 720);
	window.show();
	window.setWindowTitle("QIDE");

	QVBoxLayout vbox(&window);

	QSplitter splitter;

	QTreeView treeView;
	treeView.setModel(&fsModel);
	treeView.setRootIndex(fsModelIdx);
	//treeView.show();

	QCEdit qcEdit;

	auto srcPath = srcDir / "defs.qc";

	qcEdit.loadFile(QDir(srcPath.c_str()));

	splitter.addWidget(&treeView);
	splitter.addWidget(&qcEdit);

	vbox.addWidget(&splitter);

	return qapp.exec();
}

