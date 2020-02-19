// Undefine predefined Arduino max/min defs so they don't conflict with std methods
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include <TensorFlowLite.h>
#include <Wire.h>
#include "SparkFunCCS811.h" //Click here to get the library: http://librarymanager/All#SparkFun_CCS811
#include "SparkFunBME280.h"
#include "SparkFun_VCNL4040_Arduino_Library.h"

#include "main_functions.h"

#include "output_handler.h"
#include "greenhouse_model_data.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library


//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 9
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1

MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration

#define CCS811_ADDR 0x5B //Default I2C Address
//#define CCS811_ADDR 0x5A //Alternate I2C Address
BME280 Sensor;
CCS811 mySensor(CCS811_ADDR);
VCNL4040 proximitySensor;

unsigned long previousMillis = 0;  //will store last time prediction was performed
const long period = 5000;          //interval between predictions

namespace
{
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 6 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

void setup()
{
  Wire.begin(); //Inialize I2C Harware
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
  //It is recommended to check return status on .begin(), but it is not
  //required.
  CCS811Core::status returnCode = mySensor.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    while (1); //Hang if there was a problem.
  }
  if (Sensor.beginI2C() == false) //Begin communication over I2C
  {
    while (1); //Freeze
  }
  proximitySensor.begin(); //Initialize the sensor
  proximitySensor.powerOffProximity(); //Power down the proximity portion of the sensor
  proximitySensor.powerOnAmbient();
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_greenhouse_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
      "Model provided is schema version %d not equal "
      "to supported version %d.",
      model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::ops::micro::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);
}

void loop()
{
  unsigned long currentMillis = millis(); // store the current time
  if (currentMillis - previousMillis >= period)
  { // check if 5000ms passed
    previousMillis = currentMillis;
    oled.clear(PAGE);
    oled.setCursor(0, 0);
    if (mySensor.dataAvailable())
    {
      //feeding the sensor data as per the training dataset after normalizing with the same values we did while the training process
      input->data.f[0] = ((mySensor.getCO2()) / 1000);                         // for C02 normalizing value is 1000
      input->data.f[1] = ((proximitySensor.getAmbient()) / 10000);             // for light intensity normalizing value is 10000
      input->data.f[2] = ((Sensor.readTempC()) / 100);                         // for temperature normalizing value is 100

      // Run inference, and report any error
      TfLiteStatus invoke_status = interpreter->Invoke();
      if (invoke_status != kTfLiteOk) {
        error_reporter->Report("Invoke failed on x_vals:");
        return;
      }
      // Read the predicted y value from the model's output tensor
      float y_val = output->data.f[0];
      oled.print(y_val);
      oled.display();
      if (y_val >=0.7f && y_val<=1.0f)
      {
        oled.setCursor(0, 20);
        oled.print("ActivatingGreenhouse");
        oled.display();
      }
      if (y_val >= 0.0f && y_val <=0.6f)
      {
        oled.setCursor(0,20);
        oled.print("Plants Happy");
        oled.display();
      }
      // Output the results. A custom HandleOutput function can be implemented
      // for each supported hardware target.
      HandleOutput(error_reporter, y_val);

    }
  }
}
