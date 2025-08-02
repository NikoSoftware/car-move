#include <stdio.h>
#include "driver/ledc.h"
#include "driver/gpio.h"


typedef struct
{
    ledc_timer_config_t time_t;
    ledc_channel_config_t channel_front;
    ledc_channel_config_t channel_back;
    gpio_num_t pwd_ena;
    gpio_num_t pwd_enb;

    gpio_num_t pin_ina; 
    gpio_num_t pin_inb; 
    gpio_num_t pin_inc; 
    gpio_num_t pin_ind; 

    int speed;

    int direction;
    
} motor_manager;



//初始化电机
motor_manager *init_motor_manager(ledc_channel_t channel_a, ledc_channel_t channel_b, gpio_num_t ena, gpio_num_t enb, gpio_num_t ina, gpio_num_t inb, gpio_num_t inc, gpio_num_t ind);


void motor_run(motor_manager *manager);


void motor_brake(motor_manager *manager);