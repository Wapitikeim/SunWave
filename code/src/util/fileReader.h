#ifndef FILEREADER_H
#define FILEREADER_H

#include <filesystem>

#include <string>
#include <fstream>
#include <iostream>

class fileReader 
{

private:
	static void trimDownPathToWorkingDirectory(std::filesystem::path &pathToTrim);
	static std::string extractContentsFromFile(std::filesystem::path locationToFile);
public:
	static std::string readShader(std::string shaderFileName);
	static std::filesystem::path getPathToFileInFolder(std::string fileName, const std::string &folderToLookFor);
};

#endif