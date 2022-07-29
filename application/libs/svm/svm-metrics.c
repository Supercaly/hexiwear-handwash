#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsvm/svm.h"

FILE *input_file;
struct svm_model *model;
struct svm_node *x;
int max_nr_attr = 64;

static char *line = NULL;
static int max_line_len;

static int (*info)(const char *fmt, ...) = &printf;

static char *readline(FILE *input)
{
    int len;

    if (fgets(line, max_line_len, input) == NULL)
        return NULL;

    while (strrchr(line, '\n') == NULL)
    {
        max_line_len *= 2;
        line = (char *)realloc(line, max_line_len);
        len = (int)strlen(line);
        if (fgets(line + len, max_line_len - len, input) == NULL)
            break;
    }
    return line;
}

void exit_input_error(int line_num)
{
    fprintf(stderr, "Wrong input format at line %d\n", line_num);
    exit(1);
}

void predict(void)
{
    int correct = 0;
    int total = 0;
    double error = 0;
    double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

    int svm_type = svm_get_svm_type(model);
    int nr_class = svm_get_nr_class(model);
    int j;

    int confusion_matrix[] = {0, 0, 0,
                              0, 0, 0,
                              0, 0, 0};

    max_line_len = 1024;
    line = (char *)malloc(max_line_len * sizeof(char));
    while (readline(input_file) != NULL)
    {
        int i = 0;
        double target_label, predict_label;
        char *idx, *val, *label, *endptr;
        int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

        label = strtok(line, " \t\n");
        if (label == NULL) // empty line
            exit_input_error(total + 1);

        target_label = strtod(label, &endptr);
        if (endptr == label || *endptr != '\0')
            exit_input_error(total + 1);

        while (1)
        {
            if (i >= max_nr_attr - 1) // need one more for index = -1
            {
                max_nr_attr *= 2;
                x = (struct svm_node *)realloc(x, max_nr_attr * sizeof(struct svm_node));
            }

            idx = strtok(NULL, ":");
            val = strtok(NULL, " \t");

            if (val == NULL)
                break;
            errno = 0;
            x[i].index = (int)strtol(idx, &endptr, 10);
            if (endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
                exit_input_error(total + 1);
            else
                inst_max_index = x[i].index;

            errno = 0;
            x[i].value = strtod(val, &endptr);
            if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
                exit_input_error(total + 1);

            ++i;
        }
        x[i].index = -1;

        predict_label = svm_predict(model, x);
        // fprintf(output, "%.17g\n", predict_label);

        if (predict_label == target_label)
            ++correct;
        error += (predict_label - target_label) * (predict_label - target_label);
        sump += predict_label;
        sumt += target_label;
        sumpp += predict_label * predict_label;
        sumtt += target_label * target_label;
        sumpt += predict_label * target_label;
        ++total;

        confusion_matrix[3 * (int)predict_label + (int)target_label]++;
    }
    int tp[3], tn[3], fp[3], fn[3];

    tp[0] = confusion_matrix[0];
    tn[0] = confusion_matrix[4] + confusion_matrix[5] +
            confusion_matrix[7] + confusion_matrix[8];
    fp[0] = confusion_matrix[3] + confusion_matrix[6];
    fn[0] = confusion_matrix[1] + confusion_matrix[2];

    tp[1] = confusion_matrix[4];
    tn[1] = confusion_matrix[0] + confusion_matrix[2] +
            confusion_matrix[6] + confusion_matrix[8];
    fp[1] = confusion_matrix[1] + confusion_matrix[7];
    fn[1] = confusion_matrix[3] + confusion_matrix[5];

    tp[2] = confusion_matrix[8];
    tn[2] = confusion_matrix[0] + confusion_matrix[1] +
            confusion_matrix[3] + confusion_matrix[4];
    fp[2] = confusion_matrix[2] + confusion_matrix[5];
    fn[2] = confusion_matrix[6] + confusion_matrix[7];

    info("Class 0\n");
    info("  tp: %d tn: %d fp:%d fn: %d\n", tp[0], tn[0], fp[0], fn[0]);
    info("Class 1\n");
    info("  tp: %d tn: %d fp:%d fn: %d\n", tp[1], tn[1], fp[1], fn[1]);
    info("Class 2\n");
    info("  tp: %d tn: %d fp:%d fn: %d\n", tp[2], tn[2], fp[2], fn[2]);

    double precision = 0.0,
           recall = 0.0,
           f1 = 0.0,
           accuracy = 0.0;

    for (int i = 0; i < 3; i++)
    {
        precision += (double)tp[i] / (tp[i] + fp[i]);
        recall += (double)tp[i] / (tp[i] + fn[i]);
        accuracy += (double)(tp[i] + tn[i]) / (tp[i] + tn[i] + fp[i] + fn[i]);
    }
    precision /= 3.0;
    recall /= 3.0;
    accuracy /= 3.0;
    f1 = 2 * ((precision * recall) / (precision + recall));

    info("Precision: %f\n", precision);
    info("Recall: %f\n", recall);
    info("F1 score: %f\n", f1);
    info("Accuracy: %f\n", accuracy);

    info("Accuracy = %g%% (%d/%d) (classification)\n",
         (double)correct / total * 100, correct, total);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: svm-metrics model input_file\n");
        exit(1);
    }

    const char *model_path = argv[1];
    const char *input_data_path = argv[2];

    printf("%s %s\n", model_path, input_data_path);

    if ((model = svm_load_model(model_path)), model == NULL)
    {
        fprintf(stderr, "cannot load model from file '%s'\n", model_path);
        exit(1);
    }

    if ((input_file = fopen(input_data_path, "r")), input_file == NULL)
    {
        fprintf(stderr, "cannot open input file '%s'\n", input_data_path);
        exit(1);
    }

    x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));
    predict();

    return 0;
}