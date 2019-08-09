#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "cartoon.h"

using namespace cv;
using namespace std;

Cartoon::Cartoon(Mat src) {
    size = src.size();

    gray = Mat(size, CV_8U);
    mask = Mat(size, CV_8U);
    edges = Mat(size, CV_8U);
    edges2 = Mat(size, CV_8U);

    // Get small image, width/8 and height/8
    // Need the smaller image for speed
    smallSize.width = size.width/8;
    smallSize.height = size.height/8;
    smallImg = Mat(smallSize, CV_8UC3);

    tmp = Mat(smallSize, CV_8UC3);

    faceOutline = Mat::zeros(size, CV_8UC3);
    color = CV_RGB(255, 255, 0); // yellow color

    maskPlusBorder = Mat::zeros(smallSize.height+2, smallSize.width+2, CV_8UC1);
}

void Cartoon::cartoonifyImage(Mat srcColor, Mat dist, bool sketchMode, bool alienMode, bool evilMode) {
    // Convert image to gray
    cvtColor(srcColor, gray, CV_BGR2GRAY);
    // Blur image
    medianBlur(gray, gray, 7);

    if (evilMode) {
        Scharr(gray, edges, CV_8U, 1, 0, 1); // x edges
        Scharr(gray, edges2, CV_8U, 1, 0, -1); // y edges
        // cobine x and y
        edges += edges2;
        threshold(edges, mask, 12, 255, THRESH_BINARY_INV);
        medianBlur(mask, mask, 3);
    } else {
        // Get edges using laplacian
        Laplacian(gray, edges, CV_8U, 5);
        // Get edge mask
        threshold(edges, mask, 80, 255, THRESH_BINARY_INV);
    }

    // Resize image, width/2 and height/2
    resize(srcColor, smallImg, smallSize, 0, 0, INTER_LINEAR);

    // Cartonify image
    int repeat = 7;
    int ksize = 9; // Filter size
    double sigmaColor = 9;  // Color strength
    double sigmaSpace = 7;  // Space strenght
    for (int i = 0; i < repeat; i++) {
        // Apply filter
        bilateralFilter(smallImg, tmp, ksize, sigmaColor, sigmaSpace);  
        bilateralFilter(tmp, smallImg, ksize, sigmaColor, sigmaSpace);
    }

    if (alienMode) {
        changeSkinColor();
    }
    
    resize(smallImg, bigImg, size, 0, 0, INTER_LINEAR);
    dist.setTo(0);
    bigImg.copyTo(dist, mask);
    if (alienMode) {
        drawFace(dist);
    }
}

void Cartoon::changeSkinColor() {
    // Convert image to YUV color
    Mat yuv = Mat(smallSize, CV_8UC4);
    cvtColor(smallImg, yuv, CV_BGR2YCrCb);

    // FloodFill must have bordered image
    Mat maskA = maskPlusBorder(Rect(1,1,smallSize.width,smallSize.height));
    resize(edges, maskA, smallSize);

    // Remove weak edges
    threshold(maskA, maskA, 80, 255, THRESH_BINARY);
    // Connect edges together
    dilate(maskA, maskA, Mat());
    erode(maskA, maskA, Mat());

    // Create skin points around the face
     int const NUM_SKIN_POINTS = 6;
    Point skinPts[NUM_SKIN_POINTS];
    skinPts[0] = Point(smallSize.width/2,          smallSize.height/2 - smallSize.height/6);
    skinPts[1] = Point(smallSize.width/2 - smallSize.width/11,  smallSize.height/2 - smallSize.height/6);
    skinPts[2] = Point(smallSize.width/2 + smallSize.width/11,  smallSize.height/2 - smallSize.height/6);
    skinPts[3] = Point(smallSize.width/2,          smallSize.height/2 + smallSize.height/16);
    skinPts[4] = Point(smallSize.width/2 - smallSize.width/9,   smallSize.height/2 + smallSize.height/16);
    skinPts[5] = Point(smallSize.width/2 + smallSize.width/9,   smallSize.height/2 + smallSize.height/16);

    // Find good bounds for FloodFill
    Scalar lowerDiff = Scalar(17, 8, 6);
    Scalar upperDiff = Scalar(20, 4, 4);

    // Create filled mask
    Mat edgeMask = maskA.clone();
    for (int i = 0; i < NUM_SKIN_POINTS; i++) {
        floodFill(yuv, maskPlusBorder, skinPts[i], Scalar(), NULL, lowerDiff, upperDiff, 4 | FLOODFILL_FIXED_RANGE | FLOODFILL_MASK_ONLY);
    }

    // Change color of pixels
    maskA -= edgeMask;
    add(smallImg, Scalar(0, 70, 0), smallImg, maskA);
}

void Cartoon::drawFace(Mat dist) {

    // Face outline parameters
    int outlineThickness = 1;
    int faceHeight = size.height/2 * 70/100;
    int faceWidth = size.width/2 * 36/100;

    // Draw face
    ellipse(faceOutline, Point(size.width/2, size.height/2), Size(faceWidth, faceHeight), 0, 0, 360, color, outlineThickness, CV_AA);

    // Draw text
    int fontFace = FONT_HERSHEY_COMPLEX;
    float fontScale = 1.0f;
    int fontThickness = 2;
    char *sizeMessage = "Put your face here";
    putText(faceOutline, sizeMessage, Point(size.width * 23/100, size.height * 10/100), fontFace, fontScale, color, fontThickness, CV_AA);

    // Overlay face outline
    addWeighted(dist, 1.0, faceOutline, 0.7, 0, dist, CV_8UC3);
}

// Remove noise
void Cartoon::removeNoise() {

}