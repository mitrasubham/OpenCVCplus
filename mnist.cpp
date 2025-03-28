#include <iostream>
#include <opencv2/opencv.hpp>
#include <tensorflow/c/c_api.h> 
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/protobuf/meta_graph.pb.h>

using namespace cv;
using namespace std;
using namespace tensorflow;

// Function to load a TensorFlow model
Status LoadModel(const string& model_path, unique_ptr<Session>& session) {
    SessionOptions session_options;
    session.reset(NewSession(session_options));

    GraphDef graph_def;
    Status status = ReadBinaryProto(Env::Default(), model_path, &graph_def);
    if (!status.ok()) {
        return status;
    }

    status = session->Create(graph_def);
    return status;
}

// Function to preprocess an MNIST image for TensorFlow model
Tensor PreprocessImage(const string& image_path) {
    Mat img = imread(image_path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cerr << "Error: Cannot read image!" << endl;
        exit(1);
    }

    // Resize image to 28x28
    resize(img, img, Size(28, 28));

    // Normalize pixel values (0-1)
    img.convertTo(img, CV_32F, 1.0 / 255);

    // Reshape to match TensorFlow input shape (1, 28, 28, 1)
    Tensor input_tensor(DT_FLOAT, TensorShape({1, 28, 28, 1}));
    auto tensor_mapped = input_tensor.tensor<float, 4>();

    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            tensor_mapped(0, i, j, 0) = img.at<float>(i, j);
        }
    }

    return input_tensor;
}

int main() {
    string model_path = "lenet_mnist.pb";  // Pre-trained TensorFlow model file
    string image_path = "digit.png";       // Input handwritten digit image

    // Load TensorFlow model
    unique_ptr<Session> session;
    Status status = LoadModel(model_path, session);
    if (!status.ok()) {
        cerr << "Error loading model: " << status.ToString() << endl;
        return -1;
    }

    // Preprocess image
    Tensor input_tensor = PreprocessImage(image_path);

    // Run inference
    vector<Tensor> outputs;
    status = session->Run({{"input_layer", input_tensor}}, {"output_layer"}, {}, &outputs);
    if (!status.ok()) {
        cerr << "Error running inference: " << status.ToString() << endl;
        return -1;
    }

    // Get predicted digit
    auto predictions = outputs[0].flat<float>();
    int predicted_digit = distance(predictions.data(), max_element(predictions.data(), predictions.data() + 10));

    cout << "Predicted Digit: " << predicted_digit << endl;

    // Show the input image
    Mat img = imread(image_path, IMREAD_GRAYSCALE);
    resize(img, img, Size(200, 200));
    imshow("Input Image", img);
    waitKey(0);

    return 0;
}
