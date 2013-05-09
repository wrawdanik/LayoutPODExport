/******************************************************************************

 @File         PVRTMouse.cpp

 @Title        PVRTMouse

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  Functions for very simple DirectInput mouse input.

******************************************************************************/
#include <dinput.h>

#include "PVRTGlobal.h"
#include "PVRTContext.h"
#include "PVRTMouse.h"



/* Direct Input buffer count; larger buffer reduces overflows */
#define DINPUT_BUFFERSIZE 8


/*
** Structure Definitions
*/

typedef struct _structMouseGlobals {
	// Used to retrieve mouse events from device
	DIDEVICEOBJECTDATA	pData[DINPUT_BUFFERSIZE];

	/* DirectInput and device interfaces */
	IDirectInput8		*pDI;
	IDirectInputDevice8	*pDev;

	HANDLE				hEvent;

	/* Right, left, middle button states */
	BOOL				bMBr, bMBl, bMBm;
} SMouseGlobals;


/*
** Macros
*/
#undef RELEASE
#define RELEASE(X) { if(X) { X->Release(); X = 0; } }


/*
** Static Functions
*/
static char *DIErrorToString(HRESULT hValue);


/*
** Static Variables
*/
static SMouseGlobals g_DI;


/*
** Code
*/

/*!***************************************************************************
 @Function			PVRTMouseInit
 @Input				hInstance				Instance of the window
 @Input				hWindow					Window handle
 @Return			TRUE or FALSE
 @Description		Initialises the mouse functions.
					The application instance handle and the window handle to
					which the mouse is attached must be passed to the function.
*****************************************************************************/
BOOL PVRTMouseInit(
	const HINSTANCE hInstance,
	const HWND		hWindow)
{
	DIPROPDWORD		dipdw = {
		/* the header */
		{
			sizeof(DIPROPDWORD),	/* diph.dwSize */
			sizeof(DIPROPHEADER),	/* diph.dwHeaderSize */
			0,						/* diph.dwObj */
			DIPH_DEVICE,			/* diph.dwHow */
		},
		/* the data */
		DINPUT_BUFFERSIZE,			/* dwData */
	};
	HRESULT			hRet;


	_RPT0(_CRT_WARN, "PVRTMouseInit() {\n");

	/* clear that global */
	memset(&g_DI, 0, sizeof(g_DI));


	/*
		Start DirectInput
	*/

	hRet = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_DI.pDI, NULL);
	if(hRet != DI_OK) {
		_RPT1(_CRT_WARN, "Error creating IDirectInput %s.\n", DIErrorToString(hRet));
		return FALSE;
	}

	/* get mouse device */
	hRet = g_DI.pDI->CreateDevice(GUID_SysMouse, &g_DI.pDev, 0);
	if(hRet != DI_OK) {
		_RPT1(_CRT_WARN, "Error creating DirectInput mouse device %s.\n", DIErrorToString(hRet));
		RELEASE(g_DI.pDI);
		return FALSE;
	}

	/* Set data format */
	hRet = g_DI.pDev->SetDataFormat(&c_dfDIMouse);
	if(hRet != DI_OK) {
		_RPT1(_CRT_WARN, "mouse->SetDataFormat failed %s.\n", DIErrorToString(hRet));
		RELEASE(g_DI.pDev);
		RELEASE(g_DI.pDI);
		return FALSE;
	}

	/* Set cooperative level */
	hRet = g_DI.pDev->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if(hRet != DI_OK) {
		_RPT1(_CRT_WARN, "mouse->SetCooperativeLevel failed %s.\n", DIErrorToString(hRet));
		RELEASE(g_DI.pDev);
		RELEASE(g_DI.pDI);
		return FALSE;
	}


	/* Create mouse input event */
	g_DI.hEvent = CreateEvent(0, 0, 0, 0);

	/* Check whether the Event was created correctly */
	if(g_DI.hEvent == NULL) {
		_RPT0(_CRT_WARN, "CreateEvent (mouse handler) failed\n");
		RELEASE(g_DI.pDev);
		RELEASE(g_DI.pDI);
		return FALSE;
	}

	/* Notify the device of the event */
	hRet = g_DI.pDev->SetEventNotification(g_DI.hEvent);
	if(hRet != DI_OK) {
		_RPT1(_CRT_WARN, "mouse->SetEventNotification failed %s.\n", DIErrorToString(hRet));
	}

	/* Now we need to set the buffer size, to use buffered mouse data */
	hRet = g_DI.pDev->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if(hRet != DI_OK) {
		_RPT1(_CRT_WARN, "mouse->SetProperty failed %s.\n", DIErrorToString(hRet));
	}

	/* Acquire mouse */
	hRet = g_DI.pDev->Acquire();
	if(hRet != DI_OK) {
		_RPT1(_CRT_WARN, "mouse->Acquire failed %s.\n", DIErrorToString(hRet));
	}

	_RPT0(_CRT_WARN, "} PVRTMouseInit()\n");

	return TRUE;
}


/*!***************************************************************************
 @Function			PVRTMouseShutdown
 @Description		Shuts down DirectInput.
					Do not use any functions, other than PVRTMouseInit(),
					after calling this.
*****************************************************************************/
void PVRTMouseShutdown()
{
	_RPT0(_CRT_WARN, "PVRTMouseShutdown()...");

	/* Unnacquire mouse */
	if(g_DI.pDev)
		g_DI.pDev->Unacquire();

	/* Be nice; Release stuff */
	RELEASE(g_DI.pDev);
	RELEASE(g_DI.pDI);

	/* clear that global */
	memset(&g_DI, 0, sizeof(g_DI));

	_RPT0(_CRT_WARN, "done.\n");
}


/*!***************************************************************************
 @Function			PVRTMouseQuery
 @Output			pnX			relative motion of the mouse X axis since this function was last called.
 @Output			pnY			relative motion of the mouse Y axis since this function was last called.
 @Output			pnButtons	Bit0 1 = Left button been pressed
								Bit1 1 = Right button been pressed
								Bit2 1 = Middle button been pressed
 @Return			TRUE or FALSE
 @Description		Query the mouse status.
*****************************************************************************/
BOOL PVRTMouseQuery(
	int * const		pnX,
	int * const		pnY,
	int * const		pnButtons)
{
	HRESULT hRet;
	DWORD	dwCount = DINPUT_BUFFERSIZE;

	if(!g_DI.pDI || !g_DI.pDev)
		return FALSE;

	/* Zero return values */
	*pnX = *pnY = *pnButtons = 0;

	/* Get the mouse data */
	hRet = g_DI.pDev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), g_DI.pData, &dwCount, 0);

#ifdef _DEBUG
	if(hRet == DI_BUFFEROVERFLOW) {
		_RPT0(_CRT_WARN, "DI_BUFFEROVERFLOW");
	}
#endif

	/* If we have an error, attempt to reaqquire */
	if((hRet != DI_OK) && (hRet != DI_BUFFEROVERFLOW)) {
		/* State the error */
		_RPT1(_CRT_WARN, "PVRTMouseQuery() dev->GetDeviceData() %s\n", DIErrorToString(hRet));

		/* Try to reacquire if focus lost */
		if(hRet == DIERR_INPUTLOST) {
			if(g_DI.pDev->Acquire() != DI_OK) {
				_RPT0(_CRT_WARN, "mouse->Acquire() FAIL\n");
				return FALSE;
			}

			_RPT0(_CRT_WARN, "mouse->Acquire() Succeeded\n");
			return PVRTMouseQuery(pnX, pnY, pnButtons);
		}
	}

	/* Now set pnX, pnY & on Buttons
	** Run through all returned buffers, summing movements */
	while(dwCount--) switch(g_DI.pData[dwCount].dwOfs) {
		case DIMOFS_X:
			*pnX += g_DI.pData[dwCount].dwData;
			continue;

		case DIMOFS_Y:
			*pnY += g_DI.pData[dwCount].dwData;
			continue;

		case DIMOFS_BUTTON0:	// left button
			if(g_DI.pData[dwCount].dwData & 0x80)
				g_DI.bMBl = 1;
			else
				g_DI.bMBl = 0;
			continue;

		case DIMOFS_BUTTON1:	// Right button
			if(g_DI.pData[dwCount].dwData & 0x80)
				g_DI.bMBr = 1;
			else
				g_DI.bMBr = 0;
			continue;

		case DIMOFS_BUTTON2:	// Middle button
			if(g_DI.pData[dwCount].dwData & 0x80)
				g_DI.bMBm = 1;
			else
				g_DI.bMBm = 0;
			continue;
	}

	/* Set the button status */
	*pnButtons = (g_DI.bMBm << 2) | (g_DI.bMBr << 1) | (g_DI.bMBl << 0);

	/* Done! */
	return TRUE;
}



static char *DIErrorToString(HRESULT hValue) {
	switch(hValue) {
	case DI_OK:
		return "DI_OK";

	case DI_BUFFEROVERFLOW:
		return "DI_BUFFEROVERFLOW";

	case DIERR_INPUTLOST:
		return "DIERR_INPUTLOST";

	case DIERR_INVALIDPARAM:
		return "DIERR_INVALIDPARAM";

	case DIERR_NOTACQUIRED:
		return "DIERR_NOTACQUIRED";

	case DIERR_NOTBUFFERED:
		return "DIERR_NOTBUFFERED";

	case DIERR_NOTINITIALIZED:
		return "DIERR_NOTINITIALIZED";

	case DIERR_OTHERAPPHASPRIO:
		return "DIERR_OTHERAPPHASPRIO";

	default:
		return "UnknownErrorCode";
	}
}

/*****************************************************************************
 End of file (PVRTMouse.cpp)
*****************************************************************************/

