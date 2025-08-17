// common.h
#ifndef COMMON_H
#define COMMON_H

#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include <string.h>
#include "nvs_flash.h"

#define ESPNOW_CHANNEL 1 // 选择一个共同的信道 (1-13) 或 0（需动态同步）
#define TAG "ESP_NOW_EXAMPLE"

// 定义自定义消息结构 (示例)
typedef struct __attribute__((packed)) {
    uint8_t motion_state;  // 运动状态位图 [0:前进, 1:后退, 2:左转, 3:右转, 10:刹车]
    uint8_t pwm_left;      // 左电机PWM值 (0-255)
    uint8_t pwm_right;     // 右电机PWM值 (0-255)
} espnow_message_t;

// 函数声明
void my_wifi_init();
void my_espnow_init();
void my_espnow_add_peer(const uint8_t *peer_mac);
void my_espnow_send_data(const uint8_t *peer_mac, const espnow_message_t *message);


void my_espnow_recv_cb(const esp_now_recv_info_t *info, const uint8_t *data, int len);
void my_espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);


int mac_string_to_bytes(const char *mac_str, uint8_t *mac_bytes);

#endif // COMMON_H