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

#include "command_responder.h"

#include "am_bsp.h"  // NOLINT

// This implementation will light up the LEDs on the board in response to
// different commands.
int RespondToCommand(tflite::ErrorReporter* error_reporter,int32_t current_time, const char* found_command,uint8_t score, bool is_new_command)
 {
  static bool is_initialized = false;
  if (!is_initialized)
  {
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_BLUE, g_AM_HAL_GPIO_OUTPUT_12);
    is_initialized = true;
  }
  static int count = 0;

  // Toggle the blue LED every time an inference is performed.
  ++count;
  if (count & 1) {
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_BLUE);
  } else {
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_BLUE);
  }

  if (is_new_command)
  {
    error_reporter->Report("Heard %s (%d) @%dms", found_command, score, current_time);

    if (found_command[0] == 'c') {
      error_reporter->Report("\nChainsaw");
      return 1;
      
    }
    if (found_command[0] == 'b') {
      error_reporter->Report("\nBee");
     return 2;    

    }
    if (found_command[0] == 'm') {
      error_reporter->Report("\nMosquito");
      return 3;     

    }
    if (found_command[0] == 'u') {
      error_reporter->Report("\nUNKNOWN");
      return 0;

    }
  }
}
