#ifndef _CANNYEDGE_H
#define _CANNYEDGE_H
#endif

void GaussianFilter(const unsigned char *src, float *dst, const int w, const int h);
void SobelEdge(const float *image, float *edgeAmp, int *edgeAngle, const int numOfRows, const int numOfCols);
void NonMaxSuppression(const float *edgeAmp, const int *edgeAngle, float *imgSuppressed, const int numOfRows, const int numOfCols);
void histogram(const float *image, int *hist, const int numOfRows, const int numOfCols);
unsigned char otsu(const int *hist);
unsigned char percentile(const int *hist, const double p);
void threshold(const float *image, bool *binaryImage, const int numOfRows, const int numOfCols);