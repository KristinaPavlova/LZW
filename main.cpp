#include<iostream>
#include<fstream>
#include <unordered_map>
#include<string>
#include<cstring>
#include<sstream>
#include<algorithm>
#include <deque>
#include"Compress.cpp"
#include"Archive.cpp"
#include"operations.cpp"

void toUpperCase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void enterOperations()
{
    Operations operation;
    operation.instructions();
    std::string operationName = "" , input = "";
    std::string pathToArchive = "" , pathToFile = "" , destination = "";

    while(getline(std::cin , input))
    {
        std::stringstream ss(input);
        ss >> operationName;
        toUpperCase(operationName);
        if(operationName == "ZIP")
        {
            ss>>pathToFile;
            ss>>destination;
            operation.zip(pathToFile , destination);
        }
        else if(operationName == "UNZIPWHOLEARCHIVE")
        {
            ss>>pathToArchive;
            ss>>destination;
            operation.unzipWholeArchive(pathToArchive , destination);
        }
        else if(operationName == "UNZIPONEFILE")
        {
            ss>>pathToArchive;
            ss>>pathToFile;
            ss>>destination;
            operation.unzipOneFile(pathToArchive, pathToFile , destination);
        }
        else if(operationName == "REFRESH")
        {
            ss>>pathToArchive;
            ss>>pathToFile;
            operation.refresh(pathToArchive, pathToFile);
        }
        else if(operationName == "INFO")
        {
            ss>>pathToArchive;
            operation.info(pathToArchive );
        }
        else if(operationName == "EC")
        {
            ss>>pathToArchive;
            operation.ec(pathToArchive );
        }
        else if(operationName == "EXIT")
        {
            ss.clear();
            return;
        }
        else
        {
            std::cout<<"invalid operation"<<std::endl;
        }

    }
}

int main()
{
    enterOperations();

    return 0;
}

