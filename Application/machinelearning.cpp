#include "MachineLearning.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>


#define LEARNING_RATE 0.001
#define ITERATIONS 10000
#define REGULARIZATION_MODIFIER 600

MachineLearning::MachineLearning(const std::string& datasetPath)
    : path(datasetPath), model(LEARNING_RATE, ITERATIONS, REGULARIZATION_MODIFIER, RegularizationType::L1) {}

void MachineLearning::loadDataset() {
    std::ifstream file(path);
    std::string line;
    std::vector<Eigen::VectorXd> temp_data;
    std::vector<int> temp_labels;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        auto [features, label] = processRow(row);
        temp_data.push_back(features);
        temp_labels.push_back(label);
    }

    // Convert vectors to Eigen matrix and vector
    int num_samples = temp_data.size();
    int num_features = temp_data.front().size();
    Eigen::MatrixXd data(num_samples, num_features);
    Eigen::VectorXd labels(num_samples);

    for (int i = 0; i < num_samples; ++i) {
        data.row(i) = temp_data[i];
        labels(i) = temp_labels[i];
    }

    // Split the dataset
    splitDataset(data, labels);
}

void MachineLearning::addIntercept(Eigen::MatrixXd& X) {
    Eigen::MatrixXd newX(X.rows(), X.cols() + 1);
    newX << Eigen::VectorXd::Ones(X.rows()), X;
    X = newX;
}

void MachineLearning::train(QProgressDialog& progressDialog) {
    // add intercepts
    addIntercept(X_train);
    addIntercept(X_test);

    // train
    model.fit(X_train, y_train, progressDialog);

}

int MachineLearning::predict(Eigen::MatrixXd& diagram){
    auto prediction = model.predict(diagram);
    return (prediction(0) > 0.5) ? 1 : 0;
}

void MachineLearning::test(){
    auto predictions = model.predict(X_test);
    double accuracy = evaluateAccuracy(predictions, y_test);
    std::cout << "Accuracy: " << accuracy << std::endl;
}



double MachineLearning::evaluateAccuracy(const Eigen::VectorXd& predictions, const Eigen::VectorXd& actual) const {
    if (predictions.size() != actual.size()) {
        std::cerr << "Error: Size of predictions and actual labels must be the same." << std::endl;
        return 0.0;
    }

    int correct = 0;
    for (int i = 0; i < predictions.size(); ++i) {
        if (predictions(i) == actual(i)) {
            correct++;
        }
    }

    return static_cast<double>(correct) / predictions.size();
}


int MachineLearning::encodeColor(const std::string& color) {
    if (color == "Red") return 1;
    if (color == "Green") return 2;
    if (color == "Yellow") return 3;
    if (color == "Blue") return 4;
    return 0;
}

std::pair<Eigen::VectorXd, int> MachineLearning::processRow(const std::vector<std::string>& row) {
    std::array<std::array<double, 20>, 20> matrix = {};
    Eigen::VectorXd color_order(row.size() - 1); // Assuming color_order should be of size row.size() - 1
    int label = (row.back().find("Dangerous") != std::string::npos) ? 1 : 0;

    for (size_t i = 0; i < row.size() - 1; ++i) {
        std::stringstream ss(row[i]);
        std::string part;
        std::vector<std::string> parts;

        while (std::getline(ss, part, ' ')) {
            parts.push_back(part);
        }

        int position = std::stoi(parts[1]) - 1;
        int color = encodeColor(parts[2]);
        color_order[i] = color; // Using Eigen vector for color_order

        if (parts[0] == "Row") {
            std::fill(matrix[position].begin(), matrix[position].end(), color);
        } else {
            for (auto& row : matrix) {
                row[position] = color;
            }
        }
    }

    // Flatten the matrix and append color order
    Eigen::VectorXd feature_vector(400 + color_order.size()); // 400 for flattened matrix + size of color_order
    int index = 0;

    for (const auto& row : matrix) {
        for (double value : row) {
            feature_vector(index++) = value;
        }
    }

    // Append color order to feature vector
    feature_vector.segment(400, color_order.size()) = color_order;

    // Return a pair of feature vector and label
    return std::make_pair(feature_vector, label);
}

void MachineLearning::splitDataset(Eigen::MatrixXd& data, Eigen::VectorXd& labels) {
    int num_samples = data.rows();
    int train_size = static_cast<int>(num_samples * 0.8);
    int test_size = num_samples - train_size;

    // Shuffle the dataset
    std::vector<int> indices(num_samples);
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    // Split the dataset
    X_train.resize(train_size, data.cols());
    y_train.resize(train_size);
    X_test.resize(test_size, data.cols());
    y_test.resize(test_size);

    for (int i = 0; i < train_size; ++i) {
        X_train.row(i) = data.row(indices[i]);
        y_train(i) = labels(indices[i]);
    }

    for (int i = 0; i < test_size; ++i) {
        X_test.row(i) = data.row(indices[train_size + i]);
        y_test(i) = labels(indices[train_size + i]);
    }
}

