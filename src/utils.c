/*
* @brief utils file contains all the unitlities functions
* @author Ayushman Kumar
* @date 08-04-2022
*/

#include "../includes/utils.h";

/*
* @brief Returns the Maximum between 2 double numbers
* @param a,b double numbers
* @return Maximum of a and b
*/
double Max(double a, double b)
{
    return a > b ? a : b;
}

/*
* @brief Returns the Minimum between 2 double numbers
* @param a,b double numbers
* @return Minimum of a and b
*/
double Min(double a, double b)
{
    return a < b ? a : b;
}

/*
* @brief Converts a double image to uint8 image
* @param img double image, output uint8 image, rows, cols of the image   
* @return void
*/
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