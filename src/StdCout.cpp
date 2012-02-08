
#include <iomanip>
#include <cstring>

#include "StdCout.hpp"

#ifdef COMPRESS_OUTPUT
#include <zlib.h>
//#define DEFAULT_BUFFER_SIZE 8192
#define DEFAULT_BUFFER_SIZE 512
#endif // #ifdef COMPRESS_OUTPUT


File_And_Screen_Stream std_cout;

// **************************************************************
inline std::ofstream & Get_Stream(void *logfile_fh_stream)
{
    return (*((std::ofstream *) logfile_fh_stream));
}

// **************************************************************
void log(const char *const format, ...)
{
    va_list args;
    va_start(args, format);

    char string_to_log[2048];
    int result = vsprintf(string_to_log, format, args);
    if (result < 0)
    {
        std_cout << "Couldn't save log! Aborting.\n" << std::flush;
        abort();
    }
    std_cout << string_to_log;
}

// **************************************************************
File_And_Screen_Stream::File_And_Screen_Stream(void)
{
    logfile_fh_stream = NULL;
}

// **************************************************************
File_And_Screen_Stream::~File_And_Screen_Stream(void)
/**
 * Destructor. Close the file if open.
 */
{
    Flush();

#ifdef COMPRESS_OUTPUT
    if (logfile_fh_stream != NULL)
        gzclose((gzFile *) logfile_fh_stream);
#else
    if (logfile_fh_stream != NULL)
    {
        if (Get_Stream(logfile_fh_stream).is_open())
            Get_Stream(logfile_fh_stream).close();
        delete (std::ofstream *) logfile_fh_stream;
    }
#endif // #ifdef COMPRESS_OUTPUT

    logfile_fh_stream = NULL;
}

// **************************************************************
void File_And_Screen_Stream::Save_To_File()
{
    if (logfile_fh_stream != NULL)
    {
#ifdef COMPRESS_OUTPUT
        const int error_code = gzwrite(logfile_fh_stream, logfile_string.str().c_str(), (unsigned int) logfile_string.str().size());
        if (logfile_string.str().size() != 0)
            assert(error_code != 0);
#else // #ifdef COMPRESS_OUTPUT
        Get_Stream(logfile_fh_stream) << logfile_string.str();
#endif // #ifdef COMPRESS_OUTPUTt);
    }

    // Clear the log stream for next time
    logfile_string.str(std::string());
}

// **************************************************************
File_And_Screen_Stream & File_And_Screen_Stream::operator<<(std::ostream& (*pfun)(std::ostream&))
/**
 * This allow sending "std::endl" to the stream.
 */
{
    pfun(std::cout);
    pfun(logfile_string);

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
    gzFile tmp_file;
    if (append)
        tmp_file = gzopen(filename.c_str(), "ab");
    else
        tmp_file = gzopen(filename.c_str(), "wb");
    assert(tmp_file != NULL);
    gzbuffer(tmp_file, DEFAULT_BUFFER_SIZE);
    logfile_fh_stream = (void *) tmp_file;

    logfile_string   << "Opening file " << filename << "...\n" << std::flush;
    Save_To_File();
#else // #ifdef COMPRESS_OUTPUT
    logfile_fh_stream = (void *) new std::ofstream;
    assert(logfile_fh_stream != NULL);
    if (append)
    {
        Get_Stream(logfile_fh_stream).open(filename.c_str(), std::ios_base::app);
    }
    else
    {
        Get_Stream(logfile_fh_stream).open(filename.c_str(), std::ios_base::out);
    }
    assert(Get_Stream(logfile_fh_stream).is_open());

    Get_Stream(logfile_fh_stream) << "Opening file " << filename << "...\n" << std::flush;
#endif // #ifdef COMPRESS_OUTPUT
}

// **************************************************************
std::streamsize File_And_Screen_Stream::precision(const std::streamsize p)
/**
 * Set precision of stream.
 */
{
// #ifdef COMPRESS_OUTPUT
// #else // #ifdef COMPRESS_OUTPUT
//     Get_Stream(logfile_fh_stream).precision(p);
// #endif // #ifdef COMPRESS_OUTPUT
    Format(p+3, p, 'f');
    return std::cout.precision(p);
}

// **************************************************************
void File_And_Screen_Stream::Flush()
/**
 * Flush both file and screen output.
 */
{

    if (logfile_fh_stream != NULL)
    {
#ifdef COMPRESS_OUTPUT
        gzflush(logfile_fh_stream, Z_FINISH);
#else // #ifdef COMPRESS_OUTPUT
        Get_Stream(logfile_fh_stream) << std::flush;
#endif // #ifdef COMPRESS_OUTPUT
    }
    std::cout  << std::flush;
}

// **************************************************************
template <class T>
void Clear_Stream_Format(T &stream)
{
    stream << std::resetiosflags(std::ios_base::floatfield);
    stream << std::right;
    stream << std::noshowpos;
    stream << std::setfill(' ');
}

// **************************************************************
void File_And_Screen_Stream::Clear_Format()
{
    Clear_Stream_Format(std::cout);
    Clear_Stream_Format(logfile_string);
}

// **************************************************************
template <class T>
void Format_Stream(T &stream, const int width,
                              const int nb_after_dot,
                              const char type,
                              const char justify,
                              const char fill)
{
    if (width != 0)
    {
        stream << std::setw(width);
    }

    if (type != 'd')
    {
        // If not an integer, set the precision.
        stream << std::setprecision(nb_after_dot);

        if (type == 'f')
        {
            stream << std::fixed;
        }
        else if (type == 'e')
        {
            stream << std::scientific;
        }
    }
    if (justify == 'r')
    {
        stream << std::right;
    } else if (justify == '+')
    {
        stream << std::showpos;
    } else if (justify == 'l' || justify == '-')
    {
        stream << std::left;
    }
    stream << std::setfill(fill);
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

    Format_Stream(std::cout,         width, nb_after_dot, type, justify, fill);
    Format_Stream(logfile_string, width, nb_after_dot, type, justify, fill);
}

// ********** End of file ***************************************

