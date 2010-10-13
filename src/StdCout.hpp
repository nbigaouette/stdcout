#ifndef INC_STD_OUT_hpp
#define INC_STD_OUT_hpp

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>

// #include "Assert.hpp"
#include <cassert>

class File_And_Screen_Stream
{
    public:
        std::ofstream filestream;

        ~File_And_Screen_Stream(void);

        File_And_Screen_Stream & operator<<(std::ostream& (*pfun)(std::ostream&));

        void open(std::string filename, const bool append = false);

        std::streamsize precision(const std::streamsize p);

        void Flush();

        void Format(const int width, const int nb_after_dot, const char type, const char justify='r', const char fill=' ');
};

template <class T>
File_And_Screen_Stream & operator<<(File_And_Screen_Stream& st, const T val)
{
    st.filestream << val;
    std::cout     << val;
    return st;
}

extern File_And_Screen_Stream std_cout;

// **************************************************************
template <class T>
std::string FormattedInteger(T value, std::string format)
{
    std::string formatted_string;
    std::ostringstream oss;

    assert(format[0] == '%');

    char type = *(format.end()-1);
    assert(type == 'd');

    // Get rid of first and last characters
    format.erase(format.begin()+0);
    format.erase(format.end()-1);

    if (format.size() != 0)
    {
        if (format[0] == '0')
        {
            // Set filling to zeros and get rid of the first character
            oss << std::setfill('0');
            format.erase(format.begin()+0);
        }
        // Set width
        oss << std::setw(atoi(format.c_str()));
    }

    oss << value;

    formatted_string = std::string(oss.str());

    return formatted_string;
}

// http://www.codeproject.com/KB/stl/ostringstream.aspx
#define leftprec(x,y) (x - (((int)floor(x) - (((int)floor(x))% ((int)(pow(10,y)))))))

#endif // INC_STD_OUT_hpp

// ********** End of file ***************************************
