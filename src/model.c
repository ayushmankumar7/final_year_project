#include "../includes/model.h"

void PReLU(double *img_fltr,int rows, int cols, double bias, double prelu_coeff)
{
	int cnt = 0;
	for (int i = 0; i < rows;i++)
	for (int j = 0; j < cols; j++)
	{
		cnt = i*cols + j;
		*(img_fltr + cnt) = Max(*(img_fltr + cnt) + bias, 0) + prelu_coeff * Min(*(img_fltr + cnt) + bias, 0);
	}
}

void FSRCNN(double *img_hr, double *img_lr, int rows, int cols, int scale)
{
	// General Settings
	int num_layers = 8;

	/////////// Convolution1 -------- Layer1
	// Reading weights of first layer
	FILE *weights_layer1_ptr;
	weights_layer1_ptr = fopen(WEIGHT1, "r");
	
    if (weights_layer1_ptr == NULL) { 
        printf("Error in the reading weights of first layer\n");
    };

	double weights_layer1[1400];
	
    for (int i = 0; i < 1400; i++)
	{
		fscanf(weights_layer1_ptr, "%lf", &weights_layer1[i]);
	}
	fclose(weights_layer1_ptr);
	// Reading biases of first layer
	FILE *biases_layer1_ptr;
	biases_layer1_ptr = fopen(BIAS1, "r");
	if (biases_layer1_ptr == NULL) { printf("Error in the reading biases of first layer\n"); };
	double biases_layer1[56];
	for (int i = 0; i < 56; i++)
	{
		fscanf(biases_layer1_ptr, "%lf", &biases_layer1[i]);
	}
	fclose(biases_layer1_ptr);
	// other parameters
	int filtersize = 25; //5X5
	int patchsize = 5;
	int padsize = (patchsize - 1) / 2;
	int num_filters = 56;
	double prelu_coeff_layer1 = -0.8986;

	// Convolution
	double *img_fltr_1 = (double *)malloc(rows * cols * num_filters * sizeof(double));
	double *kernel = (double *)malloc(filtersize*sizeof(double));

	double *img_fltr_p1 = img_fltr_1; // Pointer to img_fltr1 ==>> Using this way to be able to shift it to access data

	int cnt_weight = 0;
	
	double bias_tmp;

	for (int i = 0; i < num_filters; i++)
	{
		// reading corresponding weights to kernel pointer
		for (int cnt_kernel = 0; cnt_kernel < filtersize; cnt_kernel++)
		{
			*(kernel + cnt_kernel) = weights_layer1[cnt_weight + cnt_kernel];
		}
		cnt_weight = cnt_weight + filtersize;
		imfilter(img_lr, kernel, img_fltr_p1, rows, cols, padsize);

		bias_tmp = biases_layer1[i];
		PReLU(img_fltr_p1, rows, cols, bias_tmp, prelu_coeff_layer1);

		img_fltr_p1 = img_fltr_p1 + cols*rows;
	}

	/////////// Convolution2 ------------------- Layer 2~7

	/////////// Layer2
	// Reading weights of 2nd layer
	FILE *weights_layer2_ptr;
	weights_layer2_ptr = fopen(WEIGHT2, "r");
	if (weights_layer2_ptr == NULL) { printf("Error in the reading weights of 2nd layer\n"); };
	// Note: weights must be saved in a way which that corresponding weights of each channel can be read by pointer concept ==>> for this layer 12X56 matrix is reshaped to (12X56)*1 vector
	double weights_layer2[672];
	for (int i = 0; i < 672; i++)
	{
		fscanf(weights_layer2_ptr, "%lf", &weights_layer2[i]);
	}
	fclose(weights_layer2_ptr);
	// Reading biases of 2nd layer
	FILE *biases_layer2_ptr;
	biases_layer2_ptr = fopen(BIAS2, "r");
	if (biases_layer2_ptr == NULL) { printf("Error in the reading biases of 2nd layer\n"); };
	double biases_layer2[12];
	for (int i = 0; i < 12; i++)
	{
		fscanf(biases_layer2_ptr, "%lf", &biases_layer2[i]);
	}
	fclose(biases_layer2_ptr);
	// Other parameters
	int filtersize2 = 1; //1X1
	int patchsize2 = 1;
	int padsize2 = (patchsize2 - 1) / 2;
	int num_filters2 = 12;
	int num_channels2 = 56;
	double prelu_coeff_layer2 = 0.3236;
	// Convolution
	double *img_fltr_2 = (double *)calloc(rows * cols * num_filters2 , sizeof(double)); // use calloc to initialize all variables to zero
	double *img_fltr_2_tmp = (double *)malloc(rows * cols * sizeof(double));
	double *kernel2 = (double *)malloc(filtersize2*sizeof(double));
	double *img_fltr_p2 = img_fltr_2; // Pointer to img_fltr2
	

	cnt_weight = 0;

	for (int i = 0; i < num_filters2; i++)
	{
		img_fltr_p1 = img_fltr_1; // Return pointer to the first of array which contains feature map of previous layer
		for (int j = 0; j < num_channels2; j++)
		{
			// reading corresponding weights to kernel
			for (int cnt_kernel = 0; cnt_kernel < filtersize2; cnt_kernel++)
			{
				*(kernel2 + cnt_kernel) = weights_layer2[cnt_weight + cnt_kernel];
			}

			imfilter(img_fltr_p1, kernel2, img_fltr_2_tmp, rows, cols, padsize2);
			imadd(img_fltr_p2, img_fltr_2_tmp, cols, rows);

			cnt_weight = cnt_weight + filtersize2;
			img_fltr_p1 = img_fltr_p1 + rows*cols;
		}
		bias_tmp = biases_layer2[i];
		PReLU(img_fltr_p2, rows, cols, bias_tmp, prelu_coeff_layer2);
		img_fltr_p2 = img_fltr_p2 + rows*cols;
	}

	free(img_fltr_1);
	img_fltr_1 = NULL;
	free(kernel);
	kernel = NULL;
	free(img_fltr_2_tmp);
	img_fltr_2_tmp = NULL;
	
	/////////// Layer3
	// Reading weights of 3rd layer
	FILE *weights_layer3_ptr;
	weights_layer3_ptr = fopen(WEIGHT3, "r");
	if (weights_layer3_ptr == NULL) { printf("Error in the reading weights of 3rd layer\n"); };
	double weights_layer3[1296];
	for (int i = 0; i < 1296; i++)
	{
		fscanf(weights_layer3_ptr, "%lf", &weights_layer3[i]);
	}
	fclose(weights_layer3_ptr);
	// Reading biases of 3rd layer
	FILE *biases_layer3_ptr;
	biases_layer3_ptr = fopen(BIAS3, "r");
	if (biases_layer3_ptr == NULL) { printf("Error in the reading biases of 3rd layer\n"); };
	double biases_layer3[12];
	for (int i = 0; i < 12; i++)
	{
		fscanf(biases_layer3_ptr, "%lf", &biases_layer3[i]);
	}
	fclose(biases_layer3_ptr);
	// Other parameters
	int filtersize3 = 9; //3X3
	int patchsize3 = 3;
	int padsize3 = (patchsize3 - 1) / 2;
	int num_filters3 = 12;
	int num_channels3 = 12;
	double prelu_coeff_layer3 = 0.2288;
	// Convolution
	double *img_fltr_3 = (double *)calloc(rows * cols * num_filters3 , sizeof(double));
	double *kernel3 = (double *)malloc(filtersize3*sizeof(double));
	double *img_fltr_p3 = img_fltr_3; // Pointer to img_fltr2
	double *img_fltr_3_tmp = (double *)malloc(rows * cols * sizeof(double));

	cnt_weight = 0;
	
	for (int i = 0; i < num_filters3; i++)
	{
		img_fltr_p2 = img_fltr_2; // Return pointer to the first cell of array which contains feature maps of previous layer
		for (int j = 0; j < num_channels3; j++)
		{
			// reading corresponding weights to kernel
			for (int cnt_kernel = 0; cnt_kernel < filtersize3; cnt_kernel++)
			{
				*(kernel3 + cnt_kernel) = weights_layer3[cnt_weight + cnt_kernel];
			}

			imfilter(img_fltr_p2, kernel3, img_fltr_3_tmp, rows, cols, padsize3);
			imadd(img_fltr_p3, img_fltr_3_tmp, cols, rows);

			cnt_weight = cnt_weight + filtersize3;
			img_fltr_p2 = img_fltr_p2 + rows*cols;
		}
		bias_tmp = biases_layer3[i];
		PReLU(img_fltr_p3, rows, cols, bias_tmp, prelu_coeff_layer3);
		img_fltr_p3 = img_fltr_p3 + rows*cols;
	}

	free(img_fltr_2);
	img_fltr_2 = NULL;
	free(img_fltr_2_tmp);
	img_fltr_2_tmp = NULL;
	free(kernel2);
	kernel2 = NULL;
	free(img_fltr_3_tmp);
	img_fltr_3_tmp = NULL;

	/////////// Layer4
	// Reading weights of 4th layer
	FILE *weights_layer4_ptr;
	weights_layer4_ptr = fopen(WEIGHT4, "r");
	if (weights_layer4_ptr == NULL) { printf("Error in the reading weights of 4th layer\n"); };
	double weights_layer4[1296];
	for (int i = 0; i < 1296; i++)
	{
		fscanf(weights_layer4_ptr, "%lf", &weights_layer4[i]);
	}
	fclose(weights_layer4_ptr);
	// Reading biases of 4th layer
	FILE *biases_layer4_ptr;
	biases_layer4_ptr = fopen(BIAS4, "r");
	if (biases_layer4_ptr == NULL) { printf("Error in the reading biases of 4th layer\n"); };
	double biases_layer4[12];
	for (int i = 0; i < 12; i++)
	{
		fscanf(biases_layer4_ptr, "%lf", &biases_layer4[i]);
	}
	fclose(biases_layer4_ptr);
	// Other parameters
	int filtersize4 = 9; //3X3
	int patchsize4 = 3;
	int padsize4 = (patchsize4 - 1) / 2;
	int num_filters4 = 12;
	int num_channels4 = 12;
	double prelu_coeff_layer4 = 0.2476;
	// Convolution
	double *img_fltr_4 = (double *)calloc(rows * cols * num_filters4 , sizeof(double));
	double *kernel4 = (double *)malloc(filtersize4*sizeof(double));
	double *img_fltr_p4 = img_fltr_4; // Pointer to img_fltr4
	double *img_fltr_4_tmp = (double *)malloc(rows * cols * sizeof(double));

	cnt_weight = 0;

	for (int i = 0; i < num_filters4; i++)
	{
		img_fltr_p3 = img_fltr_3; 
		for (int j = 0; j < num_channels4; j++)
		{
			// reading corresponding weights to kernel
			for (int cnt_kernel = 0; cnt_kernel < filtersize4; cnt_kernel++)
			{
				*(kernel4 + cnt_kernel) = weights_layer4[cnt_weight + cnt_kernel];
			}

			imfilter(img_fltr_p3, kernel4, img_fltr_4_tmp, rows, cols, padsize4);
			imadd(img_fltr_p4, img_fltr_4_tmp, cols, rows);

			cnt_weight = cnt_weight + filtersize4;
			img_fltr_p3 = img_fltr_p3 + rows*cols;
		}
		bias_tmp = biases_layer4[i];
		PReLU(img_fltr_p4, rows, cols, bias_tmp, prelu_coeff_layer4);
		img_fltr_p4 = img_fltr_p4 + rows*cols;
	}

	free(img_fltr_3);
	img_fltr_3 = NULL;
	free(img_fltr_3_tmp);
	img_fltr_3_tmp = NULL;
	free(kernel3);
	kernel3 = NULL;
	free(img_fltr_4_tmp);
	img_fltr_4_tmp = NULL;

	/////////// Layer5
	// Reading weights of 5th layer
	FILE *weights_layer5_ptr;
	weights_layer5_ptr = fopen(WEIGHT5, "r");
	if (weights_layer5_ptr == NULL) { printf("Error in the reading weights of 5th layer\n"); };
	double weights_layer5[1296];
	for (int i = 0; i < 1296; i++)
	{
		fscanf(weights_layer5_ptr, "%lf", &weights_layer5[i]);
	}
	fclose(weights_layer5_ptr);
	// Reading biases of 5th layer
	FILE *biases_layer5_ptr;
	biases_layer5_ptr = fopen(BIAS5, "r");
	if (biases_layer5_ptr == NULL) { printf("Error in the reading biases of 5th layer\n"); };
	double biases_layer5[12];
	for (int i = 0; i < 12; i++)
	{
		fscanf(biases_layer5_ptr, "%lf", &biases_layer5[i]);
	}
	fclose(biases_layer5_ptr);
	// Other parameters
	int filtersize5 = 9; //3X3
	int patchsize5 = 3;
	int padsize5 = (patchsize5 - 1) / 2;
	int num_filters5 = 12;
	int num_channels5 = 12;
	double prelu_coeff_layer5 = 0.3495;
	// Convolution
	double *img_fltr_5 = (double *)calloc(rows * cols * num_filters5 , sizeof(double));
	double *kernel5 = (double *)malloc(filtersize5*sizeof(double));
	double *img_fltr_p5 = img_fltr_5; // Pointer to img_fltr5
	double *img_fltr_5_tmp = (double *)malloc(rows * cols * sizeof(double));

	cnt_weight = 0;

	for (int i = 0; i < num_filters5; i++)
	{
		img_fltr_p4 = img_fltr_4;
		for (int j = 0; j < num_channels5; j++)
		{
			// reading corresponding weights to kernel
			for (int cnt_kernel = 0; cnt_kernel < filtersize5; cnt_kernel++)
			{
				*(kernel5 + cnt_kernel) = weights_layer5[cnt_weight + cnt_kernel];
			}

			imfilter(img_fltr_p4, kernel5, img_fltr_5_tmp, rows, cols, padsize5);
			imadd(img_fltr_p5, img_fltr_5_tmp, cols, rows);

			cnt_weight = cnt_weight + filtersize5;
			img_fltr_p4 = img_fltr_p4 + rows*cols;
		}
		bias_tmp = biases_layer5[i];
		PReLU(img_fltr_p5, rows, cols, bias_tmp, prelu_coeff_layer5);
		img_fltr_p5 = img_fltr_p5 + rows*cols;
	}

	free(img_fltr_4);
	img_fltr_4 = NULL;
	free(img_fltr_4_tmp);
	img_fltr_4_tmp = NULL;
	free(kernel4);
	kernel4 = NULL;
	free(img_fltr_5_tmp);
	img_fltr_5_tmp = NULL;

	/////////// Layer6
	// Reading weights of 6th layer
	FILE *weights_layer6_ptr;
	weights_layer6_ptr = fopen(WEIGHT6, "r");
	if (weights_layer6_ptr == NULL) { printf("Error in the reading weights of 6th layer\n"); };
	double weights_layer6[1296];
	for (int i = 0; i < 1296; i++)
	{
		fscanf(weights_layer6_ptr, "%lf", &weights_layer6[i]);
	}
	fclose(weights_layer6_ptr);
	// Reading biases of 6th layer
	FILE *biases_layer6_ptr;
	biases_layer6_ptr = fopen(BIAS6, "r");
	if (biases_layer6_ptr == NULL) { printf("Error in the reading biases of 6th layer\n"); };
	double biases_layer6[12];
	for (int i = 0; i < 12; i++)
	{
		fscanf(biases_layer6_ptr, "%lf", &biases_layer6[i]);
	}
	fclose(biases_layer6_ptr);
	// Other parameters
	int filtersize6 = 9; //3X3
	int patchsize6 = 3;
	int padsize6 = (patchsize6 - 1) / 2;
	int num_filters6 = 12;
	int num_channels6 = 12;
	double prelu_coeff_layer6 = 0.7806;
	// Convolution
	double *img_fltr_6 = (double *)calloc(rows * cols * num_filters6 , sizeof(double));
	double *kernel6 = (double *)malloc(filtersize6*sizeof(double));
	double *img_fltr_p6 = img_fltr_6; // Pointer to img_fltr6
	double *img_fltr_6_tmp = (double *)malloc(rows * cols * sizeof(double));

	cnt_weight = 0;

	for (int i = 0; i < num_filters6; i++)
	{
		img_fltr_p5 = img_fltr_5;
		for (int j = 0; j < num_channels6; j++)
		{
			// reading corresponding weights to kernel
			for (int cnt_kernel = 0; cnt_kernel < filtersize6; cnt_kernel++)
			{
				*(kernel6 + cnt_kernel) = weights_layer6[cnt_weight + cnt_kernel];
			}

			imfilter(img_fltr_p5, kernel6, img_fltr_6_tmp, rows, cols, padsize6);
			imadd(img_fltr_p6, img_fltr_6_tmp, cols, rows);

			cnt_weight = cnt_weight + filtersize6;
			img_fltr_p5 = img_fltr_p5 + rows*cols;
		}
		bias_tmp = biases_layer6[i];
		PReLU(img_fltr_p6, rows, cols, bias_tmp, prelu_coeff_layer6);
		img_fltr_p6 = img_fltr_p6 + rows*cols;
	}

	free(img_fltr_5);
	img_fltr_5 = NULL;
	free(img_fltr_5_tmp);
	img_fltr_5_tmp = NULL;
	free(kernel5);
	kernel5 = NULL;

	/////////// Layer7
	// Reading weights of 7th layer
	FILE *weights_layer7_ptr;
	weights_layer7_ptr = fopen(WEIGHT7, "r");
	if (weights_layer7_ptr == NULL) { printf("Error in the reading weights of 7th layer\n"); };
	double weights_layer7[672];
	for (int i = 0; i < 672; i++)
	{
		fscanf(weights_layer7_ptr, "%lf", &weights_layer7[i]);
	}
	fclose(weights_layer7_ptr);
	// Reading biases of 7th layer
	FILE *biases_layer7_ptr;
	biases_layer7_ptr = fopen(BIAS7, "r");
	if (biases_layer7_ptr == NULL) { printf("Error in the reading biases of 7th layer\n"); };
	double biases_layer7[56];
	for (int i = 0; i < 56; i++)
	{
		fscanf(biases_layer7_ptr, "%lf", &biases_layer7[i]);
	}
	fclose(biases_layer7_ptr);
	// Other parameters
	int filtersize7 = 1; //1X1
	int patchsize7 = 1;
	int padsize7 = (patchsize7 - 1) / 2;
	int num_filters7 = 56;
	int num_channels7 = 12;
	double prelu_coeff_layer7 = 0.0087;
	// Convolution
	double *img_fltr_7 = (double *)calloc(rows * cols * num_filters7 , sizeof(double));
	double *kernel7 = (double *)malloc(filtersize7*sizeof(double));
	double *img_fltr_p7 = img_fltr_7; // Pointer to img_fltr7
	double *img_fltr_7_tmp = (double *)malloc(rows * cols * sizeof(double));

	cnt_weight = 0;

	for (int i = 0; i < num_filters7; i++)
	{
		img_fltr_p6 = img_fltr_6;
		for (int j = 0; j < num_channels7; j++)
		{
			// reading corresponding weights to kernel
			for (int cnt_kernel = 0; cnt_kernel < filtersize7; cnt_kernel++)
			{
				*(kernel7 + cnt_kernel) = weights_layer7[cnt_weight + cnt_kernel];
			}

			imfilter(img_fltr_p6, kernel7, img_fltr_7_tmp, rows, cols, padsize7);
			imadd(img_fltr_p7, img_fltr_7_tmp, cols, rows);

			cnt_weight = cnt_weight + filtersize7;
			img_fltr_p6 = img_fltr_p6 + rows*cols;
		}
		bias_tmp = biases_layer7[i];
		PReLU(img_fltr_p7, rows, cols, bias_tmp, prelu_coeff_layer7);
		img_fltr_p7 = img_fltr_p7 + rows*cols;
	}

	free(img_fltr_6);
	img_fltr_6 = NULL;
	free(img_fltr_6_tmp);
	img_fltr_6_tmp = NULL;
	free(kernel6);
	kernel6 = NULL;

	/////////// Convolution3 ------------------- Layer 8

	/////////// Layer8
	// Reading weights of 8th layer
	FILE *weights_layer8_ptr;
	weights_layer8_ptr = fopen(WEIGHT8, "r");
	if (weights_layer8_ptr == NULL) { printf("Error in the reading weights of 8th layer\n"); };
	double weights_layer8[4536];
	for (int i = 0; i < 4536; i++)
	{
		fscanf(weights_layer8_ptr, "%lf", &weights_layer8[i]);
	}
	fclose(weights_layer8_ptr);
	// Reading biases of 8th layer
	double biases_layer8 = - 0.03262640000;
	// Other parameters
	int filtersize8 = 81; //9x9
	int patchsize8 = 9;
	int num_filters8 = 1;
	int num_channels8 = 56;

	// Decvolution ==> output is the img_hr
	double *img_fltr_8 = (double *)calloc((rows*scale) *(cols*scale) * num_filters8 , sizeof(double));
	double *kernel8 = (double *)malloc(filtersize8*sizeof(double));
	double *img_fltr_8_tmp = (double *)malloc((rows*scale) *(cols*scale) * sizeof(double));
	
	cnt_weight = 0;
	img_fltr_p7 = img_fltr_7;

	for (int j = 0; j < num_channels8; j++)
	{
		// reading corresponding weights to kernel
		for (int cnt_kernel = 0; cnt_kernel < filtersize8; cnt_kernel++)
		{
			*(kernel8 + cnt_kernel) = weights_layer8[cnt_weight + cnt_kernel];
		}
		cnt_weight = cnt_weight + filtersize8;

		deconv(img_fltr_p7, img_fltr_8_tmp, kernel8, cols, rows, scale);

		imadd(img_fltr_8, img_fltr_8_tmp, cols*scale, rows*scale);
		
		img_fltr_p7 = img_fltr_p7 + rows*cols;
	}


	for (int i=0;i<rows*scale;i++)
	for (int j = 0;j<cols*scale; j++)
	{
		int cnt_fnl = i*cols*scale + j;
		*(img_hr + cnt_fnl) = *(img_fltr_8 + cnt_fnl) + biases_layer8;
	}


	free(img_fltr_7);
	img_fltr_7 = NULL;
	free(img_fltr_7_tmp);
	img_fltr_7_tmp = NULL;
	free(kernel7);
	kernel7 = NULL;

	free(img_fltr_8);
	img_fltr_8 = NULL;
	free(img_fltr_8_tmp);
	img_fltr_8_tmp = NULL;
	free(kernel8);
	kernel8 = NULL;

}