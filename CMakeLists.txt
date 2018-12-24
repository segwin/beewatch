# CMakeList.txt : CMake project for beewatch, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.6)


# Include custom cmake modules
include(cmake/Custom.cmake)
include(cmake/GetDirContents.cmake)
include(cmake/GetGitRevisionDescription.cmake)


#
# Project description and (meta) information
#

# Get git revision
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)
if(NOT GIT_SHA1)
    set(GIT_REV "0")
endif()

# Meta information about the project
set(META_PROJECT_NAME        "beewatch")
set(META_PROJECT_DESCRIPTION "Keep an eye on your bees!")
set(META_AUTHOR_ORGANIZATION "Eric Seguin")
set(META_AUTHOR_DOMAIN       "https://segwin.ca")
set(META_AUTHOR_MAINTAINER   "eric@segwin.ca")
set(META_VERSION_MAJOR       "0")
set(META_VERSION_MINOR       "1")
set(META_VERSION_PATCH       "0")
set(META_VERSION_REVISION    "${GIT_REV}")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")
set(META_CMAKE_INIT_SHA      "${GIT_REV}")

string(MAKE_C_IDENTIFIER ${META_PROJECT_NAME} META_PROJECT_ID)
string(TOUPPER ${META_PROJECT_ID} META_PROJECT_ID)

set(project_exe "${META_PROJECT_NAME}")
set(project_lib "lib${META_PROJECT_NAME}")
set(project_web "web")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(project_exe "${project_exe}d")
    set(project_lib "${PROJECT_LIB}d")
endif()

# Build options
find_package(Doxygen)

if (${DOXYGEN_FOUND} STREQUAL "YES" AND ${DOXYGEN_DOT_FOUND} STREQUAL "YES")
    set(build_doc_default ON)
else()
    set(build_doc_default OFF)
endif()

option(BUILD_DOCUMENTATION "Create HTML documentation using Doxygen" ${build_doc_default})


#
# Declare project
#

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(IDE_FOLDER "")

# Declare project
project(${META_PROJECT_NAME} C CXX)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Create version file
file(WRITE "${PROJECT_BINARY_DIR}/VERSION" "${META_NAME_VERSION}")


#
# Compiler settings and options
#

include(cmake/CompileOptions.cmake)


#
# Deployment/installation setup
#

# Get project name
set(project ${META_PROJECT_NAME})

# Check for system dir install
set(SYSTEM_DIR_INSTALL FALSE)
if("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr" OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
    set(SYSTEM_DIR_INSTALL TRUE)
endif()

# Installation paths
if(UNIX AND SYSTEM_DIR_INSTALL)
    # Install into the system (/usr/bin or /usr/local/bin)
    set(INSTALL_ROOT      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_CMAKE     "share/${project}/cmake") # /usr/[local]/share/<project>/cmake
    set(INSTALL_EXAMPLES  "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_DATA      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_BIN       "bin")                    # /usr/[local]/bin
    set(INSTALL_SHARED    "lib")                    # /usr/[local]/lib
    set(INSTALL_LIB       "lib")                    # /usr/[local]/lib
    set(INSTALL_INCLUDE   "include")                # /usr/[local]/include
    set(INSTALL_DOC       "share/doc/${project}")   # /usr/[local]/share/doc/<project>
    set(INSTALL_SHORTCUTS "share/applications")     # /usr/[local]/share/applications
    set(INSTALL_ICONS     "share/pixmaps")          # /usr/[local]/share/pixmaps
    set(INSTALL_INIT      "/etc/init")              # /etc/init (upstart init scripts)
else()
    # Install into local directory
    set(INSTALL_ROOT      ".")                      # ./
    set(INSTALL_CMAKE     "cmake")                  # ./cmake
    set(INSTALL_EXAMPLES  ".")                      # ./
    set(INSTALL_DATA      ".")                      # ./
    set(INSTALL_BIN       ".")                      # ./
    set(INSTALL_SHARED    "lib")                    # ./lib
    set(INSTALL_LIB       "lib")                    # ./lib
    set(INSTALL_INCLUDE   "include")                # ./include
    set(INSTALL_DOC       "doc")                    # ./doc
    set(INSTALL_SHORTCUTS "misc")                   # ./misc
    set(INSTALL_ICONS     "misc")                   # ./misc
    set(INSTALL_INIT      "misc")                   # ./misc
endif()

# Set runtime path
set(CMAKE_SKIP_BUILD_RPATH            FALSE) # Add absolute path to all dependencies for BUILD
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE) # Use CMAKE_INSTALL_RPATH for INSTALL
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # Do NOT add path to dependencies for INSTALL

if(NOT SYSTEM_DIR_INSTALL)
    # Find libraries relative to binary
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@loader_path/../../../${INSTALL_LIB}")
    else()
        set(CMAKE_INSTALL_RPATH "$ORIGIN/${INSTALL_LIB}")
    endif()
endif()

# Check for external libraries
find_library(wiringpi_lib wiringPi)

if (wiringpi_lib)
    add_definitions(-DHAS_WIRINGPI)
endif()

set(cpprestsdk_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/)
find_package(cpprestsdk REQUIRED)

find_package(Boost REQUIRED COMPONENTS system filesystem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq")

set(PQXX /usr/local/include/pqxx)

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)


#
# Sources
#
set(global_include_path "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(dynamic_include_path "${CMAKE_CURRENT_BINARY_DIR}/include")
set(global_source_path "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Generate version-header
configure_file("${global_include_path}/version.h.in" "${dynamic_include_path}/version.h")

# Find source files
set(headers "")
set(dynamic_headers "")
set(sources "")

get_files_recursive(headers         "${global_include_path}"  "\\\\.h$|\\\\.hpp$")
get_files_recursive(dynamic_headers "${dynamic_include_path}" "\\\\.h$|\\\\.hpp$")
get_files_recursive(sources         "${global_source_path}"   "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$|")

# Include main.cpp separately from the rest
list(REMOVE_ITEM sources "${global_source_path}/main.cpp")

# Group source files
set(header_group         "Header Files (API)")
set(header_group_dynamic "Header Files (Generated)")
set(source_group         "Source Files")

source_group_by_path(${global_include_path}  "\\\\.h$|\\\\.hpp$"
    ${header_group} ${headers})
source_group_by_path(${dynamic_include_path} "\\\\.h$|\\\\.hpp$"
    ${header_group_dynamic} ${dynamic_headers})
source_group_by_path(${global_source_path}   "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
    ${source_group} ${sources})


#
# Web app
#

set(ng_opt_args "")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(ng_opt_args "--prod")
    message(STATUS "Enabling release optimisations for Angular build")
endif()

add_custom_target(${project_web}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/web
    COMMAND ng build --progress=true ${ng_opt_args}
)


#
# Library
#

add_library(${project_lib}
    ${sources}
    ${headers} ${dynamic_headers}
)

# Add include directories
target_include_directories(${project_lib}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${global_include_path}
    ${dynamic_include_path}

    SYSTEM PRIVATE
    ${Boost_INCLUDE_DIRS}
)

# Set compile options
target_compile_options(${project_lib}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

# Add web page dependency
#add_dependencies(${project_lib} ${project_web})


#
# Executable (only build on target platform)
#
if (UNIX AND wiringpi_lib)
    add_executable (${project_exe} ${global_source_path}/main.cpp)

    # Add include directories
    target_include_directories(${project_exe}
        PRIVATE
        ${DEFAULT_INCLUDE_DIRECTORIES}
        ${global_include_path}
        ${dynamic_include_path}

        SYSTEM PRIVATE
        ${Boost_INCLUDE_DIRS}
    )

    # Link external libraries
    target_link_libraries(${project_exe}
        PRIVATE
        ${DEFAULT_LIBRARIES}
        ${project_lib}
        ${wiringpi_lib}
        cpprestsdk::cpprest
        ${Boost_LIBRARIES}
        ${PQXX_LIB} ${PQ_LIB}
    )

	# Set compile options
	target_compile_options(${project_exe}
		PRIVATE
		${DEFAULT_COMPILE_OPTIONS}
	)
else()
    message(WARNING "Skipping executable: should only build on Linux with wiringPi library installed")
endif()


#
# Tests
#

# Test target
set(tests "test${META_PROJECT_NAME}")
message(STATUS "Test ${META_PROJECT_NAME}")

# Tests directories
set(tests_path         "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(tests_include_path "${tests_path}/include")
set(tests_source_path  "${tests_path}/source")
set(tests_lib_path     "${tests_path}/lib")

# Find source files
set(tests_headers "")
set(tests_sources "")
set(tests_libs    "")

get_files_recursive(tests_headers "${tests_include_path}" "\\\\.h$|\\\\.hpp$")
get_files_recursive(tests_sources "${tests_source_path}"  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$")
get_files_recursive(tests_libs    "${tests_lib_path}"     "\\\\.h$|\\\\.hpp$")

# Group source files
set(tests_header_group "Header Files")
set(tests_source_group "Source Files")

source_group_by_path(${tests_include_path}  "\\\\.h$|\\\\.hpp$"                   ${tests_header_group} ${tests_headers})
source_group_by_path(${tests_source_path}   "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" ${tests_source_group} ${tests_sources})

# Build executable
add_executable(${tests}
    ${tests_sources}
    ${tests_headers}
    ${tests_libs}
)

# Set include directories
target_include_directories(${tests}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${tests_include_path}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include

    SYSTEM PRIVATE
    ${tests_lib_path}
    ${Boost_INCLUDE_DIRS}
)

# Set compile options
target_compile_options(${tests}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
    ${DEFAULT_LINKER_OPTIONS}
)

set_target_properties(${tests} PROPERTIES LINK_FLAGS "${DEFAULT_LINKER_OPTIONS}")

# Link libraries
if (NOT wiringpi_lib)
    set(wiringpi_lib "")
endif()

target_link_libraries(${tests}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${project_lib}
    ${wiringpi_lib}
    ${Boost_LIBRARIES}
    ${PQXX_LIB} ${PQ_LIB}
)


#
# Documentation (doxygen)
#

if (BUILD_DOCUMENTATION)
    set(doxyfile_in "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
    set(doxyfile    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    configure_file(${doxyfile_in} ${doxyfile})

    set(doc_path "${CMAKE_CURRENT_SOURCE_DIR}/doc")

    add_custom_target(doc_directory
        COMMAND ${CMAKE_COMMAND} -E make_directory "${doc_path}")

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${doc_path}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    add_dependencies(doc doc_directory)
endif()



#
# Deployment (global project files)
#

# Install version file
install(FILES "${PROJECT_BINARY_DIR}/VERSION" DESTINATION ${INSTALL_ROOT} COMPONENT runtime)

# Install the project meta files
install(FILES AUTHORS   DESTINATION ${INSTALL_ROOT} COMPONENT runtime)
install(FILES LICENSE   DESTINATION ${INSTALL_ROOT} COMPONENT runtime)
install(FILES README.md DESTINATION ${INSTALL_ROOT} COMPONENT runtime)

# Install runtime data
install(DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION ${INSTALL_DATA} COMPONENT runtime)
