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

void HandleOutput(tflite::ErrorReporter* error_reporter, float y_value)
{
  static bool is_initialized = false;
  if (!is_initialized)
  {
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_BLUE, g_AM_HAL_GPIO_OUTPUT_12);
    is_initialized = true;
  }
 // finally we can implement servo control to automatically cover the farm with greenhouse material so that they can withstand cold temperatrue.
}
