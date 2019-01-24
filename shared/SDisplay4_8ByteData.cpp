//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SDisplay4_8ByteData.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/22/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Functions in file:	void 		Display2C16BImage
//								void 		Display2C24BImage
//								void 		Display3C1624BImage
//								void		UpdatePaletteFor16and24BImage
//
//	Brief description:	 
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif	


			// Prototypes for file routines that are only called from other 		
			// routines in this file.



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display1Channel4Byte8BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 1-channel
//							color image.
//							Note that this code is almost the very same code in 
//							"ConvertDataValueToBinValue". The two sets of code need to stay
//							in sync.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/25/2003
//	Revised By:			Larry L. Biehl			Date: 01/22/2019

void Display1Channel4Byte8BitLine (
				SInt16								displayCode,
				UInt32								numberSamples,
				UInt32								interval, 
				double								minValue1,
				double								binFactor1,
				HDoublePtr							ioBuffer1Ptr,
				HUCharPtr							dataDisplayPtr,
				UInt32								maxBin1,
				HUCharPtr							offScreenPtr)

{	
	double								doubleBinIndex;
	
	UInt32								binIndex,
											j;
														

	for (j=0; j<numberSamples; j+=interval)
		{
		doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1.00000001;
		if (doubleBinIndex < 0)
			binIndex = 0;
			
		else if (doubleBinIndex > (double)maxBin1)
			binIndex = maxBin1;
			
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin1);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"
			
		#if defined multispec_mac || defined multispc_win
			*offScreenPtr = dataDisplayPtr[binIndex];
		#endif
		
		#if defined multispec_lin
			if (displayCode == 101)
				{
				#if defined multispec_wxmac
							// Leave high order (alpha) byte blank.
					offScreenPtr++;
				#endif
						// Need to load the same value for R, G, B bytes.
				*offScreenPtr = dataDisplayPtr[binIndex];
				offScreenPtr++;
				*offScreenPtr = dataDisplayPtr[binIndex];
				offScreenPtr++;
				*offScreenPtr = dataDisplayPtr[binIndex];
				
				#if defined multispec_wxlin
							// Leave low order (alpha) byte blank.
					offScreenPtr++;
				#endif
				
				}	// end "if (displayCode == 51)"
		
			else	// displayCode == 151
				{
						// This is for one channel thematic images for wxWidgets
						// interface. The thematic value is loaded into the buffer.
				
				*offScreenPtr = (Byte)dataDisplayPtr[binIndex];
				
				}	// end "else displayCode == 151"
		#endif
		
		offScreenPtr++;
			
		}	// end "for (j=0;..." 
	
}	// end "Display1Channel4Byte8BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display2Channel4Byte8BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 3-channel
//							color image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 01/05/2006
//	Revised By:			Larry L. Biehl			Date: 01/22/2019

void Display2Channel4Byte8BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				double								minValue1,
				double								minValue2,
				double								binFactor1,
				double								binFactor2,
				UInt16								backgroundValueCode,
				HDoublePtr							ioBuffer1Ptr,
				HDoublePtr							ioBuffer2Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				HUCharPtr							offScreenPtr)

{	
	double								doubleBinIndex;
		
	UInt32								backgroundValue,
											binIndex,
											j;
							
							
	for (j=0; j<numberSamples; j+=interval)
		{
		#if defined multispec_wxmac
					// Leave hight order (alpha) byte blank.
			offScreenPtr++;
		#endif
		
		doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin1)
			binIndex = maxBin1;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin1);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

		backgroundValue = binIndex;		
		*offScreenPtr = dataDisplay1Ptr[binIndex];
		
		doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin2)
			binIndex = maxBin2;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin2);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

		backgroundValue += binIndex;
		*offScreenPtr += dataDisplay2Ptr[binIndex];

		if (backgroundValueCode && !backgroundValue)
			{
			if (backgroundValueCode == 1)  
				*offScreenPtr = 255;
			else	// backgroundValueCode == 2 
				*offScreenPtr = 0;
				
			}	// end "if (backgroundValueCode && !backgroundValue)"

		offScreenPtr++;
		
		#if defined multispec_wxlin
					// Leave low order (alpha) byte blank.
			offScreenPtr++;
		#endif

		}	// end "for (j=0; ..."  
		
}	// end "Display2Channel4Byte8BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display2Channel4Byte16BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 3-channel
//							color image.
//							Note that this routine is only used by Mac version
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 01/05/2006
//	Revised By:			Larry L. Biehl			Date: 12/12/2018

void Display2Channel4Byte16BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				double								minValue1,
				double								minValue2,
				double								binFactor1,
				double								binFactor2,
				UInt16								backgroundValueCode,
				HDoublePtr							ioBuffer1Ptr,
				HDoublePtr							ioBuffer2Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				HUInt16Ptr							offScreen2BytePtr,
				SInt16								rgbColors)

{	
	double								doubleBinIndex;
	
	UInt32								backgroundValue,
											binIndex,
											j;
	
	
	#if defined multispec_lin
				// 16-bit color is not used for wxWidgets version
																								return;
	#endif	// defined multispec_lin
	
	switch (rgbColors)
		{
		case kGBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac
							// Set high order bit and Red bits to 0).						
						
					*offScreen2BytePtr = 0;					
				#endif	// defined multispec_mac || ...
				
						// Green bits.				
						
				doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
				if (doubleBinIndex < 0)
					binIndex = 0;
				else if (doubleBinIndex > (double)maxBin2)
					binIndex = maxBin2;
				else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
					{
					binIndex = (UInt32)doubleBinIndex;
					
							// This will catch case when doubleBinIndex is nan or -nan. The
							// conversion from this can be a large unsigned 32-bit int.
							
					binIndex = MIN (binIndex, maxBin2);
					
					}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

				backgroundValue = binIndex;
				*offScreen2BytePtr = (dataDisplay2Ptr[binIndex] << 5);
				
						// Blue bits.				
						
				doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
				if (doubleBinIndex < 0)
					binIndex = 0;
				else if (doubleBinIndex > (double)maxBin1)
					binIndex = maxBin1;
				else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
					{
					binIndex = (UInt32)doubleBinIndex;
					
							// This will catch case when doubleBinIndex is nan or -nan. The
							// conversion from this can be a large unsigned 32-bit int.
							
					binIndex = MIN (binIndex, maxBin1);
					
					}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

				backgroundValue += binIndex;
				*offScreen2BytePtr = 
								*offScreen2BytePtr | (dataDisplay1Ptr[binIndex]);

				if (backgroundValueCode && !backgroundValue)
					{
					if (backgroundValueCode == 1)
						*offScreen2BytePtr = 0x0000;
					else	// backgroundValueCode == 2
						*offScreen2BytePtr = 0x7fff;
						
					}	// end "if (backgroundValueCode && !backgroundValue)" 
					
				offScreen2BytePtr++;
				
				}	// end "for (j=0; ..." 
			break;
			
		case kRBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac
							// Set high order bit and Green bits to 0).						
						
					*offScreen2BytePtr = 0;					
				#endif	// defined multispec_mac || ...
						
						// Red bits.				
						
				doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
				if (doubleBinIndex < 0)
					binIndex = 0;
				else if (doubleBinIndex > (double)maxBin2)
					binIndex = maxBin2;
				else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
					{
					binIndex = (UInt32)doubleBinIndex;
					
							// This will catch case when doubleBinIndex is nan or -nan. The
							// conversion from this can be a large unsigned 32-bit int.
							
					binIndex = MIN (binIndex, maxBin2);
					
					}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

				backgroundValue = binIndex;
				*offScreen2BytePtr = (dataDisplay2Ptr[binIndex] << 10);
				
						// Blue bits.					
						
				doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
				if (doubleBinIndex < 0)
					binIndex = 0;
				else if (doubleBinIndex > (double)maxBin1)
					binIndex = maxBin1;
				else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
					{
					binIndex = (UInt32)doubleBinIndex;
					
							// This will catch case when doubleBinIndex is nan or -nan. The
							// conversion from this can be a large unsigned 32-bit int.
							
					binIndex = MIN (binIndex, maxBin1);
					
					}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

				backgroundValue += binIndex;
				*offScreen2BytePtr = 
								*offScreen2BytePtr | (dataDisplay1Ptr[binIndex]);

				if (backgroundValueCode && !backgroundValue)
					{
					if (backgroundValueCode == 1)
						*offScreen2BytePtr = 0x0000;
					else	// backgroundValueCode == 2
						*offScreen2BytePtr = 0x7fff;
						
					}	// end "if (backgroundValueCode && !backgroundValue)"
					
				offScreen2BytePtr++;
				
				}	// end "for (j=0; ..." 
			break;
			
		case kRGColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac
							// Set high order bit and Blue bits to 0).						
						
					*offScreen2BytePtr = 0;					
				#endif	// defined multispec_mac || ...
				
						// Red bits.			
						
				doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
				if (doubleBinIndex < 0)
					binIndex = 0;
				else if (doubleBinIndex > (double)maxBin2)
					binIndex = maxBin2;
				else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
					{
					binIndex = (UInt32)doubleBinIndex;
					
							// This will catch case when doubleBinIndex is nan or -nan. The
							// conversion from this can be a large unsigned 32-bit int.
							
					binIndex = MIN (binIndex, maxBin2);
					
					}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

				backgroundValue = binIndex;
				*offScreen2BytePtr = (dataDisplay2Ptr[binIndex] << 10);
				
						// Green bits.						
						
				doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
				if (doubleBinIndex < 0)
					binIndex = 0;
				else if (doubleBinIndex > (double)maxBin1)
					binIndex = maxBin1;
				else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
					{
					binIndex = (UInt32)doubleBinIndex;
					
							// This will catch case when doubleBinIndex is nan or -nan. The
							// conversion from this can be a large unsigned 32-bit int.
							
					binIndex = MIN (binIndex, maxBin1);
					
					}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

				backgroundValue += binIndex;
				*offScreen2BytePtr = 
						*offScreen2BytePtr | (dataDisplay1Ptr[binIndex] << 5);

				if (backgroundValueCode && !backgroundValue)
					{
					if (backgroundValueCode == 1)
						*offScreen2BytePtr = 0x0000;
					else	// backgroundValueCode == 2
						*offScreen2BytePtr = 0x7fff;
						
					}	// end "if (backgroundValueCode && !backgroundValue)"
					
				offScreen2BytePtr++;
				
				}	// end "for (j=0; ..." 
			break;
			
		}	// end "switch (displayPalette.rgbColors)" 
	
}	// end "Display2Channel4Byte16BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display2Channel4Byte24BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input line of 4 or 
//							image data to the offscreen buffer for a 2-channel,
//							8 byte 24 bit color image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 01/05/2006
//	Revised By:			Larry L. Biehl			Date: 01/22/2019

void Display2Channel4Byte24BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				double								minValue1,
				double								minValue2,
				double								binFactor1,
				double								binFactor2,
				UInt16								backgroundValueCode,
				HDoublePtr							ioBuffer1Ptr,
				HDoublePtr							ioBuffer2Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				HUCharPtr							offScreenPtr,
				SInt16								rgbColors)

{	
	double								doubleBinIndex;
		
	UInt32								backgroundValue,
											binIndex,
											j;

	HUInt32Ptr							offScreen4BytePtr;
																			
		
	offScreen4BytePtr = (HUInt32Ptr)offScreenPtr;
			
	switch (rgbColors)
		{
		case kGBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac || defined multispec_lin
					#if defined multispec_mac || defined multispec_wxmac
								// Leave high order byte blank.
				
						offScreenPtr++;
					#endif
				
							// Set Red byte to zero.											
							
					*offScreenPtr = 0;
					offScreenPtr++;
					
							// Green byte.					
						
					doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin2)
						binIndex = maxBin2;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin2);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

					backgroundValue = binIndex;
					*offScreenPtr = dataDisplay2Ptr[binIndex];
					offScreenPtr++;
					
							// Blue byte.						
						
					doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin1)
						binIndex = maxBin1;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin1);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

					backgroundValue += binIndex;
					*offScreenPtr = dataDisplay1Ptr[binIndex];
					offScreenPtr++;
					
					#if defined multispec_wxlin
								// Leave low order byte blank.
						offScreenPtr++;
					#endif

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1)
							*offScreen4BytePtr = 0x00000000;
						else	// backgroundValueCode == 2 
							#if TARGET_CPU_X86 || TARGET_CPU_X86_64
								*offScreen4BytePtr = 0xffffff00;
							#else	// !TARGET_CPU_X86
								*offScreen4BytePtr = 0x00ffffff;
							#endif	// TARGET_CPU_X86 ... else 
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
		
					offScreen4BytePtr++;					
				#endif	// defined multispec_mac || defined multispec_lin
				
				#if defined multispec_win 					
							// Blue byte.						
						
					doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin1)
						binIndex = maxBin1;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin1);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

					backgroundValue = binIndex;
					*offScreenPtr = dataDisplay1Ptr[binIndex];
					offScreenPtr++;
					
							// Green byte.					
						
					doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin2)
						binIndex = maxBin2;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin2);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

					backgroundValue += binIndex;
					*offScreenPtr = dataDisplay2Ptr[binIndex];
					offScreenPtr++; 
				
							// Set Red byte to zero.											
							
					*offScreenPtr = 0;
					offScreenPtr++;

					if (backgroundValueCode && !backgroundValue)
						{
						offScreenPtr -= 3;

						if (backgroundValueCode == 1) 
							{
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							
							}	// end "else backgroundValueCode == 1"

						else	// backgroundValueCode == 2 
							{
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							
							}	// end "else backgroundValueCode == 2"	
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
				#endif	// defined multispec_win
				
				}	// end "for (j=0; ..." 
			break;
			
		case kRBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac || defined multispec_lin
					#if defined multispec_mac || defined multispec_wxmac
								// Leave high order byte blank.
						offScreenPtr++;
					#endif
							// Red byte.						
						
					doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin2)
						binIndex = maxBin2;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin2);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

					backgroundValue = binIndex;
					*offScreenPtr = dataDisplay2Ptr[binIndex];
					offScreenPtr++;
					
							// Set Green byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++;
					
							// Blue byte.						
						
					doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin1)
						binIndex = maxBin1;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin1);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

					backgroundValue += binIndex;
					*offScreenPtr = dataDisplay1Ptr[binIndex];
					offScreenPtr++;
					
					#if defined multispec_wxlin
								// Leave low order byte blank.
						offScreenPtr++;
					#endif

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1)
							*offScreen4BytePtr = 0x00000000;
						else	// backgroundValueCode == 2 
							#if TARGET_CPU_X86 || TARGET_CPU_X86_64
								*offScreen4BytePtr = 0xffffff00;
							#else	// !TARGET_CPU_X86
								*offScreen4BytePtr = 0x00ffffff;
							#endif	// TARGET_CPU_X86 ... else 
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
		
					offScreen4BytePtr++;					
				#endif	// defined multispec_mac
				
				#if defined multispec_win					
							// Blue byte.						
						
					doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin1)
						binIndex = maxBin1;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin1);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

					backgroundValue = binIndex;
					*offScreenPtr = dataDisplay1Ptr[binIndex];
					offScreenPtr++; 
					
							// Set Green byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++; 
				
							// Red byte.							
						
					doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin2)
						binIndex = maxBin2;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin2);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

					backgroundValue += binIndex;
					*offScreenPtr = dataDisplay2Ptr[binIndex];
					offScreenPtr++; 

					if (backgroundValueCode && !backgroundValue)
						{
						offScreenPtr -= 3;

						if (backgroundValueCode == 1) 
							{
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							
							}	// end "else backgroundValueCode == 1"

						else	// backgroundValueCode == 2 
							{
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							
							}	// end "else backgroundValueCode == 2"	
							
						}	// end "if (backgroundValueCode && !backgroundValue)"					
				#endif	// defined multispec_win
				
				}	// end "for (j=0; ..." 
			break;
			
		case kRGColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac || defined multispec_lin
					#if defined multispec_mac || defined multispec_wxmac
								// Leave high order byte blank.
						offScreenPtr++;
					#endif
				
							// Red byte.							
						
					doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin2)
						binIndex = maxBin2;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin2);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

					backgroundValue = binIndex;
					*offScreenPtr = dataDisplay2Ptr[binIndex];
					offScreenPtr++;
					
							// Green byte.							
						
					doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin1)
						binIndex = maxBin1;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin1);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

					backgroundValue += binIndex;
					*offScreenPtr = dataDisplay1Ptr[binIndex];
					offScreenPtr++;
					
							// Set Blue byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++;
				
					#if defined multispec_wxlin
								// Leave low order byte blank.
						offScreenPtr++;
					#endif

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1)
							*offScreen4BytePtr = 0x00000000;
						else	// backgroundValueCode == 2 
							#if TARGET_CPU_X86 || TARGET_CPU_X86_64
								*offScreen4BytePtr = 0xffffff00;
							#else	// !TARGET_CPU_X86
								*offScreen4BytePtr = 0x00ffffff;
							#endif	// TARGET_CPU_X86 ... else 
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
		
					offScreen4BytePtr++;					
				#endif	// defined multispec_mac || defined multispec_lin
				
				#if defined multispec_win 					
							// Set Blue byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++;
					
							// Green byte.						
						
					doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin1)
						binIndex = maxBin1;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin1);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"

					backgroundValue = binIndex;
					*offScreenPtr = dataDisplay1Ptr[binIndex];
					offScreenPtr++; 
				
							// Red byte.								
						
					doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
					if (doubleBinIndex < 0)
						binIndex = 0;
					else if (doubleBinIndex > (double)maxBin2)
						binIndex = maxBin2;
					else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
						{
						binIndex = (UInt32)doubleBinIndex;
						
								// This will catch case when doubleBinIndex is nan or -nan. The
								// conversion from this can be a large unsigned 32-bit int.
								
						binIndex = MIN (binIndex, maxBin2);
						
						}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"

					backgroundValue += binIndex;
					*offScreenPtr = dataDisplay2Ptr[binIndex];
					offScreenPtr++;
			
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1) 
							{
							offScreenPtr -= 3;
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0x00;
							offScreenPtr++;
							
							}	//  end "if (backgroundValueCode == 1)"
							
						else	// backgroundValueCode == 2 
							{
							offScreenPtr -= 3;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							
							}	//  end "else backgroundValueCode == 2"
						
						}	// end "if (backgroundValueCode && ...)"					
				#endif	// defined multispec_win
				
				}	// end "for (j=0; ..." 
			break;
			
		}	// end "switch (rgbColors)" 
	
}	// end "Display2Channel4Byte24BitLine"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display3Channel4Byte8BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 3-channel
//							color image for 4byte or more formatted data.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 01/05/2006
//	Revised By:			Larry L. Biehl			Date: 12/12/2018

void Display3Channel4Byte8BitLine (
				UInt32								numberSamples,
				UInt32								interval,
				double								minValue1,
				double								minValue2,
				double								minValue3,
				double								binFactor1,
				double								binFactor2,
				double								binFactor3,
				UInt16								backgroundValueCode,
				HDoublePtr							ioBuffer1Ptr,
				HDoublePtr							ioBuffer2Ptr,
				HDoublePtr							ioBuffer3Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				HUCharPtr							dataDisplay3Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				UInt32								maxBin3, 
				HUCharPtr							offScreenPtr)

{	
	double								doubleBinIndex;
	
	UInt32								backgroundValue,
											binIndex,
											j;
	
	#if defined multispec_lin
		UChar									offScreenValue;
	#endif
	
			
	for (j=0; j<numberSamples; j+=interval)
		{
				// Blue					
						
		doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin1)
			binIndex = maxBin1;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin1);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"
			
		backgroundValue = binIndex;
		*offScreenPtr = dataDisplay1Ptr[binIndex];
		
				// Green								
						
		doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin2)
			binIndex = maxBin2;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin2);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"
			
		backgroundValue += binIndex;
		*offScreenPtr += dataDisplay2Ptr[binIndex];
		
				// Red							
						
		doubleBinIndex = (ioBuffer3Ptr[j] - minValue3)*binFactor3 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin3)
			binIndex = maxBin3;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin3
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin3);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin3"
			
		backgroundValue += binIndex;
		*offScreenPtr += dataDisplay3Ptr[binIndex];

		if (backgroundValueCode && !backgroundValue)
			{
			if (backgroundValueCode == 1)
				*offScreenPtr = 255;
			else	// backgroundValueCode == 2
				*offScreenPtr = 0;
				
			}	// end "if (backgroundValueCode && !backgroundValue)"
			
		#if defined multispec_lin
			#if defined multispec_wxmac
						// Leave high order byte blank.	
				offScreenPtr++;
			#endif
					// linux version require red, green, blue bytes be filled with same value
			offScreenValue = *offScreenPtr;
			offScreenPtr++;
			*offScreenPtr += offScreenValue;
			offScreenPtr++;
			*offScreenPtr += offScreenValue;
		
			#if defined multispec_wxlin
						// Leave low order (alpha) byte blank.
				offScreenPtr++;
			#endif
		#endif
		
		offScreenPtr++;

		}	// end "for (j=0; ..."
		
}	// end "Display3Channel4Byte8BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display3Channel16BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 3-channel,
//							16 bit color image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 01/05/2006
//	Revised By:			Larry L. Biehl			Date: 12/12/2018

void Display3Channel4Byte16BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				double								minValue1,
				double								minValue2,
				double								minValue3,
				double								binFactor1,
				double								binFactor2,
				double								binFactor3,
				UInt16								backgroundValueCode,
				HDoublePtr							ioBuffer1Ptr,
				HDoublePtr							ioBuffer2Ptr,
				HDoublePtr							ioBuffer3Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				HUCharPtr							dataDisplay3Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				UInt32								maxBin3,
				HUInt16Ptr							offScreen2BytePtr)

{	
	double								doubleBinIndex;
	
	UInt32								backgroundValue,
											binIndex,
											j;
							
	
	#if defined multispec_lin
			// 16-bit color not used in wxWidgets version
																						return;
	#endif
	
	for (j=0; j<numberSamples; j+=interval)
		{
		*offScreen2BytePtr = 0;
		
				// Red bits.					
						
		doubleBinIndex = (ioBuffer3Ptr[j] - minValue3)*binFactor3 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin3)
			binIndex = maxBin3;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin3
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin3);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin3"
			
		backgroundValue = binIndex;
		*offScreen2BytePtr = (dataDisplay3Ptr[binIndex] << 10);
		
				// Green bits.							
						
		doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin2)
			binIndex = maxBin2;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin2);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"
			
		backgroundValue += binIndex;
		*offScreen2BytePtr = 
					*offScreen2BytePtr | (dataDisplay2Ptr[binIndex] << 5);
		
				// Blue bits.					
						
		doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
		if (doubleBinIndex < 0)
			binIndex = 0;
		else if (doubleBinIndex > (double)maxBin1)
			binIndex = maxBin1;
		else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
			{
			binIndex = (UInt32)doubleBinIndex;
			
					// This will catch case when doubleBinIndex is nan or -nan. The
					// conversion from this can be a large unsigned 32-bit int.
					
			binIndex = MIN (binIndex, maxBin1);
			
			}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"
			
		backgroundValue += binIndex;
		*offScreen2BytePtr = *offScreen2BytePtr | dataDisplay1Ptr[binIndex];

		if (backgroundValueCode && !backgroundValue)
			{
			if (backgroundValueCode == 1) 
				*offScreen2BytePtr = 0x0000;
			else	// backgroundValueCode == 2 
				*offScreen2BytePtr = 0x7fff;
				
			}	// end "if (backgroundValueCode && !backgroundValue)"
			
		offScreen2BytePtr++;
		
		}	// end "for (j=0; ..."
	
}	// end "Display3Channel16BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display3Channel4Byte24BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 3-channel,
//							24 bit color image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 06/19/2003
//	Revised By:			Larry L. Biehl			Date: 01/22/2019

void Display3Channel4Byte24BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				double								minValue1,
				double								minValue2,
				double								minValue3,
				double								binFactor1,
				double								binFactor2,
				double								binFactor3,
				UInt16								backgroundValueCode,
				HDoublePtr							ioBuffer1Ptr,
				HDoublePtr							ioBuffer2Ptr,
				HDoublePtr							ioBuffer3Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				HUCharPtr							dataDisplay3Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				UInt32								maxBin3,
				HUCharPtr							offScreenPtr)

{	
	double								doubleBinIndex;
	
	UInt32								backgroundValue,
											binIndex,
											j;

	HUInt32Ptr							offScreen4BytePtr;
																			
		
	offScreen4BytePtr = (HUInt32Ptr)offScreenPtr;
		
	for (j=0; j<numberSamples; j+=interval)
		{
		#if defined multispec_mac || defined multispec_lin
			#if defined multispec_mac || defined multispec_wxmac
						// Leave high order byte blank.
		
				//*offScreenPtr = 0;
				offScreenPtr++;
			#endif	// defined multispec_mac || ...
					// Red byte.				
						
			doubleBinIndex = (ioBuffer3Ptr[j] - minValue3)*binFactor3 + 1;
			if (doubleBinIndex < 0)
				binIndex = 0;
			else if (doubleBinIndex > (double)maxBin3)
				binIndex = maxBin3;
			else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin3
				{
				binIndex = (UInt32)doubleBinIndex;
				
						// This will catch case when doubleBinIndex is nan or -nan. The
						// conversion from this can be a large unsigned 32-bit int.
						
				binIndex = MIN (binIndex, maxBin3);
				
				}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin3"
			
			backgroundValue = binIndex;
			*offScreenPtr = dataDisplay3Ptr [binIndex];
			offScreenPtr++;
			
					// Green byte.						
						
			doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
			if (doubleBinIndex < 0)
				binIndex = 0;
			else if (doubleBinIndex > (double)maxBin2)
				binIndex = maxBin2;
			else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
				{
				binIndex = (UInt32)doubleBinIndex;
				
						// This will catch case when doubleBinIndex is nan or -nan. The
						// conversion from this can be a large unsigned 32-bit int.
						
				binIndex = MIN (binIndex, maxBin2);
				
				}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin3"
			
			backgroundValue += binIndex;
			*offScreenPtr = dataDisplay2Ptr[binIndex];
			offScreenPtr++;
			
					// Blue byte.							
						
			doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
			if (doubleBinIndex < 0)
				binIndex = 0;
			else if (doubleBinIndex > (double)maxBin1)
				binIndex = maxBin1;
			else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
				{
				binIndex = (UInt32)doubleBinIndex;
				
						// This will catch case when doubleBinIndex is nan or -nan. The
						// conversion from this can be a large unsigned 32-bit int.
						
				binIndex = MIN (binIndex, maxBin1);
				
				}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"
			
			backgroundValue += binIndex;
			*offScreenPtr = dataDisplay1Ptr [binIndex];
			offScreenPtr++;
		
			#if defined multispec_wxlin
						// Leave low order (alpha) byte blank.
				offScreenPtr++;
			#endif

			if (backgroundValueCode && !backgroundValue)
				{
				if (backgroundValueCode == 1) 
					*offScreen4BytePtr = 0x00000000;
				else	// backgroundValueCode == 2
					#if TARGET_CPU_X86 || TARGET_CPU_X86_64
						*offScreen4BytePtr = 0xffffff00;
					#else	// !TARGET_CPU_X86
						*offScreen4BytePtr = 0x00ffffff;
					#endif	// TARGET_CPU_X86 ... else 
					
				}	// end "if (backgroundValueCode && !backgroundValue)"

			offScreen4BytePtr++;			
		#endif	// defined multispec_mac
		
		#if defined multispec_win
					// Blue byte.							
						
			doubleBinIndex = (ioBuffer1Ptr[j] - minValue1)*binFactor1 + 1;
			if (doubleBinIndex < 0)
				binIndex = 0;
			else if (doubleBinIndex > (double)maxBin1)
				binIndex = maxBin1;
			else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin1
				{
				binIndex = (UInt32)doubleBinIndex;
				
						// This will catch case when doubleBinIndex is nan or -nan. The
						// conversion from this can be a large unsigned 32-bit int.
						
				binIndex = MIN (binIndex, maxBin1);
				
				}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin1"
			
			backgroundValue = binIndex;
			*offScreenPtr = dataDisplay1Ptr [binIndex];
			offScreenPtr++;
			
					// Green byte.							
						
			doubleBinIndex = (ioBuffer2Ptr[j] - minValue2)*binFactor2 + 1;
			if (doubleBinIndex < 0)
				binIndex = 0;
			else if (doubleBinIndex > (double)maxBin2)
				binIndex = maxBin2;
			else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin2
				{
				binIndex = (UInt32)doubleBinIndex;
				
						// This will catch case when doubleBinIndex is nan or -nan. The
						// conversion from this can be a large unsigned 32-bit int.
						
				binIndex = MIN (binIndex, maxBin2);
				
				}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin2"
			
			backgroundValue += binIndex;
			*offScreenPtr = dataDisplay2Ptr [binIndex];
			offScreenPtr++; 
		
					// Red byte.						
						
			doubleBinIndex = (ioBuffer3Ptr[j] - minValue3)*binFactor3 + 1;
			if (doubleBinIndex < 0)
				binIndex = 0;
			else if (doubleBinIndex > (double)maxBin3)
				binIndex = maxBin3;
			else	// doubleBinIndex >= 0 && doubleBinIndex <= maxBin3
				{
				binIndex = (UInt32)doubleBinIndex;
				
						// This will catch case when doubleBinIndex is nan or -nan. The
						// conversion from this can be a large unsigned 32-bit int.
						
				binIndex = MIN (binIndex, maxBin3);
				
				}	// end "else doubleBinIndex >= 0 && doubleBinIndex <= maxBin3"
			
			backgroundValue += binIndex;
			*offScreenPtr = dataDisplay3Ptr [binIndex];
			offScreenPtr++; 
			
			if (backgroundValueCode && !backgroundValue)
				{
				if (backgroundValueCode == 1) 
					{
					offScreenPtr -= 3;
					*offScreenPtr = (UInt8)0x00;
					offScreenPtr++;
					*offScreenPtr = (UInt8)0x00;
					offScreenPtr++;
					*offScreenPtr = (UInt8)0x00;
					offScreenPtr++;
					
					}	//  end "if (backgroundValueCode == 1)"
					
				else	// backgroundValueCode == 2
					{
					offScreenPtr -= 3;
					*offScreenPtr = (UInt8)0xff;
					offScreenPtr++;
					*offScreenPtr = (UInt8)0xff;
					offScreenPtr++;
					*offScreenPtr = (UInt8)0xff;
					offScreenPtr++;
					
					}	//  end "else backgroundValueCode == 2"
				
				}	// end "if (backgroundValueCode && ...)"
		#endif	// defined multispec_win
		
		}	// end "for (j=0; ..."
	
}	// end "Display3Channel4Byte24BitLine"
