#pragma once

#include <filesystem>

#include <string>
#include <fstream>
#include <iostream>

class fileReader 
{

	private:
		static std::string extractContentsFromFile(std::filesystem::path locationToFile);
	public:
		static void trimDownPathToWorkingDirectory(std::filesystem::path &pathToTrim);
		static std::string readShader(std::string shaderFileName);
		static std::filesystem::path getPathToFileInFolder(std::string fileName, const std::string &folderToLookFor);
};
