#ifndef COMPRESS_H
#define COMPRESS_H
#include<deque>
#include<string>
#include<fstream>
#include <unordered_map>
#include<iostream>

class Compress
{
    private:

    int numberOf16Bits{};
    int numberOf32Bits{};


    void loadEncodeDictionary(std::unordered_map<std::string , int>& );

    void loadDecodeDictionary(std::unordered_map<int , std::string>& );

    public:

    Compress();

    std::deque<int>* encode(std::string& );

    std::string* decode( std::deque<int>&);

    const int getNumberOf16Bits()const;

    const int getNumberOf32Bits()const;

    void setnumberOf16Bits(const int&);

    void setnumberOf32Bits(const int&);

};









#endif