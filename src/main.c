#include "../includes/global.h"

int main(int argc, char *argv[]){
	char *inFile = argv[1];
	char *outFile = argv[2];

	//Upsampler parameters
	int scale = 2;

	//Compressed Assault Cube
	int num = 300; //Number of frames to interpolate
	int inCols = 176; //Width of input (downsampled) video
	int inRows = 144; //Height of input (downsampled) video

	int outCols = inCols*scale;
	int outRows = inRows*scale;

	FILE *inFp, *outFp;

	inFp = fopen(inFile, "rb");
	if (inFp == NULL)
	{
		printf("\n We have null pointer \n");
	}
	outFp = fopen(outFile, "wb");
	if (outFp == NULL)
	{
		printf("\n We have null pointer \n");
	}

	// To read and write each frame in an unsigned character format
	unsigned char *inBuf = (unsigned char *)malloc(inCols*inRows*sizeof(unsigned char));
	unsigned char *outBuf = (unsigned char *)malloc(outCols*outRows*sizeof(unsigned char));
	// To work with each pixel in the range of 0~1
	double *inBuf_tmp = (double *)malloc(inCols*inRows*sizeof(double));
	double *outBuf_tmp = (double *)malloc(outCols*outRows*sizeof(double));

	for (int fcnt = 0; fcnt < num; fcnt++)
	{
		//////// Interpolate each frame using FSRCNN for Y component and simple repitition for U and V components
		// Pointer to obtain value of each tpixel of input frame
		unsigned char *inP = inBuf;
		double *inP_tmp = inBuf_tmp;
		// Pointer to obtain value of each pixel of output frame
		unsigned char *outP = outBuf;
		double *outP_tmp = outBuf_tmp;

		//Y Component
		fread(inBuf, sizeof(unsigned char), inCols*inRows, inFp);
		int i, j;

		for (i = 0; i<inRows; i++)
		for (j = 0; j<inCols; j++)
		{
			int cnt = i*inCols + j;
			int x = *inP++;
			*(inP_tmp + cnt) = (double)(x / 255.0);
		}

		FSRCNN(outP_tmp, inP_tmp, inRows, inCols, scale);

		outP_tmp = outBuf_tmp;
		
		for (i = 0; i<inRows*scale; i++)
		for (j = 0; j<inCols*scale; j++)
		{
			int cnt = i*inCols*scale + j;
			*(outP_tmp + cnt) = *(outP_tmp + cnt) * 255;
		}


		double_to_uint8( outP_tmp, outP, outCols, outRows);

		fwrite(outBuf, sizeof(unsigned char), outCols*outRows, outFp);

		//U Component
		fread(inBuf, sizeof(unsigned char), inCols*inRows / 4, inFp);

		inP = inBuf;
		outP = outBuf;

		for (i = 0; i < inRows / 2; i++)
		for (j = 0; j < inCols / 2; j++) {

			int cnt = 2 * (i * outCols / 2 + j);

			unsigned char x = *inP++;

			*(outP + cnt) = x;
			*(outP + cnt + 1) = x;
			*(outP + cnt + outCols / 2) = x;
			*(outP + cnt + outCols / 2 + 1) = x;

		}

		fwrite(outBuf, sizeof(unsigned char), outCols*outRows / 4, outFp);

		// V COmponent
		fread(inBuf, sizeof(unsigned char), inCols*inRows / 4, inFp);
		inP = inBuf;
		outP= outBuf;

		for (i = 0; i < inRows / 2; i++)
		for (j = 0; j < inCols / 2; j++) {

			int cnt = 2 * (i*outCols / 2 + j);

			unsigned char x = *inP++;

			*(outP + cnt) = x;
			*(outP + cnt + 1) = x;
			*(outP + cnt + outCols / 2) = x;
			*(outP + cnt + outCols / 2 + 1) = x;

		}

		fwrite(outBuf, sizeof(unsigned char), outCols*outRows / 4, outFp);
		
	}
	free(inBuf);
	inBuf = NULL;
	free(inBuf_tmp);
	inBuf_tmp = NULL;
	free(outBuf);
	outBuf = NULL;
	free(outBuf_tmp);
	outBuf_tmp = NULL;
}