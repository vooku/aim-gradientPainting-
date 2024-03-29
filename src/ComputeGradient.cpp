#include "ComputeGradient.h"
#include "Eigen/Sparse"
#include "Eigen/IterativeLinearSolvers"

#define NORMALIZE ///< Used in Eigen direct solver to normalize out of bounds results.

/** \brief Returns positions of neighbours for the given pixel index.
 * \param[in]   idx         Index of the input pixel
 * \param[in]   w           Image width
 * \param[in]   h           Image height
 * \param[out]  neighbours  Array of length 4 for the neighbours in the order
 *                          top, right, bottom, left.
 * \param[out]  bounds      Array of length 4 indicating whether the neighbour 
 *                          is in the boundary or in the image.
 */
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

void ComputeGradient::setup(const GradientSetup& gradientSetup) {
    done = false;
    parametres_ = gradientSetup;
}

void ComputeGradient::threadedFunction() {
    if (parametres_.width <= 0 || parametres_.height <= 0) {
        std::cerr << "Wrong image size: " << parametres_.width << ", " << parametres_.height << std::endl;
        done = true;
        return;
    }
    
    if (parametres_.useGaussSeidel) {
        this->solveGaussSeidel();
    }
    else {
        this->solveDirect();
    }
        
    done = true;
}

void ComputeGradient::solveDirect(void) {
    // construct the matrix
    std::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd b(parametres_.width * parametres_.height);
    int neighbours[4];
    bool bounds[4];

    for (int i = 0; i < parametres_.width * parametres_.height; i++) {
        b[i] = parametres_.gradient[i];

        getNeighbours(neighbours, bounds, i, parametres_.width, parametres_.height);
        triplets.push_back(Eigen::Triplet<double>(i, i, -4));

        auto addElement = [&](const int idx) {
            if (bounds[idx])
                b[i] += parametres_.boundary[neighbours[idx]];
            else
                triplets.push_back(Eigen::Triplet<double>(i, neighbours[idx], 1));
        };

        addElement(0);
        addElement(1);
        addElement(2);
        addElement(3);
    }

    // solve matrix
    Eigen::SparseMatrix<double> A(parametres_.width * parametres_.height, parametres_.width * parametres_.height);
    A.setFromTriplets(triplets.begin(), triplets.end());
    A.makeCompressed();
    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>> solver(A);

    if (solver.info() != Eigen::Success) {
        std::cerr << "Error: Decomposition failed." << std::endl;
    }
    else {
        Eigen::VectorXd x = solver.solve(b);
        if (solver.info() != Eigen::Success) {
            std::cerr << "Error: No convergence." << std::endl;
        }
        else {
            pixelData.allocate(parametres_.width, parametres_.height, 1);
#ifdef NORMALIZE
            float maxVal = -HUGE_VALF;
            for (int i = 0; i < parametres_.width * parametres_.height; i++) {
                if (abs(x[i]) > maxVal)
                    maxVal = abs(x[i]);
            }
            for (int i = 0; i < parametres_.width * parametres_.height; i++) {
                pixelData[i] = abs(x[i]) / maxVal * 255;
            }
#else 
            for (int i = 0; i < width * height; i++) {
                pixelData[i] = x[i] * 255;
            }
#endif
        }
    }
}

void ComputeGradient::solveGaussSeidel(void) {
    // construct the matrix
    float* I = new float[parametres_.width * parametres_.height];
    for (int i = 0; i < parametres_.width * parametres_.height; i++) {
        I[i] = parametres_.gradient[i];
    }

    // solve matrix
    auto gaussSeidelStep = [&](float * I, const int idx) {
        int neighbours[4];
        bool bounds[4];

        getNeighbours(neighbours, bounds, idx, parametres_.width, parametres_.height);
  
        float value = 0;
        for (int i = 0; i < 4; i++) {
            value += !bounds[i] ? I[neighbours[i]] : parametres_.givenBoundary ? parametres_.boundary[neighbours[i]] : I[idx];
        };
        I[idx] = value / 4;
    };

    for (int i = 0; i < parametres_.iterations; i++) {
        for (int j = 0; j < parametres_.width * parametres_.height; j++) {
            if (!parametres_.mask[j])
                gaussSeidelStep(I, j);
        }
    }

    pixelData.allocate(parametres_.width, parametres_.height, 1);

    for (int i = 0; i < parametres_.width * parametres_.height; i++) {
        pixelData[i] = I[i] * 255;
    }
    
    delete[] I;
}
