#include "ofApp.h"
#include <iostream>
#include <vector>
#include <cmath>

//--------------------------------------------------------------
void ofApp::setup() {
    computing = false;
	ofBackground(ofColor(40));

	loadButton.addListener(this, &ofApp::loadImage);
	saveButton.addListener(this, &ofApp::saveImage);
	invButton.addListener(this, &ofApp::inverseImage);
    genButton.addListener(this, &ofApp::generateGradient);

	gui.setup();
	gui.add(loadButton.setup("Load"));
	gui.add(saveButton.setup("Save"));
	gui.add(invButton.setup("Inverse"));
    gui.add(genButton.setup("Generate gradient"));
    gui.add(size.setup("Next generated image size", ofVec2f(100), ofVec2f(0), ofVec2f(1000)));
}

//--------------------------------------------------------------
void ofApp::update() {
    if (computeGradient.done_) {
        img.setFromPixels(computeGradient.pixelData_);
        computeGradient.done_ = false;
        computing = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (img.isAllocated()) {
		if (img.getWidth() > ofGetWidth()) {
			img.draw(0, 0, ofGetWidth(), img.getHeight() * ofGetWidth() / img.getWidth());
		}
		else img.draw((ofGetWidth() - img.getWidth()) / 2, (ofGetHeight() - img.getHeight()) / 2);
	}

    if (computing)
        ofDrawBitmapString("Computing gradient...", 250, 25);

	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
    case 'g':
        this->generateGradient();
        break;
    case 'i':
        this->inverseImage();
        break;
    case 'l':
        this->loadImage();
        break;
    case 's':
        this->saveImage();
        break;
    default:
        break;
    }
}

void ofApp::loadImage(void){

    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a jpg or png");

	if (openFileResult.bSuccess) {
		ofFile file(openFileResult.getPath());
		string fileExtension = ofToLower(file.getExtension());

		if (fileExtension == "jpg" || fileExtension == "png") {

			//Save the file extension to use when we save out
			originalFileExtension = fileExtension;

			//Load the selected image
			img.load(openFileResult.getPath());
		}
	}
}

void ofApp::saveImage(void){
	if (!img.isAllocated()) {
		return;
	}

	ofFileDialogResult saveFileResult = ofSystemSaveDialog(ofGetTimestampString() + "." + originalFileExtension, "Save your file");
	if (saveFileResult.bSuccess) {
		if (!originalFileExtension.empty())
            img.save(saveFileResult.filePath + "." + originalFileExtension);
        else
            img.save(saveFileResult.filePath);
	}
}

void ofApp::inverseImage(void){
	if (!img.isAllocated()) {
		return;
	}

	int width = img.getWidth();
	ofPixels pixelData = img.getPixels();

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < img.getHeight(); j++) {
            int channels = pixelData.getNumChannels();
            for (int k = 0; k < channels; k++) {
                pixelData[channels * (i + j * width) + k] = 255 - pixelData[channels * (i + j * width) + k];
            }
		}

	}
	img.setFromPixels(pixelData);
}

void ofApp::generateGradient(void) {
    computing = true;
    computeGradient.setup(std::round(size->x), std::round(size->y));
    computeGradient.startThread();
}
