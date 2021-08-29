find_package(Qt5 COMPONENTS Widgets REQUIRED)

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
	QCHighlighter.hpp
	QCHighlighter.cpp
	QCEdit.hpp
	QCEdit.cpp
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

target_link_libraries(qide fmt utf8::cpp gmqcclib Qt5::Widgets)
