#include "MachineLearning.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <iomanip> // for std::setw





#define LEARNING_RATE 0.1
#define ITERATIONS 5000
#define REGULARIZATION_MODIFIER 0.001

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

void printMatrix(const Eigen::MatrixXd& matrix, const std::string& matrixName) {
    std::cout << "Contents of " << matrixName << ":" << std::endl;
    for (int i = 0; i < 1; ++i) {
        // Print the leading 1
        std::cout << std::setw(3) << matrix(i, 0) << std::endl;

        // Print the remaining elements in groups of 20
        for (int j = 1; j < matrix.cols(); ++j) {
            std::cout << std::setw(3) << matrix(i, j) << " ";

            // After every 20 elements (starting from the second element), insert a line break
            if ((j - 1) % 20 == 19)
                std::cout << std::endl;
        }
        std::cout << std::endl; // Extra line break after each row of the matrix
    }


}

void MachineLearning::train(QProgressDialog& progressDialog) {
    std::vector<double> learningRates = {0.1, 0.01, 0.001, 0.0001};
    std::vector<double> regularizationModifiers = {0.01, 0.1, 100, 300, 500, 700, 1000};
    std::vector<double> thresholds = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    double bestAccuracy = 0.0;
    double bestLearningRate = 0.0;
    double bestRegularizationModifier = 0.0;
    double bestThreshold = 0.0;

    for (double thresh : thresholds) {
        for (double lr : learningRates) {
            for (double reg : regularizationModifiers) {
                model.setLearningRate(lr);
                model.setRegularizationStrength(reg);
                model.setThreshold(thresh);

                // Train the model
                model.fit(X_train, y_train, progressDialog);

                // Evaluate the model (you might need a validation set for this)
                double accuracy = test(lr, reg, thresh); // Ensure this function is updated to use the current threshold

                // Update the best parameters
                if (accuracy > bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestLearningRate = lr;
                    bestRegularizationModifier = reg;
                    bestThreshold = thresh;
                }
            }
        }
    }

    // Train final model with best hyperparameters and threshold
    model.setLearningRate(bestLearningRate);
    model.setRegularizationStrength(bestRegularizationModifier);
    model.setThreshold(bestThreshold);
    model.fit(X_train, y_train, progressDialog);
    test(bestLearningRate, bestRegularizationModifier, bestThreshold); // Ensure this function uses the best threshold
}


int MachineLearning::predict(const std::string& diagram) {
    // Parse the input string to extract color and position information
    std::istringstream ss(diagram);
    std::string token;
    //Eigen::VectorXd color_order(4); // Assuming there are 4 color wires
    std::array<std::array<double, 20>, 20> matrix = {};

    int colorOrderIndex = 0;
    while (std::getline(ss, token, ',')) {
        std::istringstream tokenStream(token);
        std::string rowOrCol, color;
        int index;
        tokenStream >> rowOrCol >> index >> color;

        // Convert color to a numerical value
        double colorValue = encodeColor(color);
        //color_order[colorOrderIndex++] = colorValue;

        // Calculate position in the flattened matrix
        int position = (rowOrCol == "Row") ? index : index;
        if (rowOrCol == "Row") {
            std::fill(matrix[position].begin(), matrix[position].end(), colorValue);
        } else {
            for (auto& row : matrix) {
                row[position] = colorValue;
            }
        }
    }

    // Flatten the matrix and append color order
    Eigen::VectorXd feature_vector(400); //+ color_order.size() 400 for flattened matrix + size of color_order
    int index = 0;

    for (const auto& row : matrix) {
        for (double value : row) {
            feature_vector(index++) = value;
        }
    }
    //feature_vector.segment(400, color_order.size()) = color_order;

    // Add an intercept term
    Eigen::VectorXd extendedFeatures(1 + feature_vector.size());
    extendedFeatures << 1, feature_vector;
    std::cout << "Shape of X_predict: " << extendedFeatures.size() << std::endl;
    // Make a prediction
    int prediction = model.singlePrediction(extendedFeatures);

    // Assuming a binary classification and threshold of 0.5
    return prediction;
}


double MachineLearning::test(double lr, double reg, double thresh) {
    auto predictions = model.predict(X_test);
    double accuracy = evaluateAccuracy(predictions, y_test);

    // Updated print statement to include the threshold
    std::cout << "Accuracy: " << accuracy
              << " with learning rate: " << lr
              << ", regularization modifier: " << reg
              << ", and threshold: " << thresh << std::endl;

    return accuracy;
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
    //Eigen::VectorXd color_order(row.size() - 1); // Assuming color_order should be of size row.size() - 1
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
        //color_order[i] = color; // Using Eigen vector for color_order

        if (parts[0] == "Row") {
            std::fill(matrix[position].begin(), matrix[position].end(), color);
        } else {
            for (auto& row : matrix) {
                row[position] = color;
            }
        }
    }

    // Flatten the matrix and append color order
    Eigen::VectorXd feature_vector(400); // + color_order.size()400 for flattened matrix + size of color_order
    int index = 0;

    for (const auto& row : matrix) {
        for (double value : row) {
            feature_vector(index++) = value;
        }
    }

    // Append color order to feature vector
    // feature_vector.segment(400, color_order.size()) = color_order;

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

    addIntercept(X_train);
    addIntercept(X_test);
}





