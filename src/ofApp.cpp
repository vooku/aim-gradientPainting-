#include "ofApp.h"
#include "Eigen/Sparse"
#include "Eigen/IterativeLinearSolvers"
#include <iostream>
#include <vector>
#include <cmath>

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

	w = 300;
	h = 200;
	gradient = new unsigned char[w * h];
	for (int j = 0; j < h; j++)	{
		for (int i = 0; i < w; i++) {
			if (j == h / 2 && i > w / 3 && i < w * 2 / 3)
				gradient[j * w + i] = 1;
			else
				gradient[j * w + i] = 0;
		}
	}

	std::vector<Eigen::Triplet<double>> triplets;
	Eigen::VectorXd b(w * h);
	int n0, n1, n2, n3;

	for (int i = 0; i < w * h; i++) {
		b[i] = gradient[i];

		getNeighbours(n0, n1, n2, n3, i, w, h);

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
	
	Eigen::SparseMatrix<double> A(w * h, w * h);
	A.setFromTriplets(triplets.begin(), triplets.end());
	Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper> solver;
	//solver.setMaxIterations(10000);
	solver.compute(A);
	if (solver.info() != Eigen::Success) {
        std::cerr << "Decomposition failed" << std::endl;
	}
	else {
		Eigen::VectorXd x = solver.solve(b);

		if (solver.info() != Eigen::Success) {
            std::cerr << "No convergence" << std::endl;
		}
        else {
            ofPixels pixelData;
            pixelData.allocate(w, h, 1);

            float maxVal = -HUGE_VALF;
            for (int i = 0; i < w * h; i++) {
                if (abs(x[i]) > maxVal)
                    maxVal = abs(x[i]);
            }
            for (int i = 0; i < w * h; i++) {
                pixelData[i] = floor(abs(x[i]) / maxVal * 255);
            }
            img.setFromPixels(pixelData);
        }
	}
    

	delete[] gradient;
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
		else img.draw((ofGetWidth() - img.getWidth()) / 2, (ofGetHeight() - img.getHeight()) / 2);
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

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
