#ifndef COMPUTE_GRADIENT
#define COMPUTE_GRADIENT

#include "ofMain.h"

class ComputeGradient : public ofThread {
public:
    struct GradientSetup {
        int width, height;
        int iterations;
        float* gradient;
        float* boundary;
        bool useGaussSeidel;
    };

    void setup(const GradientSetup& gradientSetup);

    void threadedFunction();

    bool done;
    ofPixels pixelData;    

private:
    void solveEigen(void);
    void solveGaussSeidel(void);

    GradientSetup parametres_;
};

#endif // !COMPUTE_GRADIENT