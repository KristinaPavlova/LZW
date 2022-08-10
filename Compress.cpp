#ifndef COMPRESS_CPP
#define COMPRESS_CPP
#include"Compress.h"

Compress::Compress()
{
    this->numberOf16Bits = 0;
    this->numberOf32Bits = 0;
}

void Compress::loadEncodeDictionary(std::unordered_map<std::string , int>& dictionary)
{
    std::string s="";
    for(int i = 0 ; i <= 255 ; ++i)
    {
        s += (char)i;
        dictionary[s] = i;
        s = "";
    }
}

void Compress::loadDecodeDictionary(std::unordered_map<int , std::string>& dictionary)
{
    std::string str = "";

    for(int i = 0 ; i <= 255 ; ++i)
    {
        str += (char)i;
        dictionary[i] = str;
        str = "";
    }
}

std::deque<int>* Compress::encode(std::string& textToCode)
{
    std::unordered_map<std::string , int> dictionary;
    this->loadEncodeDictionary(dictionary);
    int codeNum = 256;

    int count16Bits = 0 , count32Bits = 0;

    std::deque<int>* output = nullptr;

    try
    {
        output = new std::deque<int>;
    }
    catch(const std::exception& e)
    {
        std::cout<<"couldnt allocate memory"<<std::endl;
        throw;
    }
    
    std::string p = "" , c = "";

    p += textToCode[0];

    for(int i = 0 ; i <textToCode.length() ; ++i)
    {
        if(i != textToCode.length() - 1)
        {
            c += textToCode[i+1];
        }
        if (dictionary.find(p + c) != dictionary.end())
        {
            p = p + c;
        }
        else
        {
            
            output->push_back(dictionary[p]);
            dictionary[p + c] = codeNum;
            if(codeNum > 65000) // max value of uint16
            {
                this->numberOf32Bits++;
            }
            else
            {
                this->numberOf16Bits++;
            }
            ++codeNum;
            p = c;
        }
        c = "";
    }
    output->push_back(dictionary[p]);
    if (codeNum > 65000) // max value of uint16
    {
        this->numberOf32Bits++;
    }
    else
    {
        this->numberOf16Bits++;
    }

    return output;
}

std::string* Compress::decode( std::deque<int>& encoded)
{
    std::string* result = nullptr;

    try
    {
        result = new std::string;
    }
    catch(const std::exception& e)
    {
        std::cout<<"couldnt allocate memory"<<std::endl;
        throw;
    }
    
    std::unordered_map<int, std::string> dictionary;
    this->loadDecodeDictionary(dictionary);

    std::string str = "";

    int firstIn = encoded.front() , num;
    std::string cW = dictionary[firstIn];
    std::string pW = "";
    pW += cW[0];
    *result += cW;

    int codeNumber = 256;

    int dequeueSize = encoded.size();
    for(int i = 0 ; i < dequeueSize - 1; ++i)
    {
        num = encoded[i+1];
        if(dictionary.find(num) == dictionary.end())
        {
            cW = dictionary[firstIn];
            cW = cW + pW;
        }
        else
        {
            cW = dictionary[num];
        }
        *result += cW;
        pW = "";
        pW += cW[0];
        dictionary[codeNumber] = dictionary[firstIn] + pW;
        codeNumber++;
        firstIn = num;
    }

    return result;
}

const int Compress::getNumberOf16Bits()const
{
    return this->numberOf16Bits;
}

const int Compress::getNumberOf32Bits()const
{
    return this->numberOf32Bits;
}

void Compress::setnumberOf16Bits(const int& value)
{
    this->numberOf16Bits = value;
}

void Compress::setnumberOf32Bits(const int& value)
{
    this->numberOf32Bits = value;
}

#endif