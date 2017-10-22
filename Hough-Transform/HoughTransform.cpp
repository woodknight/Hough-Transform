#include <cmath>
#include <iostream>
#include <algorithm>

#include "HoughTransform.h"

using namespace std;

const double pi = 3.14159265;

HoughTransform::HoughTransform(size_t w, size_t h) :width(w), height(h)
{
	filteredImg = new float[width*height];		// Gaussian filtered image
	edgeAmp = new float[width*height];			// amplitute of soble edge
	edgeAngle = new int[width*height];;			// angle of soble edge
	imgSuppressed = new float[width*height];	// non maximum suppressed edge image
	binaryImage = new bool[width*height];		// binary image

	// initialize rThetaM
	rRange = 2*ceil(sqrt(width*width + height*height)) + 1;
	// theta = -90 : 1 : 89
	// set up size. (180 x rRange)
	
	rThetaM.resize(180);
	for (int i = 0; i < 180; i++)
		rThetaM[i].resize(rRange);	
}

HoughTransform::~HoughTransform()
{
	delete[] filteredImg;
	delete[] edgeAmp;
	delete[] edgeAngle;
	delete[] imgSuppressed;
	delete[] binaryImage;
}

void HoughTransform::GaussianFilter()
{
	// Gaussian filter using separable convolution
	// https://blogs.mathworks.com/steve/2006/10/04/separable-convolution/
	const size_t w = width;
	const size_t h = height;

	float *tmp = new float[w*h];

	float filter[] = { 0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f }; //Gaussian filter, width=5, sigma=1

																	  // convolve along horizontal direction
	size_t index = 0;
	float value = 0;
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
		{
			value = 0;
			for (int k = -2; k <= 2; k++)
			{
				if (j + k >= 0 && j + k < w)
				{
					index = i*w + j + k;
					value += filter[k + 2] * img[index];
				}
			}
			tmp[i*w + j] = value;
		}

	// convolve along vertical direction
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
		{
			value = 0;
			for (int k = -2; k <= 2; k++)
			{
				if (i + k >= 0 && i + k < h)
				{
					index = (i + k)*w + j;
					value += filter[k + 2] * tmp[index];
				}
			}
			filteredImg[i*w + j] = value;
		}
	delete[] tmp;
}

void HoughTransform::SobelEdge()
{
	// This function convolve the image with a Sobel filter in one dimention using
	// separable convolution, see(https://blogs.mathworks.com/steve/2006/10/04/separable-convolution/)
	// and (https://en.wikipedia.org/wiki/Sobel_operator)
	// image is h*w matrix rearranged to 1D array in row-major order

	const size_t w = width;
	const size_t h = height;

	float *Gx = new float[h * w]; // gradient along x
	float *Gy = new float[h * w]; // gradient along y	

	int A[3] = { 1, 0, -1 }; // filter kernel
	int B[3] = { 1, 2, 1 };

	float *g_1 = new float[h * w]; // temporary array
	float *g_2 = new float[h * w];

	int r = 0, c = 0; // row and column index

	size_t index = 0;
	for (int i = 0; i < h; i++) // first and last columns
	{
		index = i * w + 0;
		g_1[index] = A[1] * filteredImg[index] + A[2] * filteredImg[index + 1];
		g_2[index] = B[1] * filteredImg[index] + B[2] * filteredImg[index + 1];

		index = i * w + w - 1;
		g_1[index] = A[0] * filteredImg[index - 1] + A[1] * filteredImg[index];
		g_2[index] = B[0] * filteredImg[index - 1] + B[1] * filteredImg[index];
	}
	for (int i = 0; i < h; i++) // 2 to w -1 columns
	{
		for (int j = 1; j < w - 1; j++)
		{
			index = i * w + j;
			g_1[index] = A[0] * filteredImg[index - 1] + A[1] * filteredImg[index] + A[2] * filteredImg[index + 1];
			g_2[index] = B[0] * filteredImg[index - 1] + B[1] * filteredImg[index] + B[2] * filteredImg[index + 1];
		}
	}

	for (int j = 0; j < w; j++) // first and last rows
	{
		index = j;
		Gx[index] = B[1] * g_1[index] + B[2] * g_1[index + w];
		Gy[index] = A[1] * g_2[index] + A[2] * g_2[index + w];
		edgeAmp[index] = abs(Gx[index]) + abs(Gy[index]);

		index = (h - 1) * w + j;
		Gx[index] = B[0] * g_1[index - w] + B[1] * g_1[index];
		Gy[index] = A[0] * g_2[index - w] + A[1] * g_2[index];
		edgeAmp[index] = abs(Gx[index]) + abs(Gy[index]);
	}
	for (int i = 1; i < h - 1; i++) // 2 to h - 1 rows
	{
		for (int j = 0; j < w; j++)
		{
			index = i * w + j;
			Gx[index] = B[0] * g_1[index - w] + B[1] * g_1[index] + B[2] * g_1[index + w];
			Gy[index] = A[0] * g_2[index - w] + A[1] * g_2[index] + A[2] * g_2[index + w];

			edgeAmp[index] = abs(Gx[index]) + abs(Gy[index]);
		}
	}

	float Gdiv = 0;
	for (int i = 0; i < w * h; i++)
	{
		/* directions
		0: 90 degree
		1: 135 degree
		2: 0 degree
		3: 45 degree
		*/
		Gdiv = Gy[i] / Gx[i];
		if (Gdiv < 0)
		{
			if (Gdiv < -2.41421356237)
				edgeAngle[i] = 0;
			else if (Gdiv < -0.414213562373)
				edgeAngle[i] = 1;
			else
				edgeAngle[i] = 2;
		}
		else
		{
			if (Gdiv > 2.41421356237)
				edgeAngle[i] = 0;
			else if (Gdiv > 0.414213562373)
				edgeAngle[i] = 3;
			else
				edgeAngle[i] = 2;
		}
	}

	delete[] Gx;
	delete[] Gy;
	delete[] g_1;
	delete[] g_2;
}

void HoughTransform::NonMaxSuppression()
{
	const size_t h = height;
	const size_t w = width;

	size_t index = 0;
	for (int y = 1; y < h - 1; y++)
		for (int x = 1; x < w - 1; x++)
		{
			index = y*w + x;
			switch (edgeAngle[index])
			{
			case 0: // 90 degree
				if (edgeAmp[index] > edgeAmp[index - w] && edgeAmp[index] > edgeAmp[index + w])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			case 1: // 135 degree
				if (edgeAmp[index] > edgeAmp[index - w - 1] && edgeAmp[index] > edgeAmp[index + w + 1])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			case 2: // 0 degree
				if (edgeAmp[index] > edgeAmp[index - 1] && edgeAmp[index] > edgeAmp[index + 1])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			case 3: // 45 degree
				if (edgeAmp[index] > edgeAmp[index - w + 1] && edgeAmp[index] > edgeAmp[index + w - 1])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			default:
				std::cout << "error, edge angle out of range!" << std::endl;
				break;
			}

			if (imgSuppressed[index] > 255)
				imgSuppressed[index] = 255;
		}
	// set boundaries to zero
	for (int i = 0; i < w; i++)
	{
		imgSuppressed[i] = 0;
		imgSuppressed[(h - 1)*w + i] = 0;
	}
	for (int i = 1; i < h - 1; i++)
	{
		imgSuppressed[i*w] = 0;
		imgSuppressed[i*w - 1] = 0;
	}

}

void HoughTransform::histogram()
{
	for (int i = 0; i < 256; i++)
		hist[i] = 0;

	for (int i = 0; i < height*width; i++)
		hist[(unsigned char)(imgSuppressed[i])]++;
}

unsigned char HoughTransform::otsu()
{
	// find a threshold level that maximizes the between-class variance
	unsigned char level = 0;

	double total = 0; // total number of pixels
	for (int i = 0; i < 256; i++)
		total += hist[i];

	double sum1 = 0;
	double sumB = 0;
	double wB = 0;
	double wF = 0;
	double mF = 0;
	double maximum = 0;
	double between = 0;

	for (int i = 0; i < 256; i++)
		sum1 += i*hist[i];

	for (int i = 0; i < 256; i++)
	{
		wB += hist[i];
		wF = total - wB;
		if (wB == 0 || wF == 0)
			continue;
		sumB += i*hist[i];
		mF = (sum1 - sumB) / wF;
		between = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
		if (between >= maximum)
		{
			level = (unsigned char)i;
			maximum = between;
		}
	}

	// cout << "otsu level: " << (int)level << endl;
	return level;
}

unsigned char HoughTransform::percentile(const double p)
{
	// find a threshold level that p percent of pixels are not considered edge
	unsigned char level = 0;

	double sum = 0; // total number of pixels
	for (int i = 0; i < 255; i++)
		sum += hist[i];

	double sumB = 0;
	for (int i = 0; i < 255; i++)
	{
		sumB += hist[i];
		if (sumB > p * sum)
		{
			level = (unsigned char)i;
			return level;
		}
	}

	std::cout << "error percentile" << std::endl;
}

void HoughTransform::threshold()
{
	const size_t h = height;
	const size_t w = width;

	histogram();

	// high threshold
	unsigned char high;
	high = otsu();
	// high = percentile(hist, 0.7);

	//low threshold
	unsigned char low = high * 0.4;

	bool *strong = new bool[h*w];
	bool *weak = new bool[h*w];

	for (int i = 0; i < h*w; i++)
	{
		if (imgSuppressed[i] > high)
		{
			strong[i] = true;
			weak[i] = false;
		}
		else if (imgSuppressed[i] > low)
		{
			weak[i] = true;
			strong[i] = false;
		}
		else
		{
			strong[i] = false;
			weak[i] = false;
		}

		binaryImage[i] = false;
	}

	size_t index = 0;
	for (int i = 1; i < h - 1; i++)
		for (int j = 1; j < w - 1; j++)
		{
			index = i*w + j;
			if (strong[index])
				binaryImage[index] = true;
			else if (weak[index])
			{
				// check 8-connected neighborhood pixels
				if (strong[index + 1])
				{
					binaryImage[index] = true;
					continue;
				}
				if (strong[index + w + 1])
				{
					binaryImage[index] = true;
					continue;
				}
				if (strong[index + w])
				{
					binaryImage[index] = true;
					continue;
				}
				if (strong[index + w - 1])
				{
					binaryImage[index] = true;
					continue;
				}
				if (strong[index - 1])
				{
					binaryImage[index] = true;
					continue;
				}
				if (strong[index - w - 1])
				{
					binaryImage[index] = true;
					continue;
				}
				if (strong[index - w])
				{
					binaryImage[index] = true;
					continue;
				}
				if (strong[index - w + 1])
				{
					binaryImage[index] = true;
					continue;
				}
			}
		}

}

void HoughTransform::HoughMatrix()
{
	// find lines using Hough transform
	const size_t h = height;
	const size_t w = width;	

	// generate binary edge image for voting
	GaussianFilter();
	SobelEdge();
	NonMaxSuppression();
	threshold();

	// populate rThetaM matrix using voting
	size_t index = 0;	
	int r;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			index = y*w + x;
			if (binaryImage[index])
			{				
				for (int a = -90; a < 90; a++)
				{
					r = round(x*cos(a / 180.0*pi) + y*sin(a / 180.0*pi));
					rThetaM[a+90][r + (rRange-1)/2]++;
				}
			}
		}
	}	
}

vector<int> HoughTransform::findMax()
{
	vector<int> coord;
	int aMax = 0;
	int rMax = 0;

	int max = 0;
	int currentMax = 0;
	for (int a = 0; a < 180; a++)
		for(int r = 0; r < rRange; r++)
		{
			if (rThetaM[a][r] > max)
			{
				max = rThetaM[a][r];
				aMax = a;
				rMax = r;
			}
		}
	if (max > 0)
	{
		coord.push_back(max);
		coord.push_back(aMax);
		coord.push_back(rMax);
	}
	return coord;
}

void HoughTransform::HoughPeaks(const int numOfPeaks, const int hooda, const int hoodr)
{	
	// find peaks of Hough Transfrom matrix
	// suppress the neighborhoods. neighborhoods window size equals (hooda*2+1)x(hoodr*2+1)
	// modified from MATLAB

	HoughMatrix();

	// find maximum value of rThetaM for thresholding	
	int max = 0;
	int currentMax = 0;
	for (const auto& a : rThetaM)
	{
		currentMax = *max_element(a.cbegin(), a.cend());
		max = currentMax > max ? currentMax : max;
	}
	int threshold = (int) (max / 2);
	
	// find peaks
	int peakCount = 0;
	vector<int> coord;
		
	while (peakCount < numOfPeaks)
	{
		peakCount++;

		coord = findMax();
		if (!coord.empty() && coord[0] > threshold)
		{
			peaks.push_back(coord);

			// suppress the neighborhoods
			int aTmp = 0;
			int rTmp = 0;
			for(int a = coord[1] - hooda; a <= coord[1] + hooda; a++)
				for (int r = coord[2] - hoodr; r <= coord[2] + hoodr; r++)
				{
					// Throw away neighbor coordinates that are out of bounds in
				    // the rho direction.
					if (r >= 0 && r < rRange)
					{
						// For coordinates that are out of bounds in the theta
						// direction, we want to consider that H is antisymmetric
						// along the rho axis for theta = +/ -90 degrees.
						if (a < 0)
						{
							aTmp = a + 180;
							rTmp = rRange - r;
						}
						else if (a >= 180)
						{
							aTmp = a - 180;
							rTmp = rRange - r;
						}
						else
						{
							aTmp = a;
							rTmp = r;
						}
						
						rThetaM[aTmp][rTmp] = 0;
					}
				}
		}
	}

}

vector<vector<int>> HoughTransform::HoughPixels(const int A, const int R)
{
	// compute image pixel coordinates belonging the Hough transfrom bin (a, r)
	vector<vector<int>> pixels;
	int r;
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			if (binaryImage[i*width + j])
			{
				r = round(j*cos(A / 180.0*pi) + i*sin(A / 180.0*pi));
				if (r  == R)
					pixels.push_back({ j, i });
			}
		}
	return pixels;
}

void HoughTransform::HoughLines(const int numOfLines, const int fillGap, const int minLength)
{
	// search for line segments corresponding to peaks in the Hough transform matrix.
	// if the gap between colinear segments are smaller than fillGap, connect them.
	// if the merged line is shorter than minLength, discard it.
	// lines are stored in vectors by the coordinates of starting and ending points
	// as [x1, y1, x2, y2]

	HoughPeaks(numOfLines);
	vector<vector<int>> pixels;
	for (auto const &p : peaks)
	{
		// compute image pixel coordinates belonging the Hough transfrom bin (a, r)
		pixels = HoughPixels(p[1] - 90, p[2] - (rRange - 1) / 2);
		if (pixels.empty())
			break;

		int gap, length;
		
		// store the temporary starting and ending points
		auto q1 = pixels.begin();
		auto q2 = pixels.begin();
		
		// find gaps between line segments that are larger than threshold
		for (auto it = pixels.begin(); it != pixels.end()-1; it++)
		{
			gap = ((*it)[0] - (*(it + 1))[0]) * ((*it)[0] - (*(it + 1))[0])
				+ ((*it)[1] - (*(it + 1))[1]) * ((*it)[1] - (*(it + 1))[1]);
			if (gap > fillGap*fillGap)
			{
				q2 = it;
				length = ((*q1)[0] - (*q2)[0])*((*q1)[0] - (*q2)[0]) + ((*q1)[1] - (*q2)[1])*((*q1)[1] - (*q2)[1]);
				if (length >= minLength*minLength)
					lines.push_back({ (*q1)[0], (*q1)[1], (*q2)[0], (*q2)[1] });
				// reset the starting and ending points
				q1 = it + 1;
				q2 = it + 1;
			}
		}
		// if no large gap found, push the line
		if (q1 == q2)
		{
			q2 = pixels.end()-1;
			length = ((*q1)[0] - (*q2)[0])*((*q1)[0] - (*q2)[0]) + ((*q1)[1] - (*q2)[1])*((*q1)[1] - (*q2)[1]);
			if (length >= minLength*minLength)
				lines.push_back({ (*q1)[0], (*q1)[1], (*q2)[0], (*q2)[1] });
		}

	}
}




