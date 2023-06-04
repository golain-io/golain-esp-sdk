idf_component_register(
    SRCS "src/golain.c" "src/golain_hal.c" "protos/device_health.pb.c" "protos/logs.pb.c"
    INCLUDE_DIRS "include" "protos"
    PRIV_REQUIRES bt log driver nvs_flash nanopb mqtt app_update
)