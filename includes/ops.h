#pragma ONCE
#include <stdio.h>
#include <stdlib.h>

#ifndef OPS_H
#define OPS_H

void deconv(double *img_input, double *img_output, double *kernel, int cols, int rows, int stride);
void pad_image(double *img, double *img_pad, int rows, int cols, int padsize);
void imfilter(double *img, double *kernel, double *img_fltr, int rows, int cols, int padsize);
void imadd(double *img_fltr_crnt, double *img_fltr_prev, int cols, int rows);

#endif