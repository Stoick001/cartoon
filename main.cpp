#include <ctime>
#include <stdio.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/hal/hal.hpp>

#include "cartoon.h"

using namespace cv;

bool EVIL_MODE = false;
bool ALIEN_MODE = false;

const char *windowName = "Cartoonifier";
 
int main(int argc, char** argv ) {
  std::cout << "Enter 'c' for cartoon filter" << std::endl
            << "Enter 'e' for evil filter" << std::endl
            << "Enter 'a' for alien filter" << std::endl;

  int cameraNumber = -1;
  if (argc > 1) {
    cameraNumber = atoi(argv[1]);
  }

  // Access the camera
  VideoCapture camera;
  camera.open(cameraNumber);
  if (!camera.isOpened()) {
    std::cerr << "ERROR: Could not access the camera!" << std::endl;
    exit(1);
  }

  // Set resolution
  camera.set(CAP_PROP_FRAME_WIDTH, 640);
  camera.set(CAP_PROP_FRAME_HEIGHT, 480);

  //VideoWriter video("outcpp.avi",CV_FOURCC('M','J','P','G'),10, Size(camera.get(CV_CAP_PROP_FRAME_WIDTH),camera.get(CV_CAP_PROP_FRAME_HEIGHT)));

  Mat cameraFrame;
  camera >> cameraFrame;

  // Empty 
  Mat displayFrame = Mat(cameraFrame.size(), CV_8UC3);

  Cartoon cartoon (cameraFrame);

  namedWindow(windowName);
  while(true) {

    // Get next frame
    camera >> cameraFrame;
    if (cameraFrame.empty()) {
      std::cerr << "ERROR: Could not grab the camera frame." << std::endl;
      exit(1);
    }

    // Apply filter to image
    cartoon.cartoonifyImage(cameraFrame, displayFrame, false, ALIEN_MODE, EVIL_MODE);

    imshow(windowName, displayFrame);

    // Set framerate
    char keypress = waitKey(1);
    if (keypress == 27) {
      break;
    } else if (keypress == 'e') {
      EVIL_MODE = true;
      ALIEN_MODE = false;
    } else if (keypress == 'c') {
      EVIL_MODE = false;
      ALIEN_MODE = false;
    } else if (keypress == 'a') {
      EVIL_MODE = false;
      ALIEN_MODE = true;
    }
    
  }
  camera.release();

  return 0;
}