/*
 Name:		FreeRTOS_growcube.ino
 Created:	3/14/2022 11:08:52 PM
 Author:	Benny
*/


#include "pwm_device.h"
WiFiServer server(80);


//Queue Handles
static const int i2c_screen_queue_len = 5;   // Size of delay_queue


void setup() {
    // Watchdog Timers
    //disableCore0WDT();
    //disableCore1WDT();
    // Starting hardware communications

    Serial.begin(9600);
    Serial.setTimeout(5);
    delay(250);
    Wire.begin();

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    Serial.println(WiFi.localIP());

    // I2C stuff
    Wire.begin();
    i2c_scanner();
    PWM_init(&water_pump_source);
    PWM_init(&water_pump_drain);
    PWM_init(&food_pump);
    //PWM_init(&air_pump);
    //PWM_init(&LED);
    PWM_init(&test_device);


// Pump tasks
    
    // NOTE:
    // Wrap fill and empty into one function, possibly with an arguement to indicate the tank is starting empty, to ensure that the tank is empty before water is pumped in. 

    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        PWM_timer_handler,  // Function to be called
        "fill tank",   // Name of task
        2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &water_pump_source,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        &source_handle);         // Task handle

    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        PWM_timer_handler,  // Function to be called
        "empty tank",   // Name of task
        2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &water_pump_drain,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        &drain_handle);         // Task handle
    /*
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        turn_on_air_pump,  // Function to be called
        "air pump",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &air_pump,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL);         // Task handle
*/
    
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        PWM_timer_handler,  // Function to be called
        "food",   // Name of task
        2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
        &food_pump,         // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        &food_handle);         // Task handle
    

    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        PWM_timer_handler,  // Function to be called
        "Toggle 1",   // Name of task
        2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
        (void*) &test_device,  // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        &toggleLED_1_handle          // Task handle
        );

    /*
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        physical_controls,  // Function to be called
        "physical_controls",   // Name of task
        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,  // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        &physical_controls_handle          // Task handle
    );
    */

    xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
        keep_wifi_alive,
        "keep_wifi_alive",  // Task name
        5000,             // Stack size (bytes)
        NULL,             // Parameter
        2,                // Task priority
        &keep_wifi_alive_handle,             // Task handle
        CONFIG_ARDUINO_RUNNING_CORE
    );

    xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
        wifi_poll_server_json,
        "wifi_poll_server_json",  // Task name
        5000,             // Stack size (bytes)
        NULL,             // Parameter
        2,                // Task priority
        &wifi_poll_server_json_handle,             // Task handle
        CONFIG_ARDUINO_RUNNING_CORE
    );
    
    
    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        i2c_task_handler,  // Function to be called
        "i2c_screen",   // Name of task
        2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,  // Parameter to pass to function
        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
        &i2c_screen_handle          // Task handle
    );
    
    // Delete setup and loop "tasks"
    vTaskDelete(NULL);
}

void loop()
{
}
