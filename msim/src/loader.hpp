#ifndef LOADER_H
#define LOADER_H

#include <cstdint>

int load(const char *filename, uint8_t **data);
void save(const char *filename, uint8_t **data, int size);

#endif
