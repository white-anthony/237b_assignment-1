#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "readVector.h"

errcode readVector(const char* filename, float** A, int* size) {
    // Open file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: File %s could not be opened.\n", filename);
        return FAILURE;
    }
    // Read vector size
    if (fscanf(file, "%i", size) != 1) {
        fprintf(stderr, "ERROR: Could not read vector size from file %s.\n", filename);
        return FAILURE;
    }

    // Fill in vector
    *A = (float *)malloc(*size * sizeof(float));
    for (int i = 0; i < *size; i++) {
        if (fscanf(file, "%f", &(*A)[i]) != 1) {
            fprintf(stderr, "ERROR: Could not read element %i from file %s.\n", i, filename);
            return FAILURE;
        }
    }
    
    // Close file
    fclose(file);
    return SUCCESS;
}
