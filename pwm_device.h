// pwm_device.h

#ifndef _PWM_DEVICE_h
#define _PWM_DEVICE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_INA260.h> /* https://www.adafruit.com/product/4226 */

#ifndef PWM_DEVICE
#define PWM_DEVICE

typedef struct PWM_device {
	uint8_t pin;
	uint8_t min;
	uint8_t max;
	uint8_t motor_status;
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

extern power_consumption system_device_health;
extern PWM_device water_pump_source;
extern PWM_device water_pump_drain;
extern PWM_device food_pump;
extern PWM_device air_pump;
extern PWM_device LED;

extern void PWM_calibration(PWM_device* pwm_device);
extern void PWM_set_percent(PWM_device* pwm_device, uint8_t percent);
extern void dose_food(PWM_device* pwm_device, uint8_t ml);
extern void fill_tank(PWM_device* pwm_device);
extern void empty_tank(PWM_device* pwm_device);
extern void toggle_light(PWM_device* pwm_device);
extern void calibrate_power_draw();

#endif // !_PWM_DEVICE_h