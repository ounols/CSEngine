set(PROJECT_NAME CSEngine)

################################################################################
# Source groups
################################################################################
#set(no_group_source_files
#    "../../../src/Sample/FirstDemoScene.cpp"
#    "../../../src/Sample/FirstDemoScene.h"
#    "CSEngine.cpp"
#    "CSEngine.h"
#)
#source_group("" FILES ${no_group_source_files})
#
#set(src__Manager
#    "../../../src/Manager/AssetMgr.cpp"
#    "../../../src/Manager/AssetMgr.h"
#    "../../../src/Manager/CameraMgr.cpp"
#    "../../../src/Manager/CameraMgr.h"
#    "../../../src/Manager/GameObjectContainer.cpp"
#    "../../../src/Manager/GameObjectContainer.h"
#    "../../../src/Manager/GameObjectMgr.cpp"
#    "../../../src/Manager/GameObjectMgr.h"
#    "../../../src/Manager/LightMgr.cpp"
#    "../../../src/Manager/LightMgr.h"
#    "../../../src/Manager/MainProc.cpp"
#    "../../../src/Manager/MainProc.h"
#    "../../../src/Manager/MemoryContainer.cpp"
#    "../../../src/Manager/MemoryContainer.h"
#    "../../../src/Manager/MemoryMgr.cpp"
#    "../../../src/Manager/MemoryMgr.h"
#    "../../../src/Manager/OGLMgr.cpp"
#    "../../../src/Manager/OGLMgr.h"
#    "../../../src/Manager/RenderContainer.cpp"
#    "../../../src/Manager/RenderContainer.h"
#    "../../../src/Manager/RenderMgr.cpp"
#    "../../../src/Manager/RenderMgr.h"
#    "../../../src/Manager/ResMgr.cpp"
#    "../../../src/Manager/ResMgr.h"
#    "../../../src/Manager/SceneMgr.cpp"
#    "../../../src/Manager/SceneMgr.h"
#    "../../../src/Manager/SCloneFactory.cpp"
#    "../../../src/Manager/SCloneFactory.h"
#    "../../../src/Manager/ScriptMgr.cpp"
#    "../../../src/Manager/ScriptMgr.h"
#)
#source_group("src\\Manager" FILES ${src__Manager})
#
#set(src__Manager__Base
#    "../../../src/Manager/Base/Scene.cpp"
#    "../../../src/Manager/Base/Scene.h"
#    "../../../src/Manager/Base/SContainer.h"
#)
#source_group("src\\Manager\\Base" FILES ${src__Manager__Base})
#
#set(src__Object
#    "../../../src/Object/SGameObject.cpp"
#    "../../../src/Object/SGameObject.h"
#    "../../../src/Object/SPrefab.cpp"
#    "../../../src/Object/SPrefab.h"
#    "../../../src/Object/SResource.cpp"
#    "../../../src/Object/SResource.h"
#    "../../../src/Object/SScene.cpp"
#    "../../../src/Object/SScene.h"
#    "../../../src/Object/SScriptObject.cpp"
#    "../../../src/Object/SScriptObject.h"
#    "../../../src/SObject.cpp"
#    "../../../src/SObject.h"
#)
#source_group("src\\Object" FILES ${src__Object})
#
#set(src__Util
#    "../../../src/MacroDef.h"
#    "../../../src/OGLDef.h"
#    "../../../src/Util/AssetsDef.h"
#    "../../../src/Util/CaptureDef.h"
#    "../../../src/Util/ComponentDef.h"
#    "../../../src/Util/GLProgramHandle.cpp"
#    "../../../src/Util/GLProgramHandle.h"
#    "../../../src/Util/Matrix.h"
#    "../../../src/Util/MatrixDef.h"
#    "../../../src/Util/MoreComponentFunc.cpp"
#    "../../../src/Util/MoreComponentFunc.h"
#    "../../../src/Util/MoreMath.h"
#    "../../../src/Util/MoreString.h"
#    "../../../src/Util/Quaternion.h"
#    "../../../src/Util/SafeLog.cpp"
#    "../../../src/Util/SafeLog.h"
#    "../../../src/Util/ScriptDef.h"
#    "../../../src/Util/VariableBinder.h"
#    "../../../src/Util/Vector.h"
#)
#source_group("src\\Util" FILES ${src__Util})
#
#set(src__Util__Animation
#    "../../../src/Util/Animation/Animation.cpp"
#    "../../../src/Util/Animation/Animation.h"
#    "../../../src/Util/Animation/AnimationUtill.h"
#    "../../../src/Util/Animation/Joint.h"
#    "../../../src/Util/Animation/Skeleton.h"
#    "../../../src/Util/Loader/DAE/DAEUtil/SkinningData.h"
#    "../../../src/Util/Loader/DAE/DAEUtil/Vertex.h"
#    "../../../src/Util/Loader/DAE/DAEUtil/VertexSkinData.h"
#)
#source_group("src\\Util\\Animation" FILES ${src__Util__Animation})
#
#set(src__Util__Interface
#    "../../../src/Util/Interface/TransformInterface.h"
#)
#source_group("src\\Util\\Interface" FILES ${src__Util__Interface})
#
#set(src__Util__Loader
#    "../../../src/Util/Loader/DAE/DAEAnimationLoader.cpp"
#    "../../../src/Util/Loader/DAE/DAEAnimationLoader.h"
#    "../../../src/Util/Loader/DAE/DAELoader.cpp"
#    "../../../src/Util/Loader/DAE/DAELoader.h"
#    "../../../src/Util/Loader/DAE/DAEUtil/DAEConvertSGameObject.cpp"
#    "../../../src/Util/Loader/DAE/DAEUtil/DAEConvertSGameObject.h"
#    "../../../src/Util/Loader/SCENE/SSceneLoader.cpp"
#    "../../../src/Util/Loader/SCENE/SSceneLoader.h"
#    "../../../src/Util/Loader/STB/stb_image.h"
#    "../../../src/Util/Loader/STB/stb_image_write.h"
#    "../../../src/Util/Loader/XML/XML.cpp"
#    "../../../src/Util/Loader/XML/XML.h"
#)
#source_group("src\\Util\\Loader" FILES ${src__Util__Loader})
#
#set(src__Util__Render
#    "../../../src/Util/Render/LightHelper.h"
#    "../../../src/Util/Render/MeshSurface.cpp"
#    "../../../src/Util/Render/MeshSurface.h"
#    "../../../src/Util/Render/PBRShaderLoader.cpp"
#    "../../../src/Util/Render/PBRShaderLoader.h"
#    "../../../src/Util/Render/RenderInterfaces.h"
#    "../../../src/Util/Render/SCubeTexture.cpp"
#    "../../../src/Util/Render/SCubeTexture.h"
#    "../../../src/Util/Render/ShaderUtil.cpp"
#    "../../../src/Util/Render/ShaderUtil.h"
#    "../../../src/Util/Render/SkyboxUtil.h"
#    "../../../src/Util/Render/SMaterial.cpp"
#    "../../../src/Util/Render/SMaterial.h"
#    "../../../src/Util/Render/STexture.cpp"
#    "../../../src/Util/Render/STexture.h"
#)
#source_group("src\\Util\\Render" FILES ${src__Util__Render})
#
#set(src__component
#    "../../../src/Component/Animation/AnimatorComponent.cpp"
#    "../../../src/Component/Animation/AnimatorComponent.h"
#    "../../../src/Component/Animation/JointComponent.cpp"
#    "../../../src/Component/Animation/JointComponent.h"
#    "../../../src/Component/CameraComponent.cpp"
#    "../../../src/Component/CameraComponent.h"
#    "../../../src/Component/CustomComponent.cpp"
#    "../../../src/Component/CustomComponent.h"
#    "../../../src/Component/DrawableSkinnedMeshComponent.cpp"
#    "../../../src/Component/DrawableSkinnedMeshComponent.h"
#    "../../../src/Component/DrawableStaticMeshComponent.cpp"
#    "../../../src/Component/DrawableStaticMeshComponent.h"
#    "../../../src/Component/LightComponent.cpp"
#    "../../../src/Component/LightComponent.h"
#    "../../../src/Component/MaterialComponent.cpp"
#    "../../../src/Component/MaterialComponent.h"
#    "../../../src/Component/RenderComponent.cpp"
#    "../../../src/Component/RenderComponent.h"
#    "../../../src/Component/SComponent.h"
#    "../../../src/Component/SISComponent.h"
#    "../../../src/Component/TransformComponent.cpp"
#    "../../../src/Component/TransformComponent.h"
#)
#source_group("src\\component" FILES ${src__component})
#
#set(ALL_FILES
#    ${no_group_source_files}
#    ${src__Manager}
#    ${src__Manager__Base}
#    ${src__Object}
#    ${src__Util}
#    ${src__Util__Animation}
#    ${src__Util__Interface}
#    ${src__Util__Loader}
#    ${src__Util__Render}
#    ${src__component}
#)

file(GLOB CSENGINE_SRC
        "../../../src/*.*"
        "../../../src/*/*.*"
        "../../../src/*/*/*.*"
        "../../../src/*/*/*/*.*"
        "../../../src/*/*/*/*/*.*"
        "../../../src/*/*/*/*/*/*.*"
#        "../../../src/*/*/*/*/*/*/*.*"
        )

link_directories(${PROJECT_NAME} "../../../External/glew/lib/Release/Win32")
link_directories(${PROJECT_NAME} "../../../External/Squirrel/lib/Windows")

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} CSEngine.cpp CSEngine.h ${CSENGINE_SRC})

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
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../External/Squirrel/include"
        ">"
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
            "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../External/glew/include"
            ">"
            )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
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
            /SUBSYSTEM:WINDOWS
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
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "../../../External/glew/lib/Release/Win32/glew32;"
            "../../../External/Squirrel/lib/Windows/squirrelD;"
            "../../../External/Squirrel/lib/Windows/sqstdlibD;"
            "kernel32;"
            "user32;"
            "gdi32;"
            "winspool;"
            "comdlg32;"
            "advapi32;"
            "shell32;"
            "ole32;"
            "oleaut32;"
            "uuid;"
            "odbc32;"
            "odbccp32"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "glew32;"
            "squirrelD;"
            "sqstdlibD;"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} "${ADDITIONAL_LIBRARY_DEPENDENCIES}")