/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, TAS2559ampEvtDeviceAdd)
#pragma alloc_text (PAGE, TAS2559ampEvtDriverContextCleanup)
#endif

NTSTATUS
OnPrepareHardware(
    _In_  WDFDEVICE     FxDevice,
    _In_  WDFCMRESLIST  FxResourcesRaw,
    _In_  WDFCMRESLIST  FxResourcesTranslated
)
/*++

    Routine Description:

    This routine caches the SPB resource connection ID.

    Arguments:

    FxDevice - a handle to the framework device object
    FxResourcesRaw - list of translated hardware resources that
        the PnP manager has assigned to the device
    FxResourcesTranslated - list of raw hardware resources that
        the PnP manager has assigned to the device

    Return Value:

    Status

--*/
{
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    BOOLEAN fSpbResourceFoundA = FALSE;
    BOOLEAN fSpbResourceFoundB = FALSE;
    BOOLEAN fInterruptResourceFoundA = FALSE;
    BOOLEAN fInterruptResourceFoundB = FALSE;
    ULONG interruptIndexA = 0;
    ULONG interruptIndexB = 0;
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(FxResourcesRaw);
    FuncEntry(TRACE_DRIVER);

    //
    // Parse the peripheral's resources.
    //

    ULONG resourceCount = WdfCmResourceListGetCount(FxResourcesTranslated);

    for (ULONG i = 0; i < resourceCount; i++)
    {
        PCM_PARTIAL_RESOURCE_DESCRIPTOR pDescriptor;
        UCHAR Class;
        UCHAR Type;

        pDescriptor = WdfCmResourceListGetDescriptor(
            FxResourcesTranslated, i);

        switch (pDescriptor->Type)
        {
        case CmResourceTypeConnection:

            //
            // Look for I2C or SPI resource and save connection ID.
            //

            Class = pDescriptor->u.Connection.Class;
            Type = pDescriptor->u.Connection.Type;

            if ((Class == CM_RESOURCE_CONNECTION_CLASS_SERIAL) &&
                ((Type == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)))
            {
                if (fSpbResourceFoundB == FALSE && fSpbResourceFoundA == TRUE)
                {
                    pDevice->SpbContextB.I2cResHubId.LowPart =
                        pDescriptor->u.Connection.IdLowPart;
                    pDevice->SpbContextB.I2cResHubId.HighPart =
                        pDescriptor->u.Connection.IdHighPart;
                    fSpbResourceFoundB = TRUE;
                }
            }
            if ((Class == CM_RESOURCE_CONNECTION_CLASS_SERIAL) &&
                ((Type == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)))
            {
                if (fSpbResourceFoundA == FALSE)
                {
                    pDevice->SpbContextA.I2cResHubId.LowPart =
                        pDescriptor->u.Connection.IdLowPart;
                    pDevice->SpbContextA.I2cResHubId.HighPart =
                        pDescriptor->u.Connection.IdHighPart;
                    fSpbResourceFoundA = TRUE;
                }
            }
            break;

        case CmResourceTypeInterrupt:
            if (fInterruptResourceFoundA == TRUE && fInterruptResourceFoundB == FALSE)
            {
                fInterruptResourceFoundB = TRUE;
                interruptIndexB = i;
            }

            if (fInterruptResourceFoundA == FALSE)
            {
                fInterruptResourceFoundA = TRUE;
                interruptIndexA = i;
            }
            break;

        default:

            //
            // Ignoring all other resource types.
            //

            break;
        }
    }

    //
    // An SPB resource is required.
    //

    if (fSpbResourceFoundA == FALSE || fSpbResourceFoundB == FALSE)
    {
        status = STATUS_NOT_FOUND;
    }

    //
    // Create the interrupt if an interrupt
    // resource was found.
    //

    if (NT_SUCCESS(status))
    {
        if (fInterruptResourceFoundA == TRUE && fInterruptResourceFoundB == TRUE)
        {
            /*
            WDF_INTERRUPT_CONFIG interruptConfigA;
            WDF_INTERRUPT_CONFIG_INIT(
                &interruptConfigA,
                OnInterruptIsrA,
                NULL);
            interruptConfigA.PassiveHandling = TRUE;
            interruptConfigA.InterruptTranslated = WdfCmResourceListGetDescriptor(
                FxResourcesTranslated,
                interruptIndexA);
            interruptConfigA.InterruptRaw = WdfCmResourceListGetDescriptor(
                FxResourcesRaw,
                interruptIndexA);
            status = WdfInterruptCreate(
                pDevice->Device,
                &interruptConfigA,
                WDF_NO_OBJECT_ATTRIBUTES,
                &pDevice->InterruptA);

            
            WDF_INTERRUPT_CONFIG interruptConfigB;
            WDF_INTERRUPT_CONFIG_INIT(
                &interruptConfigB,
                OnInterruptIsrB,
                NULL);
            interruptConfigB.PassiveHandling = TRUE;
            interruptConfigB.InterruptTranslated = WdfCmResourceListGetDescriptor(
                FxResourcesTranslated,
                interruptIndexB);
            interruptConfigB.InterruptRaw = WdfCmResourceListGetDescriptor(
                FxResourcesRaw,
                interruptIndexB);
            status = WdfInterruptCreate(
                pDevice->Device,
                &interruptConfigB,
                WDF_NO_OBJECT_ATTRIBUTES,
                &pDevice->InterruptB);
            */
        }
    }
    
    status = SpbDeviceOpen(FxDevice, &pDevice->SpbContextA);
    status = SpbDeviceOpen(FxDevice, &pDevice->SpbContextB);

    //enable interrupt
    //WdfInterruptEnable(pDevice->InterruptA);

    //enable interrupt
    //WdfInterruptEnable(pDevice->InterruptB);
    
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! Status: %!STATUS!", status);
    FuncExit(TRACE_DRIVER);
    return status;
}

BOOLEAN
OnInterruptIsrA(
    _In_  WDFINTERRUPT FxInterrupt,
    _In_  ULONG        MessageID
) {
    UNREFERENCED_PARAMETER(FxInterrupt);
    UNREFERENCED_PARAMETER(MessageID);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! Enter ISR A !");


    BOOLEAN fInterruptRecognized = TRUE;
    return fInterruptRecognized;
}

BOOLEAN
OnInterruptIsrB(
    _In_  WDFINTERRUPT FxInterrupt,
    _In_  ULONG        MessageID
) {
    UNREFERENCED_PARAMETER(FxInterrupt);
    UNREFERENCED_PARAMETER(MessageID);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! Enter ISR B !");


    BOOLEAN fInterruptRecognized = TRUE;
    return fInterruptRecognized;
}

NTSTATUS
OnReleaseHardware(
    _In_  WDFDEVICE     FxDevice,
    _In_  WDFCMRESLIST  FxResourcesTranslated
)
/*++

    Routine Description:

    Arguments:

    FxDevice - a handle to the framework device object
    FxResourcesTranslated - list of raw hardware resources that
        the PnP manager has assigned to the device

    Return Value:

    Status

--*/
{
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(FxResourcesTranslated);
    FuncEntry(TRACE_DRIVER);

    //if (pDevice->Interrupt != NULL)
    //{
    //    WdfObjectDelete(pDevice->Interrupt);
    //}
    if (pDevice->CSAudioAPICallbackObj) {
        ExUnregisterCallback(pDevice->CSAudioAPICallbackObj);
        pDevice->CSAudioAPICallbackObj = NULL;
    }

    if (pDevice->CSAudioAPICallback) {
        ObfDereferenceObject(pDevice->CSAudioAPICallback);
        pDevice->CSAudioAPICallback = NULL;
    }

    SpbDeviceClose(&pDevice->SpbContextA);
    SpbDeviceClose(&pDevice->SpbContextB);
    FuncExit(TRACE_DRIVER);

    return status;
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    //
    // Initialize WPP Tracing
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = TAS2559ampEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config,
                           TAS2559ampEvtDeviceAdd
                           );

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE
                             );

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        WPP_CLEANUP(DriverObject);
        return status;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

NTSTATUS
TAS2559ampEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = TAS2559ampCreateDevice(DeviceInit);


    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

VOID
TAS2559ampEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
/*++
Routine Description:

    Free all the resources allocated in DriverEntry.

Arguments:

    DriverObject - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Stop WPP Tracing
    //
    WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)DriverObject));
}
