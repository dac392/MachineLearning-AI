#include "MachineLearning.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define LEARNING_RATE 0.001
#define ITERATIONS 30000
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
    X_train.resize(num_samples, num_features);
    y_train.resize(num_samples);

    for (int i = 0; i < num_samples; ++i) {
        X_train.row(i) = temp_data[i];
        y_train(i) = temp_labels[i];
    }

    addIntercept(X_train);
}

void MachineLearning::addIntercept(Eigen::MatrixXd& X) {
    Eigen::MatrixXd newX(X.rows(), X.cols() + 1);
    newX << Eigen::VectorXd::Ones(X.rows()), X;
    X = newX;
}

void MachineLearning::train() {
    // load the data
    loadDataset();

    // add intercepts
    addIntercept(X_train);

    // train
    model.fit(X_train, y_train);
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

