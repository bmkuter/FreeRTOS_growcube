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

// Initializing devices
PWM_device test_device = { 13, 145, 255, 0, 5000, 8 };
PWM_device water_pump_source = { 33, 145, 255, 0, 0, 0 };
PWM_device water_pump_drain = { 22, 145, 255,0, 0, 1 };
PWM_device food_pump = { 14, 180, 255, 0, 0, 2 };
PWM_device air_pump = { 32, 230, 255, 0, 0, 3 };
PWM_device LED = { 15, 145, 255, 0, 0, 4 };

// Our task: blink an LED at one rate
void toggleLED_1(void* parameter) {
    while (1) {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(rate_1 / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(rate_1 / portTICK_PERIOD_MS);
    }
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

void fill_tank(PWM_device* pwm_device)
{
    // Issued by scheduler into an RTOS task, which deletes itself at the end. When next needed, scheduler will create a new task. 
    /* Time between start and stop of pumping. */
    while (1)
    {
        PWM_set_percent(pwm_device, 80);
        vTaskDelay(pwm_device->on_time / portTICK_PERIOD_MS);
        PWM_set_percent(pwm_device, 0);
        vTaskDelay(pwm_device->off_time / portTICK_PERIOD_MS);
    }
}