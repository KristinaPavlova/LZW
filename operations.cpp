#ifndef OPERAIONS_CPP
#define OPERATIONS_CPP
#include"operations.h"


void Operations::instructions()
{
    std::cout<<"     \e[1mINSTRUCTIONS\e[0m"<<std::endl;
    try
    {
        std::ifstream in("operations.txt");
        if(in.good())
        {
            std::string line;
            while(std::getline(in , line))
            {
                std::cout<<line<<std::endl;
            }
            in.close();
        }
        else
        {
            throw std::invalid_argument("file not found");
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Operations::zip(std::string &pathToFile, std::string &destination)
{
    try
    {
        archive.zip(pathToFile , destination);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Operations::unzipWholeArchive(std::string &pathToArchive, std::string &destination)
{
    try
    {
        archive.unzipWholeArchive(pathToArchive , destination);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Operations::unzipOneFile(std::string &pathToArchive,std::string &fileToDecompress,  std::string &destination)
{
    try
    {
        archive.unzipOneFile(pathToArchive , fileToDecompress, destination);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Operations::refresh(std::string &pathToArchive,std::string &fileToRefresh)
{
    try
    {
        archive.refresh(pathToArchive , fileToRefresh);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Operations::info(std::string &pathToArchive)
{
    try
    {
        archive.info(pathToArchive );
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Operations::ec(std::string &pathToArchive)
{
    bool foundError = false;

    try
    {
        foundError = archive.ec(pathToArchive );
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    if(foundError)
    {
        std::cout<<"error detected"<<std::endl;
    }
    else
    {
        std::cout<<"no errors detected"<<std::endl;
    }
}

#endif