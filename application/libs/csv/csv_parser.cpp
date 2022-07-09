#include "csv_parser.h"
#include "getline.h"

CsvParser::CsvParser(const char *filename, char delim) : _filename(filename),
                                                         _delim(delim),
                                                         _csv_file(nullptr),
                                                         _prev_row_ptr(nullptr) {}

CsvParser::~CsvParser()
{
    _prev_row_ptr = nullptr;
    if (_csv_file != nullptr)
    {
        fclose(_csv_file);
        _csv_file = nullptr;
        FileSystem::get_default_instance()->unmount();
    }
}

bool CsvParser::open_csv()
{
    if (_csv_file != nullptr)
    {
        // TODO: Replace with log_error when it will be available
        printf("CsvParser: file already open!\n");
        return false;
    }

    FileSystem *fs = FileSystem::get_default_instance();
    if (fs->mount(BlockDevice::get_default_instance()) < 0)
    {
        // TODO: Replace with log_error when it will be available
        printf("CsvParser: error mounting filesystem!\n");
        return false;
    }

    _csv_file = fopen(_filename, "r");
    if (_csv_file == nullptr)
    {
        // TODO: Replace with log_error when it will be available
        printf("CsvParser: error opening file '%s': %s(%d)\n",
               _filename, strerror(errno), errno);
        return false;
    }

    return true;
}

char *CsvParser::read_next_row()
{
    char *line = NULL;
    size_t line_buff;

    // reset the row ptr
    _prev_row_ptr = nullptr;

    // read a line from file
    ssize_t len = _getline(&line, &line_buff, _csv_file);
    if (len < 0)
    {
        // cannot read a line
        return NULL;
    }
    // strip new-line character from the end
    line[len - 1] = '\0';

    if (!*line)
    {
        return NULL;
    }

    return line;
}

char *CsvParser::read_next_col(char *row)
{
    // resume from where we left in the row or point to the start of a new row
    char *p = _prev_row_ptr ? _prev_row_ptr : row;
    char *b = p;

    // find the first instance of delim inside the row
    for (; *p; p++)
    {
        if (*p == _delim)
        {
            break;
        }
    }
    // create a null-terminated string stripping the delimiter
    *p = '\0';
    _prev_row_ptr = p + 1;

    // if the start and end of the column is the same return nothing
    if (b == p)
    {
        return NULL;
    }

    return b;
}