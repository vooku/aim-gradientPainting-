#pragma once

#include "ofMain.h"
/** \brief Class implementing gradient computation
 *
 * Uses either Eigen library for a direct solver or Gauss-Seidel iterative solver.
 */
class ComputeGradient : public ofThread {
public:
    /** \brief Parametres for the desired gradient.
     */
    struct GradientSetup {
        int width, height;
        int iterations;
        bool* mask; ///< Masks which pixels must remain constant (usefull when using threads for separate channels).
        float* gradient; ///< The input gradient.
        float* boundary; ///< Boundary conditions. Used only when #givenBoundary is true.
        bool useGaussSeidel; ///< Indicates whether to use iterational or direct solver.
        bool givenBoundary; ///< Indicates whether to use the given #boundary or zero derivation boundary.
    };

    /** Sets all the needed parametres.
     */
    void setup(const GradientSetup& gradientSetup);

    /** \brief This is a virtual function derived from the base ofThread.
     *
     * Based on the #parametres_ calls either solveDirect() or solveGaussSeidel().
     */
    virtual void threadedFunction();

    bool done; ///< Indicates whether the computation finished.
    ofPixels pixelData; ///< Stores the result.

private:
    /** Solves for the gradient using direct solver.
     * Implemented using Eigen library.
     * Not currently used because of weird results.
     */
    void solveDirect(void);
    
    /** Solves for the gradient using iterational solver -- Gauss Seidel method.
     */
    void solveGaussSeidel(void);

    GradientSetup parametres_;
};
