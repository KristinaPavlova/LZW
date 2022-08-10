#ifndef ARCHIVE_H
#define ARCHIVE_H
#include<list>
#include<string>
#include<dirent.h>
#include<sys/stat.h>
#include<exception>
#include<stdexcept> // invalid argument
#include<fstream>
#include <cstring>
#include"Compress.h"

class Archive
{
    private:

    Compress lzwObj;

    //check if given path is file
    bool isFile(std::string&);

    //check if given path is directory
    bool isDir(std::string&);

    void erasePathName(std::string& ,const std::string&);

    void eraseIncorrectPathsManyChars(std::list<std::string>* , const std::string&);

    void eraseIncorrectPathsOneChar(std::list<std::string>* , const std::string&);

    //returns a list of all file names
    std::list<std::string>* explore(char* );

    void writeFileContentInFile(std::deque<int>* , const std::string&);

    //check if directory already exists
    bool directoryExist(std::string&);

    //create directory
    void createDir(const std::string&);

    //compress single file
    void compressFileContent(std::string &, const std::string &, std::string &);
    
    //compress directory
    void compressDir(std::string&  , std::string&);

    //compress files in directories when is used * or !
    void compressPartialPath(std::string&  , std::string&);

public:

    void zip(std::string &, std::string &);

    void unzipWholeArchive(std::string &, std::string &);

    void unzipOneFile(std::string & , std::string & , std::string &);

    void refresh(std::string & , std::string & );

    void info(std::string & );

    bool ec(std::string & );

};

#endif