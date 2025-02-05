/**
 * BSD 3-Clause License
 * 
 * Copyright (c) 2025, Peter Christoffersen
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <stdint.h>

#if CONFIG_SHIELD_ROBOT_CONTROLLER_STANDALONE
#define CONTROLLER_BUTTON_COUNT 11
#define CONTROLLER_LED_COUNT 6
#endif
#if CONFIG_SHIELD_ROBOT_CONTROLLER_EMBEDDED
#define CONTROLLER_BUTTON_COUNT 9
#define CONTROLLER_LED_GPIO_COUNT 2
#define CONTROLLER_LED_COUNT 4
#endif

#define CONTROLLER_WS2812_COUNT 4

#define CONTROLLER_AXIS_MIN -2048
#define CONTROLLER_AXIS_MAX 2047
#define CONTROLLER_AXIS_COUNT 6
#define CONTROLLER_AXIS_CHANGE_THRESHOLD 16

#define CONTROLLER_DIAL_MIN -7
#define CONTROLLER_DIAL_MAX 7


void controller_reset();
void controller_set_enabled(bool enabled);
void controller_set_suspend(bool enabled);

int controller_set_output(const uint8_t *buffer, uint16_t bufsize);
void controller_set_idle(const uint32_t duration);
uint32_t controller_get_idle();

void controller_set_autonomous_mode(bool autonomous);
bool controller_is_autonomous_mode();
void controller_input_report_done();
