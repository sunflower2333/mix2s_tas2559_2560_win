/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, TAS2559ampCreateDevice)
#endif

UINT8 g_MuteRegister = 0x07;
UINT8 g_DeviceMute = 3; // 3: mute, 0: unmute
UINT8 g_LastMuteValue = 3;
BOOLEAN
CheckMuteStatus(
    PDEVICE_CONTEXT pDevice
) {
    PAGED_CODE();
    SpbDeviceWriteRead(&pDevice->SpbContextA, &g_MuteRegister, &g_DeviceMute, sizeof(g_MuteRegister), sizeof(g_DeviceMute));
    return g_DeviceMute;
}

VOID
ShutdownAmp(PDEVICE_CONTEXT pDevice) {
    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Enter!");
    // delay Detected
    DELAY_MS(1)
    //WA(0x2c, 0x00);
    //WB(0x21, 0x00);
    WA(0x00, 0x00);
    WA(0x7f, 0x64);
    WA(0x07, 0x01);
    WB(0x07, 0x41);
    WB(0x07, 0x01);
    WA(0x00, 0x00);
    WA(0x7f, 0x00);
    WA(0x04, 0x60);
    // delay Detected
    DELAY_MS(2)
    WA(0x07, 0x03);
    WA(0x05, 0x00);
    WA(0x04, 0x00);
    WB(0x02, 0x01);
    WA(0x00, 0x01);
    WA(0x3d, 0x00);
    WA(0x3e, 0x00);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Exit!");
}

VOID
StartupAmp(PDEVICE_CONTEXT pDevice) {
    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Enter!");
    //if (g_LastMuteValue != g_DeviceMute){
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! startup current mute reg: %d, last mute reg: %d", g_LastMuteValue, g_DeviceMute);
        // delay Detected
        //WA(0x00, 0x00);
        //WA(0x7f, 0x64);
        
        // There is no reg marked spkr is Muted(or i can not found)
        // these regs must be rewrite when power up I2S and ClassD Amp again.
        // (Although nothing changes in them).
        WA(0x00, 0x00);
        WA(0x7f, 0x00);
        WA(0x00, 0x01);
        WA(0x3d, 0x01);
        WA(0x3e, 0x01);
        WA(0x00, 0x00);
        WA(0x05, 0xa3);
        WA(0x04, 0xf8);
        //// delay Detected
        //DELAY_MS(2)
        //WB(0x02, 0x02);
        WB(0x07, 0x40);
        //// delay Detected
        //DELAY_MS(2)
        //    WA(0x2c, 0x2b);
        //WB(0x21, 0x0b);



        //WB(0x00, 0x32);
        //WB(0x6c, 0x00, 0x00, 0x00, 0x00);
        //WB(0x28, 0x7f, 0xfb, 0xb5, 0x00);
        //WB(0x2c, 0x80, 0x04, 0x4c, 0x00);
        //WB(0x30, 0x7f, 0xf7, 0x6a, 0x00);
        //WB(0x1c, 0x7f, 0xff, 0xff, 0x00);
        //WB(0x20, 0x00, 0x00, 0x00, 0x00);
        //WB(0x24, 0x00, 0x00, 0x00, 0x00);
        //WB(0x00, 0x33);
        //WB(0x18, 0x04, 0xcc, 0xcc, 0x00);
        //WB(0x00, 0x34);
        //WB(0x34, 0x3a, 0x46, 0x74, 0x00);
        //WB(0x38, 0x22, 0xf3, 0x07, 0x00);
        //WB(0x3c, 0x80, 0x77, 0x61, 0x00);
        //WB(0x40, 0x22, 0xa7, 0xcc, 0x00);
        //WB(0x44, 0x3a, 0x0c, 0x93, 0x00);
        //WB(0x00, 0x32);
        //WB(0x3c, 0x3d, 0x99, 0x99, 0x00, 0x30, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00);
        //WB(0x4c, 0x02, 0x00, 0x00, 0x00);
        //WB(0x54, 0x00, 0x88, 0x3f, 0x00, 0x00, 0x06, 0xd3, 0x00);
        //WB(0x60, 0x28, 0x00, 0x00, 0x00);
        //WB(0x00, 0x33);
        //WB(0x40, 0x00, 0x00, 0x00, 0x00);
        //WB(0x64, 0x39, 0x80, 0x00, 0x00);
        //WB(0x10, 0x73, 0x33, 0x33, 0x00);
        //WB(0x14, 0x6b, 0x85, 0x1e, 0x00);
        //WB(0x20, 0x1d, 0x99, 0x99, 0x00);
        //WB(0x24, 0x20, 0xcc, 0xcc, 0x00);
        //WB(0x28, 0x24, 0x00, 0x00, 0x00);
        //WB(0x2c, 0x27, 0x33, 0x33, 0x00);
        //WB(0x30, 0x2a, 0x66, 0x66, 0x00);
        //WB(0x34, 0x2d, 0x99, 0x99, 0x00);
        //WB(0x38, 0x30, 0xcc, 0xcc, 0x00);
        //WB(0x3c, 0x34, 0x00, 0x00, 0x00);
        //WB(0x00, 0x32);
        //WB(0x0c, 0x65, 0xac, 0x8c, 0x00);

        //WA(0x7f, 0x00);
        //WA(0x00, 0x00);

        //WA(0x07, 0x00);
        //WB(0x00, 0x00);
        //WB(0x07, 0x40);
        //WA(0x00, 0x00);
        //WA(0x7f, 0x64);
        //WA(0x07, 0x00);
        //WA(0x00, 0x00);
        //WA(0x7f, 0x82);
        //WA(0x00, 0x02);


     //   g_LastMuteValue = g_DeviceMute;
    //}else
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! ignore! mute: %d", g_DeviceMute);
}

int CsAudioArg2 = 1;

VOID
CSAudioRegisterEndpoint(
    PDEVICE_CONTEXT pDevice
) {
    CsAudioArg arg;
    RtlZeroMemory(&arg, sizeof(CsAudioArg));
    arg.argSz = sizeof(CsAudioArg);
    arg.endpointType = CSAudioEndpointTypeSpeaker;
    arg.endpointRequest = CSAudioEndpointRegister;
    ExNotifyCallback(pDevice->CSAudioAPICallback, &arg, &CsAudioArg2);
}

VOID
CsAudioCallbackFunction(
    IN PDEVICE_CONTEXT pDevice,
    CsAudioArg* arg,
    PVOID Argument2
) {
    //TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "CsAudioCallbackFunction Enter.");
    if (!pDevice) {
        return;
    }

    if (Argument2 == &CsAudioArg2) {
        return;
    }

    pDevice->CSAudioManaged = TRUE;

    CsAudioArg localArg;
    RtlZeroMemory(&localArg, sizeof(CsAudioArg));
    RtlCopyMemory(&localArg, arg, min(arg->argSz, sizeof(CsAudioArg)));

    if (localArg.endpointType == CSAudioEndpointTypeDSP && localArg.endpointRequest == CSAudioEndpointRegister) {
        CSAudioRegisterEndpoint(pDevice);
    }
    else if (localArg.endpointType != CSAudioEndpointTypeSpeaker) {
        return;
    }

    if (localArg.endpointRequest == CSAudioEndpointStop) {
        ShutdownAmp(pDevice);
    }
    if (localArg.endpointRequest == CSAudioEndpointStart && !CheckMuteStatus(pDevice)) {
        StartupAmp(pDevice);
    }
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "CsAudioCallbackFunction Exit.");
}

NTSTATUS
StopAmp(PDEVICE_CONTEXT pDevice) {
    //
    // Mute Amplifier
    //
    NTSTATUS status = STATUS_SUCCESS;

    // TODO
    // mute
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "Muted Device! Status: %!STATUS!", status);
    ShutdownAmp(pDevice);
    
    // Stop Spb Controller
    //
    //WdfInterruptDisable(pDevice->Interrupt);
    //WdfIoTargetClose(pDevice->SpbContextA.SpbIoTarget);
    //WdfIoTargetClose(pDevice->SpbContextB.SpbIoTarget);
    

    return status;
}

NTSTATUS
OnD0Entry(
    _In_  WDFDEVICE               FxDevice,
    _In_  WDF_POWER_DEVICE_STATE  FxPreviousState
)
/*++

Routine Description:

This routine allocates objects needed by the driver.

Arguments:

FxDevice - a handle to the framework device object
FxPreviousState - previous power state

Return Value:

Status

--*/
{
    UNREFERENCED_PARAMETER(FxPreviousState);
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    //
    // Setup Frimware at the first time.
    //
    SetupfwAmp(pDevice);
    CalAmp1(pDevice);
    CalAmp2(pDevice);
    
    // Go back to B0 P0
    WA(0x00, 0x00);
    WA(0x7f, 0x00);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
OnD0Exit(
    _In_  WDFDEVICE               FxDevice,
    _In_  WDF_POWER_DEVICE_STATE  FxPreviousState
)
/*++

Routine Description:

This routine destroys objects needed by the driver.

Arguments:

FxDevice - a handle to the framework device object
FxPreviousState - previous power state

Return Value:

Status

--*/
{
    UNREFERENCED_PARAMETER(FxPreviousState);

    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");
    //if (pDevice->CSAudioAPICallbackObj) {
    //    ExUnregisterCallback(pDevice->CSAudioAPICallbackObj);
    //}

    if (pDevice->SpbContextA.SpbIoTarget != WDF_NO_HANDLE && pDevice->SpbContextB.SpbIoTarget != WDF_NO_HANDLE)
    {
        StopAmp(pDevice);
        WdfObjectDelete(pDevice->SpbContextA.SpbIoTarget);
        WdfObjectDelete(pDevice->SpbContextB.SpbIoTarget);
        pDevice->SpbContextA.SpbIoTarget = WDF_NO_HANDLE;
        pDevice->SpbContextB.SpbIoTarget = WDF_NO_HANDLE;
    }


    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
OnSelfManagedIoInit(
    _In_
    WDFDEVICE FxDevice
) {
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    // CS Audio Callback
    FuncEntry(TRACE_DEVICE);
    UNICODE_STRING CSAudioCallbackAPI;
    RtlInitUnicodeString(&CSAudioCallbackAPI, L"\\CallBack\\CsAudioCallbackAPI");


    OBJECT_ATTRIBUTES attributes;
    InitializeObjectAttributes(&attributes,
        &CSAudioCallbackAPI,
        OBJ_KERNEL_HANDLE | OBJ_OPENIF | OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
        NULL,
        NULL
    );

    status = ExCreateCallback(&pDevice->CSAudioAPICallback, &attributes, TRUE, TRUE);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    pDevice->CSAudioAPICallbackObj = ExRegisterCallback(pDevice->CSAudioAPICallback,
        CsAudioCallbackFunction,
        pDevice
    );

    if (!pDevice->CSAudioAPICallbackObj) {
        return STATUS_NO_CALLBACK_ACTIVE;
    }

    CSAudioRegisterEndpoint(pDevice);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");


    return status;
}

NTSTATUS
TAS2559ampCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "Registering PnpCallbacks.");
        WDF_PNPPOWER_EVENT_CALLBACKS pnpCallbacks;
        WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpCallbacks);
        pnpCallbacks.EvtDevicePrepareHardware = OnPrepareHardware;
        pnpCallbacks.EvtDeviceReleaseHardware = OnReleaseHardware;
        pnpCallbacks.EvtDeviceSelfManagedIoInit = OnSelfManagedIoInit;
        pnpCallbacks.EvtDeviceD0Entry = OnD0Entry;
        pnpCallbacks.EvtDeviceD0Exit = OnD0Exit;
        WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpCallbacks);
    }


    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status)) {
        //
        // Get a pointer to the device context structure that we just associated
        // with the device object. We define this structure in the device.h
        // header file. DeviceGetContext is an inline function generated by
        // using the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in device.h.
        // This function will do the type checking and return the device context.
        // If you pass a wrong object handle it will return NULL and assert if
        // run under framework verifier mode.
        //

        {
            WDF_DEVICE_STATE deviceState;
            WDF_DEVICE_STATE_INIT(&deviceState);

            deviceState.NotDisableable = WdfFalse;
            WdfDeviceSetDeviceState(device, &deviceState);
        }

        deviceContext = DeviceGetContext(device);

        //
        // Initialize the context.
        //
        //deviceContext->PrivateDeviceData = 0;

        //
        // Create a device interface so that applications can find and talk
        // to us.
        //
        status = WdfDeviceCreateDeviceInterface(
            device,
            &GUID_DEVINTERFACE_TAS2559amp,
            NULL // ReferenceString
            );

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = TAS2559ampQueueInitialize(device);
        }
    }
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}
