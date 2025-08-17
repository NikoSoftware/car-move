#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motorcontrol.h"
#include "now_common.h"
#include "esp_log.h"

typedef struct
{
    motor_manager *left;
    motor_manager *right;
    int motion_state;
} MotorParams_t;

static MotorParams_t *params = NULL;

const char *mac_str = "A0:85:E3:AF:2D:B0";
static uint8_t peer_b_mac[6];

void test_run(void *pvParameters)
{

    while (1)
    {

        if (params->motion_state == 10)
        {

            motor_brake(params->left);
            motor_brake(params->right);
        }
        else
        {

            motor_run(params->left);
            motor_run(params->right);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// 发送回调函数
void my_espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if (status == ESP_NOW_SEND_SUCCESS)
    {
        ESP_LOGI(TAG, "Sent to " MACSTR " succeeded", MAC2STR(mac_addr));
    }
    else
    {
        ESP_LOGE(TAG, "Sent to " MACSTR " failed", MAC2STR(mac_addr));
    }
}

// 接收回调函数
void my_espnow_recv_cb(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    if (len != sizeof(espnow_message_t))
    {
        ESP_LOGW(TAG, "Received invalid length: %d", len);
        return;
    }

    const espnow_message_t *msg = (const espnow_message_t *)data;
    ESP_LOGI(TAG, "Received from " MACSTR ": motion_state: %d, pwm_left: %d, pwm_right: %d",
             MAC2STR(info->src_addr), msg->motion_state, msg->pwm_left, msg->pwm_right);

    params->motion_state = (msg->motion_state);

    switch (msg->motion_state)
    {

    case 0:
        params->left->speed = msg->pwm_left;
        params->left->direction = 1;
        params->right->speed = msg->pwm_right;
        params->right->direction = 1;
        break;
    case 1:
        params->left->speed = msg->pwm_left;
        params->left->direction = 0;
        params->right->speed = msg->pwm_right;
        params->right->direction = 0;
        break;

    case 2:
        params->left->speed = (int)(msg->pwm_left * 0.5);
        params->left->direction = 1;
        params->right->speed = msg->pwm_right;
        params->right->direction = 1;
        break;

    case 3:
        params->left->speed = msg->pwm_left;
        params->left->direction = 1;
        params->right->speed = (int)(msg->pwm_right * 0.5);
        params->right->direction = 1;
        break;
    default:
        break;
    }
}

void print_mac()
{
    uint8_t mac[6];

    // 初始化WiFi
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    // 获取STA MAC
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    ESP_LOGI("MAC", "STA MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void app_main(void)
{
    // print_mac();
    // 初始化
    my_wifi_init();
    my_espnow_init();

    mac_string_to_bytes(mac_str, peer_b_mac);
    // 添加 Device B 为对端
    my_espnow_add_peer(peer_b_mac);


    motor_manager *left = init_motor_manager(LEDC_CHANNEL_0, LEDC_CHANNEL_1, GPIO_NUM_38, GPIO_NUM_39, GPIO_NUM_7, GPIO_NUM_6, GPIO_NUM_5, GPIO_NUM_4);
    motor_manager *right = init_motor_manager(LEDC_CHANNEL_2, LEDC_CHANNEL_3, GPIO_NUM_21, GPIO_NUM_37, GPIO_NUM_18, GPIO_NUM_17, GPIO_NUM_16, GPIO_NUM_15);

    params = (MotorParams_t *)malloc(sizeof(MotorParams_t));

    params->motion_state = 10;

    params->left = left;
    params->right = right;

    xTaskCreatePinnedToCore(test_run, "car_run", 2048, NULL, 2, NULL, 1);


}
