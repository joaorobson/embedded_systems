#include <time.h>


char* get_datetime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *datetime = (char*)malloc(30 * sizeof(char));

    sprintf(datetime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return datetime;
}


void writeOnCSV(float TI, float TE, float TR)
{
    FILE *fp;
    fp = fopen("sensors_data.csv", "a+");

    // Check if file has already a header
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);

    if (size == 0) {
        fprintf(fp,"Data/Hora, Temp Int, Temp Ext, Temp Ref");
    }

    fprintf(fp, "\n%s, %.2lf, %.2lf, %.2lf", get_datetime(), TI, TE, TR);
    fclose(fp);

}
