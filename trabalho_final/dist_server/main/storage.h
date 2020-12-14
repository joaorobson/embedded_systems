#ifndef STORAGE_H
#define STORAGE_H

char* read_nvs(char* key);
void write_on_nvs(char* key, char* value);

#endif