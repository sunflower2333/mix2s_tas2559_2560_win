/*++
    Copyright (c) Microsoft Corporation. All Rights Reserved.
    Sample code. Dealpoint ID #843729.

    Module Name:

        spb.h

    Abstract:

        This module contains the touch driver I2C helper definitions.

    Environment:

        Kernel Mode

    Revision History:

--*/

#pragma once
EXTERN_C_START
#include <wdm.h>
#include <wdf.h>
#define DEFAULT_SPB_BUFFER_SIZE 64

NTSTATUS
SpbDeviceReadReg(
    _In_ SPB_CONTEXT* SpbContext,
    _In_ UCHAR Address,
    _In_reads_bytes_(Length) PVOID Data,
    _In_ ULONG Length
);

NTSTATUS
SpbDeviceWrite(
    _In_ SPB_CONTEXT* SpbContext,
    _In_ PVOID pInputBuffer,
    _In_ size_t inputBufferLength
);

NTSTATUS
SpbDeviceOpen(
    _In_ WDFDEVICE FxDevice,
    _In_ PSPB_CONTEXT SpbContext
);

NTSTATUS
SpbDeviceClose(
    _In_  PSPB_CONTEXT  SpbContext
);

NTSTATUS
SpbDeviceWriteRead(
    _In_ SPB_CONTEXT* SpbContext,
    _In_ PVOID pInputBuffer,
    _In_ PVOID pOutputBuffer,
    _In_ size_t inputBufferLength,
    _In_ size_t outputBufferLength
);
EXTERN_C_END
