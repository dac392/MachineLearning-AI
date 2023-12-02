#ifndef MACHINELEARNING_H
#define MACHINELEARNING_H

#include <Eigen/Dense>
#include <string>

#include "logisticregression.h"

class MachineLearning {
public:
    MachineLearning(const std::string& datasetPath);
    void loadDataset();
    void train();

private:
    std::string path;
    Eigen::MatrixXd X_train;
    Eigen::VectorXd y_train;
    LogisticRegression model;

    int encodeColor(const std::string& color);
    std::pair<Eigen::VectorXd, int> processRow(const std::vector<std::string>& row);
    void addIntercept(Eigen::MatrixXd& X);
};

#endif // MACHINELEARNING_H
