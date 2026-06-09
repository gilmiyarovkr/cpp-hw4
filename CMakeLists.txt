cmake_minimum_required(VERSION 3.14)

#----------------------------------------------------------------------------
# Project
project(print_ip
        VERSION 0.0
        LANGUAGES C CXX)

# Формирование версии проекта
set(PATCH_VERSION "1" CACHE STRING "Patch version")
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PATCH_VERSION})

#----------------------------------------------------------------------------
# Опции сборки
option(WITH_GOOGLE_TEST "Build GoogleTest tests" ON)
option(ENABLE_CCACHE "Enable CCACHE for building" OFF)
option(BUILD_DOCS "Enable docs generation" ON)

if(WITH_GOOGLE_TEST)
    enable_testing()
endif()

# Настройка путей для модулей
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Флаги компилятора
include(CompilerFlags)
# Использование ccache для ускорения компиляции
if (ENABLE_CCACHE)
    include(enable-ccache)
else()
    message(STATUS "Ccache is disabled")
endif()

#----------------------------------------------------------------------------
# Подключаем модуль Doxygen
include(cmake/Doxygen.cmake)

#----------------------------------------------------------------------------
# Подключаем папки с исходным кодом и тестами
add_subdirectory(library)
add_subdirectory(app)
if(WITH_GOOGLE_TEST)
    add_subdirectory(tests)
endif()

#----------------------------------------------------------------------------
# Подключение модуля упаковки в DEB
include(Packaging)
