
#include <iomanip>
#include <cstring>

#include "StdCout.hpp"
#include "Version.hpp"

#ifdef COMPRESS_OUTPUT
#include <zlib.h>
#define DEFAULT_BUFFER_SIZE 8192
#endif // #ifdef COMPRESS_OUTPUT


File_And_Screen_Stream std_cout;
FILE *logfile;

// **************************************************************
void Open_Log_File(std::string filename, const bool append)
{
    if (append)
        logfile = fopen(filename.c_str(), "wa");
    else
        logfile = fopen(filename.c_str(), "w");
    assert(logfile != NULL);

    logging(("Opening file "+filename+"...\n").c_str());

    Log_Git_Info();
}

// **************************************************************
void logging(const char *const format, ...)
{
    {
        va_list args;
        va_start(args, format);
        int result = vfprintf(logfile, format, args);
        if (result < 0)
        {
            printf("Couldn't save log! Aborting.\n");
            abort();
        }
        va_end(args);
    }
    {
        va_list args;
        va_start(args, format);
        int result = vprintf(format, args);
        if (result < 0)
        {
            printf("Couldn't save log! Aborting.\n");
            abort();
        }
        va_end(args);
    }

    fflush(logfile);
}

// **************************************************************
void log(const char *const format, ...)
{
    va_list args;
    va_start(args, format);

    std_cout.string_to_log[0] = '\0';
    int result = vsprintf(std_cout.string_to_log, format, args);
    if (result < 0)
    {
        std_cout << "Couldn't save log! Aborting.\n" << std::flush;
        abort();
    }
    std_cout << std_cout.string_to_log;
}

// **************************************************************
File_And_Screen_Stream::File_And_Screen_Stream(void)
{
    // Clear the string
    memset(string_to_log, 0, 1000*sizeof(char));
    filepointer   = NULL;
    compressed_fh = NULL;
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

#ifdef COMPRESS_OUTPUT
    if (compressed_fh != NULL)
        gzclose((gzFile *) compressed_fh);
    compressed_fh = NULL;
#endif // #ifdef COMPRESS_OUTPUT
}

// **************************************************************
void File_And_Screen_Stream::Save(const std::string string_to_save)
{
#ifdef COMPRESS_OUTPUT
    const int error_code = gzwrite(compressed_fh, string_to_save.c_str(), string_to_save.size());
    assert(error_code != 0);
    gzflush(compressed_fh, Z_FINISH);
#else // #ifdef COMPRESS_OUTPUT
    filestream << string_to_save;
#endif // #ifdef COMPRESS_OUTPUTt);
    std::cout << string_to_save;
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
#ifdef COMPRESS_OUTPUT
    filename += ".gz";
#endif // #ifdef COMPRESS_OUTPUT

    std::cout << "Opening file " << filename << "...\n" << std::flush;
#ifdef COMPRESS_OUTPUT
    gzFile tmp_file = gzopen(filename.c_str(), "wb");
    gzbuffer(tmp_file, DEFAULT_BUFFER_SIZE);
    compressed_fh = (void *) tmp_file;
#else // #ifdef COMPRESS_OUTPUT
    if (append)
        filestream.open(filename.c_str(), std::ios_base::app);
    else
        filestream.open(filename.c_str(), std::ios_base::out);
    assert(filestream.is_open());
#endif // #ifdef COMPRESS_OUTPUT

    filepointer = fopen(filename.c_str(), "wa");
    assert(filepointer != NULL);

    filestream << "Opening file " << filename << "...\n" << std::flush;

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

#ifdef COMPRESS_OUTPUT
    gzflush(compressed_fh, Z_FINISH);
#else // #ifdef COMPRESS_OUTPUT
    filestream << std::flush;
#endif // #ifdef COMPRESS_OUTPUT
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
