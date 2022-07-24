#include "data_importer.h"

#include "log.h"

DataImporter::DataImporter() : _csv(DB_PATH) {}

DataImporter::~DataImporter() {}

bool DataImporter::init()
{
    if (_csv.open_csv())
    {
        // skip csv header
        char *header;
        header = _csv.read_next_row();
        free(header);
        return true;
    }
    return false;
}

bool DataImporter::next_chunk(RawSensorData *data, Label *label)
{
    char *row,
        *col;
    int label_acc = 0;

    for (int i = 0; i < FULL_WINDOW_SIZE; i++)
    {

        row = _csv.read_next_row();
        if (row == NULL)
        {
            log_info("DataImporter: chunk has less than %d rows\n",
                      FULL_WINDOW_SIZE);
            return false;
        }

        float values[8];
        for (int j = 0; j < 8; j++)
        {
            col = _csv.read_next_col(row);
            if (col == NULL)
            {
                log_info("DataImporter: encountered row with less elements than expected '%s'\n", row);
            }
            values[j] = (float)atof(col);
        }

        data->ax[i] = values[0];
        data->ay[i] = values[1];
        data->az[i] = values[2];
        data->gx[i] = values[3];
        data->gy[i] = values[4];
        data->gz[i] = values[5];
        label_acc += (int)values[6];

        free(row);
    }

    label_acc /= FULL_WINDOW_SIZE;
    *label = Label(label_acc);

    return true;
}