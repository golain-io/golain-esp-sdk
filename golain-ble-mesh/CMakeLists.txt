idf_component_register(
    SRCS "include/golain_ble_comp.c"
    INCLUDE_DIRS "include"
    REQUIRES nanopb log driver nvs_flash bt
)
