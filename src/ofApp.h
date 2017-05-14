#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ComputeGradient.h"
#include <string>

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void exit();

	void keyReleased(int key);

	void loadImage(void);
	void saveImage(void);
	void inverseImage(void);
    void generateGradient(void);
    void sourceGradient(void); ///< Works for color input only!!!
		
private:
	ofImage img_;
	string originalFileExtension_;

    ComputeGradient computeGradient_;
    ComputeGradient computers_[3];
    bool* mask_;
    enum class CompType {UNSET, GRAYSCALE, RGB};
    CompType compType_;
    bool computing_;

    ComputeGradient::GradientSetup sampleSetup_;
    bool sampleSetupFlag_;
    ComputeGradient::GradientSetup rgbSetup_[3];
    bool rgbSetupFlag_;

	// gui
	ofxPanel gui_;
	ofxButton loadButton_, saveButton_, invButton_, genButton_, srcButton_;
    ofxVec2Slider size_;
    ofxGuiGroup options_;
    ofxToggle solverToggle_;
    ofxToggle boundaryToggle_;
    ofxIntSlider iterations_;
};
