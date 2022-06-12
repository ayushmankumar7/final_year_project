#ifndef UTILS_H
#define UTILS_H


#ifdef __linux__
double Max(double a, double b);
double Min(double a, double b);

#elif _WIN32
__declspec(dllexport) double Max(double a, double b);
__declspec(dllexport) double Min(double a, double b);

#endif

void double_to_uint8(double *img, unsigned char *uint8_img, int cols, int rows);

#endif