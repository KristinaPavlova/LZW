#ifndef OPERAIONS_H
#define OPERATIONS_H
#include<string>
#include"Archive.h"
#include<fstream>

class Operations
{
    public:

    void instructions();

    void zip(std::string &, std::string &);

    void unzipWholeArchive(std::string &, std::string &);

    void unzipOneFile(std::string & , std::string & , std::string &);

    void refresh(std::string & , std::string & );

    void info(std::string & );

    void ec(std::string & );

    private:

    Archive archive{};

};


#endif