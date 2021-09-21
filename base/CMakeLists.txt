set(
	QIDE_BASE_SOURCES
	shapes.cpp
	Camera.cpp
	shaders.hpp
	Renderer.cpp
)

add_library(qide-base STATIC ${QIDE_HEADERS} ${QIDE_BASE_SOURCES})

set_target_properties(
	qide-base PROPERTIES
	WINDOWS_EXPORT_ALL_SYMBOLS ON
	CXX_STANDARD 20
)

target_include_directories(qide-base PUBLIC ${QIDE_INCLUDE_DIR})

target_link_libraries(qide-base PRIVATE fmt glbinding glbinding-aux)

target_link_libraries(qide-base PUBLIC glm)
