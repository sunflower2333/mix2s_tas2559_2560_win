/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include "public.h"

EXTERN_C_START
//
// SPB (I2C) context
//

typedef struct _SPB_CONTEXT
{
    WDFIOTARGET SpbIoTarget;
    LARGE_INTEGER I2cResHubId;
    WDFMEMORY WriteMemory;
    WDFMEMORY ReadMemory;
    WDFWAITLOCK SpbLock;
    WDFREQUEST SpbRequest;
    WDFMEMORY InputMemory;
} SPB_CONTEXT, *PSPB_CONTEXT;
//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    WDFDEVICE               Device;
    WDFINTERRUPT            InterruptA;
    WDFINTERRUPT            InterruptB;
    SPB_CONTEXT             SpbContextA;
    SPB_CONTEXT             SpbContextB;
    WDFREQUEST              ClientRequest;
    PCALLBACK_OBJECT        CSAudioAPICallback;
    PVOID                   CSAudioAPICallbackObj;
    BOOLEAN                 CSAudioManaged;
    WDFREQUEST              WaitOnInterruptRequest;
    KEVENT                  IsrWaitEvent;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

typedef struct _REQUEST_CONTEXT
{
    //
    // Associated framework device object
    //

    WDFDEVICE FxDevice;

    //
    // Variables to track write length for a sequence request.
    // There are needed to complete the client request with
    // correct bytesReturned value.
    //

    BOOLEAN IsSpbSequenceRequest;
    ULONG_PTR SequenceWriteLength;
} REQUEST_CONTEXT, * PREQUEST_CONTEXT;


//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, RequestGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
TAS2559ampCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DEVICE_D0_ENTRY OnD0Entry;

EVT_WDF_DEVICE_D0_EXIT OnD0Exit;

EVT_WDF_INTERRUPT_ISR OnInterruptIsrA;
EVT_WDF_INTERRUPT_ISR OnInterruptIsrB;

EVT_WDF_DEVICE_PREPARE_HARDWARE OnPrepareHardware;

EVT_WDF_DEVICE_RELEASE_HARDWARE OnReleaseHardware;
EXTERN_C_END
