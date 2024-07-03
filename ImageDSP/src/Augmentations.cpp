
#include "Augmentations.h"
#include "ColorSpaces.h"
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <list>

#define PI 3.14159265359
#define _USE_MATH_DEFINES
#define CONVERGENCE_THRESHOLD 1e-8

static const int NUM_COLORS = 16;
const uchar RGBColors[NUM_COLORS][3] = { { 255, 0, 0 },{ 0, 255, 0 },{ 0, 0, 255 },{ 0, 255, 255 },{ 255, 255, 0 },{ 255, 0, 255 },{ 0, 0, 0 },{ 255, 255, 255 },{ 128, 0, 0 },{ 0, 128, 0 },{ 0, 0, 128 },{ 0, 128, 128 },{ 128, 128, 0 },{ 128, 0, 128 },{ 128, 128, 128 },{ 64, 128, 64 } };

using namespace std;

//Augmentation 1
void HueOffset(
    const uchar input[],
    int w,
    int h,
    float hueOffset,
    float saturationScale,
    uchar output[]
    ) {

    uchar H_buff[w * h];
    uchar S_buff[w * h];
    uchar V_buff[w * h];

    RGBtoHSV(input, w, h, H_buff, S_buff, V_buff);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float H = H_buff[y * w + x];
            float S = S_buff[y * w + x];
            float V = V_buff[y * w + x];

            H += hueOffset;
            if (H < 0)
                H += 360;
            if (H >= 360)
                H -= 360;

            S *= saturationScale;
            if (S < 0)
                S = 0;
            if (S > 255)
                S = 255;

            H_buff[y * w + x] = static_cast<uchar>(H);
            S_buff[y * w + x] = static_cast<uchar>(S);
        }
    }

    HSVtoRGB(H_buff, S_buff, V_buff, w, h, output);
}

//Augmentation 2

void cropImage(
    const uchar* input,
    int x_size,
    int y_size,
    uchar* output,
    float new_x_size,
    float new_y_size,
    float center_x,
    float center_y
    ) {
    int bytesPerPixel = 3;

    int start_x = center_x - new_x_size / 2;
    int start_y = center_y - new_y_size / 2;

    for (int y = 0; y < new_y_size; y++) {
        for (int x = 0; x < new_x_size; x++) {
            // Calculate the input and output indices based on the starting coordinates
            int inputIndex = ((start_y + y) * x_size + (start_x + x)) * bytesPerPixel;
            int outputIndex = (y * new_x_size + x) * bytesPerPixel;

            // Copy pixel values from input to output
            for (int i = 0; i < bytesPerPixel; i++) {
                output[outputIndex + i] = input[inputIndex + i];
            }
        }
    }
}

int random(int min, int max){
    return (rand() % (max - min + 1) + min);
}

void cropAndStretchImage(
    const uchar* input,
    int x_size,
    int y_size,
    uchar* output,
    float new_x_size,
    float new_y_size,
    float center_x,
    float center_y
    ) {
    int bytesPerPixel = 3;

    int start_x = center_x - new_x_size / 2;
    int start_y = center_y - new_y_size / 2;

    // Randomly determine the stretching factors
    /*srand(static_cast<unsigned int>(time(0)));
    float stretch_x = 1.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (3.0f - 1.0f);
    float stretch_y = 1.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (2.0f - 1.0f);*/

    float stretch_x = 3.0f;
    float stretch_y = 1.00001f;

    for (int y = 0; y < new_y_size; y++) {
        for (int x = 0; x < new_x_size; x++) {
            // Calculate the input coordinates based on the starting coordinates and stretching factors
            float input_x = (start_x + x) / stretch_x;
            float input_y = (start_y + y) / stretch_y;

            // Calculate the input and output indices based on the input coordinates
            int inputIndex = (static_cast<int>(input_y) * x_size + static_cast<int>(input_x)) * bytesPerPixel;
            int outputIndex = (y * new_x_size + x) * bytesPerPixel;

            // Copy pixel values from input to output
            for (int i = 0; i < bytesPerPixel; i++) {
                output[outputIndex + i] = input[inputIndex + i];
            }
        }
    }
}


//Augmentation 3
void Occlusions(
    const uchar* input,
    uchar* output,
    int x_size,
    int y_size,
    int center_x,
    int center_y,
    int occlusion_width,
    int occlusion_height
    ) {
    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            // Calculate the index of the current pixel
            int index = (y * x_size + x) * 3;

            // Check if the current pixel is inside the crop region
            if (x >= center_x && x < center_x + occlusion_width && y >= center_y && y < center_y + occlusion_height) {
                // Set the pixel to black in the output image
                output[index] = 0;        // Red channel
                output[index + 1] = 0;    // Green channel
                output[index + 2] = 0;    // Blue channel
            } else {
                // Copy the pixel from the input image to the output image
                output[index] = input[index];
                output[index + 1] = input[index + 1];
                output[index + 2] = input[index + 2];
            }
        }
    }
}

//Augmentation 4

void GenerateDCTmatrix(double* DCTKernel, int order)
{
    int i, j;
    int N;
    N = order;
    double alpha;
    double denominator;
    for (j = 0; j <= N - 1; j++)
    {
        DCTKernel[0, j] = sqrt(1 / (double)N);
    }
    alpha = sqrt(2 / (double)N);
    denominator = (double)2 * N;
    for (j = 0; j <= N - 1; j++)
        for (i = 1; i <= N - 1; i++)
        {
            DCTKernel[i*N + j] = alpha * cos(((2 * j + 1) * i * PI) / denominator);
        }
}

void DCT(const uchar input[], short output[], int N, double* DCTKernel)
{
    double* temp = new double[N*N];
    double* DCTCoefficients = new double[N*N];

    double sum;
    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + DCTKernel[i*N+k] * (input[k*N+j]-128.0);
            }
            temp[i*N + j] = sum;
        }
    }

    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + temp[i*N+k] * DCTKernel[j*N+k];
            }
            DCTCoefficients[i*N+j] = sum;
        }
    }

    for(int i = 0; i < N*N; i++)
    {
        output[i] = floor(DCTCoefficients[i]+0.5);
    }

    delete[] temp;
    delete[] DCTCoefficients;

    return;
}

void IDCT(const short input[], uchar output[], int N, double* DCTKernel)
{

    double* temp = new double[N*N];
    double* DCTCoefficients = new double[N*N];

    double sum;
    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + DCTKernel[k*N+i] * (input[k*N+j]);
            }
            temp[i*N + j] = sum;
        }
    }

    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + temp[i*N+k] * DCTKernel[k*N+j];
            }
            DCTCoefficients[i*N+j] = sum;
        }
    }

    for(int i = 0; i < N*N; i++)
    {
        output[i] = floor(DCTCoefficients[i]+0.5)+128;
    }

    delete[] temp;
    delete[] DCTCoefficients;

    return;
}

void extendBorders(uchar* input, int xSize, int ySize, int N, uchar** p_output, int* newXSize, int* newYSize)
{
    int deltaX = N-xSize%N;
    int deltaY = N-ySize%N;

    *newXSize = xSize+deltaX;
    *newYSize = ySize+deltaY;

    uchar* output = new uchar[(xSize+deltaX)*(ySize+deltaY)];

    for (int i=0; i<ySize; i++)
    {
        memcpy(&output[i*(xSize+deltaX)], &input[i*(xSize)], xSize);
        if(deltaX != 0)
        {
            memset(&output[i*(xSize+deltaX)+xSize], output[i*(xSize+deltaX)+xSize - 1], deltaX);
        }
    }

    for (int i=0; i<deltaY; i++)
    {
        memcpy(&output[(i + ySize)*(xSize+deltaX)], &output[(ySize)*(xSize+deltaX)], xSize+deltaX);
    }

    *p_output = output;
}

void cropImage(uchar* input, int xSize, int ySize, uchar* output, int newXSize, int newYSize)
{
    for (int i=0; i<newYSize; i++)
    {
        memcpy(&output[i*(newXSize)], &input[i*(xSize)], newXSize);
    }
}

static void doZigZag(short block[], int N, int B)
{

    int currNum = 0;
    int currDiagonalWidth = 1;
    int i;
    int row;
    int col;

    while (currDiagonalWidth<N)
    {
        for (i = 0; i<currDiagonalWidth; i++)
        {
            if (currDiagonalWidth % 2 == 1)
            {
                row = currDiagonalWidth - i - 1;
                col = i;
            }
            else
            {
                row = i;
                col = currDiagonalWidth - i - 1;
            }

            if(currNum > B)
                block[row*N + col] = 0;
            currNum++;
        }
        currDiagonalWidth++;
    }
    while (currDiagonalWidth> 0)
    {
        for (i = currDiagonalWidth; i> 0; i--)
        {
            if (currDiagonalWidth % 2 == 1){
                row = N - currDiagonalWidth + i - 1;
                col = N - i;
            }
            else
            {
                row = N - i;
                col = N - currDiagonalWidth + i - 1;
            }
            if(currNum > B)
                block[row*N + col] = 0;
            currNum++;
        }

        currDiagonalWidth--;
    }

}


void performMaskDCTCoeffs(uchar input[], int xSize, int ySize, int N, int B)
{
    uchar* inBlock = new uchar[N*N];

    double* DCTKernel = new double[N*N];
    GenerateDCTmatrix(DCTKernel, N);

    short* dctCoeffs = new short[N*N];

    int xSize2, ySize2;
    uchar* input2;
    extendBorders(input, xSize , ySize, N, &input2, &xSize2, &ySize2);

    for (int y = 0; y < ySize2/N; y++)
    {
        for (int x = 0; x < xSize2/N; x++)
        {
            for (int j=0; j<N; j++)
            {
                for (int i=0; i<N; i++)
                {
                    inBlock[j*N+i] = input2[(N*y+j)*(xSize2)+N*x+i];
                }
            }

            DCT(inBlock, dctCoeffs, N, DCTKernel);

            doZigZag(dctCoeffs, N, B);

            IDCT(dctCoeffs, inBlock, N, DCTKernel);

            for (int j=0; j<N; j++)
            {
                for (int i=0; i<N; i++)
                {
                    input2[(N*y+j)*(xSize2)+N*x+i] = inBlock[j*N+i];
                }
            }
        }
    }

    cropImage(input2, xSize2, ySize2, input, xSize, ySize);

    delete[] dctCoeffs;
    delete[] inBlock;
    delete[] DCTKernel;
    delete[] input2;

}

void PerformDCT(
    uchar input[],
    int x_size,
    int y_size,
    int B,
    double strength
    ){
    int N = 8;
    uchar* orig = new uchar[x_size * y_size];

    memcpy(orig, input, x_size*y_size);

    performMaskDCTCoeffs(input, x_size, y_size, N, B);

}

// ColorBasedKMeans segmentation

static double getDistance(
    const vector<double>& a,
    const vector<double>& b
    ) {
    double temp = 0.0;
    auto it1 = a.begin();
    auto it2 = b.begin();
    for(; (it1 != a.end()) && (it2 != b.end()); it1++, it2++){
        temp += (*it1 - *it2) * (*it1 - *it2);
    }
    return sqrt(temp);
}

// Find nearest cluster for the given point
static int whichIsNearest(
    const vector<KMeansPoint>& centroids,
    KMeansPoint pt
    ) {
    double minDistance = 0;
    int minLabel = 0;
    for(size_t i = 0; i < centroids.size(); i++){
        double tempDistance = getDistance(centroids[i].feature, pt.feature);
        if(i == 0 || tempDistance < minDistance){
            minDistance = tempDistance;
            minLabel = i;
        }
    }
    return minLabel;
}

static vector<vector<int>> kMeans(
    const vector<KMeansPoint>& points,
    int nFeatures,
    int K
    ) {
    // Total number of points
    int nSamples = points.size();

    // Randomize initial centroids
    vector<KMeansPoint> centroids;
    for(int k = 0; k < K; k++){
        int rand_int = rand() % nSamples;
        centroids.push_back(points[rand_int]);
    }

    // Create empty vector for each cluster
    vector<vector<int> > cluster;
    for(int k = 0; k < K; k++){
        vector<int> vectemp;
        cluster.push_back(vectemp);
    }

    // Iteration counter
    int counter = 0;

    // Iteratively find better centroids
    while(true){

        // Clear each cluster
        for(int k = 0; k < K; k++){
            cluster[k].clear();
        }

        // Set convergence flag to TRUE
        bool converge = true;

        // For every sample, find which cluster it belongs to,
        // By comparing the distance between it and each clusters' centroid.
        for(int m = 0; m < nSamples; m++){
            int which = whichIsNearest(centroids, points[m]);
            cluster[which].push_back(m);
        }

        // For every cluster, re-calculate its centroid.
        for(int k = 0; k < K; k++){
            int clusterSize = cluster[k].size();

            vector<double> vectemp = vector<double>(nFeatures, 0);
            for(int i = 0; i < clusterSize; i++) {
                for(int j = 0; j < nFeatures; j++)
                    vectemp[j] =
                        vectemp[j] +
                        points[cluster[k][i]].feature[j] / (double)clusterSize;
            }
            // If centroid position change is greater then
            // CONVERGENCE_THRESHOLD set convergence flag to false.
            if(
                getDistance(vectemp, centroids[k].feature)
                > CONVERGENCE_THRESHOLD
                ){
                converge = false;
            }



            centroids[k].feature = vectemp;
        }

        // If convergence achieved break the loop
        if(converge){
            break;
        }
        counter++;
    }

    // Return cluster vectors
    return cluster;
}

void ColorBasedKMeans(
    uchar input[],
    int x_size,
    int y_size,
    int K,
    uchar output[]
    ) {
    const int numberOfFeatures = 3;

    // Calculate feature vector for each point
    vector<KMeansPoint> points;
    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            KMeansPoint newPoint = { i, j };
            newPoint.feature.push_back(input[j * 3 * x_size + i * 3]);
            newPoint.feature.push_back(input[j * 3 * x_size + i * 3 + 1]);
            newPoint.feature.push_back(input[j * 3 * x_size + i * 3 + 2]);

            points.push_back(newPoint);
        }
    }

    vector<vector<int>> clusters;
    clusters = kMeans(points, numberOfFeatures, K);

    // Paint each segment using predefined colors
    for (int k = 0; k < K; k++) {
        for (int index : clusters[k]) {
            int i = points[index].x;
            int j = points[index].y;
            output[3 * j * x_size + 3 * i] = RGBColors[k][0];
            output[3 * j * x_size + 3 * i + 1] = RGBColors[k][1];
            output[3 * j * x_size + 3 * i + 2] = RGBColors[k][2];
        }
    }
}


