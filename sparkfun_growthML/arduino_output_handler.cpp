/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  ==============================================================================*/

#include "output_handler.h"
#include "am_bsp.h"  // NOLINT
#include "arduino.h"

#define trigger 8
#define echo 14

float duration = 0.0;
float initial_device_height = 0.0;

void HandleOutput(tflite::ErrorReporter* error_reporter, float y_value, float x_value)
{
  static bool is_initialized = false;
  if (!is_initialized)
  {
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_BLUE, g_AM_HAL_GPIO_OUTPUT_12);
    is_initialized = true;
  }
  error_reporter->Report("Predicted days, %f\n for height, %f/n", y_value, x_value);
  am_hal_gpio_output_set(AM_BSP_GPIO_LED_BLUE);
}

float CropHeight()
{
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  duration = pulseIn(echo, HIGH);
  float  distance = duration * 340 / 20000; //distance in cm
  distance = initial_device_height - distance;
  return distance;
}

void HCSR04_initialize()
{
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  duration = pulseIn(echo, HIGH);
  initial_device_height = duration * 340 / 20000; //distance in cm
}
