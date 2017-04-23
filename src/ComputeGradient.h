#ifndef COMPUTE_GRADIENT
#define COMPUTE_GRADIENT

#include "ofMain.h"

class ComputeGradient : public ofThread {
public:
    void setup(const int w, const int h, const int i);

    void threadedFunction();

    bool done_;
    ofPixels pixelData_;
    int w_, h_, iterations_;

private:
    void solveEigen(const float* gradient, const float* boundary);
    void solveGaussSeidel(const float* gradient, const float* boundary);

    void gaussSeidelStep(float* I, const float* boundary, const int idx);
};

#endif // !COMPUTE_GRADIENT