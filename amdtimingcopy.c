// Copyright 2022 Wolf9466

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>

#include "vbios-tables.h"
#include "gddr6.h"
#include "VRAMInfo.h"

#define MAX_NUMBER_OF_VRAM_MODULES		16
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

json_t *DumpNaviRegsAsJSON(const uint32_t *RegTemplate);
bool LoadNaviRegsFromJSON(uint32_t *RegTemplate, json_t *JSONCfg);

json_t *DumpNavi2RegsAsJSON(const Navi21TimingFmt *val);
bool LoadNavi2RegsFromJSON(Navi21TimingFmt *val, json_t *JSONCfg);

// Parameter len is the size in bytes of asciistr, meaning rawstr
// must have (len >> 1) bytes allocated
// Maybe asciistr just NULL terminated?
// Returns length of rawstr in bytes
int ASCIIHexToBinary(void *restrict rawstr, const char *restrict asciistr, size_t len)
{
	for(int i = 0, j = 0; i < len; ++i)
	{
		char tmp = asciistr[i];
		if(tmp < 'A') tmp -= '0';
		else if(tmp < 'a') tmp = (tmp - 'A') + 10;
		else tmp = (tmp - 'a') + 10;

		if(i & 1) ((uint8_t *)rawstr)[j++] |= tmp & 0x0F;
		else ((uint8_t *)rawstr)[j] = tmp << 4;
	}

	return(len >> 1);
}

// Parameter len is bytes in rawstr, therefore, asciistr must have
// at least (len << 1) + 1 bytes allocated, the last for the NULL
void BinaryToASCIIHex(char *restrict asciistr, const void *restrict rawstr, size_t len)
{
	for(int i = 0, j = 0; i < len; ++i)
	{
		asciistr[j++] = "0123456789ABCDEF"[((uint8_t *)rawstr)[i] >> 4];
		asciistr[j++] = "0123456789ABCDEF"[((uint8_t *)rawstr)[i] & 0x0F];
	}

	asciistr[len << 1] = 0x00;
}

void usage(char *FileName)
{
	printf("Usage: %s\n", FileName);
	printf("\t--file\t\t| -f\tVBIOS File\n");
	printf("\t--show\t\t| -s\tShow Info\n");
	printf("\t--verbose\t| -v\tShow additional details\n");
	printf("\t--module\t| -m\tSelect module\n");
	printf("\t--read\t\t| -r\tMemClk\n");
	printf("\t--write\t\t| -w\tMemClk JSONTimingsFile\n");
	printf("\nIf the -m/--module flag is not specified when using -r/--read or -w/--write, it will default to module index 0.\n");
	printf("The -s/--show flag cannot be specified when using -r/--read or -w/--write.\n");
	printf("The -r/--read and -w/--write flags are mutually exclusive.\n");
	printf("The -f/--file flag is always required.\n\n");
	
	exit(-1);
}

uint32_t DecodeDensity(uint8_t DensityCode)
{
	uint16_t Size = 4 << (DensityCode >> 4);
	uint16_t Count = 4 << (DensityCode & 0x0F);

	return((((uint32_t)Size) << 16) | Count);
}

// It is the CALLER'S responsibility to ensure DataOffset has been masked
// appropriately and is nonzero, as well as to ensure IRIdx is in range.
uint32_t LookupTimingValue(uint8_t DataOffset, uint32_t IRIdx, const ATOM_MEMORY_SETTING_DATA_BLOCK *MemSettingDataBlk)
{
	uint8_t *MemBlockBase = (uint8_t *)MemSettingDataBlk;

	MemBlockBase += (IRIdx << 2) + DataOffset;
	return(*((uint32_t *)MemBlockBase));
}

bool ReadTimings(const VRAM_INFO_HEADER_V2_2 *VRInfo)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint16_t IRIdxEntries, MemClkPatchTblOffset;

	MemClkPatchTblOffset = VRInfo->MemClkPatchTblOffset;

	ATOM_INIT_REG_BLOCK *IRBlk = (ATOM_INIT_REG_BLOCK *)(VRInfoBase + MemClkPatchTblOffset);

	IRIdxEntries = IRBlk->RegIndexTblSize / sizeof(ATOM_INIT_REG_INDEX_FORMAT);

	if(IRBlk->RegIndexTblSize % sizeof(ATOM_INIT_REG_INDEX_FORMAT)) return(false);

	ATOM_INIT_REG_INDEX_FORMAT *IRIdxList = (ATOM_INIT_REG_INDEX_FORMAT *)(VRInfoBase + MemClkPatchTblOffset + sizeof(ATOM_INIT_REG_BLOCK));
	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MemClkPatchTblOffset + sizeof(ATOM_INIT_REG_BLOCK) + (sizeof(ATOM_INIT_REG_INDEX_FORMAT) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;

	do
	{
		printf("\nTimings for %u\n", ((ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk)->MemoryID.slAccess.MemClkRange / 100);

		for(int i = 0, MemIdx = 0; i < IRIdxEntries; ++i)
		{
			printf("RegIdx = 0x%04X - DataOffset = 0x%02X - ", IRIdxList[i].RegIdx, IRIdxList[i].DataOffset);
			if(!IRIdxList[i].DataOffset) printf("Data not present.\n");
			else printf("Data = 0x%08X\n", LookupTimingValue(IRIdxList[i].DataOffset & 0x7F, MemIdx++, (ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk));
		}

		CurrentMemSettingDataBlk += IRBlk->RegDataBlkSize;
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	putchar('\n');

	return(true);
}

bool ReadPhyInit(const VRAM_INFO_HEADER_V2_2 *VRInfo)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint16_t IRIdxEntries, MCPhyInitTableOffset;

	MCPhyInitTableOffset = VRInfo->MCPhyInitTableOffset;

	ATOM_INIT_REG_BLOCK *IRBlk = (ATOM_INIT_REG_BLOCK *)(VRInfoBase + MCPhyInitTableOffset);

	IRIdxEntries = IRBlk->RegIndexTblSize / sizeof(ATOM_INIT_REG_INDEX_FORMAT);

	if(IRBlk->RegIndexTblSize % sizeof(ATOM_INIT_REG_INDEX_FORMAT)) return(false);

	ATOM_INIT_REG_INDEX_FORMAT *IRIdxList = (ATOM_INIT_REG_INDEX_FORMAT *)(VRInfoBase + MCPhyInitTableOffset + sizeof(ATOM_INIT_REG_BLOCK));
	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MCPhyInitTableOffset + sizeof(ATOM_INIT_REG_BLOCK) + (sizeof(ATOM_INIT_REG_INDEX_FORMAT) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;

	do
	{
		printf("\nTimings for %u\n", ((ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk)->MemoryID.slAccess.MemClkRange / 100);

		for(int i = 0, MemIdx = 0; i < IRIdxEntries; ++i)
		{
			printf("RegIdx = 0x%04X - DataOffset = 0x%02X - ", IRIdxList[i].RegIdx, IRIdxList[i].DataOffset);
			if(!IRIdxList[i].DataOffset) printf("Data not present.\n");
			else printf("Data = 0x%08X\n", LookupTimingValue(IRIdxList[i].DataOffset & 0x7F, MemIdx++, (ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk));
		}

		CurrentMemSettingDataBlk += IRBlk->RegDataBlkSize;
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	putchar('\n');

	return(true);
}

bool ReadTimings2_3(VRAM_INFO_HEADER_V2_3 *VRInfo)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint32_t MemClkPatchTblOffset = VRInfo->MemClkPatchTblOffset;

	INIT_REG_BLOCK_VEGA *IRBlk = (INIT_REG_BLOCK_VEGA *)(VRInfoBase + MemClkPatchTblOffset);
	uint32_t IRIdxEntries = IRBlk->RegIndexTblNum;

	//if(IRBlk->RegIndexTblNum % sizeof(INIT_REG_INDEX_FORMAT_VEGA)) return(false);

	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MemClkPatchTblOffset + sizeof(INIT_REG_BLOCK_VEGA) + (sizeof(INIT_REG_INDEX_FORMAT_VEGA) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;
	INIT_REG_INDEX_FORMAT_VEGA *IRIdxList = (INIT_REG_INDEX_FORMAT_VEGA *)(VRInfoBase + MemClkPatchTblOffset + sizeof(INIT_REG_BLOCK_VEGA));

	do
	{
		printf("Timings for %u\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100);

		for(int i = 0; i < IRIdxEntries; ++i)
		{
			printf("Index = %d - RegAddr = 0x%08X - RegType = %d - ", i, IRIdxList[i].RegInfo.RegAddr << 2, IRIdxList[i].RegInfo.RegType);
			printf("Data = 0x%08X\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]);
			uint32_t MemClk = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100;
		}

		CurrentMemSettingDataBlk += (sizeof(uint32_t) * (IRIdxEntries + 1));
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	return(true);
}

bool ReadPhyInit2_3(VRAM_INFO_HEADER_V2_3 *VRInfo)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint32_t MCPhyInitTableOffset = VRInfo->MCPhyInitTableOffset;

	INIT_REG_BLOCK_VEGA *IRBlk = (INIT_REG_BLOCK_VEGA *)(VRInfoBase + MCPhyInitTableOffset);
	uint32_t IRIdxEntries = IRBlk->RegIndexTblNum;

	//if(IRBlk->RegIndexTblNum % sizeof(INIT_REG_INDEX_FORMAT_VEGA)) return(false);

	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MCPhyInitTableOffset + sizeof(INIT_REG_BLOCK_VEGA) + (sizeof(INIT_REG_INDEX_FORMAT_VEGA) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;
	INIT_REG_INDEX_FORMAT_VEGA *IRIdxList = (INIT_REG_INDEX_FORMAT_VEGA *)(VRInfoBase + MCPhyInitTableOffset + sizeof(INIT_REG_BLOCK_VEGA));

	do
	{
		printf("Timings for %u\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100);

		for(int i = 0; i < IRIdxEntries; ++i)
		{
			printf("Index = %d - RegAddr = 0x%08X - RegType = %d - ", i, IRIdxList[i].RegInfo.RegAddr << 2, IRIdxList[i].RegInfo.RegType);
			printf("Data = 0x%08X\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]);
			uint32_t MemClk = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100;
		}

		CurrentMemSettingDataBlk += (sizeof(uint32_t) * (IRIdxEntries + 1));
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	return(true);
}

bool ReadMemAdj2_3(VRAM_INFO_HEADER_V2_3 *VRInfo)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint32_t MemAdjTblOffset = VRInfo->MemAdjTblOffset;

	INIT_REG_BLOCK_VEGA *IRBlk = (INIT_REG_BLOCK_VEGA *)(VRInfoBase + MemAdjTblOffset);
	uint32_t IRIdxEntries = IRBlk->RegIndexTblNum;

	//if(IRBlk->RegIndexTblNum % sizeof(INIT_REG_INDEX_FORMAT_VEGA)) return(false);

	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MemAdjTblOffset + sizeof(INIT_REG_BLOCK_VEGA) + (sizeof(INIT_REG_INDEX_FORMAT_VEGA) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;
	INIT_REG_INDEX_FORMAT_VEGA *IRIdxList = (INIT_REG_INDEX_FORMAT_VEGA *)(VRInfoBase + MemAdjTblOffset + sizeof(INIT_REG_BLOCK_VEGA));

	do
	{
		printf("Timings for %u\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100);

		for(int i = 0; i < IRIdxEntries; ++i)
		{
			printf("Index = %d - RegAddr = 0x%08X - RegType = %d - ", i, IRIdxList[i].RegInfo.RegAddr << 2, IRIdxList[i].RegInfo.RegType);
			printf("Data = 0x%08X\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]);
			uint32_t MemClk = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100;
		}

		CurrentMemSettingDataBlk += (sizeof(uint32_t) * (IRIdxEntries + 1));
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	return(true);
}

bool ReadTimings2_4(VRAM_INFO_HEADER_V2_4 *VRInfo)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint32_t MemClkPatchTblOffset = VRInfo->MemClkPatchTblOffset;

	INIT_REG_BLOCK_VEGA *IRBlk = (INIT_REG_BLOCK_VEGA *)(VRInfoBase + MemClkPatchTblOffset);
	uint32_t IRIdxEntries = IRBlk->RegIndexTblNum;

	//if(IRBlk->RegIndexTblNum % sizeof(INIT_REG_INDEX_FORMAT_VEGA)) return(false);

	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MemClkPatchTblOffset + sizeof(INIT_REG_BLOCK_VEGA) + (sizeof(INIT_REG_INDEX_FORMAT_VEGA) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;
	INIT_REG_INDEX_FORMAT_VEGA *IRIdxList = (INIT_REG_INDEX_FORMAT_VEGA *)(VRInfoBase + MemClkPatchTblOffset + sizeof(INIT_REG_BLOCK_VEGA));

	do
	{
		printf("Timings for %u\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100);

		for(int i = 0; i < IRIdxEntries; ++i)
		{
			printf("Index = %d - RegAddr = 0x%08X - RegType = %d - ", i, IRIdxList[i].RegInfo.RegAddr << 2, IRIdxList[i].RegInfo.RegType);
			printf("Data = 0x%08X\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]);
			uint32_t MemClk = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100;
		}

		CurrentMemSettingDataBlk += (sizeof(uint32_t) * (IRIdxEntries + 1));
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	return(true);
}

bool ReadPhyInit2_4(VRAM_INFO_HEADER_V2_4 *VRInfo)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint32_t MCPhyInitTableOffset = VRInfo->MCPhyInitTableOffset;

	INIT_REG_BLOCK_VEGA *IRBlk = (INIT_REG_BLOCK_VEGA *)(VRInfoBase + MCPhyInitTableOffset);
	uint32_t IRIdxEntries = IRBlk->RegIndexTblNum;

	//if(IRBlk->RegIndexTblNum % sizeof(INIT_REG_INDEX_FORMAT_VEGA)) return(false);

	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MCPhyInitTableOffset + sizeof(INIT_REG_BLOCK_VEGA) + (sizeof(INIT_REG_INDEX_FORMAT_VEGA) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;
	INIT_REG_INDEX_FORMAT_VEGA *IRIdxList = (INIT_REG_INDEX_FORMAT_VEGA *)(VRInfoBase + MCPhyInitTableOffset + sizeof(INIT_REG_BLOCK_VEGA));

	do
	{
		printf("Timings for %u\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100);

		for(int i = 0; i < IRIdxEntries; ++i)
		{
			printf("Index = %d - RegAddr = 0x%08X - RegType = %d - ", i, IRIdxList[i].RegInfo.RegAddr << 2, IRIdxList[i].RegInfo.RegType);
			printf("Data = 0x%08X\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]);
			uint32_t MemClk = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100;
		}

		CurrentMemSettingDataBlk += (sizeof(uint32_t) * (IRIdxEntries + 1));
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	return(true);
}

bool ReadMemAdj2_4(VRAM_INFO_HEADER_V2_4 *VRInfo, bool Edit)
{
	uint8_t *VRInfoBase = (uint8_t *)VRInfo;
	uint32_t MemAdjTblOffset = VRInfo->MemAdjTblOffset;

	INIT_REG_BLOCK_VEGA *IRBlk = (INIT_REG_BLOCK_VEGA *)(VRInfoBase + MemAdjTblOffset);
	uint32_t IRIdxEntries = IRBlk->RegIndexTblNum;

	//if(IRBlk->RegIndexTblNum % sizeof(INIT_REG_INDEX_FORMAT_VEGA)) return(false);

	uint8_t *MemSettingDataBlkBase = (VRInfoBase + MemAdjTblOffset + sizeof(INIT_REG_BLOCK_VEGA) + (sizeof(INIT_REG_INDEX_FORMAT_VEGA) * IRIdxEntries));
	uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;
	INIT_REG_INDEX_FORMAT_VEGA *IRIdxList = (INIT_REG_INDEX_FORMAT_VEGA *)(VRInfoBase + MemAdjTblOffset + sizeof(INIT_REG_BLOCK_VEGA));

	do
	{
		printf("Timings for %u\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100);

		for(int i = 0; i < IRIdxEntries; ++i)
		{
			// WARNING: Does test edits to enable bank grouping
			if(((IRIdxList[i].RegInfo.RegAddr << 2) == 0x53140) && Edit)
			{
				printf("Enabling bank grouping.\n");
				((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i] |= (1U << 11);
			}
				
			printf("Index = %d - RegAddr = 0x%08X - RegType = %d - ", i, IRIdxList[i].RegInfo.RegAddr << 2, IRIdxList[i].RegInfo.RegType);
			printf("Data = 0x%08X\n", ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]);
			uint32_t MemClk = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100;
		}

		CurrentMemSettingDataBlk += (sizeof(uint32_t) * (IRIdxEntries + 1));
	} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);

	return(true);
}

void DumpVRAMModuleData(ATOM_VRAM_MODULE *Module, uint8_t VRAM_InfoFmtRev, uint8_t VRAM_InfoCntRev)
{
	if(VRAM_InfoFmtRev == 2)
	{
		if(VRAM_InfoCntRev == 2)
		{
			uint32_t Density = DecodeDensity(Module->AsModuleV8.Density);
			printf("\tMemory Vendor: %s\n", MemVendorNameStrs[Module->AsModuleV8.MemoryVendorID & 0xF]);
			printf("\tMemory Revision: %d\n", Module->AsModuleV8.MemoryVendorID >> 4);
			printf("\tPart Number: %s\n", Module->AsModuleV8.PNString);
			printf("\tMemory Size: %dMB\n", Module->AsModuleV8.MemorySize);
			printf("\tMemory Density: %dMx%d\n", Density >> 16, Density & 0xFFFF);
			printf("\tMR3 Value: 0x%04X\n\n", Module->AsModuleV8.MR3Value);
		}
		else if(VRAM_InfoCntRev == 3)
		{
			uint32_t Density = DecodeDensity(Module->AsModuleV9.Density);
			printf("\tMemory Vendor: %s\n", MemVendorNameStrs[Module->AsModuleV9.MemoryVendorRevisionID & 0xF]);
			printf("\tMemory Revision: %d\n", Module->AsModuleV9.MemoryVendorRevisionID >> 4);
			printf("\tPart Number: %s\n", Module->AsModuleV9.PNString);
			printf("\tMemory Size: %dMB\n", Module->AsModuleV9.MemorySize);
			printf("\tMemory Density: %dMx%d\n", Density >> 16, Density & 0xFFFF);
			printf("\tChannel Count: %d\n", Module->AsModuleV9.ChannelNum);
			printf("\tChannel Width: %d\n", Module->AsModuleV9.ChannelWidth);
		}
		else if(VRAM_InfoCntRev == 4)
		{
		 	uint32_t Density = DecodeDensity(Module->AsModuleV10.Density);
			printf("\tMemory Vendor: %s\n", MemVendorNameStrs[Module->AsModuleV10.MemoryVendorRevisionID & 0xF]);
			printf("\tMemory Revision: %d\n", Module->AsModuleV10.MemoryVendorRevisionID >> 4);
			printf("\tPart Number: %s\n", Module->AsModuleV10.PNString);
			printf("\tMemory Size: %dMB\n", Module->AsModuleV10.MemorySize);
			printf("\tMemory Density: %dMx%d\n", Density >> 16, Density & 0xFFFF);
			printf("\tMemory Voltage: %d\n", Module->AsModuleV10.MemVoltage);
			printf("\tChannel Count: %d\n", Module->AsModuleV10.ChannelNum);
			printf("\tChannel Width: %d\n", Module->AsModuleV10.ChannelWidth);
			printf("\tRefresh Rate Factor: %d\n", Module->AsModuleV10.RefreshRateFactor);
			printf("\tBank grouping: %s\n", ((Module->AsModuleV10.VRAMFlags & 1) ? "Enabled" : "Disabled"));
			printf("\tGDDR6 MR10: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV10.GDDR6_MR10)));
			printf("\t\tVREFC Offset: %d\n", Module->AsModuleV10.GDDR6_MR10.VREFCOffset);
			printf("\t\tWCK0 Invert/Quad Shift: %d\n", Module->AsModuleV10.GDDR6_MR10.WCK0InvertQuadShift);
			printf("\t\tWCK1 Invert/Quad Shift: %d\n", Module->AsModuleV10.GDDR6_MR10.WCK1InvertQuadShift);
			printf("\t\tWCK2CK Offset: %d\n", Module->AsModuleV10.GDDR6_MR10.WCK2CK);
			printf("\t\tWCKRatio: %d\n", Module->AsModuleV10.GDDR6_MR10.WCKRatio);
			printf("\t\tWCK Termination: %d\n", Module->AsModuleV10.GDDR6_MR10.WCKTermination);
			printf("\tGDDR6 MR1: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV10.GDDR6_MR1)));
			printf("\t\tDrive Strength: %d\n", Module->AsModuleV10.GDDR6_MR1.DriveStrength);
			printf("\t\tData Termination: %d\n", Module->AsModuleV10.GDDR6_MR1.DataTermination);
			printf("\t\tPLL Range: %d\n", Module->AsModuleV10.GDDR6_MR1.PLLDLLRange);
			printf("\t\tCalibration Update: %d\n", Module->AsModuleV10.GDDR6_MR1.CalibrationUpdate);
			printf("\t\tPLL Enable: %d\n", Module->AsModuleV10.GDDR6_MR1.PLLDLLEnable);
			printf("\t\tRead DBI: %d\n", Module->AsModuleV10.GDDR6_MR1.ReadDBI);
			printf("\t\tWrite DBI: %d\n", Module->AsModuleV10.GDDR6_MR1.WriteDBI);
			printf("\t\tCABI: %d\n", Module->AsModuleV10.GDDR6_MR1.CABI);
			printf("\t\tPLL Reset: %d\n", Module->AsModuleV10.GDDR6_MR1.PLLReset);
			printf("\tGDDR6 MR2: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV10.GDDR6_MR2)));
			printf("\t\tOCD Pulldown Driver Offset: %d\n", Module->AsModuleV10.GDDR6_MR2.OCDPulldownDriverOff);
			printf("\t\tOCD Pullup Driver Offset: %d\n", Module->AsModuleV10.GDDR6_MR2.OCDPullupDriverOff);
			printf("\t\tSelfRefresh: %d\n", Module->AsModuleV10.GDDR6_MR2.SelfRefresh);
			printf("\t\tEDC Mode: %d\n", Module->AsModuleV10.GDDR6_MR2.EDCMode);
			printf("\t\tRDQS: %d\n", Module->AsModuleV10.GDDR6_MR2.RDQS);
			printf("\t\tCADT SRF: %d\n", Module->AsModuleV10.GDDR6_MR2.CADTSRF);
			printf("\t\tEDC HR: %d\n", Module->AsModuleV10.GDDR6_MR2.EDCHR);
			printf("\tGDDR6 MR7: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV10.GDDR6_MR7)));
			printf("\t\tWCK Alignment Point: %d\n", Module->AsModuleV10.GDDR6_MR7.WCKAlignPt);
			printf("\t\tHibernate: %d\n", Module->AsModuleV10.GDDR6_MR7.Hibernate);
			printf("\t\tLow Freq Mode: %d\n", Module->AsModuleV10.GDDR6_MR7.LowFreqMode);
			printf("\t\tAuto Sync: %d\n", Module->AsModuleV10.GDDR6_MR7.AutoSync);
			printf("\t\tHalf VREFC: %d\n", Module->AsModuleV10.GDDR6_MR7.HalfVREFC);
			printf("\t\tHalf VREFD: %d\n", Module->AsModuleV10.GDDR6_MR7.HalfVREFD);
			printf("\t\tVDD Range: %d\n", Module->AsModuleV10.GDDR6_MR7.VDDRange);
		}
		else if(VRAM_InfoCntRev == 5)
		{
			uint32_t Density = DecodeDensity(Module->AsModuleV11.Density);
			printf("\tMemory Vendor: %s\n", MemVendorNameStrs[Module->AsModuleV11.MemoryVendorRevisionID & 0xF]);
			printf("\tMemory Revision: %d\n", Module->AsModuleV11.MemoryVendorRevisionID >> 4);
			printf("\tPart Number: %s\n", Module->AsModuleV11.PNString);
			printf("\tMemory Size: %dMB\n", Module->AsModuleV11.MemorySize);
			printf("\tMemory Density: %dMx%d\n", Density >> 16, Density & 0xFFFF);
			printf("\tMemory Voltage: %d\n", Module->AsModuleV11.MemVoltage);
			printf("\tChannel Count: %d\n", Module->AsModuleV11.ChannelNum);
			printf("\tChannel Width: %d\n", Module->AsModuleV11.ChannelWidth);
			printf("\tRefresh Rate Factor: %d\n", Module->AsModuleV11.RefreshRateFactor);
			printf("\tBank grouping: %s\n", ((Module->AsModuleV11.VRAMFlags & 1) ? "Enabled" : "Disabled"));
			printf("\tGDDR6 MR10: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV11.GDDR6_MR10)));
			printf("\t\tVREFC Offset: %d\n", Module->AsModuleV11.GDDR6_MR10.VREFCOffset);
			printf("\t\tWCK0 Invert/Quad Shift: %d\n", Module->AsModuleV11.GDDR6_MR10.WCK0InvertQuadShift);
			printf("\t\tWCK1 Invert/Quad Shift: %d\n", Module->AsModuleV11.GDDR6_MR10.WCK1InvertQuadShift);
			printf("\t\tWCK2CK Offset: %d\n", Module->AsModuleV11.GDDR6_MR10.WCK2CK);
			printf("\t\tWCKRatio: %d\n", Module->AsModuleV11.GDDR6_MR10.WCKRatio);
			printf("\t\tWCK Termination: %d\n", Module->AsModuleV11.GDDR6_MR10.WCKTermination);
			printf("\tGDDR6 MR1: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV11.GDDR6_MR1)));
			printf("\t\tDrive Strength: %d\n", Module->AsModuleV11.GDDR6_MR1.DriveStrength);
			printf("\t\tData Termination: %d\n", Module->AsModuleV11.GDDR6_MR1.DataTermination);
			printf("\t\tPLL Range: %d\n", Module->AsModuleV11.GDDR6_MR1.PLLDLLRange);
			printf("\t\tCalibration Update: %d\n", Module->AsModuleV11.GDDR6_MR1.CalibrationUpdate);
			printf("\t\tPLL Enable: %d\n", Module->AsModuleV11.GDDR6_MR1.PLLDLLEnable);
			printf("\t\tRead DBI: %d\n", Module->AsModuleV11.GDDR6_MR1.ReadDBI);
			printf("\t\tWrite DBI: %d\n", Module->AsModuleV11.GDDR6_MR1.WriteDBI);
			printf("\t\tCABI: %d\n", Module->AsModuleV11.GDDR6_MR1.CABI);
			printf("\t\tPLL Reset: %d\n", Module->AsModuleV11.GDDR6_MR1.PLLReset);
			printf("\tGDDR6 MR2: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV11.GDDR6_MR2)));
			printf("\t\tOCD Pulldown Driver Offset: %d\n", Module->AsModuleV11.GDDR6_MR2.OCDPulldownDriverOff);
			printf("\t\tOCD Pullup Driver Offset: %d\n", Module->AsModuleV11.GDDR6_MR2.OCDPullupDriverOff);
			printf("\t\tSelfRefresh: %d\n", Module->AsModuleV11.GDDR6_MR2.SelfRefresh);
			printf("\t\tEDC Mode: %d\n", Module->AsModuleV11.GDDR6_MR2.EDCMode);
			printf("\t\tRDQS: %d\n", Module->AsModuleV11.GDDR6_MR2.RDQS);
			printf("\t\tCADT SRF: %d\n", Module->AsModuleV11.GDDR6_MR2.CADTSRF);
			printf("\t\tEDC HR: %d\n", Module->AsModuleV11.GDDR6_MR2.EDCHR);
			printf("\tGDDR6 MR4: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV11.GDDR6_MR4)));
			printf("\t\tEDC Hold Pattern: %d\n", Module->AsModuleV11.GDDR6_MR4.EDCHoldPattern);
			printf("\t\tCRCWL: %d\n", Module->AsModuleV11.GDDR6_MR4.CRCWL);
			printf("\t\tCRCRL: %d\n", Module->AsModuleV11.GDDR6_MR4.CRCRL);
			printf("\t\tRDCRC: %d\n", Module->AsModuleV11.GDDR6_MR4.RDCRC);
			printf("\t\tWRCRC: %d\n", Module->AsModuleV11.GDDR6_MR4.WRCRC);
			printf("\t\tEDC Inv: %d\n", Module->AsModuleV11.GDDR6_MR4.EDCInv);
			printf("\tGDDR6 MR7: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV11.GDDR6_MR7)));
			printf("\t\tWCK Alignment Point: %d\n", Module->AsModuleV11.GDDR6_MR7.WCKAlignPt);
			printf("\t\tHibernate: %d\n", Module->AsModuleV11.GDDR6_MR7.Hibernate);
			printf("\t\tLow Freq Mode: %d\n", Module->AsModuleV11.GDDR6_MR7.LowFreqMode);
			printf("\t\tAuto Sync: %d\n", Module->AsModuleV11.GDDR6_MR7.AutoSync);
			printf("\t\tHalf VREFC: %d\n", Module->AsModuleV11.GDDR6_MR7.HalfVREFC);
			printf("\t\tHalf VREFD: %d\n", Module->AsModuleV11.GDDR6_MR7.HalfVREFD);
			printf("\t\tVDD Range: %d\n", Module->AsModuleV11.GDDR6_MR7.VDDRange);
			printf("\tGDDR6 MR8: 0x%04X\n", *((uint16_t *)(&Module->AsModuleV11.GDDR6_MR8)));
			printf("\t\tCALTerm: %d\n", Module->AsModuleV11.GDDR6_MR8.CALTerm);
			printf("\t\tCAHTerm: %d\n", Module->AsModuleV11.GDDR6_MR8.CAHTerm);
			printf("\t\tCATO: %d\n", Module->AsModuleV11.GDDR6_MR8.CATO);
			printf("\t\tEDC High Z: %d\n", Module->AsModuleV11.GDDR6_MR8.EDCHighZ);
			printf("\t\tCKAC: %d\n", Module->AsModuleV11.GDDR6_MR8.CKAC);
			printf("\t\tREFPB: %d\n", Module->AsModuleV11.GDDR6_MR8.REFPB);
			printf("\t\tRLEHF: %d\n", Module->AsModuleV11.GDDR6_MR8.RLEHF);
			printf("\t\tWREHF: %d\n", Module->AsModuleV11.GDDR6_MR8.WREHF);
			printf("\t\tCKTerm: %d\n", Module->AsModuleV11.GDDR6_MR8.CKTerm);
		}
		else
		{
			printf("Unknown VRAM_Info table revision!\n");
		}
	}
	else
	{
		printf("Unknown VRAM_Info table revision!\n");
	}
}

#define NEXT_ARG_CHECK(arg) do { if(i == (argc - 1)) { printf("Argument \"%s\" requires a parameter.\n", arg); if(VBIOSFile) fclose(VBIOSFile); return(-1); } } while(0)

int main(int argc, char **argv)
{
	size_t BytesRead;
	FILE *VBIOSFile = NULL;
	json_t *ReqTimingsJSON;
	uint32_t MemClk = 0, Module = 0, ReqTimingsLen = 0;
	uint8_t *VBIOSImage, VBIOSFileNameIdx = 0, ReqTimingsIdx = 0;
	
	// This is used for both Navi1x and Polaris, so we size
	// it to fit the larger of the two.
	uint8_t RequestedTimings[AMD_NAVI1X_INPUT_TIMING_HEX_SIZE];
	
	// This is used for Navi2x timings.
	Navi21TimingFmt ReqTimingsNavi21;
	bool Writing = false, Show = false, Verbose = false, Found = false;
	bool TestFuncs = false;
	
	fprintf(stderr, "amdtimingcopy by Wolf9466 (aka Wolf0/OhGodAPet)\n");
	fprintf(stderr, "Donation address (BTC): 1WoLFumNUvjCgaCyjFzvFrbGfDddYrKNR\n");
	
	if(argc < 4 || argc > 10) usage(argv[0]);

	for(int i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file"))
		{
			NEXT_ARG_CHECK(argv[i]);
			
			VBIOSFile = fopen(argv[++i], "rb+");
			VBIOSFileNameIdx = i;
			
			if(!VBIOSFile)
			{
				printf("Unable to open %s (does it exist?)\n", argv[i]);
				return(-2);
			}
		}
		else if(!strcmp(argv[i], "-m") || !strcmp(argv[i], "--module"))
		{
			NEXT_ARG_CHECK(argv[i]);

			Module = strtoul(argv[++i], NULL, 10);
		}
		else if(!strcmp(argv[i], "-r") | !strcmp(argv[i], "--read"))
		{
			NEXT_ARG_CHECK(argv[i]);

			if(MemClk)
			{
				printf("Read and write parameters are mutually exclusive.\n");
				if(VBIOSFile) fclose(VBIOSFile);
				return(-1);
			}

			if(Show)
			{
				printf("Show Info parameter cannot be used with read or write parameters.\n");
				if(VBIOSFile) fclose(VBIOSFile);
				return(-1);
			}
			MemClk = strtoul(argv[++i], NULL, 10);
		}
		else if(!strcmp(argv[i], "-w") | !strcmp(argv[i], "--write"))
		{	
			if((argc - i) < 2)
			{
				printf("Argument \"%s\" requires two parameters.\n", argv[i]);
				if(VBIOSFile) fclose(VBIOSFile);
				return(-1);
			}

			if(MemClk)
			{
				printf("Read and write parameters are mutually exclusive.\n");
				if(VBIOSFile) fclose(VBIOSFile);
				return(-1);
			}

			if(Show)
			{
				printf("Show Info parameter cannot be used with read or write parameters.\n");
				if(VBIOSFile) fclose(VBIOSFile);
				return(-1);
			}

			MemClk = strtol(argv[++i], NULL, 10);
			
			// It would be nice to parse the timings right here
			// and now, but we have not yet examined the ROM, and
			// as such we do not yet know what format to expect.
			ReqTimingsIdx = ++i;

			Writing = true;
		}
		else if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--show"))
		{
			if(MemClk)
			{
				printf("Show Info parameter cannot be used with read or write parameters.\n");
				if(VBIOSFile) fclose(VBIOSFile);
				return(-1);
			}

			Show = true;
		}
		else if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
		{
			Verbose = true;
		}
		else if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--test"))
		{
			TestFuncs = true;
		}
		else
		{
			printf("Unknown parameter \"%s\".\n", argv[i]);
			if(VBIOSFile) fclose(VBIOSFile);
			usage(argv[0]);
		}
	}

	if(!VBIOSFile)
	{
		printf("No VBIOS file specified.\n");
		return(1);
	}
	
	//MemClk *= 100;
	VBIOSImage = (uint8_t *)malloc(sizeof(uint8_t) * 0x80000);
	BytesRead = fread(VBIOSImage, sizeof(uint8_t), 0x80000, VBIOSFile);

	/*if((BytesRead != 0x10000) && (BytesRead != 0x20000) && (BytesRead != 0x40000) && (BytesRead != 0x80000))
	{
		printf("Invalid VBIOS size.\n");
		fclose(VBIOSFile);
		return(1);
	}*/

	ATOM_ROM_HEADER_V2_2 *hdr = (ATOM_ROM_HEADER_V2_2 *)(VBIOSImage + ((uint16_t *)(VBIOSImage + OFFSET_TO_POINTER_TO_ATOM_ROM_HEADER))[0]);
	ATOM_MASTER_DATA_TABLE_LIST_V2_1 *DataTblList = ((ATOM_MASTER_DATA_TABLE_LIST_V2_1 *)(VBIOSImage + hdr->MasterDataTableOffset));

	void *VRInfoBase = (void *)(VBIOSImage + DataTblList->VRAM_Info);
	VRAM_INFO_HEADER_V2_2 *VRInfoHdr22 = (VRAM_INFO_HEADER_V2_2 *)(VBIOSImage + DataTblList->VRAM_Info);

	ATOM_INIT_REG_BLOCK *IRBlk;

	uint32_t MemClkPatchTblOffset;
	
	fprintf(stderr, "\nVRAM_Info table format revision %d, content revision %d\n", ((ATOM_TABLE_HEADER *)VRInfoBase)->TableFormatRevision, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision);

	// We were passed the -w/--write flag, and now we must read
	// and interpret JSON timings. Issue is... we do not know
	// what format to expect, as we do not know the GPU type yet.
	// However - the VBIOS file parameter is ALWAYS required, and
	// reading the version numbers of the VRAM_Info table will
	// tell us exactly what sort of data we need to interpret.
	if(Writing)
	{	
		// If we are dealing with Navi1x or newer, the timings
		// parameter inside argv[ReqTimingsIdx] is the filename
		// of a JSON formatted file with timings.
		if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision > 3)
		{
			FILE *JSONFile = fopen(argv[ReqTimingsIdx], "rb");
			if(!JSONFile)
			{
				printf("Failed to open file (does it exist?)\n");
				return(-1);
			}
			
			ReqTimingsJSON = json_loadf(JSONFile, JSON_DISABLE_EOF_CHECK, NULL);
			if(!ReqTimingsJSON)
			{
				printf("Error parsing JSON.\n");
				fclose(JSONFile);
				return(-1);
			}
		
			fclose(JSONFile);
		}
		else if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision < 4)
		{
			// Polaris is an exception to the JSON input/output,
			// because it is legacy functionality. Timings may
			// only be input in the form of a hex string. It
			// must be 96 hex digits (48 bytes) without exception.
			ASCIIHexToBinary(RequestedTimings, argv[ReqTimingsIdx], 96);
		}
		
		if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 5)
		{
			// Our Navi2x support is broken. Oops.
			printf("Navi2x VRAM_Info table detected. Support for this format is currently broken.\n");
			json_decref(ReqTimingsJSON);
			if(VBIOSFile) fclose(VBIOSFile);
			return(-1);
			
			//memset(&ReqTimingsNavi21, 0x00, sizeof(ReqTimingsNavi21));
			//LoadNavi2RegsFromJSON(&ReqTimingsNavi21, ReqTimingsJSON);
			//json_decref(ReqTimingsJSON);
		}
		else if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 4)
		{
			memset(RequestedTimings, 0x00, 28 * 4);
			LoadNaviRegsFromJSON(RequestedTimings, ReqTimingsJSON);
			json_decref(ReqTimingsJSON);
		}
		else
		{
			// If we get here, we have no idea what version the table is.
			// Let the user know and bail.
			printf("Unknown VRAM_Info table content revision %d detected. Unable to proceed.\n", ((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision);
			if(VBIOSFile) fclose(VBIOSFile);
			return(-1);
		}
	}

	if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 2)
	{
		ATOM_VRAM_MODULE_V8 *VRAMModules = (ATOM_VRAM_MODULE_V8 *)(((uint8_t *)VRInfoBase) + sizeof(VRAM_INFO_HEADER_V2_2));

		if(Module > (((VRAM_INFO_HEADER_V2_2 *)VRInfoBase)->NumberOfVRAMModules - 1))
		{
			printf("Specified module does not exist.\n");
			return(-3);
		}

		uint32_t VRAMModulesEnd = 0;

		for(int i = 0; i < ((VRAM_INFO_HEADER_V2_2 *)VRInfoBase)->NumberOfVRAMModules; ++i)
		{
			if(Show)
			{
				printf("VRAM Module %d (offset 0x%08X):\n", i, ((uint8_t *)VRAMModules) - ((uint8_t *)VRInfoBase));
				DumpVRAMModuleData(VRAMModules, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableFormatRevision, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision);
			}

			VRAMModulesEnd += VRAMModules->ModuleSize;
			VRAMModules = (ATOM_VRAM_MODULE_V8 *)(((uint8_t *)VRAMModules) + VRAMModules->ModuleSize);
		}

		MemClkPatchTblOffset = ((VRAM_INFO_HEADER_V2_2 *)VRInfoBase)->MemClkPatchTblOffset;
		if(Verbose)
		{
			VRAM_INFO_HEADER_V2_2 *vrhdr = (VRAM_INFO_HEADER_V2_2 *)VRInfoBase;
			printf("MemAdjTblOffset = 0x%08X\nMemClkPatchTblOffset = 0x%08X\nMCAdjustPerTileTblOffset = 0x%08X\nMCPhyInitTableOffset = 0x%08X\n\n", vrhdr->MemAdjTblOffset, vrhdr->MemClkPatchTblOffset, vrhdr->MCAdjustPerTileTblOffset, vrhdr->MCPhyInitTableOffset);
			ReadTimings(VRInfoBase);
			ReadPhyInit(VRInfoBase);
		}
	}
	else if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 3)
	{
		ATOM_VRAM_MODULE_V9 *VRAMModules = (ATOM_VRAM_MODULE_V9 *)(((uint8_t *)VRInfoBase) + sizeof(VRAM_INFO_HEADER_V2_3));

		if(Module > (((VRAM_INFO_HEADER_V2_3 *)VRInfoBase)->NumberOfVRAMModules - 1))
		{
			printf("Specified module does not exist.\n");
			return(-3);
		}

		for(int i = 0; i < ((VRAM_INFO_HEADER_V2_3 *)VRInfoBase)->NumberOfVRAMModules; ++i)
		{
			if(Show)
			{
				printf("VRAM Module %d (offset 0x%08X):\n", i, ((uint8_t *)VRAMModules) - ((uint8_t *)VRInfoBase));
				DumpVRAMModuleData(VRAMModules, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableFormatRevision, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision);
			}

			VRAMModules = (ATOM_VRAM_MODULE_V9 *)(((uint8_t *)VRAMModules) + VRAMModules->ModuleSize);
		}

		MemClkPatchTblOffset = ((VRAM_INFO_HEADER_V2_3 *)VRInfoBase)->MemClkPatchTblOffset;
		if(Verbose)
		{
			VRAM_INFO_HEADER_V2_3 *vrhdr = (VRAM_INFO_HEADER_V2_3 *)VRInfoBase;
			printf("MemAdjTblOffset = 0x%08X\nMemClkPatchTblOffset = 0x%08X\nMCAdjustPerTileTblOffset = 0x%08X\nMCPhyInitTableOffset = 0x%08X\n\n", vrhdr->MemAdjTblOffset, vrhdr->MemClkPatchTblOffset, vrhdr->MCAdjustPerTileTblOffset, vrhdr->MCPhyInitTableOffset);
			ReadTimings2_3(VRInfoBase);
			if(Verbose) ReadMemAdj2_4(VRInfoBase, TestFuncs);
		}
	}
	else if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 4)
	{
		ATOM_VRAM_MODULE_V10 *VRAMModules = (ATOM_VRAM_MODULE_V10 *)(((uint8_t *)VRInfoBase) + sizeof(VRAM_INFO_HEADER_V2_4));

		if(Module > (((VRAM_INFO_HEADER_V2_4 *)VRInfoBase)->NumberOfVRAMModules - 1))
		{
			printf("Specified module does not exist.\n");
			return(-3);
		}

		for(int i = 0; i < ((VRAM_INFO_HEADER_V2_4 *)VRInfoBase)->NumberOfVRAMModules; ++i)
		{
			if(Show)
			{
				printf("VRAM Module %d (offset 0x%08X):\n", i, ((uint8_t *)VRAMModules) - ((uint8_t *)VRInfoBase));
				DumpVRAMModuleData(VRAMModules, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableFormatRevision, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision);
			}

			if(TestFuncs)
			{
				if((((VRAM_INFO_HEADER_V2_4 *)VRInfoBase)->NumberOfVRAMModules != 1) || (i != 0))
				printf("Changing PLLRange to 2 and enabling bank grouping...\n");
				//VRAMModules->RefreshRateFactor = 9;
				VRAMModules->VRAMFlags |= 1;
				//VRAMModules->GDDR6_MR1.PLLDLLRange = 2;
			}
			VRAMModules = (ATOM_VRAM_MODULE_V10 *)(((uint8_t *)VRAMModules) + VRAMModules->ModuleSize);
			
		}

		MemClkPatchTblOffset = ((VRAM_INFO_HEADER_V2_4 *)VRInfoBase)->MemClkPatchTblOffset;
		if(Verbose) ReadTimings2_4(VRInfoBase);
		if(Verbose) ReadPhyInit2_4(VRInfoBase);
		if(Verbose) ReadMemAdj2_4(VRInfoBase, TestFuncs);
	}
	else if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 5)
	{
		ATOM_VRAM_MODULE_V11 *VRAMModules = (ATOM_VRAM_MODULE_V11 *)(((uint8_t *)VRInfoBase) + sizeof(VRAM_INFO_HEADER_V2_5));

		if(Module > (((VRAM_INFO_HEADER_V2_5 *)VRInfoBase)->NumberOfVRAMModules - 1))
		{
			printf("Specified module does not exist.\n");
			return(-3);
		}

		for(int i = 0; i < ((VRAM_INFO_HEADER_V2_5 *)VRInfoBase)->NumberOfVRAMModules; ++i)
		{
			if(Show)
			{
				printf("VRAM Module %d (offset 0x%08X):\n", i, ((uint8_t *)VRAMModules) - ((uint8_t *)VRInfoBase));
				DumpVRAMModuleData(VRAMModules, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableFormatRevision, ((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision);
			}

			if(TestFuncs)
			{
				//if((((VRAM_INFO_HEADER_V2_5 *)VRInfoBase)->NumberOfVRAMModules != 1) || (i != 0))
				//printf("Changing PLLRange to 2 and enabling bank grouping...\n");
				//VRAMModules->RefreshRateFactor = 9;
				//VRAMModules->VRAMFlags |= 1;
				//VRAMModules->GDDR6_MR1.PLLDLLRange = 2;
			}
			VRAMModules = (ATOM_VRAM_MODULE_V11 *)(((uint8_t *)VRAMModules) + VRAMModules->ModuleSize);
			
		}

	}

	if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 2)
	{
		IRBlk = (ATOM_INIT_REG_BLOCK *)(((uint8_t *)VRInfoBase) + MemClkPatchTblOffset);

		uint32_t IRIdxEntries = IRBlk->RegIndexTblSize / sizeof(ATOM_INIT_REG_INDEX_FORMAT);

		uint8_t *MemSettingDataBlkBase = (uint8_t *)(((uint8_t *)VRInfoBase) + MemClkPatchTblOffset + sizeof(ATOM_INIT_REG_BLOCK) + (sizeof(ATOM_INIT_REG_INDEX_FORMAT) * IRIdxEntries));
		uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;

		// TODO/FIXME: Ensure the FIRST entry is valid!

		uint32_t CurModule = ((ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk)->MemoryID.slAccess.MemBlkID;

		for(int Blk = 0; *((uint32_t *)CurrentMemSettingDataBlk) != 0x00000000UL; CurrentMemSettingDataBlk += IRBlk->RegDataBlkSize, Blk++)
		{
			uint32_t MemClkRange = ((ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk)->MemoryID.slAccess.MemClkRange / 100;

			if(!Blk || (((ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk)->MemoryID.slAccess.MemBlkID != CurModule))
			{
				CurModule = ((ATOM_MEMORY_SETTING_DATA_BLOCK *)CurrentMemSettingDataBlk)->MemoryID.slAccess.MemBlkID;
				if(Show) printf("\nModule %d:\n", CurModule);
			}

			if(Show)
			{
				printf("\n\t%u:\t", MemClkRange);

				// Don't print leading zeroes
				printf("%X", __builtin_bswap32(((uint32_t *)CurrentMemSettingDataBlk)[1]));

				for(int i = 2; i < (IRBlk->RegDataBlkSize >> 2); ++i)
					printf("%08X", __builtin_bswap32(((uint32_t *)CurrentMemSettingDataBlk)[i]));

				putchar('\n');
			}
			else if(MemClk == MemClkRange && Module == CurModule)
			{
				// We found the requested memclk range in the requested module
				Found = true;
				uint32_t IRIdxEntries = (IRBlk->RegDataBlkSize >> 2);

				if(!Writing)
				{
					// Don't print leading zeroes
					printf("%X", __builtin_bswap32(((uint32_t *)CurrentMemSettingDataBlk)[1]));

					for(int i = 2; i < (IRBlk->RegDataBlkSize >> 2); ++i)
						printf("%08X", __builtin_bswap32(((uint32_t *)CurrentMemSettingDataBlk)[i]));
				}
				else
				{
					//if(ReqTimingsLen != (IRIdxEntries * sizeof(uint32_t)))
					//{
					//	printf("Incorrect length of timing data. Unable to write.\n");
					//	return(-4);
					//}

					memcpy(CurrentMemSettingDataBlk + sizeof(uint32_t), RequestedTimings, IRBlk->RegDataBlkSize - 4);
				}
			}
		}
	}
	// Navi1x
	else if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 4)
	{
		VRAM_INFO_HEADER_V2_4 *VRInfo = (VRAM_INFO_HEADER_V2_4 *)VRInfoBase;
		INIT_REG_BLOCK_VEGA *IRBlk = (INIT_REG_BLOCK_VEGA *)(((uint8_t *)VRInfoBase) + VRInfo->MemClkPatchTblOffset);

		uint32_t IRIdxEntries = IRBlk->RegIndexTblNum;

		uint8_t *MemSettingDataBlkBase = (uint8_t *)(((uint8_t *)VRInfoBase) + VRInfo->MemClkPatchTblOffset + sizeof(INIT_REG_BLOCK_VEGA) + (sizeof(INIT_REG_INDEX_FORMAT_VEGA) * IRIdxEntries));
		uint8_t *CurrentMemSettingDataBlk = MemSettingDataBlkBase;

		// TODO/FIXME: Ensure the FIRST entry is valid!

		uint32_t CurModule = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemBlkID;

		//for(int Blk = 0; *((uint32_t *)CurrentMemSettingDataBlk) != 0x00000000UL; , Blk++)
		if(Show) printf("\nModule %d:\n", 0UL);

		do
		{
			uint32_t MemClkRange = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemClkRange / 100;

			if(((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemBlkID != CurModule)
			{
				CurModule = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->BlockID.IDAccess.MemBlkID;
				if(Show) printf("\nModule %d:\n", CurModule);
			}

			if(Show)
			{
				printf("\n\t%u:\t", MemClkRange);

				// Don't print leading zeroes
				//printf("%X", __builtin_bswap32(((uint32_t *)CurrentMemSettingDataBlk)[1]));

				for(int i = 0; i < IRIdxEntries; ++i)
					printf("%08X", __builtin_bswap32(((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]));

				putchar('\n');
			}
			else if(MemClk == MemClkRange && Module == CurModule)
			{
				// We found the requested memclk range in the requested module
				Found = true;

				if(!Writing)
				{	
					uint32_t RegValues[28];
					json_t *JSONTimings;
					for(int i = 0; i < IRIdxEntries; ++i) RegValues[i] = ((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i];
					
					//for(int i = 0; i < IRIdxEntries; ++i)
					//	printf("%08X", __builtin_bswap32(((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data[i]));
					
					JSONTimings = DumpNaviRegsAsJSON(RegValues);
					printf("\n%s\n", json_dumps(JSONTimings, JSON_INDENT(4)));
					json_decref(JSONTimings);
				}
				else
				{
					//if(ReqTimingsLen != (IRIdxEntries * sizeof(uint32_t)))
					//{
					//	printf("Incorrect length of timing data. Unable to write.\n");
					//	return(-4);
					//}
					memcpy(((VEGA_DATA_BLOCK *)CurrentMemSettingDataBlk)->Data, RequestedTimings, IRIdxEntries * sizeof(uint32_t));
				}
			}
			CurrentMemSettingDataBlk += (sizeof(uint32_t) * (IRIdxEntries + 1));
		} while(((uint32_t *)CurrentMemSettingDataBlk)[0] != 0x00000000UL);
	}
	// Navi2x
	else if(((ATOM_TABLE_HEADER *)VRInfoBase)->TableContentRevision == 5)
	{
		VRAM_INFO_HEADER_V2_5 *VRInfo = (VRAM_INFO_HEADER_V2_5 *)VRInfoBase;
		Navi21TimingFmt *Timings = (Navi21TimingFmt *)(((uint8_t *)VRInfoBase) + VRInfo->GDDR6ACTimingOffset);
		
		uint32_t CurModule = Timings->u32umc_id_access.ModuleNum;
		if(Show) printf("\nModule %d:\n", Timings->u32umc_id_access.ModuleNum);
		
		for(; Timings->u32umc_id_access.MemClkRange; Timings++)
		{
			uint32_t CurMemClkRange = Timings->u32umc_id_access.MemClkRange / 100;
			
			// If module has changed...
			if(CurModule != Timings->u32umc_id_access.ModuleNum)
			{
				CurModule = Timings->u32umc_id_access.ModuleNum;
				if(Show) printf("\nModule %d:\n", CurModule);
			}
			
			if(Show)
			{
				printf("\n\t%u:\t", CurMemClkRange);
				
				// Note that there are no Polaris/Navi1x GPUs with
				// this table content revision.
				
				// Skip the first dword, that is the UMC metadata
				for(int i = 1; i < (sizeof(Navi21TimingFmt) >> 2); ++i)
					printf("%08X", __builtin_bswap32(((uint32_t *)(Timings))[i]));
			}

			if(MemClk == CurMemClkRange && Module == CurModule)
			{
				// We found the requested memclk range in the requested module
				Found = true;

				if(!Writing)
				{
					json_t *JSONTimings = DumpNavi2RegsAsJSON(Timings);

					printf("\n%s\n", json_dumps(JSONTimings, JSON_INDENT(4)));
					json_decref(JSONTimings);

					// Debug
					//printf("\nUMC ID: %d\n", Timings->u32umc_id_access);
				}
				else
				{
					// Skip the ID!
					memcpy(((uint32_t *)Timings) + 1, ((uint32_t *)&ReqTimingsNavi21) + 1, sizeof(Navi21TimingFmt) - sizeof(uint32_t));
				}
				
			}
			//Timings++;
		} 
	}

	putchar('\n');

	if(MemClk && !Found)
	{
		printf("Memory clock range not found in module %d.\n", Module);
		fclose(VBIOSFile);
		return(-3);
	}
		
	if(TestFuncs) Writing = true;

	if(Writing)
	{
		rewind(VBIOSFile);
		fwrite(VBIOSImage, sizeof(uint8_t), BytesRead, VBIOSFile);
		fprintf(stderr, "Modified VBIOS image; new image written to %s.\n", argv[VBIOSFileNameIdx]);
	}

	fclose(VBIOSFile);
	return(0);
}
