/** @file

  Fixed ACPI Description Table (FADT)

  Copyright (c) 2012 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/**

  Derived from:
   ArmPlatformPkg/ArmJunoPkg/AcpiTables/Fadt.aslc

**/

#include <AmdStyxAcpiLib.h>

#define FADT_FLAGS          ( EFI_ACPI_5_1_HW_REDUCED_ACPI |           \
                              EFI_ACPI_5_1_LOW_POWER_S0_IDLE_CAPABLE | \
                              EFI_ACPI_5_1_HEADLESS )

#pragma pack(push, 1)

STATIC EFI_ACPI_5_1_FIXED_ACPI_DESCRIPTION_TABLE AcpiFadt = {
    AMD_ACPI_HEADER (EFI_ACPI_5_1_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
                     EFI_ACPI_5_1_FIXED_ACPI_DESCRIPTION_TABLE,
                     EFI_ACPI_5_1_FIXED_ACPI_DESCRIPTION_TABLE_REVISION),
  0,                                                                        // UINT32     FirmwareCtrl
  0,                                                                        // UINT32     Dsdt
  EFI_ACPI_RESERVED_BYTE,                                                   // UINT8      Reserved0
  EFI_ACPI_5_1_PM_PROFILE_ENTERPRISE_SERVER,                                // UINT8      PreferredPmProfile
  0,                                                                        // UINT16     SciInt
  0,                                                                        // UINT32     SmiCmd
  0,                                                                        // UINT8      AcpiEnable
  0,                                                                        // UINT8      AcpiDisable
  0,                                                                        // UINT8      S4BiosReq
  0,                                                                        // UINT8      PstateCnt
  0,                                                                        // UINT32     Pm1aEvtBlk
  0,                                                                        // UINT32     Pm1bEvtBlk
  0,                                                                        // UINT32     Pm1aCntBlk
  0,                                                                        // UINT32     Pm1bCntBlk
  0,                                                                        // UINT32     Pm2CntBlk
  0,                                                                        // UINT32     PmTmrBlk
  0,                                                                        // UINT32     Gpe0Blk
  0,                                                                        // UINT32     Gpe1Blk
  0,                                                                        // UINT8      Pm1EvtLen
  0,                                                                        // UINT8      Pm1CntLen
  0,                                                                        // UINT8      Pm2CntLen
  0,                                                                        // UINT8      PmTmrLen
  0,                                                                        // UINT8      Gpe0BlkLen
  0,                                                                        // UINT8      Gpe1BlkLen
  0,                                                                        // UINT8      Gpe1Base
  0,                                                                        // UINT8      CstCnt
  0,                                                                        // UINT16     PLvl2Lat
  0,                                                                        // UINT16     PLvl3Lat
  0,                                                                        // UINT16     FlushSize
  0,                                                                        // UINT16     FlushStride
  0,                                                                        // UINT8      DutyOffset
  0,                                                                        // UINT8      DutyWidth
  0,                                                                        // UINT8      DayAlrm
  0,                                                                        // UINT8      MonAlrm
  0,                                                                        // UINT8      Century
  0,                                                                        // UINT16     IaPcBootArch
  0,                                                                        // UINT8      Reserved1
  FADT_FLAGS,                                                               // UINT32     Flags
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  ResetReg
  0,                                                                        // UINT8      ResetValue
  EFI_ACPI_5_1_ARM_PSCI_COMPLIANT,                                          // UINT16     ArmBootArch
  1,                                                                        // UINT8      MinorVersion
  0,                                                                        // UINT64     XFirmwareCtrl
  0,                                                                        // UINT64     XDsdt
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XPm1aEvtBlk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XPm1bEvtBlk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XPm1aCntBlk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XPm1bCntBlk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XPm2CntBlk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XPmTmrBlk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XGpe0Blk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  XGpe1Blk
  NULL_GAS,                                                                 // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  SleepControlReg
  NULL_GAS                                                                  // EFI_ACPI_5_1_GENERIC_ADDRESS_STRUCTURE  SleepStatusReg
};

#pragma pack(pop)

EFI_ACPI_DESCRIPTION_HEADER *
FadtTable (
  VOID
  )
{
  return (EFI_ACPI_DESCRIPTION_HEADER *) &AcpiFadt;
}

