
# 
# External dependencies
# 

# 
# Executable name and options
# 

# Target name
set(target "test${META_PROJECT_NAME}")
message(STATUS "Test ${META_PROJECT_NAME}")


# 
# Sources
# 

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(headers "")

get_dir_files(include_files ${include_dir})
foreach(include_file ${include_files})
    list(APPEND headers "${include_dir}/${include_file}")
endforeach()

set(sources "")

get_dir_files(source_files ${source_dir})
foreach(source_file ${source_files})
    list(APPEND sources "${source_dir}/${source_file}")
endforeach()


# 
# Create executable
# 

# Build executable
add_executable(${target}
    ${sources}
    ${headers}
)


# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_BINARY_DIR}/include
)


# 
# Libraries
# 

if (UNIX)
    set(WIRING_PI_LIB "wiringPi")
endif()

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${PROJECT_LIB}
    ${WIRING_PI_LIB}
)


# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
