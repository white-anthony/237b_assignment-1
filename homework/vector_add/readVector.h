typedef enum errcode {SUCCESS, FAILURE} errcode;
errcode readVector(const char* filename, float** A, int* size);