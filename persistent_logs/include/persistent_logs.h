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

const char* LOGGING_TAG = "PERSISTENT_LOGS";

int errorCountSinceLastReset = 0;

#define P_LOG_I(tag, format, ...)                           \
  ESP_LOGI(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_INFO, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#define P_LOG_W(tag, format, ...)                           \
  ESP_LOGW(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_WARN, __func__, "[%s]: " format, tag, ##__VA_ARGS__);

#ifdef CONFIG_DEVICE_HEALTH_STORE
#define P_LOG_E(tag, format, ...)                           \
  ESP_LOGE(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_ERROR, __func__, "[%s]: " format, tag, ##__VA_ARGS__);\
  errorCountSinceLastReset++;
#else
  #define P_LOG_E(tag, format, ...)                           \
  ESP_LOGE(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_ERROR, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#endif

esp_err_t check_nvs_errors(esp_err_t err) {
  switch (err) {
    case ESP_OK:
      printf("Done\n");
      return err;
    case ESP_ERR_NVS_NOT_FOUND:
      printf("The value is not initialized yet!\n");
      return err;
    default:
      printf("Error (%s) reading!\n", esp_err_to_name(err));
      return err;
  }
}

esp_err_t write_to_nvs(uint8_t* data, size_t len) {
  // open nvs_flash in readwrite mode
  esp_err_t err;
  nvs_handle_t p_log_handle;
  err = nvs_open("p_logs", NVS_READWRITE, &p_log_handle);
  if (err != ESP_OK) {
    return err;
  }

  // get last log id
  int last_log_id;
  if (nvs_get_i32(p_log_handle, "last_log_id", &last_log_id) ==
      ESP_ERR_NVS_NOT_FOUND) {
    last_log_id = 0;
  }

  if (last_log_id == CONFIG_MAX_PERSISTENT_LOGS)
    last_log_id = 0; 
  else
    last_log_id++;

  // write to nvs flash
  char key[2];
  sprintf(key, "%d", last_log_id);
  err = nvs_set_blob(p_log_handle, key, data, len);
  if (err != ESP_OK) {
    return err;
  }

  // store current log id
  nvs_set_i32(p_log_handle, "last_log_id", last_log_id);

  // commit changes
  err = nvs_commit(p_log_handle);
  if (err != ESP_OK) {
    return err;
  }
  nvs_close(p_log_handle);
  ESP_LOGI(LOGGING_TAG, "Wrote to NVS: PLogID:%d", last_log_id);
  return err;
}

esp_err_t write_p_log(esp_log_level_t level, const char* func,
                      const char* tag, const char* format, ...) {                       
  // create log entry
  PLog plog = PLog_init_default;
  plog.level = level;

  plog.tag.funcs.encode = &encode_string;
  plog.tag.arg = (void*)tag;

  plog.function.funcs.encode = &encode_string;
  plog.function.arg = (void*)func;

  // get variable arguments
  va_list list;
  va_start(list, format);

  /*
   * buffer here is used to create the string log message as well as
   * store the encoded protobuf message
   */
  uint8_t buffer[256];
  memset(buffer, 0, 256);
  vsprintf((char*)buffer, format, list);
  // vprintf(format, list);
  va_end(list);

  plog.message.funcs.encode = &encode_string;
  plog.message.arg = buffer;

  plog.time_ms = esp_log_timestamp();

  // create output stream
  uint8_t out_buffer[256];
  pb_ostream_t ostream = pb_ostream_from_buffer(out_buffer, sizeof(buffer));
  pb_encode(&ostream, PLog_fields, &plog);

  // write to nvs flash
  esp_err_t err = write_to_nvs(out_buffer, ostream.bytes_written);
  // clear buffer
  memset(out_buffer, 0, 256);

  // return error code
  if (err != ESP_OK) {
    ESP_LOGE("PERSISTENT_LOGS", "Error writing to NVS (%s)",
             esp_err_to_name(err));
  }
  return err;
}

esp_err_t read_old_logs(esp_err_t (*_send_mqtt_message)(uint8_t* buffer, size_t len), uint8_t *buffer) {
  esp_err_t err;
  nvs_handle_t p_log_handle;
  err = nvs_open("p_logs", NVS_READWRITE, &p_log_handle);
  if (err != ESP_OK) {
    ESP_LOGE(LOGGING_TAG, "Error opening NVS (%s)", esp_err_to_name(err));
    return err;
  }
  for (uint8_t i = 0; i <= CONFIG_MAX_PERSISTENT_LOGS; i++) {
    char key[4];
    sprintf(key, "%d", i);
    ESP_LOGI(LOGGING_TAG, "Reading NVS key: %s", key);
    // get size of stored BLOB
    size_t len;
    err = nvs_get_blob(p_log_handle, key, NULL, &len);
    
    ESP_LOGI(LOGGING_TAG, "Size of stored data: %d", len);

    if (len == 0) {ESP_LOGI(LOGGING_TAG, "Nothing stored here."); continue;}
    // get stored BLOB
    err = nvs_get_blob(p_log_handle, key, buffer, &len);
    if (err != ESP_OK) {
      ESP_LOGE(LOGGING_TAG, "Error reading NVS (%s)", esp_err_to_name(err));
      continue;
    }
    ESP_LOGI(LOGGING_TAG, "Read from NVS: PLogID:%d : %s", i, (char*)buffer);
    _send_mqtt_message(buffer, len);
  }
  memset(buffer, 0, 256);
  nvs_close(p_log_handle);
  return err;
}

#endif  // PERSISTENT_LOGS