#include "logisticregression.h"
#include <cmath>
#include <Eigen/Dense>

LogisticRegression::LogisticRegression(double lr, int iter, double regStrength, RegularizationType regType)
    : learningRate(lr), iterations(iter), regularizationStrength(regStrength), regType(regType) {}

void LogisticRegression::fit(const Eigen::MatrixXd& X, const Eigen::VectorXd& y) {
    size_t n_features = X.cols();
    weights = Eigen::VectorXd::Random(n_features); // Small random initialization

    for (int i = 0; i < iterations; ++i) {
        Eigen::VectorXd gradients = computeGradient(X, y);
        weights -= learningRate * gradients;
    }
}

Eigen::VectorXd LogisticRegression::predict(const Eigen::MatrixXd& X) const {
    Eigen::VectorXd predictions = (X * weights).unaryExpr(&LogisticRegression::sigmoid);
    return (predictions.array() > 0.5).cast<double>();
}

double LogisticRegression::sigmoid(double z) {
    return 1.0 / (1.0 + exp(-z));
}

double LogisticRegression::computeCost(const Eigen::MatrixXd& X, const Eigen::VectorXd& y) const {
    Eigen::VectorXd predictions = (X * weights).unaryExpr(&LogisticRegression::sigmoid);
    double cost = -(y.array() * predictions.array().log() + (1 - y.array()) * (1 - predictions.array()).log()).mean();

    // Add regularization term
    double regTerm = 0.0;
    if (regType == RegularizationType::L1) {
        regTerm = weights.array().abs().sum();
    } else if (regType == RegularizationType::L2) {
        regTerm = weights.squaredNorm();
    }
    cost += (regularizationStrength / (2 * X.rows())) * regTerm;

    return cost;
}

Eigen::VectorXd LogisticRegression::computeGradient(const Eigen::MatrixXd& X, const Eigen::VectorXd& y) const {
    Eigen::VectorXd predictions = (X * weights).unaryExpr(&LogisticRegression::sigmoid);
    Eigen::VectorXd gradients = X.transpose() * (predictions - y) / X.rows();

    // Regularization gradients
    if (regType == RegularizationType::L1) {
        for (int i = 0; i < weights.size(); ++i) {
            gradients[i] += regularizationStrength * (weights[i] > 0 ? 1 : -1) / X.rows();
        }
    } else if (regType == RegularizationType::L2) {
        gradients += regularizationStrength * weights / X.rows();
    }

    return gradients;
}

