/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_AudFilter,
    0xfe3b60e3,0xf000,0x43ac,0xa4,0xfe,0xa8,0x4f,0x52,0xf7,0xdd,0x09);
// {fe3b60e3-f000-43ac-a4fe-a84f52f7dd09}
