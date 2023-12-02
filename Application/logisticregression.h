#ifndef LOGISTICREGRESSION_H
#define LOGISTICREGRESSION_H

#include <Eigen/Dense>
#include <vector>

enum class RegularizationType {
    None,
    L1,
    L2
};

class LogisticRegression {
public:
    LogisticRegression(double learningRate, int iterations, double regularizationStrength, RegularizationType regType = RegularizationType::None);

    void fit(const Eigen::MatrixXd& X, const Eigen::VectorXd& y);
    Eigen::VectorXd predict(const Eigen::MatrixXd& X) const;
    Eigen::VectorXd getWeights() const { return weights; }

private:
    double learningRate;
    int iterations;
    double regularizationStrength;
    RegularizationType regType;
    Eigen::VectorXd weights;

    static double sigmoid(double z);
    double computeCost(const Eigen::MatrixXd& X, const Eigen::VectorXd& y) const;
    Eigen::VectorXd computeGradient(const Eigen::MatrixXd& X, const Eigen::VectorXd& y) const;
};

#endif // LOGISTICREGRESSION_H
