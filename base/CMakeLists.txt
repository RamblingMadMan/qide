set(
	QIDE_BASE_SOURCES
	shapes.cpp
	Camera.cpp
	shaders.hpp
	Renderer.cpp
)

add_library(qide-base SHARED ${QIDE_HEADERS} ${QIDE_BASE_SOURCES})

target_include_directories(qide-base PUBLIC ${QIDE_INCLUDE_DIR})

target_link_libraries(qide-base PRIVATE fmt glbinding glbinding-aux)
