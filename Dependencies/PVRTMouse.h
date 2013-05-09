/******************************************************************************

 @File         PVRTMouse.h

 @Title        PVRTMouse

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  Functions for very simple DirectInput mouse input.

******************************************************************************/
#ifndef _PVRTMOUSE_H_
#define _PVRTMOUSE_H_


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
	const HWND		hWindow);

/*!***************************************************************************
 @Function			PVRTMouseShutdown
 @Description		Shuts down DirectInput.
					Do not use any functions, other than PVRTMouseInit(),
					after calling this.
*****************************************************************************/
void PVRTMouseShutdown();

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
	int * const		pnButtons);


#endif /* _PVRTMOUSE_H_ */

/*****************************************************************************
 End of file (PVRTMouse.h)
*****************************************************************************/

