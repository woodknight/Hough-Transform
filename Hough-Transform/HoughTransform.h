#ifndef _HOUGHTRANSFORM_H
#define _HOUGHTRANSFORM_H

#include <vector>

using std::vector;

class HoughTransform 
{
public:
	HoughTransform(size_t w, size_t h);
	~HoughTransform();
		
	const size_t width;
	const size_t height;
	unsigned char *img;	
	
	// find lines from peaks of Hough transfrom matrix
	void HoughLines(const int numOfLines = 1, const int fillGap = 20, const int minLength = 40);

	// lines are stored in vectors by the coordinates of starting and ending points
	// as [x1, y1, x2, y2]
	vector<vector<int>> lines;
	bool *binaryImage;		// binary image

private:
	float *filteredImg;		// Gaussian filtered image
	float *edgeAmp;			// amplitute of soble edge
	int *edgeAngle;			// angle of soble edge
	float *imgSuppressed;	// non maximum suppressed edge image
	

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
	vector<vector<int>> HoughPixels(const int a, const int r);
};


#endif
