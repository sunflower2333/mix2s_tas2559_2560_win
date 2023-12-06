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
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    ULONG PrivateDeviceData;  // just a placeholder
    PCALLBACK_OBJECT        CSAudioAPICallback;
    PVOID                   CSAudioAPICallbackObj;
    BOOLEAN                 CSAudioManaged;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

typedef struct _FILTER_EXTENSION
{
    WDFDEVICE WdfDevice;
    // More context data here

}FILTER_EXTENSION, * PFILTER_EXTENSION;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FILTER_EXTENSION,
    FilterGetData)

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
AudFilterCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );


VOID
EvtIoStop(
    WDFQUEUE Queue,
    WDFREQUEST Request
    //,
    //ULONG ActionFlags
);

VOID
OnRequestCompletionRoutine(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  Context
);

VOID
OnDeviceControl(
    IN WDFQUEUE      Queue,
    IN WDFREQUEST    Request,
    IN size_t        OutputBufferLength,
    IN size_t        InputBufferLength,
    IN ULONG         IoControlCode
);

VOID
FilterForwardRequest(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target
);

VOID
FilterForwardRequestWithCompletionRoutine(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target
);

VOID
FilterRequestCompletionRoutine(
    IN WDFREQUEST                  Request,
    IN WDFIOTARGET                 Target,
    PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
    IN WDFCONTEXT                  Context
);

typedef enum {
    CSAudioEndpointTypeDSP,
    CSAudioEndpointTypeSpeaker,
    CSAudioEndpointTypeHeadphone,
    CSAudioEndpointTypeMicArray,
    CSAudioEndpointTypeMicJack
} CSAudioEndpointType;

typedef enum {
    CSAudioEndpointRegister,
    CSAudioEndpointStart,
    CSAudioEndpointStop,
    CSAudioEndpointOverrideFormat
} CSAudioEndpointRequest;

typedef struct CSAUDIOFORMATOVERRIDE {
    UINT16 channels;
    UINT16 frequency;
    UINT16 bitsPerSample;
    UINT16 validBitsPerSample;
    BOOLEAN force32BitOutputContainer;
} CsAudioFormatOverride;

typedef struct CSAUDIOARG {
    UINT32 argSz;
    CSAudioEndpointType endpointType;
    CSAudioEndpointRequest endpointRequest;
    union {
        CsAudioFormatOverride formatOverride;
    }CsUnion;
} CsAudioArg, * PCsAudioArg;

VOID
CsAudioCallbackFunction(
    PDEVICE_CONTEXT pDevice,
    CsAudioArg* arg,
    PVOID Argument2
);
EXTERN_C_END
