# - Functions to get contents of a directory
# 
# Courtesy of Jan Rüegg:
# https://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory
# 
# Example:
#   get_subdirs(subdirectories ${some_dir})
#   get_files(files_in_dir ${some_dir})
macro(get_dir_contents contents dir)
    set(${contents} "")

    file(GLOB children RELATIVE ${dir} ${dir}/*)
    foreach(child ${children})
        list(APPEND ${contents} ${child})
    endforeach()
endmacro()

macro(get_dir_subdirs subdirs dir)
    set(${subdirs} "")

    get_dir_contents(dir_contents ${dir})
    foreach(child ${dir_contents})
        if(IS_DIRECTORY ${dir}/${child})
            list(APPEND ${subdirs} ${child})
        endif()
    endforeach()
endmacro()

macro(get_dir_files dir_files dir)
    set(${dir_files} "")

    get_dir_contents(dir_contents ${dir})
    foreach(child ${dir_contents})
        if(NOT IS_DIRECTORY ${dir}/${child})
            list(APPEND ${dir_files} ${child})
        endif()
    endforeach()
endmacro()

macro(get_files_recursive files_out dir file_name_regex)
    # Get files in directory
    get_dir_files(dir_files ${dir})
    foreach(dir_file ${dir_files})
        if("${dir_file}" MATCHES "${file_name_regex}")
            list(APPEND ${files_out} "${dir}/${dir_file}")
        endif()
    endforeach()

    # Get files in subdirectories (recurse)
    get_dir_subdirs(dir_subdirs ${dir})
    foreach(dir_subdir ${dir_subdirs})
        get_files_recursive(${files_out} "${dir}/${dir_subdir}" ${file_name_regex})
    endforeach()
endmacro()