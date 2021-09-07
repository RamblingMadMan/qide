find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)
find_package(LibZip REQUIRED)

set(
	QIDE_RESOURCES
	id1.qrc
	icon.qrc
	fonts.qrc
)

set(
	QIDE_SOURCES
	main.cpp
	QCType.hpp
	QCToken.hpp
	QCLexer.hpp
	QCLexer.cpp
	QCExpr.hpp
	QCParser.hpp
	QCParser.cpp
	QCByteCode.hpp
	QCByteCode.cpp
	QCHighlighter.hpp
	QCHighlighter.cpp
	QCCompleter.hpp
	QCCompleter.cpp
	QCMap.hpp
	QCMap.cpp
	QCEdit.hpp
	QCEdit.cpp
	QideEditor.hpp
	QideEditor.cpp
	QideMapEditor.hpp
	QideMapEditor.cpp
	QideCompiler.hpp
	QideCompiler.cpp
	QCVM.hpp
	QCVM.cpp
	QideGame.hpp
	QideGame.cpp
	QideSetup.hpp
	QideSetup.cpp
	QideWindow.hpp
	QideWindow.cpp
	${QIDE_RESOURCES}
)

add_executable(qide ${QIDE_SOURCES})

set_target_properties(
	qide PROPERTIES
	CXX_STANDARD 20
	STANDARD_REQUIRED ON
	AUTOMOC ON
	AUTORCC ON
	AUTOUIC ON
)

target_include_directories(
	qide PUBLIC
	${LIBZIP_INCLUDE_DIR}
)

target_link_libraries(
	qide
	fmt rapidfuzz::rapidfuzz
	${LIBZIP_LIBRARY}
	gmqcclib
	Qt5::Core Qt5::Widgets Qt5::Network
)
