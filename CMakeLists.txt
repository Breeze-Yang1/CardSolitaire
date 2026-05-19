cmake_minimum_required(VERSION 3.6)

set(APP_NAME CardSolitaire)
project(${APP_NAME})

if(NOT DEFINED COCOS2DX_ROOT)
    if(DEFINED ENV{COCOS2DX_ROOT})
        set(COCOS2DX_ROOT $ENV{COCOS2DX_ROOT})
    else()
        message(FATAL_ERROR "Please set COCOS2DX_ROOT to a Cocos2d-x 3.17.x directory.")
    endif()
endif()

set(COCOS2DX_ROOT ${COCOS2DX_ROOT} CACHE PATH "Cocos2d-x root directory")
set(COCOS2DX_ROOT_PATH ${COCOS2DX_ROOT} CACHE PATH "Cocos2d-x root path used by Cocos2d-x 3.x cmake")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type for Cocos2d-x external libraries" FORCE)
endif()
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT}/cmake/Modules/)

include(CocosBuildSet)
add_subdirectory(${COCOS2DX_ROOT}/cocos ${CMAKE_BINARY_DIR}/cocos)

set(GAME_SOURCE
    proj.win32/main.cpp
    Classes/AppDelegate.cpp
    Classes/CardSprite.cpp
    Classes/CardTypes.cpp
    Classes/GameModel.cpp
    Classes/GameScene.cpp
    Classes/LevelCatalog.cpp
    Classes/LevelConfig.cpp
    Classes/LevelSelectScene.cpp
)

set(GAME_HEADER
    Classes/AppDelegate.h
    Classes/CardSprite.h
    Classes/CardTypes.h
    Classes/GameModel.h
    Classes/GameScene.h
    Classes/LevelCatalog.h
    Classes/LevelConfig.h
    Classes/LevelSelectScene.h
)

add_executable(${APP_NAME} WIN32 ${GAME_SOURCE} ${GAME_HEADER})
target_include_directories(${APP_NAME} PRIVATE Classes)
target_link_libraries(${APP_NAME} cocos2d)

set_target_properties(${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(GLOB COCOS_WIN32_RUNTIME_DLLS
    "${COCOS2DX_ROOT}/external/curl/prebuilt/win32/*.dll"
    "${COCOS2DX_ROOT}/external/lua/luajit/prebuilt/win32/*.dll"
    "${COCOS2DX_ROOT}/external/openssl/prebuilt/win32/*.dll"
    "${COCOS2DX_ROOT}/external/sqlite3/libraries/win32/*.dll"
    "${COCOS2DX_ROOT}/external/uv/prebuilt/win32/*.dll"
    "${COCOS2DX_ROOT}/external/websockets/prebuilt/win32/*.dll"
    "${COCOS2DX_ROOT}/external/win32-specific/*/prebuilt/*.dll"
)

foreach(COCOS_RUNTIME_DLL ${COCOS_WIN32_RUNTIME_DLLS})
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${COCOS_RUNTIME_DLL}"
        "$<TARGET_FILE_DIR:${APP_NAME}>"
    )
endforeach()

add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    "$<TARGET_FILE_DIR:${APP_NAME}>/Resources"
)
