#ifndef _NAVIMEMREGS_H
#define _NAVIMEMREGS_H

#include <stdint.h>
#include <assert.h>

#pragma pack(push, 1)

// DramBaseAddress0

// Burst length (ASIC) == 1 << ((UMC_CONFIG & 3) + 1)

// Timings are somewhat self-descriptive relating to DRAM.
// Top two state regs should be 0x55204 & 0x56204.
// WARNING: Modification of tCL without changing the Mode Register is stupid.
typedef struct _Navi10DRAMTiming1
{
	uint32_t tCL : 6;
	uint32_t Pad0 : 2;
	uint32_t tRAS : 7;
	uint32_t Pad1 : 1;
	uint32_t tRCDRD : 6;
	uint32_t Pad2 : 2;
	uint32_t tRCDWR : 6;
	uint32_t Pad3 : 2;
} Navi10DRAMTiming1;

// What they say on the tin. Top two state regs should be 0x55208 & 0x56208.
// Remember tRC_S = tRP_S + tRAS, and tRC_L = tRP_L + tRAS.
typedef struct _Navi10DRAMTiming2
{
	uint32_t tRC_S : 8;		// tRCAB
	uint32_t tRC_L : 8;		// tRCPB
	uint32_t tRP_S : 6;		// tRPAB
	uint32_t Pad0 : 2;
	uint32_t tRP_L : 6;		// tRPPB
	uint32_t Pad1 : 2;
} Navi10DRAMTiming2;

// Same shit, different toilet. Top two state regs should be 0x5520C & 0x5620C
typedef struct _Navi10DRAMTiming3
{
	uint32_t tRRDS : 5;
	uint32_t Pad0 : 3;
	uint32_t tRRDL : 5;
	uint32_t Pad1 : 11;
	uint32_t tRTP : 5;
	uint32_t Pad2 : 3;
} Navi10DRAMTiming3;

typedef struct _Navi10DRAMTiming4
{
	uint32_t tFAW : 7;
	uint32_t Pad0 : 1;
	uint32_t t32AW : 9;
	uint32_t Pad1 : 15;
} Navi10DRAMTiming4;

// Top two state regs should be 0x55214 & 0x56214
// POSSIBLY CN CRITICAL! Write-to-Read shit. tWTR{S,L} == tW2R
typedef struct _Navi10DRAMTiming5
{
	uint32_t tWL : 6;
	uint32_t Pad0 : 2;
	uint32_t tWTRS : 5;
	uint32_t Pad1 : 3;
	uint32_t tWTRL : 7;
	uint32_t Pad2 : 7;
} Navi10DRAMTiming5;

// Top two state regs should be 0x55218 & 0x56218
typedef struct _Navi10DRAMTiming6
{
	uint32_t tWR : 7;
	uint32_t Pad0 : 25;
} Navi10DRAMTiming6;

// Top two state regs should be 0x5521C & 0x5621C
typedef struct _Navi10DRAMTiming7
{
	uint32_t tPPD : 3;
	uint32_t Pad0 : 1;
	uint32_t tCRCRL : 3;
	uint32_t Pad1 : 1;
	uint32_t tRREFD : 6;
	uint32_t Pad2 : 1;
	uint32_t tCRCWL : 5;
	uint32_t tRCPAGE : 12;
} Navi10DRAMTiming7;

// All values are to be taken as the real value minus ((BL/2) - 1).
// Or, since BL is fixed for us, just minus 7. To compensate, add 7.
// tRD_RDDD and tRD_RD_BAN are exceptions to the above.
// Top two state regs: 0x55224 & 0x56224
typedef struct _Navi10DRAMTiming8
{
	uint32_t tRD_RDDD : 4;
	uint32_t Pad0 : 4;
	uint32_t tRD_RDSD : 4;
	uint32_t Pad1 : 4;
	uint32_t tRD_RDSC : 4;
	uint32_t Pad2 : 4;
	uint32_t tRD_RDSCL : 4;
	uint32_t Pad3 : 2;
	uint32_t tRD_RD_BAN : 2;
} Navi10DRAMTiming8;

// All values are to be taken as the real value minus ((BL/2) - 1).
// As earlier, they are effectively minus 7. To compensate, add 7.
// tWR_RDDD and tWD_WD_BAN are exceptions to the above.
// Top two state regs: 0x55228 & 0x56228
typedef struct _Navi10DRAMTiming9
{
	uint32_t tWR_WRMW : 5;
	uint32_t Pad0 : 11;
	uint32_t tWR_WRSC : 4;
	uint32_t Pad1 : 4;
	uint32_t tWR_WRSCL : 6;
	uint32_t tWR_WR_BAN : 2;
} Navi10DRAMTiming9;

// 0x55228 & 0x56228 - stock is 0x00001301
// Default is 0x00001301 (tWRRD == 1; tRDWR == 0x13 (19))
// May be important to CN - tRDWR is the tR2W.
typedef struct _Navi10DRAMTiming10
{
	uint32_t tWRRD : 4;
	uint32_t Pad0 : 4;
	uint32_t tRDWR : 6;
	uint32_t Pad1 : 2;
	uint32_t RDRspDelay : 6;
	uint32_t tRefttAdj : 7;
	uint32_t Pad2 : 3;
} Navi10DRAMTiming10;

// There is no DramTiming11. <-- True story

typedef struct _Navi10DRAMTiming12
{
	uint32_t tREF : 16;
	uint32_t Pad0 : 16;
} Navi10DRAMTiming12;

// Top two state regs: 0x55234 & 0x56234
typedef struct _Navi10DRAMTiming13
{
	uint32_t tMRD : 6;
	uint32_t Pad0 : 2;
	uint32_t tMOD : 6;
	uint32_t Pad1 : 18;
} Navi10DRAMTiming13;

// Top two state regs: 0x55238 & 0x56238
typedef struct _Navi10DRAMTiming14
{
	uint32_t tXS : 11;
	uint32_t Pad0 : 5;
	uint32_t tDLLK : 11;
} Navi10DRAMTiming14;

typedef struct _Navi10DRAMTiming15
{
	uint32_t AlertCrcDly : 7;
	uint32_t Pad0 : 1;
	uint32_t AlertParDly : 7;
	uint32_t PL : 4;			// AKA CMDPARLATENCY - it defined the command parity latency.
	uint32_t Pad1 : 4;
	uint32_t RankBusyDly : 7;
	uint32_t Pad2 : 1;
} Navi10DRAMTiming15;

// Top two state regs: 0x55240 & 0x56240
typedef struct _Navi10DRAMTiming16
{
	uint32_t tXSMRS : 11;
	uint32_t Pad0 : 21;
} Navi10DRAMTiming16;

// Top two state regs: 0x55244 & 0x56244
typedef struct _Navi10DRAMTiming17
{
	uint32_t tPD : 5;
	uint32_t tCKSRE : 6;
	uint32_t tCKSRX : 6;
	uint32_t PwrDownDly : 8;
	uint32_t AggPwrDownDly : 6;
	uint32_t Pad0 : 1;
} Navi10DRAMTiming17;

// DramTiming18 & DramTiming19 do not exist.

// Top two state regs: 0x55250 & 0x56250
typedef struct _Navi10DRAMTiming20
{
	uint32_t tRFCSB : 11;
	uint32_t Pad0 : 5;
	uint32_t tSTAG : 8;
	uint32_t Pad1 : 8;
} Navi10DRAMTiming20;

// Top two state regs: 0x55254 & 0x56254
typedef struct _Navi10DRAMTiming21
{
	uint32_t tXP : 6;
	uint32_t Pad0 : 10;
	uint32_t tCPDED : 4;
	uint32_t Pad1 : 4;
	uint32_t tCKE : 5;
	uint32_t Pad2 : 3;
} Navi10DRAMTiming21;

// Top two state regs: 0x55258 & 0x56258
typedef struct _Navi10DRAMTiming22
{
	uint32_t tRDDATA_EN : 7;		// Must be set to CL - 1
	uint32_t Pad0 : 1;
	uint32_t tPHY_WRLAT : 5;		// Must be set to WL - 2
	uint32_t Pad1 : 3;
	uint32_t tPHY_RDLAT : 6;		// ??
	uint32_t Pad2 : 2;
	uint32_t tPHY_WRDATA : 3;		// ??
	uint32_t tPARIN_LAT : 2;
	uint32_t Pad3 : 2;
} Navi10DRAMTiming22;

typedef struct _Navi10DRAMTiming23
{
	uint32_t LpDly : 6;
	uint32_t Pad0 : 2;
	uint32_t LpExitDly : 6;
	uint32_t Pad1 : 2;
	uint32_t CKESTAGDLY : 4;
	uint32_t Pad2 : 12;
} Navi10DRAMTiming23;

typedef struct _Navi10DRAMTiming34
{
	uint32_t tPhyupd_resp : 4;
	uint32_t tRDEDC_EN : 7;
	uint32_t Pad0 : 1;
	uint32_t tWREDC_EN : 7;
	uint32_t Pad1 : 13;
} Navi10DRAMTiming34;

typedef struct _Navi10DRAMTiming35
{
	uint32_t ReceiverWait : 11;
	uint32_t CmdStageCnt : 11;
	uint32_t Pad0 : 2;
	uint32_t tWRMPR : 6;
	uint32_t Pad1 : 2;
} Navi10DRAMTiming35;

typedef struct _Navi10DRAMTiming36
{
	uint32_t tWTRTR : 6;
	uint32_t tREFTT : 6;
	uint32_t tTTROW : 6;
	uint32_t tLDTLD : 6;
	uint32_t tUPDN : 6;
	uint32_t tREFTT_MSB : 1;
	uint32_t Pad0 : 1;
} Navi10DRAMTiming36;

typedef struct _Navi10TRFCTimingCS01
{
	uint32_t tRFC : 11;
	uint32_t Pad0 : 21;
} Navi10TRFCTimingCS01;

typedef struct _Navi10ChanPipeDly
{
	uint32_t TXCtrlChanDly : 3;
	uint32_t Pad0 : 1;
	uint32_t TXDataChanDly : 3;
	uint32_t Pad1 : 1;
	uint32_t RXDataChanDly : 3;
	uint32_t Pad2 : 21;
} Navi10ChanPipeDly;

static_assert(sizeof(Navi10DRAMTiming1) == sizeof(uint32_t), "Register definition Navi10DRAMTiming1 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming2) == sizeof(uint32_t), "Register definition Navi10DRAMTiming2 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming3) == sizeof(uint32_t), "Register definition Navi10DRAMTiming3 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming4) == sizeof(uint32_t), "Register definition Navi10DRAMTiming4 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming5) == sizeof(uint32_t), "Register definition Navi10DRAMTiming5 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming6) == sizeof(uint32_t), "Register definition Navi10DRAMTiming6 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming7) == sizeof(uint32_t), "Register definition Navi10DRAMTiming7 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming8) == sizeof(uint32_t), "Register definition Navi10DRAMTiming8 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming9) == sizeof(uint32_t), "Register definition Navi10DRAMTiming9 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming10) == sizeof(uint32_t), "Register definition Navi10DRAMTiming10 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming12) == sizeof(uint32_t), "Register definition Navi10DRAMTiming12 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming13) == sizeof(uint32_t), "Register definition Navi10DRAMTiming13 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming14) == sizeof(uint32_t), "Register definition Navi10DRAMTiming14 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming15) == sizeof(uint32_t), "Register definition Navi10DRAMTiming15 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming16) == sizeof(uint32_t), "Register definition Navi10DRAMTiming16 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming17) == sizeof(uint32_t), "Register definition Navi10DRAMTiming17 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming20) == sizeof(uint32_t), "Register definition Navi10DRAMTiming20 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming21) == sizeof(uint32_t), "Register definition Navi10DRAMTiming21 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming22) == sizeof(uint32_t), "Register definition Navi10DRAMTiming22 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming23) == sizeof(uint32_t), "Register definition Navi10DRAMTiming23 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming34) == sizeof(uint32_t), "Register definition Navi10DRAMTiming34 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming35) == sizeof(uint32_t), "Register definition Navi10DRAMTiming35 is not 32 bits.");
static_assert(sizeof(Navi10DRAMTiming36) == sizeof(uint32_t), "Register definition Navi10DRAMTiming36 is not 32 bits.");
static_assert(sizeof(Navi10TRFCTimingCS01) == sizeof(uint32_t), "Register definition Navi10TRFCTimingCS01 is not 32 bits.");
static_assert(sizeof(Navi10ChanPipeDly) == sizeof(uint32_t), "Register definition Navi10ChanPipeDly is not 32 bits.");


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

// tCL == tCL + 5
// WR == WR - 1
typedef struct _Navi10UMCCTRL_MISC2
{
	GDDR6_MODE_REG_5 MR5;
	uint16_t Pad0;
} Navi10UMCCTRL_MISC2;

typedef struct _Navi10UMCCTRL_PMG_CMD_MRS
{
	GDDR6_MODE_REG_0 MR0;
	uint16_t Pad0;
} Navi10UMCCTRL_PMG_CMD_MRS;

typedef struct _Navi10UMCCTRL_PMG_CMD_MRS1
{
	GDDR6_MODE_REG_4 MR4;
	uint16_t Pad0;
} Navi10UMCCTRL_PMG_CMD_MRS1;

typedef struct _Navi10PMG_CMD
{
	GDDR6_MODE_REG_8 MR8;
	uint16_t Pad0;
} Navi10PMG_CMD;

typedef struct _Navi10TimingFmt
{
	uint32_t UMCCTRL_MISC2;
	uint32_t UMCCTRL_PMG_CMD_MRS;
	uint32_t UMCCTRL_PMG_CMD_EMRS;
	uint32_t UMCCTRL_PMG_CMD_MRS1;
	uint32_t PMG_CMD;
	Navi10DRAMTiming1 DramTiming1;
	Navi10DRAMTiming2 DramTiming2;
	Navi10DRAMTiming3 DramTiming3;
	Navi10DRAMTiming4 DramTiming4;
	Navi10DRAMTiming5 DramTiming5;
	Navi10DRAMTiming6 DramTiming6;
	Navi10DRAMTiming7 DramTiming7;
	Navi10DRAMTiming8 DramTiming8;
	Navi10DRAMTiming9 DramTiming9;
	Navi10DRAMTiming10 DramTiming10;
	Navi10DRAMTiming12 DramTiming12;
	Navi10DRAMTiming13 DramTiming13;
	Navi10DRAMTiming14 DramTiming14;
	Navi10DRAMTiming16 DramTiming16;
	Navi10DRAMTiming17 DramTiming17;
	Navi10DRAMTiming20 DramTiming20;
	Navi10DRAMTiming21 DramTiming21;
	Navi10DRAMTiming22 DramTiming22;
	Navi10DRAMTiming23 DramTiming23;
	Navi10DRAMTiming35 DramTiming35;
	Navi10DRAMTiming36 DramTiming36;
	Navi10TRFCTimingCS01 TRFC;
	uint32_t ChanPipeDly;
} Navi10TimingFmt;

#pragma pack(pop)

#endif
