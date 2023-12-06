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

DEFINE_GUID (GUID_DEVINTERFACE_TAS2559amp,
    0x32aa757a,0x4d2e,0x431e,0xa5,0xa2,0xff,0xaa,0x7f,0xe5,0xb1,0x5e);
// {32aa757a-4d2e-431e-a5a2-ffaa7fe5b15e}
