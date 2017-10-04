#pragma once
#include "Image.h"

template <typename T>
class HoughTransform 
{
public:
	HoughTransform(size_t width, size_t height);
	
	Image<T> img;



};

template <typename T>
HoughTransform<T>::HoughTransform(size_t width, size_t height) :img(width, height)
{
}

