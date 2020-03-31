#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>

#include "vram/navi.h"

uint32_t amd_bfe(uint32_t s0, uint32_t s1, uint32_t s2)
{
	uint32_t offset = s1 & 0x1F, width = s2 & 0x1F;
	
	if(!width) return(0);
	else if((offset + width) < 32) return((s0 << (32 - offset - width)) >> (32 - width));
	else return(s0 >> offset);
}

// String validity check is caller's responsibility
// Length checking is performed by the callee, though
// The offset MUST 31 or less, the bitsize MUST be 32 or less.
uint32_t ASCIIHexToDword(const char *asciistr, uint32_t offset, uint32_t bitsize)
{
	int len = strlen(asciistr);
	uint32_t num;
	
	if((bitsize > 32) || (offset > 31)) return(0UL);
	
	if(asciistr[0] == '0' && asciistr[1] == 'x')
	{
		asciistr += 2;
		len -= 2;
	}
	
	if(len > 8) return(0UL);
	
	num = strtoul(asciistr, NULL, 16);
	return(amd_bfe(num, offset, bitsize));
}

// String validity check is caller's responsibility
// Length checking is performed by the callee, though
// The offset MUST 31 or less, the bitsize MUST be 32 or less.
uint32_t ASCIIDecimalToDword(const char *asciistr, uint32_t offset, uint32_t bitsize)
{
	int len = strlen(asciistr);
	uint32_t num;
	
	if((bitsize > 32) || (offset > 31)) return(0UL);
	
	if(len > 16) return(0UL);
	
	num = strtoul(asciistr, NULL, 10);
	return(amd_bfe(num, offset, bitsize));
}

void DwordToASCIIHex(char *asciistr, uint32_t dword, uint8_t size)
{
	int pad = 2;
	asciistr[0] = '0';
	asciistr[1] = 'x';
	
	if(size < 4) pad += (4 - size) << 1;
	
	for(int i = 2; i < pad; ++i) asciistr[i] = '0';
	
	for(int i = 0, j = 8; i < size; ++i, j -= 2)
	{
		asciistr[j] = "0123456789ABCDEF"[((uint8_t *)&dword)[i] >> 4];
		asciistr[j + 1] = "0123456789ABCDEF"[((uint8_t *)&dword)[i] & 0x0F];
	}
	
	asciistr[10] = 0x00;
}

void DwordToASCIIDecimal(char *asciistr, uint32_t dword, uint8_t size)
{
	sprintf(asciistr, "%d", dword);
}

json_t *DumpNavi10UMCCTRL_MISC2AsJSON(const Navi10UMCCTRL_MISC2 *val)
{
	char tmpstrs[5][16];
	json_t *NewObj;
	
	DwordToASCIIDecimal(tmpstrs[0], val->MR5.LP1, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->MR5.LP2, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->MR5.LP3, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->MR5.PLLDLLBW, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->MR5.RAS, 1);
	
	NewObj = json_pack("{ssssssssss}", "LP1", tmpstrs[0], "LP2", tmpstrs[1], "LP3", tmpstrs[2], "PLLDLLBW", tmpstrs[3], "RAS", tmpstrs[4]);
	
	return(NewObj);
}

json_t *DumpNavi10UMCCTRL_PMG_CMD_MRSAsJSON(const Navi10UMCCTRL_PMG_CMD_MRS *val)
{
	char tmpstrs[4][16];
	json_t *NewObj;
	
	DwordToASCIIDecimal(tmpstrs[0], val->MR0.WL, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->MR0.tCL, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->MR0.TestMode, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->MR0.tWR, 1);
	
	NewObj = json_pack("{ssssssss}", "WL", tmpstrs[0], "tCL", tmpstrs[1], "TestMode", tmpstrs[2], "tWR", tmpstrs[3]);
	
	return(NewObj);
}

json_t *DumpNavi10UMCCTRL_PMG_CMD_MRS1AsJSON(const Navi10UMCCTRL_PMG_CMD_MRS1 *val)
{
	char tmpstrs[6][16];
	json_t *NewObj;
	
	DwordToASCIIDecimal(tmpstrs[0], val->MR4.EDCHoldPattern, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->MR4.CRCWL, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->MR4.CRCRL, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->MR4.RDCRC, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->MR4.WRCRC, 1);
	DwordToASCIIDecimal(tmpstrs[5], val->MR4.EDCInv, 1);
	
	NewObj = json_pack("{ssssssssssss}", "EDCHoldPattern", tmpstrs[0], "CRCWL", tmpstrs[1], "CRCRL", tmpstrs[2], "RDCRC", tmpstrs[3], "WRCRC", tmpstrs[4], "EDCInv", tmpstrs[5]);
	
	return(NewObj);
}

json_t *DumpNavi10PMG_CMDAsJSON(const Navi10PMG_CMD *val)
{
	char tmpstrs[9][16];
	json_t *NewObj;
	
	DwordToASCIIDecimal(tmpstrs[0], val->MR8.CALTerm, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->MR8.CAHTerm, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->MR8.CATO, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->MR8.EDCHighZ, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->MR8.CKAC, 1);
	DwordToASCIIDecimal(tmpstrs[5], val->MR8.REFPB, 1);
	DwordToASCIIDecimal(tmpstrs[6], val->MR8.RLEHF, 1);
	DwordToASCIIDecimal(tmpstrs[7], val->MR8.WREHF, 1);
	DwordToASCIIDecimal(tmpstrs[8], val->MR8.CKTerm, 1);
	
	NewObj = json_pack("{ssssssssssssssssss}", "CALTerm", tmpstrs[0], "CAHTerm", tmpstrs[1], "CATO", tmpstrs[2], "EDCHighZ", tmpstrs[3], "CKAC", tmpstrs[4], "REFPB", tmpstrs[5], \
		"RLEHF", tmpstrs[6], "WREHF", tmpstrs[7], "CKTerm", tmpstrs[8]);
	
	return(NewObj);
}

json_t *DumpDramTiming1AsJSON(const Navi10DRAMTiming1 *val)
{
	char tmpstrs[4][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tCL, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tRAS, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tRCDRD, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tRCDWR, 1);

	NewObj = json_pack("{ssssssss}", "tCL", tmpstrs[0], "tRAS", tmpstrs[1], "tRCDRD", tmpstrs[2], "tRCDWR", tmpstrs[3]);

	return(NewObj);
}

json_t *DumpDramTiming2AsJSON(const Navi10DRAMTiming2 *val)
{
	char tmpstrs[4][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tRC_S, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tRC_L, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tRP_S, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tRP_L, 1);

	NewObj = json_pack("{ssssssss}", "tRC_S", tmpstrs[0], "tRC_L", tmpstrs[1], "tRP_S", tmpstrs[2], "tRP_L", tmpstrs[3]);

	return(NewObj);
}

json_t *DumpDramTiming3AsJSON(const Navi10DRAMTiming3 *val)
{
	char tmpstrs[3][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tRRDS, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tRRDL, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tRTP, 1);

	NewObj = json_pack("{ssssss}", "tRRDS", tmpstrs[0], "tRRDL", tmpstrs[1], "tRTP", tmpstrs[2]);

	return(NewObj);
}

json_t *DumpDramTiming4AsJSON(const Navi10DRAMTiming4 *val)
{
	char tmpstrs[2][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tFAW, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->t32AW, 1);

	NewObj = json_pack("{ssss}", "tFAW", tmpstrs[0], "t32AW", tmpstrs[1]);
	
	return(NewObj);
}

json_t *DumpDramTiming5AsJSON(const Navi10DRAMTiming5 *val)
{
	char tmpstrs[3][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tWL, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tWTRS, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tWTRL, 1);
	
	NewObj = json_pack("{ssssss}", "tWL", tmpstrs[0], "tWTRS", tmpstrs[1], "tWTRL", tmpstrs[2]);

	return(NewObj);
}

json_t *DumpDramTiming6AsJSON(const Navi10DRAMTiming6 *val)
{
	char tmpstr[16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstr, val->tWR, 1);

	NewObj = json_pack("{ss}", "tWR", tmpstr);

	return(NewObj);
}

json_t *DumpDramTiming7AsJSON(const Navi10DRAMTiming7 *val)
{
	char tmpstrs[5][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tPPD, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tCRCRL, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tRREFD, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tCRCWL, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->tRCPAGE, 1);
	
	NewObj = json_pack("{ssssssssss}", "tPPD", tmpstrs[0], "tCRCRL", tmpstrs[1], "tRREFD", tmpstrs[2], "tCRCWL", tmpstrs[3], "tRCPAGE", tmpstrs[4]);

	return(NewObj);
}

json_t *DumpDramTiming8AsJSON(const Navi10DRAMTiming8 *val)
{
	char tmpstrs[5][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tRD_RDDD, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tRD_RDSD, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tRD_RDSC, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tRD_RDSCL, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->tRD_RD_BAN, 1);

	NewObj = json_pack("{ssssssssss}", "tRD_RDDD", tmpstrs[0], "tRD_RDSD", tmpstrs[1], "tRD_RDSC", tmpstrs[2], "tRD_RDSCL", tmpstrs[3], "tRD_RD_BAN", tmpstrs[4]);

	return(NewObj);
}

json_t *DumpDramTiming9AsJSON(const Navi10DRAMTiming9 *val)
{
	char tmpstrs[4][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tWR_WRMW, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tWR_WRSC, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tWR_WRSCL, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tWR_WR_BAN, 1);

	NewObj = json_pack("{ssssssss}", "tWR_WRMW", tmpstrs[0], "tWR_WRSC", tmpstrs[1], "tWR_WRSCL", tmpstrs[2], "tWR_WR_BAN", tmpstrs[3]);

	return(NewObj);
}

json_t *DumpDramTiming10AsJSON(const Navi10DRAMTiming10 *val)
{
	char tmpstrs[4][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tWRRD, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tRDWR, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->RDRspDelay, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tRefttAdj, 1);
	
	NewObj = json_pack("{ssssssss}", "tWRRD", tmpstrs[0], "tRDWR", tmpstrs[1], "RDRspDelay", tmpstrs[2], "tRefttAdj", tmpstrs[3]);

	return(NewObj);
}

json_t *DumpDramTiming12AsJSON(const Navi10DRAMTiming12 *val)
{
	char tmpstr[16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstr, val->tREF, 2);
		
	NewObj = json_pack("{ss}", "tREF", tmpstr);

	return(NewObj);
}

json_t *DumpDramTiming13AsJSON(const Navi10DRAMTiming13 *val)
{
	char tmpstrs[2][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tMRD, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tMOD, 1);
	
	NewObj = json_pack("{ssss}", "tMRD", tmpstrs[0], "tMOD", tmpstrs[1]);

	return(NewObj);
}

json_t *DumpDramTiming14AsJSON(const Navi10DRAMTiming14 *val)
{
	char tmpstrs[2][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tXS, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tDLLK, 2);

	NewObj = json_pack("{ssss}", "tXS", tmpstrs[0], "tDLLK", tmpstrs[1]);

	return(NewObj);
}

json_t *DumpDramTiming16AsJSON(const Navi10DRAMTiming16 *val)
{
	char tmpstr[16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstr, val->tXSMRS, 2);

	NewObj = json_pack("{ss}", "tXSMRS", tmpstr);

	return(NewObj);
}

json_t *DumpDramTiming17AsJSON(const Navi10DRAMTiming17 *val)
{
	char tmpstrs[5][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tPD, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tCKSRE, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tCKSRX, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->PwrDownDly, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->AggPwrDownDly, 1);

	NewObj = json_pack("{ssssssssss}", "tPD", tmpstrs[0], "tCKSRE", tmpstrs[1], "tCKSRX", tmpstrs[2], "PwrDownDly", tmpstrs[3], "AggPwrDownDly", tmpstrs[4]);

	return(NewObj);
}

json_t *DumpDramTiming20AsJSON(const Navi10DRAMTiming20 *val)
{
	char tmpstrs[2][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tRFCSB, 2);
	DwordToASCIIDecimal(tmpstrs[1], val->tSTAG, 1);
	
	NewObj = json_pack("{ssss}", "tRFCSB", tmpstrs[0], "tSTAG", tmpstrs[1]);

	return(NewObj);
}

json_t *DumpDramTiming21AsJSON(const Navi10DRAMTiming21 *val)
{
	char tmpstrs[3][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tXP, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tCPDED, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tCKE, 1);
	
	NewObj = json_pack("{ssssss}", "tXP", tmpstrs[0], "tCPDED", tmpstrs[1], "tCKE", tmpstrs[2]);

	return(NewObj);
}

json_t *DumpDramTiming22AsJSON(const Navi10DRAMTiming22 *val)
{
	char tmpstrs[5][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->tRDDATA_EN, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tPHY_WRLAT, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tPHY_RDLAT, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tPHY_WRDATA, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->tPARIN_LAT, 1);

	NewObj = json_pack("{ssssssssss}", "tRDDATA_EN", tmpstrs[0], "tPHY_WRLAT", tmpstrs[1], "tPHY_RDLAT", tmpstrs[2], "tPHY_WRDATA", tmpstrs[3], "tPARIN_LAT", tmpstrs[4]);

	return(NewObj);
}

json_t *DumpDramTiming23AsJSON(const Navi10DRAMTiming23 *val)
{
	char tmpstrs[3][16];
	json_t *NewObj;
	
	DwordToASCIIDecimal(tmpstrs[0], val->LpDly, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->LpExitDly, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->CKESTAGDLY, 1);

	NewObj = json_pack("{ssssss}", "LpDly", tmpstrs[0], "LpExitDly", tmpstrs[1], "CKESTAGDLY", tmpstrs[2]);

	return(NewObj);
}

json_t *DumpDramTiming35AsJSON(const Navi10DRAMTiming35 *val)
{
	char tmpstrs[3][16];
	json_t *NewObj;
	
	DwordToASCIIDecimal(tmpstrs[0], val->ReceiverWait, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->CmdStageCnt, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tWRMPR, 1);

	NewObj = json_pack("{ssssss}", "ReceiverWait", tmpstrs[0], "CmdStageCnt", tmpstrs[1], "tWRMPR", tmpstrs[2]);

	return(NewObj);
}

json_t *DumpDramTiming36AsJSON(const Navi10DRAMTiming36 *val)
{
	char tmpstrs[6][16];
	json_t *NewObj;
	
	DwordToASCIIDecimal(tmpstrs[0], val->tWTRTR, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->tREFTT, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->tTTROW, 1);
	DwordToASCIIDecimal(tmpstrs[3], val->tLDTLD, 1);
	DwordToASCIIDecimal(tmpstrs[4], val->tUPDN, 1);
	DwordToASCIIDecimal(tmpstrs[5], val->tREFTT_MSB, 1);
	
	NewObj = json_pack("{ssssssssssss}", "tWTRTR", tmpstrs[0], "tREFTT", tmpstrs[1], "tTTROW", tmpstrs[2], "tLDTLD", tmpstrs[3], "tUPDN", tmpstrs[4], "tREFTT_MSB", tmpstrs[5]);

	return(NewObj);
}

json_t *DumpTRFCTimingCS01AsJSON(const Navi10TRFCTimingCS01 *val)
{
	char tmpstr[16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstr, val->tRFC, 2);

	NewObj = json_pack("{ss}", "tRFC", tmpstr);

	return(NewObj);
}

json_t *DumpChanPipeDlyAsJSON(const Navi10ChanPipeDly *val)
{
	char tmpstrs[3][16];
	json_t *NewObj;

	DwordToASCIIDecimal(tmpstrs[0], val->TXCtrlChanDly, 1);
	DwordToASCIIDecimal(tmpstrs[1], val->TXDataChanDly, 1);
	DwordToASCIIDecimal(tmpstrs[2], val->RXDataChanDly, 1);
		
	NewObj = json_pack("{ssssss}", "TXCtrlChanDly", tmpstrs[0], "TXDataChanDly", tmpstrs[1], "RXDataChanDly", tmpstrs[2]);

	return(NewObj);
}

// RegTemplate is to be 26 register values, in order, as loaded in the function.
json_t *DumpNaviRegsAsJSON(const uint32_t *RegTemplate)
{
	json_t *DramTiming1JSON, *DramTiming2JSON, *DramTiming3JSON, *DramTiming4JSON, *DramTiming5JSON, *DramTiming6JSON, *DramTiming7JSON, *DramTiming8JSON;
	json_t *DramTiming9JSON, *DramTiming10JSON, *DramTiming12JSON, *DramTiming13JSON, *DramTiming14JSON, *DramTiming16JSON, *DramTiming17JSON, *DramTiming20JSON;
	json_t *DramTiming21JSON, *DramTiming22JSON, *DramTiming23JSON, *DramTiming35JSON, *DramTiming36JSON, *TRFCTimingCS01JSON, *ChanPipeDlyJSON, *CfgObj;
	json_t *UMCCTRL_MISC2JSON, *UMCCTRL_PMG_CMD_MRSJSON, *UMCCTRL_PMG_CMD_MRS1JSON, *PMG_CMDJSON;
	json_error_t err;
	
	UMCCTRL_MISC2JSON = DumpNavi10UMCCTRL_MISC2AsJSON((const Navi10UMCCTRL_MISC2 *)(RegTemplate + 0));
	UMCCTRL_PMG_CMD_MRSJSON = DumpNavi10UMCCTRL_PMG_CMD_MRSAsJSON((const Navi10UMCCTRL_PMG_CMD_MRS *)(RegTemplate + 1));
	// Note the always zeroed UMCCTRL_PMG_CMD_EMRS
	UMCCTRL_PMG_CMD_MRS1JSON = DumpNavi10UMCCTRL_PMG_CMD_MRS1AsJSON((const Navi10UMCCTRL_PMG_CMD_MRS1 *)(RegTemplate + 3));
	PMG_CMDJSON = DumpNavi10PMG_CMDAsJSON((const Navi10PMG_CMD *)(RegTemplate + 4));
	DramTiming1JSON = DumpDramTiming1AsJSON((const Navi10DRAMTiming1 *)(RegTemplate + 5));
	DramTiming2JSON = DumpDramTiming2AsJSON((const Navi10DRAMTiming2 *)(RegTemplate + 6));
	DramTiming3JSON = DumpDramTiming3AsJSON((const Navi10DRAMTiming3 *)(RegTemplate + 7));
	DramTiming4JSON = DumpDramTiming4AsJSON((const Navi10DRAMTiming4 *)(RegTemplate + 8));
	DramTiming5JSON = DumpDramTiming5AsJSON((const Navi10DRAMTiming5 *)(RegTemplate + 9));
	DramTiming6JSON = DumpDramTiming6AsJSON((const Navi10DRAMTiming6 *)(RegTemplate + 10));
	DramTiming7JSON = DumpDramTiming7AsJSON((const Navi10DRAMTiming7 *)(RegTemplate + 11));
	DramTiming8JSON = DumpDramTiming8AsJSON((const Navi10DRAMTiming8 *)(RegTemplate + 12));
	DramTiming9JSON = DumpDramTiming9AsJSON((const Navi10DRAMTiming9 *)(RegTemplate + 13));
	DramTiming10JSON = DumpDramTiming10AsJSON((const Navi10DRAMTiming10 *)(RegTemplate + 14));
	DramTiming12JSON = DumpDramTiming12AsJSON((const Navi10DRAMTiming12 *)(RegTemplate + 15));
	DramTiming13JSON = DumpDramTiming13AsJSON((const Navi10DRAMTiming13 *)(RegTemplate + 16));
	DramTiming14JSON = DumpDramTiming14AsJSON((const Navi10DRAMTiming14 *)(RegTemplate + 17));
	DramTiming16JSON = DumpDramTiming16AsJSON((const Navi10DRAMTiming16 *)(RegTemplate + 18));
	DramTiming17JSON = DumpDramTiming17AsJSON((const Navi10DRAMTiming17 *)(RegTemplate + 19));
	DramTiming20JSON = DumpDramTiming20AsJSON((const Navi10DRAMTiming20 *)(RegTemplate + 20));
	DramTiming21JSON = DumpDramTiming21AsJSON((const Navi10DRAMTiming21 *)(RegTemplate + 21));
	DramTiming22JSON = DumpDramTiming22AsJSON((const Navi10DRAMTiming22 *)(RegTemplate + 22));
	DramTiming23JSON = DumpDramTiming23AsJSON((const Navi10DRAMTiming23 *)(RegTemplate + 23));
	DramTiming35JSON = DumpDramTiming35AsJSON((const Navi10DRAMTiming35 *)(RegTemplate + 24));
	DramTiming36JSON = DumpDramTiming36AsJSON((const Navi10DRAMTiming36 *)(RegTemplate + 25));
	TRFCTimingCS01JSON = DumpTRFCTimingCS01AsJSON((const Navi10TRFCTimingCS01 *)(RegTemplate + 26));
	ChanPipeDlyJSON = DumpChanPipeDlyAsJSON((const Navi10ChanPipeDly *)(RegTemplate + 27));

	CfgObj = json_pack_ex(&err, 0, "{sososososososososososososososososososososososososososo}", "UMCCTRL_MISC2", UMCCTRL_MISC2JSON, "UMCCTRL_PMG_CMD_MRS", UMCCTRL_PMG_CMD_MRSJSON, \
		"UMCCTRL_PMG_CMD_MRS1", UMCCTRL_PMG_CMD_MRS1JSON, "PMG_CMD", PMG_CMDJSON, "DramTiming1", DramTiming1JSON, "DramTiming2", DramTiming2JSON, "DramTiming3", DramTiming3JSON, \
		"DramTiming4", DramTiming4JSON, "DramTiming5", DramTiming5JSON, "DramTiming6", DramTiming6JSON, "DramTiming7", DramTiming7JSON, "DramTiming8", DramTiming8JSON, \
		"DramTiming9", DramTiming9JSON, "DramTiming10", DramTiming10JSON, "DramTiming12", DramTiming12JSON, "DramTiming13", DramTiming13JSON, "DramTiming14", DramTiming14JSON, \
		"DramTiming16", DramTiming16JSON, "DramTiming17", DramTiming17JSON, "DramTiming20", DramTiming20JSON, "DramTiming21", DramTiming21JSON, \
		"DramTiming22", DramTiming22JSON, "DramTiming23", DramTiming23JSON, "DramTiming35", DramTiming35JSON, "DramTiming36", DramTiming36JSON, "TRFCTimingCS01", TRFCTimingCS01JSON, \
		"ChanPipeDly", ChanPipeDlyJSON);
	
	if(!CfgObj)
	{
		printf("Error: %s\n", err.text);
	}
	
	return(CfgObj);
}

bool LoadNavi10UMCCTRL_MISC2FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10UMCCTRL_MISC2 *ptr = (Navi10UMCCTRL_MISC2 *)val;
	char *tmpstrs[5];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssss}", "LP1", &tmpstrs[0], "LP2", &tmpstrs[1], "LP3", &tmpstrs[2], "PLLDLLBW", &tmpstrs[3], "RAS", &tmpstrs[4]);
	
	if(ret == -1) return(false);
	
	ptr->MR5.LP1 = strtoul(tmpstrs[0], NULL, 10);
	ptr->MR5.LP2 = strtoul(tmpstrs[1], NULL, 10);
	ptr->MR5.LP3 = strtoul(tmpstrs[2], NULL, 10);
	ptr->MR5.PLLDLLBW = strtoul(tmpstrs[3], NULL, 10);
	ptr->MR5.RAS = strtoul(tmpstrs[4], NULL, 10);
	
	return(true);
}

bool LoadNavi10UMCCTRL_PMG_CMD_MRSFromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10UMCCTRL_PMG_CMD_MRS *ptr = (Navi10UMCCTRL_PMG_CMD_MRS *)val;
	char *tmpstrs[4];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssss}", "WL", &tmpstrs[0], "tCL", &tmpstrs[1], "TestMode", &tmpstrs[2], "tWR", &tmpstrs[3]);
	
	if(ret == -1) return(false);
	
	ptr->MR0.WL = strtoul(tmpstrs[0], NULL, 10);
	ptr->MR0.tCL = strtoul(tmpstrs[1], NULL, 10);
	ptr->MR0.TestMode = strtoul(tmpstrs[2], NULL, 10);
	ptr->MR0.tWR = strtoul(tmpstrs[3], NULL, 10);
	
	return(true);
}

bool LoadNavi10UMCCTRL_PMG_CMD_MRS1FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10UMCCTRL_PMG_CMD_MRS1 *ptr = (Navi10UMCCTRL_PMG_CMD_MRS1 *)val;
	char *tmpstrs[6];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssssss}", "EDCHoldPattern", &tmpstrs[0], "CRCWL", &tmpstrs[1], "CRCRL", &tmpstrs[2], "RDCRC", &tmpstrs[3], "WRCRC", &tmpstrs[4], "EDCInv", &tmpstrs[5]);
	
	ptr->MR4.EDCHoldPattern = strtoul(tmpstrs[0], NULL, 10);
	ptr->MR4.CRCWL = strtoul(tmpstrs[1], NULL, 10);
	ptr->MR4.CRCRL = strtoul(tmpstrs[2], NULL, 10);
	ptr->MR4.RDCRC = strtoul(tmpstrs[3], NULL, 10);
	ptr->MR4.WRCRC = strtoul(tmpstrs[4], NULL, 10);
	ptr->MR4.EDCInv = strtoul(tmpstrs[5], NULL, 10);
	
	return(true);
}
	
bool LoadNavi10PMG_CMDFromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10PMG_CMD *ptr = (Navi10PMG_CMD *)val;
	char *tmpstrs[9];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssssssssssss}", "CALTerm", &tmpstrs[0], "CAHTerm", &tmpstrs[1], "CATO", &tmpstrs[2], "EDCHighZ", &tmpstrs[3], "CKAC", &tmpstrs[4], "REFPB", &tmpstrs[5], \
		"RLEHF", &tmpstrs[6], "WREHF", &tmpstrs[7], "CKTerm", &tmpstrs[8]);
	
	ptr->MR8.CALTerm = strtoul(tmpstrs[0], NULL, 10);
	ptr->MR8.CAHTerm = strtoul(tmpstrs[1], NULL, 10);
	ptr->MR8.CATO = strtoul(tmpstrs[2], NULL, 10);
	ptr->MR8.EDCHighZ = strtoul(tmpstrs[3], NULL, 10);
	ptr->MR8.CKAC = strtoul(tmpstrs[4], NULL, 10);
	ptr->MR8.REFPB = strtoul(tmpstrs[5], NULL, 10);
	ptr->MR8.RLEHF = strtoul(tmpstrs[6], NULL, 10);
	ptr->MR8.WREHF = strtoul(tmpstrs[7], NULL, 10);
	ptr->MR8.CKTerm = strtoul(tmpstrs[8], NULL, 10);
	
	return(true);
}

bool LoadDramTiming1FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming1 *ptr = (Navi10DRAMTiming1 *)val;
	char *tmpstrs[4];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssss}", "tCL", &tmpstrs[0], "tRAS", &tmpstrs[1], "tRCDRD", &tmpstrs[2], "tRCDWR", &tmpstrs[3]);

	if(ret == -1) return(false);
	
	ptr->tCL = strtoul(tmpstrs[0], NULL, 10);
	ptr->tRAS = strtoul(tmpstrs[1], NULL, 10);
	ptr->tRCDRD = strtoul(tmpstrs[2], NULL, 10);
	ptr->tRCDWR = strtoul(tmpstrs[3], NULL, 10);

	return(true);
}

bool LoadDramTiming2FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming2 *ptr = (Navi10DRAMTiming2 *)val;
	char *tmpstrs[4];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssss}", "tRC_S", &tmpstrs[0], "tRC_L", &tmpstrs[1], "tRP_S", &tmpstrs[2], "tRP_L", &tmpstrs[3]);

	if(ret == -1) return(false);
	
	ptr->tRC_S = strtoul(tmpstrs[0], NULL, 10);
	ptr->tRC_L = strtoul(tmpstrs[1], NULL, 10);
	ptr->tRP_S = strtoul(tmpstrs[2], NULL, 10);
	ptr->tRP_L = strtoul(tmpstrs[3], NULL, 10);

	return(true);
}

bool LoadDramTiming3FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming3 *ptr = (Navi10DRAMTiming3 *)val;
	char *tmpstrs[3];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssss}", "tRRDS", &tmpstrs[0], "tRRDL", &tmpstrs[1], "tRTP", &tmpstrs[2]);

	if(ret == -1) return(false);
	
	ptr->tRRDS = strtoul(tmpstrs[0], NULL, 10);
	ptr->tRRDL = strtoul(tmpstrs[1], NULL, 10);
	ptr->tRTP = strtoul(tmpstrs[2], NULL, 10);

	return(true);
}

bool LoadDramTiming4FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming4 *ptr = (Navi10DRAMTiming4 *)val;
	char *tmpstrs[2];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssss}", "tFAW", &tmpstrs[0], "t32AW", &tmpstrs[1]);

	if(ret == -1) return(false);
	
	ptr->tFAW = strtoul(tmpstrs[0], NULL, 10);
	ptr->t32AW = strtoul(tmpstrs[1], NULL, 10);

	return(true);
}

bool LoadDramTiming5FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming5 *ptr = (Navi10DRAMTiming5 *)val;
	char *tmpstrs[3];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssss}", "tWL", &tmpstrs[0], "tWTRS", &tmpstrs[1], "tWTRL", &tmpstrs[2]);

	if(ret == -1) return(false);
	
	ptr->tWL = strtoul(tmpstrs[0], NULL, 10);
	ptr->tWTRS = strtoul(tmpstrs[1], NULL, 10);
	ptr->tWTRL = strtoul(tmpstrs[2], NULL, 10);

	return(true);
}
	
bool LoadDramTiming6FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming6 *ptr = (Navi10DRAMTiming6 *)val;
	char *tmpstr;
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ss}", "tWR", &tmpstr);

	if(ret == -1) return(false);
	
	ptr->tWR = strtoul(tmpstr, NULL, 10);

	return(true);
}

bool LoadDramTiming7FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming7 *ptr = (Navi10DRAMTiming7 *)val;
	char *tmpstrs[5];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssss}", "tPPD", &tmpstrs[0], "tCRCRL", &tmpstrs[1], "tRREFD", &tmpstrs[2], "tCRCWL", &tmpstrs[3], "tRCPAGE", &tmpstrs[4]);

	if(ret == -1) return(false);
	
	ptr->tPPD = strtoul(tmpstrs[0], NULL, 10);
	ptr->tCRCRL = strtoul(tmpstrs[1], NULL, 10);
	ptr->tRREFD = strtoul(tmpstrs[2], NULL, 10);
	ptr->tCRCWL = strtoul(tmpstrs[3], NULL, 10);
	ptr->tRCPAGE = strtoul(tmpstrs[4], NULL, 10);

	return(true);
}

bool LoadDramTiming8FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming8 *ptr = (Navi10DRAMTiming8 *)val;
	char *tmpstrs[5];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssss}", "tRD_RDDD", &tmpstrs[0], "tRD_RDSD", &tmpstrs[1], "tRD_RDSC", &tmpstrs[2], "tRD_RDSCL", &tmpstrs[3], "tRD_RD_BAN", &tmpstrs[4]);

	if(ret == -1) return(false);
	
	ptr->tRD_RDDD = strtoul(tmpstrs[0], NULL, 10);
	ptr->tRD_RDSD = strtoul(tmpstrs[1], NULL, 10);
	ptr->tRD_RDSC = strtoul(tmpstrs[2], NULL, 10);
	ptr->tRD_RDSCL = strtoul(tmpstrs[3], NULL, 10);
	ptr->tRD_RD_BAN = strtoul(tmpstrs[4], NULL, 10);
	
	return(true);
}

bool LoadDramTiming9FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming9 *ptr = (Navi10DRAMTiming9 *)val;
	char *tmpstrs[4];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssss}", "tWR_WRMW", &tmpstrs[0], "tWR_WRSC", &tmpstrs[1], "tWR_WRSCL", &tmpstrs[2], "tWR_WR_BAN", &tmpstrs[3]);

	if(ret == -1) return(false);
	
	ptr->tWR_WRMW = strtoul(tmpstrs[0], NULL, 10);
	ptr->tWR_WRSC = strtoul(tmpstrs[1], NULL, 10);
	ptr->tWR_WRSCL = strtoul(tmpstrs[2], NULL, 10);
	ptr->tWR_WR_BAN = strtoul(tmpstrs[3], NULL, 10);
	
	return(true);
}

bool LoadDramTiming10FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming10 *ptr = (Navi10DRAMTiming10 *)val;
	char *tmpstrs[4];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssss}", "tWRRD", &tmpstrs[0], "tRDWR", &tmpstrs[1], "RDRspDelay", &tmpstrs[2], "tRefttAdj", &tmpstrs[3]);

	if(ret == -1) return(false);
	
	ptr->tWRRD = strtoul(tmpstrs[0], NULL, 10);
	ptr->tRDWR = strtoul(tmpstrs[1], NULL, 10);
	ptr->RDRspDelay = strtoul(tmpstrs[2], NULL, 10);
	ptr->tRefttAdj = strtoul(tmpstrs[3], NULL, 10);
	
	return(true);
}

bool LoadDramTiming12FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming12 *ptr = (Navi10DRAMTiming12 *)val;
	char *tmpstr;
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ss}", "tREF", &tmpstr);

	if(ret == -1) return(false);
	
	ptr->tREF = strtoul(tmpstr, NULL, 10);

	return(true);
}

bool LoadDramTiming13FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming13 *ptr = (Navi10DRAMTiming13 *)val;
	char *tmpstrs[2];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssss}", "tMRD", &tmpstrs[0], "tMOD", &tmpstrs[1]);

	if(ret == -1) return(false);
	
	ptr->tMRD = strtoul(tmpstrs[0], NULL, 10);
	ptr->tMOD = strtoul(tmpstrs[1], NULL, 10);

	return(true);
}

bool LoadDramTiming14FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming14 *ptr = (Navi10DRAMTiming14 *)val;
	char *tmpstrs[2];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssss}", "tXS", &tmpstrs[0], "tDLLK", &tmpstrs[1]);

	if(ret == -1) return(false);
	
	ptr->tXS = strtoul(tmpstrs[0], NULL, 10);
	ptr->tDLLK = strtoul(tmpstrs[1], NULL, 10);

	return(true);
}

bool LoadDramTiming16FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming16 *ptr = (Navi10DRAMTiming16 *)val;
	char *tmpstr;
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ss}", "tXSMRS", &tmpstr);

	if(ret == -1) return(false);
	
	ptr->tXSMRS = strtoul(tmpstr, NULL, 10);

	return(true);
}

bool LoadDramTiming17FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming17 *ptr = (Navi10DRAMTiming17 *)val;
	char *tmpstrs[5];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssss}", "tPD", &tmpstrs[0], "tCKSRE", &tmpstrs[1], "tCKSRX", &tmpstrs[2], "PwrDownDly", &tmpstrs[3], "AggPwrDownDly", &tmpstrs[4]);

	if(ret == -1) return(false);
	
	ptr->tPD = strtoul(tmpstrs[0], NULL, 10);
	ptr->tCKSRE = strtoul(tmpstrs[1], NULL, 10);
	ptr->tCKSRX = strtoul(tmpstrs[2], NULL, 10);
	ptr->PwrDownDly = strtoul(tmpstrs[3], NULL, 10);
	ptr->AggPwrDownDly = strtoul(tmpstrs[4], NULL, 10);
	
	return(true);
}

bool LoadDramTiming20FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming20 *ptr = (Navi10DRAMTiming20 *)val;
	char *tmpstrs[2];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssss}", "tRFCSB", &tmpstrs[0], "tSTAG", &tmpstrs[1]);

	if(ret == -1) return(false);
	
	ptr->tRFCSB = strtoul(tmpstrs[0], NULL, 10);
	ptr->tSTAG = strtoul(tmpstrs[1], NULL, 10);

	return(true);
}

bool LoadDramTiming21FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming21 *ptr = (Navi10DRAMTiming21 *)val;
	char *tmpstrs[3];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssss}", "tXP", &tmpstrs[0], "tCPDED", &tmpstrs[1], "tCKE", &tmpstrs[2]);

	if(ret == -1) return(false);
	
	ptr->tXP = strtoul(tmpstrs[0], NULL, 10);
	ptr->tCPDED = strtoul(tmpstrs[1], NULL, 10);
	ptr->tCKE = strtoul(tmpstrs[2], NULL, 10);

	return(true);
}

bool LoadDramTiming22FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming22 *ptr = (Navi10DRAMTiming22 *)val;
	char *tmpstrs[5];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssss}", "tRDDATA_EN", &tmpstrs[0], "tPHY_WRLAT", &tmpstrs[1], "tPHY_RDLAT", &tmpstrs[2], "tPHY_WRDATA", &tmpstrs[3], "tPARIN_LAT", &tmpstrs[4]);

	if(ret == -1) return(false);
	
	ptr->tRDDATA_EN = strtoul(tmpstrs[0], NULL, 10);
	ptr->tPHY_WRLAT = strtoul(tmpstrs[1], NULL, 10);
	ptr->tPHY_RDLAT = strtoul(tmpstrs[2], NULL, 10);
	ptr->tPHY_WRDATA = strtoul(tmpstrs[3], NULL, 10);
	ptr->tPARIN_LAT = strtoul(tmpstrs[4], NULL, 10);
	
	return(true);
}

bool LoadDramTiming23FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming23 *ptr = (Navi10DRAMTiming23 *)val;
	char *tmpstrs[3];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssss}", "LpDly", &tmpstrs[0], "LpExitDly", &tmpstrs[1], "CKESTAGDLY", &tmpstrs[2]);

	if(ret == -1) return(false);
	
	ptr->LpDly = strtoul(tmpstrs[0], NULL, 10);
	ptr->LpExitDly = strtoul(tmpstrs[1], NULL, 10);
	ptr->CKESTAGDLY = strtoul(tmpstrs[2], NULL, 10);

	return(true);
}

bool LoadTRFCTimingCS01FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10TRFCTimingCS01 *ptr = (Navi10TRFCTimingCS01 *)val;
	char *tmpstr;
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ss}", "tRFC", &tmpstr);

	if(ret == -1) return(false);
	
	ptr->tRFC = strtoul(tmpstr, NULL, 10);

	return(true);
}

bool LoadDramTiming35FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming35 *ptr = (Navi10DRAMTiming35 *)val;
	char *tmpstrs[3];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssss}", "ReceiverWait", &tmpstrs[0], "CmdStageCnt", &tmpstrs[1], "tWRMPR", &tmpstrs[2]);

	if(ret == -1) return(false);
	
	ptr->ReceiverWait = strtoul(tmpstrs[0], NULL, 10);
	ptr->CmdStageCnt = strtoul(tmpstrs[1], NULL, 10);
	ptr->tWRMPR = strtoul(tmpstrs[2], NULL, 10);

	return(true);
}

bool LoadDramTiming36FromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10DRAMTiming36 *ptr = (Navi10DRAMTiming36 *)val;
	char *tmpstrs[6];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssssssssss}", "tWTRTR", &tmpstrs[0], "tREFTT", &tmpstrs[1], "tTTROW", &tmpstrs[2], "tLDTLD", &tmpstrs[3], "tUPDN", &tmpstrs[4], "tREFTT_MSB", &tmpstrs[5]);

	if(ret == -1) return(false);
	
	ptr->tWTRTR = strtoul(tmpstrs[0], NULL, 10);
	ptr->tREFTT = strtoul(tmpstrs[1], NULL, 10);
	ptr->tTTROW = strtoul(tmpstrs[2], NULL, 10);
	ptr->tLDTLD = strtoul(tmpstrs[3], NULL, 10);
	ptr->tUPDN = strtoul(tmpstrs[4], NULL, 10);
	ptr->tREFTT_MSB = strtoul(tmpstrs[5], NULL, 10);

	return(true);
}

bool LoadChanPipeDlyFromJSON(uint32_t *val, json_t *RegObj)
{
	Navi10ChanPipeDly *ptr = (Navi10ChanPipeDly *)val;
	char *tmpstrs[3];
	int ret;
	
	ret = json_unpack_ex(RegObj, NULL, JSON_STRICT, "{ssssss}", "TXCtrlChanDly", &tmpstrs[0], "TXDataChanDly", &tmpstrs[1], "RXDataChanDly", &tmpstrs[2]);
	
	if(ret == -1) return(false);
	
	ptr->TXCtrlChanDly = strtoul(tmpstrs[0], NULL, 10);
	ptr->TXDataChanDly = strtoul(tmpstrs[0], NULL, 10);
	ptr->RXDataChanDly = strtoul(tmpstrs[0], NULL, 10);
	
	return(true);
}

bool LoadNaviRegsFromJSON(uint32_t *RegTemplate, json_t *JSONCfg)
{
	bool err = false;
	err |= LoadNavi10UMCCTRL_MISC2FromJSON((RegTemplate + 0), json_object_get(JSONCfg, "UMCCTRL_MISC2"));
	err |= LoadNavi10UMCCTRL_PMG_CMD_MRSFromJSON((RegTemplate + 1), json_object_get(JSONCfg, "UMCCTRL_PMG_CMD_MRS"));
	err |= LoadNavi10UMCCTRL_PMG_CMD_MRS1FromJSON((RegTemplate + 3), json_object_get(JSONCfg, "UMCCTRL_PMG_CMD_MRS1"));
	err |= LoadNavi10PMG_CMDFromJSON((RegTemplate + 4), json_object_get(JSONCfg, "PMG_CMD"));
	err |= LoadDramTiming1FromJSON((RegTemplate + 5), json_object_get(JSONCfg, "DramTiming1"));
	err |= LoadDramTiming2FromJSON((RegTemplate + 6), json_object_get(JSONCfg, "DramTiming2"));
	err |= LoadDramTiming3FromJSON((RegTemplate + 7), json_object_get(JSONCfg, "DramTiming3"));
	err |= LoadDramTiming4FromJSON((RegTemplate + 8), json_object_get(JSONCfg, "DramTiming4"));
	err |= LoadDramTiming5FromJSON((RegTemplate + 9), json_object_get(JSONCfg, "DramTiming5"));
	err |= LoadDramTiming6FromJSON((RegTemplate + 10), json_object_get(JSONCfg, "DramTiming6"));
	err |= LoadDramTiming7FromJSON((RegTemplate + 11), json_object_get(JSONCfg, "DramTiming7"));
	err |= LoadDramTiming8FromJSON((RegTemplate + 12), json_object_get(JSONCfg, "DramTiming8"));
	err |= LoadDramTiming9FromJSON((RegTemplate + 13), json_object_get(JSONCfg, "DramTiming9"));
	err |= LoadDramTiming10FromJSON((RegTemplate + 14), json_object_get(JSONCfg, "DramTiming10"));
	err |= LoadDramTiming12FromJSON((RegTemplate + 15), json_object_get(JSONCfg, "DramTiming12"));
	err |= LoadDramTiming13FromJSON((RegTemplate + 16), json_object_get(JSONCfg, "DramTiming13"));
	err |= LoadDramTiming14FromJSON((RegTemplate + 17), json_object_get(JSONCfg, "DramTiming14"));
	err |= LoadDramTiming16FromJSON((RegTemplate + 18), json_object_get(JSONCfg, "DramTiming16"));
	err |= LoadDramTiming17FromJSON((RegTemplate + 19), json_object_get(JSONCfg, "DramTiming17"));
	err |= LoadDramTiming20FromJSON((RegTemplate + 20), json_object_get(JSONCfg, "DramTiming20"));
	err |= LoadDramTiming21FromJSON((RegTemplate + 21), json_object_get(JSONCfg, "DramTiming21"));
	err |= LoadDramTiming22FromJSON((RegTemplate + 22), json_object_get(JSONCfg, "DramTiming22"));
	err |= LoadDramTiming23FromJSON((RegTemplate + 23), json_object_get(JSONCfg, "DramTiming23"));
	err |= LoadDramTiming35FromJSON((RegTemplate + 24), json_object_get(JSONCfg, "DramTiming35"));
	err |= LoadDramTiming36FromJSON((RegTemplate + 25), json_object_get(JSONCfg, "DramTiming36"));
	err |= LoadTRFCTimingCS01FromJSON((RegTemplate + 26), json_object_get(JSONCfg, "TRFCTimingCS01"));
	err |= LoadChanPipeDlyFromJSON((RegTemplate + 27), json_object_get(JSONCfg, "ChanPipeDly"));

	return(err);
}
