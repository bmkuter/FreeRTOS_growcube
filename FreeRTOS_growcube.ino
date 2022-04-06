/*
 Name:		FreeRTOS_growcube.ino
 Created:	3/14/2022 11:08:52 PM
 Author:	Benny
*/



// Use only core 1 for demo purposes
//#include <Adafruit_INA260.h>
//#include <Adafruit_SPIDevice.h>
//#include <Adafruit_I2CRegister.h>
//#include <Adafruit_I2CDevice.h>
//#include <Adafruit_BusIO_Register.h>

#include "pwm_device.h"

volatile int delay_time = 4096;

//Task Handles
    TaskHandle_t toggleLED_1_handle = NULL;
    TaskHandle_t keep_wifi_alive_handle = NULL;
    WiFiServer server(80);



void setup() {
    // Starting hardware communications
    
    Serial.begin(9600);
    Serial.setTimeout(10);
    Wire.begin();


    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    //initWiFi();

    /*
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    */
    
    // I2C stuff
    i2c_scanner();
    
    PWM_init(&water_pump_source);
    PWM_init(&water_pump_drain);
    PWM_init(&food_pump);
    PWM_init(&air_pump);
    PWM_init(&LED);

// Configure pin
    pinMode(led_pin, OUTPUT);

    //PWM_set_percent(&test_device, 50);

// Pump tasks
    // Source water pump task. 
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        PWM_timer_handler,  // Function to be called
        "fill tank",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &water_pump_source,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL);         // Task handle


    // Drain water pump task. 
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        PWM_timer_handler,  // Function to be called
        "empty tank",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &water_pump_drain,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL);         // Task handle

    // Drain water pump task. 
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        turn_on_air_pump,  // Function to be called
        "air pump",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &air_pump,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL);         // Task handle

    // Light Control task. 
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        PWM_timer_handler,  // Function to be called
        "light",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &LED,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL);         // Task handle


// Task to run forever
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        toggleLED_1,  // Function to be called
        "Toggle 1",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        (void*) & delay_time,  // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        &toggleLED_1_handle          // Task handle
        );     
    
// Task to run forever    
    xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
            keep_wifi_alive,
            "keepWiFiAlive",  // Task name
            5000,             // Stack size (bytes)
            NULL,             // Parameter
            1,                // Task priority
            &keep_wifi_alive_handle,             // Task handle
            CONFIG_ARDUINO_RUNNING_CORE
        );     
}

void loop() {
    // Instantly adjusts schedule based on new parameters, even if the timer is in the middle of a cycle. 
    if (Serial.available() > 0) 
    {
        delay_time = Serial.parseInt();
        // prints the received integer
        Serial.print("I received: ");
        Serial.println(delay_time);

        restart_task(  // Use xTaskCreate() in vanilla FreeRTOS
            toggleLED_1,  // Function to be called
            "Toggle 1",   // Name of task
            1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
            (void*)&delay_time,  // Parameter to pass to function
            1,            // Task priority (0 to configMAX_PRIORITIES - 1)
            &toggleLED_1_handle          // Task handle
        );
    }
    //wifi_poll_server();
}