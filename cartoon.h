#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void cartoonifyImage(Mat srcColor, Mat dist, bool sketchMode, 
    bool alienMode, bool evilMode, int debugType);

void drawFace(Mat dist);

class Cartoon {
    public:
        void cartoonifyImage(Mat srcColor, Mat dist, bool sketchMode, bool alienMode, bool evilMode);

        void drawFace(Mat dist);
        void changeSkinColor();
        void removeNoise();

        Cartoon(Mat src);
    private:
        Mat gray;
        Mat mask;
        Mat edges;
        Mat edges2;
        Size size;
        Size smallSize;
        Mat smallImg;
        Mat tmp;
        Mat bigImg;
        Mat faceOutline;
        Scalar color;

        // For alien, mask with borders, for floodFill
        Mat maskPlusBorder;
};