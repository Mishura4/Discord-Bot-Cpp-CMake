cmake_minimum_required(VERSION 3.16)

set(BUILD_SHARED_LIBRARIES OFF)

set(BOT_NAME ExampleBot)

# define project
project(${BOT_NAME} LANGUAGES CXX)

add_executable(${BOT_NAME})

set_target_properties(${BOT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(${BOT_NAME} PUBLIC cxx_std_20)

# define sources
set(BOT_SOURCES
    src/main.cpp
    src/commands/commands.h
    src/commands/command_handler.cpp
    #src/commands/ping.cpp
)

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

list(TRANSFORM BOT_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/)

source_group(
    TREE
        ${CMAKE_CURRENT_LIST_DIR}/src/
    PREFIX
        "Source Files"
    FILES
        ${BOT_SOURCES}
)

target_include_directories(${BOT_NAME} PRIVATE src/)

target_sources(${BOT_NAME} PRIVATE ${BOT_SOURCES} "src/commands/ping.cpp" "src/commands/emoji.cpp")

# dependencies
add_subdirectory(dep)

target_link_libraries(${BOT_NAME} PUBLIC dpp)

if(MSVC)
    target_compile_options(${BOT_NAME} PRIVATE /W3)

    if (NOT WINDOWS_32_BIT)
        set(DPP_DEP_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dep/DPP/win32/bin")
    else()
        set(DPP_DEP_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dep/DPP/win32/32")
    endif()

    set(BOT_DEPENDENCIES
            "${DPP_DEP_DIRECTORY}/libcrypto-1_1-x64.dll"
            "${DPP_DEP_DIRECTORY}/libsodium.dll"
            "${DPP_DEP_DIRECTORY}/libssl-1_1-x64.dll"
            "${DPP_DEP_DIRECTORY}/opus.dll"
            "${DPP_DEP_DIRECTORY}/zlib1.dll"
            $<TARGET_FILE:dpp>)

    add_custom_target(COPY_DEPENDENCIES ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
	        ${BOT_DEPENDENCIES}
	        $<TARGET_FILE_DIR:${BOT_NAME}>
    )
else()
  target_compile_options(${BOT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS ${BOT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)