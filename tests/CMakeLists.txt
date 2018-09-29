
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
set(headers "")
get_files_recursive(headers "${include_path}" ".*\\\\.h$|\\\\.hpp$")
set(sources "")
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
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_BINARY_DIR}/include
)

target_include_directories(${target}
    SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Set compile options
target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


# 
# Libraries
# 

if (NOT WIRINGPI_LIB)
    set(WIRINGPI_LIB "")
endif()

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${PROJECT_LIB}
    ${WIRINGPI_LIB}
)

