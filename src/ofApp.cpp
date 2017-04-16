#include "ofApp.h"
#include <iostream>
#include <vector>
#include <cmath>

//--------------------------------------------------------------
void ofApp::setup() {
    computing_ = false;
	ofBackground(ofColor(40));

	loadButton_.addListener(this, &ofApp::loadImage);
	saveButton_.addListener(this, &ofApp::saveImage);
	invButton_.addListener(this, &ofApp::inverseImage);
    genButton_.addListener(this, &ofApp::generateGradient);

	gui_.setup();
	gui_.add(loadButton_.setup("Load"));
	gui_.add(saveButton_.setup("Save"));
	gui_.add(invButton_.setup("Inverse"));
    gui_.add(genButton_.setup("Generate gradient"));
    gui_.add(size_.setup("Next generated image size", ofVec2f(100), ofVec2f(0), ofVec2f(200)));
}

//--------------------------------------------------------------
void ofApp::update() {
    if (computeGradient_.done_) {
        if (computeGradient_.pixelData_.isAllocated()) {
            img_.setFromPixels(computeGradient_.pixelData_);
        }
        computeGradient_.done_ = false;
        computing_ = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (img_.isAllocated()) {
		if (img_.getWidth() > ofGetWidth()) {
			img_.draw(0, 0, ofGetWidth(), img_.getHeight() * ofGetWidth() / img_.getWidth());
		}
		else img_.draw((ofGetWidth() - img_.getWidth()) / 2, (ofGetHeight() - img_.getHeight()) / 2);
	}

    if (computing_)
        ofDrawBitmapString("Computing gradient...", 250, 25);

	gui_.draw();
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
            originalFileExtension_ = fileExtension;
            img_.load(openFileResult.getPath());
		}
	}
}

void ofApp::saveImage(void){
	if (!img_.isAllocated()) {
		return;
	}

	ofFileDialogResult saveFileResult = ofSystemSaveDialog(ofGetTimestampString() + "." + originalFileExtension_, "Save your file");
	if (saveFileResult.bSuccess) {
		if (!originalFileExtension_.empty())
            img_.save(saveFileResult.filePath + "." + originalFileExtension_);
        else
            img_.save(saveFileResult.filePath);
	}
}

void ofApp::inverseImage(void){
	if (!img_.isAllocated()) {
		return;
	}

	int width = img_.getWidth();
	ofPixels pixelData = img_.getPixels();

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < img_.getHeight(); j++) {
            int channels = pixelData.getNumChannels();
            for (int k = 0; k < channels; k++) {
                pixelData[channels * (i + j * width) + k] = 255 - pixelData[channels * (i + j * width) + k];
            }
		}

	}
	img_.setFromPixels(pixelData);
}

void ofApp::generateGradient(void) {
    computing_ = true;
    computeGradient_.setup(std::round(size_->x), std::round(size_->y));
    computeGradient_.startThread();
}
