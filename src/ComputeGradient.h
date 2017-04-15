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
};

#endif // !COMPUTE_GRADIENT