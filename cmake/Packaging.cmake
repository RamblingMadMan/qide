set(CPACK_PACKAGE_VENDOR "Hamsmith")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "Hamsmith <contact@hamsmith.dev>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")

# set human names to exetuables
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "QIDE")
set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}")
set(CPACK_STRIP_FILES TRUE) 

set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/BundleConfig.cmake")

add_custom_target(
	bundle
	COMMAND ${CMAKE_CPACK_COMMAND} "--config" "${CMAKE_BINARY_DIR}/BundleConfig.cmake"
	COMMENT "Running CPACK. Please wait..."
	DEPENDS ${PROJECT_NAME}
)

set(CPACK_GENERATOR)

# Qt IFW packaging framework
if(BINARYCREATOR_EXECUTABLE)
    list(APPEND CPACK_GENERATOR IFW)
    message(STATUS "   + Qt Installer Framework               YES ")
else()
    message(STATUS "   + Qt Installer Framework                NO ")
endif()

message(STATUS "   + AppImage                             YES ")
if(CMAKE_VERSION VERSION_LESS 3.13)
	linuxdeployqt("${CPACK_PACKAGE_DIRECTORY}/_CPack_Packages/Linux/AppImage" "share/applications/example.desktop")
else()
	set(CPACK_GENERATOR "External;${CPACK_GENERATOR}")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CPackLinuxDeployQt.cmake.in "${CMAKE_BINARY_DIR}/CPackExternal.cmake")
	set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_BINARY_DIR}/CPackExternal.cmake")
endif()
