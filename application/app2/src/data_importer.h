#ifndef DATA_IMPORTER_H_
#define DATA_IMPORTER_H_

#include "FileSystem.h"
#include "csv_parser.h"
#include "label.h"
#include "mbed.h"
#include "raw_sensor_data.h"

class DataImporter
{
public:
    DataImporter();
    ~DataImporter();

    bool init();
    bool next_chunk(RawSensorData *data, Label *label);

private:
    CsvParser _csv;
};

#endif // DATA_IMPORTER_H_