#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H
#endif

template <typename T>
class Image {
public:
	size_t width;
	size_t height;
	T *pixels;

	Image(size_t width, size_t height);
	Image(const Image& image) = delete;
	Image& operator=(const Image&) = delete;

	~Image();

	std::size_t GetBytesPerPixel();
	std::size_t GetSizeInBytes();

	bool operator==(const Image& rhs) const;

protected:
	Image(size_t width, size_t height, T *pixels);
};

