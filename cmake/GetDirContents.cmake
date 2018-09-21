# - Functions to get contents of a directory
# 
# Courtesy of Jan Rüegg:
# https://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory
# 
# Example:
#   get_subdirs(subdirectories ${some_dir})
#   get_files(files_in_dir ${some_dir})
macro(get_dir_contents contents dir)
    set(tmp "")

    file(GLOB children RELATIVE ${dir} ${dir}/*)
    foreach(child ${children})
        list(APPEND tmp ${child})
    endforeach()

    set(${contents} ${tmp})
endmacro()

macro(get_dir_subdirs subdirs dir)
    set(tmp "")

    get_dir_contents(dir_contents ${dir})
    foreach(child ${dir_contents})
        if(IS_DIRECTORY ${dir}/${child})
            list(APPEND tmp ${child})
        endif()
    endforeach()

    set(${subdirs} ${tmp})
endmacro()

macro(get_dir_files dir_files dir)
    set(tmp "")

    get_dir_contents(dir_contents ${dir})
    foreach(child ${dir_contents})
        if(NOT IS_DIRECTORY ${dir}/${child})
            list(APPEND tmp ${child})
        endif()
    endforeach()

    set(${dir_files} ${tmp})
endmacro()