import numpy as np
import matplotlib.pyplot as plt

class LogisticRegression:
    def __init__(self, learning_rate=0.01, iterations=1000, regularization_strength=0.1):
        self.learning_rate = learning_rate
        self.iterations = iterations
        self.regularization_strength = regularization_strength
        self.weights = None
        self.loss_history = []

    def sigmoid(self, z):
        return 1 / (1 + np.exp(-z))

    def compute_cost(self, X, y):
        m = X.shape[0]
        y_pred = self.sigmoid(np.dot(X, self.weights))
        cost = -(1/m) * np.sum(y * np.log(y_pred) + (1 - y) * np.log(1 - y_pred))
        # L1 Regularization
        l1_penalty = (self.regularization_strength / m) * np.sum(np.abs(self.weights))
        return cost + l1_penalty

    def gradient_descent(self, X, y):
        m = X.shape[0]
        for _ in range(self.iterations):
            y_pred = self.sigmoid(np.dot(X, self.weights))
            dw = (1/m) * np.dot(X.T, (y_pred - y))
            # Subgradient of L1
            dw_reg = np.sign(self.weights) * self.regularization_strength / m
            self.weights -= self.learning_rate * (dw + dw_reg)
            self.loss_history.append(self.compute_cost(X, y))

    def fit(self, X, y):
        self.weights = np.zeros(X.shape[1])
        self.gradient_descent(X, y)

    def predict(self, X):
        y_pred = self.sigmoid(np.dot(X, self.weights))
        y_pred_class = [1 if i > 0.5 else 0 for i in y_pred]
        return np.array(y_pred_class)

    def plot_loss(self):
        plt.plot(range(self.iterations), self.loss_history, label='Training Loss')
        plt.xlabel('Iterations')
        plt.ylabel('Loss')
        plt.title('Loss Over Iterations')
        plt.legend()
        plt.show()
