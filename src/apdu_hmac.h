#pragma once

#include "apdu.h"

size_t handle_apdu_hmac(uint8_t instruction, volatile uint32_t *flags);
