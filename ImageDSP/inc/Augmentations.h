
#pragma once

#include <QDebug>
#include <QtGlobal>
#include <vector>

struct KMeansPoint {
    //point coordinates
    int x;
    int y;

    //feature vector
    std::vector<double> feature;

    KMeansPoint(int x_, int y_)
        : x(x_), y(y_) {

    }
};

void HueOffset(
    const uchar input[],
    int w,
    int h,
    float hueOffset,
    float saturationScale,
    uchar output[]
);

void Occlusions(
    const uchar* input,
    uchar* output,
    int x_size,
    int y_size,
    int center_x,
    int center_y,
    int occlusion_width,
    int occlusion_height
);

int random(int min, int max);

void cropAndStretchImage(
    const uchar* input,
    int x_size,
    int y_size,
    uchar* output,
    float new_x_size,
    float new_y_size,
    float center_x,
    float center_y
);

void GenerateDCTmatrix
    (double* DCTKernel,
    int order
);

void DCT(
    const uchar input[],
    short output[],
    int N,
    double* DCTKernel
);

void IDCT(
    const short input[],
    uchar output[],
    int N,
    double* DCTKernel
);

void extendBorders(
    uchar* input,
    int xSize,
    int ySize,
    int N,
    uchar** p_output,
    int* newXSize,
    int* newYSize
);

void cropImage(
    uchar* input,
    int xSize,
    int ySize,
    uchar* output,
    int newXSize,
    int newYSize
);

static void doZigZag(
    short block[],
    int N,
    int B
);

void performMaskDCTCoeffs(
    uchar input[],
    int xSize,
    int ySize,
    int N,
    int B
);

void PerformDCT(
    uchar input[],
    int x_size,
    int y_size,
    int B,
    double strength
);

void cropImage(
    const uchar* input,
    int x_size,
    int y_size,
    uchar* output,
    float new_x_size,
    float new_y_size,
    float center_x,
    float center_y
    );

void ColorBasedKMeans(
    uchar input[],
    int x_size,
    int y_size,
    int K,
    uchar output[]
);
