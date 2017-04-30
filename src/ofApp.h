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

	//void keyPressed(int key);
	void keyReleased(int key);
	//void mouseMoved(int x, int y );
	//void mouseDragged(int x, int y, int button);
	//void mousePressed(int x, int y, int button);
	//void mouseReleased(int x, int y, int button);
	//void mouseEntered(int x, int y);
	//void mouseExited(int x, int y);
	//void windowResized(int w, int h);
	//void dragEvent(ofDragInfo dragInfo);
	//void gotMessage(ofMessage msg);

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
    ofxToggle toggle_;
    ofxIntSlider iterations_;
};
