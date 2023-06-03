idf_component_register(
    SRCS "protos/device_health.pb.c"
    INCLUDE_DIRS "protos" "include"
    REQUIRES nanopb log driver nvs_flash persistent_logs
)