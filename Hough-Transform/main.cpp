#include <iostream>
#include <chrono>

#include "CImg.h"
#include "Image.h"
#include "HoughTransform.h"

using namespace std;
using namespace cimg_library;

int main()
{
	
	CImg<unsigned char> img("valve.bmp");
	
	size_t h = img.height();
	size_t w = img.width();
	
	// Record start time
	auto start = std::chrono::high_resolution_clock::now();

	unsigned char *imageArray = img.data();
	
	HoughTransform H(w,h);
	H.img.pixels = imageArray;

	H.HoughPeaks(5);
	for (auto const&c : H.peaks)
	{
		for (auto const&a : c)
		{
			cout << a << " ";
		}
		cout << endl;
	}


	// Record end time
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << endl;


	/*
	for (int i = 0; i < 180; i++)
	{
		for (int j = 0; j < h / 2; j++)
			cout << H.rThetaM[i][j] << " ";
		cout << endl;
	}
	*/
	
	/*
	
	// display images
	CImg<unsigned char> img2(imageArray, w, h);
	
	// imageSuppressed.save("suppressed.bmp");

	CImgDisplay src_disp(img, "source");
	while (!src_disp.is_closed())
	{
		src_disp.wait();
	}
	
	*/
	// return 0;
}