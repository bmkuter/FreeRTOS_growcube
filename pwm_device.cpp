//
//
//

#include "pwm_device.h"


// Example variables
// LED rates
int rate_1 = 1000;  // ms
int rate_2 = 323;  // ms
volatile int delay_time = 4096;

// Pins
int led_pin = LED_BUILTIN;

//Networking Objects
HTTPClient http; //Declare an object of class HTTPClient
int pod_id = 1;
//String server_name = "http://localhost:5000/query_device/";
String server_name = "http://192.168.86.112:5000/query_device/";

/*
typedef struct PWM_device {
    uint8_t enabled;
    uint8_t pin;
    uint8_t min;
    uint8_t max;
    uint8_t motor_status; //0 off, 1 on
    uint8_t init; // Flag to check if the pwm_device's ledcSetup has been properly initialized.
    uint8_t pwm_channel;
    uint32_t freq;
    uint8_t resolution;
    uint32_t on_time; // Used to indicate how long device should run for. May be changed often.
    uint32_t off_time;
} PWM_device;
*/

//Task Handles
TaskHandle_t toggleLED_1_handle = NULL;
TaskHandle_t keep_wifi_alive_handle = NULL;
TaskHandle_t wifi_poll_server_handle = NULL;
TaskHandle_t wifi_poll_server_json_handle = NULL;
TaskHandle_t physical_controls_handle = NULL;
TaskHandle_t source_handle = NULL;
TaskHandle_t drain_handle = NULL;
TaskHandle_t LED_handle = NULL;
TaskHandle_t food_handle = NULL;

// Initializing devices
/*
typedef struct PWM_device {
    uint8_t enabled;
    uint8_t pin;
    uint8_t min;
    uint8_t max;
    uint8_t pulse_width;
    uint8_t motor_status; //0 off, 1 on
    uint8_t init; // Flag to check if the pwm_device's ledcSetup has been properly initialized.
    uint8_t pwm_channel;
    uint32_t freq;
    uint8_t resolution;
    uint32_t on_time; // Used to indicate how long device should run for. May be changed often.
    uint32_t off_time;
} PWM_device;
*/
/*ON*/ PWM_device test_device =         { 1, 21, 0, 255, 25 , 0, 0, 0, 5000, 8, 1000, 1000};
/*ON*/ PWM_device water_pump_source =   { 1, 17, 0, 255, 0, 0, 0, 1, 5000, 8, 0, 0 };
/*ON*/ PWM_device water_pump_drain =    { 1, 16, 0, 255, 0, 0, 0, 2, 5000, 8, 0, 0 };
/*ON*/ PWM_device food_pump =           { 1, 19, 0, 255, 0, 0, 0, 3, 5000, 8, 0, 0 };
PWM_device air_pump =                   { 1, 32, 0, 255, 0, 0, 0, 4, 5000, 8, 0, 0 };
PWM_device LED =                        { 1, 15, 0, 255, 0, 0, 0, 5, 5000, 8, 0, 0 };;

//https://arduinojson.org/v5/doc/tricks/
class HashPrint : public Print {
public:
    HashPrint() {
        _hash = _hasher.crc32(NULL, 0);
    }

    virtual size_t write(uint8_t c) {
        _hash = _hasher.crc32_upd(&c, 1);
    }

    uint32_t hash() const {
        return _hash;
    }

private:
    FastCRC32 _hasher;
    uint32_t _hash;
};

// Our task: blink an LED at one rate
void toggleLED_1(void* parameter) {

    // Casting arguement as integer so we can use it to get delay from a global variable.
    //int* local_delay;
    //local_delay = (int*)parameter;

    PWM_device* local_PWM_device = (PWM_device*)parameter;

    while (1) {
        if (local_PWM_device->off_time > 0 && local_PWM_device->on_time > 0 && local_PWM_device->pulse_width > 0)
        {
            PWM_set_percent(local_PWM_device, local_PWM_device->pulse_width);
            vTaskDelay(local_PWM_device->on_time / portTICK_PERIOD_MS);
            PWM_set_percent(local_PWM_device, 0);
            vTaskDelay(local_PWM_device->off_time / portTICK_PERIOD_MS);
        }
        else
        {
            PWM_set_percent(local_PWM_device, 0);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

    }

    vTaskDelete(NULL);
}

// PWM
/* Sets a PWM device to a specific pulse width, determined by percent. 0% turns off device, and 100% turns it to the device's max. */
void PWM_set_percent(PWM_device* pwm_device, uint8_t pulse_width)
{
    //Serial.printf("\nInside PWM_Set...:\ngpio_pin = %d\nmin = %d\nmax = %d\n", gpio_pin, min, max);
    /* If percent is 0, turn device off. */
    if (pulse_width == 0)
    {
        ledcWrite(pwm_device->pwm_channel, 0); //Why does 256 turn off the LED lmao https://github.com/espressif/arduino-esp32/issues/689
        pwm_device->motor_status = 0;
    }
    /* If percent is 100, turn device to max. */
    else if (pulse_width >= pwm_device->max)
    {
        ledcWrite(pwm_device->pwm_channel, pwm_device->max);
        pwm_device->motor_status = 1;
    }
    else if (pulse_width <= pwm_device->min)
    {
        ledcWrite(pwm_device->pwm_channel, pwm_device->min);
        pwm_device->motor_status = 1;
    }
    /* In between, calculate pulse_width based on offset and percent of max working range for the specific pump. */
    else if ((pulse_width < pwm_device->max) && (pulse_width > pwm_device->min)) {
        ledcWrite(pwm_device->pwm_channel, pulse_width);
        pwm_device->motor_status = 1;
    }
    else
    {
        return;
    }
    return;
}

void PWM_timer_handler(void* pwm_device)
{
    PWM_device* local_PWM_device = (PWM_device*)pwm_device;
    // Issued by scheduler into an RTOS task, which deletes itself at the end. When next needed, scheduler will create a new task.
    /* Time between start and stop of pumping. */
    while (1)
    {
        // Ensures the PWM Device is generally enabled. Enabled can be toggled by emergency stop functions.
        if (local_PWM_device->enabled == 1 )
        {
            //Serial.printf("local_PWM_device->on_time = %d\n", local_PWM_device->on_time);
            PWM_set_percent(local_PWM_device, local_PWM_device->pulse_width);
            vTaskDelay(local_PWM_device->on_time / portTICK_PERIOD_MS);
            PWM_set_percent(local_PWM_device, 0);
            vTaskDelay(local_PWM_device->off_time / portTICK_PERIOD_MS);
        }
        else
        {
            Serial.println("PWM OFF");
            PWM_set_percent(local_PWM_device, 0);
            local_PWM_device->motor_status = 0;
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

void simple_pwm_function()
{
    while (1) {
        PWM_set_percent(&test_device, 80);
        vTaskDelay((&test_device)->on_time / portTICK_PERIOD_MS);
        ledcWrite((&test_device)->pwm_channel, 0);
        vTaskDelay((&test_device)->off_time / portTICK_PERIOD_MS);
        /*
        for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
            ledcWrite(test_device.pwm_channel, dutyCycle);
            vTaskDelay(7);
        }

        for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
            ledcWrite(test_device.pwm_channel, dutyCycle);
            vTaskDelay(7);
        }
        */
    }
}

void turn_on_air_pump(void* pwm_device)
{
    PWM_device* local_PWM_device = (PWM_device*)pwm_device;
    // Issued by scheduler into an RTOS task, which deletes itself at the end. When next needed, scheduler will create a new task.
    /* Time between start and stop of pumping. */
    while (1)
    {
        // Ensures the PWM Device is generally enabled. Enabled can be toggled by emergency stop functions.
        if (local_PWM_device->enabled)
        {
            PWM_set_percent(local_PWM_device, 80);
            local_PWM_device->motor_status = 1;
        }
        else if (local_PWM_device->enabled == 0)
        {
            PWM_set_percent(local_PWM_device, 0);
            local_PWM_device->motor_status = 0;
        }
        else
        {
        }
        vTaskDelay(500 / portTICK_PERIOD_MS); //Yield for 500ms
    }
}

extern void change_setting(void* pwm_device)
{
    PWM_device* local_PWM_device = (PWM_device*)pwm_device;
    while (1)
    {
        (local_PWM_device->freq)++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void PWM_init(PWM_device* local_PWM_device)
{
    // configure LED PWM functionalitites
    if (ledcSetup(local_PWM_device->pwm_channel, local_PWM_device->freq, local_PWM_device->resolution) == 0)
    {
        Serial.printf("Error in PWM_init %d", local_PWM_device->pin);
    }

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(local_PWM_device->pin, local_PWM_device->pwm_channel);
    local_PWM_device->init = 1;
}

void i2c_scanner()
{
    byte error, address;
    int Devices;
    Serial.println("Scanning...");
    Devices = 0;
    for (address = 1; address < 127; address++)
    {

        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");
            Devices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (Devices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
    delay(500);
}

void restart_task(
    TaskFunction_t pvTaskCode,
    const char* const pcName,
    const uint32_t usStackDepth,
    void* const pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t* pvCreatedTask)
{
    
    vTaskDelete(*pvCreatedTask);

    xTaskCreate(  // Use xTaskCreate() in vanilla FreeRTOS
        pvTaskCode,  // Function to be called
        pcName,   // Name of task
        usStackDepth,         // Stack size (bytes in ESP32, words in FreeRTOS)
        pvParameters,  // Parameter to pass to function
        uxPriority,            // Task priority (0 to configMAX_PRIORITIES - 1)
        pvCreatedTask          // Task handle
    );
    
}

void physical_controls(void* parameter)
//Monitors a rotary encoder to adjust values
//https://github.com/igorantolic/ai-esp32-rotary-encoder
{
    while (1)
    {
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
    }
}

void keep_wifi_alive(void* parameter)
{
    while (1)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            //Serial.println("Currently connected to WiFi!");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.print("Connecting to WiFi ..");
        while (WiFi.status() != WL_CONNECTED) {
            Serial.print('.');
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        Serial.println(WiFi.localIP());
    }

    vTaskDelete(NULL);
}

void wifi_poll_server(void* parameter)
{
    while (1)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            http.begin(server_name + pod_id);
            int httpCode = http.GET(); //Send the request
            if (httpCode > 0)
            { //Check the returning code
                String payload = http.getString();   //Get the request response payload
                int payload_int = payload.toInt();
                if (delay_time != payload_int && payload_int != 0)
                {
                    Serial.printf("New delay time: %d\n", payload_int);
                    delay_time = payload_int;
                    restart_task(  // Use xTaskCreate() in vanilla FreeRTOS
                        toggleLED_1,  // Function to be called
                        "Toggle 1",   // Name of task
                        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                        (void*)&delay_time,  // Parameter to pass to function
                        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
                        &toggleLED_1_handle          // Task handle
                    );
                }
            }
            http.end();   //Close connection
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

//Take as a parameter a pointer to a struct array, each element containing info to restart task its own task.
void wifi_poll_server_json(void* parameter)
{
    //ArduinoJson Stuff
    StaticJsonDocument<512> doc;
    uint32_t new_payload_hash = 0, old_payload_hash = 0;

    while (1)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            http.begin(server_name + pod_id);
            int httpCode = http.GET(); //Send the request
            if (httpCode > 0)
            { //Check the returning code
                String payload = http.getString();   //Get the request response payload
                DeserializationError error = deserializeJson(doc, payload);


                if (error) {
                    Serial.print("deserializeJson() failed: ");
                    Serial.println(error.c_str());
                    return;
                }
                //Configure via https://arduinojson.org/v6/assistant/
                int id = doc["id"]; // 1
                int delay_on = doc["delay_on"]; // 1000
                int delay_off = doc["delay_off"]; // 1000
                int pulse_width = doc["pulse_width"]; // 100

                JsonObject source = doc["source"];
                int source_delay_on = source["delay_on"]; // 1000
                int source_delay_off = source["delay_off"]; // 1000
                int source_pulse_width = source["pulse_width"]; // 100

                JsonObject drain = doc["drain"];
                int drain_delay_on = drain["delay_on"]; // 1000
                int drain_delay_off = drain["delay_off"]; // 1000
                int drain_pulse_width = drain["pulse_width"]; // 100

                JsonObject food = doc["food"];
                int food_delay_on = food["delay_on"]; // 1000
                int food_delay_off = food["delay_off"]; // 1000
                int food_pulse_width = food["pulse_width"]; // 100

                JsonObject air = doc["air"];
                int air_delay_on = air["delay_on"]; // 1000
                int air_delay_off = air["delay_off"]; // 1000
                int air_pulse_width = air["pulse_width"]; // 100

                JsonObject LED_json = doc["LED"];
                int LED_delay_on = LED_json["delay_on"]; // 1000
                int LED_delay_off = LED_json["delay_off"]; // 1000
                int LED_pulse_width = LED_json["pulse_width"]; // 100
                
                // Message packet
                /* We need to control the schedule for 5 devices: Source, Drain, Food, Air, Light.
                   Each of these has a pulse wifth, time to stay on, time to stay off. For light, this is simpler, 
                   But the source, drain, and food pumps need to operate for a much shorter period than the light, 
                   as the containers don't take very long to fill (relative to the time between operations. 
                   Should we use a JSON?

                */

                HashPrint hashPrint;
                serializeJson(doc,hashPrint);
                new_payload_hash = hashPrint.hash();
  



                // We compare to the hash of the previous message
                if (new_payload_hash != old_payload_hash)
                {
                    Serial.printf("New Hash: ");
                    Serial.println(new_payload_hash);
                    Serial.printf("Old Hash: ");
                    Serial.println(old_payload_hash);

                    //Generalise
                    
                    (& test_device)->on_time = delay_on;
                    (& test_device)->off_time = delay_off;
                    (& test_device)->pulse_width = pulse_width;

                    (&water_pump_source)->on_time = source_delay_on;
                    (&water_pump_source)->off_time = source_delay_off;
                    (&water_pump_source)->pulse_width = source_pulse_width;

                    (&water_pump_drain)->on_time = drain_delay_on;
                    (&water_pump_drain)->off_time = drain_delay_off;
                    (&water_pump_drain)->pulse_width = drain_pulse_width;

                    (&food_pump)->on_time = food_delay_on;
                    (&food_pump)->off_time = food_delay_off;
                    (&food_pump)->pulse_width = food_pulse_width;
                    /*
                    (&air_pump)->on_time = air_delay_on;
                    (&air_pump)->off_time = air_delay_off;
                    (&air_pump)->pulse_width = air_pulse_width;

                    (&LED)->on_time = LED_delay_on;
                    (&LED)->off_time = LED_delay_off;
                    (&LED)->pulse_width = LED_pulse_width;
                    */
                    restart_task(  // Use xTaskCreate() in vanilla FreeRTOS
                        toggleLED_1,  // Function to be called
                        "Toggle 1",   // Name of task
                        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                        (void*)&test_device,  // Parameter to pass to function
                        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
                        &toggleLED_1_handle          // Task handle
                    );
                    restart_task(  // Use xTaskCreate() in vanilla FreeRTOS
                        PWM_timer_handler,  // Function to be called
                        "empty tank",   // Name of task
                        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                        (void*)&water_pump_drain,         // Parameter to pass to function
                        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
                        &drain_handle         // Task handle
                    );
                    restart_task(  // Use xTaskCreate() in vanilla FreeRTOS
                        PWM_timer_handler,  // Function to be called
                        "food",   // Name of task
                        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                        (void*)&food_pump,         // Parameter to pass to function
                        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
                        &food_handle         // Task handle
                    );
                    restart_task(  // Use xTaskCreate() in vanilla FreeRTOS
                        PWM_timer_handler,  // Function to be called
                        "fill tank",   // Name of task
                        1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                        (void*)&water_pump_source,         // Parameter to pass to function
                        1,            // Task priority (0 to configMAX_PRIORITIES - 1)
                        &source_handle         // Task handle       // Task handle
                    );
                }
                old_payload_hash = new_payload_hash;
}
            http.end();   //Close connection
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
    }
}

