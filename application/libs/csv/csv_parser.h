#ifndef CSV_H_
#define CSV_H_

#include "FileSystem.h"
#include "mbed.h"

class CsvParser
{
public:
    // Creates an instance of CsvParser for given
    // filename and using given delimiter.
    CsvParser(const char *filename, char delim = ',');
    ~CsvParser();

    // Open the given csv file for reading.
    bool open_csv();

    // Read a row from given csv file.
    // A row is returned as a null-terminated string where the new-line 
    // character is stripped out of the string.
    // Note that the row string is allocated in memory and must be freed by
    // the user application.
    char *read_next_row();

    // Read a value from given csv row.
    // A value is everything up to given delimiter and
    // is returned as a null-terminated string.
    // The delimiter is stripped out of the value
    char *read_next_col(char *row);

private:
    const char *_filename;
    FILE *_csv_file;
    char _delim;

    // This variable keeps track of the last value in a
    // row string being processed; if it's null it means
    // we are processing a new now.
    char *_prev_row_ptr;
};

#endif // CSV_H_