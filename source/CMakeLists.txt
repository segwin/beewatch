
# 
# Configuration for all sub-projects
# 

# Generate version-header
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME}/${META_PROJECT_NAME}-version.h)


# 
# Sub-projects
# 

# Libraries
set(IDE_FOLDER "")
add_subdirectory(ctrl)
add_subdirectory(health)
add_subdirectory(hw)
add_subdirectory(io)
add_subdirectory(mass)
add_subdirectory(reporting)
add_subdirectory(util)

# Executable
add_subdirectory(${META_PROJECT_NAME})

# Tests
if(OPTION_BUILD_TESTS)
    set(IDE_FOLDER "Tests")
    add_subdirectory(tests)
endif()

# 
# Deployment
# 

# Deploy generated headers
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME} DESTINATION include COMPONENT dev)
