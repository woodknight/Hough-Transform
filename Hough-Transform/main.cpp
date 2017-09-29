#include <iostream>

#include "CannyEdge.h"
#include "CImg.h"

using namespace std;
using namespace cimg_library;

int main()
{
	CImg<unsigned char> img("valve.bmp");
	
	int h = img.height();
	int w = img.width();
	
	unsigned char *imageArray = img.data();
	
	float *blurred = new float[w*h];
	GaussianFilter(imageArray, blurred, w, h);
		
	float *edgeAmp = new float[w*h];
	int *edgeAngle = new int[w*h];
	SobelEdge(blurred, edgeAmp, edgeAngle, h, w);

	float *suppressed = new float[w*h];
	NonMaxSuppression(edgeAmp, edgeAngle, suppressed, h, w);

	bool *binaryImage = new bool[w*h];
	threshold(suppressed, binaryImage, h, w);
	
	// display images
	CImg<unsigned char> imageBlurred(blurred, w, h);
	CImg<unsigned char> imageEdgeAmp(edgeAmp, w, h);
	CImg<unsigned char> imageSuppressed(suppressed, w, h);
	CImg<bool> imageBinary(binaryImage, w, h);

	imageSuppressed.save("suppressed.bmp");

	CImgDisplay src_disp(img, "source");
	CImgDisplay edgeAmp_disp(imageEdgeAmp, "edge amp");
	CImgDisplay suppressed_disp(imageSuppressed, "suppressed");
	CImgDisplay binary_disp(imageBinary, "binary");
	while (!src_disp.is_closed())
	{
		src_disp.wait();
	}
		
	delete blurred;
	delete edgeAmp;
	delete edgeAngle;
	delete suppressed;
	delete binaryImage;
	return 0;
}