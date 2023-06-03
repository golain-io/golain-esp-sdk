idf_component_register(
    SRCS "include/golain_blemesh_comp.c"
    INCLUDE_DIRS "include"
    REQUIRES bt nanopb log driver nvs_flash
)
