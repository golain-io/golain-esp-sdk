#ifndef PERSISTENT_LOGS
#define PERSISTENT_LOGS

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "pb.h"
#include "pb_encode.h"

#include "logs.pb.h"
#include "decode_callbacks.h"
#include "encode_callbacks.h"

const char *LOGGING_TAG = "PERSISTENT_LOGS";

int errorCountSinceLastReset = 0;

#define P_LOG_I(tag, format, ...)                           \
  ESP_LOGI(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_INFO, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#define P_LOG_W(tag, format, ...)                           \
  ESP_LOGW(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_WARN, __func__, "[%s]: " format, tag, ##__VA_ARGS__);

#ifdef CONFIG_DEVICE_HEALTH_STORE
#define P_LOG_E(tag, format, ...)                                            \
  ESP_LOGE(tag, "(%s)-> " format, __func__, ##__VA_ARGS__);                  \
  write_p_log(ESP_LOG_ERROR, __func__, "[%s]: " format, tag, ##__VA_ARGS__); \
  errorCountSinceLastReset++;
#else
#define P_LOG_E(tag, format, ...)                           \
  ESP_LOGE(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_ERROR, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#endif

esp_err_t check_nvs_errors(esp_err_t err);

esp_err_t write_to_nvs(uint8_t *data, size_t len)

esp_err_t write_p_log(esp_log_level_t level, const char *func,
                          const char *tag, const char *format, ...)

esp_err_t read_old_logs( uint8_t *buffer)

#endif // PERSISTENT_LOGS