#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motorcontrol.h"

typedef struct {
    motor_manager* left;  
    motor_manager* right; 
} MotorParams_t;



void test_run(void * pvParameters){

    MotorParams_t* params = (MotorParams_t*)pvParameters; 

    while(1){

        params->left->speed = 250;
        params->left->direction = 1;

        params->right->speed = 250;
        params->right->direction = 1;
        
        motor_run(params->left);
        motor_run(params->right);
        vTaskDelay(pdMS_TO_TICKS(2000));


        motor_brake(params->left);  
        motor_brake(params->right);                 
        vTaskDelay(pdMS_TO_TICKS(1000));

    }


}


void app_main(void)
{

    motor_manager* left = init_motor_manager(LEDC_CHANNEL_0,LEDC_CHANNEL_1,GPIO_NUM_38, GPIO_NUM_39, GPIO_NUM_7,GPIO_NUM_6,GPIO_NUM_5,GPIO_NUM_4);
    motor_manager* right = init_motor_manager(LEDC_CHANNEL_2,LEDC_CHANNEL_3,GPIO_NUM_21, GPIO_NUM_37, GPIO_NUM_18,GPIO_NUM_17,GPIO_NUM_16,GPIO_NUM_15);

   MotorParams_t* params = (MotorParams_t*)malloc(sizeof(MotorParams_t));
    params->left = left;
    params->right = right;

    xTaskCreatePinnedToCore(test_run,"car_run",2048,(void*)params,2,NULL,1);
    



}
