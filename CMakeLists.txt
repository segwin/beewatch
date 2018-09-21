# CMakeList.txt : CMake project for beewatch, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8.2)

#project()


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

set(PROJECT_EXE "${META_PROJECT_NAME}")
set(PROJECT_LIB "lib${META_PROJECT_NAME}")


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


# 
# Sources
# 
set(global_include_path "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(global_source_path "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Generate version-header
configure_file("${global_include_path}/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/version.h")

# Populate headers variable
set(headers "")

get_dir_files(header_files ${global_include_path})
foreach(header_file ${header_files})
    list(APPEND headers "${global_include_path}/${header_file}")
endforeach()

get_dir_subdirs(include_dirs ${global_include_path})
foreach(include_dir ${include_dirs})
    set(include_path "${global_include_path}/${include_dir}")
    get_dir_files(header_files "${include_path}")

    foreach(header_file ${header_files})
        list(APPEND headers "${include_path}/${header_file}")
    endforeach()
endforeach()

# Populate sources variable
set(sources "")

get_dir_files(source_files ${global_source_path})
foreach(source_file ${source_files})
    list(APPEND sources "${global_source_path}/${source_file}")
endforeach()

get_dir_subdirs(source_dirs ${global_source_path})
foreach(source_dir ${source_dirs})
    set(source_path "${global_source_path}/${source_dir}")
    get_dir_files(source_files "${source_path}")

    foreach(source_file ${source_files})
        list(APPEND sources "${source_path}/${source_file}")
    endforeach()
endforeach()

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")

source_group_by_path(${include_path} "\\\\.h$|\\\\.hpp$" 
    ${header_group} ${headers})
source_group_by_path(${source_path}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" 
    ${source_group} ${sources})

#
# Library
#

add_library(${PROJECT_LIB}
    ${sources}
    ${headers}
)

#
# Executable
#

# Add source to this project's executable.
add_executable (${PROJECT_EXE} "${sources}" "${headers}")

# TODO: Add tests and install targets if needed.


# 
# Include directories
# 

target_include_directories(${PROJECT_LIB}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_include_directories(${PROJECT_EXE}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)


#
# Tests
#

add_subdirectory(tests)


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
