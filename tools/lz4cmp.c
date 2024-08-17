#include <stdio.h>
#include <stdlib.h>
#include <lz4.h>

#define CHUNK_SIZE 8192

int main(int argc, char *argv[]) {
    size_t allocated_size = CHUNK_SIZE;
    size_t total_size = 0;
    char *data = (char *)malloc(allocated_size);

    if (!data) {
        perror("error allocating memory");
        return 1;
    }

    size_t read_size;
    while ((read_size = fread(data + total_size, 1, CHUNK_SIZE, stdin)) > 0) {
        total_size += read_size;
        if (total_size + CHUNK_SIZE > allocated_size) {
            allocated_size *= 2;
            data = realloc(data, allocated_size);
            if (!data) {
                perror("error reallocating memory");
                return 1;
            }
        }
    }
    if (ferror(stdin)) {
        perror("error reading input stream");
        free(data);
        return 1;
    }
    size_t maxCmpLen = LZ4_compressBound(total_size);
    char *cmpData = (char *)malloc(maxCmpLen);
    if (!cmpData) {
        perror("error allocating memory for compression");
        free(data);
        return 1;
    }
    int cmpLen = LZ4_compress_default(data, cmpData, total_size, maxCmpLen);
    if (cmpLen <= 0) {
        fprintf(stderr, "compression failed\n");
        return 1;
    } else {
        fwrite(cmpData, 1, cmpLen, stdout);

    }
    free(data);
    free(cmpData);
    return cmpLen <= 0;
}
