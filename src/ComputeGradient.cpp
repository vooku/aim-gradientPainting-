#include "ComputeGradient.h"
#include "Eigen/Sparse"
#include "Eigen/IterativeLinearSolvers"

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
    std::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd b(w_ * h_);
    int n0, n1, n2, n3;
    for (int i = 0; i < w_ * h_; i++) {
        b[i] = gradient[i];
        getNeighbours(n0, n1, n2, n3, i, w_, h_);
        triplets.push_back(Eigen::Triplet<double>(i, i, -4));
        if (n0 >= 0)
            triplets.push_back(Eigen::Triplet<double>(i, n0, 1));
        if (n1 >= 0)
            triplets.push_back(Eigen::Triplet<double>(i, n1, 1));
        if (n2 >= 0)
            triplets.push_back(Eigen::Triplet<double>(i, n2, 1));
        if (n3 >= 0)
            triplets.push_back(Eigen::Triplet<double>(i, n3, 1));
    }
    
    // solve matrix
    Eigen::SparseMatrix<double> A(w_ * h_, w_ * h_);
    A.setFromTriplets(triplets.begin(), triplets.end());
    A.makeCompressed();
    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>> solver(A);

    if (solver.info() != Eigen::Success) {
        std::cerr << "Decomposition failed" << std::endl;
    }
    else {
        Eigen::VectorXd x = solver.solve(b);
        if (solver.info() != Eigen::Success) {
            std::cerr << "No convergence" << std::endl;
        }
        else {
            pixelData_.allocate(w_, h_, 1);
            float maxVal = -HUGE_VALF;
            for (int i = 0; i < w_ * h_; i++) {
                if (abs(x[i]) > maxVal)
                    maxVal = abs(x[i]);
            }
            for (int i = 0; i < w_ * h_; i++) {
                pixelData_[i] = floor(abs(x[i]) / maxVal * 255);
            }
        }
    }

    // cleanup
    delete[] gradient;
    done_ = true;
}