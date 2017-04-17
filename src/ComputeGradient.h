#ifndef COMPUTE_GRADIENT
#define COMPUTE_GRADIENT

#include "ofMain.h"

class ComputeGradient : public ofThread {
public:
    void setup(const int w, const int h);

    void threadedFunction();

    bool done_;
    ofPixels pixelData_;
    int w_, h_;

private:
    void solveEigen(const float* gradient, const float* boundary);
    void solveGaussSeidel(const float* gradient, const float* boundary);

    void gaussSeidelStep(float* I, const float* b, const int idx);
};

#endif // !COMPUTE_GRADIENT