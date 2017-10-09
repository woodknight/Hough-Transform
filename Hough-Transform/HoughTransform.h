#ifndef _HOUGHTRANSFORM_H
#define _HOUGHTRANSFORM_H

#include <vector>

#include "Image.h"

using std::vector;

class HoughTransform 
{
public:
	HoughTransform(size_t width, size_t height);
	~HoughTransform();
	
	Image<unsigned char> img;
	
	// find lines from peaks of Hough transfrom matrix
	void HoughLines(const int numOfLines = 1, const int fillGap = 10, const int minLength = 10);

	vector<vector<int>> lines; // coordinates of lines of img

private:
	float *filteredImg;		// Gaussian filtered image
	float *edgeAmp;			// amplitute of soble edge
	int *edgeAngle;			// angle of soble edge
	float *imgSuppressed;	// non maximum suppressed edge image
	bool *binaryImage;		// binary image
	unsigned int hist[256];				// histogram of imgSuppressed

	int rRange;				// maximum r of r-theta matrix
	vector<vector<int>> rThetaM; // 2D array to store the r-theta voting matrix
	vector<vector<int>> peaks; // coordinates of peaks of rThetaM

	void GaussianFilter();
	void SobelEdge();
	void NonMaxSuppression();
	void histogram();
	unsigned char otsu();
	unsigned char percentile(const double p);
	void threshold();	// convert the image to binary

	void HoughMatrix(); // compute hough transform matrix	
	vector<int> findMax(); // find coordinates of maximum of hough transform matrix						   
	void HoughPeaks(const int numOfPeaks = 1, const int hooda = 2, const int hoodr = 5); // find coordinates of peaks of Hough transform matrix
};


#endif
