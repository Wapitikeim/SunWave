#include "fileReader.h"

void fileReader::trimDownPathToWorkingDirectory(std::filesystem::path &pathToTrim)
{
    if(pathToTrim.empty())
    {
        return;
    }
    while(pathToTrim.filename() != "out" && !pathToTrim.empty())
    {
        pathToTrim._Remove_filename_and_separator();
    }
    if(!pathToTrim.empty())
    {
        pathToTrim.remove_filename();
    }
    

}

std::string fileReader::extractContentsFromFile(std::filesystem::path locationToFile)
{   
    std::string fileContent;

    std::ifstream fileToReadFrom;
    fileToReadFrom.open(locationToFile);
    std::string lineToGetRead;
    while(std::getline(fileToReadFrom, lineToGetRead))
    {
        fileContent += lineToGetRead + "\n";
    }
    fileToReadFrom.close();

    return fileContent;
}

std::string fileReader::readShader(std::string shaderFileName)
{
    std::filesystem::path path(std::filesystem::current_path());
    trimDownPathToWorkingDirectory(path);
    path.append("src");
    path.append("shaders");
    path.append("");
    path.append(shaderFileName);
    if(!std::filesystem::exists(path))
    {
        std::cout << "Couldnt find shader file : " << shaderFileName;
    }
    
    return extractContentsFromFile(path);
}

std::filesystem::path fileReader::getPathToFileInFolder(std::string fileName, const std::string &folderToLookFor)
{
    //Needs work to be more flexible its just looking for the folder in the rootDirectory
    std::filesystem::path path(std::filesystem::current_path());
    trimDownPathToWorkingDirectory(path);
    path.append(folderToLookFor);
    path.append("");
    path.append(fileName);
    if(!std::filesystem::exists(path))
    {
        std::cout << "Couldnt find path : " << fileName << "in" << folderToLookFor;
    }
    return path;
}

std::vector<std::string> fileReader::getAllLevelFileNames()
{
    std::vector<std::string> fileNames;
    std::filesystem::path path(std::filesystem::current_path());
    trimDownPathToWorkingDirectory(path);
    path.append("src");
    path.append("scenes");
    path.append("LevelConfigurations");

    // Iterate through the files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            // Get the file name without the extension
            std::string fileName = entry.path().stem().string();
            fileNames.push_back(fileName);
        }
    }

    return fileNames;
}
