//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include <string>
#include <vector>

namespace beewatch::file
{
    
    //==============================================================================
    /// Returns true if the given path exists on the filesystem
    bool exists(std::string path);

    /// Returns true if the given path matches a directory
    bool isDirectory(std::string path);
    /// Returns true if the given path matches a file
    bool isFile(std::string path);
    
    //==============================================================================
    /**
     * @brief Return directory contents as list of paths, recursing into subdirectories
     *
     * @param [in] directory    Directory to list contents
     * @param [in] relative     If true, return paths relative to given directory instead of absolute paths
     *
     * @returns List of directory contents as absolute or relative paths
     */
    std::vector<std::string> listContents(std::string directory, bool relative = false);

    //==============================================================================
    /// Read text file to string
    std::string readText(std::string path);

} // namespace beewatch::file
