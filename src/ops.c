/*
* @brief Neural Network related Operations included in ops.c
* @author Ayushman Kumar
* @date 08-04-2022
*/

#include "../includes/ops.h"

/*
* @brief Pad images for a given pad size
* @param img : Image to be padded
* @param pad_size : Size of padding
* @param img+pad : Padded image
* @param rows : Number of rows in image
* @param cols : Number of columns in image
* @return void
*/
void pad_image(double *img, double *img_pad, int rows, int cols, int pad_size){
    int cols_pad = cols + (2 * pad_size);
    int rows_pad = rows + (2 * pad_size);
    int c, c_pad;

    // Central Pixels
    for (int i = pad_size; i < rows_pad - pad_size; i++) {
        for (int j = pad_size; j < cols_pad - pad_size; j++) {
            c = (i - pad_size) * cols + (j - pad_size);
            c_pad = i * cols_pad + j;
            img_pad[c_pad] = img[c];
        }
    }

    // Top and Bottom Rows
    for (int j = pad_size; j< cols_pad - pad_size; j++){
        for (int k = 0; k<pad_size; k++){
            
            c_pad = j + k * cols_pad;
            c = j - pad_size; 
            img_pad[c_pad] = img[c];
        }
    }

    // Left and Right Columns
    for (int i = pad_size; i<rows_pad - pad_size; i++){
        for (int k = 0; k<pad_size; k++){
            // Left Column
            c_pad = i * cols_pad + k;
            c = (i - pad_size) * cols;
            img_pad[c_pad] = img[c];

            // Right Column
            c_pad = i * cols_pad + (cols_pad - 1 - k);
            c = (i - pad_size) * cols + (cols - 1);
            img_pad[c_pad] = img[c];
        }
    }

    // Corner Pixels
    for (int k1 = 0; k1 < pad_size; k1++){
        for (int k2=0; k2 < pad_size; k2++){
            // Upper Left Corner
            c_pad = k1 * cols_pad + k2;
            img_pad[c_pad] = *(img);

            // Upper Right Corner
            c_pad = k1 * cols_pad + (cols_pad - 1 - k2);
            img_pad[c_pad] = *(img + (cols - 1));

            // Lower Left Corner
            c_pad = (rows_pad - 1 - k1) * cols_pad + k2;
            img_pad[c_pad] = *(img + ((rows - 1) *cols));

            // Lower Right Corner
            c_pad = (rows_pad - 1 - k1) * cols_pad + (cols_pad - 1 - k2);
            img_pad[c_pad] = *(img + ((rows - 1) *cols) + (cols - 1));
        }
    }
}

/*
* @brief Pad images for a given pad size
* @param img : Image to be padded
* @param pad_size : Size of padding
* @param img+pad : Padded image
* @param rows : Number of rows in image
* @param cols : Number of columns in image
* @return void
*/
void imfilter(double *img, double *kernel, double *img_fltr, int rows, int cols, int pad_size){
    int cols_pad = cols + 2 * pad_size;
	int rows_pad = rows + 2 * pad_size;
	int i, j, cnt, cnt_pad, c_kernel, k1, k2;
	double sum;

	double *img_pad = (double *)malloc(rows_pad * cols_pad * sizeof(double));
	pad_image(img, img_pad, rows, cols, pad_size);

	for (i = pad_size; i < rows_pad - pad_size; i++){
	    for (j = pad_size; j < cols_pad - pad_size; j++){
            cnt = (i - pad_size)*cols + (j - pad_size); 
            sum = 0;
            c_kernel = 0; 
            for (k1 = -pad_size; k1 <= pad_size; k1++){
                for (k2 = -pad_size; k2 <= pad_size; k2++){
                    {
                        cnt_pad = (i + k1)*cols_pad + j + k2; 
                        sum = sum + (*(img_pad + cnt_pad))*(*(kernel + c_kernel));
                        c_kernel++;
                    }
                }
            }
            *(img_fltr + cnt) = sum;
        }
    }

	free(img_pad);
	img_pad = NULL;
}

/*
* @brief Add Feature maps
* @param img_fltr_sum : pointer to current feature map
* @param img_fltr_crnt : pointer to the cumulutive feature map
* @param rows : Number of rows in image
* @param cols : Number of columns in image
* @return void
*/
void imadd(double *img_fltr_sum, double *img_fltr_crnt, int cols, int rows){
	int cnt = 0;
	for (int i = 0; i < rows;i++){
	    for (int j = 0; j < cols; j++){
            {
                cnt = i*cols + j;
                *(img_fltr_sum + cnt) = *(img_fltr_sum + cnt) + *(img_fltr_crnt + cnt);
            }
        }
    }
}

/*
* @brief Deconvolution Operation
* @param img_input : Input image
* @param img_output : Output image
* @param kernel : Kernel for convolution
* @param rows : Number of rows in image
* @param cols : Number of columns in image
* @param stride : Size of stride
* @return void
*/
void deconv(double *img_input, double *img_output, double *kernel, int cols, int rows, int stride){
	int border = 1;
	int fsize = 9;
	int rows_pad = rows + 2 * border;
	int cols_pad = cols + 2 * border;
	double *img_input_padded = (double *)malloc(rows_pad * cols_pad * sizeof(double));
	pad_image(img_input, img_input_padded, rows, cols, border);
	
	int rows_out_pad = rows_pad * stride;
	int cols_out_pad = cols_pad * stride;
	double *img_output_tmp = (double *)calloc((rows_out_pad + fsize - 1)* (cols_out_pad + fsize - 1), sizeof(double));
	double *kernel_modif = (double *)malloc(fsize * fsize * sizeof(double));

	int idx, idy;
	for (int i = 0; i < rows_pad; i++){
        for (int j = 0; j < cols_pad; j++)
        {
            int cnt_img = i*cols_pad + j;
            idx = i*stride;
            idy = j*stride;
            int cnt_img_output = idx*(cols_out_pad + fsize - 1) + idy;
            int cnt_kernel = 0;
            for (int k_r = 0; k_r < fsize; k_r++)
            {
            for (int k_c = 0; k_c < fsize; k_c++)
            {
                cnt_kernel = k_r*fsize + k_c;
                *(kernel_modif + cnt_kernel) = (*(kernel + cnt_kernel))*(*(img_input_padded + cnt_img));
                *(img_output_tmp + cnt_img_output + k_c) = *(img_output_tmp + cnt_img_output + k_c) + *(kernel_modif + cnt_kernel);
                
            }
            cnt_img_output = cnt_img_output + (cols_out_pad + fsize - 1);
            }            
        }
    }

	int rows_out = rows*stride;
	int cols_out = cols*stride;

	for (int i = 0; i < rows_out; i++){
        for (int j = 0; j < cols_out; j++){
            int i_tmp = i + ((fsize + 1) / 2) + stride*border - 1;
            int j_tmp = j + ((fsize + 1) / 2) + stride*border - 1;
            int cnt_img_out = i*cols_out + j;
            int cnt_img_out_tmp = i_tmp*(cols_out_pad + fsize - 1) + j_tmp; // (cols-pad+fsize-1) is the number of columns in the img_out_tmp
            *(img_output + cnt_img_out) = *(img_output_tmp + cnt_img_out_tmp);
        }
    }

	free(img_input_padded); 
    img_input_padded = NULL;
	free(img_output_tmp); 
    img_output_tmp = NULL;
	free(kernel_modif); 
    kernel_modif = NULL;
}