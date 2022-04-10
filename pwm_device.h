// pwm_device.h

#ifndef _PWM_DEVICE_h
#define _PWM_DEVICE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_INA260.h> /* https://www.adafruit.com/product/4226 */
#include <LiquidCrystal_I2C.h>
#include "password.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


#define WIFI_TIMEOUT_MS 20000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 30000 // Wait 30 seconds after a failed connection attempt


#ifndef PWM_DEVICE
#define PWM_DEVICE

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

#endif // !PWM_DEVICE

#ifndef POWER_CONSUMPTION
#define POWER_CONSUMPTION

typedef struct power_consumption
{
	float source_current;
	float drain_current;
	float food_current;
	float light_current;
} power_consumption;

#endif // !POWER_CONSUMPTION

//PWM Devices
extern power_consumption system_device_health;
extern PWM_device water_pump_source;
extern PWM_device water_pump_drain;
extern PWM_device food_pump;
extern PWM_device air_pump;
extern PWM_device LED;
extern PWM_device test_device;

//Task Handles
extern TaskHandle_t toggleLED_1_handle;
extern TaskHandle_t keep_wifi_alive_handle;
extern TaskHandle_t wifi_poll_server_handle;
extern TaskHandle_t wifi_poll_server_json_handle;
extern TaskHandle_t physical_controls_handle;

//Task Handler Structs
typedef struct task_fields
{
	TaskFunction_t pvTaskCode;
	const char* const pcName;
	const uint32_t usStackDepth;
	PWM_device PWM_device_ptr; //Cast as void*
	UBaseType_t uxPriority;
	TaskHandle_t* const pvCreatedTask;
} task_fields;
extern task_fields water_pump_source_task_field;
extern task_fields water_pump_drain_task_field;
extern task_fields food_pump_task_field;
extern task_fields air_pump_task_field;
extern task_fields test_device_task_field;


//Wifi & Communication Functions
extern HTTPClient http;
extern String server_name;
extern int pod_id;
extern void keep_wifi_alive(void* parameter);
extern void wifi_poll_server(void* parameter);
extern void wifi_poll_server_json(void* parameter);

//Motor Management Functions
extern void restart_task(
	TaskFunction_t pvTaskCode,
	const char* const pcName,
	const uint32_t usStackDepth,
	void* const pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t* const pvCreatedTask);
extern void turn_on_air_pump(void* pwm_device);
extern void PWM_init(PWM_device* pwm_device);
extern void PWM_calibration(void* pwm_device);
extern void PWM_set_percent(PWM_device* pwm_device, uint8_t percent);
extern void dose_food(void* pwm_device, uint8_t ml); // USE A LOAD CELL TO DETERMINE WATER DOSAGE
extern void PWM_timer_handler(void* pwm_device);
extern void toggle_light(void* pwm_device);
extern void calibrate_power_draw();
extern void turn_on(void* pwm_device);
extern void simple_pwm_function();

//Scheduling Functions
extern void delete_pwm_tasks();		//Used to reset any tasks if needed in combination with start_pwm_tasks();
extern void start_pwm_tasks();

//Screen Functions
extern void i2c_scanner();
extern void physical_controls(void* parameter);

//Example Functions
// Used to model dynamics
extern void toggleLED_1(void* parameter);
extern void toggleLED_2(void* parameter);
extern void change_setting(void* pwm_device);

extern int rate_1;
extern int rate_2;
extern int led_pin;
extern volatile int delay_time;

#endif // !_PWM_DEVICE_h
