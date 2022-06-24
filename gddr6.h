// Copyright 2022 Wolf9466

#ifndef __GDDR6_H
#define __GDDR6_H

#include "vbios-tables.h"

// Since valid values begin at 4, and that represents
// a value of 9, we offset the input by adding 5 before
// we take care of the changes CLEHF causes.

// It is the responsibility of the caller to ensure
// the validity of the arguments. tCLVal must be a
// 4 bit value (0 - 15, inclusive), and CLEHF must
// only be one bit - 0 or 1. Any returned value
// resulting from invalid inputs is considered
// to be undefined.

#define TRANSLATEMR0CASVALUE(tCLVal, CLEHF)		(((((uint8_t)(tCLVal)) & 0x0F) + 5U) | ((((uint8_t)(CLEHF)) & 1) << 4U))
#define TRANSLATEMR0TWRVALUE(tWRVal, WREHF)		(((((uint8_t)(tWRVal)) & 0x0F) + 4U) | ((((uint8_t)(WREHF)) & 1) << 4U))


typedef struct _GDDR6_MODE_REG_0
{
	uint16_t WL : 3;
	uint16_t tCL : 4;
	uint16_t TestMode : 1;
	uint16_t tWR : 4;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_0;

typedef struct _GDDR6_MODE_REG_1
{
	uint16_t DriveStrength : 2;
	uint16_t DataTermination : 2;
	uint16_t PLLDLLRange : 2;
	uint16_t CalibrationUpdate : 1;
	uint16_t PLLDLLEnable : 1;
	uint16_t ReadDBI : 1;
	uint16_t WriteDBI : 1;
	uint16_t CABI : 1;
	uint16_t PLLReset : 1;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_1;

typedef struct _GDDR6_MODE_REG_2
{
	uint16_t OCDPulldownDriverOff : 3;
	uint16_t OCDPullupDriverOff : 3;
	uint16_t SelfRefresh : 2;
	uint16_t EDCMode : 1;
	uint16_t RDQS : 1;
	uint16_t CADTSRF : 1;
	uint16_t EDCHR : 1;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_2;

typedef struct _GDDR6_MODE_REG_3
{
	uint16_t DataWCKTermOffset : 3;
	uint16_t CATermOffset : 3;
	uint16_t DRAMInfo : 2;
	uint16_t WRScaling : 2;
	uint16_t BankGroups : 2;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_3;

typedef struct _GDDR6_MODE_REG_4
{
	uint16_t EDCHoldPattern : 4;
	uint16_t CRCWL : 3;
	uint16_t CRCRL : 2;
	uint16_t RDCRC : 1;
	uint16_t WRCRC : 1;
	uint16_t EDCInv : 1;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_4;

typedef struct _GDDR6_MODE_REG_5
{
	uint16_t LP1 : 1;
	uint16_t LP2 : 1;
	uint16_t LP3 : 1;
	uint16_t PLLDLLBW : 3;
	uint16_t RAS : 6;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_5;

typedef struct _GDDR6_MODE_REG_7
{
	uint16_t WCKAlignPt : 1;
	uint16_t Hibernate : 1;
	uint16_t Reserved0 : 1;
	uint16_t LowFreqMode : 1;
	uint16_t AutoSync : 1;
	uint16_t Reserved1 : 1;
	uint16_t HalfVREFC : 1;
	uint16_t HalfVREFD : 1;
	uint16_t VDDRange : 2;
	uint16_t Reserved2 : 2;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_7;

typedef struct _GDDR6_MODE_REG_8
{
	uint16_t CALTerm : 2;
	uint16_t CAHTerm : 2;
	uint16_t CATO : 1;
	uint16_t EDCHighZ : 1;
	uint16_t CKAC : 1;
	uint16_t REFPB : 1;
	uint16_t RLEHF : 1;
	uint16_t WREHF : 1;
	uint16_t CKTerm : 2;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_8;

typedef struct _GDDR6_MODE_REG_10
{
	uint16_t VREFCOffset : 4;
	uint16_t WCK0InvertQuadShift : 2;
	uint16_t WCK1InvertQuadShift : 2;
	uint16_t WCK2CK : 1;
	uint16_t WCKRatio : 1;
	uint16_t WCKTermination  : 1;
	uint16_t ModeRegID : 4;
} GDDR6_MODE_REG_10;

#endif
