if(WIN32 AND MINGW)
    # make deployment with mingw easier
	set(Qt5_USE_STATIC_LIBS ON)
	set(Qt5_USE_STATIC_RUNTIME ON)
else()
	find_package(OpenSSL) # this doesn't work with MinGW cross compiles
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
	QideProjectWizard.hpp
	QideProjectWizard.cpp
	QideFileWizard.hpp
	QideFileWizard.cpp
	QideWindow.hpp
	QideWindow.cpp
	${QIDE_RESOURCES}
)

add_executable(qide ${QIDE_SOURCES})

set_target_properties(
	qide PROPERTIES
	CXX_STANDARD 17
	STANDARD_REQUIRED ON
	AUTOMOC ON
	AUTORCC ON
	AUTOUIC ON
)

target_include_directories(
	qide PRIVATE
	${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(
	qide
	qide-base
	fmt rapidfuzz::rapidfuzz
	#${LIBZIP_LIBRARY}
	gmqcclib
	${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY}
	Qt5::Core Qt5::Widgets Qt5::Network
)

if(WIN32)
	target_link_libraries(
		qide
		dwmapi
	)

    #add_custom_command(TARGET qide POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:qide> $<TARGET_FILE_DIR:qide>
	#	COMMAND_EXPAND_LISTS
	#)
endif()
