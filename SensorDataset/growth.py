# -*- coding: utf-8 -*-
"""growth.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1sR0szFkIZT4iYwLcuemr1Obe7mlnVDh-

# Step 1: Importing the modules
"""

# TensorFlow is an open source machine learning library
!pip install tensorflow==2.0
import tensorflow as tf
# NumPy is a math library
import numpy as np
# Matplotlib is a graphing library
import matplotlib.pyplot as plt
# math is Python's math library
import math

from keras.models import Sequential
from keras.layers import Dense 
from tensorflow.keras import layers
import numpy

seed = 9
numpy.random.seed(seed)

from pandas import read_csv

filename = '/content/PlantGrowth.csv'
dataframe = read_csv(filename)

array = dataframe.values

X = array[:,0]
Y = array[:,1]
print(X)
print(Y)

dataframe.head()

import tensorflow as tf

model = tf.keras.Sequential()
# First layer takes a scalar input and feeds it through 16 "neurons." The
# neurons decide whether to activate based on the 'relu' activation function.
model.add(layers.Dense(16, activation='relu',input_shape=(1,)))
# The new second layer may help the network learn more complex representations
model.add(layers.Dense(16, activation='relu'))
# Final layer is a single neuron, since we want to output a single value
model.add(layers.Dense(1,activation='sigmoid'))
# Compile the model using a standard optimizer and loss function for regression
model.compile(optimizer='rmsprop', loss='mse', metrics=['mae'])
# Show a summary of the model
model.summary()

history = model.fit(X,Y,epochs=400,batch_size=10)

score=model.evaluate(X,Y)
#print("%s: %.2f%%" % (model.metrics_names[1], score[1]*100))

predictions = model.predict(X)

# Plot the predictions along with to the test data
plt.clf()
plt.title('Training data predicted vs actual values')
plt.plot(X, Y, 'bo', label='Actual')
plt.plot(X, predictions, 'ro', label='Predicted')
plt.legend()
plt.show()

converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Save the model to disk
open("growth_model.tflite", "wb").write(tflite_model)

# Convert the model to the TensorFlow Lite format with quantization
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_SIZE]
tflite_model = converter.convert()

# Save the model to disk
open("growth_model_quantized.tflite", "wb").write(tflite_model)

growth_model = tf.lite.Interpreter('growth_model.tflite')
growth_model_quantized = tf.lite.Interpreter('growth_model_quantized.tflite')

# Allocate memory for each model
growth_model.allocate_tensors()
growth_model_quantized.allocate_tensors()

# Get the input and output tensors so we can feed in values and get the results
growth_model_input = growth_model.tensor(growth_model.get_input_details()[0]["index"])
growth_model_output = growth_model.tensor(growth_model.get_output_details()[0]["index"])
growth_model_quantized_input = growth_model_quantized.tensor(growth_model_quantized.get_input_details()[0]["index"])
growth_model_quantized_output = growth_model_quantized.tensor(growth_model_quantized.get_output_details()[0]["index"])

# Create arrays to store the results
growth_model_predictions = np.empty(X.size)
growth_model_quantized_predictions = np.empty(X.size)

# Run each model's interpreter for each value and store the results in arrays
for i in range(X.size):
  growth_model_input().fill(X[i])
  growth_model.invoke()
  growth_model_predictions[i] = growth_model_output()[0]

  growth_model_quantized_input().fill(X[i])
  growth_model_quantized.invoke()
  growth_model_quantized_predictions[i] = growth_model_quantized_output()[0]

# See how they line up with the data
plt.clf()
plt.title('Comparison of various models against actual values')
plt.plot(X, Y, 'bo', label='Actual')
plt.plot(X, predictions, 'ro', label='Original predictions')
plt.plot(X, growth_model_predictions, 'bx', label='Lite predictions')
plt.plot(X, growth_model_quantized_predictions, 'gx', label='Lite quantized predictions')
plt.legend()
plt.show()

import os
basic_model_size = os.path.getsize("growth_model.tflite")
print("Basic model is %d bytes" % basic_model_size)
quantized_model_size = os.path.getsize("growth_model_quantized.tflite")
print("Quantized model is %d bytes" % quantized_model_size)
difference = basic_model_size - quantized_model_size
print("Difference is %d bytes" % difference)

# Install xxd if it is not available
!apt-get -qq install xxd
# Save the file as a C source file
!xxd -i growth_model_quantized.tflite > growth_model_quantized.cc
# Print the source file
!cat growth_model_quantized.cc





