
#include <wtypes.h>
    extern "C" {
    // Declare the C libraries used
    #include "setupapi.h"
    #include "hidsdi.h"
    }
bool FindTheHID(void);
void GetDeviceCapabilities(void);
void PrepareForOverlappedTransfer(void);
void WriteOutputReport(char * outbuffer);
void DisplayInputReport(char * buffer);
void ReadInputReport( char * inbuffer);