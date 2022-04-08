#ifndef
#define MODEL_H 

void FSRCNN(double *img_hr, double *img_lr, int rows, int cols, int scale);
void PReLU(double *img_fltr, int rows, int cols, double bias, double prelu_coeff);

#endif