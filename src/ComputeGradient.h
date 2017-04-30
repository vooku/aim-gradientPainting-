#ifndef COMPUTE_GRADIENT
#define COMPUTE_GRADIENT

#include "ofMain.h"

class ComputeGradient : public ofThread {
public:
    void setup(const int w, const int h, const int i, const bool gs);

    void threadedFunction();

    bool done;
    ofPixels pixelData;
    int width, height, iterations;

private:
    void solveEigen(const float* gradient, const float* boundary);
    void solveGaussSeidel(const float* gradient, const float* boundary);
    void gaussSeidelStep(float* I, const float* boundary, const int idx);

    bool useGaussSeidel;
};

#endif // !COMPUTE_GRADIENT