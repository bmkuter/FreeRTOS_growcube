// 
// 
// 

#include "pwm_device.h"


// Example variables
// LED rates
int rate_1 = 1000;  // ms
int rate_2 = 323;  // ms

// Pins
int led_pin = LED_BUILTIN;

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

// Initializing devices
PWM_device test_device = { 1, 13, 145, 255, 0, 0, 0, 5000, 8, 2000, 2000 };
PWM_device water_pump_source = { 1, 33, 145, 255, 0, 0, 1, 5000, 8, 2000, 2000 };
PWM_device water_pump_drain = { 1, 22, 145, 255, 0, 0, 2, 5000, 8, 2000, 2000 };
PWM_device food_pump = { 1, 14, 145, 255, 0, 0, 3, 5000, 8, 2000, 2000 };
PWM_device air_pump = { 1, 32, 145, 255, 0, 0, 4, 5000, 8, 2000, 2000 };
PWM_device LED = { 1, 15, 145, 255, 0, 0, 5, 5000, 8, 2000, 2000 };;

// Our task: blink an LED at one rate
void toggleLED_1(void* parameter) {

    // Casting arguement as integer so we can use it to get delay from a global variable. 
    int* local_delay;
    local_delay = (int*)parameter;

    while (1) {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(*local_delay / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(*local_delay / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

// Our task: blink an LED at another rate
void toggleLED_2(void* parameter) {
    while (1) {
        digitalWrite(led_pin, HIGH);

        vTaskDelay(rate_2 / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(rate_2 / portTICK_PERIOD_MS);
    }
}

// PWM
/* Sets a PWM device to a specific pulse width, determined by percent. 0% turns off device, and 100% turns it to the device's max. */
void PWM_set_percent(PWM_device* pwm_device, uint8_t percent)
{
    /* Making local copies for code readability. */
    uint8_t gpio_pin, min, max, pulse_width, working_range;
    gpio_pin = pwm_device->pin;
    min = pwm_device->min;
    max = pwm_device->max;
    working_range = max - min;

    /* If percent is 0, turn device off. */
    if (percent == 0)
    {
        ledcWrite(gpio_pin, 0);
        pwm_device->motor_status = 0;

    }
    /* If percent is 100, turn device to max. */
    else if (percent == 100)
    {
        ledcWrite(gpio_pin, max);
        pwm_device->motor_status = 1;
    }
    /* In between, calculate pulse_width based on offset and percent of max working range for the specific pump. */
    else if ((percent < 100) && (percent > 0)) {
        pulse_width = min + ((percent / 100) * working_range);
        ledcWrite(gpio_pin, pulse_width);
        pwm_device->motor_status = 1;
    }
    else
    {
        Serial.println("Parameter Error: Outside range (0 - 100).");
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
            PWM_set_percent(local_PWM_device, 80);
            local_PWM_device->motor_status = 1;
            vTaskDelay(local_PWM_device->on_time / portTICK_PERIOD_MS);
            PWM_set_percent(local_PWM_device, 0);
            local_PWM_device->motor_status = 0;
            vTaskDelay(local_PWM_device->off_time / portTICK_PERIOD_MS);
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
    // setting PWM properties
    local_PWM_device->freq = 5000;
    local_PWM_device->pwm_channel = 0;
    local_PWM_device->resolution = 8;
    // configure LED PWM functionalitites
    ledcSetup(local_PWM_device->pwm_channel, local_PWM_device->freq, local_PWM_device->resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(local_PWM_device->pin, local_PWM_device->pwm_channel);
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

void wifi_poll_server()
{
   //Reference: https://randomnerdtutorials.com/esp32-http-get-post-arduino/
}
