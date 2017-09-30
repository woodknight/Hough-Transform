
#include "HoughTransform.h"

template <typename T>
Image<T>::Image(size_t width, size_t height)
	: width{ width }, height{ height }, pixels{ new T[width*height] } {
}

template <typename T>
Image<T>::~Image() {
	delete[] pixels;
}

template <typename T>
size_t Image<T>::GetBytesPerPixel() {
	return sizeof(T);
}

template <typename T>
size_t Image<T>::GetSizeInBytes() {
	return this->width * this->height * this->GetBytesPerPixel();
}

template <typename T>
bool Image<T>::operator==(const Image& rhs) const {
	if (width == rhs.width && height == rhs.height) {
		for (auto i = 0; i < width*height; ++i) {
			if (pixels[i] != rhs.pixels[i]) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}

template <typename T>
Image<T>::Image(size_t width, size_t height, T *pixels)
	: width{ width }, height{ height }, pixels{ pixels } {
}