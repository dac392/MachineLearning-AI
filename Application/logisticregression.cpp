#include "logisticregression.h"
#include <cmath>
#include <QProgressDialog>
#include <QApplication>


LogisticRegression::LogisticRegression(double lr, int iter, double regStrength, RegularizationType regType)
    : learningRate(lr), iterations(iter), regularizationStrength(regStrength), regType(regType) {}

void LogisticRegression::fit(const Eigen::MatrixXd& X, const Eigen::VectorXd& y, QProgressDialog& progressDialog) {
    size_t n_features = X.cols();
    weights = Eigen::VectorXd::Random(n_features); // Small random initialization

    progressDialog.setRange(0, iterations);
    progressDialog.setValue(0);

    for (int i = 0; i < iterations; ++i) {
        Eigen::VectorXd gradients = computeGradient(X, y);
        weights -= learningRate * gradients;

        // Update the progress dialog
        progressDialog.setValue(i);
        QApplication::processEvents();

        // Check if the operation was canceled
        if (progressDialog.wasCanceled()) {
            break;
        }
    }

    progressDialog.setValue(iterations); // Indicate completion
}


Eigen::VectorXd LogisticRegression::predict(const Eigen::MatrixXd& X) const {
    Eigen::VectorXd predictions = (X * weights).unaryExpr(&LogisticRegression::sigmoid);
    return (predictions.array() > 0.5).cast<double>();
}

double LogisticRegression::sigmoid(double z) {
    return 1.0 / (1.0 + exp(-z));
}

int LogisticRegression::singlePrediction(const Eigen::VectorXd& extendedFeatures) {
    // Assuming that 'weights' is the trained model parameters
    double linearCombination = extendedFeatures.dot(weights);
    double probability = 1.0 / (1.0 + exp(-linearCombination));

    return (probability > 0.5) ? 1 : 0;  // Return 1 for 'Dangerous', 0 for 'Safe'
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

