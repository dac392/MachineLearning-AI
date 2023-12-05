
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from LogisticRegression import LogisticRegression  # Make sure to import the revised LogisticRegression class

def encode_color(color):
    return {'Red': 1, 'Green': 2, 'Yellow': 3, 'Blue': 4}.get(color, 0)

def process_row(row):
    matrix = np.zeros((20, 20))
    features = row[:-1]
    label = 1 if row[-1].strip() == 'Dangerous' else 0

    color_order = []
    for feature in features:
        parts = feature.split()
        position = int(parts[1]) - 1
        color = encode_color(parts[2])
        color_order.append(color)

        if parts[0] == 'Row':
            matrix[position, :] = color
        else:  # Column
            matrix[:, position] = color

    # Feature Engineering: Including color order as part of the features
    feature_vector = np.append(matrix.flatten(), color_order)
    return feature_vector, label

def load_data(file_path):
    df = pd.read_csv(file_path, header=None)
    data = df.values

    X, y = [], []
    for row in data:
        feature_vector, label = process_row(row)
        X.append(feature_vector)
        y.append(label)
    
    return np.array(X), np.array(y)

def accuracy(y_pred, y_test):
    return np.sum(y_pred == y_test) / len(y_test)

# Load data
file_path = 'dataset.csv'
X, y = load_data(file_path)

# Split data into training and test sets (80/20 split)
train_size = int(0.8 * X.shape[0])
X_train, X_test = X[:train_size], X[train_size:]
y_train, y_test = y[:train_size], y[train_size:]

# Add intercept
X_train = np.hstack((np.ones((X_train.shape[0], 1)), X_train))
X_test = np.hstack((np.ones((X_test.shape[0], 1)), X_test))

# train the model
model = LogisticRegression(learning_rate=0.001, iterations=5000, regularization_strength=600)
model.fit(X_train, y_train)
y_pred = model.predict(X_test)
print(f'Accuracy: {accuracy(y_pred, y_test)}')
model.plot_loss()  # Plot the loss over iterations