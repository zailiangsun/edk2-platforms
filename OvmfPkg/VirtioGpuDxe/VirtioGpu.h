/** @file

  Internal type and macro definitions for the Virtio GPU hybrid driver.

  Copyright (C) 2016, Red Hat, Inc.

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VIRTIO_GPU_DXE_H_
#define _VIRTIO_GPU_DXE_H_

#include <IndustryStandard/VirtioGpu.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Protocol/VirtioDevice.h>

//
// Forward declaration of VGPU_GOP.
//
typedef struct VGPU_GOP_STRUCT VGPU_GOP;

//
// The abstraction that directly corresponds to a Virtio GPU device.
//
// This structure will be installed on the handle that has the VirtIo Device
// Protocol interface, with GUID gEfiCallerIdGuid. A similar trick is employed
// in TerminalDxe, and it is necessary so that we can look up VGPU_DEV just
// from the VirtIo Device Protocol handle in the Component Name 2 Protocol
// implementation.
//
typedef struct {
  //
  // VirtIo represents access to the Virtio GPU device. Never NULL.
  //
  VIRTIO_DEVICE_PROTOCOL   *VirtIo;

  //
  // BusName carries a customized name for
  // EFI_COMPONENT_NAME2_PROTOCOL.GetControllerName(). It is expressed in table
  // form because it can theoretically support several languages. Never NULL.
  //
  EFI_UNICODE_STRING_TABLE *BusName;

  //
  // VirtIo ring used for VirtIo communication.
  //
  VRING                    Ring;

  //
  // Event to be signaled at ExitBootServices().
  //
  EFI_EVENT                ExitBoot;

  //
  // Common running counter for all VirtIo GPU requests that ask for fencing.
  //
  UINT64                   FenceId;

  //
  // The Child field references the GOP wrapper structure. If this pointer is
  // NULL, then the hybrid driver has bound (i.e., started) the
  // VIRTIO_DEVICE_PROTOCOL controller without producing the child GOP
  // controller (that is, after Start() was called with RemainingDevicePath
  // pointing to and End of Device Path node). Child can be created and
  // destroyed, even repeatedly, independently of VGPU_DEV.
  //
  // In practice, this field represents the single head (scanout) that we
  // support.
  //
  VGPU_GOP                 *Child;
} VGPU_DEV;

//
// The Graphics Output Protocol wrapper structure.
//
#define VGPU_GOP_SIG SIGNATURE_64 ('V', 'G', 'P', 'U', '_', 'G', 'O', 'P')

struct VGPU_GOP_STRUCT {
  UINT64                               Signature;

  //
  // ParentBus points to the parent VGPU_DEV object. Never NULL.
  //
  VGPU_DEV                             *ParentBus;

  //
  // GopName carries a customized name for
  // EFI_COMPONENT_NAME2_PROTOCOL.GetControllerName(). It is expressed in table
  // form because it can theoretically support several languages. Never NULL.
  //
  EFI_UNICODE_STRING_TABLE             *GopName;

  //
  // GopHandle is the UEFI child handle that carries the device path ending
  // with the ACPI ADR node, and the Graphics Output Protocol. Never NULL.
  //
  EFI_HANDLE                           GopHandle;

  //
  // The GopDevicePath field is the device path installed on GopHandle,
  // ending with an ACPI ADR node. Never NULL.
  //
  EFI_DEVICE_PATH_PROTOCOL             *GopDevicePath;

  //
  // The Gop field is installed on the child handle as Graphics Output Protocol
  // interface.
  //
  // For now it is just a placeholder.
  //
  UINT8                                Gop;
};

//
// VirtIo GPU initialization, and commands (primitives) for the GPU device.
//
/**
  Configure the VirtIo GPU device that underlies VgpuDev.

  @param[in,out] VgpuDev  The VGPU_DEV object to set up VirtIo messaging for.
                          On input, the caller is responsible for having
                          initialized VgpuDev->VirtIo. On output, VgpuDev->Ring
                          has been initialized, and synchronous VirtIo GPU
                          commands (primitives) can be submitted to the device.

  @retval EFI_SUCCESS      VirtIo GPU configuration successful.

  @retval EFI_UNSUPPORTED  The host-side configuration of the VirtIo GPU is not
                           supported by this driver.

  @retval                  Error codes from underlying functions.
**/
EFI_STATUS
VirtioGpuInit (
  IN OUT VGPU_DEV *VgpuDev
  );

/**
  De-configure the VirtIo GPU device that underlies VgpuDev.

  @param[in,out] VgpuDev  The VGPU_DEV object to tear down VirtIo messaging
                          for. On input, the caller is responsible for having
                          called VirtioGpuInit(). On output, VgpuDev->Ring has
                          been uninitialized; VirtIo GPU commands (primitives)
                          can no longer be submitted to the device.
**/
VOID
VirtioGpuUninit (
  IN OUT VGPU_DEV *VgpuDev
  );

/**
  EFI_EVENT_NOTIFY function for the VGPU_DEV.ExitBoot event. It resets the
  VirtIo device, causing it to release its resources and to forget its
  configuration.

  This function may only be called (that is, VGPU_DEV.ExitBoot may only be
  signaled) after VirtioGpuInit() returns and before VirtioGpuUninit() is
  called.

  @param[in] Event    Event whose notification function is being invoked.

  @param[in] Context  Pointer to the associated VGPU_DEV object.
**/
VOID
EFIAPI
VirtioGpuExitBoot (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  The following functions send requests to the VirtIo GPU device model, await
  the answer from the host, and return a status. They share the following
  interface details:

  @param[in,out] VgpuDev  The VGPU_DEV object that represents the VirtIo GPU
                          device. The caller is responsible to have
                          successfully invoked VirtioGpuInit() on VgpuDev
                          previously, while VirtioGpuUninit() must not have
                          been called on VgpuDev.

  @retval EFI_INVALID_PARAMETER  Invalid command-specific parameters were
                                 detected by this driver.

  @retval EFI_SUCCESS            Operation successful.

  @retval EFI_DEVICE_ERROR       The host rejected the request. The host error
                                 code has been logged on the EFI_D_ERROR level.

  @return                        Codes for unexpected errors in VirtIo
                                 messaging.

  For the command-specific parameters, please consult the GPU Device section of
  the VirtIo 1.0 specification (see references in
  "OvmfPkg/Include/IndustryStandard/VirtioGpu.h").
**/
EFI_STATUS
VirtioGpuResourceCreate2d (
  IN OUT VGPU_DEV           *VgpuDev,
  IN     UINT32             ResourceId,
  IN     VIRTIO_GPU_FORMATS Format,
  IN     UINT32             Width,
  IN     UINT32             Height
  );

EFI_STATUS
VirtioGpuResourceUnref (
  IN OUT VGPU_DEV *VgpuDev,
  IN     UINT32   ResourceId
  );

EFI_STATUS
VirtioGpuResourceAttachBacking (
  IN OUT VGPU_DEV *VgpuDev,
  IN     UINT32   ResourceId,
  IN     VOID     *FirstBackingPage,
  IN     UINTN    NumberOfPages
  );

EFI_STATUS
VirtioGpuResourceDetachBacking (
  IN OUT VGPU_DEV *VgpuDev,
  IN     UINT32   ResourceId
  );

EFI_STATUS
VirtioGpuSetScanout (
  IN OUT VGPU_DEV *VgpuDev,
  IN     UINT32   X,
  IN     UINT32   Y,
  IN     UINT32   Width,
  IN     UINT32   Height,
  IN     UINT32   ScanoutId,
  IN     UINT32   ResourceId
  );

EFI_STATUS
VirtioGpuTransferToHost2d (
  IN OUT VGPU_DEV *VgpuDev,
  IN     UINT32   X,
  IN     UINT32   Y,
  IN     UINT32   Width,
  IN     UINT32   Height,
  IN     UINT64   Offset,
  IN     UINT32   ResourceId
  );

EFI_STATUS
VirtioGpuResourceFlush (
  IN OUT VGPU_DEV *VgpuDev,
  IN     UINT32   X,
  IN     UINT32   Y,
  IN     UINT32   Width,
  IN     UINT32   Height,
  IN     UINT32   ResourceId
  );

#endif // _VIRTIO_GPU_DXE_H_