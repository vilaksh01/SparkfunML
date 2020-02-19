# -*- coding: utf-8 -*-
"""Fire.ipynb

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

filename = '/content/fireData.csv'
dataframe = read_csv(filename)

array = dataframe.values

X = array[:,0:4]
Y = array[:,4]
print(X)
print(Y)

dataframe.head()

import tensorflow as tf

model = tf.keras.Sequential()
# First layer takes a scalar input and feeds it through 16 "neurons." The
# neurons decide whether to activate based on the 'relu' activation function.
model.add(layers.Dense(16, activation='relu',kernel_initializer='uniform',input_shape=(4,)))
# The new second layer may help the network learn more complex representations
model.add(layers.Dense(16, activation='relu',kernel_initializer='uniform'))
# Final layer is a single neuron, since we want to output a single value
model.add(layers.Dense(1,kernel_initializer='uniform',activation='sigmoid'))
# Compile the model using a standard optimizer and loss function for regression
model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
# Show a summary of the model
model.summary()

history = model.fit(X,Y,epochs=30,batch_size=15)

score=model.evaluate(X,Y)
print("%s: %.2f%%" % (model.metrics_names[1], score[1]*100))

filename1 = '/content/validateFire.csv'
dataframe1 = read_csv(filename)

array1 = dataframe1.values

X1 = array1[:,0:4]
Y1 = array1[:,4]
print(X1)
print(Y1)

dataframe.head()

loss = model.evaluate(X1,Y1)
predictions = model.predict(X1)

plt.title('Comparison of predictions and actual values')
plt.plot(X1, Y1, 'b.', label='Actual')
plt.plot(X1, predictions, 'r.', label='Predicted')
plt.legend()
plt.show()

converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Save the model to disk
open("fire_model.tflite", "wb").write(tflite_model)

# Convert the model to the TensorFlow Lite format with quantization
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_SIZE]
tflite_model = converter.convert()

# Save the model to disk
open("fire_model_quantized.tflite", "wb").write(tflite_model)

# Instantiate an interpreter for each model
fire_model = tf.lite.Interpreter('fire_model.tflite')
fire_model_quantized = tf.lite.Interpreter('fire_model_quantized.tflite')

# Allocate memory for each model
fire_model.allocate_tensors()
fire_model_quantized.allocate_tensors()

# Get the input and output tensors so we can feed in values and get the results
fire_model_input = fire_model.tensor(fire_model.get_input_details()[0]["index"])
fire_model_output = fire_model.tensor(fire_model.get_output_details()[0]["index"])
fire_model_quantized_input = fire_model_quantized.tensor(fire_model_quantized.get_input_details()[0]["index"])
fire_model_quantized_output = fire_model_quantized.tensor(fire_model_quantized.get_output_details()[0]["index"])

# Create arrays to store the results
fire_model_predictions = np.empty(X1.size)
fire_model_quantized_predictions = np.empty(X1.size)

# Run each model's interpreter for each value and store the results in arrays
for i in range(X1.size):
  fire_model_input().fill(X1[i])
  fire_model.invoke()
  fire_model_predictions[i] = fire_model_output()[0]

  fire_model_quantized_input().fill(X1[i])
  fire_model_quantized.invoke()
  fire_model_quantized_predictions[i] = fire_model_quantized_output()[0]

# See how they line up with the data
plt.clf()
plt.title('Comparison of various models against actual values')
plt.plot(X1, Y1, 'bo', label='Actual')
plt.plot(X1, predictions, 'ro', label='Original predictions')
plt.plot(X1, fire_model_predictions, 'bx', label='Lite predictions')
plt.plot(X1, fire_model_quantized_predictions, 'gx', label='Lite quantized predictions')
plt.legend()
plt.show()

import os
basic_model_size = os.path.getsize("fire_model.tflite")
print("Basic model is %d bytes" % basic_model_size)
quantized_model_size = os.path.getsize("fire_model_quantized.tflite")
print("Quantized model is %d bytes" % quantized_model_size)
difference = basic_model_size - quantized_model_size
print("Difference is %d bytes" % difference)

# Install xxd if it is not available
!apt-get -qq install xxd
# Save the file as a C source file
!xxd -i fire_model_quantized.tflite > fire_model_quantized.cc
# Print the source file
!cat fire_model_quantized.cc




