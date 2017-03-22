#include "ofApp.h"
#include <iostream>

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(ofColor(40));

	loadButton.addListener(this, &ofApp::loadImage);
	saveButton.addListener(this, &ofApp::saveImage);
	invButton.addListener(this, &ofApp::inverseImage);

	gui.setup();
	gui.add(loadButton.setup("Load"));
	gui.add(saveButton.setup("Save"));
	gui.add(invButton.setup("Inverse"));

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	if (img.isAllocated()) {
		if (img.getWidth() > ofGetWidth()) {
			img.draw(0, 0, ofGetWidth(), img.getHeight() * ofGetWidth() / img.getWidth());
		}
		else img.draw(0, 0);
	}

	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::loadImage(void){
	//D:\Program Files\openFrameworks\apps\myApps\gradientPaint\bin\data\img
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
		img.save(saveFileResult.filePath + "." + originalFileExtension);
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
			pixelData[3 * (i + j * width) + 0] = 255 - pixelData[3 * (i + j * width) + 0];
			pixelData[3 * (i + j * width) + 1] = 255 - pixelData[3 * (i + j * width) + 1];
			pixelData[3 * (i + j * width) + 2] = 255 - pixelData[3 * (i + j * width) + 2];
		}

	}
	img.setFromPixels(pixelData);
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
