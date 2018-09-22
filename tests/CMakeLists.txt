
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

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_path "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Find source files
get_files_recursive(headers "${include_path}" ".*\\\\.h$|\\\\.hpp$")
get_files_recursive(sources "${source_path}"  ".*\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$")


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

# Set platform abstraction path include path if necessary
if (NOT UNIX)
    set(PLATFORM_ABSTRACTION_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../extra/platform_abstraction/include)
endif()

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_BINARY_DIR}/include
    ${PLATFORM_ABSTRACTION_INCLUDE}
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${PROJECT_LIB}
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
