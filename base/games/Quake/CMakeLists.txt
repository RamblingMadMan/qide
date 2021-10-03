set(
	QIDE_QUAKE_SOURCES
	Game.cpp
)

add_library(qide-quake MODULE ${QIDE_QUAKE_SOURCES})

set_target_properties(
	qide-quake PROPERTIES
	WINDOWS_EXPORT_ALL_SYMBOLS ON
	CXX_STANDARD 17
)

target_link_libraries(qide-quake qide-base)
