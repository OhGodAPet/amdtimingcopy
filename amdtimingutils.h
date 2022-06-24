#pragma once

#include <jansson.h>		// for the json_t type

// Structures, function prototypes, and other things
// useful to any tool requiring AMD timing utilities.
#include "VRAMInfo.h"

#define MAX_NUMBER_OF_VRAM_MODULES			16
#define AMD_VBIOS_GDDR5_TIMING_RAW_SIZE		0x30
#define AMD_VBIOS_GDDR5_TIMING_HEX_SIZE		(AMD_VBIOS_TIMING_RAW_SIZE << 1)

// Polaris uses less, but Navi1x needs at least
// 112 bytes (224 ASCII hex digits) for the GDDR6
// timing set. The Polaris timing set only requires
// 48 bytes (96 ASCII hex digits) for its timing set.

#define AMD_NAVI1X_INPUT_TIMING_HEX_SIZE	224
#define AMD_NAVI1X_INPUT_TIMING_RAW_SIZE	(AMD_NAVI1X_INPUT_TIMING_HEX_SIZE >> 1)
#define AMD_POLARIS_INPUT_TIMING_HEX_SIZE	96
#define AMD_POLARIS_INPUT_TIMING_RAW_SIZE	(AMD_POLARIS_INPUT_TIMING_HEX_SIZE >> 1)
#define AMD_VBIOS_GDDR6_TIMING_RAW_SIZE		0x70
#define AMD_VBIOS_GDDR6_TIMING_HEX_SIZE		(AMD_VBIOS_GDDR6_TIMING_RAW_SIZE << 1)

// Prototypes for the dumping functions located in navi10_json.c
// and navi21_json.c.
json_t *DumpNaviRegsAsJSON(const uint32_t *RegTemplate);
bool LoadNaviRegsFromJSON(uint32_t *RegTemplate, json_t *JSONCfg);

json_t *DumpNavi2RegsAsJSON(const Navi21TimingFmt *val);
bool LoadNavi2RegsFromJSON(Navi21TimingFmt *val, json_t *JSONCfg);
