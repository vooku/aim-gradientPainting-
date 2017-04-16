#include "ComputeGradient.h"
#include "Eigen/Sparse"
#include "Eigen/IterativeLinearSolvers"

void getNeighbours(int* neighbours, bool* bounds, const int idx, const int w, const int h) {
    // first row
    if (idx < w) {
        neighbours[0] = idx;
        bounds[0] = true;
    }
    else {
        neighbours[0] = idx - w;
        bounds[0] = false;
    }
    // last column
    if ((idx + 1) % w == 0) {
        neighbours[1] = w + (idx + 1) / w - 1;
        bounds[1] = true;
    }
    else {
        neighbours[1] = idx + 1;
        bounds[1] = false;
    }
    // last row
    if (idx >= w * h - w) {
        neighbours[2] = w + h + (w - 1 - idx % w);
        bounds[2] = true;
    }
    else {
        neighbours[2] = idx + w;
        bounds[2] = false;
    }
    // first column
    if (idx % w == 0) {
        neighbours[3] = 2 * w + h + (h - 1 - idx / w);
        bounds[3] = true;
    }
    else {
        neighbours[3] = idx - 1;
        bounds[3] = false;
    }
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
                gradient[j * w_ + i] = 1;
            else
                gradient[j * w_ + i] = 0;
        }
    }

    // prepare the boundary
    float* boundary = new float[2 * (w_ + h_)];
    for (int i = 0; i < 2 * (w_ + h_); i++) {
        if (i < w_ + h_)
            boundary[i] = 0;
        else
            boundary[i] = 1;
    }

    // construct the matrix
    std::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd b(w_ * h_);
    int neighbours[4];
    bool bounds[4];

    for (int i = 0; i < w_ * h_; i++) {
        b[i] = gradient[i];

        getNeighbours(neighbours, bounds, i, w_, h_);
        triplets.push_back(Eigen::Triplet<double>(i, i, -4));
        if (bounds[0])
            b[i] -= boundary[neighbours[0]];
        else
            triplets.push_back(Eigen::Triplet<double>(i, neighbours[0], 1));
        if (bounds[1])
            b[i] -= boundary[neighbours[1]];
        else
            triplets.push_back(Eigen::Triplet<double>(i, neighbours[1], 1));
        if (bounds[2])
            b[i] -= boundary[neighbours[2]];
        else
            triplets.push_back(Eigen::Triplet<double>(i, neighbours[2], 1));
        if (bounds[3])
            b[i] -= boundary[neighbours[3]];
        else
            triplets.push_back(Eigen::Triplet<double>(i, neighbours[3], 1));
    }
    
    // solve matrix
    Eigen::SparseMatrix<double> A(w_ * h_, w_ * h_);
    A.setFromTriplets(triplets.begin(), triplets.end());
    //A.makeCompressed();
    //Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>> solver(A);
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper> solver;
    solver.compute(A);
    Eigen::VectorXd x = solver.solve(b);

   /* if (solver.info() != Eigen::Success) {
        std::cerr << "Decomposition failed" << std::endl;
    }
    else {
        Eigen::VectorXd x = solver.solve(b);*/
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
             //   pixelData_[i] = floor(abs(x[i]) / maxVal * 255);
                pixelData_[i] = x[i] * 255;
            }
            //std::cout << x << std::endl;
        }
   // }

    // cleanup
    delete[] gradient;
    delete[] boundary;
    done_ = true;
}