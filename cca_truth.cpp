#include <opencv2/opencv.hpp>
#include <iostream>
#include "dataPath.hpp"
using namespace std;
using namespace cv;

int main() {
    // Load the input image
    cv::Mat img = cv::imread(DATA_PATH + "images/truth.png");

    if (img.empty()) {
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Threshold the image (convert to binary)
    cv::Mat binary;
    cv::threshold(gray, binary, 100, 255, cv::THRESH_BINARY_INV);

    // Apply connected components analysis

    cv::Mat labels, stats, centroids;
    int num_components = cv::connectedComponentsWithStats(binary, labels, stats, centroids);

    // Create an output image for visualization
    cv::Mat output = img.clone();

    // Random colors for labeling
    std::vector<cv::Vec3b> colors(num_components);
    colors[0] = cv::Vec3b(0, 0, 0); // Background color
    for (int i = 1; i < num_components; i++) {
        colors[i] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);
    }

    // Color the components
    for (int y = 0; y < labels.rows; y++) {
        for (int x = 0; x < labels.cols; x++) {
            int label = labels.at<int>(y, x);
            if (label > 0) {
                output.at<cv::Vec3b>(y, x) = colors[label];
            }
        }
    }

    // Draw bounding boxes and labels
    for (int i = 1; i < num_components; i++) {
        int x = stats.at<int>(i, cv::CC_STAT_LEFT);
        int y = stats.at<int>(i, cv::CC_STAT_TOP);
        int w = stats.at<int>(i, cv::CC_STAT_WIDTH);
        int h = stats.at<int>(i, cv::CC_STAT_HEIGHT);

        // Draw bounding box
        cv::rectangle(output, cv::Point(x, y), cv::Point(x + w, y + h), cv::Scalar(0, 255, 0), 2);

        // Put label number
        cv::putText(output, std::to_string(i), cv::Point(x, y - 5), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
    }

    // Show results
    cv::imshow("Connected Components", output);
    cv::waitKey(0);
    return 0;
}
