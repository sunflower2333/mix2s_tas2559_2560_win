/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include <ntddk.h>
#include <wdm.h>
#include <wdf.h>
#include <initguid.h>
#include <stddef.h>
#include <stdarg.h>

#include "device.h"
#include "queue.h"
#include "trace.h"
#include "spb.h"
#include "firmware.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD TAS2559ampEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP TAS2559ampEvtDriverContextCleanup;

BOOLEAN
OnInterruptIsrA(
	_In_  WDFINTERRUPT FxInterrupt,
	_In_  ULONG        MessageID
);

BOOLEAN
OnInterruptIsrB(
	_In_  WDFINTERRUPT FxInterrupt,
	_In_  ULONG        MessageID
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
} CsAudioArg, *PCsAudioArg;


NTSTATUS
SetupfwAmp(PDEVICE_CONTEXT pDevice);

NTSTATUS
StopAmp(PDEVICE_CONTEXT pDevice);

#define AMP_POOL_TAG    (ULONG) '2SAT'
EXTERN_C_END
