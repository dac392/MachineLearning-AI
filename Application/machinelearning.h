#ifndef MACHINELEARNING_H
#define MACHINELEARNING_H

#include <Eigen/Dense>
#include <string>
#include <QProgressDialog>

#include "logisticregression.h"

class MachineLearning {
public:
    MachineLearning(const std::string& datasetPath);
    void loadDataset();
    void train(QProgressDialog& progressDialog);
    double test(double lr, double reg, double thresh);
    int predict(const std::string& diagram);


private:
    std::string path;
    Eigen::MatrixXd X_train;
    Eigen::VectorXd y_train;
    Eigen::MatrixXd X_test;
    Eigen::VectorXd y_test;
    LogisticRegression model;

    int encodeColor(const std::string& color);
    std::pair<Eigen::VectorXd, int> processRow(const std::vector<std::string>& row);
    void addIntercept(Eigen::MatrixXd& X);
    void splitDataset(Eigen::MatrixXd& data, Eigen::VectorXd& labels);
    double evaluateAccuracy(const Eigen::VectorXd& predictions, const Eigen::VectorXd& actual) const;
    void playNotificationSound();

};

#endif // MACHINELEARNING_H
