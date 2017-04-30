#include "ofApp.h"
#include <iostream>
#include <vector>
#include <cmath>

//--------------------------------------------------------------
void ofApp::setup() {
    computing_ = false;
    sampleSetupFlag_ = false;
    rgbSetupFlag_ = false;
    compType_ = CompType::UNSET;

	ofBackground(ofColor(40));

	gui_.setup();
	gui_.add(loadButton_.setup("Load"));
	gui_.add(saveButton_.setup("Save"));
	gui_.add(invButton_.setup("Inverse"));
    gui_.add(srcButton_.setup("Gradient from source"));
    gui_.add(genButton_.setup("Example gradient"));
    options_.setup();
    options_.setName("Options");
    options_.add(toggle_.setup("Use Gauss Seidel", true));
    options_.add(iterations_.setup("Iterations", 1000, 0, 5000));
    options_.add(size_.setup("Size", ofVec2f(100), ofVec2f(0), ofVec2f(1000)));
    gui_.add(&options_);    

    loadButton_.addListener(this, &ofApp::loadImage);
    saveButton_.addListener(this, &ofApp::saveImage);
    invButton_.addListener(this, &ofApp::inverseImage);
    genButton_.addListener(this, &ofApp::generateGradient);
    srcButton_.addListener(this, &ofApp::sourceGradient);
}

//--------------------------------------------------------------
void ofApp::update() {
    if (compType_ == CompType::GRAYSCALE) {
        if (computeGradient_.done) {
            if (computeGradient_.pixelData.isAllocated()) {
                img_.setFromPixels(computeGradient_.pixelData);
            }
            computeGradient_.done = false;
            computing_ = false;
        }
    }
    else if (compType_ == CompType::RGB) {
        if (computers_[0].done && computers_[1].done && computers_[2].done) {
            const int width = computers_[0].pixelData.getWidth();
            const int height = computers_[0].pixelData.getHeight();
            const int channels = 3;

            ofPixels pixelData;
            pixelData.allocate(width, height, channels);

            for (int i = 0; i < width * height; i++) {
                pixelData[channels * i + 0] = computers_[0].pixelData[i];
                pixelData[channels * i + 1] = computers_[1].pixelData[i];
                pixelData[channels * i + 2] = computers_[2].pixelData[i];
            }

            img_.setFromPixels(pixelData);

            computers_[0].done = computers_[1].done = computers_[2].done = false;
            computing_ = false;
        }
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

    if (computing_) {
        ofDrawBitmapString("Computing gradient...", 250, 25);
    }

	gui_.draw();
}

void ofApp::exit() {
    if (sampleSetupFlag_) {
        delete[] sampleSetup_.gradient;
        delete[] sampleSetup_.boundary;
    }
    if (rgbSetupFlag_) {
        for (int i = 0; i < 3; i++) {
            delete[] rgbSetup_[i].gradient;
            delete[] rgbSetup_[i].boundary;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
    case 'G':
        this->sourceGradient();
        break;
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
    if (computing_) {
        std::cerr << "Error: Computation in progress!" << std::endl;
        return;
    }
        
    computing_ = true;
    compType_ = CompType::GRAYSCALE;

    int width = std::round(size_->x);
    int height = std::round(size_->y);

    if (sampleSetupFlag_) {
        delete[] sampleSetup_.gradient;
        delete[] sampleSetup_.boundary;
    }
    sampleSetup_.gradient = new float[width * height];
    sampleSetup_.boundary = new float[2 * (width + height)];
    sampleSetupFlag_ = true;

    // prepare sample gradient
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (j == height / 2 && i > width / 3 && i < width * 2 / 3.0f)
                sampleSetup_.gradient[j * width + i] = 1;
            else
                sampleSetup_.gradient[j * width + i] = 0;
        }
    }

    // prepare sample boundary
    for (int i = 0; i < 2 * (width + height); i++) {
        if (i < 2 * width + height)
            sampleSetup_.boundary[i] = 0;
        else
            sampleSetup_.boundary[i] = 1;
    }

    sampleSetup_.width = width;
    sampleSetup_.height = height;
    sampleSetup_.iterations = iterations_;
    sampleSetup_.useGaussSeidel = toggle_;

    computeGradient_.setup(sampleSetup_);
    computeGradient_.startThread();

}

void ofApp::sourceGradient(void) {
    if (!img_.isAllocated()) {
        std::cerr << "Error: No source image loaded!" << std::endl;
        return;
    }
    if (computing_) {
        std::cerr << "Error: Computation in progress!" << std::endl;
        return;
    }
    
    computing_ = true;
    compType_ = CompType::RGB;

    if (rgbSetupFlag_) {
        for (int i = 0; i < 3; i++) {
            delete[] rgbSetup_[i].gradient;
            delete[] rgbSetup_[i].boundary;
        }
    }
    rgbSetupFlag_ = true;

    const int width = img_.getWidth();
    const int height = img_.getHeight();
    const ofPixels pixelData = img_.getPixels();
    const int channels = pixelData.getNumChannels();

    auto setupChannel = [&](const int idx) {
        rgbSetup_[idx].width = width;
        rgbSetup_[idx].height = height;
        rgbSetup_[idx].useGaussSeidel = toggle_;
        rgbSetup_[idx].iterations = iterations_;

        rgbSetup_[idx].gradient = new float[width * height];

        for (int i = 0; i < width * height; i++) {
            rgbSetup_[idx].gradient[i] = pixelData[channels * i + idx] / 255.0f;
        }

        rgbSetup_[idx].boundary = new float[2 * (width + height)];

        for (int i = 0; i < width; i++) {
            rgbSetup_[idx].boundary[i] = pixelData[channels * i + idx] / 255.0f;
            rgbSetup_[idx].boundary[width + height + i] = pixelData[channels * (width * height - 1 - i) + idx] / 255.0f;
        }
        for (int i = 0; i < height; i++) {
            rgbSetup_[idx].boundary[width + i] = pixelData[channels * (((i + 1)* width) - 1) + idx] / 255.0f;
            rgbSetup_[idx].boundary[2 * width + height + i] = pixelData[channels * (width * (height - 1 - i)) + idx] / 255.0f;
        }

        computers_[idx].setup(rgbSetup_[idx]);
        computers_[idx].startThread();
    };

    setupChannel(0);
    setupChannel(1);
    setupChannel(2);
}
