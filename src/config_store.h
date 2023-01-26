#pragma once

#include <Arduino.h>
#include "feature_types.h"


class ConfigStore {
    public:
        using flash_value = uint16;

        ConfigStore();

        void begin();

        void erase();

        bool load(Feature::OutputConfigs &feature);
        bool store(Feature::OutputConfigs &feature);

        bool load(Feature::ModeConfigs &feature);
        bool store(Feature::ModeConfigs &feature);

        bool load(Feature::ColorLUT &feature);
        bool store(Feature::ColorLUT &feature);

        bool load(Feature::BrightnessLUT &feature);
        bool store(Feature::BrightnessLUT &feature);

    private:

        bool _load(uint16 off, flash_value *data, uint16 size);
        bool _store(uint16 off, flash_value *data, uint16 size);
};