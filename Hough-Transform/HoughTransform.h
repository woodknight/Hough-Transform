#ifndef _HOUGHTRANSFORM_H
#define _HOUGHTRANSFORM_H

#include <vector>

#include "Image.h"

class HoughTransform 
{
public:
	HoughTransform(size_t width, size_t height);
	~HoughTransform();
	
	Image<unsigned char> img;

	void HoughLines();

	std::vector<std::vector<int>> rThetaM; // 2D array to store the r-theta voting matrix

private:
	float *filteredImg;		// Gaussian filtered image
	float *edgeAmp;			// amplitute of soble edge
	int *edgeAngle;			// angle of soble edge
	float *imgSuppressed;	// non maximum suppressed edge image
	bool *binaryImage;		// binary image
	unsigned int hist[256];				// histogram of imgSuppressed

	int rRange;				// maximum r of r-theta matrix
		
	void GaussianFilter();
	void SobelEdge();
	void NonMaxSuppression();
	void histogram();
	unsigned char otsu();
	unsigned char percentile(const double p);
	void threshold();	// convert the image to binary
};


#endif
