//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						MLongControls.c
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/15/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The routines in this file handle the long min, max and value
//								parameters needed for controls.
//
//								The code comes from:
//									WASTE Demo Project:
//									Macintosh Controls with Long Values
//
//									Copyright (c) 1993-1998 Marco Piovanelli
//									All Rights Reserved
//
//									C port by John C. Daub
//
//								These files have been updated so that the Apple API for long
//								control values are used if the appearance manager is present.
//
//								Note that the "powerc" definition is not available for macho
//								compiler.  The destinction of powerc and older 68K processors
//								has to be handled differently. (10/28/2008) 
//
//	Functions in file:	
//
//	Include files:			"MultiSpecHeaders"
//								"SMultiSpec.h"
//
//------------------------------------------------------------------------------------

//#include	"SMultiSpec.h"
//#include	"SExternalGlobals.h"

		// long control auxiliary record used for keeping long settings.
		// A handle to this record is stored in the reference field of the control 
		// record.

struct LCAuxRec
	{
	SInt32	value;	// long value
	SInt32	min;		// long min
	SInt32	max;		// long max
	
	};
	
typedef struct LCAuxRec LCAuxRec, *LCAuxPtr, **LCAuxHandle;



OSErr LCAttach (
				ControlHandle 						inControl)
				
{
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		{
	//#endif
		LCAuxHandle 						aux;
		

				//	Alocate the auxiliary record that will hold long settings
				
		if ((aux = (LCAuxHandle)NewHandleClear (sizeof (LCAuxRec))) == nil)
			{
																					return	MemError ();
			
			}

				//	Store a handle to the auxiliary record in the contrlRfCon field
				
		SetControlReference (inControl, (SInt32)aux);

				//	Copy current control settings into the auxiliary record
				
		(*aux)->value = GetControlValue (inControl);
		(*aux)->min = GetControlMinimum (inControl);
		(*aux)->max = GetControlMaximum (inControl);
		
	//#if defined (powerc)
		}	// end "if (!gAppearanceManagerFlag)"
	//#endif

	return noErr;
	
}	// end "LCAttach"



void LCDetach (
				ControlHandle 						inControl)
				
{
	Handle 								aux;
	
	
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		{
		if ((aux = (Handle)GetControlReference (inControl)) != nil)
			{
			SetControlReference (inControl, 0L);
			DisposeHandle (aux);
			
			}	// end "if ((aux = (Handle)GetControlReference (..."
		
		}	// end "if (!gAppearanceManagerFlag)"
	//#endif
	/*
	#if !defined (powerc)
		if ((aux = (Handle)GetControlReference (inControl)) != nil)
			{
			SetControlReference (inControl, 0L);
			DisposeHandle (aux);
			
			}	// end "if ((aux = (Handle)GetControlReference (..."
	#endif
	*/
}	// end "LCDetach"



SInt32 LCGetMax (
				ControlHandle 						inControl)
				
{
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		return (*(LCAuxHandle)GetControlReference (inControl))->max;
		
	else	// gAppearanceManagerFlag
		return GetControl32BitMaximum ((ControlRef)inControl);
	//#endif

	//#if !defined (powerc)
		//return (*(LCAuxHandle)GetControlReference (inControl))->max;
	//#endif
	
}	// end "LCGetMax"



SInt32 LCGetMin (
				ControlHandle 						inControl)
 
{
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		return (*(LCAuxHandle)GetControlReference (inControl))->min;
		
	else	// gAppearanceManagerFlag
		return GetControl32BitMinimum ((ControlRef)inControl);
	//#endif

	//#if !defined (powerc)
		//return (*(LCAuxHandle)GetControlReference (inControl))->min;
	//#endif
	
}	// end "LCGetMin"



SInt32 LCGetValue (
				ControlHandle 						inControl)
				
{
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		return (*(LCAuxHandle)GetControlReference (inControl))->value;
		
	else	// gAppearanceManagerFlag
		return GetControl32BitValue ((ControlRef)inControl);
	//#endif

	//#if !defined (powerc)
		//return (*(LCAuxHandle)GetControlReference (inControl))->value;
	//#endif
	
}	// end "LCGetValue"



void LCSetMax (
				ControlHandle 						inControl, 
				SInt32 								inMax)

{
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		{
	//#endif
		LCAuxHandle aux ;

		aux = (LCAuxHandle)GetControlReference (inControl);

				//	Make sure inMax is greater than or equal to min
				
		if (inMax < (*aux)->min)
			inMax = (*aux)->min;

				//	Save inMax in auxiliary record
				
		(*aux)->max = inMax;

				//	Set control maximum to inMax or SHRT_MAX, whichever is less
				
		SetControlMaximum (inControl, (inMax <= SHRT_MAX) ? inMax : SHRT_MAX);

				//	Reset value
				
		LCSetValue (inControl, (*aux)->value);
			
	//#if defined (powerc)
		}	// end "if (!gAppearanceManagerFlag)"
		
	else	// gAppearanceManagerFlag
		SetControl32BitMaximum ((ControlRef)inControl, inMax);
	//#endif
	
}	// end "LCSetMax"



void LCSetMin (
				ControlHandle 						inControl, 
				SInt32 								inMin)
				
{						
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		{
	//#endif
		LCAuxHandle aux;

		aux = (LCAuxHandle)GetControlReference (inControl);

				//	Make sure inMin is less than or equal to max
				
		if (inMin > (*aux)->max)
			inMin = (*aux)->max;

				//	Save inMin in auxiliary record
				
		(*aux)->min = inMin;

				//	Set control minimum to inMin or SHRT_MIN, whichever is greater
				
		SetControlMinimum (inControl, (inMin >= SHRT_MIN) ? inMin : SHRT_MIN);

				//	Reset value
				
		LCSetValue (inControl, (*aux)->value);
			
	//#if defined (powerc)	
		}	// end "if (!gAppearanceManagerFlag)"
		
	else	// gAppearanceManagerFlag
		SetControl32BitMinimum ((ControlRef)inControl, inMin);
	//#endif
		
}	// end "LCSetMin"



void LCSetValue (
				ControlHandle 						inControl, 
				SInt32 								inValue)
				
{
	LCAuxHandle 						aux;
	
	SInt16 								controlMin, 
											controlMax, 
											newControlValue;
											
	//#if defined (powerc)
	if (!gAppearanceManagerFlag)
		{
	//#endif
		aux = (LCAuxHandle)GetControlReference (inControl);

				//	Make sure inValue is in the range min...max
				
		if (inValue < (*aux)->min)
			inValue = (*aux)->min;
			
		if (inValue > (*aux)->max)
			inValue = (*aux)->max;

				//	Save inValue in auxiliary record
				
		(*aux)->value = inValue;

				//	Calculate new thumb position
				
		controlMin = GetControlMinimum (inControl);
		controlMax = GetControlMaximum (inControl);
		newControlValue = controlMin + FixRound (
														FixMul (FixDiv (inValue - (*aux)->min,
														(*aux)->max - (*aux)->min), 
														(SInt32)(controlMax - controlMin)<<16));

				//	Do nothing if the thumb position hasn't changed
		
		if (newControlValue != GetControlValue (inControl))
			SetControlValue (inControl, newControlValue);
				
	//#if defined (powerc)		
		}	// end "if (!gAppearanceManagerFlag)"
		
	else	// gAppearanceManagerFlag
		SetControl32BitValue ((ControlRef)inControl, inValue);
	//#endif
		
}	// end "LCSetValue"



void LCSynch (
				ControlHandle 						inControl)
				
{
	if (!gAppearanceManagerFlag)
		{
		LCAuxHandle aux ;
		SInt16 controlMin, controlMax, controlValue ;

		controlMin = GetControlMinimum (inControl) ;
		controlMax = GetControlMaximum (inControl) ;
		controlValue = GetControlValue (inControl) ;
		aux = (LCAuxHandle) GetControlReference (inControl) ;

				//	Calculate new long value
				
		(*aux)->value = (*aux)->min + FixMul (FixRatio (controlValue - controlMin,
															controlMax - controlMin), 
															(*aux)->max - (*aux)->min);
					  
		}	// end "gAppearanceManagerFlag"
		
}	// end "LCSynch"


#if !__POWERPC__
pascal SInt32 GetControl32BitMaximum (
				ControlHandle	 					inControl)

{
	return (GetControlMaximum (inControl));

}	// end "GetControl32BitMaximum"



pascal SInt32 GetControl32BitValue (
				ControlHandle	 					inControl)

{
	return (GetControlValue (inControl));

}	// end "GetControl32BitValue"



pascal void SetControl32BitMaximum (
				ControlHandle	 					inControl, 
				SInt32 								inValue)

{
	SetControlMaximum (inControl, (SInt16)inValue);

}	// end "SetControl32BitMaximum"


/*
pascal void SetControl32BitValue (
				ControlHandle	 					inControl, 
				SInt32 								inValue)

{
	SetControlValue (inControl, (SInt16)inValue);

}	// end "SetControl32BitValue"
*/
#endif	// !__POWERPC__
