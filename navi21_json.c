// Copyright 2022 Wolf9466

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>

#include "gddr6.h"
#include "vram/navi.h"

void DwordToASCIIDecimal(char *asciistr, uint32_t dword, uint8_t size);

json_t *DumpNavi2RegsAsJSON(const Navi21TimingFmt *val)
{
	char tmpstrs[47][16];
	json_t *NewObj;

    DwordToASCIIDecimal(tmpstrs[0], val->RL, 1);
    DwordToASCIIDecimal(tmpstrs[1], val->WL, 1);
    DwordToASCIIDecimal(tmpstrs[2], val->tRAS, 1);
    DwordToASCIIDecimal(tmpstrs[3], val->tRC, 1);
    DwordToASCIIDecimal(tmpstrs[4], val->tREFI, 2);
    DwordToASCIIDecimal(tmpstrs[5], val->tRFC, 1);
    DwordToASCIIDecimal(tmpstrs[6], val->tRFCpb, 1);
    DwordToASCIIDecimal(tmpstrs[7], val->tRREFD, 1);
    DwordToASCIIDecimal(tmpstrs[8], val->tRCDRD, 1);
    DwordToASCIIDecimal(tmpstrs[9], val->tRCDWR, 1);
    DwordToASCIIDecimal(tmpstrs[10], val->tRP, 1);
    DwordToASCIIDecimal(tmpstrs[11], val->tRRDS, 1);
    DwordToASCIIDecimal(tmpstrs[12], val->tRRDL, 1);
    DwordToASCIIDecimal(tmpstrs[13], val->tWR, 1);
    DwordToASCIIDecimal(tmpstrs[14], val->tWTRS, 1);
    DwordToASCIIDecimal(tmpstrs[15], val->tWTRL, 1);
    DwordToASCIIDecimal(tmpstrs[16], val->tFAW, 1);
    DwordToASCIIDecimal(tmpstrs[17], val->tCCDS, 1);
    DwordToASCIIDecimal(tmpstrs[18], val->tCCDL, 1);
    DwordToASCIIDecimal(tmpstrs[19], val->tCRCRL, 1);
    DwordToASCIIDecimal(tmpstrs[20], val->tCRCWL, 1);
    DwordToASCIIDecimal(tmpstrs[21], val->tCKE, 1);
    DwordToASCIIDecimal(tmpstrs[22], val->tCKSRE, 1);
    DwordToASCIIDecimal(tmpstrs[23], val->tCKSRX, 1);
    DwordToASCIIDecimal(tmpstrs[24], val->tRTPS, 1);
    DwordToASCIIDecimal(tmpstrs[25], val->tRTPL, 1);
    DwordToASCIIDecimal(tmpstrs[26], val->tMRD, 1);
    DwordToASCIIDecimal(tmpstrs[27], val->tMOD, 1);
    DwordToASCIIDecimal(tmpstrs[28], val->tXS, 1);
    DwordToASCIIDecimal(tmpstrs[29], val->tXHP, 1);
    DwordToASCIIDecimal(tmpstrs[30], val->tXSMRS, 1);
    DwordToASCIIDecimal(tmpstrs[31], val->tXSH, 4);
    DwordToASCIIDecimal(tmpstrs[32], val->tPD, 1);
    DwordToASCIIDecimal(tmpstrs[33], val->tXP, 1);
    DwordToASCIIDecimal(tmpstrs[34], val->tCPDED, 1);
    DwordToASCIIDecimal(tmpstrs[35], val->tACTPDE, 1);
    DwordToASCIIDecimal(tmpstrs[36], val->tPREPDE, 1);
    DwordToASCIIDecimal(tmpstrs[37], val->tREFPDE, 1);
    DwordToASCIIDecimal(tmpstrs[38], val->tMRSPDEN, 1);
    DwordToASCIIDecimal(tmpstrs[39], val->tRDSRE, 1);
    DwordToASCIIDecimal(tmpstrs[40], val->tWRSRE, 1);
    DwordToASCIIDecimal(tmpstrs[41], val->tPPD, 1);
    DwordToASCIIDecimal(tmpstrs[42], val->tCCDMW, 1);
    DwordToASCIIDecimal(tmpstrs[43], val->tWTRTR, 1);
    DwordToASCIIDecimal(tmpstrs[44], val->tLTLTR, 1);
    DwordToASCIIDecimal(tmpstrs[45], val->tREFTR, 1);
    DwordToASCIIDecimal(tmpstrs[46], val->VNDR, 1);

    NewObj = json_pack("{ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss}", "RL", tmpstrs[0], "WL", tmpstrs[1], "tRAS", tmpstrs[2], "tRC", tmpstrs[3], "tREFI", tmpstrs[4], "tRFC", tmpstrs[5], "tRFCpb", tmpstrs[6], "tRREFD", tmpstrs[7], "tRCDRD", tmpstrs[8], "tRCDWR", tmpstrs[9], "tRP", tmpstrs[10], "tRRDS", tmpstrs[11], "tRRDL", tmpstrs[12], "tWR", tmpstrs[13], "tWTRS", tmpstrs[14], "tWTRL", tmpstrs[15], "tFAW", tmpstrs[16], "tCCDS", tmpstrs[17], "tCCDL", tmpstrs[18], "tCRCRL", tmpstrs[19], "tCRCWL", tmpstrs[20], "tCKE", tmpstrs[21], "tCKSRE", tmpstrs[22], "tCKSRX", tmpstrs[23], "tRTPS", tmpstrs[24], "tRTPL", tmpstrs[25], "tMRD", tmpstrs[26], "tMOD", tmpstrs[27], "tXS", tmpstrs[28], "tXHP", tmpstrs[29], "tXSMRS", tmpstrs[30], "tXSH", tmpstrs[31], "tPD", tmpstrs[32], "tXP", tmpstrs[33], "tCPDED", tmpstrs[34], "tACTPDE", tmpstrs[35], "tPREPDE", tmpstrs[36], "tREFPDE", tmpstrs[37], "tMRSPDEN", tmpstrs[38], "tRDSRE", tmpstrs[39], "tWRSRE", tmpstrs[40], "tPPD", tmpstrs[41], "tCCDMW", tmpstrs[42], "tWTRTR", tmpstrs[43], "tLTLTR", tmpstrs[44], "tREFTR", tmpstrs[45], "VNDR", tmpstrs[46]);
    return(NewObj);
}

bool LoadNavi2RegsFromJSON(Navi21TimingFmt *val, json_t *JSONCfg)
{
    char *tmpstrs[47];
    int ret = json_unpack_ex(JSONCfg, NULL, JSON_STRICT, "{ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss}", "RL", &tmpstrs[0], "WL", &tmpstrs[1], "tRAS", &tmpstrs[2], "tRC", &tmpstrs[3], "tREFI", &tmpstrs[4], "tRFC", &tmpstrs[5], "tRFCpb", &tmpstrs[6], "tRREFD", &tmpstrs[7], "tRCDRD", &tmpstrs[8], "tRCDWR", &tmpstrs[9], "tRP", &tmpstrs[10], "tRRDS", &tmpstrs[11], "tRRDL", &tmpstrs[12], "tWR", &tmpstrs[13], "tWTRS", &tmpstrs[14], "tWTRL", &tmpstrs[15], "tFAW", &tmpstrs[16], "tCCDS", &tmpstrs[17], "tCCDL", &tmpstrs[18], "tCRCRL", &tmpstrs[19], "tCRCWL", &tmpstrs[20], "tCKE", &tmpstrs[21], "tCKSRE", &tmpstrs[22], "tCKSRX", &tmpstrs[23], "tRTPS", &tmpstrs[24], "tRTPL", &tmpstrs[25], "tMRD", &tmpstrs[26], "tMOD", &tmpstrs[27], "tXS", &tmpstrs[28], "tXHP", &tmpstrs[29], "tXSMRS", &tmpstrs[30], "tXSH", &tmpstrs[31], "tPD", &tmpstrs[32], "tXP", &tmpstrs[33], "tCPDED", &tmpstrs[34], "tACTPDE", &tmpstrs[35], "tPREPDE", &tmpstrs[36], "tREFPDE", &tmpstrs[37], "tMRSPDEN", &tmpstrs[38], "tRDSRE", &tmpstrs[39], "tWRSRE", &tmpstrs[40], "tPPD", &tmpstrs[41], "tCCDMW", &tmpstrs[42], "tWTRTR", &tmpstrs[43], "tLTLTR", &tmpstrs[44], "tREFTR", &tmpstrs[45], "VNDR", &tmpstrs[46]);

    if(ret == -1) return(false);

    val->RL = strtoul(tmpstrs[0], NULL, 10);
    val->WL = strtoul(tmpstrs[1], NULL, 10);
    val->tRAS = strtoul(tmpstrs[2], NULL, 10);
    val->tRC = strtoul(tmpstrs[3], NULL, 10);
    val->tREFI = strtoul(tmpstrs[4], NULL, 10);
    val->tRFC = strtoul(tmpstrs[5], NULL, 10);
    val->tRFCpb = strtoul(tmpstrs[6], NULL, 10);
    val->tRREFD = strtoul(tmpstrs[7], NULL, 10);
    val->tRCDRD = strtoul(tmpstrs[8], NULL, 10);
    val->tRCDWR = strtoul(tmpstrs[9], NULL, 10);
    val->tRP = strtoul(tmpstrs[10], NULL, 10);
    val->tRRDS = strtoul(tmpstrs[11], NULL, 10);
    val->tRRDL = strtoul(tmpstrs[12], NULL, 10);
    val->tWR = strtoul(tmpstrs[13], NULL, 10);
    val->tWTRS = strtoul(tmpstrs[14], NULL, 10);
    val->tWTRL = strtoul(tmpstrs[15], NULL, 10);
    val->tFAW = strtoul(tmpstrs[16], NULL, 10);
    val->tCCDS = strtoul(tmpstrs[17], NULL, 10);
    val->tCCDL = strtoul(tmpstrs[18], NULL, 10);
    val->tCRCRL = strtoul(tmpstrs[19], NULL, 10);
    val->tCRCWL = strtoul(tmpstrs[20], NULL, 10);
    val->tCKE = strtoul(tmpstrs[21], NULL, 10);
    val->tCKSRE = strtoul(tmpstrs[22], NULL, 10);
    val->tCKSRX = strtoul(tmpstrs[23], NULL, 10);
    val->tRTPS = strtoul(tmpstrs[24], NULL, 10);
    val->tRTPL = strtoul(tmpstrs[25], NULL, 10);
    val->tMRD = strtoul(tmpstrs[26], NULL, 10);
    val->tMOD = strtoul(tmpstrs[27], NULL, 10);
    val->tXS = strtoul(tmpstrs[28], NULL, 10);
    val->tXHP = strtoul(tmpstrs[29], NULL, 10);
    val->tXSMRS = strtoul(tmpstrs[30], NULL, 10);
    val->tXSH = strtoul(tmpstrs[31], NULL, 10);
    val->tPD = strtoul(tmpstrs[32], NULL, 10);
    val->tXP = strtoul(tmpstrs[33], NULL, 10);
    val->tCPDED = strtoul(tmpstrs[34], NULL, 10);
    val->tACTPDE = strtoul(tmpstrs[35], NULL, 10);
    val->tPREPDE = strtoul(tmpstrs[36], NULL, 10);
    val->tREFPDE = strtoul(tmpstrs[37], NULL, 10);
    val->tMRSPDEN = strtoul(tmpstrs[38], NULL, 10);
    val->tRDSRE = strtoul(tmpstrs[39], NULL, 10);
    val->tWRSRE = strtoul(tmpstrs[40], NULL, 10);
    val->tPPD = strtoul(tmpstrs[41], NULL, 10);
    val->tCCDMW = strtoul(tmpstrs[42], NULL, 10);
    val->tWTRTR = strtoul(tmpstrs[43], NULL, 10);
    val->tLTLTR = strtoul(tmpstrs[44], NULL, 10);
    val->tREFTR = strtoul(tmpstrs[45], NULL, 10);
    val->VNDR = strtoul(tmpstrs[46], NULL, 10);

    return(true);
}
