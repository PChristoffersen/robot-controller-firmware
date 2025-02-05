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

#include "util.h"

#include <zephyr/kernel.h>
#include <zephyr/version.h>
#include <zephyr/drivers/hwinfo.h>


void print_reset_cause()
{
	uint32_t cause = 0;
	hwinfo_get_reset_cause(&cause);
	printf("Reset cause(%04x): ", cause);
	if (cause & RESET_PIN) {
		printf(" pin");
		cause &= ~RESET_PIN;
	}
	if (cause & RESET_SOFTWARE) {
		printf(" software");
		cause &= ~RESET_SOFTWARE;
	}
	if (cause & RESET_BROWNOUT) {
		printf(" brownout");
		cause &= ~RESET_BROWNOUT;
	}
	if (cause & RESET_POR) {
		printf(" power-on-reset");
		cause &= ~RESET_POR;
	}
	if (cause & RESET_WATCHDOG) {
		printf(" watchdog");
		cause &= ~RESET_WATCHDOG;
	}
	if (cause & RESET_SECURITY) {
		printf(" security");
		cause &= ~RESET_SECURITY;
	}
	if (cause & RESET_LOW_POWER_WAKE) {
		printf(" low-power-wake");
		cause &= ~RESET_LOW_POWER_WAKE;
	}
	if (cause) {
		printf(" unknown(%04x)", cause);
	}
}