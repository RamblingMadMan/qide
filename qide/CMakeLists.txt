if(NOT WIN32 OR NOT MINGW)
	find_package(OpenSSL REQUIRED) # this doesn't work with MinGW cross compiles
endif()

find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)
#find_package(LibZip REQUIRED)

set(
	QIDE_RESOURCES
	id1.qrc
	icon.qrc
	fonts.qrc
	templates.qrc
	shaders.qrc
)

if(WIN32)
	list(APPEND QIDE_RESOURCES QIDE.exe.manifest)
endif()

set(
	QIDE_SOURCES
	main.cpp
	QuakeColors.hpp
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
	QCProgs.hpp
	QCProgs.cpp
	QCVM.hpp
	QCVM.cpp
	QCMap.hpp
	QCMap.cpp
	QCEdit.hpp
	QCEdit.cpp
	QidePakExplorer.hpp
	QidePakExplorer.cpp
	QideEditor.hpp
	QideEditor.cpp
	QideMapEditor.hpp
	QideMapEditor.cpp
	QideCompiler.hpp
	QideCompiler.cpp
	QideGame.hpp
	QideGame.cpp
	QideSetup.hpp
	QideSetup.cpp
	QideProjectWizard.hpp
	QideProjectWizard.cpp
	QideFileWizard.hpp
	QideFileWizard.cpp
	QideSettings.hpp
	QideSettings.cpp
	QideWindow.hpp
	QideWindow.cpp
	${QIDE_RESOURCES}
)

add_executable(qide ${QIDE_SOURCES})

add_custom_command(
	TARGET qide
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:qide-quake>
		$<TARGET_FILE_DIR:qide>/games/$<TARGET_FILE_NAME:qide-quake>
)

set_target_properties(
	qide PROPERTIES
	CXX_STANDARD 17
	STANDARD_REQUIRED ON
	AUTOMOC ON
	AUTORCC ON
	AUTOUIC ON
)

check_cxx_compiler_flag("-pipe" HAVE_PIPE_FLAG)

if(HAVE_PIPE_FLAG)
	target_compile_options(qide PUBLIC "-pipe")
endif()

target_include_directories(
	qide PRIVATE
	${OPENSSL_INCLUDE_DIR}
	${PHYSFS_INCLUDE_DIRS}
)

target_link_libraries(
	qide
	qide-base
	fmt rapidfuzz::rapidfuzz
	physfs-static
	gmqcclib
	${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY}
	Qt5::Core Qt5::Widgets Qt5::Network
)

if(WIN32)
	target_link_libraries(
		qide
		dwmapi
	)
endif()
