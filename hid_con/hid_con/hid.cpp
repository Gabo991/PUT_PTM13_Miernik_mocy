#include "stdafx.h"
#include "hid.h"
#include <wtypes.h>

#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"hid.lib")

    extern "C" {
    // Declare the C libraries used
    #include "setupapi.h"
    #include "hidsdi.h"
    }

//#include <atlstr.h>
#include <iostream>
using namespace std;

	DWORD								ActualBytesRead;
	DWORD								BytesRead;
	HIDP_CAPS							Capabilities;
	DWORD								cbBytesRead;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData;
	HANDLE								DeviceHandle;
	DWORD								dwError;
	char								FeatureReport[256];
	HANDLE								hEventObject;
	HANDLE								hDevInfo;
	GUID								HidGuid;
	OVERLAPPED							HIDOverlapped;
	char								InputReport[256];
	ULONG								Length;
	LPOVERLAPPED						lpOverLap;
	bool								MyDeviceDetected = FALSE; 
	char								MyDevicePathName[50];
	DWORD								NumberOfBytesRead;
	char								OutputReport[256];
	HANDLE								ReadHandle;
	DWORD								ReportType;
	ULONG								Required;
	char								ValueToDisplay[50];
	HANDLE								WriteHandle;

	int VendorID = 0x0477;
	int ProductID = 0x5620;

bool FindTheHID()
{
	//Use a series of API calls to find a HID with a specified Vendor IF and Product ID.

	HIDD_ATTRIBUTES						Attributes;
	DWORD								DeviceUsage;
	SP_DEVICE_INTERFACE_DATA			devInfoData;
	bool								LastDevice = FALSE;
	int									MemberIndex = 0;
	LONG								Result;	
	char								UsageDescription[50];

	Length = 0;
	detailData = NULL;
	DeviceHandle=NULL;

	/*
	API function: HidD_GetHidGuid
	Get the GUID for all system HIDs.
	Returns: the GUID in HidGuid.
	*/

	HidD_GetHidGuid(&HidGuid);	
	
	/*
	API function: SetupDiGetClassDevs
	Returns: a handle to a device information set for all installed devices.
	Requires: the GUID returned by GetHidGuid.
	*/
	
	hDevInfo=SetupDiGetClassDevs 
		(&HidGuid, 
		NULL, 
		NULL, 
		DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
		
	devInfoData.cbSize = sizeof(devInfoData);

	//Step through the available devices looking for the one we want. 
	//Quit on detecting the desired device or checking all available devices without success.

	MemberIndex = 0;
	LastDevice = FALSE;

	do
	{
		/*
		API function: SetupDiEnumDeviceInterfaces
		On return, MyDeviceInterfaceData contains the handle to a
		SP_DEVICE_INTERFACE_DATA structure for a detected device.
		Requires:
		The DeviceInfoSet returned in SetupDiGetClassDevs.
		The HidGuid returned in GetHidGuid.
		An index to specify a device.
		*/

		Result=SetupDiEnumDeviceInterfaces 
			(hDevInfo, 
			0, 
			&HidGuid, 
			MemberIndex, 
			&devInfoData);

		if (Result != 0)
		{
			//A device has been detected, so get more information about it.

			/*
			API function: SetupDiGetDeviceInterfaceDetail
			Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
			containing information about a device.
			To retrieve the information, call this function twice.
			The first time returns the size of the structure in Length.
			The second time returns a pointer to the data in DeviceInfoSet.
			Requires:
			A DeviceInfoSet returned by SetupDiGetClassDevs
			The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
			
			The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
			This application doesn't retrieve or use the structure.			
			If retrieving the structure, set 
			MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
			and pass the structure's address.
			*/
			
			//Get the Length value.
			//The call will return with a "buffer too small" error which can be ignored.

			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				NULL, 
				0, 
				&Length, 
				NULL);

			//Allocate memory for the hDevInfo structure, using the returned Length.

			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			
			//Set cbSize in the detailData structure.

			detailData -> cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			//Call the function again, this time passing it the returned buffer size.

			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				detailData, 
				Length, 
				&Required, 
				NULL);

			// Open a handle to the device.
			// To enable retrieving information about a system mouse or keyboard,
			// don't request Read or Write access for this handle.

			/*
			API function: CreateFile
			Returns: a handle that enables reading and writing to the device.
			Requires:
			The DevicePath in the detailData structure
			returned by SetupDiGetDeviceInterfaceDetail.
			*/

			DeviceHandle=CreateFile 
				(detailData->DevicePath, 
				0, 
				FILE_SHARE_READ|FILE_SHARE_WRITE, 
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, 
				0, 
				NULL);

			cout << "CreateFile: " << endl;

			/*
			API function: HidD_GetAttributes
			Requests information from the device.
			Requires: the handle returned by CreateFile.
			Returns: a HIDD_ATTRIBUTES structure containing
			the Vendor ID, Product ID, and Product Version Number.
			Use this information to decide if the detected device is
			the one we're looking for.
			*/

			//Set the Size to the number of bytes in the structure.

			Attributes.Size = sizeof(Attributes);

			Result = HidD_GetAttributes 
				(DeviceHandle, 
				&Attributes);
			
			cout << "HidD_GetAttributes: " << endl;
			
			//Is it the desired device?

			MyDeviceDetected = FALSE;
			

			if (Attributes.VendorID == VendorID)
			{
				if (Attributes.ProductID == ProductID)
				{
					//Both the Vendor ID and Product ID match.

					MyDeviceDetected = TRUE;
//					MyDevicePathName = detailData->DevicePath;
					cout << "Device detected" <<endl;

					//Register to receive device notifications.

				//	RegisterForDeviceNotifications();

					//Get the device's capablities.

					GetDeviceCapabilities();

					// Find out if the device is a system mouse or keyboard.
					
					DeviceUsage = (Capabilities.UsagePage * 256) + Capabilities.Usage;

					if (DeviceUsage == 0x102)
						{
						strcpy_s(UsageDescription,"mouse");
						}
				
					if (DeviceUsage == 0x106)
						{
						strcpy_s(UsageDescription,"keyboard");
						}

					if ((DeviceUsage == 0x102) | (DeviceUsage == 0x106)) 
						{
						cout << "*************************" << endl;
						cout << "The device is a system " << UsageDescription << "." << endl;
						cout << "Windows 2000 and Windows XP don't allow applications" << endl;
						cout << "to directly request Input reports from or " << endl; 
						cout << "write Output reports to these devices." << endl;
						cout << "*************************" << endl;
						}

					// Get a handle for writing Output reports.

					WriteHandle=CreateFile 
						(detailData->DevicePath, 
						GENERIC_WRITE, 
						FILE_SHARE_READ|FILE_SHARE_WRITE, 
						(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING, 
						0, 
						NULL);

					cout << "CreateFile: " << endl;

					// Prepare to read reports using Overlapped I/O.

					PrepareForOverlappedTransfer();

				} //if (Attributes.ProductID == ProductID)

				else
					//The Product ID doesn't match.

					CloseHandle(DeviceHandle);

			} //if (Attributes.VendorID == VendorID)

			else
				//The Vendor ID doesn't match.

				CloseHandle(DeviceHandle);

		//Free the memory used by the detailData structure (no longer needed).

		free(detailData);

		}  //if (Result != 0)

		else
			//SetupDiEnumDeviceInterfaces returned 0, so there are no more devices to check.

			LastDevice=TRUE;

		//If we haven't found the device yet, and haven't tried every available device,
		//try the next one.

		MemberIndex = MemberIndex + 1;

	} //do

	while ((LastDevice == FALSE) && (MyDeviceDetected == FALSE));

	if (MyDeviceDetected == FALSE)
		cout << "Device not detected" << endl;
	else
		cout << "Device detected" << endl;

	//Free the memory reserved for hDevInfo by SetupDiClassDevs.

	SetupDiDestroyDeviceInfoList(hDevInfo);
	cout << "SetupDiDestroyDeviceInfoList" << endl;

	return MyDeviceDetected;
}

void GetDeviceCapabilities()
{
	//Get the Capabilities structure for the device.

	PHIDP_PREPARSED_DATA	PreparsedData;

	/*
	API function: HidD_GetPreparsedData
	Returns: a pointer to a buffer containing the information about the device's capabilities.
	Requires: A handle returned by CreateFile.
	There's no need to access the buffer directly,
	but HidP_GetCaps and other API functions require a pointer to the buffer.
	*/

	HidD_GetPreparsedData 
		(DeviceHandle, 
		&PreparsedData);
	cout << "HidD_GetPreparsedData: " << endl;

	/*
	API function: HidP_GetCaps
	Learn the device's capabilities.
	For standard devices such as joysticks, you can find out the specific
	capabilities of the device.
	For a custom device, the software will probably know what the device is capable of,
	and the call only verifies the information.
	Requires: the pointer to the buffer returned by HidD_GetPreparsedData.
	Returns: a Capabilities structure containing the information.
	*/
	
	HidP_GetCaps 
		(PreparsedData, 
		&Capabilities);
	cout << "HidP_GetCaps: "<< endl;

	//Display the capabilities

	cout << "Usage Page: " << Capabilities.UsagePage<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Input Report Byte Length: " << Capabilities.InputReportByteLength<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Output Report Byte Length: " << Capabilities.OutputReportByteLength<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Feature Report Byte Length: " << Capabilities.FeatureReportByteLength<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Link Collection Nodes: " << Capabilities.NumberLinkCollectionNodes<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Input Button Caps: " << Capabilities.NumberInputButtonCaps<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of InputValue Caps: " << Capabilities.NumberInputValueCaps<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of InputData Indices: " << Capabilities.NumberInputDataIndices<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Output Button Caps: " << Capabilities.NumberOutputButtonCaps<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Output Value Caps: " << Capabilities.NumberOutputValueCaps<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Output Data Indices: " <<  Capabilities.NumberOutputDataIndices<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Feature Button Caps: " << Capabilities.NumberFeatureButtonCaps<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Feature Value Caps: " << Capabilities.NumberFeatureValueCaps<< endl;
	cout << ValueToDisplay<< endl;
	cout << "Number of Feature Data Indices: " << Capabilities.NumberFeatureDataIndices<< endl;
	cout << ValueToDisplay<< endl;

	//No need for PreparsedData any more, so free the memory it's using.

	HidD_FreePreparsedData(PreparsedData);
	cout << "HidD_FreePreparsedData: "<< endl;
}

void PrepareForOverlappedTransfer()
{
	//Get a handle to the device for the overlapped ReadFiles.

	ReadHandle=CreateFile 
		(detailData->DevicePath, 
		GENERIC_READ, 
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		(LPSECURITY_ATTRIBUTES)NULL, 
		OPEN_EXISTING, 
		FILE_FLAG_OVERLAPPED, 
		NULL);

	cout << "CreateFile (ReadHandle): "<<endl;

	//Get an event object for the overlapped structure.

	/*API function: CreateEvent
	Requires:
	  Security attributes or Null
	  Manual reset (true). Use ResetEvent to set the event object's state to non-signaled.
	  Initial state (true = signaled) 
	  Event object name (optional)
	Returns: a handle to the event object
	*/

	if (hEventObject == 0)
	{
		hEventObject = CreateEvent 
			(NULL, 
			TRUE, 
			TRUE, 
			NULL);
	cout << "CreateEvent: " << endl;

	//Set the members of the overlapped structure.

	HIDOverlapped.hEvent = hEventObject;
	HIDOverlapped.Offset = 0;
	HIDOverlapped.OffsetHigh = 0;
	}
}

void DisplayInputReport( char * buffer)
{
	USHORT	ByteNumber;
	CHAR	ReceivedByte;
	
	//Display the received data in the log and the Bytes Received List boxes.
	//Start at the top of the List Box.

	//m_BytesReceived.ResetContent();
	
	//Step through the received bytes and display each.

	for (ByteNumber=0; ByteNumber < Capabilities.InputReportByteLength; ByteNumber++)
	{
		//Get a byte.

		ReceivedByte = buffer[ByteNumber];

		//Display it.

		cout << ReceivedByte <<", ";
	}
	cout << endl;
}


void ReadInputReport( char * inbuffer)
{

	// Retrieve an Input report from the device.

//	CString	ByteToDisplay = "";

	DWORD	Result;
	

	// Find out if the "Use Control Transfers Only" check box is checked.

	//UpdateData(true);

	int m_UseControlTransfersOnly =0;

	if (m_UseControlTransfersOnly) 
	{

		//Read a report from the device using a control transfer.
	
		/*
		HidD_GetInputReport
		Returns:
		True on success
		Requires: 
		A device handle returned by CreateFile.
		A buffer to hold the report.
		The report length returned by HidP_GetCaps in Capabilities.InputReportByteLength.
		*/
		
		if (ReadHandle != INVALID_HANDLE_VALUE)
			{
			Result = HidD_GetInputReport
			(ReadHandle,
			inbuffer,
			Capabilities.InputReportByteLength);

			cout << "HidD_GetInputReport: " <<endl;
			}
		else
			{
			Result = FALSE;
			}

		if (!Result)
			{
			//The read attempt failed, so close the handles, display a message,
			//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

			//CloseHandles();
			cout << "Can't read from device" << endl;
			MyDeviceDetected = FALSE;
			}
		else
			{
			cout << "Received Input report: " <<endl;
			
			//Display the report data.

//			DisplayInputReport();
		
			/*
			USHORT	ByteNumber;
			CHAR	ReceivedByte;
		
			//Display the received data in the log and the Bytes Received List boxes.
			//Start at the top of the List Box.

			m_BytesReceived.ResetContent();
	
			//Step through the received bytes and display each.

			for (ByteNumber=0; ByteNumber < Capabilities.InputReportByteLength; ByteNumber++)
			{
				//Get a byte.

				ReceivedByte = InputReport[ByteNumber];

				//Display it.

				DisplayReceivedData(ReceivedByte);
			}
			*/
		}
	} 

	else
	{
 	
	/*API call:ReadFile
	'Returns: the report in InputReport.
	'Requires: a device handle returned by CreateFile
	'(for overlapped I/O, CreateFile must be called with FILE_FLAG_OVERLAPPED),
	'the Input report length in bytes returned by HidP_GetCaps,
	'and an overlapped structure whose hEvent member is set to an event object.
	*/

	if (ReadHandle != INVALID_HANDLE_VALUE)
		{
		Result = ReadFile 
		(ReadHandle, 
		inbuffer, 
		Capabilities.InputReportByteLength, 
		&NumberOfBytesRead,
		(LPOVERLAPPED) &HIDOverlapped); 
		}
 
	cout << "ReadFile: " <<endl;

	/*API call:WaitForSingleObject
	'Used with overlapped ReadFile.
	'Returns when ReadFile has received the requested amount of data or on timeout.
	'Requires an event object created with CreateEvent
	'and a timeout value in milliseconds.
	*/

	Result = WaitForSingleObject 
		(hEventObject, 
		6000);

	cout << "WaitForSingleObject: "<<endl;
 
	switch (Result)
	{
	case WAIT_OBJECT_0:
		{
		cout << "Received Input report,"<<endl;
					
		break;
		}
	case WAIT_TIMEOUT:
		{
		cout << "ReadFile timeout" <<endl;
		cout << ValueToDisplay <<endl;
		//Cancel the Read operation.

		/*API call: CancelIo
		Cancels the ReadFile
        Requires the device handle.
        Returns non-zero on success.
		*/
		
		Result = CancelIo(ReadHandle);
		
		//A timeout may mean that the device has been removed. 
		//Close the device handles and set MyDeviceDetected = False 
		//so the next access attempt will search for the device.
		//CloseHandles();
		cout << "Can't read from device" <<endl;
		MyDeviceDetected = FALSE;
		break;
		}
	default:
		{
		cout << "Undefined error" <<endl;

		//Close the device handles and set MyDeviceDetected = False 
		//so the next access attempt will search for the device.

		//CloseHandles();
		cout << "Can't read from device" <<endl;
		MyDeviceDetected = FALSE;
		break;
		}
	}

	/*
	API call: ResetEvent
	Sets the event object to non-signaled.
	Requires a handle to the event object.
	Returns non-zero on success.
	*/

	ResetEvent(hEventObject);

	//Display the report data.

	}
}

void WriteOutputReport(char * outbuffer)
{
	//Send a report to the device.

	DWORD	BytesWritten = 0;
	INT		Index =0;
	ULONG	Result;
	//CString	strBytesWritten = "";


	
	//If Autoincrement is checked, increment the selection.
	int m_UseControlTransfersOnly =0;
	if (m_UseControlTransfersOnly)
		{

		//Send a report to the device.

		/*
		HidD_SetOutputReport
		Sends a report to the device.
		Returns: success or failure.
		Requires:
		The device handle returned by CreateFile.
		A buffer that holds the report.
		The Output Report length returned by HidP_GetCaps,
		*/

		if (WriteHandle != INVALID_HANDLE_VALUE)
			{
			Result = HidD_SetOutputReport
			(WriteHandle,
			outbuffer,
			Capabilities.OutputReportByteLength);

			cout << "HidD_SetOutputReport: " << endl;
			}

		if (Result)
			{
			cout << "An Output report was written to the device." << endl;
			}
		else
			{
			//The write attempt failed, so close the handles, display a message,
			//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

			//CloseHandles();
			cout << "Can't write to device" <<endl;
			MyDeviceDetected = FALSE;
			}

		}
	else
		{

		/*
		API Function: WriteFile
		Sends a report to the device.
		Returns: success or failure.
		Requires:
		A device handle returned by CreateFile.
		A buffer that holds the report.
		The Output Report length returned by HidP_GetCaps,
		A variable to hold the number of bytes written.
		*/

		if (WriteHandle != INVALID_HANDLE_VALUE)
			{
			Result = WriteFile 
			(WriteHandle, 
			outbuffer, 
			Capabilities.OutputReportByteLength, 
			&BytesWritten, 
			NULL);
		}

		//Display the result of the API call and the report bytes.

		cout << "WriteFile: " <<endl;

		if (!Result)
			{
			//The WriteFile failed, so close the handles, display a message,
			//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

			//CloseHandles();
			cout << "Can't write to device" <<endl;
			MyDeviceDetected = FALSE;
			}
		else
			{
			cout << "An Output report was written to the device." <<endl;
			cout << "Bytes Written: " <<endl; 
			//cout << strBytesWritten << endl;
			}
		}
}
