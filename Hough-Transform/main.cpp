#include <iostream>
#include <chrono>

#include "CImg.h"
#include "Image.h"
#include "HoughTransform.h"

using namespace std;
using namespace cimg_library;

int main()
{
	
	CImg<unsigned char> img("house.bmp");
	
	size_t h = img.height();
	size_t w = img.width();
	
	// Record start time
	auto start = std::chrono::high_resolution_clock::now();

	unsigned char *imageArray = img.data();
	
	HoughTransform H(w,h);
	H.img = imageArray;

	H.HoughLines(10, 5, 60);
	
	// Record end time
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << endl;
	
	// draw lines on image
	const unsigned char color[3] = { 0, 255, 0 };
	for (const auto L : H.lines)
	{
		cout << L[0] << " " << L[1] << " " << L[2] << " " << L[3] << endl; // cout coordinates of line segments
		img.draw_line(L[0], L[1], L[2], L[3], color, 1.0f);  // wish the draw_line function has a parameter to change line width
	}
	
	// display images	
	CImgDisplay src_disp(img, "source");

	while (!src_disp.is_closed())
	{
		src_disp.wait();
	}
	
	
	return 0;
}