##########################################################################################
## sensing_algo: WiFi sensing algorithms (radar, ...), built on AP side only.

ameba_add_subdirectory(radar)

ameba_submodule_register(sensing_algo
    p_EXAMPLE_DIRS
    ${c_CMPT_SENSING_ALGO_DIR}/example/radar/radar_detect
    ${c_CMPT_SENSING_ALGO_DIR}/example/radar/radar_offline
    ${c_CMPT_SENSING_ALGO_DIR}/example/radar/radar_raw
)
