idf_component_register(
    SRCS "src/golain.c" "src/golain_hal.c"
    INCLUDE_DIRS "include" "old"
    PRIV_REQUIRES bt log driver nvs_flash nanopb mqtt
)