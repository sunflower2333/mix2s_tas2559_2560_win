/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    peripheral.cpp

Abstract:

    This module contains the function for interaction
    with the SPB API.

Environment:

    kernel-mode only

Revision History:

--*/

#include "driver.h"
#include <spb.h>

#include "spb.tmh"
#define RESHUB_USE_HELPER_ROUTINES
#include <reshub.h>

NTSTATUS
SpbDeviceOpen(
    _In_ WDFDEVICE FxDevice,
    _In_ PSPB_CONTEXT SpbContext
)
{
    WDF_IO_TARGET_OPEN_PARAMS  openParams;
    NTSTATUS status;
    DECLARE_UNICODE_STRING_SIZE(DevicePath, RESOURCE_HUB_PATH_SIZE);
    RESOURCE_HUB_CREATE_PATH_FROM_ID(
        &DevicePath,
        SpbContext->I2cResHubId.LowPart,
        SpbContext->I2cResHubId.HighPart);


    WDF_OBJECT_ATTRIBUTES targetAttributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&targetAttributes);
    status = WdfIoTargetCreate(
        FxDevice,
        &targetAttributes,
        &SpbContext->SpbIoTarget);
    
    if (!NT_SUCCESS(status))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_SPBAPI,
            "Error creating IoTarget object - %!STATUS!",
            status);

        WdfObjectDelete(SpbContext->SpbIoTarget);
        goto exit;
    }

    //
    // Allocate a waitlock to guard access to the default buffers
    //
    status = WdfWaitLockCreate(
        WDF_NO_OBJECT_ATTRIBUTES,
        &SpbContext->SpbLock);

    //
    // Open a handle to the SPB controller.
    //

    WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(
        &openParams,
        &DevicePath,
        (GENERIC_READ | GENERIC_WRITE));

    openParams.ShareAccess = 0;
    openParams.CreateDisposition = FILE_OPEN;
    openParams.FileAttributes = FILE_ATTRIBUTE_NORMAL;

    status = WdfIoTargetOpen(
        SpbContext->SpbIoTarget,
        &openParams);


   
exit:
    if (!NT_SUCCESS(status))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_FLAG_SPBAPI,
            "Failed to Open SpbDevice - %!STATUS!",
            status);
        SpbDeviceClose(SpbContext);
    }

    TraceEvents(
        TRACE_LEVEL_ERROR,
        TRACE_FLAG_SPBAPI,
        "Successfully Opened SpbDevice - %!STATUS!",
        status);

    return status;
}

NTSTATUS
SpbDeviceClose(
    _In_  PSPB_CONTEXT  SpbContext
)
{
    // WdfInterruptDisable(pDevice->Interrupt);

    NTSTATUS status = STATUS_SUCCESS;
    UNREFERENCED_PARAMETER(SpbContext);
    //WdfIoTargetClose(SpbContext->SpbIoTarget);
    return status;
}

NTSTATUS
SpbDeviceWrite(
    _In_ SPB_CONTEXT* SpbContext,
    _In_ PVOID pInputBuffer,
    _In_ size_t inputBufferLength
)
{
    WDF_MEMORY_DESCRIPTOR  inMemoryDescriptor;
    ULONG_PTR  bytesWritten = (ULONG_PTR)NULL;
    NTSTATUS status;

    WdfWaitLockAcquire(SpbContext->SpbLock, NULL);
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inMemoryDescriptor,
        pInputBuffer,
        (ULONG)inputBufferLength);

    status = WdfIoTargetSendWriteSynchronously(
        SpbContext->SpbIoTarget,
        NULL,
        &inMemoryDescriptor,
        NULL,
        NULL,
        &bytesWritten
    );

    WdfWaitLockRelease(SpbContext->SpbLock);
    /*if (!NT_SUCCESS(status))
    {
       
    }
    */
    //TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! write.");
    return status;
}

NTSTATUS
SpbDeviceWriteRead(
    _In_ SPB_CONTEXT* SpbContext,
    _In_ PVOID pInputBuffer,
    _In_ PVOID pOutputBuffer,
    _In_ size_t inputBufferLength,
    _In_ size_t outputBufferLength
)
{
    WDF_MEMORY_DESCRIPTOR  inMemoryDescriptor;
    WDF_MEMORY_DESCRIPTOR  outMemoryDescriptor;
    ULONG_PTR  bytesWritten = (ULONG_PTR)NULL;
    ULONG_PTR  bytesRead = (ULONG_PTR)NULL;
    NTSTATUS status;

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inMemoryDescriptor,
        pInputBuffer,
        (ULONG)inputBufferLength);

    WdfWaitLockAcquire(SpbContext->SpbLock, NULL);
    status = WdfIoTargetSendWriteSynchronously(
        SpbContext->SpbIoTarget,
        NULL,
        &inMemoryDescriptor,
        NULL,
        NULL,
        &bytesWritten
    );

    if (!NT_SUCCESS(status))
    {
    }

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outMemoryDescriptor,
        pOutputBuffer,
        (ULONG)outputBufferLength);

    status = WdfIoTargetSendReadSynchronously(
        SpbContext->SpbIoTarget,
        NULL,
        &outMemoryDescriptor,
        NULL,
        NULL,
        &bytesRead
    );

    if (!NT_SUCCESS(status))
    {
    }
    WdfWaitLockRelease(SpbContext->SpbLock);
    //TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WriteRead.");
    return status;
}

NTSTATUS
SpbDeviceReadReg(
    _In_ SPB_CONTEXT* SpbContext,
    _In_ UCHAR Address,
    _In_reads_bytes_(Length) PVOID Data,
    _In_ ULONG Length
) {
    return SpbDeviceWriteRead(SpbContext, &Address, Data, 1, Length);
}

VOID
SpbWriteRead(
    PSPB_CONTEXT SpbContext,
    UINT8* i_data,
    UINT8* r_data,
    UINT16 i_len,
    UINT16 o_len,
    UINT8 dev
) {
    UINT8 o_data[121] = { 0 };
    SpbDeviceWriteRead(SpbContext, i_data, o_data, i_len, o_len);
    if (o_data[0] != r_data[0])
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Incorrect Reply Here: chl: %d: , o_data: 0x%04x r_data 0x%04x", dev, o_data[0], r_data[0]);
    //else
//        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Correct Reply: o:0x%04x, r:0x%04x", o_data[0], r_data[0]);

}