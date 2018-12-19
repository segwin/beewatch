//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "util/file.h"

#include "global/logging.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

namespace beewatch::file
{
    
    //==============================================================================
    bool exists(std::string path)
    {
        return boost::filesystem::exists(path);
    }

    bool isDirectory(std::string path)
    {
        return boost::filesystem::is_directory(path);
    }

    bool isFile(std::string path)
    {
        return boost::filesystem::is_regular_file(path);
    }
    
    //==============================================================================
    std::vector<std::string> listContents(std::string directory, bool relative)
    {
        std::vector<std::string> directoryContents;

        try
        {
            if (!exists(directory))
                throw std::invalid_argument("Directory not found");

            if (!isDirectory(directory))
                throw std::invalid_argument("Not a directory");

            for (auto& entry : boost::filesystem::recursive_directory_iterator(directory))
            {
                auto path = entry.path();

                if (relative)
                {
                    path = boost::filesystem::relative(path, directory);
                }

                directoryContents.push_back(path.string());
            }
        }
        catch (const std::exception& e)
        {
            g_logger.error("Failed to get contents of directory \"" + directory + "\": " + e.what());
        }

        return directoryContents;
    }

    //==============================================================================
    std::string readText(std::string path)
    {
        std::ifstream fileStream(path);
        std::ostringstream textStream;

        textStream << fileStream.rdbuf();

        return textStream.str();
    }

} // namespace beewatch::file
