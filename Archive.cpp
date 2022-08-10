#ifndef ARCHIVE_CPP
#define ARCHIVE_CPP

#include"Archive.h"
#include<exception>

bool Archive::isFile(std::string& path)
{
    if(path.empty())
    {
        throw std::invalid_argument("empty path");
    }
    char* ptr = &path[0];
    struct stat s;
    if(stat(ptr , &s) == 0)
    {
        if(s.st_mode & S_IFREG) // its a file
        {
            return true;
        }
        else if(s.st_mode & S_IFDIR) // its a directory
        {
            return false;
        }
        else // its something else
        {
            return false;
        }
    }
    else // error
    {
        throw std::invalid_argument("file doesn't exist");
    }

}

bool Archive::isDir(std::string& path)
{
    if(path.empty())
    {
        throw std::invalid_argument("empty path");
    }
    char* ptr = &path[0];
    struct stat s;
    if(stat(ptr , &s) == 0)
    {
        if(s.st_mode & S_IFDIR) // its a directory
        {
            return true;
        }
        else if(s.st_mode & S_IFREG) // its a file
        {
            return false;
        }
        else // its something else
        {
            return false;
        }
    }
    else // error
    {
        throw std::invalid_argument("error in the path");
    }

}

void Archive::erasePathName(std::string& pathName , const std::string& originalPath)
{
    int len = 0;
    if(originalPath == "") //compress single file
    {
        for (int i = pathName.length(); pathName[i] != '/'; --i)
        {
            len++;
        }
        pathName.erase(0, pathName.length() - len + 1);
    }
    else
    {
        for (int i = 0; pathName[i] == originalPath[i]; ++i)
        {
            len++;
        }
        pathName.erase(0, len + 1);
    }

}

void Archive::eraseIncorrectPathsManyChars(std::list<std::string>* pathNames, const std::string& originalPath)
{
    // int len = pathNames->size();
    bool isRemoved = false;
    for(auto it = pathNames->begin() ; it != pathNames->end() ;   )
    {
        isRemoved = false;
        if( (*it).length() < originalPath.length())
        {
            auto itCpy = it;
            itCpy++;
            pathNames->erase((it));
            if (itCpy != pathNames->end())
            {
                it = itCpy;
            }
           continue;
        }
        
        for(int j = 0 ; j < originalPath.length() -1 ; ++j)
        {
            if((*it)[j] != originalPath[j])
            {
                auto itCpy = it;
                itCpy++;
                pathNames->erase((it));
                if(itCpy != pathNames->end())
                {
                    it = itCpy;
                }
                isRemoved = true;
                break;
            }
        }
        if(!isRemoved)
        {
           ++it ; 
        }
        
    }
}

void Archive::eraseIncorrectPathsOneChar(std::list<std::string>* pathNames, const std::string& originalPath)
{
    // int len = pathNames->size();
    bool isRemoved = false;
    for(auto it = pathNames->begin() ; it != pathNames->end() ; )
    {
        isRemoved = false;
        if( (*it).length() != originalPath.length())
        {
            auto itCpy = it;
            itCpy++;
            pathNames->erase((it));
            // if (itCpy != pathNames->end())
            // {
                it = itCpy;
            // }
           continue;
        }
        
        for(int j = 0 ; j < originalPath.length() - 1 ; ++j)
        {
            if((*it)[j] != originalPath[j])
            {
                auto itCpy = it;
                itCpy++;
                pathNames->erase((it));
                if(itCpy != pathNames->end())
                {
                    it = itCpy;
                }
                isRemoved = true;
                break;
            }
        }
        if(!isRemoved)
        {
           ++it ; 
        }
        
    }
}

std::list<std::string>* Archive::explore(char* path)
{
    std::list<std::string>* filesPaths = nullptr;
    try
    {
        filesPaths = new std::list<std::string>;
    }
    catch(...)
    {
       throw std::bad_alloc();
    }
    DIR* dir; // pointer to directory
    struct dirent* entry;// everything in the directory
    struct stat info; //information about each entry

    // open
    dir = opendir(path);
    if(!dir) // dir = nullptr -> could't open the directory
    {
        throw std::invalid_argument("wrong path");    
    }
    while( (entry = readdir(dir)) != NULL)
    {
        if(entry->d_name[0] != '.')
        {
            std::string pathNew ;
            pathNew = std::string(path) + "/" + std::string(entry->d_name);
            stat(pathNew.c_str() , &info);
            if(S_ISDIR(info.st_mode))
            {
                std::list<std::string> n;
                n = *(explore((char*)pathNew.c_str()));
                if(!n.empty())
                {
                    filesPaths->insert(filesPaths->end() , n.begin() , n.end()); 
                }
                else
                {
                    filesPaths->push_back((pathNew + "/")); // if directory is empty add "/" -> to indicate directory not file
                }
                
            }
            else 
            {
                filesPaths->push_back(pathNew);
            }

        }
    }
    closedir(dir);

    return filesPaths;

}

void Archive::writeFileContentInFile(std::deque<int>* encoded  , const std::string& pathDestination)
{
    std::ofstream out(pathDestination , std::ios::binary | std::ios::app);
    if(out.good())
    {
        int numOf16Bits = this->lzwObj.getNumberOf16Bits();
        int numOf32Bits = this->lzwObj.getNumberOf32Bits();

        out.write((const char*)&numOf16Bits  , sizeof(int));
        for(int i = 0 ; i < numOf16Bits ; ++i)
        {
            out.write((const char*)&encoded->front()  , sizeof(uint16_t));
            encoded->pop_front();
        }

        out.write((const char*)&numOf32Bits  , sizeof(int));
        for(int i = 0 ; i < numOf32Bits ; ++i)
        {
            out.write((const char*)&encoded->front()  , sizeof(int));
            encoded->pop_front();
        }
       
        out.close();
    }
}

void Archive::compressFileContent(std::string& path , const std::string& originalPath , std::string& destination)
{
    std::ifstream in(path);
    if(in.is_open())
    {
        std::string fileContent = "";
        char word;
        while(!in.eof())
        {
            in>>std::noskipws>>word;
            fileContent += word;
            
        }

        std::deque<int>* encodedContent = nullptr;

        in.close();
        
        encodedContent = this->lzwObj.encode(fileContent);

        double ratio = ( (double)encodedContent->size() * 100) / fileContent.length();

        if(encodedContent == nullptr)
        {
            throw std:: bad_exception();
        }
        
        erasePathName(path , originalPath);

        uint16_t fileNameLenght = (uint16_t)path.size();

        std::ofstream out(destination, std::ios::binary  | std::ios::app);
        if (out.good())
        {
            out.write((const char*)& fileNameLenght , sizeof(uint16_t));

            out.write( path.c_str() , fileNameLenght);

            out.write((const char*)&ratio , sizeof(double));

            out.close();
        }
 
       this->writeFileContentInFile(encodedContent , destination);

       this->lzwObj.setnumberOf16Bits(0);
       this->lzwObj.setnumberOf32Bits(0);
        
    }
}

bool Archive::directoryExist( std::string& path)
{
    char* ptr = &path[0];

    DIR *dir = opendir(ptr); 
    // if dir != nullptr -> directory exist
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else
    {
        return false;
    }
}

void Archive::createDir(const std::string& fullPath)
{
    std::string pathCpy = fullPath;
    if (!this->directoryExist(pathCpy))
    {
        int status;
        const char *c = &pathCpy[0];
        status = mkdir(c, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (status == -1) // coudn't create directory for some reason
        {
            throw std::bad_alloc();
        }
    }
}

void Archive::compressDir(std::string& path , std::string& destination)
{
    std::list<std::string>* pathNames = nullptr;
    try
    {
        pathNames = new std::list<std::string>;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        throw;
    }

    pathNames = this->explore(&path[0]);

    for(auto it = pathNames->begin() ; it != pathNames->end() ; ++it )
    {
        if(isFile((*it)))
        {
            this->compressFileContent((*it) , path , destination);
        }
        else if(isDir((*it)))
        {
            (*it).pop_back();
            erasePathName(*it , "");

            *it += '/';

            uint16_t fileNameLenght = (*it).length();

            std::ofstream out(destination, std::ios::binary  | std::ios::app);
            if (out.good())
            {
                out.write((const char *)&fileNameLenght, sizeof(uint16_t));

                out.write((*it).c_str(), fileNameLenght);

                out.close();
            }
            else
            {
                throw std::invalid_argument("can't open file");
            }
        }
        else
        {
            throw std::invalid_argument("undefined file");
        }
    }
    
}

void Archive::compressPartialPath(std::string& path , std::string& destination)
{
    std::list<std::string>* pathNames = nullptr;
    try
    {
        pathNames = new std::list<std::string>;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        throw;
    }

    std::string originalPathCpy = path;

    while(originalPathCpy.back() != '/')
    {
        originalPathCpy.pop_back();
    }

    originalPathCpy.pop_back();

    pathNames = this->explore(&originalPathCpy[0]);

    if(path.back() == '*')
    {
        eraseIncorrectPathsManyChars(pathNames , path);
    }
    else if(path.back() == '?')
    {
        eraseIncorrectPathsOneChar(pathNames , path);
    }
    else
    {
        throw std::invalid_argument("not valid operation");
    }


    for(auto it = pathNames->begin() ; it != pathNames->end() ; ++it )
    {
        if(isFile((*it)))
        {
            this->compressFileContent((*it) , originalPathCpy , destination);
        }
        else if(isDir((*it)))
        {
            (*it).pop_back();
            erasePathName(*it , "");

            *it += '/';

            uint16_t fileNameLenght = (*it).length();

            std::ofstream out(destination, std::ios::binary  | std::ios::app);
            if (out.good())
            {
                out.write((const char *)&fileNameLenght, sizeof(uint16_t));

                out.write((*it).c_str(), fileNameLenght);

                out.close();
            }
            else
            {
                throw std::invalid_argument("can't open file");
            }
        }
        else
        {
            throw std::invalid_argument("undefined file");
        }
    }

    
}

void Archive::zip(std::string& path , std::string& destination)
{
    try
    {
        if (isFile(path))
        {
            this->compressFileContent(path, "", destination);
            return;
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "incorrect file" << std::endl;
        return;
    }
    try
    {
        if (isDir(path))
        {
            this->compressDir(path, destination);
            return;
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "incorrect file" << std::endl;
        return;
    }
    //partial path
    try
    {
       compressPartialPath(path, destination);
    }
    catch(const std::exception& e)
    {
        std::cout << "incorrect file" << std::endl;
        return;
    }



}

void Archive::unzipWholeArchive(std::string& pathToCompressedFile , std::string& destination)
{
    if(!isFile(pathToCompressedFile))
    {
        throw std::invalid_argument("argument is not a file");
    }
    std::ifstream in(pathToCompressedFile , std::ios::binary);
    if(in.good())
    {
        std::string pathName= "";
        char* pathNameptr = nullptr;
        uint16_t size = 0;

        //get file size
        in.seekg(0 , in.end);
        int fileSize = (int)in.tellg();
        in.seekg(0 , in.beg);
        while(in.tellg() < fileSize)
        {            
            in.read((char*)&size , sizeof(uint16_t));
            try
            {
                pathNameptr = new char[size + 1];
            }
            catch(const std::exception& e)
            {
                in.close();
                std::cerr << e.what() << '\n';
                throw;
            }
            
            in.read(pathNameptr , size);
            if(in.tellg() >= fileSize)
            {
                if(pathNameptr != nullptr)
                {
                    delete[] pathNameptr;
                }
                break;
            }


            pathName.append(pathNameptr , size);
            
            delete[] pathNameptr;
 
            if(pathName.back() == '/') // its an empty dir
            {
                pathName.pop_back();
                this->createDir(destination + '/' + pathName);
                pathName = "";
                continue;
            }
            else//its file
            {
                // create directory if needed
                std::string newDir = "";
                for(int i = 0 ; i < pathName.length() ; ++i)
                {
                    newDir += pathName[i];
                    if(pathName[i+1] == '/')
                    {
                        this->createDir(destination + '/' + newDir);
                    }
                }

                // read file content
                
                std::deque<int> encodedFileContent;

                uint16_t num16Bits = 0;
                int num32Bits = 0;

                // //read ratio
                double ratio;
                in.read((char*)&ratio , sizeof(double));

                //read 16 bits
                int numOf16Bits;
                in.read((char*)&numOf16Bits , sizeof(int));

                for(int  i =0 ; i < numOf16Bits ; ++i)
                {
                    in.read((char*)&num16Bits , sizeof(uint16_t));
                    encodedFileContent.push_back(num16Bits);
                }

                //read 32 bits
                int numOf32Bits;
                in.read((char*)&numOf32Bits , sizeof(int));

                for(int  i =0 ; i < numOf32Bits ; ++i)
                {
                    in.read((char*)&num32Bits , sizeof(int));
                    encodedFileContent.push_back(num32Bits);
                }

                //decode file content

                std::string* decodedFileContent = this->lzwObj.decode(encodedFileContent);

                //create file and write decoded content

                std::string result = "";

                result += (*decodedFileContent);

                std::ofstream out(destination + '/' + pathName );

                if(out.is_open())
                {
                    out<<result;
                    out.close();
                }

                pathName = "";
                if(decodedFileContent != nullptr)
                {
                    delete decodedFileContent;
                }
            }
        }

        in.close();
    }
 
}

void Archive::unzipOneFile(std::string &pathToCompressedFile, std::string &fileToUnzip, std::string &destination)
{
    if(!isFile(pathToCompressedFile))
    {
        throw std::invalid_argument("argument is not a file");
    }
    std::ifstream in(pathToCompressedFile , std::ios::binary);
    if(in.good())
    {
        std::string pathName= "";
        char* pathNameptr = nullptr;
        uint16_t size = 0;

        //get file size
        in.seekg(0 , in.end);
        int fileSize = (int)in.tellg();
        in.seekg(0 , in.beg);
        while(in.tellg() < fileSize)
        {            
            in.read((char*)&size , sizeof(uint16_t));
            try
            {
                pathNameptr = new char[size + 1];
            }
            catch(const std::exception& e)
            {
                in.close();
                std::cerr << e.what() << '\n';
                throw;
            }
            
            in.read(pathNameptr , size);
            if(in.tellg() >= fileSize)
            {
                if(pathNameptr != nullptr)
                {
                    delete[] pathNameptr;
                }
                break;
            }

            pathName = "";

            pathName.append(pathNameptr , size);
            
            delete[] pathNameptr;

            if (pathName == fileToUnzip)
            {

                if (pathName.back() == '/') // its an empty dir
                {
                    pathName.pop_back();
                    this->createDir(destination + '/' + pathName);
                    pathName = "";
                    continue;
                }
                else // its file
                {
                    // create directory if needed
                    std::string newDir = "";
                    for (int i = 0; i < pathName.length(); ++i)
                    {
                        newDir += pathName[i];
                        if (pathName[i + 1] == '/')
                        {
                            this->createDir(destination + '/' + newDir);
                        }
                    }

                    // read file content

                    std::deque<int> encodedFileContent;

                    uint16_t num16Bits = 0;
                    int num32Bits = 0;

                    // //read size of file
                    double ratio;
                    in.read((char *)&ratio, sizeof(double));

                    // read 16 bits
                    int numOf16Bits;
                    in.read((char *)&numOf16Bits, sizeof(int));

                    for (int i = 0; i < numOf16Bits; ++i)
                    {
                        in.read((char *)&num16Bits, sizeof(uint16_t));
                        encodedFileContent.push_back(num16Bits);
                    }

                    // read 32 bits
                    int numOf32Bits;
                    in.read((char *)&numOf32Bits, sizeof(int));

                    for (int i = 0; i < numOf32Bits; ++i)
                    {
                        in.read((char *)&num32Bits, sizeof(int));
                        encodedFileContent.push_back(num32Bits);
                    }

                    // decode file content

                    std::string *decodedFileContent = this->lzwObj.decode(encodedFileContent);

                    // create file and write decoded content

                    std::string result = "";

                    result += (*decodedFileContent);

                    std::ofstream out(destination + '/' + pathName);

                    if (out.is_open())
                    {
                        out << result;
                        out.close();
                    }

                    pathName = "";
                    if (decodedFileContent != nullptr)
                    {
                        delete decodedFileContent;
                    }
                }
                break;
            }
            else
            {
                if (pathName.back() == '/')
                {
                    pathName = "";
                    continue;
                }
                else
                {
                    uint16_t num16Bits = 0;
                    int num32Bits = 0;

                    // //read ratio
                    double ratio;
                    in.read((char *)&ratio, sizeof(double));

                    // read 16 bits
                    int numOf16Bits;
                    in.read((char *)&numOf16Bits, sizeof(int));

                    for (int i = 0; i < numOf16Bits; ++i)
                    {
                        in.read((char *)&num16Bits, sizeof(uint16_t));
                        
                    }

                     // read 32 bits
                    int numOf32Bits;
                    in.read((char *)&numOf32Bits, sizeof(int));

                    for (int i = 0; i < numOf32Bits; ++i)
                    {
                        in.read((char *)&num32Bits, sizeof(int));
                       
                    }
                }
            }
        }

        in.close();
    }
}

void Archive::refresh(std::string &pathToCompressedFile, std::string &fileToRefresh)
{
    if(!isFile(pathToCompressedFile))
    {
        throw std::invalid_argument("argument is not a file");
    }
    bool foundMatch = false;
    std::ifstream in(pathToCompressedFile , std::ios::binary | std::ios::app);
    std::string tempFileName = "helperFile.bin";
    std::ofstream out( tempFileName, std::ios::binary);
    std::string searchedName = fileToRefresh;
    this->erasePathName(searchedName , "");

    if(in.good() && out.good())
    {
        std::string pathName= "";
        char* pathNameptr = nullptr;
        uint16_t size = 0;

        //get file size
        in.seekg(0 , in.end);
        int fileSize = (int)in.tellg();
        in.seekg(0 , in.beg);
        int sizeFileCOntent;
        while(in.tellg() < fileSize)
        {            
            in.read((char*)&size , sizeof(uint16_t));
            try
            {
                pathNameptr = new char[size + 1];
            }
            catch(const std::exception& e)
            {
                in.close();
                std::cerr << e.what() << '\n';
                throw;
            }
            
            in.read(pathNameptr , size);
            if(in.tellg() >= fileSize)
            {
                if(pathNameptr != nullptr)
                {
                    delete[] pathNameptr;
                }
                break;
            }

            pathName = "";

            pathName.append(pathNameptr , size);
            
            delete[] pathNameptr;

            if(pathName == searchedName)
            {
                foundMatch = true;

                if (pathName.back() == '/') // its an empty dir
                {
                    continue;
                }
                else // its file
                {
                    // read file content

                    uint16_t num16Bits = 0;
                    int num32Bits = 0;

                    // //read ratio
                    double ratio;
                    in.read((char *)&ratio, sizeof(double));

                    // read 16 bits
                    int numOf16Bits;
                    in.read((char *)&numOf16Bits, sizeof(int));

                    for (int i = 0; i < numOf16Bits; ++i)
                    {
                        in.read((char *)&num16Bits, sizeof(uint16_t));
                    }

                    // read 32 bits
                    int numOf32Bits;
                    in.read((char *)&numOf32Bits, sizeof(int));

                    for (int i = 0; i < numOf32Bits; ++i)
                    {
                        in.read((char *)&num32Bits, sizeof(int));
                    }
                }
            }
            else
            {
                out.write((const char*)&size , sizeof(uint16_t));
                out.write(pathName.c_str() , size);
                if (pathName.back() == '/') // its an empty dir
                {
                    continue;
                }
                else // its file
                {
                    // read file content

                    uint16_t num16Bits = 0;
                    int num32Bits = 0;

                    // //read ratio
                    double ratio;
                    in.read((char *)&ratio, sizeof(double));
                    out.write((const char*)&ratio , sizeof(double));

                    // read 16 bits
                    int numOf16Bits;
                    
                    in.read((char *)&numOf16Bits, sizeof(int));
                    
                    out.write((const char*)&numOf16Bits , sizeof(int));

                    for (int i = 0; i < numOf16Bits; ++i)
                    {
                        in.read((char *)&num16Bits, sizeof(uint16_t));
                        out.write((const char*)&num16Bits , sizeof(uint16_t));
                    }

                    // read 32 bits
                    int numOf32Bits;
                    in.read((char *)&numOf32Bits, sizeof(int));
                    out.write((const char*)&numOf32Bits , sizeof(int));

                    for (int i = 0; i < numOf32Bits; ++i)
                    {
                        in.read((char *)&num32Bits, sizeof(int));
                        out.write((const char*)&num32Bits , sizeof(int));
                    }
                }
            }
        }

        in.close();
        out.close();

        if(foundMatch)
        {
            this->compressFileContent(fileToRefresh, "", tempFileName);
            // Remove and rename
            std::remove(pathToCompressedFile.c_str());
            std::rename(tempFileName.c_str(), pathToCompressedFile.c_str());
        }
        else
        {
            std::remove(pathToCompressedFile.c_str());
            throw std::invalid_argument("no match found");
        }
    }
}

void Archive::info(std::string &pathToCompressedFile)
{
    if(!isFile(pathToCompressedFile))
    {
        throw std::invalid_argument("argument is not a file");
    }
    bool foundMatch = false;
    std::ifstream in(pathToCompressedFile , std::ios::binary | std::ios::app);

    if(in.good() )
    {
        std::string pathName= "";
        char* pathNameptr = nullptr;
        uint16_t size = 0;

        //get file size
        in.seekg(0 , in.end);
        int fileSize = (int)in.tellg();
        in.seekg(0 , in.beg);
        int sizeFileCOntent;
        while(in.tellg() < fileSize)
        {            
            in.read((char*)&size , sizeof(uint16_t));
            try
            {
                pathNameptr = new char[size + 1];
            }
            catch(const std::exception& e)
            {
                in.close();
                std::cerr << e.what() << '\n';
                throw;
            }
            
            in.read(pathNameptr , size);
            if(in.tellg() >= fileSize)
            {
                if(pathNameptr != nullptr)
                {
                    delete[] pathNameptr;
                }
                break;
            }

            pathName = "";

            pathName.append(pathNameptr , size);

            delete[] pathNameptr;

            std::cout << pathName;
            if (pathName.back() == '/') // its an empty dir
            {
                std::cout<<"\n\n";
                continue;
            }
            else // its file
            {
                // read file content

                uint16_t num16Bits = 0;
                int num32Bits = 0;

                // //read ratio
                double ratio;
                in.read((char *)&ratio, sizeof(double));
                std::cout<<" : "<<ratio<<"%\n"<<std::endl;

                // read 16 bits
                int numOf16Bits;
                in.read((char *)&numOf16Bits, sizeof(int));

                for (int i = 0; i < numOf16Bits; ++i)
                {
                    in.read((char *)&num16Bits, sizeof(uint16_t));
                }

                // read 32 bits
                int numOf32Bits;
                in.read((char *)&numOf32Bits, sizeof(int));

                for (int i = 0; i < numOf32Bits; ++i)
                {
                    in.read((char *)&num32Bits, sizeof(int));
                }
            }
        }

        in.close();
    }
    else
    {
        throw std::invalid_argument("incorrect file path");
    }
}

bool Archive::ec(std::string &pathToCompressedFile )
{
    if(!isFile(pathToCompressedFile))
    {
        throw std::invalid_argument("argument is not a file");
    }
    std::ifstream in(pathToCompressedFile , std::ios::binary);
    if(in.good())
    {
        std::string pathName= "";
        char* pathNameptr = nullptr;
        uint16_t size = 0;

        //get file size
        in.seekg(0 , in.end);
        int fileSize = (int)in.tellg();
        in.seekg(0 , in.beg);
        int sizeFileCOntent;
        while(in.tellg() < fileSize)
        {            
            in.read((char*)&size , sizeof(uint16_t));
            try
            {
                pathNameptr = new char[size + 1];
            }
            catch(const std::exception& e)
            {
                in.close();
                std::cerr << e.what() << '\n';
                throw;
            }
            
            in.read(pathNameptr , size);
            if(in.tellg() >= fileSize)
            {
                if(pathNameptr != nullptr)
                {
                    delete[] pathNameptr;
                }
                break;
            }

            pathName = "";

            pathName.append(pathNameptr , size);

            delete[] pathNameptr;

            if (pathName.back() == '/') // its an empty dir
            {
                continue;
            }
            else // its file
            {
                // read file content

                uint16_t num16Bits = 0;
                int num32Bits = 0;

                // //read ratio
                double ratio;
                in.read((char *)&ratio, sizeof(double));

                // read 16 bits
                int numOf16Bits;
                in.read((char *)&numOf16Bits, sizeof(int));

                for (int i = 0; i < numOf16Bits; ++i)
                {
                    in.read((char *)&num16Bits, sizeof(uint16_t));
                }

                // read 32 bits
                int numOf32Bits;
                in.read((char *)&numOf32Bits, sizeof(int));

                for (int i = 0; i < numOf32Bits; ++i)
                {
                    in.read((char *)&num32Bits, sizeof(int));
                }
                
                if(numOf16Bits <= 0 && numOf32Bits > 0)
                {
                    return false;
                }
            }
        }
        in.close();
        return false;
    }
    else
    {
        return true;
    }
}

#endif