#pragma once
#include "../../include/toml.hpp"

struct PatchConfig {
    bool initialized = false;

    struct {
        bool active;
    } logging;

    struct {
        bool on;
    } s13_mode;

    void from_table(toml::parse_result &table) {
        logging.active = table["logging"]["active"].value_or(false);
        s13_mode.on = table["s13_mode"]["on"].value_or(false);

        initialized = true;
    }
};

extern PatchConfig global_config;