#include <iostream>

#include "CImg.h"
#include "Image.h"
#include "HoughTransform.h"

using namespace std;
using namespace cimg_library;

int main()
{
	CImg<unsigned char> img("valve.bmp");
	
	int h = img.height();
	int w = img.width();
	
	unsigned char *imageArray = img.data();
	
	
	// display images
	CImg<unsigned char> img2(imageArray, w, h);
	
	// imageSuppressed.save("suppressed.bmp");

	CImgDisplay src_disp(img, "source");
	while (!src_disp.is_closed())
	{
		src_disp.wait();
	}
		
	return 0;
}