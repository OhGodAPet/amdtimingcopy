// Copyright 2022 Wolf9466

#pragma once

#include <stdint.h>
#include "vram/navi.h"

#pragma pack(push, 1)

#define MAX_NUMBER_OF_VRAM_MODULES		16

typedef struct _ATOM_VRAM_MODULE_V7
{
	uint32_t	ChannelMapCfg;
	uint16_t	ModuleSize;
	uint16_t	PrivateReserved;
	uint16_t	EnableChannels;
	uint8_t		ExtMemoryID;
	uint8_t		MemoryType;
	uint8_t		ChannelNum;
	uint8_t		ChannelWidth;
	uint8_t		Density;
	uint8_t		Reserved0;
	uint8_t		Misc;
	uint8_t		VREFI;
	uint8_t		NPL_RT;
	uint8_t		Preamble;
	uint8_t		MemorySize;
	uint16_t	SEQSettingOffset;
	uint8_t		Reserved1;
	uint16_t	MR2Value;
	uint16_t	MR3Value;
	uint8_t		MemoryVendorID;
	uint8_t		RefreshRateFactor;
	uint8_t		FIFODepth;
	uint8_t		CDR_Bandwidth;
	char		PNString[20];
} ATOM_VRAM_MODULE_V7;


typedef struct _ATOM_VRAM_MODULE_V8
{
	uint32_t	ChannelMapCfg;
	uint16_t	ModuleSize;
	uint16_t	MCRamCfg;
	uint16_t	EnableChannels;
	uint8_t		ExtMemoryID;
	uint8_t		MemoryType;
	uint8_t		ChannelNum;
	uint8_t		ChannelWidth;
	uint8_t		Density;
	uint8_t		BankCol;
	uint8_t		Misc;
	uint8_t		VREFI;
	uint16_t	Reserved0;
	uint16_t	MemorySize;
	uint8_t		MCTuningSetID;
	uint8_t		RowNum;
	uint16_t	MR2Value;
	uint16_t	MR3Value;
	uint8_t		MemoryVendorID;
	uint8_t		RefreshRateFactor;
	uint8_t		FIFODepth;
	uint8_t		CDR_Bandwidth;
	uint32_t	ChannelMapCfg1;
	uint32_t	BankMapCfg;
	uint32_t	Reserved1;
	char		PNString[20];
} ATOM_VRAM_MODULE_V8;

// ATOM_VRAM_MODULE_V8 structs follow
typedef struct _VRAM_INFO_HEADER_V2_2
{
	ATOM_TABLE_HEADER CommonHeader;
	uint16_t MemAdjTblOffset;
	uint16_t MemClkPatchTblOffset;
	uint16_t MCAdjustPerTileTblOffset;
	uint16_t MCPhyInitTableOffset;
	uint16_t DRAMDataRemapTblOffset;
	uint16_t Reserved;
	uint8_t NumberOfVRAMModules;
	uint8_t MemoryClkPatchTblVersion;
	uint8_t VRAMModuleVersion;
	uint8_t MCPhyTileNumber;
} VRAM_INFO_HEADER_V2_2;

typedef struct _ATOM_VRAM_MODULE_V9
{
	uint32_t	MemorySize;
	uint32_t	ChannelEnable;
	uint32_t	AddrCfg;
	uint32_t	AddrSel;
	uint32_t	ColSel;
	uint16_t	ModuleSize;
	uint8_t		ExtMemoryID;
	uint8_t		MemoryType;
	uint8_t		ChannelNum;
	uint8_t		ChannelWidth;
	uint8_t		Density;
	uint8_t		MCTuningSetID;
	uint8_t		MemoryVendorRevisionID;		// [7:4] Revision, [3:0] Vendor
	uint8_t		RefreshRateFactor;
	uint16_t	Reserved1;
	char		PNString[20];
} ATOM_VRAM_MODULE_V9;

typedef struct _VRAM_INFO_HEADER_V2_3
{
	ATOM_TABLE_HEADER CommonHeader;
	uint16_t MemAdjTblOffset;
	uint16_t MemClkPatchTblOffset;
	uint16_t MCAdjustPerTileTblOffset;
	uint16_t MCPhyInitTableOffset;
	uint16_t DRAMDataRemapTblOffset;
	uint16_t Reserved1[3];
	uint8_t NumberOfVRAMModules;
	uint8_t Reserved2[2];
	uint8_t MCPhyTileNumber;
} VRAM_INFO_HEADER_V2_3;

typedef struct _ATOM_VRAM_MODULE_V10
{
	uint32_t 	MemorySize;					// 0x00 - 0x03
	uint32_t	ChannelEnable;				// 0x04 - 0x07
	uint32_t	MaxMemClk;					// 0x08 - 0x0B
	uint16_t	Reserved[3];				// 0x0C - 0x0D
	uint16_t	MemVoltage;					// 0x0E - 0x0F
	uint16_t	ModuleSize;					// 0x10 - 0x11
	uint8_t		ExtMemoryID;				// 0x12
	uint8_t		MemoryType;					// 0x13
	uint8_t		ChannelNum;					// 0x14
	uint8_t		ChannelWidth;				// 0x15
	uint8_t		Density;					// 0x16
	uint8_t		MCTuningSetID;				// 0x17
	uint8_t		MemoryVendorRevisionID;		// 0x18; [7:4] Revision, [3:0] Vendor
	uint8_t		RefreshRateFactor;			// 0x19
	uint8_t		VRAMFlags;					// 0x1A; Bit 0: Bank grouping enable/disable
	uint8_t		Reserved2;					// 0x1B
	GDDR6_MODE_REG_10	GDDR6_MR10;			// 0x1C - 0x1D
	GDDR6_MODE_REG_1	GDDR6_MR1;			// 0x1E - 0x1F
	GDDR6_MODE_REG_2	GDDR6_MR2;			// 0x20 - 0x21
	GDDR6_MODE_REG_7	GDDR6_MR7;			// 0x22 - 0x23
	char PNString[20];						// 0x24 - 0x37
} ATOM_VRAM_MODULE_V10;

typedef struct _ATOM_VRAM_MODULE_V11
{
	uint32_t 	MemorySize;					// 0x00 - 0x03
	uint32_t	ChannelEnable;				// 0x04 - 0x07
	uint16_t	MemVoltage;					// 0x08 - 0x0A
	uint16_t	ModuleSize;					// 0x0A - 0x0C
	uint8_t		ExtMemoryID;				// 0x0D
	uint8_t		MemoryType;					// 0x0E
	uint8_t		ChannelNum;					// 0x0F
	uint8_t		ChannelWidth;				// 0x10
	uint8_t		Density;					// 0x11
	uint8_t		MCTuningSetID;				// 0x12
	uint16_t	Reserved[4];				// 0x13 - 0x1A
	uint8_t		MemoryVendorRevisionID;		// 0x1A; [7:4] Revision, [3:0] Vendor
	uint8_t		RefreshRateFactor;			// 0x1B; [1:0]=RefreshFactor (00=8ms, 01=16ms, 10=32ms,11=64ms)
	uint8_t		VRAMFlags;					// 0x1C; Bit 0: Bank grouping enable/disable
	uint8_t		Reserved2;					// 0x1D
	GDDR6_MODE_REG_10	GDDR6_MR10;			// 0x1E - 0x1F
	GDDR6_MODE_REG_0	GDDR6_MR0;			// 0x20 - 0x22
	GDDR6_MODE_REG_1	GDDR6_MR1;			// 0x23 - 0x24
	GDDR6_MODE_REG_2	GDDR6_MR2;			// 0x25 - 0x26
	GDDR6_MODE_REG_4	GDDR6_MR4;			// 0x27 - 0x28
	GDDR6_MODE_REG_7 	GDDR6_MR7;			// 0x29 - 0x1A
	GDDR6_MODE_REG_8	GDDR6_MR8;			// 0x1B - 0x1C
	char PNString[40];						// Ends with NULL
} ATOM_VRAM_MODULE_V11;

typedef union _ATOM_VRAM_MODULE
{
	ATOM_VRAM_MODULE_V7		AsModuleV7;
	ATOM_VRAM_MODULE_V8		AsModuleV8;
	ATOM_VRAM_MODULE_V9		AsModuleV9;
	ATOM_VRAM_MODULE_V10	AsModuleV10;
	ATOM_VRAM_MODULE_V11	AsModuleV11;
} ATOM_VRAM_MODULE;

typedef struct _VRAM_INFO_HEADER_V2_4
{
	ATOM_TABLE_HEADER CommonHeader;		// 0x00 - 0x03
	uint16_t MemAdjTblOffset;			// 0x04 - 0x05
	uint16_t MemClkPatchTblOffset;		// 0x06 - 0x07
	uint16_t MCAdjustPerTileTblOffset;	// 0x08 - 0x09
	uint16_t MCPhyInitTableOffset;		// 0x0A - 0x0B
	uint16_t DRAMDataRemapTblOffset;	// 0x0C - 0x0D
	uint16_t Reserved1;					// 0x0E - 0x0F
	uint16_t PostUcodeInitOffset;		// 0x10 - 0x11
	uint16_t Reserved2;					// 0x12 - 0x13
	uint8_t NumberOfVRAMModules;		// 0x14
	uint8_t UMC_IP_MIN_VER;				// 0x15
	uint8_t UMC_IP_MAX_VER;				// 0x16
	uint8_t MCPhyTileNumber;			// 0x17
} VRAM_INFO_HEADER_V2_4;

typedef struct _VRAM_INFO_HEADER_V2_5
{
	ATOM_TABLE_HEADER CommonHeader;
	uint16_t MemAdjTblOffset;
	uint16_t GDDR6ACTimingOffset;
	uint16_t MCAdjustPerTileTblOffset;
	uint16_t MCPhyInitTableOffset;
	uint16_t DRAMDataRemapTblOffset;
	uint16_t Reserved;
	uint16_t PostUcodeInitOffset;
	uint16_t StrobeModePatchTblOffset;
	uint8_t NumberOfVRAMModules;
	uint8_t UMC_IP_MIN_VER;
	uint8_t UMC_IP_MAX_VER;
	uint8_t MCPhyTileNumber;
} VRAM_INFO_HEADER_V2_5;

typedef struct _TIMING_ENTRY
{
	uint32_t StartingMemClk;		// Must mask highest 8 bits
	uint32_t Timings[12];
} TIMING_ENTRY;

// MemClkRange is given in 10Khz steps.
typedef struct _ATOM_MEMORY_SETTING_ID_CONFIG
{
	uint32_t MemClkRange : 24;
	uint32_t MemBlkID : 8;
} ATOM_MEMORY_SETTING_ID_CONFIG;

typedef struct _ATOM_MEMORY_SETTING_ID_CONFIG_ACCESS
{
	ATOM_MEMORY_SETTING_ID_CONFIG slAccess;
	uint32_t ulAccess;
} ATOM_MEMORY_SETTING_ID_CONFIG_ACCESS;

typedef struct _ATOM_MEMORY_SETTING_DATA_BLOCK
{
	ATOM_MEMORY_SETTING_ID_CONFIG_ACCESS MemoryID;
} ATOM_MEMORY_SETTING_DATA_BLOCK;

typedef struct _ATOM_INIT_REG_INDEX_FORMAT
{
	uint16_t RegIdx;				// Index of MC register
	uint8_t DataOffset;				// Offset within the ATOM_MEMORY_SETTING_DATA_BLOCK structures
} ATOM_INIT_REG_INDEX_FORMAT;

typedef struct _REG_ADDR_INFO
{
	uint32_t RegAddr : 24;
	uint32_t RegType : 1;
	uint32_t Reserved : 7;
} REG_ADDR_INFO;

typedef union _INIT_REG_INDEX_FORMAT_VEGA
{
	REG_ADDR_INFO RegInfo;
	uint32_t RegInfo32;
} INIT_REG_INDEX_FORMAT_VEGA;

typedef struct _VEGA_ID_ACCESS
{
	uint32_t MemClkRange : 24;
	uint32_t MemBlkID : 8;
} VEGA_ID_ACCESS;

typedef union _VEGA_DATA_BLOCK_ID_CONFIG_ACCESS
{
	VEGA_ID_ACCESS IDAccess;
	uint32_t IDAccess32;
} VEGA_DATA_BLOCK_ID_CONFIG_ACCESS;

typedef struct _VEGA_DATA_BLOCK
{
	VEGA_DATA_BLOCK_ID_CONFIG_ACCESS BlockID;
	uint32_t Data[1];
} VEGA_DATA_BLOCK;

typedef struct _INIT_REG_BLOCK_VEGA
{
	uint16_t RegIndexTblNum;
	uint16_t Reserved;
} INIT_REG_BLOCK_VEGA;

// Following this is a variable-sized list of ATOM_INIT_REG_INDEX_FORMAT
// structures, followed by a variable-sized list of ATOM_MEMORY_SETTING_DATA_BLOCK
// structures. RegDataBlkSize is the size of one ATOM_MEMORY_SETTING_DATA_BLOCK.
// The number of data blocks is not known ahead of time, but is terminated by
// a structure consisting of all zero (32-bits of zero.) The register index table
// is terminated by a structure with RegIdx == 0xFFFF and DataOffset == 0x00.

// Parsing works as follows: For every entry in the register index table,
// seek forward one dword (32 bits, 4 bytes.) So, to access the value for
// the register listed in the third index table entry (counting from zero),
// you would use the following offset into your data block (getting DataOffset
// from the same register index table entry, of course):
// sizeof(uint32_t) * 3 + (DataOffset & 0x7F)
// There is a slight problem, though - entries with DataOffset == 0x00
// don't count towards your index. Keep this in mind.

// The masking out of the high bit is due to it seeming to be a flag of some
// sort, although for what is currently unknown. In the event DataOffset is
// zero, this probably indicates that the value for that MC register is not
// present, although this is currently only speculation. The DataOffset,
// if valid, should be at LEAST 4 bytes, as one must seek past the data
// block's header.
typedef struct _ATOM_INIT_REG_BLOCK
{
	uint16_t RegIndexTblSize;
	uint16_t RegDataBlkSize;
} ATOM_INIT_REG_BLOCK;

#define SAMSUNG             0x1
#define INFINEON            0x2
#define ELPIDA              0x3
#define ETRON               0x4
#define NANYA               0x5
#define HYNIX               0x6
#define MOSEL               0x7
#define WINBOND             0x8
#define ESMT                0x9
#define MICRON              0xF

const char *MemVendorNameStrs[0x10] =
{
	"Reserved",
	"Samsung",
	"Infineon",
	"Elpida",
	"Etron",
	"Nanya",
	"Hynix",
	"Mosel",
	"Winbond",
	"ESMT",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Micron"
};

#pragma pack(pop)
