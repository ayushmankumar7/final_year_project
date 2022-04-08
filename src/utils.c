#include "../includes/utils.h";

double Max(double a, double b)
{
    return a > b ? a : b;
}

double Min(double a, double b)
{
    return a < b ? a : b;
}

void double_to_uint8(double *img, unsigned char *uint8_img, int cols, int rows){
    int c;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {   
            c = i * cols + j;
            if (*(img + c) < 0){
                *(uint8_img + c) = 0;
            }
            else if (*(img + c) > 255){
                *(uint8_img + c) = 255;
            }
            
            for (int k = 0; k < 255; k++){
                if (*(img + c) >= k && *(img + c) < (k + 0.5)){
                    *(uint8_img + c) = k;
                }
                if (*(img + c) >= (k + 0.5) && *(img + c) < (k + 1)){
                    *(uint8_img + c) = k + 1;
                }
            }
        }

    }
}