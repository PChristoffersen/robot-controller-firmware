#include "feature_engine.h"

namespace Feature {


Engine::Engine()
{

}


void Engine::begin()
{
    
}


void Engine::set_inputs(uint32 values, uint32 mask)
{

}


void Engine::update()
{
    m_output_configs_dirty = false;
    m_mode_configs_dirty = false;
    m_color_lut_dirty = false;
    m_brightness_lut_dirty = false;

    m_state_dirty = false;
}

}
