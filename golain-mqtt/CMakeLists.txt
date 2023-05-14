idf_component_register(
    SRCS "include/golain_mqtt.c"
    INCLUDE_DIRS "include"
    REQUIRES bt nanopb log driver nvs_flash Shadow mqtt
)
