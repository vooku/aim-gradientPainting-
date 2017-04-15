#include "ComputeGradient.h"

void getNeighbours(int & n0, int & n1, int & n2, int & n3, const int idx, const int w, const int h) {
    if (idx < w) // first row
        n0 = -1;
    else
        n0 = idx - w;

    if ((idx + 1) % w == 0) // last column
        n1 = -1;
    else
        n1 = idx + 1;

    if (idx >= w * h - w) // last row
        n2 = -1;
    else
        n2 = idx + w;

    if (idx % w == 0) // first column
        n3 = -1;
    else
        n3 = idx - 1;
}

void ComputeGradient::gaussSeidelStep(float* I, const float* b, const int idx) {
    int n0, n1, n2, n3;
    getNeighbours(n0, n1, n2, n3, idx, w_, h_);

    I[idx] = 0;
    I[idx] += I[n0] >= 0 ? I[n0] : 0;
    I[idx] += I[n1] >= 0 ? I[n1] : 0;
    I[idx] += I[n2] >= 0 ? I[n2] : 0;
    I[idx] += I[n3] >= 0 ? I[n3] : 0;
    I[idx] -= b[idx];
    I[idx] /= 4.0f;
}

ComputeGradient::~ComputeGradient(void) {
    pixelData_.clear();
}

void ComputeGradient::setup(const int w, const int h) {
    done_ = false;
    w_ = w;
    h_ = h;
}

void ComputeGradient::threadedFunction() {
    if (w_ <= 0 || h_ <= 0) {
        std::cerr << "Wrong image size: " << w_ << ", " << h_ << std::endl;
        done_ = true;
        return;
    }
    // prepare the desired gradient
    float* gradient = new float[w_ * h_];
    for (int j = 0; j < h_; j++) {
        for (int i = 0; i < w_; i++) {
            if (j == h_ / 2 && i > w_ / 3 && i < w_ * 2 / 3.0f)
                gradient[j * w_ + i] = 255;
            else
                gradient[j * w_ + i] = 0;
        }
    }

    // construct the matrix
    float* I = new float[w_ * h_];
    float* b = new float[w_ * h_];
    for (int i = 0; i < w_ * h_; i++) {
        I[i] = gradient[i];
        // TODO generate non-black borders
        b[i] = gradient[i]; // - ...?
    }


    // solve matrix
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < w_ * h_; j++) {
            this->gaussSeidelStep(I, b, j);
        }
    }

    pixelData_.allocate(w_, h_, 1);

    for (int i = 0; i < w_ * h_; i++) {
        pixelData_[i] = I[i];
    }

    // cleanup
    delete[] gradient;
    delete[] I;
    delete[] b;
    done_ = true;
}
