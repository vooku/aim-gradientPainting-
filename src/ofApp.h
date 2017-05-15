#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ComputeGradient.h"
#include <string>


/** \brief Standard <a href="http://openframeworks.cc/">openFrameworks</a> class defining the application.
 */
class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void exit();

	void keyReleased(int key);

	void loadImage(void);
	void saveImage(void);
	void inverseImage(void); ///< Originally used to test basic pixel manipulation.

    /** \brief Generates example gradient.
     *
     * The example gradient is a white line in the middle of the image and
     * the boundary conditions (if #boundaryToggle_ is set to true) are all
     * black except for the left side, which is all white.
     *
     * The image size is given by #size_ and the number of iterations for 
     * Gauss Seidel iterational solver is given by #iterations_.
     *
     * The output image is grayscale and if used as input for sourceGradient(),
     * the behaviour is undefined.
     */
    void generateGradient(void);

    /** \brief Generates a gradient from an already loaded image, replacing it when done.
     *
     * The output image has the same dimensions as the input image, the #size_ slider
     * is ignored. the number of iterations for Gauss Seidel iterational solver 
     * is given by #iterations_.
     *
     * If #boundaryToggle_ is set to true, the pixels on the edges of the image
     * are copied to the boundary. Otherwise zero derivation boundary is used.
     *
     * Works for color input only.
     */
    void sourceGradient(void);
		
private:
	ofImage img_;
	string originalFileExtension_;

    ComputeGradient computeGradient_; ///< Used for grayscale images.

    /** \brief Used for color images.
     * 
     * The channels are computed independently by separate threads.
     */
    ComputeGradient computers_[3];
    enum class CompType { UNSET, GRAYSCALE, RGB };
    bool* mask_; ///< Masks the input pixels, which shall remain unchanged.
    
    CompType compType_; ///< Carries the information whether the desired picture is grayscale or in color.
    bool computing_; ///< Used to indicate state of the app.

    ComputeGradient::GradientSetup sampleSetup_;
    bool sampleSetupFlag_; ///< Indicates whether #sampleSetup_ stores any allocated data.
    ComputeGradient::GradientSetup rgbSetup_[3];
    bool rgbSetupFlag_; ///< Indicates whether #rgbSetup_ stores any allocated data.

	// gui
	ofxPanel gui_; ///< Graphical user interface panel.
    ofxButton loadButton_; ///< Calls loadImage() when pressed.
    ofxButton saveButton_; ///< Calls saveImage() when pressed.
    ofxButton invButton_; ///< Calls inverseImage() when pressed.
    ofxButton genButton_; ///< Calls generateGradient() when pressed.
    ofxButton srcButton_; ///< Calls sourceGradient() when pressed.
    ofxGuiGroup options_; ///< Holds #size_, #solverToggle_, #boundaryToggle_ and #iterations_.
    ofxVec2Slider size_; ///< Sets the dimensions for the next gradient computed by generateGradient().
    ofxToggle solverToggle_; ///< Use iterative or direct solver. Currently removed from the gui.
    ofxToggle boundaryToggle_; ///< Use Dirichlet or zero derivation boundary conditions.
    ofxIntSlider iterations_; ///< Number of iterations for iterational solver.
};
