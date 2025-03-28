#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Load an image
    Mat img = imread("shapes.png");
    Mat gray, blurred, edged;

    // Convert to grayscale
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Apply Gaussian Blur
    GaussianBlur(gray, blurred, Size(5, 5), 0);

    // Detect edges using Canny
    Canny(blurred, edged, 50, 150);

    // Find contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(edged, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
        // Approximate contour with accuracy proportional to perimeter
        double epsilon = 0.02 * arcLength(contours[i], true);
        vector<Point> approx;
        approxPolyDP(contours[i], approx, epsilon, true);

        // Draw the approximated contour
        drawContours(img, vector<vector<Point>>{approx}, -1, Scalar(0, 255, 0), 2);

        // Identify shape based on vertices count
        string shape;
        int vertices = (int)approx.size();

        if (vertices == 3)
            shape = "Triangle";
        else if (vertices == 4) {
            // Check if it's a square or rectangle
            Rect rect = boundingRect(approx);
            double aspectRatio = (double)rect.width / rect.height;
            shape = (aspectRatio > 0.95 && aspectRatio < 1.05) ? "Square" : "Rectangle";
        } else if (vertices > 4)
            shape = "Circle";

        // Draw text on the detected shape
        putText(img, shape, approx[0], FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2);
    }

    // Show the result
    imshow("Detected Shapes", img);
    waitKey(0);
    return 0;
}
