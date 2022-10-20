set(PROJECT_NAME CSEditor)

################################################################################
# Source groups
################################################################################

file(GLOB CSENGINE_SRC
        "../../../../src/*.*"
        "../../../../src/*/*.*"
        "../../../../src/*/*/*.*"
        "../../../../src/*/*/*/*.*"
        "../../../../src/*/*/*/*/*.*"
        "../../../../src/*/*/*/*/*/*.*"
#        "../../../../src/*/*/*/*/*/*/*.*"
        )

file(GLOB CSEDITOR_SRC
        "../../../src/*.*"
        "../../../src/*/*.*"
        "../../../src/*/*/*.*"
        "../../../src/*/*/*/*.*"
        "../../../src/*/*/*/*/*.*"
        "../../../src/*/*/*/*/*/*.*"
        "../../../src/*/*/*/*/*/*/*.*"
        )

file(GLOB IMGUI_SRC
        "../../../../External/imgui/*.*"
        )

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    link_directories(${PROJECT_NAME} "../../../../External/glew/lib/Win32")
    link_directories(${PROJECT_NAME} "../../../../External/Squirrel/lib/Windows/Win32")
    link_directories(${PROJECT_NAME} "../../../../External/glfw/lib/Win32/lib-vc2022")
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    link_directories(${PROJECT_NAME} "../../../../External/glew/lib/Win64")
    link_directories(${PROJECT_NAME} "../../../../External/Squirrel/lib/Windows/Win64")
    link_directories(${PROJECT_NAME} "../../../../External/glfw/lib/Win64/lib-vc2022")
endif()

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} main.cpp ${CSENGINE_SRC} ${CSEDITOR_SRC} ${IMGUI_SRC})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE CSEngine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
        MultiThreadedDebug
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/Squirrel/include"
        ">"
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/glew/include"
            ">"
            )
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/imgui"
            ">"
            )
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/glfw/include"
            ">"
            )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/Squirrel/include"
            ">"
            )
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/glew/include"
            ">"
            )
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/imgui"
            ">"
            )
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../External/glfw/include"
            ">"
            )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            "$<$<CONFIG:Debug>:"
                "_DEBUG;"
                "_CRT_SECURE_NO_WARNINGS;"
                "_MBCS"
            ">"
            "$<$<CONFIG:Release>:"
                "NDEBUG;"
                "UNICODE;"
                "_UNICODE"
            ">"
            "_WINDOWS"
            )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "WIN32;"
        "_WINDOWS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "glew32;"
            "squirrelD;"
            "sqstdlibD;"
            "glfw3_mt;"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "glew32;"
            "squirrelD;"
            "sqstdlibD;"
            "glfw3_mt;"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} "${ADDITIONAL_LIBRARY_DEPENDENCIES}")