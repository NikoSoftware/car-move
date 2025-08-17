#include "motorcontrol.h"

#define PWM_FREQ 5000            // PWM频率(Hz)
#define PWM_RES LEDC_TIMER_8_BIT // 8位分辨率(0-255)

motor_manager *init_motor_manager(ledc_channel_t channel_a, ledc_channel_t channel_b, gpio_num_t ena, gpio_num_t enb, gpio_num_t ina, gpio_num_t inb, gpio_num_t inc, gpio_num_t ind)
{

    ledc_timer_config_t time_t =
        {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = PWM_RES,
            .timer_num = LEDC_TIMER_0,
            .freq_hz = PWM_FREQ,
            .clk_cfg = LEDC_AUTO_CLK,

        };

    ledc_channel_config_t channel_01 =
        {
            .channel = channel_a,
            .gpio_num = ena,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,

        };
    ledc_channel_config_t channel_02 =
        {
            .channel = channel_b,
            .gpio_num = enb,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,

        };

    ledc_timer_config(&time_t);

    ledc_channel_config(&channel_01);
    ledc_channel_config(&channel_02);

    motor_manager *manager = (motor_manager *)malloc(sizeof(motor_manager));

    manager->time_t = time_t;
    manager->channel_front = channel_01;
    manager->channel_back = channel_02;
    manager->pwd_ena = ena;
    manager->pwd_enb = enb;
    manager->pin_ina = ina;
    manager->pin_inb = inb;
    manager->pin_inc = inc;
    manager->pin_ind = ind;

    gpio_set_direction(ina, GPIO_MODE_OUTPUT);
    gpio_set_direction(inb, GPIO_MODE_OUTPUT);
    gpio_set_direction(inc, GPIO_MODE_OUTPUT);
    gpio_set_direction(ind, GPIO_MODE_OUTPUT);

    return manager;
}

void motor_run(motor_manager *manager)
{


    gpio_set_level(manager->pin_ina, manager->direction);
    gpio_set_level(manager->pin_inb, !manager->direction);
    gpio_set_level(manager->pin_inc, manager->direction);
    gpio_set_level(manager->pin_ind, !manager->direction);


    ledc_set_duty(LEDC_LOW_SPEED_MODE, manager->channel_front.channel, manager->speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, manager->channel_front.channel);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, manager->channel_back.channel, manager->speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, manager->channel_back.channel);

    //printf("电机方向：%d,运行速度：%d\n", manager->direction, manager->speed);
}

// 刹车操作
void motor_brake(motor_manager *manager)
{

    gpio_set_level(manager->pin_ina, 1);
    gpio_set_level(manager->pin_inb, 1);
    gpio_set_level(manager->pin_inc, 1);
    gpio_set_level(manager->pin_ind, 1);

    //printf("电机刹车！\n");
}