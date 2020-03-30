#pragma once

#include <stdint.h>

#define OFFSET_TO_POINTER_TO_ATOM_ROM_HEADER		0x00000048L

typedef struct _ATOM_TABLE_HEADER
{
	uint16_t TableSize;
	uint8_t  TableFormatRevision;
	uint8_t  TableContentRevision;
} ATOM_TABLE_HEADER;

/****************************************************************************/
// Structure stores the ROM header.
/****************************************************************************/
typedef struct _ATOM_ROM_HEADER_V1_1
{
	ATOM_TABLE_HEADER Header;
	uint8_t	 ATOMBIOSString[4];		// "ATOM"
	uint16_t BIOSRuntimeSegmentAddress;
	uint16_t ProtectedModeInfoOffset;
	uint16_t ConfigFilenameOffset;
	uint16_t CRCBlockOffset;
	uint16_t VBIOSBootupMessageOffset;
	uint16_t Int10Offset;
	uint16_t PCIBusDevInitCode;
	uint16_t IOBaseAddress;
	uint16_t SubsystemVendorID;
	uint16_t SubsystemID;
	uint16_t PCIInfoOffset;
	uint16_t MasterCommandTableOffset; /*Offset for SW to get all command table offsets, Don't change the position */
	uint16_t MasterDataTableOffset;   /*Offset for SW to get all data table offsets, Don't change the position */
	uint8_t  ExtendedFunctionCode;
	uint8_t  Reserved;
} ATOM_ROM_HEADER_V1_1;

typedef struct _ATOM_ROM_HEADER_V2_2
{
	ATOM_TABLE_HEADER Header;
	uint8_t  ATOMBIOSString[4];				//enum atom_string_def atom_bios_string;     //Signature to distinguish between Atombios and non-atombios,
	uint16_t BIOSRuntimeSegmentAddress;
	uint16_t ProtectedModeInfoOffset;
	uint16_t ConfigFilenameOffset;
	uint16_t CRCBlockOffset;
	uint16_t VBIOSBootupMessageOffset;
	uint16_t Int10Offset;
	uint16_t PCIBusDevInitCode;
	uint16_t IOBaseAddress;
	uint16_t SubsystemVendorID;
	uint16_t SubsystemID;
	uint16_t PCIInfoOffset;
	uint16_t MasterCommandTableOffset;		//Offest for SW to get all command function offsets, Don't change the position
	uint16_t MasterDataTableOffset;			//Offest for SW to get all data table offsets, Don't change the position
	uint16_t Reserved;
	uint32_t PSPDirTableOffset;
} ATOM_ROM_HEADER_V2_2;

typedef struct _ATOM_MASTER_DATA_TABLE_LIST_V1_1
{
	ATOM_TABLE_HEADER Header;
	uint16_t        UtilityPipeLine;	        // Offest for the utility to get parser info,Don't change this position!
	uint16_t        MultimediaCapabilityInfo; // Only used by MM Lib,latest version 1.1, not configuable from Bios, need to include the table to build Bios
	uint16_t        MultimediaConfigInfo;     // Only used by MM Lib,latest version 2.1, not configuable from Bios, need to include the table to build Bios
	uint16_t        StandardVESA_Timing;      // Only used by Bios
	uint16_t        FirmwareInfo;             // Shared by various SW components,latest version 1.4
	uint16_t        DAC_Info;                 // Will be obsolete from R600
	uint16_t        LCD_Info;                 // Shared by various SW components,latest version 1.3, was called LVDS_Info
	uint16_t        TMDS_Info;                // Will be obsolete from R600
	uint16_t        AnalogTV_Info;            // Shared by various SW components,latest version 1.1
	uint16_t        SupportedDevicesInfo;     // Will be obsolete from R600
	uint16_t        GPIO_I2C_Info;            // Shared by various SW components,latest version 1.2 will be used from R600
	uint16_t        VRAM_UsageByFirmware;     // Shared by various SW components,latest version 1.3 will be used from R600
	uint16_t        GPIO_Pin_LUT;             // Shared by various SW components,latest version 1.1
	uint16_t        VESA_ToInternalModeLUT;   // Only used by Bios
	uint16_t        ComponentVideoInfo;       // Shared by various SW components,latest version 2.1 will be used from R600
	uint16_t        PowerPlayInfo;            // Shared by various SW components,latest version 2.1,new design from R600
	uint16_t        CompassionateData;        // Will be obsolete from R600
	uint16_t        SaveRestoreInfo;          // Only used by Bios
	uint16_t        PPLL_SS_Info;             // Shared by various SW components,latest version 1.2, used to call SS_Info, change to new name because of int ASIC SS info
	uint16_t        OemInfo;                  // Defined and used by external SW, should be obsolete soon
	uint16_t        XTMDS_Info;               // Will be obsolete from R600
	uint16_t        MclkSS_Info;              // Shared by various SW components,latest version 1.1, only enabled when ext SS chip is used
	uint16_t        Object_Header;            // Shared by various SW components,latest version 1.1
	uint16_t        IndirectIOAccess;         // Only used by Bios,this table position can't change at all!!
	uint16_t        MC_InitParameter;         // Only used by command table
	uint16_t        ASIC_VDDC_Info;						// Will be obsolete from R600
	uint16_t        ASIC_InternalSS_Info;			// New tabel name from R600, used to be called "ASIC_MVDDC_Info"
	uint16_t        TV_VideoMode;							// Only used by command table
	uint16_t        VRAM_Info;								// Only used by command table, latest version 1.3
	uint16_t        MemoryTrainingInfo;				// Used for VBIOS and Diag utility for memory training purpose since R600. the new table rev start from 2.1
	uint16_t        IntegratedSystemInfo;			// Shared by various SW components
	uint16_t        ASIC_ProfilingInfo;				// New table name from R600, used to be called "ASIC_VDDCI_Info" for pre-R600
	uint16_t        VoltageObjectInfo;				// Shared by various SW components, latest version 1.1
	uint16_t		PowerSourceInfo;					// Shared by various SW components, latest versoin 1.1
	uint16_t		ServiceInfo;
} ATOM_MASTER_DATA_TABLE_LIST_V1_1;

typedef struct _ATOM_MASTER_DATA_TABLE_LIST_V2_1
{
	ATOM_TABLE_HEADER Header;
	uint16_t Utilitypipeline;               /* Offest for the utility to get parser info,Don't change this position!*/
	uint16_t MultimediaCapabilityInfo;
	uint16_t SMC_DPM_Info;
	uint16_t SWDataTable3;
	uint16_t FirmwareInfo;                  /* Shared by various SW components */
	uint16_t SWDataTable5;
	uint16_t LCD_Info;                      /* Shared by various SW components */
	uint16_t SWDataTable7;
	uint16_t SMU_Info;
	uint16_t SWDataTable9;
	uint16_t SWDataTable10;
	uint16_t VRAM_UsageByFirmware;          /* Shared by various SW components */
	uint16_t GPIO_Pin_LUT;                  /* Shared by various SW components */
	uint16_t SWDataTable13;
	uint16_t GFX_Info;
	uint16_t PowerPlayInfo;                 /* Shared by various SW components */
	uint16_t SWDataTable16;
	uint16_t SWDataTable17;
	uint16_t SWDataTable18;
	uint16_t SWDataTable19;
	uint16_t SWDataTable20;
	uint16_t SWDataTable21;
	uint16_t DisplayObjectInfo;             /* Shared by various SW components */
	uint16_t IndirectIOAccess;			  /* used as an internal one */
	uint16_t UMC_Info;                      /* Shared by various SW components */
	uint16_t SWDataTable25;
	uint16_t SWDataTable26;
	uint16_t DCE_Info;                      /* Shared by various SW components */
	uint16_t VRAM_Info;                     /* Shared by various SW components */
	uint16_t SWDataTable29;
	uint16_t IntegratedSystemInfo;          /* Shared by various SW components */
	uint16_t ASIC_ProfilingInfo;           /* Shared by various SW components */
	uint16_t VoltageObjectInfo;            /* shared by various SW components */
	uint16_t SWDataTable33;
	uint16_t SWDataTable34;
} ATOM_MASTER_DATA_TABLE_LIST_V2_1;

typedef struct _ATOM_MASTER_COMMAND_TABLE_LIST_V1_1
{
	ATOM_TABLE_HEADER Header;
	uint16_t ASIC_Init;                              //Function Table, used by various SW components,latest version 1.1
	uint16_t GetDisplaySurfaceSize;                  //Atomic Table,  Used by Bios when enabling HW ICON
	uint16_t ASIC_RegistersInit;                     //Atomic Table,  indirectly used by various SW components,called from ASIC_Init
	uint16_t VRAM_BlockVenderDetection;              //Atomic Table,  used only by Bios
	uint16_t DIGxEncoderControl;										 //Only used by Bios
	uint16_t MemoryControllerInit;                   //Atomic Table,  indirectly used by various SW components,called from ASIC_Init
	uint16_t EnableCRTCMemReq;                       //Function Table,directly used by various SW components,latest version 2.1
	uint16_t MemoryParamAdjust; 										 //Atomic Table,  indirectly used by various SW components,called from SetMemoryClock if needed
	uint16_t DVOEncoderControl;                      //Function Table,directly used by various SW components,latest version 1.2
	uint16_t GPIOPinControl;												 //Atomic Table,  only used by Bios
	uint16_t SetEngineClock;                         //Function Table,directly used by various SW components,latest version 1.1
	uint16_t SetMemoryClock;                         //Function Table,directly used by various SW components,latest version 1.1
	uint16_t SetPixelClock;                          //Function Table,directly used by various SW components,latest version 1.2
	uint16_t DynamicClockGating;                     //Atomic Table,  indirectly used by various SW components,called from ASIC_Init
	uint16_t ResetMemoryDLL;                         //Atomic Table,  indirectly used by various SW components,called from SetMemoryClock
	uint16_t ResetMemoryDevice;                      //Atomic Table,  indirectly used by various SW components,called from SetMemoryClock
	uint16_t MemoryPLLInit;
	uint16_t AdjustDisplayPll;												//only used by Bios
	uint16_t AdjustMemoryController;                 //Atomic Table,  indirectly used by various SW components,called from SetMemoryClock
	uint16_t EnableASIC_StaticPwrMgt;                //Atomic Table,  only used by Bios
	uint16_t ASIC_StaticPwrMgtStatusChange;          //Obsolete ,     only used by Bios
	uint16_t DAC_LoadDetection;                      //Atomic Table,  directly used by various SW components,latest version 1.2
	uint16_t LVTMAEncoderControl;                    //Atomic Table,directly used by various SW components,latest version 1.3
	uint16_t LCD1OutputControl;                      //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t DAC1EncoderControl;                     //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t DAC2EncoderControl;                     //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t DVOOutputControl;                       //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t CV1OutputControl;                       //Atomic Table,  Atomic Table,  Obsolete from Ry6xx, use DAC2 Output instead
	uint16_t GetConditionalGoldenSetting;            //only used by Bios
	uint16_t TVEncoderControl;                       //Function Table,directly used by various SW components,latest version 1.1
	uint16_t TMDSAEncoderControl;                    //Atomic Table,  directly used by various SW components,latest version 1.3
	uint16_t LVDSEncoderControl;                     //Atomic Table,  directly used by various SW components,latest version 1.3
	uint16_t TV1OutputControl;                       //Atomic Table,  Obsolete from Ry6xx, use DAC2 Output instead
	uint16_t EnableScaler;                           //Atomic Table,  used only by Bios
	uint16_t BlankCRTC;                              //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t EnableCRTC;                             //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t GetPixelClock;                          //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t EnableVGA_Render;                       //Function Table,directly used by various SW components,latest version 1.1
	uint16_t GetSCLKOverMCLKRatio;                   //Atomic Table,  only used by Bios
	uint16_t SetCRTC_Timing;                         //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t SetCRTC_OverScan;                       //Atomic Table,  used by various SW components,latest version 1.1
	uint16_t SetCRTC_Replication;                    //Atomic Table,  used only by Bios
	uint16_t SelectCRTC_Source;                      //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t EnableGraphSurfaces;                    //Atomic Table,  used only by Bios
	uint16_t UpdateCRTC_DoubleBufferRegisters;
	uint16_t LUT_AutoFill;                           //Atomic Table,  only used by Bios
	uint16_t EnableHW_IconCursor;                    //Atomic Table,  only used by Bios
	uint16_t GetMemoryClock;                         //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t GetEngineClock;                         //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t SetCRTC_UsingDTDTiming;                 //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t ExternalEncoderControl;                 //Atomic Table,  directly used by various SW components,latest version 2.1
	uint16_t LVTMAOutputControl;                     //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t VRAM_BlockDetectionByStrap;             //Atomic Table,  used only by Bios
	uint16_t MemoryCleanUp;                          //Atomic Table,  only used by Bios
	uint16_t ProcessI2cChannelTransaction;           //Function Table,only used by Bios
	uint16_t WriteOneByteToHWAssistedI2C;            //Function Table,indirectly used by various SW components
	uint16_t ReadHWAssistedI2CStatus;                //Atomic Table,  indirectly used by various SW components
	uint16_t SpeedFanControl;                        //Function Table,indirectly used by various SW components,called from ASIC_Init
	uint16_t PowerConnectorDetection;                //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t MC_Synchronization;                     //Atomic Table,  indirectly used by various SW components,called from SetMemoryClock
	uint16_t ComputeMemoryEnginePLL;                 //Atomic Table,  indirectly used by various SW components,called from SetMemory/EngineClock
	uint16_t MemoryRefreshConversion;                //Atomic Table,  indirectly used by various SW components,called from SetMemory or SetEngineClock
	uint16_t VRAM_GetCurrentInfoBlock;               //Atomic Table,  used only by Bios
	uint16_t DynamicMemorySettings;                  //Atomic Table,  indirectly used by various SW components,called from SetMemoryClock
	uint16_t MemoryTraining;                         //Atomic Table,  used only by Bios
	uint16_t EnableSpreadSpectrumOnPPLL;             //Atomic Table,  directly used by various SW components,latest version 1.2
	uint16_t TMDSAOutputControl;                     //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t SetVoltage;                             //Function Table,directly and/or indirectly used by various SW components,latest version 1.1
	uint16_t DAC1OutputControl;                      //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t DAC2OutputControl;                      //Atomic Table,  directly used by various SW components,latest version 1.1
	uint16_t SetupHWAssistedI2CStatus;               //Function Table,only used by Bios, obsolete soon.Switch to use "ReadEDIDFromHWAssistedI2C"
	uint16_t ClockSource;                            //Atomic Table,  indirectly used by various SW components,called from ASIC_Init
	uint16_t MemoryDeviceInit;                       //Atomic Table,  indirectly used by various SW components,called from SetMemoryClock
	uint16_t EnableYUV;                              //Atomic Table,  indirectly used by various SW components,called from EnableVGARender
	uint16_t DIG1EncoderControl;                     //Atomic Table,directly used by various SW components,latest version 1.1
	uint16_t DIG2EncoderControl;                     //Atomic Table,directly used by various SW components,latest version 1.1
	uint16_t DIG1TransmitterControl;                 //Atomic Table,directly used by various SW components,latest version 1.1
	uint16_t DIG2TransmitterControl;	               //Atomic Table,directly used by various SW components,latest version 1.1
	uint16_t ProcessAuxChannelTransaction;					 //Function Table,only used by Bios
	uint16_t DPEncoderService;											 //Function Table,only used by Bios
	uint16_t GetVoltageInfo;
} ATOM_MASTER_COMMAND_TABLE_LIST_V1_1;

struct _ATOM_MASTER_COMMAND_TABLE_LIST_V2_1
{
	ATOM_TABLE_HEADER Header;
	uint16_t ASIC_Init;                   //Function
	uint16_t SWCommandTable1;               //used as an internal one
	uint16_t SWCommandTable2;               //used as an internal one
	uint16_t SWCommandTable3;               //used as an internal one
	uint16_t DIGxEncoderControl;          //Function
	uint16_t SWCommandTable5;               //used as an internal one
	uint16_t SWCommandTable6;               //used as an internal one
	uint16_t SWCommandTable7;               //used as an internal one
	uint16_t SWCommandTable8;               //used as an internal one
	uint16_t SWCommandTable9;               //used as an internal one
	uint16_t SetEngineClock;              //Function
	uint16_t SetMemoryClock;              //Function
	uint16_t SetPixelClock;               //Function
	uint16_t EnableDispPowerGating;       //Function
	uint16_t SWCommandTable14;              //used as an internal one
	uint16_t SWCommandTable15;              //used as an internal one
	uint16_t SWCommandTable16;              //used as an internal one
	uint16_t SWCommandTable17;              //used as an internal one
	uint16_t SWCommandTable18;              //used as an internal one
	uint16_t SWCommandTable19;              //used as an internal one
	uint16_t SWCommandTable20;              //used as an internal one
	uint16_t SWCommandTable21;              //used as an internal one
	uint16_t SWCommandTable22;              //used as an internal one
	uint16_t SWCommandTable23;              //used as an internal one
	uint16_t SWCommandTable24;              //used as an internal one
	uint16_t SWCommandTable25;              //used as an internal one
	uint16_t SWCommandTable26;              //used as an internal one
	uint16_t SWCommandTable27;              //used as an internal one
	uint16_t SWCommandTable28;              //used as an internal one
	uint16_t SWCommandTable29;              //used as an internal one
	uint16_t SWCommandTable30;              //used as an internal one
	uint16_t SWCommandTable31;              //used as an internal one
	uint16_t SWCommandTable32;              //used as an internal one
	uint16_t SWCommandTable33;              //used as an internal one
	uint16_t BlankCRTC;                   //Function
	uint16_t EnableCRTC;                  //Function
	uint16_t SWCommandTable36;              //used as an internal one
	uint16_t SWCommandTable37;              //used as an internal one
	uint16_t SWCommandTable38;              //used as an internal one
	uint16_t SWCommandTable39;              //used as an internal one
	uint16_t SWCommandTable40;              //used as an internal one
	uint16_t GetSMUClockInfo;             //Function
	uint16_t SelectCRTC_Source;           //Function
	uint16_t SWCommandTable43;              //used as an internal one
	uint16_t SWCommandTable44;              //used as an internal one
	uint16_t SWCommandTable45;              //used as an internal one
	uint16_t SetDCEClock;                 //Function
	uint16_t GetMemoryClock;              //Function
	uint16_t GetEngineClock;              //Function
	uint16_t SetCRTC_UsingDTDTiming;      //Function
	uint16_t ExternalEncoderControl;      //Function
	uint16_t SWCommandTable51;              //used as an internal one
	uint16_t SWCommandTable52;              //used as an internal one
	uint16_t SWCommandTable53;              //used as an internal one
	uint16_t ProcessI2cChannelTransaction;//Function
	uint16_t SWCommandTable55;              //used as an internal one
	uint16_t SWCommandTable56;              //used as an internal one
	uint16_t SWCommandTable57;              //used as an internal one
	uint16_t SWCommandTable58;              //used as an internal one
	uint16_t SWCommandTable59;              //used as an internal one
	uint16_t ComputeGPUClockParam;        //Function
	uint16_t SWCommandTable61;              //used as an internal one
	uint16_t SWCommandTable62;              //used as an internal one
	uint16_t DynamicMemorySettings;       //Function function
	uint16_t MemoryTraining;              //Function function
	uint16_t SWCommandTable65;              //used as an internal one
	uint16_t SWCommandTable66;              //used as an internal one
	uint16_t SetVoltage;                  //Function
	uint16_t SWCommandTable68;              //used as an internal one
	uint16_t ReadEfuseValue;              //Function
	uint16_t SWCommandTable70;              //used as an internal one
	uint16_t SWCommandTable71;              //used as an internal one
	uint16_t SWCommandTable72;              //used as an internal one
	uint16_t SWCommandTable73;              //used as an internal one
	uint16_t SWCommandTable74;              //used as an internal one
	uint16_t SWCommandTable75;              //used as an internal one
	uint16_t Dig1TransmitterControl;      //Function
	uint16_t SWCommandTable77;              //used as an internal one
	uint16_t ProcessAuxChannelTransaction;//Function
	uint16_t SWCommandTable79;              //used as an internal one
	uint16_t GetVoltageInfo;              //Function
};
