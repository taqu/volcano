cmake_minimum_required(VERSION 3.21)

include("CMakeCommon.txt")
set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(PROJECT_NAME test)
project(${PROJECT_NAME})

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR}/../crater)

set(VOLCANORT_BACKEND_CPU 1)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../volcanort/volcanort_conf.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/../volcanort/volcanort_conf.h" NEWLINE_STYLE UNIX)

########################################################################
# Sources
set(CRATER_HEADER "../lava/lava.h")
set(CRATER_SOURCE "../lava/lava.c")

set(LAVA_HEADER "../crater/crater.h")
set(LAVA_SOURCE "../crater/crater.c")

set(VOLCANORT_HEADER "../volcanort/volcanort.h;../volcanort/volcanort_conf.h")
set(VOLCANORT_SOURCE "../volcanort/volcanort.cpp")

set(TEST_HEADER "Window.h")
set(TEST_SOURCE "main.cpp;Window.cpp")

#if(DEFINED VOLCANORT_BACKEND_CPU)
#    set(VOLCANORT_CPU_HEADER "../volcanort/cpu/volcanort_cpu.h")
#    set(VOLCANORT_CPU_SOURCE "../volcanort/cpu/volcanort_cpu.cpp")
#    source_group("include/volcano/cpu" FILES ${VOLCANORT_CPU_HEADER})
#    source_group("src/volcano/cpu" FILES ${VOLCANORT_CPU_SOURCE})
#endif()

set(HEADERS "${TEST_HEADER};${CRATER_HEADER};${LAVA_HEADER};${VOLCANO_HEADER};${VOLCANORT_HEADER}")
set(SOURCES "${TEST_SOURCE};${CRATER_SOURCE};${LAVA_SOURCE};${VOLCANO_SOURCE};${VOLCANORT_SOURCE}")

source_group("include/crater" FILES ${CRATER_HEADER})
source_group("src/crater" FILES ${CRATER_SOURCE})
source_group("include/lava" FILES ${LAVA_HEADER})
source_group("src/lava" FILES ${LAVA_SOURCE})
source_group("include/volcano" FILES ${VOLCANO_HEADER})
source_group("src/volcano" FILES ${VOLCANO_SOURCE})
source_group("include/volcanort" FILES ${VOLCANORT_HEADER})
source_group("src/volcanort" FILES ${VOLCANORT_SOURCE})
source_group("include" FILES ${TEST_HEADER})
source_group("src" FILES ${TEST_SOURCE})

set(FILES ${HEADERS} ${SOURCES})

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_executable(${PROJECT_NAME} ${FILES})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(ISPC_SRC "https://github.com/ispc/ispc/releases/download/v1.18.0/ispc-v1.18.0-windows.zip")
    set(ISPC_DST ${CMAKE_CURRENT_SOURCE_DIR}/tools/ispc.zip)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(ISPC_SRC "https://github.com/ispc/ispc/releases/download/v1.18.0/ispc-v1.18.0-linux.tar.gz")
    set(ISPC_DST ${CMAKE_CURRENT_SOURCE_DIR}/tools/ispc.tar.gz)
endif()
if(NOT EXISTS ${ISPC_DST})
    file(DOWNLOAD ${ISPC_SRC} ${ISPC_DST} SHOW_PROGRESS)
endif()

if(MSVC)
    #file(DOWNLOAD <url> [<file>] [...])
    set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_UNICODE /DUNICODE /W4 /WX- /nologo /fp:precise /arch:AVX /Zc:wchar_t /TP /Gd /std:c++17 /std:c11 /DLG3_GFX_USE_WIN32 /DVK_USE_PLATFORM_WIN32_KHR")
    if(MSVC_VERSION VERSION_LESS_EQUAL "1900")
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /Zc:__cplusplus /std:c++latest")
    else()
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /Zc:__cplusplus /std:c++17")
    endif()
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm- /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Oi /GL /GR- /DNDEBUG /EHsc-")

elseif(UNIX)
    set(DEFAULT_CXX_FLAGS "-Wall -O2 -std=c++17 -std=gnu++17 -std=c99 -march=x86-64-v3 -fno-exceptions -DVK_USE_PLATFORM_WIN32_KHR")
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
elseif(APPLE)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME_DEBUG "${PROJECT_NAME}" OUTPUT_NAME_RELEASE "${PROJECT_NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

