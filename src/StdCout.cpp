
#include <iomanip>
#include <cstring>

#include "StdCout.hpp"
#include "Version.hpp"

File_And_Screen_Stream std_cout;

// **************************************************************
void log(const char *const format, ...)
{
    va_list args;
    va_start(args, format);

    /*
    vprintf(format, args);

    if (std_cout.filepointer != NULL)
        vfprintf(std_cout.filepointer, format, args);
    */
    std_cout.string_to_log[0] = '\0';
    int result = vsprintf(std_cout.string_to_log, format, args);
    if (result < 0)
    {
        std_cout << "Couldn't save log! Aborting.\n" << std::flush;
        abort();
    }
    std_cout << std_cout.string_to_log;

    va_end(args);
}

// **************************************************************
File_And_Screen_Stream::File_And_Screen_Stream(void)
{
    // Clear the string
    memset(string_to_log, 0, 1000*sizeof(char));
}

// **************************************************************
File_And_Screen_Stream::~File_And_Screen_Stream(void)
/**
 * Destructor. Close the file if open.
 */
{
    Flush();

    if (filestream.is_open())
        filestream.close();

    if (filepointer != NULL)
        fclose(filepointer);
    filepointer = NULL;
}

// **************************************************************
File_And_Screen_Stream & File_And_Screen_Stream::operator<<(std::ostream& (*pfun)(std::ostream&))
/**
 * This allow sending "std::endl" to the stream.
 */
{
    pfun(filestream);
    pfun(std::cout);
    return *this;
}

// **************************************************************
void File_And_Screen_Stream::open(std::string filename, const bool append)
/**
 * Open file.
 */
{
    std::cout << "Opening file " << filename << "...\n";
    if (append)
        filestream.open(filename.c_str(), std::ios_base::app);
    else
        filestream.open(filename.c_str(), std::ios_base::out);
    assert(filestream.is_open());

    filepointer = fopen(filename.c_str(), "wa");
    assert(filepointer != NULL);

    Log_Git_Info();
}

// **************************************************************
std::streamsize File_And_Screen_Stream::precision(const std::streamsize p)
/**
 * Set precision of stream.
 */
{
    filestream.precision(p);
    return std::cout.precision(p);
}

// **************************************************************
void File_And_Screen_Stream::Flush()
/**
 * Flush both file and screen output.
 */
{
    filestream << std::flush;
    std::cout  << std::flush;
}

// **************************************************************
void File_And_Screen_Stream::Clear_Format()
{
//     std::cout  << std::resetiosflags(std::ios_base::width);
//     filestream << std::resetiosflags(std::ios_base::width);

    //std::cout  << std::resetiosflags(std::ios::precision);
//     std::cout.unsetf(std::ios_base::precision);
//     filestream << std::resetiosflags(std::ios_base::precision);

//     std::cout  << std::resetiosflags(); // Fixed? Scientific?
//     filestream << std::resetiosflags();
//     std::cout.floatfield = none;
//     filestream.floatfield = none;
    std::cout  << std::resetiosflags(std::ios_base::floatfield);
    filestream << std::resetiosflags(std::ios_base::floatfield);

    std::cout  << std::right;
    filestream << std::right;

    std::cout  << std::noshowpos;
    filestream << std::noshowpos;

    std::cout  << std::setfill(' ');
    filestream << std::setfill(' ');


}

// **************************************************************
void File_And_Screen_Stream::Format(const int width,
                                    const int nb_after_dot,
                                    const char type,
                                    const char justify,
                                    const char fill)
/**
 * Set the output format. Similar to printf() format.
 * @param width         Output's width (in characters)
 * @param nb_after_dot  Number of digits after decimal
 * @param type          Character describing the type
 * @param justify       Left (l) or right (r) justification.
 *                      Default to right.
 * @param fill          Filling character (default to space)
 *
 * Examples:
 *      Format(3, 0, 'd')       equivalent to printf("%3d", [...])
 *      Format(0, 4, 'f')       equivalent to printf("%.4f", [...])
 *      Format(11, 4, 'f')      equivalent to printf("%11.4f", [...])
 *      Format(12, 6, 'g', 'l') equivalent to printf("%-12.6g", [...])
 *      Format(12, 6, 'g', '-') equivalent to printf("%-12.6g", [...])
 *      Format(12, 6, 'g', '+') equivalent to printf("%+12.6g", [...])
 */
{
    Clear_Format();

    if (width != 0)
    {
        std::cout  << std::setw(width);
        filestream << std::setw(width);
    }

    if (type != 'd')
    {
        // If not an integer, set the precision.
        std::cout  << std::setprecision(nb_after_dot);
        filestream << std::setprecision(nb_after_dot);

        if (type == 'f')
        {
            std::cout  << std::fixed;
            filestream << std::fixed;
        }
        else if (type == 'e')
        {
            std::cout  << std::scientific;
            filestream << std::scientific;
        }
    }
    if (justify == 'r')
    {
        std::cout  << std::right;
        filestream << std::right;
    } else if (justify == '+')
    {
        std::cout  << std::showpos;
        filestream << std::showpos;
    } else if (justify == 'l' || justify == '-')
    {
        std::cout  << std::left;
        filestream << std::left;
    }
    std::cout  << std::setfill(fill);
    filestream << std::setfill(fill);
}

// ********** End of file ***************************************
