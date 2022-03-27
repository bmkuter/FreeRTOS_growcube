/*
 Name:		FreeRTOS_growcube.ino
 Created:	3/14/2022 11:08:52 PM
 Author:	Benny
*/

#include "pwm_device.h"

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
#include <Adafruit_INA260.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif


void setup() {

// Setting up ESP32 Analog Write. Make into function using a pwm_device struct later
    // setting PWM properties
    int freq = 5000;
    int ledChannel = 0;
    int resolution = 8;
    // configure LED PWM functionalitites
    ledcSetup(ledChannel, freq, resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(12, ledChannel);

// Configure pin
    pinMode(led_pin, OUTPUT);

// Task to run forever
    xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
        toggleLED_1,  // Function to be called
        "Toggle 1",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL,         // Task handle
        app_cpu);     // Run on one core for demo purposes (ESP32 only)

// Task to run forever
    xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
        toggleLED_2,  // Function to be called
        "Toggle 2",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL,         // Task handle
        app_cpu);     // Run on one core for demo purposes (ESP32 only)

// If this was vanilla FreeRTOS, you'd want to call vTaskStartScheduler() in
// main after setting up your tasks.
}

void loop() {
    // Do nothing
    // setup() and loop() run in their own task with priority 1 in core 1
    // on ESP32
}
