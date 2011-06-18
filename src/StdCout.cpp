
#include <iomanip>
#include <cstring>

#include "StdCout.hpp"
#include "Version.hpp"

#ifdef COMPRESS_OUTPUT
#include <zlib.h>
#define DEFAULT_BUFFER_SIZE 8192
#endif // #ifdef COMPRESS_OUTPUT


File_And_Screen_Stream std_cout;

{
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
    logfile_fh = NULL;
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

#ifdef COMPRESS_OUTPUT
    if (logfile_fh != NULL)
        gzclose((gzFile *) logfile_fh);
    logfile_fh = NULL;
#endif // #ifdef COMPRESS_OUTPUT
}

// **************************************************************
void File_And_Screen_Stream::Save_To_File()
{
#ifdef COMPRESS_OUTPUT
    const int error_code = gzwrite(logfile_fh, logfile_stream.str().c_str(), logfile_stream.str().size());
    gzflush(logfile_fh, Z_FINISH);
    if (logfile_stream.str().size() != 0)
        assert(error_code != 0);
    logfile_stream.str(std::string());
#else // #ifdef COMPRESS_OUTPUT
    filestream << logfile_stream.str();
#endif // #ifdef COMPRESS_OUTPUTt);
}

// **************************************************************
File_And_Screen_Stream & File_And_Screen_Stream::operator<<(std::ostream& (*pfun)(std::ostream&))
/**
 * This allow sending "std::endl" to the stream.
 */
{
    pfun(filestream);
    pfun(std::cout);
    pfun(logfile_stream);

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
    assert(tmp_file != NULL);
    gzbuffer(tmp_file, DEFAULT_BUFFER_SIZE);
    logfile_fh = (void *) tmp_file;
#else // #ifdef COMPRESS_OUTPUT
    if (append)
        filestream.open(filename.c_str(), std::ios_base::app);
    else
        filestream.open(filename.c_str(), std::ios_base::out);
    assert(filestream.is_open());
#endif // #ifdef COMPRESS_OUTPUT

    filestream << "Opening file " << filename << "...\n" << std::flush;

    Log_Git_Info();
}

// **************************************************************
void File_And_Screen_Stream::Flush()
/**
 * Flush both file and screen output.
 */
{

#ifdef COMPRESS_OUTPUT
    gzflush(logfile_fh, Z_FINISH);
#else // #ifdef COMPRESS_OUTPUT
    filestream << std::flush;
#endif // #ifdef COMPRESS_OUTPUT
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
    Clear_Stream_Format(filestream);
    Clear_Stream_Format(logfile_stream);
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
    Format_Stream(filestream,        width, nb_after_dot, type, justify, fill);
    Format_Stream(logfile_stream, width, nb_after_dot, type, justify, fill);
}

// ********** End of file ***************************************

