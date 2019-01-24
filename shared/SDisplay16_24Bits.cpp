//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SDisplay16_24Bits.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/21/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	The routines in this file handle displaying 2 and 3 channel in 16 
//								and 24 bit color
//
//	Functions in file:	void 		Display2C16BImage
//								void 		Display2C24BImage
//								void 		Display3C1624BImage
//								void		UpdatePaletteFor16and24BImage	 
//
//      Linux Note: wxImage constructor always requires image data in rgb raster format
//      This is similar to mac
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif	

//#include "SExtGlob.h"

			// Prototypes for file routines that are only called from other 		
			// routines in this file.



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display1Channel16BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 1-channel
//							gray-scale image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 09/14/2001
//	Revised By:			Larry L. Biehl			Date: 01/21/2019

void Display1Channel16BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				Boolean								bytesEqualOneFlag,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBufferPtr,
				HUCharPtr							dataDisplayPtr,
				UInt32								maxBin,
				HUInt16Ptr							offScreen2BytePtr)

{	
	UInt32								backgroundValue,
											dataValue,
											j;
											

	if (!gOSXCoreGraphicsFlag)
		{
		for (j=0; j<numberSamples; j+=interval)
			{				
			dataValue = (bytesEqualOneFlag) ?
					(UInt32)ioBufferPtr->data.onebyte[j] :
						(UInt32)ioBufferPtr->data.twobyte[j];
			if (dataValue > maxBin)  
				dataValue = 0;
				
			backgroundValue = dataValue;
				
			dataValue = dataDisplayPtr[dataValue];
            
			#ifndef multispec_lin
						// Red bits.															
						
				*offScreen2BytePtr = (SInt16)(dataValue << 10);
				
						// Green bits.	
					
				*offScreen2BytePtr = *offScreen2BytePtr | (SInt16)(dataValue << 5);
				
						// Blue bits.															
				
				*offScreen2BytePtr = *offScreen2BytePtr | (SInt16)dataValue;

				if (backgroundValueCode && !backgroundValue)
					{
					if (backgroundValueCode == 1) 
						*offScreen2BytePtr = 0x0000;
					else		// backgroundValueCode == 2
						*offScreen2BytePtr = 0x7fff;
						
					}		// end "if (backgroundValueCode && !backgroundValue)"

				#if TARGET_CPU_X86 || TARGET_CPU_X86_64
					Swap2Bytes ((HUInt16Ptr)offScreen2BytePtr, 1);
				#endif		// TARGET_CPU_X86 ... else 
					
				offScreen2BytePtr++;
			#else	// defined multispec_lin
						// First define a pointer of size byte
						
            HUCharPtr imageptr;
            imageptr = (HUCharPtr)offScreen2BytePtr;
				#if defined multispec_wxmac
							// Leave high order (alpha) byte blank.
					imageptr++;
				#endif
            if (backgroundValueCode && !backgroundValue)
					{
               if (backgroundValueCode == 1) 
						{
                  *imageptr = (Byte)(0x00);
                  imageptr++;

                  *imageptr = (Byte)(0x00);
                  imageptr++;

                  *imageptr = (Byte)(0x00);
                  imageptr++;
						
						}	// end "if (backgroundValueCode == 1)"
						
               else	// else backgroundValueCode != 1
						{
                  *imageptr = (Byte)(0xff);
                  imageptr++;

                  *imageptr = (Byte)(0xff);
                  imageptr++;

                  *imageptr = (Byte)(0xff);
                  imageptr++;
						
						}	// end "else backgroundValueCode != 1"
					
					}	// end "if (zeroAsBackgroundFlag && !backgroundValue)"
					
            else	// !zeroAsBackgroundFlag || backgroundValue
					{
							// Red bits
					*imageptr = (Byte)(dataValue);
					imageptr++;
							// Green bits
					*imageptr = (Byte)(dataValue);
					imageptr++;
							// Blue bits
					*imageptr = (Byte)(dataValue);
					imageptr++;

					}	// else !zeroAsBackgroundFlag || backgroundValue
					
				#if defined multispec_wxlin
							// Leave lower order (alpha) byte blank.
					imageptr++;
				#endif
					
            offScreen2BytePtr = (HUInt16Ptr)imageptr;
			#endif	// !defined multispec_lin, else

			}	// end "for (j=0; ..."
			
		}	// end "if (!gOSXCoreGraphicsFlag)"

}	// end "Display1Channel16BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display2Channel16BitLine
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
//	Coded By:			Larry L. Biehl			Date: 07/12/1988
//	Revised By:			Larry L. Biehl			Date: 01/21/2019

void Display2Channel16BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				HUInt16Ptr							offScreen2BytePtr,
				SInt16								rgbColors)

{	
	UInt32								backgroundValue,
											dataValue,
											j;
							
	HUCharPtr imageptr;
   imageptr = (HUCharPtr)offScreen2BytePtr;
	switch (rgbColors)
		{
		case kGBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac || defined multispec_wxmac
							// Set high order bit and Red bits to 0).						
						
					*offScreen2BytePtr = 0;
				#endif	// defined multispec_mac ||

				#if defined multispec_lin
							// Set Red bits to zero
							
					if (backgroundValueCode == 2)
						{
						*imageptr = 0xff;
						imageptr++;
						}
						
                else	// backgroundValueCode != 2
						{
                  *imageptr = (Byte)(0x00);
                  imageptr++;
						}
				#endif
				
						// Green bits.															
				
				dataValue = (bytesEqualOneFlag2) ?
						(UInt32)ioBuffer2Ptr->data.onebyte[j] :
							(UInt32)ioBuffer2Ptr->data.twobyte[j];
				if (dataValue > maxBin2)  
					dataValue = 0;
					
				backgroundValue = dataValue;
				#if defined multispec_lin
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 2)
							{
							*imageptr = 0xff;
							imageptr++;
							
							}	// end "if (backgroundValueCode == 2)"
							
						else	// backgroundValueCode != 2
							{
							*imageptr = 0x00;
							imageptr++;  
							
							}
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
						
					else
						{
						*imageptr = (Byte)dataDisplay2Ptr[dataValue];
						imageptr++;
						}
				#else
					*offScreen2BytePtr = (dataDisplay2Ptr[dataValue] << 5);
				#endif
						// Blue bits.															
				
				dataValue = (bytesEqualOneFlag1) ?
						(UInt32)ioBuffer1Ptr->data.onebyte[j] :
							(UInt32)ioBuffer1Ptr->data.twobyte[j];
				if (dataValue > maxBin1)  
					dataValue = 0;
				backgroundValue += dataValue;
				#if defined multispec_lin
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 2)
							{
							*imageptr = 0xff;
							imageptr++;
							}
							
						else 
							{
							*imageptr = 0x00;
							imageptr++;
							}
						}
						
					else
						{
						*imageptr = (Byte)dataDisplay1Ptr[dataValue];
						imageptr++;
						}
					
					#if defined multispec_wxlin
								// Leave lower order (alpha) byte blank.
						imageptr++;
					#endif
						
					offScreen2BytePtr = (HUInt16Ptr)imageptr;
				#else	// !defined multispec_lin
					*offScreen2BytePtr = 
								*offScreen2BytePtr | (dataDisplay1Ptr[dataValue]);

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1) 
							*offScreen2BytePtr = 0x0000;
						else		// backgroundValueCode == 2
							*offScreen2BytePtr = 0x7fff;
						
						}	// end "if (backgroundValueCode && !backgroundValue)"

					#if TARGET_CPU_X86 || TARGET_CPU_X86_64
						Swap2Bytes ((HUInt16Ptr)offScreen2BytePtr, 1);
					#endif		// TARGET_CPU_X86 ... else 
					
					offScreen2BytePtr++;
				#endif
				}	// end "for (j=0; ..." 
			break;
			
		case kRBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac || defined multispec_wxmac
							// Set high order bit and Green bits to 0).						
						
					*offScreen2BytePtr = 0;
				#endif	// defined multispec_win	
						
						// Red bits.															
						
				dataValue = (bytesEqualOneFlag2) ?
						(UInt32)ioBuffer2Ptr->data.onebyte[j] :
							(UInt32)ioBuffer2Ptr->data.twobyte[j];
				if (dataValue > maxBin2)  
					dataValue = 0;
				backgroundValue = dataValue;
				#if defined multispec_lin
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 2)
							{
							*imageptr = 0xff;
							imageptr++;
							}
							
						else 
							{
							*imageptr = 0x00;
							imageptr++;
							}
						}
						
					else
						{
						*imageptr = (Byte)dataDisplay2Ptr[dataValue];
						imageptr++;
						}
				#else
					*offScreen2BytePtr = (dataDisplay2Ptr[dataValue] << 10);
				#endif

				#if defined multispec_lin
							// Set Green bits to zero
					if (backgroundValueCode && !backgroundValue)
						{
						*imageptr = 0xff;
						imageptr++;
						}
						
					else
						{
						*imageptr = (Byte)(0x00);
						imageptr++;
						}
				#endif
				
						// Blue bits.															
				
				dataValue = (bytesEqualOneFlag1) ?
						(UInt32)ioBuffer1Ptr->data.onebyte[j] :
							(UInt32)ioBuffer1Ptr->data.twobyte[j];
				if (dataValue > maxBin1)  
					dataValue = 0;
				backgroundValue += dataValue;
				#if defined multispec_lin
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 2)
							{
							*imageptr = 0xff;
							imageptr++;
							}
						else 
							{
							*imageptr = 0x00;
							imageptr++;
							}
							
						}	// end "backgroundValueCode && !backgroundValue"
						
					else
						{
						*imageptr = (Byte)dataDisplay1Ptr[dataValue];
						imageptr++;
						}
					
					#if defined multispec_wxlin
								// Leave lower order (alpha) byte blank.
						imageptr++;
					#endif
						
					offScreen2BytePtr = (HUInt16Ptr)imageptr;
				#else
					*offScreen2BytePtr = 
								*offScreen2BytePtr | (dataDisplay1Ptr[dataValue]);

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1) 
							*offScreen2BytePtr = 0x0000;
						else		// backgroundValueCode == 2
							*offScreen2BytePtr = 0x7fff;
							
						}	// end "if (backgroundValueCode && !backgroundValue)"

					#if TARGET_CPU_X86 || TARGET_CPU_X86_64
						Swap2Bytes ((HUInt16Ptr)offScreen2BytePtr, 1);
					#endif		// TARGET_CPU_X86 ... else 
						
					offScreen2BytePtr++;
				#endif
				}	// end "for (j=0; ..." 
			break;
			
		case kRGColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac || defined mulispec_wxmac
							// Set high order bit and Blue bits to 0).						
						
					*offScreen2BytePtr = 0;
				#endif	// defined multispec_win	
				
						// Red bits.															
						
				dataValue = (bytesEqualOneFlag2) ?
						(UInt32)ioBuffer2Ptr->data.onebyte[j] :
							(UInt32)ioBuffer2Ptr->data.twobyte[j];
				if (dataValue > maxBin2)  
					dataValue = 0;
				backgroundValue = dataValue;
				#if defined multispec_lin
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 2)
							{
							*imageptr = 0xff;
							imageptr++;
							}
							
						else 
							{
							*imageptr = 0x00;
							imageptr++;
							}
						}
						
					else		// !zeroAsBackgroundFlag || backgroundValue
						{
						*imageptr = (Byte)dataDisplay2Ptr[dataValue];
						imageptr++;
						}
				#else	// !defined multispec_lin
					*offScreen2BytePtr = (dataDisplay2Ptr[dataValue] << 10);
				#endif	// defined multispec_lin, else
				
						// Green bits.															
				
				dataValue = (bytesEqualOneFlag1) ?
						(UInt32)ioBuffer1Ptr->data.onebyte[j] :
							(UInt32)ioBuffer1Ptr->data.twobyte[j];
				if (dataValue > maxBin1)  
					dataValue = 0;
				backgroundValue += dataValue;
				#if defined multispec_lin
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 2)
							{
							*imageptr = 0xff;
							imageptr++;
							}
						else 
							{
							*imageptr = 0x00;
							imageptr++;
							}
						}
						
					else
						{
						*imageptr = (Byte)dataDisplay2Ptr[dataValue];
						imageptr++;
						}
                
				#else	// !defined multispec_lin
					*offScreen2BytePtr = 
							*offScreen2BytePtr | (dataDisplay1Ptr[dataValue] << 5);

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1) 
							*offScreen2BytePtr = 0x0000;
						else		// backgroundValueCode == 2
							*offScreen2BytePtr = 0x7fff;
							
						}	// end "if (backgroundValueCode && !backgroundValue)"

					#if TARGET_CPU_X86 || TARGET_CPU_X86_64
						Swap2Bytes ((HUInt16Ptr)offScreen2BytePtr, 1);
					#endif	// TARGET_CPU_X86 ... else 
					
					offScreen2BytePtr++;
				#endif	// defined multispec_lin, else
				
						// Blue bits
				#if defined multispec_lin
						// Set Blue bits to zero
					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 2)
							{
							*imageptr = 0xff;
							imageptr++;
							}
						else 
							{
							*imageptr = 0x00;
							imageptr++;
							}
						}
                
					else
						{
						*imageptr = (Byte)(0x00);
						imageptr++;
						}
					
					#if defined multispec_wxlin
								// Leave lower order (alpha) byte blank.
						imageptr++;
					#endif
						
					offScreen2BytePtr = (HUInt16Ptr)imageptr;
				#endif
				}	// end "for (j=0; ..." 
			break;
			
		}	// end "switch (displayPalette.rgbColors)" 
	
}	// end "Display2Channel16BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display2Channel24BitLine
//
//	Software purpose:	The purpose of this routine is to copy the input
//							line of data to the offscreen buffer for a 2-channel,
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
//	Coded By:			Larry L. Biehl			Date: 07/12/1988
//	Revised By:			Larry L. Biehl			Date: 01/21/2019

void Display2Channel24BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				HUCharPtr							offScreenPtr,
				SInt16								rgbColors)

{	
	UInt32								backgroundValue,
											dataValue,
											j;

	HUInt32Ptr							offScreen4BytePtr;
																			
		
	offScreen4BytePtr = (HUInt32Ptr)offScreenPtr;
			
	switch (rgbColors)
		{
		case kGBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{		 
				#if defined multispec_mac
							// Leave high order byte blank.					
						
					offScreenPtr++;
				
							// Set Red byte to zero.											
							
					*offScreenPtr = 0;
					offScreenPtr++;
					
							// Green byte.															
					
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
					offScreenPtr++;
					
							// Blue byte.															
					
					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
								(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++; 

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1) 
							*offScreen4BytePtr = 0x00000000;
						else		// backgroundValueCode == 2
							#if TARGET_CPU_X86 || TARGET_CPU_X86_64
								*offScreen4BytePtr = 0xffffff00;
							#else		// !TARGET_CPU_X86
								*offScreen4BytePtr = 0x00ffffff;
							#endif		// TARGET_CPU_X86 ... else
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
		
					offScreen4BytePtr++;
				#endif	// defined multispec_mac
				
				#if defined multispec_win
							// Blue byte.															
					
					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
								(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++;
					
							// Green byte.															
					
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
					offScreenPtr++; 
				
							// Set Red byte to zero.											
							
					*offScreenPtr = 0;
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
							
						else		// backgroundValueCode == 2 
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

				#if defined multispec_lin
					#if defined multispec_wxmac
								// Leave high order (alpha) byte blank.
						offScreenPtr++;
					#endif
						// Set Red byte to zero.

					*offScreenPtr = 0;
					offScreenPtr++;

							// Green byte.

					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
									(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)
					  dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
					offScreenPtr++;

							// Blue byte.

					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
									(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)
					  dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++;
					
					#if defined multispec_wxlin
								// Leave lower order (alpha) byte blank.
						offScreenPtr++;
					#endif

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1)
							{
							offScreenPtr -= 3;
							*offScreenPtr = (UInt8) 0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8) 0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8) 0xff;
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
						
						}	// end "if (backgroundValueCode && !backgroundValue)"
				#endif	// defined multispec_lin

				}	// end "for (j=0; ..."
			
			break;
			
		case kRBColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac
							// Leave high order byte blank.					
						
					offScreenPtr++;
				
							// Red byte.															
							
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
					offScreenPtr++;
					
							// Set Green byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++;
					
							// Blue byte.															
					
					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
								(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++;

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1) 
							*offScreen4BytePtr = 0x00000000;
						else		// backgroundValueCode == 2
							#if TARGET_CPU_X86 || TARGET_CPU_X86_64
								*offScreen4BytePtr = 0xffffff00;
							#else		// !TARGET_CPU_X86
								*offScreen4BytePtr = 0x00ffffff;
							#endif		// TARGET_CPU_X86 ... else
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
						
					offScreen4BytePtr++;
				#endif	// defined multispec_mac
				
				#if defined multispec_win
							// Blue byte.															
					
					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
									(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++; 
					
							// Set Green byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++; 
				
							// Red byte.															
							
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
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
							
							}		//  end "if (backgroundValueCode == 1)"
							
						else		// backgroundValueCode == 2 
							{
							offScreenPtr -= 3;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							*offScreenPtr = (UInt8)0xff;
							offScreenPtr++;
							
							}		//  end "else backgroundValueCode == 2"
						
						}		// end "if (backgroundValueCode && ...)"
					#endif	// defined multispec_win
				
					#if defined multispec_lin
						#if defined multispec_wxmac
									// Leave high order (alpha) byte blank.
							offScreenPtr++;
						#endif
							// Red byte.
							
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
									(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
					offScreenPtr++; 

							// Set Green byte to zero.											

					*offScreenPtr = 0;
					offScreenPtr++; 

							// Blue byte.															

					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
									(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++;
					
					#if defined multispec_wxlin
								// Leave lower order (alpha) byte blank.
						offScreenPtr++;
					#endif
									
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
				#endif	// defined multispec_lin
				}	// end "for (j=0; ..."
			
			break;
			
		case kRGColor:	
			for (j=0; j<numberSamples; j+=interval)
				{
				#if defined multispec_mac
							// Leave high order byte blank.					
						
					offScreenPtr++; 
				
							// Red byte.															
							
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
									(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
					offScreenPtr++;
					
							// Green byte.															
					
					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
									(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++;
					
							// Set Blue byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++;

					if (backgroundValueCode && !backgroundValue)
						{
						if (backgroundValueCode == 1) 
							*offScreen4BytePtr = 0x00000000;
						else		// backgroundValueCode == 2
							#if TARGET_CPU_X86 || TARGET_CPU_X86_64
								*offScreen4BytePtr = 0xffffff00;
							#else		// !TARGET_CPU_X86
								*offScreen4BytePtr = 0x00ffffff;
							#endif		// TARGET_CPU_X86 ... else
							
						}	// end "if (backgroundValueCode && !backgroundValue)"
						
					offScreen4BytePtr++;
				#endif	// defined multispec_mac
				
				#if defined multispec_win
							// Set Blue byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++;
					
							// Green byte.															
					
					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
									(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++; 
				
							// Red byte.															
							
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
									(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
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
				
				#if defined multispec_lin
					#if defined multispec_wxmac
								// Leave high order (alpha) byte blank.
						offScreenPtr++;
					#endif
						// Red byte.
							
					dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
					if (dataValue > maxBin2)  
						dataValue = 0;
					backgroundValue += dataValue;
					*offScreenPtr = dataDisplay2Ptr[dataValue];
					offScreenPtr++;
               
               		// Green byte.															
					
					dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
								(UInt32)ioBuffer1Ptr->data.twobyte[j];
					if (dataValue > maxBin1)  
						dataValue = 0;
					backgroundValue = dataValue;
					*offScreenPtr = dataDisplay1Ptr[dataValue];
					offScreenPtr++; 
				
               		// Set Blue byte to zero.											
					
					*offScreenPtr = 0;
					offScreenPtr++;
					
					#if defined multispec_wxlin
								// Leave lower order (alpha) byte blank.
						offScreenPtr++;
					#endif
               
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
							
							}		//  end "if (backgroundValueCode == 1)"
							
						else		// backgroundValueCode == 2 
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
					
				#endif// defined multispec_lin
				}	// end "for (j=0; ..."
			
			break;
			
		}	// end "switch (rgbColors)"
	
}	// end "Display2Channel24BitLine"



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
//	Coded By:			Larry L. Biehl			Date: 08/04/1989
//	Revised By:			Larry L. Biehl			Date: 01/21/2019

void Display3Channel16BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				Boolean								bytesEqualOneFlag3,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HFileIOBufferPtr					ioBuffer3Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				HUCharPtr							dataDisplay3Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				UInt32								maxBin3,
				HUInt16Ptr							offScreen2BytePtr)

{	
	HUCharPtr							offScreenPtr;
	
	UInt32								backgroundValue,
											dataValue,
											j;
	
	
    offScreenPtr = (HUCharPtr)offScreen2BytePtr;
    for (j = 0; j < numberSamples; j += interval)
		{
		#if defined multispec_mac || defined multispec_win
			*offScreen2BytePtr = 0;

					// Red bits.																
					
			dataValue = backgroundValue = (bytesEqualOneFlag3) ?
							(UInt32)ioBuffer3Ptr->data.onebyte[j] :
								(UInt32)ioBuffer3Ptr->data.twobyte[j];
			if (dataValue > maxBin3)  
				dataValue = 0;
				
			*offScreen2BytePtr = (dataDisplay3Ptr[dataValue] << 10);

					// Green bits.																
					
			dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
			if (dataValue > maxBin2)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreen2BytePtr = 
						*offScreen2BytePtr | (dataDisplay2Ptr[dataValue] << 5);

					// Blue bits.																
					
			dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
									(UInt32)ioBuffer1Ptr->data.twobyte[j];
			if (dataValue > maxBin1)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreen2BytePtr = *offScreen2BytePtr | dataDisplay1Ptr[dataValue];

			if (backgroundValueCode && !backgroundValue)
				{
				if (backgroundValueCode == 1) 
					*offScreen2BytePtr = 0x0000;
				else		// backgroundValueCode == 2
					*offScreen2BytePtr = 0x7fff;
					
				}		// end "if (backgroundValueCode && !backgroundValue)"

			#if TARGET_CPU_X86 || TARGET_CPU_X86_64
				Swap2Bytes ((HUInt16Ptr)offScreen2BytePtr, 1);
			#endif		// TARGET_CPU_X86 ... else
							
			offScreen2BytePtr++;
		#endif	// defined multispec_mac || defined multispec_win
		
		#if defined multispec_lin
			#if defined multispec_wxmac
						// Leave high order (alpha) byte blank.
				offScreenPtr++;
			#endif
					// Red bits.
      
			dataValue = backgroundValue = (bytesEqualOneFlag3) ?
					 (UInt32)ioBuffer3Ptr->data.onebyte[j] :
							(UInt32)ioBuffer3Ptr->data.twobyte[j];
			if (dataValue > maxBin3)
				dataValue = 0;
			*offScreenPtr = (Byte)(dataDisplay3Ptr[dataValue]);
			offScreenPtr++;
		
					// Green bits.

			dataValue = (bytesEqualOneFlag2) ?
					 (UInt32)ioBuffer2Ptr->data.onebyte[j] :
							(UInt32)ioBuffer2Ptr->data.twobyte[j];
			if (dataValue > maxBin2)
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay2Ptr[dataValue];
			offScreenPtr++;
		
					// Blue bits.

			dataValue = (bytesEqualOneFlag1) ?
					 (UInt32)ioBuffer1Ptr->data.onebyte[j] :
							(UInt32)ioBuffer1Ptr->data.twobyte[j];
			if (dataValue > maxBin1)
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay1Ptr[dataValue];
			offScreenPtr++;
		
			if (backgroundValueCode && !backgroundValue)
				{
				offScreenPtr -= 3;
				if (backgroundValueCode == 1) 
					{
					*offScreenPtr = (UInt8) 0x00;
					offScreenPtr++;
					*offScreenPtr = (UInt8) 0x00;
					offScreenPtr++;
					*offScreenPtr = (UInt8) 0x00;
					offScreenPtr++;
					
					}	// end "if (backgroundValueCode == 1) "
				
				else
					{
					*offScreenPtr = (UInt8) 0xff;
					offScreenPtr++;
					*offScreenPtr = (UInt8) 0xff;
					offScreenPtr++;
					*offScreenPtr = (UInt8) 0xff;
					offScreenPtr++;
					
					}	// end "if (backgroundValueCode != 1) "

				}	// end "if (zeroAsBackgroundFlag && ...)"
		
			#if defined multispec_wxlin
						// Leave lower order (alpha) byte blank.
				offScreenPtr++;
			#endif

			offScreen2BytePtr = (HUInt16Ptr)offScreenPtr;
		#endif
		
		}	// end "for (j=0; ..."
	
}	// end "Display3Channel16BitLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Display3Channel24BitLine
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
//	Coded By:			Larry L. Biehl			Date: 08/04/1989
//	Revised By:			Larry L. Biehl			Date: 01/21/2019

void Display3Channel24BitLine (
				UInt32								numberSamples,
				UInt32								interval, 
				Boolean								bytesEqualOneFlag1,
				Boolean								bytesEqualOneFlag2,
				Boolean								bytesEqualOneFlag3,
				UInt16								backgroundValueCode,
				HFileIOBufferPtr					ioBuffer1Ptr,
				HFileIOBufferPtr					ioBuffer2Ptr,
				HFileIOBufferPtr					ioBuffer3Ptr,
				HUCharPtr							dataDisplay1Ptr,
				HUCharPtr							dataDisplay2Ptr,
				HUCharPtr							dataDisplay3Ptr,
				UInt32								maxBin1,
				UInt32								maxBin2,
				UInt32								maxBin3,
				HUCharPtr							offScreenPtr)

{	
	UInt32								backgroundValue,
											dataValue,
											j;

	#if defined multispec_mac
		HUInt32Ptr							offScreen4BytePtr = (UInt32*)offScreenPtr;
	#endif

	for (j=0; j<numberSamples; j+=interval)
		{
		#if defined multispec_mac
					// Leave high order byte blank.					
				
			//*offScreenPtr = 0;
			offScreenPtr++;
		
					// Red byte.																
					
			dataValue = backgroundValue = (bytesEqualOneFlag3) ?
							(UInt32)ioBuffer3Ptr->data.onebyte[j] :
								(UInt32)ioBuffer3Ptr->data.twobyte[j];
			if (dataValue > maxBin3)  
				dataValue = 0;
			*offScreenPtr = dataDisplay3Ptr [dataValue];
			offScreenPtr++;
			
					// Green byte.																
					
			dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
			if (dataValue > maxBin2)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay2Ptr [dataValue];
			offScreenPtr++;
			
					// Blue byte.																
					
			dataValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
								(UInt32)ioBuffer1Ptr->data.twobyte[j];
			if (dataValue > maxBin1)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay1Ptr [dataValue];
			offScreenPtr++;

			if (backgroundValueCode && !backgroundValue)
				{
				if (backgroundValueCode == 1) 
					*offScreen4BytePtr = 0x00000000;
				else		// backgroundValueCode == 2
					#if TARGET_CPU_X86 || TARGET_CPU_X86_64
						*offScreen4BytePtr = 0xffffff00;
					#else		// !TARGET_CPU_X86
						*offScreen4BytePtr = 0x00ffffff;
					#endif		// TARGET_CPU_X86 ... else
					
				}	// end "if (backgroundValueCode && !backgroundValue)"

			offScreen4BytePtr++;
		#endif	// defined multispec_mac
		
		#if defined multispec_win
					// Blue byte.																
					
			dataValue = backgroundValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
								(UInt32)ioBuffer1Ptr->data.twobyte[j];
			if (dataValue > maxBin1)  
				dataValue = 0;
			*offScreenPtr = dataDisplay1Ptr [dataValue];
			offScreenPtr++; 
			
					// Green byte.																
					
			dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
			if (dataValue > maxBin2)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay2Ptr [dataValue];
			offScreenPtr++; 
		
					// Red byte.																
					
			dataValue = (bytesEqualOneFlag3) ?
							(UInt32)ioBuffer3Ptr->data.onebyte[j] :
								(UInt32)ioBuffer3Ptr->data.twobyte[j];
			if (dataValue > maxBin3)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay3Ptr [dataValue];
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

		#if defined multispec_lin
			#if defined multispec_wxmac
						// Leave high order (alpha) byte blank.
				offScreenPtr++;
			#endif
		
               // Red byte.																
					
			dataValue = (bytesEqualOneFlag3) ?
							(UInt32)ioBuffer3Ptr->data.onebyte[j] :
								(UInt32)ioBuffer3Ptr->data.twobyte[j];
			if (dataValue > maxBin3)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay3Ptr [dataValue];
			offScreenPtr++;
         
         		// Green byte.																
					
			dataValue = (bytesEqualOneFlag2) ?
							(UInt32)ioBuffer2Ptr->data.onebyte[j] :
								(UInt32)ioBuffer2Ptr->data.twobyte[j];
			if (dataValue > maxBin2)  
				dataValue = 0;
			backgroundValue += dataValue;
			*offScreenPtr = dataDisplay2Ptr [dataValue];
			offScreenPtr++; 
			
					// Blue byte.																
					
			dataValue = backgroundValue = (bytesEqualOneFlag1) ?
							(UInt32)ioBuffer1Ptr->data.onebyte[j] :
								(UInt32)ioBuffer1Ptr->data.twobyte[j];
			if (dataValue > maxBin1)  
				dataValue = 0;
			*offScreenPtr = dataDisplay1Ptr [dataValue];
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
					
				else		// backgroundValueCode == 2 
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
		
			#if defined multispec_wxlin
						// Leave lower order (alpha) byte blank.
				offScreenPtr++;
			#endif
			
		#endif	// defined multispec_lin
		}	// end "for (j=0; ..."
	
}	// end "Display3Channel24BitLine"


 
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdatePaletteFor16and24BImage
//
//	Software purpose:	The purpose of this routine is to update the palette
//							for the 24 bit offscreen image.  The new palette will
//							be based on the best one that the Picture Utilities
//							package can do for the colors that are in the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None				
// 
// Called By:			DisplayColorImage in display.c
//
//	Coded By:			Larry L. Biehl			Date: 03/03/1992
//	Revised By:			Larry L. Biehl			Date: 10/21/1999

void UpdatePaletteFor16and24BImage (void)

{
	PictInfo								thePictInfo;
	Rect									rect;
	
	CTabPtr								cTabPtr;
	DisplaySpecsPtr					displaySpecsPtr = NULL; 
	
	CTabHandle							tempCTableHandle;
	GWorldFlags							gWorldFlags;
	
	Handle								displaySpecsHandle,
											windowInfoHandle;
	
	SInt16								handleStatus,
											numberEntries,
											resultCode;
											
	Boolean								continueFlag = TRUE;
		    		
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
	if (gQuickDrawVersion < 0x00000230)
																									return;
		
			// If all monitors are set for more than 8 bits of color then we
			// do not need to create a palette for 8 bits or fewer.
																									
	//if (GetMinSystemPixelSize () > 8)
	//																								return;
			
	continueFlag = SetUpActiveImageInformationGlobals (
									&windowInfoHandle, &handleStatus, kDoNotUseProject);
									
	if (continueFlag)
		{
		displaySpecsPtr = GetActiveDisplaySpecsPtr (&displaySpecsHandle);
		continueFlag = (displaySpecsPtr != NULL);
		
		}	// end "if (continueFlag)"
				
	if (continueFlag)
		{																	
	   gProcessorCode = kBetter8BitPaletteProcessor;
		
				// Determine the number of palette entries to get based on whether	
				// the display is in 4 bit mode or 8 or higher mode.						
				// This can be determined from the current number of palette 			
				// entries.																				
				
		numberEntries = 240;
		if (displaySpecsPtr->paletteObject &&
									(*(displaySpecsPtr->paletteObject))->pmEntries < 20)
			numberEntries = 14;
																							
		resultCode = GetPixMapInfo (
								(PixMapHandle)gImageWindowInfoPtr->offScreenMapHandle, 
								&thePictInfo, 
								returnPalette,
								numberEntries,
								systemMethod,
								0);
		
		if (resultCode == noErr)
			{
			if (displaySpecsPtr->paletteObject)
				DisposePalette (displaySpecsPtr->paletteObject);
			displaySpecsPtr->paletteObject = thePictInfo.thePalette;
			displaySpecsPtr->numPaletteEntriesUsed = 
												(*(displaySpecsPtr->paletteObject))->pmEntries;
										
					// Get a temporary color table handle.										
					
			numberEntries = (*(displaySpecsPtr->paletteObject))->pmEntries;
			
			tempCTableHandle = (CTabHandle)MNewHandle (
						(SInt32)numberEntries*sizeof (ColorSpec) + sizeof (ColorTable));
			if (tempCTableHandle == NULL)					
																							return;
					
					// Now initialize the table with the palette entries.					
																							
			Palette2CTab (displaySpecsPtr->paletteObject, tempCTableHandle);
			cTabPtr = *tempCTableHandle;
			cTabPtr->ctFlags = cTabPtr->ctFlags | 0x4000;
			
					// Dispose of a place holder pointer so that UpdateGWorld will use
					// that memory and not some memory that will end up being in the	
					// middle of the heap.															
					
			if (gNumberAvailableGWorldPtrs > 0)
				{
				gNumberAvailableGWorldPtrs--;
				if (gGWorldPlaceHolderList[gNumberAvailableGWorldPtrs])
					DisposePtr (gGWorldPlaceHolderList[gNumberAvailableGWorldPtrs]);
				gGWorldPlaceHolderList[gNumberAvailableGWorldPtrs] = NULL;
				
				}	// end "if (gNumberAvailableGWorldPtrs > 0)"
				
			rect.top = 0;
			rect.left = 0;	
			rect.bottom = displaySpecsPtr->imageDimensions[kVertical];
			rect.right = displaySpecsPtr->imageDimensions[kHorizontal];
		
			gWorldFlags = UpdateGWorld (&gImageWindowInfoPtr->offscreenGWorld, 
												displaySpecsPtr->pixelSize, 
												&rect, 
												tempCTableHandle, 
												NULL, 
												clipPix);
												
			displaySpecsPtr->better8BitPaletteFlag = TRUE;
							
			UnlockAndDispose ((Handle)tempCTableHandle);
			
			if (gImageWindowInfoPtr->offscreenGWorld != NULL)
				gImageWindowInfoPtr->offScreenMapHandle = 
										(Handle)GetPortPixMap (gImageWindowInfoPtr->offscreenGWorld);
							
			else		// gImageWindowInfoPtr->offscreenGWorld == NULL
				gImageWindowInfoPtr->offScreenMapHandle = NULL;
										
			NSetPalette	(gActiveImageWindow, thePictInfo.thePalette, pmAllUpdates);
			ActivatePalette (gActiveImageWindow);
			
			}	// end "if (resultCode == noErr)"
							
	   gProcessorCode = 0;
	   
	   gUpdatePaletteMenuItemsFlag = TRUE;
		
		}		// end "if (continueFlag)" 
		
	CheckAndUnlockHandle (displaySpecsHandle);
		
	UnlockActiveImageInformationGlobals (handleStatus);
	
}	// end "UpdatePaletteFor16and24BImage" 
#endif	// defined multispec_mac
