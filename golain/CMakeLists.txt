idf_component_register(
    INCLUDE_DIRS "include"
    REQUIRES golain-ble-mesh device-shadow golain-mqtt MQTT_OTA persistent_logs device-health
)
