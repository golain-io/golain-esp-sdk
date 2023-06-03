idf_component_register(
    SRCS "src/*.c"
    INCLUDE_DIRS "include"
    REQUIRES bt log driver nvs_flash nanopb
)