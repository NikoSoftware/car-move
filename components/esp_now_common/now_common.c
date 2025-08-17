// common.c
#include "now_common.h"

// 全局变量 (简化示例)
esp_now_peer_info_t peer_info = {
    .channel = ESPNOW_CHANNEL,
    .ifidx = WIFI_IF_STA,
    .encrypt = false, // 为简化示例禁用加密
    .priv = NULL
};


// Wi-Fi 初始化 (STA模式)
void my_wifi_init() {
    // 初始化NVS（关键修复！）
    esp_err_t nvs_ret = nvs_flash_init();
    if (nvs_ret == ESP_ERR_NVS_NO_FREE_PAGES || nvs_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_ret);

    // 继续原有初始化流程
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Wi-Fi initialized in STA mode");
}


// ESP-NOW 初始化
void my_espnow_init() {
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(my_espnow_recv_cb));
    ESP_ERROR_CHECK(esp_now_register_send_cb(my_espnow_send_cb));
    ESP_LOGI(TAG, "ESP-NOW initialized");
}

// 添加对端
void my_espnow_add_peer(const uint8_t *peer_mac) {
    memcpy(peer_info.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    esp_err_t ret = esp_now_add_peer(&peer_info);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Added peer: " MACSTR, MAC2STR(peer_mac));
    } else if (ret == ESP_ERR_ESPNOW_EXIST) {
        ESP_LOGW(TAG, "Peer already exists: " MACSTR, MAC2STR(peer_mac));
    } else {
        ESP_LOGE(TAG, "Failed to add peer: %d (%s)", ret, esp_err_to_name(ret));
    }
}

// 发送数据
void my_espnow_send_data(const uint8_t *peer_mac, const espnow_message_t *message) {
    size_t len = sizeof(espnow_message_t);
    esp_err_t ret = esp_now_send(peer_mac, (const uint8_t *)message, len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_now_send failed: %d (%s)", ret, esp_err_to_name(ret));
    }

}


int mac_string_to_bytes(const char *mac_str, uint8_t *mac_bytes) {
    if (strlen(mac_str) != 17) {
        return -1; // 长度无效
    }
    
    // 临时变量存储转换结果
    unsigned int bytes[6];
    
    // 使用sscanf解析
    int result = sscanf(mac_str, "%x:%x:%x:%x:%x:%x", 
                       &bytes[0], &bytes[1], &bytes[2],
                       &bytes[3], &bytes[4], &bytes[5]);
    
    if (result != 6) {
        return -2; // 解析失败
    }
    
    // 转换为uint8_t
    for (int i = 0; i < 6; i++) {
        mac_bytes[i] = (uint8_t)bytes[i];
    }
    
    return 0;
}