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
#pragma alloc_text (PAGE, AudFilterCreateDevice)
#endif
PDEVICE_CONTEXT g_pDevice;

NTSTATUS
AudFilterCreateDevice(
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
    WDFDEVICE device;
    WDF_IO_QUEUE_CONFIG queueConfig;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    PAGED_CODE();
    //
    // Tell the framework that you are filter driver. Framework
    // takes care of inherting all the device flags & characterstics
    // from the lower device you are attaching to.
    //
    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
    status = WdfDeviceCreate(
        &DeviceInit,
        &deviceAttributes,
        &device);
    
    g_pDevice = DeviceGetContext(device);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "WdfDeviceCreate failed - 0x%08lX",
            status);

        goto exit;
    }

    // CS Audio Callback
    UNICODE_STRING CSAudioCallbackAPI;
    RtlInitUnicodeString(&CSAudioCallbackAPI, L"\\CallBack\\CsAudioCallbackAPI");


    OBJECT_ATTRIBUTES attributes;
    InitializeObjectAttributes(&attributes,
        &CSAudioCallbackAPI,
        OBJ_KERNEL_HANDLE | OBJ_OPENIF | OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
        NULL,
        NULL
    );

    status = ExCreateCallback(&g_pDevice->CSAudioAPICallback, &attributes, TRUE, TRUE);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    g_pDevice->CSAudioAPICallbackObj = ExRegisterCallback(g_pDevice->CSAudioAPICallback,
        CsAudioCallbackFunction,
        g_pDevice
    );

    if (!g_pDevice->CSAudioAPICallbackObj) {
        return STATUS_NO_CALLBACK_ACTIVE;
    }

    //
    // Create a parallel dispatch queue to handle requests from HID Class
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
        &queueConfig,
        WdfIoQueueDispatchParallel);

    queueConfig.EvtIoDeviceControl = OnDeviceControl;
    queueConfig.EvtIoResume = EvtIoStop;

    status = WdfIoQueueCreate(
        device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        WDF_NO_HANDLE); // pointer to default queue

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "Error creating WDF default queue - 0x%08lX",
            status);

        goto exit;
    }

    TraceEvents(
        TRACE_LEVEL_ERROR,
        TRACE_DEVICE,
        "AudFilterCreateDeviceExit: %!STATUS!\n", status);

exit:
    return status;
}
int CsAudioArg2 = 1;

VOID
CsAudioCallbackFunction(
    PDEVICE_CONTEXT pDevice,
    CsAudioArg* arg,
    PVOID Argument2
) {/* We do nothing here */
    UNREFERENCED_PARAMETER(pDevice);
    UNREFERENCED_PARAMETER(Argument2);
    UNREFERENCED_PARAMETER(arg);
}

VOID
CSAudioNotifyEndpoint(
    CSAudioEndpointRequest RequestAction
) {
    //TraceEvents(
    //    TRACE_LEVEL_ERROR,
    //    TRACE_DEVICE,
    //    "%!FUNC! Enter");
    CsAudioArg arg;
    RtlZeroMemory(&arg, sizeof(CsAudioArg));
    arg.argSz = sizeof(CsAudioArg);
    arg.endpointType = CSAudioEndpointTypeSpeaker;
    arg.endpointRequest = RequestAction;
    ExNotifyCallback(g_pDevice->CSAudioAPICallback, &arg, &CsAudioArg2);
}

VOID
OnDeviceControl(
    IN WDFQUEUE      Queue,
    IN WDFREQUEST    Request,
    IN size_t        OutputBufferLength,
    IN size_t        InputBufferLength,
    IN ULONG         IoControlCode
)
/*++

Routine Description:

    This routine is the dispatch routine for internal device control requests.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.

Return Value:

   VOID

--*/
{
    NTSTATUS status;
    WDFDEVICE device;
    BOOLEAN CheckAmpStatus = FALSE;
    BOOLEAN requestSent = TRUE;
    WDF_REQUEST_SEND_OPTIONS options;
    //WDFMEMORY memory;
    WDFIOTARGET Target;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    //UNREFERENCED_PARAMETER(InputBufferLength);

    PAGED_CODE();

    device = WdfIoQueueGetDevice(Queue);

    Target = WdfDeviceGetIoTarget(device);
    
    switch(IoControlCode){
        case 0x2f0003:
            if (InputBufferLength == 0x18)
                CheckAmpStatus = TRUE;
    }
    //
    // Forward the request down. WdfDeviceGetIoTarget returns
    // the default target, which represents the device attached to us below in
    // the stack.
    //
    if (CheckAmpStatus) {
        // Signal Callback func in TAS driver.
        CSAudioNotifyEndpoint(CSAudioEndpointStart);
    }

    //
    // We are not interested in processing the IRP so
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(
        &options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    requestSent = WdfRequestSend(Request, Target, &options);

    if (requestSent == FALSE) {
        status = WdfRequestGetStatus(Request);

        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "WdfRequestSend failed: 0x%x\n",
            status);

        WdfRequestComplete(Request, status);
    }

    return;
}


VOID
EvtIoStop(
    WDFQUEUE Queue,
    WDFREQUEST Request
    //,
    //ULONG ActionFlags
)
/*++

Routine Description:

    This routine is the dispatch routine for internal device control requests.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.

Return Value:

   VOID

--*/
{
    NTSTATUS status;
    WDFDEVICE device;
    //BOOLEAN forwardWithCompletionRoutine = FALSE;
    BOOLEAN requestSent = TRUE;
    WDF_REQUEST_SEND_OPTIONS options;
    //WDFMEMORY memory;
    WDFIOTARGET Target;

   // UNREFERENCED_PARAMETER(ActionFlags);

    PAGED_CODE();

    device = WdfIoQueueGetDevice(Queue);
    Target = WdfDeviceGetIoTarget(device);

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    TraceEvents(
        TRACE_LEVEL_ERROR,
        TRACE_DEVICE,
        "EvtWdfIoQueueIoStop Enter");

    //   Mute Device Here.
    CSAudioNotifyEndpoint(CSAudioEndpointStop);

    //
    // We are not interested in post processing the IRP so
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(
        &options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    requestSent = WdfRequestSend(Request, Target, &options);

    if (requestSent == FALSE) {
        status = WdfRequestGetStatus(Request);

        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "WdfRequestSend failed: 0x%x\n",
            status);

        WdfRequestComplete(Request, status);
    }

    return;
}


VOID
OnRequestCompletionRoutine(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  Context
)
/*++

Routine Description:

    Completion Routine

Arguments:

    Target - Target handle
    Request - Request handle
    Params - request completion params
    Context - Driver supplied context


Return Value:

    VOID

--*/
{
    NTSTATUS    status = Params->IoStatus.Status;
    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Context);
    
//exit:
    WdfRequestComplete(Request, status);
    return;
}


/*++
Routine Description:

    Passes a request on to the lower driver.

Arguments:

    Request - The request to pass on to the lower driver

    Target  - The lower driver to pass the request to

Return Value:

    VOID

--*/
VOID
FilterForwardRequest(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET Target
)
{
    WDF_REQUEST_SEND_OPTIONS Options;
    BOOLEAN                  RequestSent;
    NTSTATUS                 Status;

    //
    // We are not interested in post processing the IRP so 
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&Options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    RequestSent = WdfRequestSend(Request, Target, &Options);

    if (RequestSent == FALSE) {
        Status = WdfRequestGetStatus(Request);
        KdPrint(("WdfRequestSend failed: 0x%x\n", Status));
        WdfRequestComplete(Request, Status);
    }

    return;
}