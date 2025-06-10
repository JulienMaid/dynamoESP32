/*
 * pinout_definition.h
 *
 *  Created on: 15 mars 2024
 *      Author: julien
 */

#pragma once

#include <stdint.h>

// Pattes drivers pont H
static constexpr uint32_t c_u32_SD1 = 7;
static constexpr uint32_t c_u32_SD2 = 0;
static constexpr uint32_t c_u32_Pwm1 = 8;
static constexpr uint32_t c_u32_Pwm2 = 1;

// Entrees analogique
static constexpr uint32_t c_u32_ThrottleSetPoint = 5;
static constexpr uint32_t c_u32_NeutralSetPoint = 10;

static constexpr uint32_t c_u32_TemperatureSensor = 9;

static constexpr uint32_t c_u32_Led1 = 2;
static constexpr uint32_t c_u32_Led2 = 3;
static constexpr uint32_t c_u32_Led3 = 4;
