//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								Copyright (1988-2019)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SStrings.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/19/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec that are 
//								shared in the Mac and Windows versions.
//
//	Functions in file:	void							CheckStringLength
//								Boolean 						CheckTextWindowSpaceNeeded
//								Boolean 						CompareSuffixNoCase
//								StringPtr 					ConcatPStrings
//								void 							CopyPToP
//								CharPtr 						CtoPstring
//								void 							ForceTextToEnd
//								void 							GetActiveImageWindowTitle
//								void 							GetOutputWindowTitle
//								SInt32 						GetSpecifiedString
//								Boolean 						GetSpecifiedStringNumber
//								Boolean 						ListChannelsUsed
//								void 							ListCPUTimeInformation
//								Boolean 						ListHeaderInfo
//								Boolean						ListLineColumnIntervalString
//								Boolean 						ListProcessorTitleLine
//								Boolean 						ListProjectAndImageName
//								Boolean 						ListSpecifiedStringNumber
//								Boolean 						ListString
//								Boolean 						LoadSpecifiedStringNumberLong
//								Boolean 						LoadSpecifiedStringNumberLongP
//								Boolean 						LoadSpecifiedStringNumberString
//								Boolean 						LoadSpecifiedStringNumberStringP
//								Boolean 						MGetString
//								void 							MSetWindowTitle
//								Boolean						OutputString
//								void 							pstr
//								CharPtr 						PtoCstring
//								void 							RemoveCharsAddVersion
//								void 							RemoveCharsNoCase
//								void 							SetActiveImageWindowTitle
//								void 							SetOutputWTitle
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include	<ctype.h>  

#if defined multispec_lin
	#include	"SMultiSpec.h"
	
	#include "wx/string.h"
	#include "wx/textfile.h"
   #include "LGraphDoc.h"
   #include "LGraphFrame.h"
   #include "LGraphView.h"
	#include "LImageView.h"
	#include "LImageFrame.h"
	#include "LImageDoc.h"
	#include "LTextView.h"
	#include "wx/stdpaths.h"
#endif   // defined multispec_lin

#if defined multispec_mac || defined multispec_mac_swift
	#include	"MGraphView.h"
	
	#define	IDS_Processor20					20
	#define	IDS_Processor21					21
	#define	IDS_Processor22					22
	#define	IDS_Processor23					23
	#define	IDS_Processor24					24
	#define	IDS_Processor25					25
	#define	IDS_Processor26					26
#endif	// defined multispec_mac || defined multispec_mac_swift  
                            
#if defined multispec_win
	#include	"CDisplay.h"
	#include	"CImageWindow.h"
	#include "CProcessor.h"

	#include	"WGraphView.h"
	#include	"WImageView.h"
	#include "WImageDoc.h"
	#include "WStatisticsView.h"
	#include "WTextDoc.h"
	#include "WTextView.h"

	Boolean MGetString (
				TBYTE*								outTextPtr, 
				UInt16								stringListID, 
				UInt16								stringID,
				UInt16								maxStringLength=255);
	/*
	Boolean GetSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				TBYTE*								textStringPtr, 
				Boolean								continueFlag,
				UInt16								maxStringLength=255);
	*/
#endif	// defined multispec_win


extern Boolean	ListString (
				char*									textBuffer,
				unsigned int						textLength);


		// Prototypes for routines in this file that are only called by
		// other routines in this file.			

void		ConcatPStringsUnicode (
				FileStringPtr						ioFirstStr,
				const UCharPtr						inSecondStr,
				SInt16								inDestSize);						

Boolean 	LoadSpecifiedStringNumberLong (
				SInt16								strListID,
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				SInt32								lValue1,
				SInt32								lValue2);

Boolean 	LoadSpecifiedStringNumberString (
				SInt16								strListID,
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				char*									inputStringPtr);

Boolean 	LoadSpecifiedStringNumberString (
				SInt16								strListID,
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				FileStringPtr						inputStringPtr);

#if defined multispec_lin
	void		MSetWindowTitle (
					wxDocument*							documentCPtr,
					UCharPtr								titleStringPtr);
#endif
				
				

#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CheckStringLength
//
//	Software purpose:	The purpose of this routine is to check the length
//							of the input string relative to the width to be
//							drawn in.  If the string is too long, then remove
//							characters from the end until the string does fit
//							and add the ellipses "".
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DrawClassPrompt
//							DrawFieldPrompt
//
//	Coded By:			Larry L. Biehl			Date: 02/07/1989
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void CheckStringLength (
				SInt16								maxWidth, 
				Str255*								inputString, 
				Str255*								outputString)

{
			// Declare local variables and structures										
	
	SInt16								inputStringWidth,
											newCharLength,
											space = 2;

		
			// Get the width of the input string.											
	
	inputStringWidth = StringWidth (*inputString);
	
			// Adjust maximum width to allow for some space at the beginning and	
			// end of the string.																
			
	maxWidth -= space + space;
	
			// Copy input string to the output string.									
			
	if (inputStringWidth > 0)
		CopyPToP ((UCharPtr)outputString, (UCharPtr)inputString);
		
	else	// inputStringWidth <= 0
		(*outputString)[0] = 0;
	
			// If input string width is greater than the maximum width allowed, 	
			// then remove character from the end until it fits.						
			
	if (inputStringWidth > maxWidth)
		{
		newCharLength = (*outputString)[0];
		//maxWidth -= CharWidth ('\p…');
		//maxWidth -= CharWidth ('…');
		maxWidth -= CharWidth (0xC9);	
		
		do
			{
			inputStringWidth -= CharWidth ((*outputString)[newCharLength]);
			newCharLength--;
			
			}	while (inputStringWidth > maxWidth && newCharLength >= 0);
			
				// Add the ellipses character.												
		
		newCharLength++;	
		(*outputString)[newCharLength] = 0xC9;		// ellipsis character (...)
		(*outputString)[0] = newCharLength;
		
		}	// end "if (inputStringWidth > maxWidth)" 
	
}	// end "CheckStringLength"
#endif	// defined multispec_mac 
				

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckTextWindowSpaceNeeded
//
//	Software purpose:	The purpose of this routine is to check if there is
// 						enough memory to list the input number of characters 
//							in the text window. This has been set up for the Macintosh
//							OS. I have not determined an equivalent for Windows. For now
//							I will just use the same. The TextBlock structure under 
//							Windows is really just filler.
//
//	Parameters in:		Estimated number of characters to go into the text output
//								window.
//
//	Parameters out:	None
//
// Value Returned: 		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/03/1993
//	Revised By:			Larry L. Biehl			Date: 03/22/2019
// Linux: Just return True
Boolean CheckTextWindowSpaceNeeded (
				UInt32								numberBytesNeeded)

{
	#ifndef multispec_lin
		double								floatTextSizeNeeded;
	
		SInt64								contiguousMemory;
		
		UInt32								textSizeNeeded;
		
		SInt16								returnCode;
		
			
				// Check amount of contiguous memory available.  We will use			
				// contiguous memory here to be on the safe side; the memory does		
				// not have to be contiguous.														
				
		MGetFreeMemory (&contiguousMemory);
		
		textSizeNeeded = numberBytesNeeded;
		
				// Take into account Word Solution overhead for text buffers.  		
				// one text buffer for around every 700 characters.  The maximum		
				// buffer size is 1536.																
				
		numberBytesNeeded = textSizeNeeded/700 + 1;
		//textSizeNeeded += numberBytesNeeded * sizeof (TextBlock);
		
				// For now use the following for WASTE until I know more.
				
		floatTextSizeNeeded = textSizeNeeded;
		floatTextSizeNeeded += (double)numberBytesNeeded * 720;
		
		if (floatTextSizeNeeded > contiguousMemory - gTextMemoryMinimum)
			{
					// Display an alert to indicated that there is not enough space	
					// available for the classification output in the text window. 	
					 
			returnCode = DisplayAlert (kErrorAlert2ID, 
												kStopAlert,
												kAlertStrID, 
												IDS_Alert57, 
												0, 
												NULL);
			return (FALSE);
			
			}	// end "if (floatTextSizeNeeded > contiguousMemory)" 
			
		return (TRUE);
   #else // if it is linux
		return true;
	#endif

}	// end "CheckTextWindowSpaceNeeded" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CompareStringsNoCase
//
//	Software purpose:	The purpose of this routine is to compare strings str1 and str2
//							ignoring the case of the characters in the string.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	0:			the strings are the same.
//							not 0:	the strings are not the same.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 10/19/2017

SInt16 CompareStringsNoCase (
				const UCharPtr 					str1, 
				const UCharPtr						str2)
	
{
	UCharPtr								p1 = str1;
	UCharPtr								p2 = str2;
	
	UInt32								c1, 
											c2;
											
	
	c1 = toupper (*p1);
	c2 = toupper (*p2);
		
	while (c1 == c2)
		{
		if (c1 == 0)
																								return (0);
		
		p1++;
		p2++;
			
		c1 = toupper (*p1);
		c2 = toupper (*p2);
			
		}	// end "while (c1 == c2)"
	
	return ((SInt16)(c1-c2));
	
}	// end "CompareStringsNoCase"

   

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CompareStringsNoCase
//
//	Software purpose:	The purpose of this routine is to compare the first 
//							numberCharacters in strings str1 and str2
//							ignoring the case of the characters in the string.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	0:			the strings are the same.
//							not 0:	the strings are not the same.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 10/19/2017

SInt16 CompareStringsNoCase (
				const UCharPtr 					str1, 
				const UCharPtr 					str2,
				UInt32								numberCharacters)
	
{
	const	unsigned char* 			p1 = (unsigned char*)str1;
	const	unsigned char* 			p2 = (unsigned char*)str2;
	
	UInt32								c1, 
											c2,
											index;
											
											
	if (str1 == NULL || str2 == NULL)
																								return (0);
	
	index = 0;
	do
		{
		if (index >= numberCharacters)
																								return (0);
																				
		c1 = toupper (*p1);
		c2 = toupper (*p2);
		
		p1++;
		p2++;
		
		index++;
			
		}	while (c1 == c2);
	
	return ((SInt16)(c1-c2));
	
}	// end "CompareStringsNoCase"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CompareSuffixNoCase
//
//	Software purpose:	This routine compares the input suffix of length n
//							with the last n characters of the input string.
//							TRUE is returned if they compare and FALSE otherwise.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1988
//	Revised By:			Larry L. Biehl			Date: 03/09/2017

Boolean CompareSuffixNoCase (
				const CharPtr						suffixStringPtr, 
				StringPtr							stringPtr,
				UInt16*								numSuffixCharsPtr)

{
	StringPtr							tempPtr;
	
	SInt32								diff,
											index;
								
	UInt32								lengthSuffixString;
	
	Boolean								returnFlag;

	
	returnFlag = FALSE;
	lengthSuffixString = (UInt32)*suffixStringPtr;

	if (lengthSuffixString == 0) 
		{
				// This must be a C string.  Get the length.
				
		lengthSuffixString = (UInt32)strlen ((char*)&suffixStringPtr[1]);
		
		lengthSuffixString = MIN (lengthSuffixString, 255);
		
		}	// end "if (lengthSuffixString == 0)"
	
	if (numSuffixCharsPtr != NULL)	
		*numSuffixCharsPtr = (UInt16)lengthSuffixString;
	
	index = (SInt32)*stringPtr - lengthSuffixString + 1;
	if (index > 1)
		{
		stringPtr = &stringPtr[index];
		tempPtr = (StringPtr)&suffixStringPtr[1];

		if (tempPtr == stringPtr)
																							return (TRUE);

		while (lengthSuffixString > 0)
			{
			if (*tempPtr != '?')
				{
				diff = toupper (*tempPtr) - toupper (*stringPtr);
				if (diff != 0)                                                    
																							return (FALSE);
																				
				}	// end "if (*tempPtr != '?')"

			tempPtr++; 
			stringPtr++; 
			lengthSuffixString--;

			}	// end "while (lengthSuffixString > 0)"

																							return (TRUE);
			
		}	// end "if (index > 1)" 

	return (returnFlag);

}	// end "CompareSuffixNoCase"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CompareSuffixNoCase
//
//	Software purpose:	This routine compares the input suffix of length n
//							with the last n characters of the input string.
//							TRUE is returned if they compare and FALSE otherwise.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1988
//	Revised By:			Larry L. Biehl			Date: 03/14/2017

Boolean CompareSuffixNoCase (
				const CharPtr						suffixStringPtr, 
				wchar_t*								stringPtr,
				UInt16*								numSuffixCharsPtr)

{
	char*									tempPtr;
	
	SInt32								diff,
											index;
								
	UInt32								lengthSuffixString;
	
	Boolean								returnFlag;

	
	returnFlag = FALSE;
	lengthSuffixString = (UInt32)*suffixStringPtr;

	if (lengthSuffixString == 0) 
		{
				// This must be a C string.  Get the length.
				
		lengthSuffixString = (UInt32)strlen ((char*)&suffixStringPtr[1]);
		lengthSuffixString = MIN (lengthSuffixString, 255);
		
		}	// end "if (lengthSuffixString == 0)"
	
	if (numSuffixCharsPtr != NULL)	
		*numSuffixCharsPtr = (UInt16)lengthSuffixString;
	
	index = (SInt32)stringPtr[0] - lengthSuffixString + 1;
	if (index > 1)
		{
		stringPtr = &stringPtr[index];
		tempPtr = &suffixStringPtr[1];

		if ((wchar_t*)tempPtr == stringPtr)
																							return (TRUE);

		while (lengthSuffixString > 0)
			{
			if (*tempPtr != L'?')
				{
				wchar_t  wideTempChar;
				mbstowcs (&wideTempChar, (const char*)tempPtr, 1);
				diff = towupper (wideTempChar) - towupper (*stringPtr);

				if (diff != 0)                                                    
																							return (FALSE);
																				
				}	// end "if (*tempPtr != '?')"

			tempPtr++; 
			stringPtr++; 
			lengthSuffixString--;

			}	// end "while (lengthSuffixString > 0)"

																							return (TRUE);
			
		}	// end "if (index > 1)" 

	return (returnFlag);

}	// end "CompareSuffixNoCase"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConcatFilenameSuffix
//
//	Software purpose:	The purpose of this routine is to append the input suffix to
//							the input file name. First the length of the file name will
//							be shorten if needed to allow the suffix.
//
//	Parameters in:		Address of input file name
//							Address of input suffix to be added
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/21/2001
//	Revised By:			Larry L. Biehl			Date: 03/13/2017

void ConcatFilenameSuffix (
				wchar_t*								fileNamePtr,
				const StringPtr					suffixPtr,
				SInt16								charFormatCode)

{
			// Limit combined string to inDestSize chars

   UInt16	charsToCopy = suffixPtr[0];

			// If length of the second string is 0, then assume that
			// it is a C string and get the length.

	if (charsToCopy == 0)
		charsToCopy = (UInt16)strlen ((CharPtr)&suffixPtr[1]);
	
	if (gFileNameLengthLimit > charsToCopy)
		{		
				// Make sure that the name with the suffix fits within the max file 
				// name length.
				
		if (charFormatCode == kAsciiFormatCode)
			{
			UCharPtr charFileNamePtr = (UCharPtr)fileNamePtr;
			charFileNamePtr[0] = MIN (charFileNamePtr[0], gFileNameLengthLimit-charsToCopy);
			ConcatPStrings (charFileNamePtr, suffixPtr, 255);
			
			}	// end "if (charFormatCode == kAsciiFormatCode)"
			
		else	// charFormatCode != kAsciiFormatCode
			{
			fileNamePtr[0] = MIN (fileNamePtr[0], gFileNameLengthLimit-charsToCopy);
		
					// Convert char suffix to unicode suffix
 
			TBYTE		wideSuffix[16];
			mbstowcs (&wideSuffix[1], (const char*)&suffixPtr[1], charsToCopy);
			wideSuffix[0] = charsToCopy;
			//ConcatPStrings (fileNamePtr, wideSuffix, 255);

			}	// end "else charFormatCode != kAsciiFormatCode"
		
		}	// end "if (gFileNameLengthLimit > charsToCopy)"
		
			// Force a c terminator as the last character of the input string.
			
	fileNamePtr[255] = 0;
	
}	// end "ConcatFilenameSuffix" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConcatFilenameSuffix
//
//	Software purpose:	The purpose of this routine is to append the input suffix to
//							the input file name. First the length of the file name will
//							be shorten if needed to allow the suffix.
//
//	Parameters in:		Address of input file name
//							Address of input suffix to be added
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/10/2017
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void ConcatFilenameSuffix (
				UInt8*								fileNamePtr,
				UInt8*								charSuffixPtr)

{
			// Limit combined string to inDestSize chars

   UInt16	charsToCopy = charSuffixPtr[0];

			// If length of the second string is 0, then assume that
			// it is a C string and get the length.

	if (charsToCopy == 0)
		charsToCopy = (UInt16)strlen ((CharPtr)&charSuffixPtr[1]);
	
	if (gFileNameLengthLimit > charsToCopy)
		{		
		fileNamePtr[0] = MIN (fileNamePtr[0], gFileNameLengthLimit-charsToCopy);
		ConcatPStrings ((UCharPtr)fileNamePtr, (UCharPtr)charSuffixPtr, 255);
		
		}	// end "if (gFileNameLengthLimit > charsToCopy)"
		
			// Force a c terminator as the last character of the input string.
			
	fileNamePtr[255] = 0;
	
}	// end "ConcatFilenameSuffix" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConcatPStrings
//
//	Software purpose:	The purpose of this routine is to append a pascal
//							string to the end of another pascal string.
//
//	Parameters in:		Address of base string
//							Address of string to be copied to end of the base string.
//							Maximum number of characters allowed for the appended string.
//								This does not include the pascal character count.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/19/1995
//	Revised By:			Larry L. Biehl			Date: 12/02/2011

void ConcatPStrings (
				UCharPtr								ioFirstStr,
				const UCharPtr						inSecondStr,
				SInt16								inDestSize)

{
	if (ioFirstStr != NULL && inSecondStr != NULL)
		{
				// Limit combined string to inDestSize chars

		SInt16	charsToCopy = (SInt16)inSecondStr[0];

				// If length of the second string is 0, then assume that
				// it is a C string and get the length.

		if (charsToCopy == 0)
			charsToCopy = (SInt16)strlen ((CharPtr)&inSecondStr[1]);

		if ((SInt16)*ioFirstStr + charsToCopy > inDestSize)
			charsToCopy = inDestSize - *ioFirstStr;

		if (charsToCopy < 0)
			charsToCopy = 0;

				// Copy second to end of first string

		memcpy (ioFirstStr + *ioFirstStr + 1, 
					inSecondStr + 1, 
					charsToCopy);

				// Set length of combined string

		ioFirstStr[0] += (UInt8)charsToCopy;

				// Also make the string a C string if there is enough space.

		if (ioFirstStr[0]+1 <= inDestSize) 
			ioFirstStr[ioFirstStr[0]+1] = 0;
			
		}	// end "if (ioFirstStr != NULL && inSecondStr != NULL)"
	
}	// end "ConcatPStrings"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConvertMultibyteStringToUnicodeString
//
//	Software purpose:	The purpose of this routine is to convert the input multibyte
//							string to a unicode string. This will use the global gWideTextString
//							as the location for the output string. Note that the output string
//							cannot be larger than 254 characters plus pascal character count and
//							c terminator.
//
//	Parameters in:		Address of input multibyte string set up as pascal string.
//
//	Parameters out:	None
//
//	Value Returned:	Address of output unicode string
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/21/2017
//	Revised By:			Larry L. Biehl			Date: 06/19/2017

wchar_t* ConvertMultibyteStringToUnicodeString (
				UCharPtr								inputMultibyteStringPtr)

{

	#if defined multispec_lin
		wxWCharBuffer						unicodeString;
		size_t								numberUnicodeChars;
		

		unicodeString = wxConvUTF8.cMB2WC ((char*)inputMultibyteStringPtr, 
														wxNO_LEN, 
														&numberUnicodeChars);
																			
		wcsncpy (&gWideTextString[1], unicodeString, numberUnicodeChars);
		gWideTextString[0] = numberUnicodeChars;
	#endif // end "multispec_lin"

	#if defined multispec_win
		if (inputMultibyteStringPtr != NULL)
			{
			int sizeNeeded = MultiByteToWideChar (
									CP_UTF8, 0, (LPCSTR)inputMultibyteStringPtr, -1, NULL, 0);

			sizeNeeded = MIN (sizeNeeded, 254);
			MultiByteToWideChar (CP_UTF8, 
										0,
										(LPCSTR)inputMultibyteStringPtr, 
										-1, 
										(LPWSTR)&gWideTextString[1], 
										sizeNeeded);

			gWideTextString[0] = (wchar_t)wcslen (&gWideTextString[1]);

			}	// end "if (inputMultibyteStringPtr != NULL)"

		else	// inputMultibyteStringPtr == NULL
			{
			gWideTextString[0] = 0;
			gWideTextString[1] = 0;
			
			}	// else inputMultibyteStringPtr == NULL

		return (gWideTextString);
	#endif

	return (NULL);
	
}	// end "ConvertMultibyteStringToUnicodeString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConvertUnicodeStringToMultibyteString
//
//	Software purpose:	The purpose of this routine is to convert the input unicode
//							string to a UTF8 string. 
//
//	Parameters in:		Address of input unicode string set up as pascal string.
//							Address of output utf8 formatted string set up as a C string.
//
//	Parameters out:	Number of characters in the output string
//
//	Value Returned:	Address of output unicode string
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/22/2017
//	Revised By:			Larry L. Biehl			Date: 07/27/2018

void ConvertUnicodeStringToMultibyteString (
				wchar_t*								inputUnicodeStringPtr,
				UCharPtr								outputUTF8StringPtr,
				UInt16								numberCharactersToCopy,
				SInt16*								outputStringLengthPtr)

{
	#if defined multispec_mac
		UniChar*								uniCharInputStringPtr;
		CFStringRef							cfStringRef;
		UInt16								numberChars;
		
											
		outputUTF8StringPtr[0] = 0;
		outputUTF8StringPtr[1] = 0;
		
		if (numberCharactersToCopy == 0)
			numberChars = inputUnicodeStringPtr[0];
		else
			numberChars = MIN (inputUnicodeStringPtr[0], numberCharactersToCopy);
		
		uniCharInputStringPtr = (UniChar*)inputUnicodeStringPtr;
		cfStringRef = CFStringCreateWithCharacters (
											kCFAllocatorDefault,
											(UniChar*)&uniCharInputStringPtr[1],
											numberChars);
											
		if (cfStringRef != NULL)
			{
			CFStringGetCString (cfStringRef,
										(char*)outputUTF8StringPtr,
										(CFIndex)255,
										kCFStringEncodingUTF8);
			*outputStringLengthPtr = strlen ((char*)outputUTF8StringPtr);
			
			}	// end "if (cfStringRef != NULL)"
	#endif

	#if defined multispec_lin
		wxCharBuffer						multiByteString;
		UInt16								numberChars;
		size_t								numberMultiByteChars;
		
											
		outputUTF8StringPtr[0] = 0;
		outputUTF8StringPtr[1] = 0;
		
		if (numberCharactersToCopy == 0)
			numberChars = inputUnicodeStringPtr[0];
		else
			numberChars = MIN (inputUnicodeStringPtr[0], numberCharactersToCopy);

		multiByteString = wxConvUTF8.cWC2MB (&inputUnicodeStringPtr[1], 
															numberChars, 
															&numberMultiByteChars);
																			
		strncpy ((char*)outputUTF8StringPtr, multiByteString, numberMultiByteChars);
		*outputStringLengthPtr = numberMultiByteChars;
	#endif // end "multispec_lin"

	#if defined multispec_win
		UInt16								numberChars;
		
											
		outputUTF8StringPtr[0] = 0;
		outputUTF8StringPtr[1] = 0;
		
		if (numberCharactersToCopy == 0)
			numberChars = inputUnicodeStringPtr[0];
		else
			numberChars = MIN (inputUnicodeStringPtr[0], numberCharactersToCopy);

		//int size = wcslen (titleBufferPtr);
		int sizeNeeded = WideCharToMultiByte (
				CP_UTF8, 0, &inputUnicodeStringPtr[1], numberChars, NULL, 0, NULL, NULL);
		WideCharToMultiByte (CP_UTF8, 
									0,
									&inputUnicodeStringPtr[1], 
									numberChars, 
									(LPSTR)outputUTF8StringPtr,
									sizeNeeded, 
									NULL, 
									NULL);
		*outputStringLengthPtr = sizeNeeded;
	#endif	// end "multispec_win"

			// Make sure this can also be treated as a c string.

	if (*outputStringLengthPtr < 256)
		outputUTF8StringPtr[*outputStringLengthPtr] = 0;
	
}	// end "ConvertUnicodeStringToMultibyteString"

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConcatPStrings
//
//	Software purpose:	The purpose of this routine is to append a pascal
//							string to the end of another pascal string.
//
//	Parameters in:		Address of base string
//							Address of string to be copied to end of the base string.
//							Maximum number of characters allowed for the appended string.
//								This does not include the pascal character count.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2017
//	Revised By:			Larry L. Biehl			Date: 03/14/2017

void ConcatPStrings (
				wchar_t*								ioFirstStr,
				const wchar_t*						inSecondStr,
				SInt16								inDestSize)

{
	if (ioFirstStr != NULL && inSecondStr != NULL)
		{
				// Limit combined string to inDestSize chars

		SInt16	charsToCopy = (SInt16)inSecondStr[0];

				// If length of the second string is 0, then assume that it is a C 
				// string and get the length.

		if (charsToCopy == 0)
			charsToCopy = wcslen (&inSecondStr[1]);

		if ((SInt16)*ioFirstStr + charsToCopy > inDestSize)
			charsToCopy = inDestSize - *ioFirstStr;

		if (charsToCopy < 0)
			charsToCopy = 0;

				// Copy second to end of first string

		wmemcpy (&ioFirstStr[*ioFirstStr+1],
					&inSecondStr[1],
					charsToCopy);

				// Set length of combined string

		ioFirstStr[0] += (UInt8)charsToCopy;

				// Also make the string a C string if there is enough space.

		if (ioFirstStr[0]+1 <= inDestSize) 
			ioFirstStr[ioFirstStr[0]+1] = 0;
			
		}	// end "if (ioFirstStr != NULL && inSecondStr != NULL)"
	
}	// end "ConcatPStrings"
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConcatPStringsUnicode
//
//	Software purpose:	The purpose of this routine is to append a pascal
//							string to the end of another pascal string.
//
//	Parameters in:		Address of base string
//							Address of string to be copied to end of the base string.
//							Maximum number of characters allowed for the appended string.
//								This does not include the pascal character count.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/22/2017
//	Revised By:			Larry L. Biehl			Date: 02/22/2017

void ConcatPStringsUnicode (
				UCharPtr								ioFirstStr,
				const UCharPtr						inSecondStr,
				SInt16								inDestSize)

{
	if (ioFirstStr != NULL && inSecondStr != NULL)
		{
				// Limit combined string to inDestSize chars

		SInt16	charsToCopy = (SInt16)inSecondStr[0];

				// If length of the second string is 0, then assume that
				// it is a C string and get the length.

		if (charsToCopy == 0)
			charsToCopy = strlen ((CharPtr)&inSecondStr[1]);

		if ((SInt16)*ioFirstStr + charsToCopy > inDestSize)
			charsToCopy = inDestSize - *ioFirstStr;

		if (charsToCopy < 0)
			charsToCopy = 0;

				// Copy second to end of first string

		memcpy (ioFirstStr + *ioFirstStr + 1, 
					inSecondStr + 1, 
					charsToCopy);

				// Set length of combined string

		ioFirstStr[0] += (UInt8)charsToCopy;

				// Also make the string a C string if there is enough space.

		if (ioFirstStr[0]+1 <= inDestSize) 
			ioFirstStr[ioFirstStr[0]+1] = 0;
			
		}	// end "if (ioFirstStr != NULL && inSecondStr != NULL)"
	
}	// end "ConcatPStringsUnicode" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CopyPToP
//
//	Software purpose:	The purpose of this routine is to copy a pascal
//							string from one address to another address.
//
//	Parameters in:		Address of base string (p1)
//							Address of string to copy to p1 (p2)
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:  ?/ ?/????
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void CopyPToP (
				UCharPtr								p1, 
				UCharPtr								p2)				

{
			// Force the output string to be empty
			
	if (p1 != NULL)
		p1[0] = 0;
	
	ConcatPStrings ((StringPtr)p1, (StringPtr)p2, 255);
		
}	// end "CopyPToP"  


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CopyPToP
//
//	Software purpose:	The purpose of this routine is to copy a pascal
//							wide string from one address to another address.
//							Note that after going to UTF8 strings, this routine is not
//							needed any more.
//
//	Parameters in:		Address of base string (p1)
//							Address of string to copy to p1 (p2)
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2017
//	Revised By:			Larry L. Biehl			Date: 02/23/2017

void CopyPToP (
				wchar_t*								p1,
				wchar_t*								p2)

{
			// Force the output string to be empty
			
	if (p1 != NULL)
		p1[0] = 0;
	
	ConcatPStrings ((wchar_t*)p1, (wchar_t*)p2, 255);
		
}	// end "CopyPToP"  
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CreateNumberWithCommasInString
//
//	Software purpose:	This routine converts a decimal value to a string with commas.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/01/2003
//	Revised By:			Larry L. Biehl			Date: 05/01/2003	
							               	   
SInt16 CreateNumberWithCommasInString (
				char*									inputStringPtr, 
				SInt32								dataValue)
				
{
	SInt16								numberChars;
	
		
	numberChars = sprintf (inputStringPtr, "%ld", dataValue);
			
	numberChars = InsertCommasInNumberString (inputStringPtr, 
															numberChars,
															-1,
															numberChars);
																
	return (numberChars);
	
}	// end "CreateNumberWithCommasInString"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CreateNumberWithCommasInString
//
//	Software purpose:	This routine converts a decimal value to a string with commas.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/23/2010
//	Revised By:			Larry L. Biehl			Date: 08/26/2010	

SInt16 CreateNumberWithCommasInString (
				char*									inputStringPtr, 
				SInt64								dataValue)

{
	SInt16								numberChars;
	
	
	numberChars = sprintf (inputStringPtr, "%lld", dataValue);
	
	numberChars = InsertCommasInNumberString (inputStringPtr, 
															 numberChars,
															 -1,
															 numberChars);
	
	return (numberChars);
	
}	// end "CreateNumberWithCommasInString"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CharPtr CtoPstring
//
//	Software purpose:	This routine copies the input C string to an output
//							pascal string. The input and output string pointers
//							can represent the same string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/24/1995
//	Revised By:			Larry L. Biehl			Date: 03/09/2019
							               	   
UCharPtr CtoPstring (
				UCharPtr								inCStringPtr, 
				UCharPtr								outPStringPtr,
				int									maxStringLength)
				
{
	if (inCStringPtr != NULL && outPStringPtr != NULL)
		{
		size_t bytesToMove = strlen ((CharPtr)inCStringPtr);
		
		bytesToMove = MIN (bytesToMove, maxStringLength);

		if (bytesToMove > 0)
			memmove (&outPStringPtr[1], inCStringPtr, bytesToMove);
			
		outPStringPtr[0] = (UInt8)bytesToMove;
		
				// Also make it a c string.
				
		outPStringPtr[bytesToMove+1] = 0;

		}	// end "if (inCStringPtr != NULL && ...)"

	return outPStringPtr;

}	// end "CtoPstring"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		FileStringPtr CtoPstring
//
//	Software purpose:	This routine copies the input C string to an output
//							pascal string. The input and output string pointers
//							can represent the same string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2017
//	Revised By:			Larry L. Biehl			Date: 03/09/2019
							               	   
wchar_t* CtoPstring (
				wchar_t*								inCStringPtr,
				wchar_t*								outPStringPtr,
				int									maxStringLength)
				
{
	if (inCStringPtr != NULL && outPStringPtr != NULL)
		{
		size_t bytesToMove = wcslen (inCStringPtr);
		
		bytesToMove = MIN (bytesToMove, maxStringLength);

		if (bytesToMove > 0)
			wmemmove (&outPStringPtr[1], inCStringPtr, bytesToMove);
			
		outPStringPtr[0] = (UInt8)bytesToMove;
		
				// Also make it a c string.
				
		outPStringPtr[bytesToMove+1] = 0;

		}	// end "if (inCStringPtr != NULL && ...)

	return outPStringPtr;

}	// end "CtoPstring"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceTextToEnd
//
//	Software purpose:	The purpose of this routine is to force the text
//							selection location to be at the end of the current
//							text in the output text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
//	Called By:			ClassifyControl in classify.c
//
//	Coded By:			Larry L. Biehl			Date: 12/14/1990
//	Revised By:			Larry L. Biehl			Date: 03/27/2017	

void ForceTextToEnd (void)

{                         
	#if defined multispec_mac         
		GrafPtr								savedPort;
		
		
		if (gOutputWindow != NULL)
			{
					// Save the current port.															
					
			GetPort (&savedPort);
			SetPortWindowPort (gOutputWindow);
					
					// Force text selection to start from end of present text
				
			#if use_mlte_for_text_window
				TXNSetSelection (gOutputTextH, kTXNEndOffset, kTXNEndOffset);
			#endif		// !use_mlte_for_text_window
			
			#if !use_mlte_for_text_window
				WESetSelection (LONG_MAX, LONG_MAX, gOutputTextH);
			#endif		// !use_mlte_for_text_window
			
			SetPort (savedPort);
			
			}	// end "if (gOutputWindow != NULL)"
	#endif	// defined multispec_mac 

	#if defined multispec_win   
		if (gOutputViewCPtr != NULL)
			gOutputViewCPtr->GetEditCtrl ().SetSel (2147483646, 2147483646, TRUE);
	#endif	// defined multispec_win 
	 
	#if defined multispec_lin
		 //      TODO: For linux when developing gui
	#endif
}	// end "ForceTextToEnd"   	  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetActiveImageWindowTitle
//
//	Software purpose:	The purpose of this routine is to get the
//							title for the active image window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1995
//	Revised By:			Larry L. Biehl			Date: 03/20/2018 
 
void GetActiveImageWindowTitle (
				UCharPtr								titleStringPtr)

{  
	titleStringPtr[0] = 0;
	         
	#if defined multispec_mac  	   
	   if (gActiveImageWindow != NULL)
	   	{                                                
			GetWTitle (gActiveImageWindow, titleStringPtr);
			titleStringPtr[titleStringPtr[0]+1] = 0;
			
			}	// end "if (gActiveImageWindow != NULL)"		
	#endif	// defined multispec_mac 
		
	#if defined multispec_win	   
	   if (gActiveImageViewCPtr != NULL)
	   	{  
			CMImageDoc* documentCPtr = gActiveImageViewCPtr->GetDocument ();
			CString titleString = documentCPtr->GetTitle ();
			     
			UInt16 titleLength = titleString.GetLength ();
			LPTSTR titleBufferPtr = titleString.GetBuffer (titleLength);

			int size = (int)wcslen (titleBufferPtr);
			int sizeNeeded = WideCharToMultiByte (
									CP_UTF8, 0, titleBufferPtr, size, NULL, 0, NULL, NULL);
			WideCharToMultiByte (CP_UTF8,
										0,
										titleBufferPtr,
										size,
										(LPSTR)&titleStringPtr[1],
										sizeNeeded,
										NULL,
										NULL);

			//memcpy (&titleStringPtr[1], 
			//				titleBufferPtr,
			//				titleLength);
							
			titleString.ReleaseBuffer ();
				
			//titleStringPtr[0] = (UInt8)strlen ((char*)&titleStringPtr[1]);
			titleStringPtr[0] = (UInt8)sizeNeeded;
			titleStringPtr[sizeNeeded+1] = 0; 
			
			}	// end "if (gActiveImageViewCPtr != NULL)"
	#endif	// defined multispec_win 
	
	#if defined multispec_lin
      wxString titleString = (gActiveImageViewCPtr->m_frame)->GetTitle ();
      UInt16 titleLength = MIN ((UInt16)titleString.Length (), 254);
			
		strncpy ((char*)&titleStringPtr[1],
					(const char*)titleString.mb_str (wxConvUTF8),
					254);
							
		titleStringPtr[0] = (UInt8)titleLength;
		titleStringPtr[titleLength+1] = 0; 
	#endif

}	// end "GetActiveImageWindowTitle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetImageWindowName
//
//	Software purpose:	The purpose of this routine is to get the text for a window
//							name or a tiff file name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
//	Called By:			SetImageWTitle in SDisMulc.cpp
//							FileSpecificationDialogOK in SOpenDlg.cpp
//							SaveImageWindowAs in SSaveWrt.cpp
//							CreateImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 10/23/2000
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

void GetImageWindowName (
				DisplaySpecsPtr					displaySpecsPtr, 
				FileInfoPtr							fileInfoPtr,
				UCharPtr								namePtr,
				Boolean								removeSuffixFlag)

{	
	SInt16								length,
											savedLength,
											totalLength;
	
		
	if (namePtr == NULL)
																											return;
	if (fileInfoPtr == NULL) 
		{
		namePtr[1] = 0;
																											return;
		}
																											
			// Initialize local variables.																
			
	totalLength = 1;
	
			// First check if this window represents several file layers.			
			
	if (gImageWindowInfoPtr != NULL && gImageWindowInfoPtr->numberImageFiles > 1)
		{
		sprintf ((char*)&namePtr[0], "L%hd-", gImageWindowInfoPtr->numberImageFiles);
		CtoPstring (namePtr, namePtr);
		totalLength += namePtr[0];
		
		}	// end "if (gImageWindowInfoPtr != NULL && gImageWindowInfoPtr->..."
	
	length = 0;		
	if (fileInfoPtr->hdfHandle != NULL)
		{
//		GetHdfDataSetName (fileInfoPtr, 
//									fileInfoPtr->hdfDataSetSelection,
//									(StringPtr)gTextString2,
//									NULL);
									
		CreateFullDataSetIdentifierName (fileInfoPtr, 
														fileInfoPtr->hdfDataSetSelection,
														(StringPtr)gTextString2,
														60);
		
		length = gTextString2[0];
		sprintf ((char*)&namePtr[totalLength],
						"%s",
						&gTextString2[1]);
		
		}	// end "if (fileInfoPtr->format == kHDF4Type || ..."
		
	if (length == 0)
		{
		char* fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (fileInfoPtr);
		length = (SInt16)strlen (fileNamePtr);
		sprintf ((char*)&namePtr[totalLength],
						"%s",
						fileNamePtr);
						
		}	// end "if (length == 0)"
					
	totalLength += length;
	
	if (removeSuffixFlag)
		{
		namePtr[0] = totalLength - 1;
		//RemoveSuffix ((FileStringPtr)namePtr, kASCIICharString);
		RemoveSuffix ((FileStringPtr)namePtr);
		totalLength = namePtr[0] + 1;
		
		savedLength = totalLength - 1;
		
		}	// end "if (removeSuffixFlag)"
		
	else	// !removeSuffixFlag
		{
		sprintf ((char*)&namePtr[totalLength], " ");
		totalLength++;
		
		}	// end "else !removeSuffixFlag"
		
	if (displaySpecsPtr != NULL && !fileInfoPtr->thematicType)
		{		
		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType || 
							displaySpecsPtr->displayType == k1_ChannelGrayLevelDisplayType)
			{
			#undef	MONTH_DAY_DESCRIPTION
			#ifdef MONTH_DAY_DESCRIPTION
				Str15				monthString[12] = {
															"Jan",
															"Feb",
															"Mar",
															"Apr",
															"May",
															"Jun",
															"Jul",
															"Aug",
															"Sep",
															"Oct",
															"Nov",
															"Dec" };
				SInt16			day,
									leapYearOffset = 1,
									monthIndex;
				
						// For GDD image sequence create string for date (month-day)
				if (displaySpecsPtr->channelNumber <= 31)
					{
					monthIndex = 0;
					day = displaySpecsPtr->channelNumber;
					}
				else if (displaySpecsPtr->channelNumber <= 59+leapYearOffset)
					{
					monthIndex = 1;
					day = displaySpecsPtr->channelNumber - 31;
					}
				else if (displaySpecsPtr->channelNumber <= 90+leapYearOffset)
					{
					monthIndex = 2;
					day = displaySpecsPtr->channelNumber - (59+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 120+leapYearOffset)
					{
					monthIndex = 3;
					day = displaySpecsPtr->channelNumber - (90+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 151+leapYearOffset)
					{
					monthIndex = 4;
					day = displaySpecsPtr->channelNumber - (120+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 181+leapYearOffset)
					{
					monthIndex = 5;
					day = displaySpecsPtr->channelNumber - (151+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 212+leapYearOffset)
					{
					monthIndex = 6;
					day = displaySpecsPtr->channelNumber - (181+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 243+leapYearOffset)
					{
					monthIndex = 7;
					day = displaySpecsPtr->channelNumber - (212+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 273+leapYearOffset)
					{
					monthIndex = 8;
					day = displaySpecsPtr->channelNumber - (243+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 304+leapYearOffset)
					{
					monthIndex = 9;
					day = displaySpecsPtr->channelNumber - (273+leapYearOffset);
					}
				else if (displaySpecsPtr->channelNumber <= 334+leapYearOffset)
					{
					monthIndex = 10;
					day = displaySpecsPtr->channelNumber - (304+leapYearOffset);
					}
				else	// displaySpecsPtr->channelNumber <= 365)
					{
					monthIndex = 11;
					day = displaySpecsPtr->channelNumber - (334+leapYearOffset);
					}
					
				sprintf (&namePtr[totalLength], 
							"(%s %hd)",
							monthString[monthIndex],
							day);
			#endif	// MONTH_DAY_DESCRIPTION
				
			#ifndef MONTH_DAY_DESCRIPTION
				if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType)
					sprintf ((char*)&namePtr[totalLength], 
								"(ch_%hdT)",
								//"(day %hd)",
								displaySpecsPtr->channelNumber);
				else
					sprintf ((char*)&namePtr[totalLength], 
								"(ch_%hd)",
								//"(day %hd)",
								displaySpecsPtr->channelNumber);
			#endif	// !MONTH_DAY_DESCRIPTION
			
			}	// end "if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType || ..." 
			
		else if (displaySpecsPtr->displayType == k3_ChannelDisplayType || 
											displaySpecsPtr->displayType == k3_2_ChannelDisplayType)
			{
			sprintf ((char*)&namePtr[totalLength], 
						"(chs_%hd,%hd,%hd)",
						displaySpecsPtr->redChannelNumber,
						displaySpecsPtr->greenChannelNumber,
						displaySpecsPtr->blueChannelNumber);
			
			}	// end "else if (displaySpecsPtr->displayType == k3_ChannelDisplayType || ..." 
			
		else if (displaySpecsPtr->displayType == k2_ChannelDisplayType)
			{
			SInt16		channel1 = 1,
							channel2 = 1;
						
			if (displaySpecsPtr->rgbColors == kRGColor)
				{
				channel1 = displaySpecsPtr->redChannelNumber;
				channel2 = displaySpecsPtr->greenChannelNumber;
				
				}	// end "if (sDisplaySpecsPtr->rgbColors == kRGColor)" 
				
			else if (displaySpecsPtr->rgbColors == kRBColor)
				{
				channel1 = displaySpecsPtr->redChannelNumber;
				channel2 = displaySpecsPtr->blueChannelNumber;
					
				}	// end "if (displaySpecsPtr->rgbColors == kRBColor)" 
				
			else if (displaySpecsPtr->rgbColors == kGBColor)
				{
				channel1 = displaySpecsPtr->greenChannelNumber;
				channel2 = displaySpecsPtr->blueChannelNumber;
				
				}	// end "if (displaySpecsPtr->rgbColors == kGBColor)"
				 
			sprintf ((char*)&namePtr[totalLength], 
						"(chs_%hd,%hd)",
						channel1,
						channel2); 
				
			}	// end "else if (displaySpecsPtr->displayType == k2_ChannelDisplayType)" 
			
		else if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
			{
			sprintf ((char*)&namePtr[totalLength], 
						"(multichannels)");
			
			}	// end "else if (displaySpecsPtr->displayType == 7)"
			
		}	// end "if (displaySpecsPtr != NULL)"
		
			// If the name is going to be the default for a tiff image file, then
			// verify that the name will not be two long. If so remove the "(...)"
			// info.
			
	totalLength = (SInt16)strlen ((char*)&namePtr[1]);
	
	if (removeSuffixFlag)
		{
		if (totalLength > gFileNameLengthLimit-4)
			totalLength = savedLength;
		
		}	// end "if (removeSuffixFlag)"
	
			// Make string a pascal string.
			
	namePtr[0] = (char)totalLength;
	
}	// end "GetImageWindowName"    	  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetImageWindowTitle
//
//	Software purpose:	The purpose of this routine is to get the
//							title for the input image window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 08/13/2013 
 
void GetImageWindowTitle (
				Handle								windowInfoHandle,
				UCharPtr								titleStringPtr)

{  
	WindowPtr							windowPtr;
	
	
	titleStringPtr[0] = 0;
	windowPtr = GetWindowPtr (windowInfoHandle);
	
	GetWindowTitle (windowPtr, titleStringPtr);
/*	         
	#if defined multispec_mac  
	   
	   if (windowPtr != NULL)
	   	{                                                
			GetWTitle (windowPtr, titleStringPtr);
			titleStringPtr[titleStringPtr[0]+1] = 0;
			
			}	// end "if (windowPtr != NULL)"
		
	#endif	// defined multispec_mac 
	
	
	#if defined multispec_win
	   
	   if (windowPtr != NULL)
	   	{  
			CMImageDoc* documentCPtr = windowPtr->GetDocument ();
			CString titleString = documentCPtr->GetTitle ();
			     
			UInt16 titleLength = titleString.GetLength ();
			char* titleBufferPtr = titleString.GetBuffer (titleLength);

			memcpy (	&titleStringPtr[1], 
						titleBufferPtr, 
						titleLength);
							
			titleString.ReleaseBuffer ();
				
			titleStringPtr[0] = (UInt8)titleLength;
			titleStringPtr[titleLength+1] = 0; 
			
			}	// end "if (windowPtr != NULL)"
		
	#endif	// defined multispec_win 
*/		
}	// end "GetImageWindowTitle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt16 GetNumberLeadingDecimalZeros
//
//	Software purpose:	The purpose of this routine is to get the number of decimal
//							digits such that the user can see unique values when printed.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/16/2005
//	Revised By:			Larry L. Biehl			Date: 12/30/2005 
 
UInt16 GetNumberLeadingDecimalZeros (
				double								dataValue)

{      
	UInt16								numberDigitsAfter;   
	   

	numberDigitsAfter = 0;
	
	dataValue = fabs (dataValue);
	if (dataValue > 0)
		{
		while (dataValue < 1)
			{
			numberDigitsAfter++;
			dataValue *= 10;
				
			}	// end "while (dataValue < 1)" 
			
		}	// end "if (dataValue > 0)"
			
	return (numberDigitsAfter);
		
}	// end "GetNumberLeadingDecimalZeros" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt16 GetNumberWholeDigits
//
//	Software purpose:	The purpose of this routine is to get the number of decimal
//							digits such that the user can see unique values when printed.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/09/2006
//	Revised By:			Larry L. Biehl			Date: 09/26/2017 
 
UInt16 GetNumberWholeDigits (
				double								dataValue)

{  
	SInt16								exponent;
	UInt16								numberWholeDigits;
	
	    
	numberWholeDigits = 1;
	
	if (dataValue != 0)
		{
		exponent = (SInt16)log10 (fabs (dataValue));
		numberWholeDigits = MAX (0, exponent) + 1;
		
		}	// end "if (dataValue != 0)"
		
	return (numberWholeDigits);
		
}	// end "GetNumberWholeDigits" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetOutputWindowTitle
//
//	Software purpose:	The purpose of this routine is to get the
//							title for the output window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1995
//	Revised By:			Larry L. Biehl			Date: 05/19/1997 
 
void GetOutputWindowTitle (
				UCharPtr								titleStringPtr)

{           
	#if defined multispec_mac  
	   if (gOutputWindow != NULL)                                                
			GetWTitle (gOutputWindow, titleStringPtr);
	#endif	// defined multispec_mac 
	
	#if defined multispec_win
	   if (gOutputViewCPtr != NULL)
	   	{                                                           
			CMTextDoc* documentCPtr = gOutputViewCPtr->GetDocument ();
			CString titleString = documentCPtr->GetTitle ();
			     
			UInt16 titleLength = titleString.GetLength ();
			LPTSTR titleBufferPtr = titleString.GetBuffer (titleLength);
         
         		// Move the window title making sure that there are no
         		// spaces in the name.
         
         UInt16 		inIndex, 
         				outIndex = 1;
         
         for (inIndex=0; inIndex<titleLength; inIndex++)
         	{
         	if (titleBufferPtr[inIndex] != ' ')
         		{
         		titleStringPtr[outIndex] = (char)titleBufferPtr[inIndex];
         		outIndex++;
         		
         		}	// end "if (titleBufferPtr[inIndex] != ' ')"
         		
         	}	// end "for (inIndex=0; inIndex<titleLength; inIndex++)"
							
			titleString.ReleaseBuffer ();
			
					// Make the string a pascal string and terminate it with as
					// if it were a c string.
				
			titleStringPtr[0] = (UInt8)(outIndex - 1);
			titleStringPtr[outIndex] = 0; 
			
			}	// end "if (gOutputViewCPtr != NULL)"
	#endif	// defined multispec_win  
	
	#if defined multispec_lin
		// TODO: After Basic GUI setup
	#endif
	
}	// end "GetOutputWindowTitle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetSpecifiedString
//
//	Software purpose:	The purpose of this routine is to get a handle, pointer
//							and length of the specified string from the Resource file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/17/1993
//	Revised By:			Larry L. Biehl			Date: 12/12/2011

SInt32 GetSpecifiedString (
				SInt16								stringID, 
				StringHandle*						stringHandlePtr, 
				char**								stringPtrPtr)

{	
	SInt32								stringLength;
	
	
	stringLength = 0;
	*stringPtrPtr = NULL;
	
	#if defined multispec_mac  
		*stringHandlePtr = GetString (stringID);
		if (*stringHandlePtr)
			{
			HLock ((Handle)*stringHandlePtr);
			*stringPtrPtr = (char*)**stringHandlePtr;
			stringLength = GetHandleSize ((Handle)*stringHandlePtr);
			
			}	// end "if (*stringHandlePtr)"
	#endif	// defined multispec_mac  
	
	#if defined multispec_win || defined multispec_lin	
		unsigned char*		stringPtr = NULL;  

		unsigned char		string128[256] = {  
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x3e, 0x3c, 0x28, 0x3a, 0x5e,
											  0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x5c,
											  0x3d, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x5f, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x3a, 0x23, 0x40, 0x25, 0x3d, 0x22,
											  0x00, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
											  0x68, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
											  0x71, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
											  0x79, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x5b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
											  0x48, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x5d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
											  0x51, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x5c, 0x00, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
											  0x59, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
											  0x38, 0x39, 0x5e, 0x00, 0x00, 0x00, 0x00, 0x00};
/*	
		unsigned char		string129[70] = {  
											  0x22, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
											  0x52, 0x00, 0x59, 0x00, 0x61, 0x00, 0x70, 0x00,
											  0x89, 0x00, 0x91, 0x00, 0x97, 0x00, 0x9C, 0x00,
											  0xA5, 0x00, 0xB1, 0x00, 0xB8, 0x00, 0xBC, 0x00,
											  0xC1, 0x00, 0xC6, 0x00, 0xCA, 0x00, 0xCE, 0x00,
											  0xD2, 0x00, 0xD6, 0x00, 0xE4, 0x00, 0xEB, 0x00,
											  0xF4, 0x00, 0xF9, 0x00, 0x01, 0x01, 0x09, 0x01,
											  0x38, 0x00, 0x18, 0x01, 0x1F, 0x01, 0x25, 0x01,
											  0x2E, 0x01, 0x32, 0x01, 0x37, 0x01};
	
		unsigned char		string132[65] = {   
											  0x07, 0x00, 0x10, 0x00, 0x18, 0x00, 0x1C, 0x00,
											  0x20, 0x00, 0x24, 0x00, 0x2B, 0x00, 0x39, 0x00,
											  'U',  'n',  'k',  'n',  'o',  'w',  'n',  0x00,
											  'B',  'I',  'L',  0x00, 'B',  'S',  'Q',  0x00, 
											  'B',  'I',  'S',  0x00, 'B',  'N',  'o',  'n',
											  'S',  'Q',  0x00, 'B',  'N',  'o',  'n',  'S',
											  'Q',  'B',  'l',  'o',  'c',  'k',  'e',  'd',
											  0x00, 'B',  'I',  'B',  'l',  'o',  'c',  'k',
											  0x00 }; 
	
		unsigned char		string133[320] = {  
											  0x22, 0x00, 0x38, 0x00, 0x40, 0x00, 0x48, 0x00,
											  0x52, 0x00, 0x59, 0x00, 0x61, 0x00, 0x70, 0x00,
											  0x89, 0x00, 0x91, 0x00, 0x97, 0x00, 0x9C, 0x00,
											  0xA5, 0x00, 0xB1, 0x00, 0xB8, 0x00, 0xBC, 0x00,
											  0xC1, 0x00, 0xC6, 0x00, 0xCA, 0x00, 0xCE, 0x00,
											  0xD2, 0x00, 0xD6, 0x00, 0xE4, 0x00, 0xEB, 0x00,
											  0xF4, 0x00, 0xF9, 0x00, 0x01, 0x01, 0x09, 0x01, 
											  'U',  'n',  'k',  'n',  'o',  'w',  'n',  0x00,		// 64
											  'E',  'r',  'd',  'a',  's',  '7',  '3',  0x00,
											  'M',  'u',  'l',  't',  'i',  'S',  'p',  'e',
											  'c',  0x00, 'A',  'V',  'I',  'R',  'I',  'S',
											  0x00, 'U',  'n',  'k',  'n',  'o',  'w',  'n',
											  0x00, 'S',  'u',  'n',  ' ',  'S',  'c',  'r',
											  'e',  'e',  'n',  'D',  'u',  'm',  'p',  0x00,
											  'M',  'u',  'l',  't',  'i',  'S',  'p',  'e',
											  'c',  ' ',  'C',  'l',  'a',  's',  's',  'i',		// 128
											  'f',  'i',  'c',  'a',  't',  'i',  'o',  'n',
											  0x00, 'E',  'r',  'd',  'a',  's',  '7',  '4', 
											  0x00, 'V',  'I',  'C',  'A',  'R',  0x00, 'G',
											  'A',  'I',  'A',  0x00, 'M',  'a',  'c',  'S', 
											  'A',  'D',  'I',  'E' , 0x00, 'L',  'A',  'R',
											  'S',  'Y',  'S',  ' ',  'M',  'I',  'S',  'T', 
											  0x00, 'M',  'A',  'T',  'L',  'A',  'B',  0x00, 
											  'T',  'G',  'A',  0x00, 'G',  'A',  'I',  'A',		// 192 
											  0x00, 'T',  'I',  'F',  'F',  0x00, 'L',  'A', 
											  'S',  0x00, 'L',  'A',  'S',  0x00, 'H',  'D', 
											  'F',  0x00, 'P',  'D',  'S',  0x00, 'E',  'R',
											  'D',  'A',  'S',  ' ',  'I',  'm',  'a',  'g',
											  'i',  'n',  'e',  0x00, 'L',  'G',  'S',  'O', 
											  'W',  'G',  0x00, 'F',  'A',  'S',  'T',  '-', 
											  'L',  '7',  'A',  0x00, 'E',  'N',  'V',  'I',  
											  0x00, 'A',  'r',  'c',  'V',  'i',  'e',  'w',		// 256
											  0x00, 'G',  'e',  'o',  'T',  'I',  'F',  'F',		// 264
											  0x00, 'W',  'i',  'n',  'd',  'o',  'w',  's',		// 272
											  ' ',  'B',  'i',  't',  'M',  'a',  'p',  0x00,		// 280
											  'n',  'e',  't',  'C',  'D',  'F',  0x00, 'N',		// 288
											  'L',  'A',  'P',  'S', 0x00,  'J',  'P',  'E',		// 296
											  'G',  '2',  '0',  '0',  '0',  0x00, 'P',  'N',		// 304
											  'G',  0x00, 'J',  'P',  'E',  'G',  0x00, 'H',		// 312
											  'D',  'F',  '5', 0x00, 0x00, 0x00, 0x00, 0x00 };		// 320
*/	
		unsigned char		string137[1063] = { 
											  ' ', 'G', 'R', 'O', 'U', 'P', ' ', 'D',
											  'E', 'S', 'C', 'R', 'I', 'P', 'T', 'I',
											  'O', 'N', 'S', ' ', '1', ' ', '1', ' ',
											  '9', ' ', 'G', 'r', 'o', 'u', 'p', 'i',
											  'n', 'g', '1', ' ', ' ', ' ', '1', '6',
											  ' ', '1', ' ', ' ', ' ', ' ', '1', ' ',
											  ' ', ' ', ' ', '2', ' ', ' ', ' ', ' ',
											  '2', ' ', ' ', ' ', ' ', '2', ' ', ' ',
											  ' ', ' ', '2', ' ', ' ', ' ', ' ', '2',
											  ' ', ' ', ' ', ' ', '2', ' ', ' ', ' ', 
											  ' ', '2', ' ', ' ', ' ', ' ', '2', ' ', 
											  ' ', ' ', ' ', '2', ' ', ' ', ' ', ' ',
											  '2', ' ', ' ', ' ', ' ', '2', ' ', ' ',
											  ' ', ' ', '2', ' ', ' ', ' ', ' ', '2',
											  ' ', ' ', ' ', ' ', '2', ' ', ' ', ' ', 
											  ' ', '2', ' ', ' ', ' ', ' ', '2', ' ',
											  ' ', ' ', ' ', '2', ' ', ' ', ' ', ' ',
											  '2', ' ', ' ', ' ', ' ', '2', ' ', ' ',
											  ' ', ' ', '2', ' ', ' ', ' ', ' ', '2',
											  ' ', ' ', ' ', ' ', '2', ' ', ' ', ' ', 
											  ' ', '3', ' ', ' ', ' ', ' ', '4', ' ', 
											  ' ', ' ', ' ', '5', ' ', ' ', ' ', ' ',
											  '6', ' ', ' ', ' ', ' ', '7', ' ', ' ',
											  ' ', ' ', '7', ' ', ' ', ' ', ' ', '7',
											  ' ', ' ', ' ', ' ', '7', ' ', ' ', ' ', 
											  ' ', '7', ' ', ' ', ' ', ' ', '8', ' ',
											  ' ', ' ', ' ', '8', ' ', ' ', ' ', ' ',
											  '8', ' ', ' ', ' ', ' ', '8', ' ', ' ',
											  ' ', ' ', '8', ' ', ' ', ' ', ' ', '8',
											  ' ', ' ', ' ', ' ', '8', ' ', ' ', ' ', 
											  ' ', '8', ' ', ' ', ' ', ' ', '8', ' ',
											  ' ', ' ', ' ', '8', ' ', ' ', ' ', ' ',
											  '9', ' ', ' ', ' ', ' ', '9', ' ', ' ',
											  ' ', ' ', '9', ' ', ' ', ' ', ' ', '9',
											  ' ', ' ', ' ', ' ', '9', ' ', ' ', ' ', 
											  ' ', '9', ' ', ' ', ' ', ' ', '9', ' ',
											  ' ', ' ', ' ', '9', ' ', ' ', ' ', ' ',
											  '9', ' ', ' ', ' ', ' ', '9', ' ', ' ',
											  ' ', '1', '0', ' ', ' ', ' ', '1', '0',
											  ' ', ' ', ' ', '1', '0', ' ', ' ', ' ', 
											  '1', '0', ' ', ' ', ' ', '1', '0', ' ',
											  ' ', ' ', '1', '0', ' ', ' ', ' ', '1',
											  '0', ' ', ' ', ' ', '1', '0', ' ', ' ',
											  ' ', '1', '0', ' ', ' ', ' ', '1', '0',
											  ' ', ' ', ' ', '1', '1', ' ', ' ', ' ', 
											  '1', '1', ' ', ' ', ' ', '1', '1', ' ',
											  ' ', ' ', '1', '1', ' ', ' ', ' ', '1',
											  '1', ' ', ' ', ' ', '1', '1', ' ', ' ',
											  ' ', '1', '1', ' ', ' ', ' ', '1', '1',
											  ' ', ' ', ' ', '1', '1', ' ', ' ', ' ', 
											  '1', '1', ' ', ' ', ' ', '1', '2', ' ',
											  ' ', ' ', '1', '2', ' ', ' ', ' ', '1',
											  '2', ' ', ' ', ' ', '1', '2', ' ', ' ',
											  ' ', '1', '2', ' ', ' ', ' ', '1', '2',
											  ' ', ' ', ' ', '1', '2', ' ', ' ', ' ', 
											  '1', '2', ' ', ' ', ' ', '1', '2', ' ',
											  ' ', ' ', '1', '2', ' ', ' ', ' ', '1',
											  '3', ' ', ' ', ' ', '1', '3', ' ', ' ',
											  ' ', '1', '3', ' ', ' ', ' ', '1', '3',
											  ' ', ' ', ' ', '1', '3', ' ', ' ', ' ', 
											  '1', '3', ' ', ' ', ' ', '1', '3', ' ',
											  ' ', ' ', '1', '3', ' ', ' ', ' ', '1',
											  '3', ' ', ' ', ' ', '1', '3', ' ', ' ',
											  ' ', '1', '4', ' ', ' ', ' ', '1', '4',
											  ' ', ' ', ' ', '1', '4', ' ', ' ', ' ', 
											  '1', '4', ' ', ' ', ' ', '1', '4', ' ',
											  ' ', ' ', '1', '4', ' ', ' ', ' ', '1',
											  '4', ' ', ' ', ' ', '1', '4', ' ', ' ',
											  ' ', '1', '4', ' ', ' ', ' ', '1', '4',
											  ' ', ' ', ' ', '1', '5', ' ', ' ', ' ', 
											  '1', '5', ' ', ' ', ' ', '1', '5', ' ',
											  ' ', ' ', '1', '5', ' ', ' ', ' ', '1',
											  '5', ' ', ' ', ' ', '1', '5', ' ', ' ',
											  ' ', '1', '5', ' ', ' ', ' ', '1', '5',
											  ' ', ' ', ' ', '1', '5', ' ', ' ', ' ', 
											  '1', '5', ' ', ' ', ' ', '1', '6', ' ',
											  ' ', ' ', '1', '6', ' ', ' ', ' ', '1',
											  '6', ' ', ' ', ' ', '1', '6', ' ', ' ',
											  ' ', '1', '6', ' ', ' ', ' ', '1', '6',
											  ' ', ' ', ' ', '1', '6', ' ', ' ', ' ', 
											  '1', '6', ' ', ' ', ' ', '1', '6', ' ',
											  ' ', ' ', '1', '6', ' ', ' ', ' ', '1',
											  '0', ' ', 'b', 'a', 'c', 'k', 'g', 'r',
											  'o', 'u', 'n', 'd', ' ', 'f', 'f', '0',
											  '0', ' ', 'f', 'f', '0', '0', ' ', 'f', 
											  'f', '0', '0', ' ', ' ', ' ', ' ', '3',
											  ' ', '0', '-', '1', ' ', ' ', ' ', ' ',
											  '0', ' ', ' ', ' ', ' ', '0', ' ', 'f',
											  'e', '0', '0', ' ', ' ', ' ', ' ', '3',
											  ' ', '1', '-', '2', ' ', ' ', ' ', ' ', 
											  '0', ' ', 'f', 'a', 'e', '0', ' ', 'a',
											  '1', '4', '7', ' ', ' ', ' ', ' ', '3',
											  ' ', '2', '-', '3', ' ', ' ', ' ', ' ',
											  '0', ' ', 'f', 'a', 'e', '0', ' ', '8', 
											  'f', '5', 'b', ' ', ' ', ' ', ' ', '3',
											  ' ', '3', '-', '4', ' ', ' ', ' ', ' ',
											  '0', ' ', 'f', 'a', 'e', '0', ' ', '7',
											  'f', 'f', 'f', ' ', ' ', ' ', ' ', '3',
											  ' ', '4', '-', '5', ' ', ' ', ' ', ' ', 
											  '0', ' ', 'f', 'a', 'e', '0', ' ', '7',
											  '0', 'a', '3', ' ', ' ', ' ', ' ', '4',
											  ' ', '5', '-', '1', '0', ' ', ' ', ' ',
											  ' ', '0', ' ', 'f', 'a', 'e', '0', ' ',
											  '4', 'f', '5', 'b', ' ', ' ', ' ', ' ',
											  '5', ' ', '1', '0', '-', '2', '0', ' ',
											  ' ', ' ', ' ', '0', ' ', 'f', 'a', 'e',
											  '0', ' ', ' ', 'f', '5', 'c', ' ', ' ',
											  ' ', ' ', '5', ' ', '2', '0', '-', '3',
											  '0', ' ', '3', 'f', 'f', 'f', ' ', 'f',
											  'a', 'e', '0', ' ', ' ', ' ', ' ', '0',
											  ' ', ' ', ' ', ' ', '5', ' ', '3', '0',
											  '-', '4', '0', ' ', '8', 'f', '5', 'b',
											  ' ', 'f', 'a', 'e', '0', ' ', ' ', ' ',
											  ' ', '0', ' ', ' ', ' ', ' ', '5', ' ',
											  '4', '0', '-', '5', '0', ' ', 'e', '1',
											  '4', '6', ' ', 'e', 'a', 'e', '0', ' ',
											  ' ', ' ', ' ', '0', ' ', ' ', ' ', ' ',
											  '5', ' ', '5', '0', '-', '6', '0', ' ',
											  'f', 'd', '6', 'f', ' ', 'c', '5', '1',
											  'd', ' ', ' ', ' ', ' ', '0', ' ', ' ',
											  ' ', ' ', '5', ' ', '6', '0', '-', '7',
											  '0', ' ', 'f', 'd', '6', 'f', ' ', '8',
											  'c', 'c', 'c', ' ', ' ', ' ', ' ', '0', 
											  ' ', ' ', ' ', ' ', '5', ' ', '7', '0',
											  '-', '8', '0', ' ', 'f', 'd', '6', 'f',
											  ' ', '6', '3', 'd', '6', ' ', ' ', ' ',
											  ' ', '0', ' ', ' ', ' ', ' ', '5', ' ',
											  '8', '0', '-', '9', '0', ' ', 'f', 'e',
											  '6', 'f', ' ', '3', 'd', '7', '0', ' ',
											  ' ', ' ', ' ', '0', ' ', ' ', ' ', ' ',
											  '6', ' ', '9', '0', '-', '1', '0', '0',
											  ' ', 'f', 'd', '6', 'f', ' ', ' ', '5',
											  '1', 'e', ' ', ' ', ' ', ' ', '0',  }; 
		
		
		stringLength = 256; 
		/*		
		if (stringID == 129)
			{
			stringLength = sizeof (string129);
			stringPtr = &string129[0];
			
			}	// end "if (stringID == 129)"
		
		if (stringID == 132)
			{
			stringLength = sizeof (string132);
			stringPtr = &string132[0];
			
			}	// end "if (stringID == 132)" 
		
		else if (stringID == 133)
			{
			stringLength = sizeof (string133);
			stringPtr = &string133[0];
			
			}	// end "else if (stringID == 133)"
		*/		
		if (stringID == 128)
			{
			stringLength = sizeof (string128);
			stringPtr = &string128[0];
			
			}	// end "else if (stringID == 128)"

		else if (stringID == 137)
			{
			stringLength = sizeof (string137);
			stringPtr = &string137[0];
			
			}	// end "else if (stringID == 137)"
			
			
		*stringHandlePtr = (StringHandle)MNewHandle (stringLength);
		
		*stringPtrPtr = (char*)GetHandlePointer ((Handle)*stringHandlePtr, kLock); 
															
		if (*stringPtrPtr != NULL)
			{                    
			if (stringPtr != NULL)
				{
				BlockMoveData (stringPtr, *stringPtrPtr, stringLength);
				
				}	// end "if (stringID == 137)" 
				  
			else	// stringPtr == NULL
				{
				MGetString ((UCharPtr)*stringPtrPtr, NULL, stringID);
			                     
				stringLength = (*stringPtrPtr)[0];
				
				}	// end "else stringPtr == NULL" 
			
			}	// end "if (*stringPtrPtr != NULL)"
	#endif	// defined multispec_win || defined multispec_lin
		
	return (stringLength);

}	// end "GetSpecifiedString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSpecifiedStringNumber
//
//	Software purpose:	This routine get the specified string number.  A
//							NULL character is added to the end of the string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string obtained successfully.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1994
//	Revised By:			Larry L. Biehl			Date: 02/27/2017			

Boolean GetSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				UCharPtr								textStringPtr, 
				Boolean								continueFlag)
													
{
	if (continueFlag)
		{												
		MGetString ((UCharPtr)textStringPtr, strListID, index);
		continueFlag = (textStringPtr[0] != 0);
		
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "GetSpecifiedStringNumber"  


#if defined multispec_win
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSpecifiedStringNumber
//
//	Software purpose:	This routine get the specified string number.  A
//							NULL character is added to the end of the string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string obtained successfully.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/26/2016
//	Revised By:			Larry L. Biehl			Date: 08/28/2017			

Boolean GetSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				TBYTE*								textStringPtr, 
				Boolean								continueFlag,
				UInt16								maxStringLength)
													
{
	if (continueFlag)
		{												
		MGetString (textStringPtr, strListID, index, maxStringLength);
		continueFlag = (textStringPtr[0] != 0);
		
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "GetSpecifiedStringNumber"  
#endif	// defined multispec_win



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetStringToComma
//
//	Software purpose:	This routine copies the characters from the input string to the
//							output string until a comma is found.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			ReadENVIHeaderMapInfo in SOpnImag.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/14/2006
//	Revised By:			Larry L. Biehl			Date: 08/25/2006

char* GetStringToComma (
				char*			 						inputStringPtr,
				char*									inputStringEndPtr, 
				char*									stringPtr,
				SInt16								maxStringLength)

{		
	char*									outputStringPtr;
	
	SInt16								numberCharacters;
	
	
	numberCharacters = 0;
	if (inputStringPtr != NULL && inputStringEndPtr > inputStringPtr)
		{
				// Skip any leading blanks.
			
		while (*inputStringPtr == ' ')
			inputStringPtr++;
				
			// Now find the ',' if requested.  For this option one will want to read
			// all characters to the ','
			
		outputStringPtr = (char*)strstr (inputStringPtr, ",");
		
		if (outputStringPtr == NULL || outputStringPtr > inputStringEndPtr)
			outputStringPtr = inputStringEndPtr;
		
		if (outputStringPtr != NULL)
			numberCharacters = (SInt16)(outputStringPtr - inputStringPtr);
			
		if (numberCharacters >= 0)
			{
					// Set string limit to allow for pascal and c string identifiers.
		
			numberCharacters = MIN (numberCharacters, maxStringLength-2);
		
					// Get the string that follows the identifer that is numberCharacters
					// long.				
			
			BlockMoveData (inputStringPtr, &stringPtr[1], numberCharacters);
			
					// Skip the comma
					
			outputStringPtr++;
				
			}	// end "if (numberCharacters >= 0" 
		
		}	// end "if (inputStringPtr != NULL && ..."
		
	else	// inputStringPtr == NULL || ...
		outputStringPtr = NULL;
	
	if (maxStringLength >= 2)
		{
				// Make pascal and c string.
				
		stringPtr[0] = (char)numberCharacters;
		stringPtr[numberCharacters+1] = 0;
			
		}	// end "if (maxStringLength >= 2)"
		
	return (outputStringPtr);
	
}	// end "GetStringToComma"   	  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetWindowTitle
//
//	Software purpose:	The purpose of this routine is to get the title for the input 
//							window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/13/2013
//	Revised By:			Larry L. Biehl			Date: 06/11/2015 
 
void GetWindowTitle (
				WindowPtr							windowPtr,
				UCharPtr								titleStringPtr)

{  	
	titleStringPtr[0] = 0;
	titleStringPtr[1] = 0;
	if (windowPtr != NULL)
		{
		#if defined multispec_mac  
			GetWTitle (windowPtr, titleStringPtr);
			titleStringPtr[titleStringPtr[0]+1] = 0;
		#endif	// defined multispec_mac 
	
		#if defined multispec_win
			CMImageDoc* documentCPtr = windowPtr->GetDocument ();
			CString titleString = documentCPtr->GetTitle ();
			     
			UInt16 titleLength = titleString.GetLength ();
			LPTSTR titleBufferPtr = titleString.GetBuffer (titleLength);

			memcpy (&titleStringPtr[1], 
						titleBufferPtr, 
						titleLength);
							
			titleString.ReleaseBuffer ();
				
			titleStringPtr[0] = (UInt8)titleLength;
			titleStringPtr[titleLength+1] = 0; 
		#endif	// defined multispec_win 
	
		#if defined multispec_lin
         wxString titleString;
			titleString = windowPtr->m_frame->GetTitle ();
			     
			UInt16 titleLength = MIN ((UInt16)titleString.Length (), 254);
			
			strncpy ((char*)&titleStringPtr[1], (const char*)titleString.mb_str (wxConvUTF8), 254);
							
			titleStringPtr[0] = (UInt8)titleLength;
			titleStringPtr[titleLength+1] = 0; 
		#endif	// defined multispec_lin 
			
		}	// end "if (windowPtr != NULL)"
		
}	// end "GetWindowTitle"


#if defined multispec_lin
void GetGraphWindowTitle (
				WindowPtr							windowPtr,
				UCharPtr								titleStringPtr)
{
   
   titleStringPtr[0] = 0;
	titleStringPtr[1] = 0;
   
   
	if (windowPtr != NULL)
		{
      wxString titleString = ((CMGraphView*)windowPtr)->m_frame->GetTitle ();
      UInt16 titleLength = MIN ((UInt16)titleString.Length (), 254);
			
		strncpy ((char*)&titleStringPtr[1], (const char*)titleString.mb_str (wxConvUTF8), 254);
							
		titleStringPtr[0] = (UInt8)titleLength;
		titleStringPtr[titleLength+1] = 0; 
		}
	
}	// end "GetGraphWindowTitle"
#endif	// defined multispec_lin


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeDateVersionStrings
//
//	Software purpose:	The purpose of this routine is to initialize the global date 
//							version string and the application identifier string.  It is 
//							called once just after MultiSpec is started.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/09/2007
//	Revised By:			Larry L. Biehl			Date: 08/19/2019

void InitializeDateVersionStrings ()

{
		// Date version string
		
	sprintf (gDateVersionString, "2019.08.19");

		// Application identifier string
		
	#if defined multispec_mac
		#ifndef __XCODE__
			#ifdef TARGET_CPU_PPC
				sprintf (gApplicationIdentifierString, "MultiSpecPPC_%s", gDateVersionString);
			#endif		// TARGET_CPU_PPC
			
			sprintf (gApplicationIdentifierString, "MultiSpecCarb_%s", gDateVersionString);
		#endif		// !__XCODE__
			
		#ifdef __XCODE__		
			if (gSystemArchitectureCode == kIntel)
				sprintf (gApplicationIdentifierString, "MultiSpecIntel_%s", gDateVersionString);
				
			if (gSystemArchitectureCode == kPPC)
				sprintf (gApplicationIdentifierString, "MultiSpecUniversalPPC_%s", gDateVersionString);
		#endif		// __XCODE__
	#endif 	// defined multispec_mac
		
	#if defined multispec_win
		#if defined _WIN64
			sprintf (gApplicationIdentifierString, "MultiSpecWin64_%s", gDateVersionString);
		#else
			sprintf (gApplicationIdentifierString, "MultiSpecWin32_%s", gDateVersionString);
		#endif
	#endif 	// defined multispec_win
	
   #if defined multispec_lin
		#ifdef NetBeansProject
			sprintf ((char*)gApplicationIdentifierString, "MultiSpecLinux_%s", gDateVersionString);
		#else
			#if defined multispec_wxmac
				sprintf ((char*)gApplicationIdentifierString, "MultiSpec64_%s", gDateVersionString);
			#else
				sprintf ((char*)gApplicationIdentifierString, "MultiSpec_on_MyGeohub_%s", gDateVersionString);
			#endif
		#endif
   #endif

}	// end "InitializeDateVersionStrings"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 InsertCommasInNumberString
//
//	Software purpose:	The purpose of this routine is to insert commas into the input
//							number string. The insertion is done within the input string
//							so make sure that the input string is long enough.
//
//	Parameters in:		decimalPlaces:  if <0, then integer value is assumed.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/09/2000
//	Revised By:			Larry L. Biehl			Date: 07/02/2002	

SInt16 InsertCommasInNumberString (
				char*									inputStringPtr, 
				UInt32								numberInputCharacters, 
				SInt16								decimalPlaces,
				SInt16								numberOutputCharacters)

{
	SInt32								decimalLocation,
											firstBlankIndex,
											index,
											newIndex;
											
	UInt32								numberBlanks,
											numberCommas,
											numberPlaces,
											numberWholeValueDigits;
											
	SInt16								justificationCode;
	
	
			// Initialize local variables.
			
	justificationCode = 1;
	if (numberOutputCharacters < 0)
		justificationCode = -1;
		
	numberOutputCharacters = abs (numberOutputCharacters);

			// Get the number of digits less any decimal places and the period.
			
	if (decimalPlaces < 0)
		{
		decimalPlaces = 0;
		decimalLocation = numberInputCharacters;
		
		}	// end "if (decimalPlaces < 0)"
			
	else	// decimalPlaces >= 0
		{
		decimalPlaces++;
		decimalLocation = numberInputCharacters - decimalPlaces;
		decimalLocation = MAX (0, decimalLocation);
		
		}	// end "else decimalPlaces >= 0"
	
	numberWholeValueDigits = decimalLocation;
	
			// Do not include a blank as a whole number digit
	
	index = 0;
	while (inputStringPtr[index] == ' ' && index < (SInt32)numberWholeValueDigits)
		{
		numberWholeValueDigits--;
		index++;
		
		}	// end "while (inputStringPtr[index] == ' ')"
		
	numberBlanks = index;
	
			// Do not include a leading '-' as a whole number digit
			
	if (inputStringPtr[index] == '-')
		numberWholeValueDigits--;
	
	if (numberWholeValueDigits > 0)
		numberCommas = (numberWholeValueDigits - 1)/3;
		
	else	// numberWholeValueDigits == 0
		numberCommas = 0;
	
	if (numberCommas > 0 || numberInputCharacters < (UInt32)numberOutputCharacters)
		{
				// Make sure that the number of output characters will handle
				// the number of commas being added.
				
		numberOutputCharacters = (SInt16)MAX ((UInt32)numberOutputCharacters,
										numberInputCharacters + numberCommas);
				
				// Limit the number of output characters to 60 to be on the
				// safe side.
				
		numberOutputCharacters = MIN (numberOutputCharacters, 60);
					
		index = numberInputCharacters - 1;
		newIndex = numberOutputCharacters - 1;
												
		if (justificationCode < 0)
			{
					// Left justify the number.
					
					// Add spaces to the end of the number if needed.
			
			firstBlankIndex = numberInputCharacters + numberCommas;
			while (newIndex >= firstBlankIndex)
				{
				inputStringPtr[newIndex] = ' ';
				newIndex--;
				
				}	// end "while (while (newIndex >= firstBlankIndex)"
				
			}	// end "if (justificationCode < 0)"
		
				// Make sure that the decimal separator is the correct character.
				
		if (decimalPlaces > 0)
			inputStringPtr[decimalLocation] = gDecimalSeparator;
			
					// Move decimal digits.
		
		while (index >= decimalLocation)
			{
			inputStringPtr[newIndex] = inputStringPtr[index];
			index--;
			newIndex--;
				
			}	// end "while (index >= decimalLocation)"
		
		numberPlaces = 0;		
		while (newIndex >= 0)
			{
			if (numberPlaces == 3 && index >= 0 && 
					inputStringPtr[index] != ' ' && inputStringPtr[index] != '-')
				{
				inputStringPtr[newIndex] = gThousandsSeparator;
				newIndex--;
				numberPlaces = 0;
				
				}	// end "if (numberPlaces == 3 && ..."
				
			if (index == newIndex)
				break;
				
			if (index >= 0)
				{
				inputStringPtr[newIndex] = inputStringPtr[index];
				index--;
				
				if (index < decimalLocation)
					numberPlaces++;
				
				if (index == 0 && inputStringPtr[index] == '-')
					numberPlaces = 0;
					
				}	// end "if (index >= 0)"
				
			else	// index < 0
				inputStringPtr[newIndex] = ' ';
				
			newIndex--;
				
			}	// end "while (newIndex >= 0)"
			
		inputStringPtr[numberOutputCharacters] = 0;
				
		}	// end "if (numberCommas > 0)"
		
	return (numberOutputCharacters);
	
}	// end "InsertCommasInNumberString"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListChannelsUsed
//
//	Software purpose:	The purpose of this routine is to list the channels
//							used for the processor.  The output will be listed
//							in the output text window and/or the outputdisk file 
//							as specified.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1989
//	Revised By:			Larry L. Biehl			Date: 08/28/2017	

Boolean ListChannelsUsed (
				FileInfoPtr							fileInfoPtr, 
				SInt16*								featuresPtr, 
				SInt16*								channelsPtr, 
				SInt16								numberChannels, 
				CMFileStream*						outputFilePtr, 
				SInt16*								outputCodePtr, 
				Boolean								useTransformFlag)

{
			// Declare local variables and structures										
	
	ChannelDescriptionPtr			channelDescriptionPtr;
	FileInfoPtr							localFileInfoPtr;							
	LayerInfoPtr						localImageLayerInfoPtr;
	SInt16*								lChannelsPtr;
	
	SInt32								stringLength;
	
	UInt32								channelNum,
											index, 
											layerChanIndex,
											nextChannelNum,
											nextIndex;
	
	SInt16								channelNumberLength,
											fileChanNum,
											fileInfoIndex;
	
	Boolean								continueFlag,
											shortListFlag;
	
	
			// Check input variables.															
			
	if (fileInfoPtr == NULL && gImageLayerInfoPtr != NULL)
																				return (FALSE);
			
	if (fileInfoPtr != NULL && gImageLayerInfoPtr == NULL)
																				return (FALSE);
	
			// Initialize local variables.													
			
	channelDescriptionPtr = NULL;
	localFileInfoPtr = NULL;
	continueFlag = TRUE;
	gTextString2[16] = 0;
	shortListFlag = TRUE;
	
	lChannelsPtr = channelsPtr;
	if (useTransformFlag)
		lChannelsPtr = featuresPtr;
	
			// List the channels or features used.											
	
	if (useTransformFlag)
		{
		if (gProcessorCode == kRefChangeFileFormatProcessor)
			stringLength = sprintf ((char*)gTextString, 
						"    Features created: %s",
						gEndOfLine);
						
		else	// gProcessorCode != kRefChangeFileFormatProcessor
			stringLength = sprintf ((char*)gTextString, 
						"    Features used: %s",
						gEndOfLine);
						
		}	// end "if (useTransformFlag)"
	
	else	// !useTransformFlag 
		stringLength = sprintf ((char*)gTextString, 
					"    Channels used: %s",
					gEndOfLine);
	
			// If all channels are to be used and there are no channel				
			// descriptions then just list the channels.									
			// For now we will only do this check for those cases where there is	
			// only one disk file for the image "file" being used.					
			
	if (fileInfoPtr && !useTransformFlag && fileInfoPtr->channelDescriptionH)
		shortListFlag = FALSE;
			
	if (gImageWindowInfoPtr && 
			gImageWindowInfoPtr->numberImageFiles > 1 && 
				gProcessorCode != kRefChangeFileFormatProcessor)
		shortListFlag = FALSE;
		
	if (fileInfoPtr != gImageFileInfoPtr && 
						gImageWindowInfoPtr->numberImageFiles > 1 && 
								gProcessorCode != kListDescriptionProcessor)
				// This indicates that the gImageLayerInfoPtr does not go with
				// fileInfoPtr.  This is from a new image being created by a reformat
				// processor. More work needs to be done to be able to determine 
				// what the channel descriptions are if any when linked files are
				// involved.
		shortListFlag = TRUE;
		
			// If a short list is to be used, verify that the list is 				
			// continuous.																				
			
	if (shortListFlag)
		{
		Boolean 		dashFlag = FALSE;
		Boolean 		listFlag = FALSE;
		UInt32 		startChannel;
		
		startChannel = 0;
		if (lChannelsPtr != NULL)
			startChannel = lChannelsPtr[0];
		
		stringLength -= gNumberOfEndOfLineCharacters;
		channelNum = startChannel;
		for (index=0; index<(UInt16)numberChannels; index++)
			{
			channelNum++;
			
			nextIndex = MIN (index + 1, (UInt32)(numberChannels - 1));
			
			nextChannelNum = index + 1;
			if (lChannelsPtr != NULL)
				nextChannelNum = lChannelsPtr[nextIndex];
			
			if ((index == (UInt16)(numberChannels - 1)) ||
									channelNum != (UInt16)nextChannelNum)
						
				{
				listFlag = TRUE;
				
				}	// end "if (index == (UInt16)(numberChannels - 1) || ..." 
				
			else	// channelNum == nextChannel && ...
				{
				if (!dashFlag)
					{
					startChannel = channelNum - 1;
					dashFlag = TRUE;
					
					}	// end "if (!dashFlag && ...)"
				
				}	// end "else lChannelsPtr[index] == channelNum && ..."
				
			if (listFlag)
				{
				if (dashFlag)
					{
							// List the continuous channel list first.
							
					channelNum = nextChannelNum;
					if (lChannelsPtr != NULL)
						channelNum = lChannelsPtr[index] + 1;
						
					stringLength += sprintf ((char*)&gTextString[stringLength], 
														"%ld-%ld,", 
														startChannel+1,
														(UInt32)channelNum);
						
					dashFlag = FALSE;
						
					}	// end "if (dashFlag)"
					
				else	// !dashFlag
					{
							// List channel number separated by comma
							
					stringLength += sprintf ((char*)&gTextString[stringLength], 
						"%ld,", 
						startChannel+1);
						
					}	// end "else !dashFlag"
				
				listFlag = FALSE;
				
				if (index < (UInt16)(numberChannels - 1))
					{
					startChannel = (UInt16)nextChannelNum;
					channelNum = startChannel;
					
					}	// end "if ((index < (UInt16)numberChannels - 1))"
					
				if (stringLength >= 256-15)
					break;
					
				}	// end "if (listFlag)"
				
			}	// end "for (index=0; index<numberChannels; index++)" 
		
				// Remove last comma and add 2 returns.
					
		sprintf ((char*)&gTextString[stringLength-1], 
					"%s",
					gEndOfLine);

		stringLength += gNumberOfEndOfLineCharacters - 1;
			
		}	// end "if (shortListFlag)"
		
	continueFlag = OutputString (outputFilePtr, 
											(char*)gTextString, 
											stringLength, 
											*outputCodePtr, 
											continueFlag);
	
	if (!shortListFlag)
		{	
		fileInfoIndex = -1;
		layerChanIndex = 1;
		localFileInfoPtr = fileInfoPtr;
		
				// If this is a call from the List Description Information processor,
				// this routine will be called once for each linked file. Therefore,
				// we need to find the actual channels to be processed for this
				// call. That will be when the input fileInfoPtr is equal to that
				// for the respective fileInfoIndex.
			
		localImageLayerInfoPtr = gImageLayerInfoPtr;
	   if (gProcessorCode == kListDescriptionProcessor &&
	   										gImageWindowInfoPtr->numberImageFiles > 1)
	   	{
			localImageLayerInfoPtr = NULL;
			
	   			// Find the index into gImageLayerInfoPtr for the correct set
	   			// of channels.
	   	
	   	for (index=0; index<gImageWindowInfoPtr->totalNumberChannels; index++)
	   		{
				if (localImageLayerInfoPtr != NULL && fileInfoIndex != 
								(SInt16)localImageLayerInfoPtr[layerChanIndex].fileInfoIndex)
					{
					fileInfoIndex = localImageLayerInfoPtr[layerChanIndex].fileInfoIndex;
					localFileInfoPtr = &gImageFileInfoPtr[fileInfoIndex];
					if (localFileInfoPtr == fileInfoPtr)
						break;
					
					}	// end "if (gImageLayerInfoPtr != NULL && ..."
						
				layerChanIndex++;
					
				}	// end "for (index=0; index<gImageWindowInfoPtr->..."
	      
	      }	// end "if (gProcessorCode == kListDescriptionProcessor && ..."
						
		stringLength = 9 + gNumberOfEndOfLineCharacters;
		channelNum = 0;
		
		channelNumberLength = 4;
		if (lChannelsPtr != NULL && lChannelsPtr[numberChannels-1] >= 10000)
			channelNumberLength = 5;
		else if (numberChannels >= 10000)
			channelNumberLength = 5;
			
		for (index=0; index<(UInt16)numberChannels; index++)
			{
			if (lChannelsPtr != NULL)
				channelNum = lChannelsPtr[index];
				
			channelNum++;
			
	   	if (gProcessorCode != kListDescriptionProcessor)
	   		layerChanIndex = channelNum;
			
			if (localImageLayerInfoPtr != NULL && fileInfoIndex != 
							(SInt16)localImageLayerInfoPtr[layerChanIndex].fileInfoIndex)
				{
				if (fileInfoIndex != -1)	
					CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
					
				fileInfoIndex = localImageLayerInfoPtr[layerChanIndex].fileInfoIndex;
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
					
				}	// end "if (localImageLayerInfoPtr && fileInfoIndex != ..." 
				
			if (localFileInfoPtr && !useTransformFlag)
				{
				channelDescriptionPtr = NULL;
				stringLength = 5 + channelNumberLength + gNumberOfEndOfLineCharacters;
				if (localFileInfoPtr->channelDescriptionH)
					{
					channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
																localFileInfoPtr->channelDescriptionH,
																kLock);
					stringLength = 23 + channelNumberLength + gNumberOfEndOfLineCharacters;
					
					}	// end "if (localFileInfoPtr->channelDescriptionH)" 
				
				}	// end "if (localFileInfoPtr && !useTransformFlag)" 
			
			if (channelDescriptionPtr == NULL)
				sprintf ((char*)gTextString, 
								"     %*ld%s", 
								channelNumberLength,
								channelNum,
								gEndOfLine);
											
			else	// channelDescriptionPtr != NULL
				{
				fileChanNum = (SInt16)(index + 1);
				if (localImageLayerInfoPtr != NULL)
					fileChanNum = localImageLayerInfoPtr[layerChanIndex].fileChannelNumber;
					
				BlockMoveData (&channelDescriptionPtr[fileChanNum-1], gTextString2, 16);
				sprintf ((char*)gTextString, 
								"     %*ld: %s%s", 
								channelNumberLength,
								channelNum, 
								gTextString2,
								gEndOfLine);
				
				}	// end "else channelDescriptionPtr" 
			
			continueFlag = OutputString (outputFilePtr, 
													(char*)gTextString, 
													stringLength, 
													*outputCodePtr, 
													continueFlag);
			
			if (!continueFlag)
				break;
				
			layerChanIndex++;
				
			}	// end "for (channel=0;..." 
			
		}	// end "if (!shortListFlag)" 
		
			// Insert a blank line after the table.
		
	continueFlag = OutputString (outputFilePtr, 
											gEndOfLine, 
											gNumberOfEndOfLineCharacters, 
											*outputCodePtr, 
											continueFlag);
		
	if (localFileInfoPtr != NULL)
		CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
										
	return (continueFlag);
											
}	// end "ListChannelsUsed"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListCountValue
//
//	Software purpose:	The purpose of this routine is to list the area value within
//							the specific classification summary line.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListClassificationSummary in listResults.c
//
//	Coded By:			Larry L. Biehl			Date: 03/26/2004
//	Revised By:			Larry L. Biehl			Date: 08/14/2010

SInt16 ListCountValue (
				char*									stringPtr,
				SInt64								count,
				SInt16								countNumberWidth,
				SInt16								strLength,
				SInt16								countWidthWithCommas,
				Boolean								includeTabFlag)

{  
	SInt16								numberChars;
	
	
	if (includeTabFlag)
		{		
		numberChars = sprintf (&stringPtr[strLength],  
										"\t");
		strLength += numberChars;
		
		}	// end "if (includeTabFlag)"
	
	if (countNumberWidth > 0)
		numberChars = sprintf (&stringPtr[strLength],  
										"%*lld",
										countNumberWidth,
										count);
										
	else	// countNumberWidth <= 0
		numberChars = sprintf (&stringPtr[strLength],  
										"%lld",
										count);
										
	if (countWidthWithCommas <= 0)
		countWidthWithCommas = numberChars;
		
	numberChars = InsertCommasInNumberString (&stringPtr[strLength], 
															numberChars, 
															-1,
															countWidthWithCommas);
	strLength += numberChars;
										
	return (strLength);
	
}	// end "ListCountValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ListCPUTimeInformation
//
//	Software purpose:	The purpose of this routine is to list the cpu time
//							that the operation took and whether the operation
//							was cancelled.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if listing to output was accomplished with no problems
//							FALSE if not
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/30/1993
//	Revised By:			Larry L. Biehl			Date: 06/29/2016			

Boolean ListCPUTimeInformation (
				CMFileStream*						resultsFileStreamPtr, 
				Boolean								continueFlag, 
				time_t								startTime)
													
{
	UInt32								hours,
											minutes,
											totalTime;
											
	Boolean								localContinueFlag;
	
	
	if (gOutputForce1Code == 0)
																			return (continueFlag);
																				
	localContinueFlag = continueFlag;
	if (gOperationCanceledFlag)
		localContinueFlag = TRUE;
						
			// List the CPU time taken for the processor.							
				
	totalTime = GetTotalTime (startTime);
			
	switch (gProcessorCode)
		{
		case kOpenImageFileProcessor:
			sprintf ((char*)gTextString2, "opening image");
			break;
			
		case kDisplayProcessor:
			sprintf ((char*)gTextString2, "displaying image");
			break;
			
		case kHistogramProcessor:
			sprintf ((char*)gTextString2, "histograming image");
			break;
			
		case kRefChangeHeaderProcessor:
		case kRefChangeFileFormatProcessor:
		case kRefMosaicImagesProcessor:
		case kRefFieldsToThematicFileProcessor:
		case kRectifyImageProcessor:
		case kRefModifyChanDescriptionsProcessor:
		case kMultispecToThematicProcessor:
		case kENVIROIToThematic:
		case kRecodeThematicImageProcessor:
			sprintf ((char*)gTextString2, "reformatting");
			break;
						
		case kListDataProcessor:
			sprintf ((char*)gTextString2, "listing data");
			break;
		
		case kClusterProcessor:
			sprintf ((char*)gTextString2, "clustering");
			break;
		
		case kComputeStatsProcessor:
			sprintf ((char*)gTextString2, "statistics computation");
			break;
			
		case kListStatsProcessor:
			sprintf ((char*)gTextString2, "listing statistics");
			break;
			
		case kHistogramStatsProcessor:
			sprintf ((char*)gTextString2, "histograming statistics");
			break;

		case kStatEnhanceProcessor:
			sprintf ((char*)gTextString2, "enhance statistics");
			break;
			
		case kFeatureExtractionProcessor:
			sprintf ((char*)gTextString2, "feature extraction");
			break;
						
		case kSeparabilityProcessor:
			sprintf ((char*)gTextString2, "feature selection");
			break;
						
		case kClassifyProcessor:
			sprintf ((char*)gTextString2, "classification");
			break;
						
		case kListResultsProcessor:
			sprintf ((char*)gTextString2, "results listing");
			break;
						
		case kPrincipalComponentsProcessor:
			sprintf ((char*)gTextString2, "principal component analysis");
			break;
			
		case kStatisticsImageProcessor:
			sprintf ((char*)gTextString2, "creating statistics images");
			break;
						
		case kBiPlotDataProcessor:
			sprintf ((char*)gTextString2, "creating biplot of data");
			break;
			
		case kCovarianceCheckProcessor:
			sprintf ((char*)gTextString2, "checking covariance statistics");
			break;
			
		case kTransformCheckProcessor:
			sprintf ((char*)gTextString2, "checking transformation matrix");
			break;
			
		default:
			sprintf ((char*)gTextString2, "operation");
			break;
			
		}	// end "switch (gProcessorCode)" 
			
	if (gOperationCanceledFlag)
		{
		if (gAlertReturnCode == 1)
						// Operation was stopped and results to that point were listed.	

			sprintf ((char*)gTextString, 
						"%s %s was stopped.  Results at that point were listed.",
						gEndOfLine, 
						gTextString2);
			
		else	// gAlertReturnCode != 1 
			sprintf ((char*)gTextString, 
						"%s %s was cancelled.",
						gEndOfLine, 
						gTextString2);

		localContinueFlag = OutputString (
						resultsFileStreamPtr, 
						(char*)gTextString, 
						0, 
						gOutputForce1Code, 
						localContinueFlag);
							
		}	// end "if (gOperationCanceledFlag)"

			// Add the date and time to the line.

	time_t currentTime = time (NULL);
	struct tm *currentDate = localtime (&currentTime);
	strftime ((char*)gTextString3, 254, "  %m-%d-%Y  %X", currentDate);
	
	if (totalTime < 60)
		sprintf ((char*)gTextString,
					"%s  %lu CPU seconds for %s.%s%s",
					gEndOfLine,
					totalTime, 
					gTextString2,
					gTextString3,
					gEndOfLine);
					
	else	// totalTime >= 60)
		{
				// Get number of minutes and seconds
				
		minutes = totalTime/60;
		totalTime -= minutes*60;
		
		if (minutes < 60)
			{
			sprintf ((char*)gTextString,
						"%s  %lu minutes and %lu seconds CPU time for %s.%s%s",
						gEndOfLine,
						minutes,
						totalTime, 
						gTextString2,
						gTextString3,
						gEndOfLine);
					
			}	// end "if (minutes < 60)"
					
		else	// minutes >= 60
			{
					// Get number of hours and minutes
					
			hours = minutes/60;
			minutes -= hours*60;
			
			sprintf ((char*)gTextString,
						"%s  %lu hours, %lu minutes and %lu seconds CPU time for %s.%s%s",
						gEndOfLine,
						hours,
						minutes,
						totalTime, 
						gTextString2,
						gTextString3,
						gEndOfLine);
						
			}	// end "else startTime >= 3600"
						
		}	// end "else startTime >= 60)"
	
	continueFlag = OutputString (
					resultsFileStreamPtr, 
					(char*)gTextString, 
					0, 
					gOutputForce1Code, 
					localContinueFlag);	
	/*					
			// Add a carriage return.
			
	continueFlag = OutputString (resultsFileStreamPtr, 
											&gEndOfLine[0], 
											gNumberOfEndOfLineCharacters, 
											gOutputCode, 
											continueFlag);
	*/					
			// List the dashed line separator string to indicate end of processor
			// output.
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
															IDS_Shared6, 
															(unsigned char*)gTextString, 
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															continueFlag);
	
	return (continueFlag);
		
}	// end "ListCPUTimeInformation"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHeaderInfo
//
//	Software purpose:	The purpose of this routine is to list the requested
//							information for the header in the output window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListDataControl in listData.c
//							PrincipalComponentControl in principalComponents.c
//							ListClassificationHeaderInfo in projectUtilities.c
//							SeparabilityControl in separability.c
//
//	Coded By:			Larry L. Biehl			Date: 01/03/1994
//	Revised By:			Larry L. Biehl			Date: 08/02/2013	

Boolean ListHeaderInfo (
				CMFileStream* 						resultsFileStreamPtr, 
				SInt16								listCode, 
				SInt16*								outputCodePtr, 
				SInt16								covarianceStatsToUse, 
				Boolean								continueFlag)

{	
	if (continueFlag && *outputCodePtr > 0)
		{
				// List the dashed line separator string to indicate start of processor
				// output.
				
		ListSpecifiedStringNumber (kSharedStrID, 
												IDS_Shared5, 
												(unsigned char*)gTextString, 
												NULL, 
												*outputCodePtr, 
												continueFlag);
												
				// List the processor name, date and time.									
			
		if (continueFlag)
			continueFlag = ListProcessorTitleLine (
											resultsFileStreamPtr, outputCodePtr, continueFlag);
					
				// List "  Input Parameters:".
				
		if (continueFlag && listCode != 0)	
			continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																	IDS_Shared7, 
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	*outputCodePtr, 
																	continueFlag);
							
				// List image file name and statistics information.						
		
		if (continueFlag && listCode != 0 && listCode != kLInputParametersLineOnly)			
			continueFlag = ListProjectAndImageName (resultsFileStreamPtr, 
																	outputCodePtr, 
																	listCode, 
																	covarianceStatsToUse,
																	continueFlag);
																	
		}	// end "if (continueFlag && *outputCodePtr > 0)"
						
	return (continueFlag);
											
}	// end "ListHeaderInfo" 
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListLineColumnIntervalString
//
//	Software purpose:	The purpose of this routine is to list the input line-
//							column-interval values.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/03/1997
//	Revised By:			Larry L. Biehl			Date: 09/03/1997

Boolean ListLineColumnIntervalString (
				CMFileStream*	 					resultsFileStreamPtr, 
				SInt16								outputCode,
				UInt32								lineStart, 
				UInt32								lineEnd, 
				UInt32								lineInterval,
				UInt32								columnStart, 
				UInt32								columnEnd, 
				UInt32								columnInterval, 
				Boolean 								continueFlag)

{
			// "  Lines %ld to %ld by %ld.  Columns %ld to %ld by %ld.%s"

	if (continueFlag)
		continueFlag = MGetString (gTextString2, kSharedStrID, IDS_Shared3);
		
	if (continueFlag)
		sprintf ((char*)gTextString, 
							(char*)&gTextString2[1],
							lineStart,
							lineEnd,
							lineInterval,
							columnStart,
							columnEnd,
							columnInterval);
			
	continueFlag = OutputString (
							resultsFileStreamPtr, 
							(char*)gTextString, 
							0, 
							outputCode,
							continueFlag);
								
	return (continueFlag);
											
}	// end "ListLineColumnIntervalString" 
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListMapProjectionString
//
//	Software purpose:	The purpose of this routine is to list the name of the map
//							projection to the output text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2007
//	Revised By:			Larry L. Biehl			Date: 03/15/2012	

Boolean ListMapProjectionString (
				Handle								mapProjectionHandle,
				char*									spaceStringPtr,
				Boolean 								continueFlag)

{
	SInt16							projectionCode;
	
	
	if (mapProjectionHandle != NULL)
		{
		projectionCode = GetProjectionCode (mapProjectionHandle);
		
				// Allow for MultiSpec Geographic code which is -1;
				
/*		if (projectionCode > 0)
			projectionCode++;
			
		else if (projectionCode < 0)
			projectionCode = 1;
					
		if (projectionCode <= 0)
			{	
			sprintf ((char*)gTextString,
							"%sNone specified%s",
							spaceStringPtr,
							gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString (
					(char*)gTextString,  strlen ((char*)gTextString), gOutputTextH);
			
			}	// end "if (projectionCode <= 0)"
*/			
		if	(projectionCode > 0)
			{
			#if defined multispec_mac 
				GetMenuItemText (gPopUpProjectionMenu, 
										projectionCode+1, 
										gTextString2);
				gTextString2[gTextString2[0]+1] = 0;
			#endif	// defined multispec_mac   
                             
         #if defined multispec_win || defined multispec_lin
				MGetString (gTextString2,
						0,
						IDS_ProjectionType01+projectionCode);
			#endif	// defined multispec_win
								
			sprintf ((char*)gTextString,
							"%sProjection:       %s%s",
							spaceStringPtr,
							&gTextString2[1],
							gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString (
					(char*)gTextString,  (UInt32)strlen ((char*)gTextString), gOutputTextH);
					
			}	// end "if (projectionCode > 0)"
					
		}	// end "if (mapProjectionHandle != NULL)"
		
	return (continueFlag);
											
}	// end "ListMapProjectionString" 
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListMapReferenceSystemString
//
//	Software purpose:	The purpose of this routine is to list the name of the map
//							reference system to the output text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2012
//	Revised By:			Larry L. Biehl			Date: 03/14/2012	

Boolean ListMapReferenceSystemString (
				Handle								mapProjectionHandle,
				char*									spaceStringPtr,
				Boolean 								continueFlag)

{
	SInt16							referenceSystemCode;
	
	
	if (mapProjectionHandle != NULL)
		{
		referenceSystemCode = GetReferenceSystemCode (mapProjectionHandle);
							
		if (referenceSystemCode <= 0)
			{	
			sprintf ((char*)gTextString,
							"%sNone specified%s",
							spaceStringPtr,
							gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString (
					(char*)gTextString, (UInt32)strlen ((char*)gTextString), gOutputTextH);
			
			}	// end "if (projectionCode <= 0)"
			
		else	// referenceSystemCode > 0
			{
			#if defined multispec_mac 
				GetMenuItemText (gPopUpReferenceSystemMenu, 
										referenceSystemCode+1, 
										gTextString2);
				gTextString2[gTextString2[0]+1] = 0;
			#endif	// defined multispec_mac   
                             
         #if defined multispec_win | defined multispec_lin
				MGetString (gTextString2,
						0,
						IDS_ReferenceSystem01+referenceSystemCode);
			#endif	// defined multispec_win
								
			sprintf ((char*)gTextString,
							"%sReference System: %s%s",
							spaceStringPtr,
							&gTextString2[1],
							gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString (
					(char*)gTextString, (UInt32)strlen ((char*)gTextString), gOutputTextH);
					
			}	// end "else projectionCode > 0"
					
		}	// end "if (mapProjectionHandle != NULL)"
		
	return (continueFlag);
											
}	// end "ListMapReferenceSystemString" 
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ListMemoryMessage
//
//	Software purpose:	The purpose of this routine is to list a message in the text
//							output window indicating that memory was not available to 
//							complete the process.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/10/2009
//	Revised By:			Larry L. Biehl			Date: 07/10/2009	

Boolean ListMemoryMessage (
				CMFileStream*						resultsFileStreamPtr)

{
	Boolean								continueFlag = TRUE;
	
	
	if (gMemoryError != 0)
		{				
		continueFlag = ListSpecifiedStringNumber (kAlertStrID, 
																	IDS_Alert136, 
																	gTextString, 
																	resultsFileStreamPtr, 
																	gOutputForce1Code, 
																	continueFlag);
		
		if (gProcessorCode == kClusterProcessor)
			continueFlag = ListSpecifiedStringNumber (kClusterStrID, 
																		IDS_Cluster50, 
																		gTextString, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code, 
																		continueFlag);
		
		else if (gProcessorCode == kClassifyProcessor)
			continueFlag = ListSpecifiedStringNumber (kClassifyStrID, 
																		IDS_Classify66, 
																		gTextString, 
																		resultsFileStreamPtr, 
																		gOutputForce1Code, 
																		continueFlag);

      #if defined multispec_win | defined multispec_lin
			gMemoryError = 0;
		#endif	// defined multispec_win
		
		}	// end "if (gMemoryError != 0)"
		
	return (continueFlag);
											
}	// end "ListMemoryMessage" 
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListProcessorTitleLine
//
//	Software purpose:	The purpose of this routine is to list the title
//							line for the processor being run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ListHeaderInfo in multiSpecUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1991
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean ListProcessorTitleLine (
				CMFileStream*	 					resultsFileStreamPtr, 
				SInt16*								outputCodePtr, 
				Boolean 								continueFlag)

{
	char									textString[512];
	
	//int									count;

	SInt16								indexStart,
											stringIndex;
											
	Boolean								listClassificationIdentifierFlag;
	
	
			// Initialize local variables.
			
	indexStart = 0;
	listClassificationIdentifierFlag = FALSE;
	stringIndex = IDS_ProcessorDisplay;
	switch (gProcessorCode)
		{		
		case kDisplayProcessor:
			stringIndex = IDS_ProcessorDisplay;
			break;
			
		case kDisplayOverlayProcessor:
			stringIndex = IDS_Processor19;
			break;
			
		case kHistogramProcessor:
			stringIndex = IDS_ProcessorHistogram;
//			indexStart = gNumberOfEndOfLineCharacters;
			break;
			
		case kReformatProcessor:
		case kRefChangeHeaderProcessor:
		case kRefModifyChanDescriptionsProcessor:
		case kMultispecToThematicProcessor:
			stringIndex = IDS_ProcessorReformat;
			break;
			
		case kRefChangeFileFormatProcessor:
			stringIndex = IDS_Processor20;
			break;
			
		case kRectifyImageProcessor:
			stringIndex = IDS_Processor21;
			break;
			
		case kRefShapeToThematicFileProcessor:
			stringIndex = IDS_Processor22;
			break;
			
		case kRecodeThematicImageProcessor:
			stringIndex = IDS_Processor23;
			break;
			
		case kRefFieldsToThematicFileProcessor:
			stringIndex = IDS_Processor24;
			break;
			
		case kENVIROIToThematic:
			stringIndex = IDS_Processor25;
			break;
			
		case kRefMosaicImagesProcessor:
			stringIndex = IDS_Processor26;
			break;
						
		case kListDataProcessor:
			stringIndex = IDS_ProcessorListData;
			break;
			
		case kClusterProcessor:
			stringIndex = IDS_ProcessorCluster;
			listClassificationIdentifierFlag = TRUE;
//			indexStart = gNumberOfEndOfLineCharacters;
			break;
		
		case kComputeStatsProcessor:
			stringIndex = IDS_Processor18;
			break;
						
		case kListStatsProcessor:
			stringIndex = IDS_ProcessorListStats;
			break;
						
		case kHistogramStatsProcessor:
			stringIndex = IDS_ProcessorHistStats;
			break;
						
		case kStatEnhanceProcessor:
			stringIndex = IDS_ProcessorEnhance;
			break;
			
		case kFeatureExtractionProcessor:
			stringIndex = IDS_ProcessorExtract;
			break;
						
		case kSeparabilityProcessor:
			stringIndex = IDS_ProcessorFeature;
			break;
						
		case kClassifyProcessor:
			stringIndex = IDS_ProcessorClassify;
			listClassificationIdentifierFlag = TRUE;
//			indexStart = gNumberOfEndOfLineCharacters;
			break;
						
		case kListResultsProcessor:
			stringIndex = IDS_ProcessorResults;
			break;
						
		case kPrincipalComponentsProcessor:
			stringIndex = IDS_ProcessorPrinc;
			break;
			
		case kStatisticsImageProcessor:
			stringIndex = IDS_ProcessorCreateStat;
			break;
						
		case kBiPlotDataProcessor:
			stringIndex = IDS_ProcessorBiPlot;
			break;
						
		case kCovarianceCheckProcessor:
			stringIndex = IDS_ProcessorCheckCov;
			break;
				
		case kTransformCheckProcessor:
			stringIndex = IDS_ProcessorCheckTran;
			break;
			
		}	// end "switch (gProcessorCode)" 											
	
			// List the MultiSpec classification header identifier line.												
	
	if (listClassificationIdentifierFlag)
		{		
		MGetString (gTextString2, kFileIOStrID, IDS_MultiSpecType);  // 42 
		sprintf ((char*)gTextString, "%s%s", &gTextString2[1], gEndOfLine);
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												*outputCodePtr, 
												TRUE);
												
		}	// end "if (listClassificationIdentifierFlag)"
											
	MGetString ((UCharPtr)gTextString2, kProcessorStrID, stringIndex);
	
	if (gProcessorCode == kDisplayProcessor)
		{
				// Include the image file name
				
		char* fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (gImageFileInfoPtr);
		sprintf ((char*)&gTextString2[gTextString2[0]+1],
										(char*)" '%s'",
										fileNamePtr);
	
		}	// end "if (gProcessorCode == kDisplayProcessor)"
	
			// Add the date and time to the line.

	time_t currentTime = time (NULL);
	struct tm *currentDate = localtime (&currentTime);
	strftime ((char*)gTextString3, 254, "  %m-%d-%Y  %X", currentDate);
	sprintf (&textString[0],
					"%s%s (%s)%s",
					&gTextString2[1], 
					gTextString3,
					gApplicationIdentifierString,
					gEndOfLine);

	continueFlag = OutputString (resultsFileStreamPtr,
											(char*)&textString[indexStart], 
											0, 
											*outputCodePtr, 
											continueFlag,
											kUTF8CharString);
	
	if (gProcessorCode == kDisplayProcessor)
		{
				// Include the hdf data set name if needed.
		
		if (gImageFileInfoPtr->hdfHandle != NULL)
			{
			Str255						hdfDataSetName;
			
			UInt16						numChars;


			GetHdfDataSetName (gImageFileInfoPtr, 
										gImageFileInfoPtr->hdfDataSetSelection,
										(StringPtr)&hdfDataSetName,
										NULL);		
			
			numChars = hdfDataSetName[0];
			
			if (numChars > 0)
				{
				numChars = sprintf ((char*)gTextString, 
											"    %s", 
											"(hdf set: ");

				numChars = sprintf ((char*)&gTextString[numChars], 
											"%s)%s", 
											&hdfDataSetName[1],
											gEndOfLine);
					
				continueFlag = OutputString (	resultsFileStreamPtr, 
														(char*)gTextString, 
														0, 
														*outputCodePtr, 
														continueFlag);

				}	// end "if (numChars > 0)"
													
			}	// end "if (gImageFileInfoPtr == kHDF4Type || ..."
	
		}	// end "if (gProcessorCode == kDisplayProcessor)"
		
	return (continueFlag);

}	// end "ListProcessorTitleLine" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListProjectAndImageName
//
//	Software purpose:	The purpose of this routine is to list the project
//							name and the base image file name.
//
//	Parameters in:		The dialog pointer.
//							The dialog item number.
//							Address to the draw routine.
//
//	Parameters out:	Rectangle for the input item number.
//
// Value Returned:	None. 	
// 
// Called By:			ListHeaderInfo in multiSpecUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 05/20/1992
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean ListProjectAndImageName (
				CMFileStream* 						resultsFileStreamPtr, 
				SInt16*								outputCodePtr,
				SInt16	 							listCode,
				SInt16								covarianceStatsToUse, 
				Boolean								continueFlag)

{
	SInt16								strLength;
	

	if (continueFlag)
		{
		if ((listCode & kLProjectName) && gProjectInfoPtr != NULL)
			{
					// List the project name.													
	
			char* projectFileNamePtr = (char*)GetFileNameCPointerFromProjectInfo (gProjectInfoPtr);

			if (strlen (projectFileNamePtr) == 0)
				sprintf ((char*)gTextString2,
								(char*)"Untitled Project");
				
			else	// projectFileNamePtr[0] != 0
				sprintf ((char*)gTextString2,
									projectFileNamePtr);
			
			sprintf ((char*)gTextString, 
							(char*)"    Project = '%s'%s",
							gTextString2,
							gEndOfLine);
							
			continueFlag = OutputString (resultsFileStreamPtr,
													(char*)gTextString, 
													0, 
													*outputCodePtr,
													continueFlag,
													kUTF8CharString);
													
			}	// end "if ((listCode & kProjectName) && gProjectInfoPtr)" 
		
		if ((listCode & kLStatType) && gProjectInfoPtr != NULL)
			{
			SInt16 stringCode = 0;
			if (covarianceStatsToUse == kOriginalStats)
				stringCode = IDS_Project46;
			
			else if (covarianceStatsToUse == kLeaveOneOutStats)
				{
				if (gProjectInfoPtr->mixingParameterCode == kComputedOptimum)
					stringCode = IDS_Project47;
					
				else if (gProjectInfoPtr->mixingParameterCode == kUserSet)
					stringCode = IDS_Project73;
					
				else if (gProjectInfoPtr->mixingParameterCode == kIdentityMatrix)
					stringCode = IDS_Project74;
				
				}	// end "else if (covarianceStatsToUse == kLeaveOneOutStats)"
				
			else if (covarianceStatsToUse == kEnhancedStats)
				{
				if (gProjectInfoPtr->enhancedStatsOrigin == kOriginalStats)
					stringCode = IDS_Project48;
					
				else if (gProjectInfoPtr->enhancedStatsOrigin == kLeaveOneOutStats)
					stringCode = IDS_Project75;
					
				else	// ...->enhancedStatsOrigin == kMixedStats
					stringCode = IDS_Project76;
				
				}	// end "else if (covarianceStatsToUse == kEnhancedStats)"
				
			else if (covarianceStatsToUse == kMixedStats)
				stringCode = IDS_Project49;
			
			if (stringCode != 0)	
				continueFlag = ListSpecifiedStringNumber (
								kProjectStrID, 
								stringCode, 
								(unsigned char*)gTextString, 
								NULL, 
								*outputCodePtr,
								continueFlag);
								
			if (covarianceStatsToUse == kLeaveOneOutStats && 
												!gProjectInfoPtr->useCommonCovarianceInLOOCFlag)
				continueFlag = ListSpecifiedStringNumber (
								kProjectStrID, 
								IDS_Project58, 
								(unsigned char*)gTextString, 
								NULL, 
								*outputCodePtr,
								continueFlag);
			
			}	// end "if ((listCode & kLStatType) && gProjectInfoPtr != NULL)" 
			
		if ((listCode & kLProjectImageName) && gProjectInfoPtr)
			{
					// List the project image file name.									
		
			pstr ((char*)gTextString2, 
					(char*)gProjectInfoPtr->imageFileName,
					&strLength);
			sprintf ((char*)gTextString, 
						(char*)"    Base image file = '%s'%s",
						gTextString2,
						gEndOfLine);
													
			}	// end "if ((listCode & kLProjectImageName) && ..." 
			
		else	// !(listCode & kLProjectImageName) || !gProjectInfoPtr 
			{
					// List just the image file name.
					
			char* fileNamePtr =
						(char*)GetFileNameCPointerFromFileInfo (gImageFileInfoPtr);
			sprintf ((char*)gTextString,
						(char*)"    Image file = '%s'%s",
						fileNamePtr,
						gEndOfLine);
			
			}	// end "else !(listCode & kProjectName) || !gProjectInfoPtr" 
			
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												*outputCodePtr,
												continueFlag,
												kUTF8CharString);
			
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
 
}	// end "ListProjectAndImageName"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/28/1993
//	Revised By:			Larry L. Biehl			Date: 10/28/1993			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index, 
				UCharPtr								stringPtr, 
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				Boolean								continueFlag)
													
{
	Str255*								str255Ptr;
	
	
	if (continueFlag)
		{
		str255Ptr = (Str255*)stringPtr;
		
		continueFlag = MGetString (*str255Ptr, strListID, index);
				                                  
		if (continueFlag)
			continueFlag = OutputString (resultsFileStreamPtr, 
													(char*)&stringPtr[1], 
													(SInt32)stringPtr[0], 
													outputCode, 
													TRUE);
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input double value.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1994
//	Revised By:			Larry L. Biehl			Date: 01/26/1994			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				double								dValue, 
				Boolean								continueFlag)
													
{
	char*									stringPtr2;
	
	
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
			
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					dValue);
							
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE);
												
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input long value.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1994
//	Revised By:			Larry L. Biehl			Date: 01/26/1994			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				SInt32								lValue, 
				Boolean								continueFlag)
													
{
	char*									stringPtr2;
	
	
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
		
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					lValue);
							
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE);
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with two input long values.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/21/1994
//	Revised By:			Larry L. Biehl			Date: 10/21/1994			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				SInt32								lValue, 
				SInt32								lValue2, 
				Boolean								continueFlag)
													
{
	CharPtr								stringPtr2;
	
	
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
		
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					lValue,
					lValue2);
							
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE);
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with three input long values.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/01/1996
//	Revised By:			Larry L. Biehl			Date: 08/01/1996			

Boolean ListSpecifiedStringNumber (
				SInt16							strListID, 
				SInt16							index,
				CMFileStream*					resultsFileStreamPtr, 
				SInt16							outputCode, 
				SInt32							lValue, 
				SInt32							lValue2, 
				SInt32							lValue3, 
				Boolean							continueFlag)
													
{
	 CharPtr								stringPtr2;
	
	
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
		
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					lValue,
					lValue2,
					lValue3);
							
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE);
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input long value.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/20/2010
//	Revised By:			Larry L. Biehl			Date: 08/20/2010			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				SInt64								lValue, 
				Boolean								continueFlag)

{
	char*									stringPtr2;
	
	
	continueFlag = GetSpecifiedStringNumber (
						strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
		
		sprintf ((char*)gTextString,
				  &stringPtr2[1],
				  lValue);
		
		continueFlag = OutputString (resultsFileStreamPtr, 
											  (char*)gTextString, 
											  0, 
											  outputCode, 
											  TRUE);
		
		}	// end "if (continueFlag)" 
	
	return (continueFlag);
	
}	// end "ListSpecifiedStringNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with two input long values.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/23/2010
//	Revised By:			Larry L. Biehl			Date: 08/23/2010			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				SInt64								llValue, 
				SInt32								lValue2, 
				Boolean								continueFlag)

{
	CharPtr								stringPtr2;
	
	
	continueFlag = GetSpecifiedStringNumber (
									strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
		
		sprintf (	(char*)gTextString,
				  &stringPtr2[1],
				  llValue,
				  lValue2);
		
		continueFlag = OutputString (resultsFileStreamPtr, 
											  (char*)gTextString, 
											  0, 
											  outputCode, 
											  TRUE);
		
		}	// end "if (continueFlag)" 
	
	return (continueFlag);

}	// end "ListSpecifiedStringNumber"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input Pascal string.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to
//													the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/24/1994
//	Revised By:			Larry L. Biehl			Date: 06/26/1995			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				UCharPtr								inputStringPtr, 
				Boolean								continueFlag)
													
{
	SInt16								strLength;
	
			// Make certain the pascal string has a null at the end so that it 	
			// can be treated as a C string.													

	pstr ((char*)gTextString3, (char*)inputStringPtr, &strLength);
		
	continueFlag = ListSpecifiedStringNumber (
					strListID, 
					index,
					resultsFileStreamPtr, 
					outputCode, 
					(CharPtr)gTextString3, 
					continueFlag);
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input C string.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//							This is an overload function
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to
//													the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/05/2017
//	Revised By:			Larry L. Biehl			Date: 09/05/2017			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				CharPtr								inputStringPtr, 
				Boolean								continueFlag)
													
{
	CharPtr								stringPtr2;
	
	
			// Get the base string from the resource fork.
												
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
			
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					inputStringPtr);
							
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE,
												kASCIICharString);
												
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input C string.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to
//													the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/26/1995
//	Revised By:			Larry L. Biehl			Date: 03/13/2017			

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				CharPtr								inputStringPtr, 
				Boolean								continueFlag,
				SInt16								charFormatCode)
													
{
	CharPtr								stringPtr2;
	
	
			// Get the base string from the resource fork.
												
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
			
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					inputStringPtr);
							
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE,
												charFormatCode);
												
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input C string.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//							This is an overload function
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to
//													the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/05/1999
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				CharPtr								inputStringPtr, 
				CharPtr								inputString2Ptr, 
				Boolean								continueFlag)
													
{
	CharPtr								stringPtr2;
	
	
			// Get the base string from the resource fork.
												
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
			
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					inputStringPtr, 
					inputString2Ptr);
							
		continueFlag = OutputString (resultsFileStreamPtr,
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE,
												kASCIICharString);
												
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedStringNumber
//
//	Software purpose:	This routine lists the specified string number in the
//							resource file to the output text window and/or
//							disk file with the input C string.  Note that
//							gTextString and gTextString2 are used for temporary
//							storage.  Make certain that they are not being used
//							for other items.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	TRUE if string was listed and/or written to
//													the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/16/1999
//	Revised By:			Larry L. Biehl			Date: 03/16/2017

Boolean ListSpecifiedStringNumber (
				SInt16								strListID, 
				SInt16								index,
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode, 
				CharPtr								inputStringPtr, 
				CharPtr								inputString2Ptr, 
				Boolean								continueFlag,
				SInt16								charFormatCode)
													
{
	CharPtr								stringPtr2;
	
	
			// Get the base string from the resource fork.
												
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, gTextString2, continueFlag);
	
	if (continueFlag)
		{
		stringPtr2 = (char*)&gTextString2;
			
		sprintf (	(char*)gTextString,
					&stringPtr2[1],
					inputStringPtr, 
					inputString2Ptr);
							
		continueFlag = OutputString (resultsFileStreamPtr,
												(char*)gTextString, 
												0, 
												outputCode, 
												TRUE,
												charFormatCode);
												
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "ListSpecifiedStringNumber"



Boolean ListString (
				char*									textBuffer,
				unsigned int						textLength)
{	  
	return (ListString ((HPtr)textBuffer, 
								(UInt32)textLength, 
								gOutputTextH, 
								kASCIICharString));
					
}



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ListString
//
//	Software purpose:	The purpose of this routine is to call the OS specific
//							routine to list the string in the output test window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/05/1988
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

Boolean ListString (
				HPtr									stringPtr, 
				UInt32								stringLength,
				#if use_mlte_for_text_window 
					TXNObject							textH)
				#endif
				#if !use_mlte_for_text_window
					WEReference							textH)
				#endif

{   
	return (ListString ((HPtr)stringPtr, 
								(UInt32)stringLength, 
								textH, 
								kASCIICharString));

}	// end "ListString"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ListString
//
//	Software purpose:	The purpose of this routine is to call the OS specific
//							routine to list the string in the output test window.
//
//							Only verify available memory if the request is for more than
//							10 MB; will assume 10 MB is relatively easily available. This
//							is to save on calls to determine available memory.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/30/1988
//	Revised By:			Larry L. Biehl			Date: 03/22/2019

Boolean ListString (
				HPtr									stringPtr, 
				UInt32								stringLength,
				#if use_mlte_for_text_window 
					TXNObject							textH,
				#endif
				#if !use_mlte_for_text_window
					WEReference							textH,
				#endif
				SInt16								charFormatCode)

{   
	SInt64								lContBlock;
								
	Boolean								memoryOKFlag = TRUE,
											returnFlag = FALSE;
	
	
	if (2*stringLength > 10000000)
		{
		MGetFreeMemory (&lContBlock);
		
		if (lContBlock < 2*stringLength)
			memoryOKFlag = FALSE;
		
		}	// end "if (2*stringLength > 10000000)"
	
			// Continue if memory ok.															
			
	if (memoryOKFlag)
		{		
		#if defined multispec_mac
					// Local Declarations 	
			
			SInt16		savedMemoryError;
			
			if (gOutputWindow == NULL)
																						return (returnFlag);
																						
					// Save the memory error.  We want to be sure to catch the any memory error caused by
					// listing the message.  The message may already be about a memory allocation error.
					
			savedMemoryError = gMemoryError;
			gMemoryError = noErr;
			
					// Add input string to text buffer.											
					// Also -																			
					// Update maximum string length if needed.  This assumes that the	
					// string has a carriage return at the end.  Even if wrong this	
					// shouldn't cause too much problem.										
		
			#if !use_mlte_for_text_window
				if (stringLength <= gMaxCharsAllowedInLine)
					{
			#endif	// !use_mlte_for_text_window
					#if use_mlte_for_text_window
						UniChar				tempBuffer[512];
						char*					txnSetDataStringPtr;
						CFStringRef			cfStringRef = NULL;
						OSStatus				returnCode;
					
						txnSetDataStringPtr = stringPtr;
						TXNDataType			stringDataType = kTXNTextData;
						if (charFormatCode == kUnicodeCharString || charFormatCode == kUTF8CharString)
							{
							CFIndex				usedBufferLength = 0;
							
							
							stringDataType = kTXNUnicodeTextData;
							
							if (charFormatCode == kUTF8CharString)
								{
								CFIndex	numerCharacters = 0;
								
										// Now get the UTF8 formatted file name.
										
								cfStringRef = CFStringCreateWithBytes (
																	kCFAllocatorDefault,
																	(UInt8*)stringPtr,
																	stringLength,
																	kCFStringEncodingUTF8,
																	false);
																	
								if (cfStringRef != NULL)
									{
//									CFIndex CFStringGetBytes (CFStringRef theString, CFRange range, CFStringEncoding encoding, UInt8 lossByte, Boolean isExternalRepresentation, UInt8 *buffer, CFIndex maxBufLen, CFIndex *usedBufLen);
									CFStringGetBytes (cfStringRef, 
																CFRangeMake (0, stringLength),
																kCFStringEncodingUnicode, 
																'?', 
																false, 
																(UInt8*)tempBuffer, 
																512, 
																&usedBufferLength);
									
											// Get the number of characters in the unicode string.
											
									numerCharacters = CFStringGetLength (cfStringRef);
									/*
									CFStringGetCString (cfStringRef,
															(char*)&fileStreamPtr->fileName[1],
															(CFIndex)255,
															kCFStringEncodingUTF8);
									fileStreamPtr->fileName[0] = strlen ((char*)&fileStreamPtr->fileName[1]);
									*/
									}	// end "if (cfStringRef != NULL)"
									
								txnSetDataStringPtr = (char*)tempBuffer;
								stringLength = 2 * numerCharacters;
								
								}	// end "if (charFormatCode == kUTF8CharString)"
							/*
							else	// charFormatCode == kUnicodeCharString
								{
										// Need to convert wchar_t (4 bytes) to unicode char (2 bytes). For now
										// will just leave out bytes 3 & 4 of all of the input wchar_t characters
										// to see how it works.
								
								char* tempCharPtr = (char*)stringPtr;
								UInt32	characterCount;
								UInt32	fromIndex = 4;
								UInt32	toIndex = 2;
								for (characterCount = 1; characterCount<stringLength; characterCount++)
									{
									tempCharPtr[toIndex] = tempCharPtr[fromIndex];
									toIndex++;
									fromIndex++;
									tempCharPtr[toIndex] = tempCharPtr[fromIndex];
									toIndex++;
									fromIndex += 3;
									
									}	// end "for (characterCount = 1; characterCount<stringLength; characterCount++)"
									
								stringLength *= 2;
								
								}	// end "else charFormatCode == kUnicodeCharString"
							*/
							}	// end "if (charFormatCode == kUnicodeCharString || ...; else"
					
						returnCode = TXNSetData (textH,
															stringDataType,
															txnSetDataStringPtr,
															stringLength,
															kTXNEndOffset,
															kTXNEndOffset);
							
						if (cfStringRef != NULL)
							//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
							CFRelease (cfStringRef);
					#endif	// use_mlte_for_text_window
				
					#if !use_mlte_for_text_window
						//redisplayFlag = WSInsert (textH, stringPtr, stringLength);
						WEInsert (stringPtr, stringLength, NULL, NULL, textH);
						/*
						WEPut (0,
									0,
									stringPtr, 
									stringLength, 
									kTextEncodingMacRoman,
									0,
									0,
									NULL,
									NULL,
									textH);
						*/
					#endif	// !use_mlte_for_text_window
					
				gMaxCharsInLine = MAX (gMaxCharsInLine, stringLength);
				
			#if !use_mlte_for_text_window
					}	// end "if (stringLength <= gMaxCharsAllowedInLine)"
			
				else	// stringLength > gMaxCharsAllowedInLine
					{
					SInt32					lTextLength;
				
					lTextLength = gMaxCharsAllowedInLine;
					while (stringLength > 0)
						{
						//redisplayFlag = WSInsert (textH, stringPtr, lTextLength);
						WEInsert (stringPtr, lTextLength, NULL, NULL, textH);
						/*
						WEPut (0,
									0, 
									stringPtr, 
									lTextLength, 
									kTextEncodingMacRoman,
									0,
									0,
									NULL,
									NULL,
									textH);
						*/
						stringPtr += lTextLength;
						stringLength -= lTextLength;
						lTextLength = MIN (lTextLength, stringLength);
					
						if (lTextLength > 0)
							{
									// Add a carriage return in the text since the length
									// of the line exceeds the maximum length allowed for a	
									// line of text.  This assumes that there are no other	
									// carriage returns within this line of text which is		
									// the case for any long text strings in MultiSpec.		
								
							//redisplayFlag = WSInsert (textH, gEndOfLine, 1);
							WEInsert (gEndOfLine, 1, NULL, NULL, textH);
							/*
							WEPut (0,
										0, 
										gEndOfLine, 
										1, 
										kTextEncodingMacRoman,
										0,
										0,
										NULL,
										NULL,
										textH);
							*/
							}	// end "if (lTextLength > 0)"
					
						}	// end "while (stringLength > 0)"
					
					gMaxCharsInLine = gMaxCharsAllowedInLine;
				
					}	// end "else stringLength > gMaxCharsAllowedInLine"
			#endif	// !use_mlte_for_text_window
			
			returnFlag = (gMemoryError == noErr); 
			gMemoryError = savedMemoryError;
		
					// Indicate that the window has changed.
					
			SetOutputWindowChangedFlag (TRUE);
		#endif	// defined multispec_mac
	
		#if defined multispec_win || defined multispec_lin
			if (gOutputViewCPtr == NULL)
																				return (returnFlag);
																								
			returnFlag = gOutputViewCPtr->ListString (stringPtr, stringLength, charFormatCode);  
		#endif	// defined multispec_win
		
		}	// end "if (memoryOKFlag)"
		
	else	// !memoryOKFlag
		{	
		DisplayAlert (kTextWindowLowMemoryAlertID, 
							kCautionAlert,
							kAlertStrID, 
							IDS_Alert58,
							0, 
							NULL);
		
		returnFlag = FALSE;
		
		}	// end "else !memoryOKFlag"
		
	return (returnFlag);
		
}	// end "ListString" 
 

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListZoneMapProjectionString
//
//	Software purpose:	The purpose of this routine is to list the name of the map
//							projection being used for the UTM or State Plane map projection
//							system to the output text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/01/2007
//	Revised By:			Larry L. Biehl			Date: 03/14/2012	

Boolean ListZoneMapProjectionString (
				SInt16								projectionCode,
				char*									spaceStringPtr,
				Boolean 								continueFlag)

{	
	if (projectionCode >= -1)
		{
				// Allow for MultiSpec Geographic code which is -1;
				
		if (projectionCode > 0)
			projectionCode++;
			
		else if (projectionCode < 0)
			projectionCode = 1;
					
		if (projectionCode <= 0)
			{	
			sprintf ((char*)gTextString,
							"%sNone specified%s",
							spaceStringPtr,
							gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString (
					(char*)gTextString,  strlen ((char*)gTextString), gOutputTextH);
			
			}	// end "if (projectionCode <= 0)"
			
		else	// projectionCode > 0
			{
			#if defined multispec_mac 
				GetMenuItemText (gPopUpProjectionMenu, 
										projectionCode+1, 
										gTextString2);
				gTextString2[gTextString2[0]+1] = 0;
			#endif	// defined multispec_mac   
                             
			#if defined multispec_win
				MGetString (gTextString2,
						0,
						IDS_ProjectionType01+projectionCode);
			#endif	// defined multispec_win
								
			sprintf ((char*)gTextString,
							"%sZone Projection:   %s%s",
							spaceStringPtr,
							&gTextString2[1],
							gEndOfLine);
					
			if (continueFlag)	
				continueFlag = ListString (
					(char*)gTextString,  strlen ((char*)gTextString), gOutputTextH);
					
			}	// end "else projectionCode > 0"
					
		}	// end "if (projectionCode >= -1)"
		
	return (continueFlag);
											
}	// end "ListZoneMapProjectionString" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadRealValueString
//
//	Software purpose:	This routine loads the input string with the real value with the
//							specified field size and decimals in either f or E format
//							depending on the input data value.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	Length of the string
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/09/2006
//	Revised By:			Larry L. Biehl			Date: 03/12/2012	

SInt16 LoadRealValueString (
				char*									stringPtr,
				double								dataValue,
				SInt16								dataValueFieldSize,
				SInt16								numberFDecimalDigits,
				SInt16								numberEDecimalDigits,
				char*									startStringPtr,
				char*									endStringPtr)
													
{
	double								absValue;
	
	SInt16								numberChars;
	
	
	absValue = fabs (dataValue);
	if (absValue == 0. ||
				(absValue >= (double)kMinValueToListWith_f && absValue <= (double)kMaxValueToListWith_f))
		{
		if (numberFDecimalDigits >= 0)
			numberChars = sprintf (stringPtr, 
											"%s%*.*f%s", 
											startStringPtr,
											dataValueFieldSize,
											numberFDecimalDigits,
											dataValue,
											endStringPtr);
											
		else
			numberChars = sprintf (stringPtr, 
											"%s%g%s", 
											startStringPtr,
											dataValue,
											endStringPtr);
											
		}	// end "if (absValue == 0. || ..."

						
	else	// absValue != 0 && (absValue < kMinValueToListWith_f || ...
		numberChars = sprintf (stringPtr, 
										"%s%*.*E%s", 
										startStringPtr,
										dataValueFieldSize,
										numberEDecimalDigits,
										dataValue,
										endStringPtr);
										
	return (numberChars);
		
}	// end "LoadRealValueString" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSpecifiedStringNumberDouble
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file to the input string with the input double values.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2007
//	Revised By:			Larry L. Biehl			Date: 04/25/2007	

Boolean LoadSpecifiedStringNumberDouble (
				SInt16								strListID, 
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				double								doubleValue1,
				double								doubleValue2,
				double								doubleValue3,
				double								doubleValue4,
				double								doubleValue5)
													
{
	continueFlag = GetSpecifiedStringNumber (
									strListID, index, (UCharPtr)stringPtr2, continueFlag);
	
	if (continueFlag)
		{
		sprintf ((char*)stringPtr1,
						&stringPtr2[1],
						doubleValue1,
						doubleValue2,
						doubleValue3,
						doubleValue4,
						doubleValue5);
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "LoadSpecifiedStringNumberDouble"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSpecifiedStringNumberLong
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file to the input string the input long value.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1995
//	Revised By:			Larry L. Biehl			Date: 02/10/1995	

Boolean LoadSpecifiedStringNumberLong (
				SInt16								strListID, 
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				SInt32								lValue1,
				SInt32								lValue2)
													
{
	continueFlag = GetSpecifiedStringNumber (
									strListID, index, (UCharPtr)stringPtr2, continueFlag);
	
	if (continueFlag)
		{
		sprintf (	(char*)stringPtr1,
					&stringPtr2[1],
					lValue1,
					lValue2);
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "LoadSpecifiedStringNumberLong"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSpecifiedStringNumberLongP
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file to the specified output pascal string 
//							with the input long value. 
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1995
//	Revised By:			Larry L. Biehl			Date: 02/10/1995	

Boolean LoadSpecifiedStringNumberLongP (
				SInt16								strListID, 
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				SInt32								lValue1,
				SInt32								lValue2)
													
{
	UInt16								stringLength;


	continueFlag = LoadSpecifiedStringNumberLong (
								strListID, 
								index, 
								&stringPtr1[1],
								stringPtr2,
								continueFlag,
								lValue1,
								lValue2);
	
	if (continueFlag)
		{
		stringLength = (UInt16)strlen (&stringPtr1[1]);
		stringPtr1[0] = (UInt8)stringLength;
					
		}	// end "if (continueFlag)" 
		
	return (continueFlag);

}	// end "LoadSpecifiedStringNumberLongP"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSpecifiedStringNumberString
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file to the output string with the input string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was loaded.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/1995
//	Revised By:			Larry L. Biehl			Date: 07/20/1999	

Boolean LoadSpecifiedStringNumberString (
				SInt16								strListID, 
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				char*									inputStringPtr)
													
{
	continueFlag = GetSpecifiedStringNumber (
									strListID, index, (UCharPtr)stringPtr2, continueFlag);
	
	if (continueFlag)
		{
		sprintf (	(char*)stringPtr1,
					&stringPtr2[1],
					inputStringPtr);
					
		}	// end "if (continueFlag)"
		
	else	// !continueFlag
		stringPtr2[1] = 0;
		
	return (continueFlag);

}	// end "LoadSpecifiedStringNumberString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSpecifiedStringNumberString
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file to the output string with the input string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was loaded.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2017
//	Revised By:			Larry L. Biehl			Date: 02/23/2017	

Boolean LoadSpecifiedStringNumberString (
				SInt16								strListID, 
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				FileStringPtr						inputStringPtr)
													
{
	continueFlag = GetSpecifiedStringNumber (
									strListID, index, (UCharPtr)stringPtr2, continueFlag);
	
	if (continueFlag)
		{
		sprintf ((char*)stringPtr1,
					&stringPtr2[1],
					inputStringPtr);
					
		}	// end "if (continueFlag)"
		
	else	// !continueFlag
		stringPtr2[1] = 0;
		
	return (continueFlag);

}	// end "LoadSpecifiedStringNumberString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSpecifiedStringNumberStringP
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file to the specified output pascal string. 
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1995
//	Revised By:			Larry L. Biehl			Date: 07/20/1999	

Boolean LoadSpecifiedStringNumberStringP (
				SInt16								strListID, 
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				char*									inputStringPtr)
													
{
	UInt16								stringLength;


	continueFlag = LoadSpecifiedStringNumberString (
								strListID, 
								index, 
								&stringPtr1[1],
								stringPtr2,
								continueFlag,
								inputStringPtr);
	
	if (continueFlag)
		{
		stringLength = (UInt16)strlen (&stringPtr1[1]);
		stringPtr1[0] = (UInt8)stringLength;
					
		}	// end "if (continueFlag)"
		
	else	// !continueFlag
		stringPtr1[0] = 0;
		
	return (continueFlag);

}	// end "LoadSpecifiedStringNumberStringP"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSpecifiedStringNumberStringP
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file to the specified output pascal string. 
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was listed and/or written to the output file.
//							FALSE if not.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2017
//	Revised By:			Larry L. Biehl			Date: 02/23/2017	

Boolean LoadSpecifiedStringNumberStringP (
				SInt16								strListID, 
				SInt16								index, 
				char*									stringPtr1,
				char*									stringPtr2,
				Boolean								continueFlag,
				FileStringPtr						inputStringPtr)
													
{
	UInt16								stringLength;


	continueFlag = LoadSpecifiedStringNumberString (
								strListID, 
								index, 
								&stringPtr1[1],
								stringPtr2,
								continueFlag,
								inputStringPtr);
	
	if (continueFlag)
		{
		stringLength = (UInt16)strlen (&stringPtr1[1]);
		stringPtr1[0] = (UInt8)stringLength;
					
		}	// end "if (continueFlag)"
		
	else	// !continueFlag
		stringPtr1[0] = 0;
		
	return (continueFlag);

}	// end "LoadSpecifiedStringNumberStringP"

   
/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void* MemoryCopy
//
//	Software purpose:	This routine is a helper function to handle the conversion of
//							code from character base to unicode base. The routine will use
//							memcpy when character string is being used and wmemcpy when 
//							unicode strings are being used.
//
//	Parameters in:		Pointer to title string.
//
//	Parameters out:	None.
//
// Value Returned:	None.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2017
//	Revised By:			Larry L. Biehl			Date: 03/08/2017

void* MemoryCopy (
				void* 								str1,
				void* 								str2,
				int									numberCharacters,
				Boolean								wideCharacterStringFlag)
	
{		
	if (wideCharacterStringFlag)
		return (void*)wmemcpy ((wchar_t*)str1, (wchar_t*)str2, numberCharacters);
		
	else // !wideCharacterStringFlag
		return (void*)memcpy ((char*)str1, (char*)str2, numberCharacters);
	
}	// end "MemoryCopy"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MGetString
//
//	Software purpose:	The purpose of this routine is to obtain the requested
//							string from the resource file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/14/1995
//	Revised By:			Larry L. Biehl			Date: 03/15/2019
//
// For Linux: The String Table must be defined in LStringTable.def
// Format for linux string table is given in LStringTable.def
//
Boolean MGetString (
				UCharPtr								outTextPtr, 
				UInt16								stringListID, 
				UInt16								stringID,
				UInt16								maxStringLength)

{		       
	outTextPtr[0] = 0;
	outTextPtr[1] = 0;
	
	#if defined multispec_mac
		UCharPtr							tempStringPtr;
		
		
		tempStringPtr = outTextPtr;
		//if (returnAsWideCharStringFlag)
		//	tempStringPtr = (UCharPtr)tempString;
		
		::GetIndString (tempStringPtr, (SInt16)stringListID, (SInt16)stringID);
		//CFBundleCopyLocalizedString (CFBundleGetBundleWithIdentifier (CFSTR("com.my.bundle.id")), CFSTR("my_key"), CFSTR("my_key"), NULL);
		//CFStringRef string = ::CFBundleCopyLocalizedString (gApplicationServicesBundle, (SInt16)stringListID, (SInt16)stringID);
		
				// Force a c-string terminator. Do not allow strings over 254
				// characters in length.
				
		SInt16 stringLength = tempStringPtr[0];
		stringLength = MIN (stringLength, 254);		
		outTextPtr[stringLength+1] = kNullTerminator;
		/*
		if (returnAsWideCharStringFlag)
			{
			wchar_t*		wideOutputTextPtr = (wchar_t*)outTextPtr;
			mbstowcs (&wideOutputTextPtr[1], (const char*)&tempStringPtr[1], stringLength);
			wideOutputTextPtr[0] = stringLength;
			
			//CFStringRef temp = CFStringCreateWithPascalString (NULL, 
			//																	outTextPtr, 
			//																	kCFStringEncodingMacRoman); // kCFStringEncodingUTF32
			
			//wideOutputTextPtr[0] = CFStringGetLength (temp);
			//CFStringGetCharacters (temp, 
			//								CFRangeMake (0, wideOutputTextPtr[0]), 
			//								(UniChar*)&wideOutputTextPtr[1]);
			//CFStringGetCString (temp,  
			//								&wideOutputTextPtr[1],
			//								wideOutputTextPtr[0],
			//								kCFStringEncodingUTF32);
			//CFRelease (temp);
			
			}	// end "if (returnAsWideCharStringFlag)"
		*/
		return (stringLength != 0);
	#endif	// defined multispec_mac

	#if defined multispec_mac_swift
		return (FALSE);
	#endif	//  || defined multispec_mac_swift

		
	#if defined multispec_win
		USES_CONVERSION;

		TBYTE			string[512];
		SInt16 		numberCharacters;


		numberCharacters = ::LoadString (AfxGetInstanceHandle (), 
														stringID, 
														(LPWSTR)string,
														510);
		strcpy ((char*)&outTextPtr[1], T2A(string));
	
				// Note that the incorrect count of characters could cause a problem. The only current case
				// with the number of characters are more than 255 is one of the filter strings used for
				// GetFile and PutFile. In that case the number of characters is not used ... only the c terminator.
	
		outTextPtr[0] = (Byte)MIN (numberCharacters, 255);
													
		return (numberCharacters > 0);
	#endif	// defined multispec_win
   
	#if defined multispec_lin
		#ifdef NetBeansProject
			wxTextFile file (wxT("LStringTable.def"));
		#else
					// First get the path to this executable file
	
			wxStandardPaths std = wxStandardPaths::Get ();
			wxString exePath = std.GetExecutablePath ();
			#ifdef multispec_wxmac
				wxString exeDir = exePath.BeforeLast (wxUniChar('M'));
				exeDir = exeDir.BeforeLast (wxUniChar('M'));
				exeDir.Append ("Resources/LStringTable.def");
			#else
						// This will be for MultiSpec on mygeohub
				wxString exeDir = exePath.BeforeLast ('/');
				exeDir.Append ("/LStringTable.def");
			#endif
	
			wxTextFile file (exeDir);
		#endif
		if (!file.Exists ())

																									return false;
		
		if (!file.Open ())
																									return false;
   
		bool found = false;
		wxString str, strout, strend;
		wxString index (wxT("#"));
		index << stringID; // Append string id
		for (str = file.GetFirstLine (); !file.Eof (); str = file.GetNextLine ()) 
			{
			if (str.Contains (index)) 
				{
						// Now extract string
				if (str.Contains (wxT("\""))) 
					{
							// Verify that this is the last character in the string. If
							// not, then the '"' is part of the string not the designator
							// for the end of the string. Will need to go to the next line
							// to get the end.
					
					str = str.AfterFirst ('"');
					strend = str.AfterLast ('"');
					if (str.Contains (wxT("\"")) && strend.IsEmpty ())
						{
						strout = str.BeforeLast ('"');
						
						strncpy ((char*)&outTextPtr[1],
									(const char*)strout.mb_str (wxConvUTF8),
									maxStringLength);
						outTextPtr[0] = (unsigned char) strout.Len ();
						found = true;
						
						}	// end "if (str.Contains (wxT("\"")))"
						 
					else // read next line for search of second"
						{
						strout = str;
						while (!file.Eof () && !found) 
							{
							strout << '\n';
							str = file.GetNextLine ();
							//str = str.Trim (false);
							if (str.Contains (wxT("\""))) 
								{
								found = true;
								strout << str.BeforeLast ('"');
								strncpy ((char*)&outTextPtr[1],
											(const char*) strout.mb_str (wxConvUTF8),
											maxStringLength);
								outTextPtr[0] = (unsigned char)strout.Len ();
								
								}	// end "if (str.Contains (wxT("\"")))"
							else
								strout << str;
							
							}	// end "while (!file.Eof () && !found)"
							
						}	// end "else read next line for search of second"
						
					}	// end "if (str.Contains (wxT("\"")))"
				
				}	// end "if (str.Contains (index))"
				
			if (found)
				break;
			
			}	// end "for (str = file.GetFirstLine ();..."
			
		file.Close ();
		return (outTextPtr[1] != 0 && found);
	#endif // defined multispec_lin

}	// end "MGetString"  


#if defined multispec_win 
#if defined _UNICODE
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MGetString
//
//	Software purpose:	The purpose of this routine is to obtain the requested
//							string from the resource file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/14/1995
//	Revised By:			Larry L. Biehl			Date: 08/28/2017

Boolean MGetString (
				TBYTE*								outTextPtr, 
				UInt16								stringListID, 
				UInt16								stringID,
				UInt16								maxStringLength)

{		        
   SInt16 		numberCharacters;
    
   numberCharacters = ::LoadString (AfxGetInstanceHandle (), 
   											stringID, 
   											&outTextPtr[1],
												maxStringLength);
   
   outTextPtr[0] = (Byte)numberCharacters;
   											
   return (numberCharacters > 0);
   
}	// end "MGetString"  
#endif	// defined _UNICODE
#endif	// defined multispec_win



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MSetWindowTitle
//
//	Software purpose:	The purpose of this routine is to set the specified
//							window title.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/10/1995
//	Revised By:			Larry L. Biehl			Date: 06/19/2017
                   
void MSetWindowTitle (
				WindowPtr							windowPtr,
				UCharPtr								titleStringPtr)

{   
	if (windowPtr != NULL)
		{
		if (titleStringPtr[0] == 0)
			titleStringPtr[0] = (UInt8)strlen ((char*)&titleStringPtr[1]);
			
		#if defined multispec_mac      
			CFStringRef				cfStringRef;
			//SetWTitle (windowPtr, titleStringPtr); 	
			cfStringRef = CFStringCreateWithBytes (kCFAllocatorDefault,
																	(UInt8*)&titleStringPtr[1],
																	titleStringPtr[0],
																	kCFStringEncodingUTF8,
																	false);
																	
			if (cfStringRef != NULL)
				{
				SetWindowTitleWithCFString (windowPtr, cfStringRef);
						// Using CFallocatorDeallocate causes system to crash later.
						// Not sure when one should use it and not use it.
				//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);	
				CFRelease (cfStringRef);
				
				}	// end "if (cfStringRef != NULL)"	
		#endif	// defined multispec_mac  
		
		#if defined multispec_win 	             
			CDocument* documentCPtr = windowPtr->GetDocument ();
			MSetWindowTitle (documentCPtr, titleStringPtr);
		#endif	// defined multispec_win  
		
		#if defined multispec_lin 	             
			//wxString ntitle =  wxString::FromUTF8 ((char*)&titleStringPtr[1]);
			//(windowPtr->m_frame)->SetTitle (ntitle);          
         wxDocument* documentCPtr = windowPtr->GetDocument ();
//         MSetWindowTitle (documentCPtr, titleStringPtr);
         documentCPtr->SetTitle (titleStringPtr);
		#endif	// defined multispec_lin 
		
		}	// end "if (windowPtr != NULL)"

}	// end "MSetWindowTitle" 


     
#if defined multispec_win               
void MSetWindowTitle (
				StatisticsWindowPtr				windowPtr,
				UCharPtr								titleStringPtr)

{              
	if (titleStringPtr[0] == 0)
		titleStringPtr[0] = (UInt8)strlen ((char*)&titleStringPtr[1]); 
	
	             
	if (windowPtr != NULL)
		{
		CDocument* documentCPtr = windowPtr->GetDocument ();
		MSetWindowTitle (documentCPtr, titleStringPtr);
		
		}	// end "if (windowPtr != NULL)"
		
}	// end "MSetWindowTitle" 
#endif	// defined multispec_win 

#if defined multispec_win
void MSetWindowTitle (
				CDocument*		documentCPtr,
				UCharPtr			titleStringPtr)

{  
	//TBYTE						tempWideCharacterString[1000];
	TBYTE*						tempUnicodeCharacterStringPtr;


	if (documentCPtr != NULL)
		{
   		// Make sure there is a C string terminator at the end.
                                                  		
		titleStringPtr[titleStringPtr[0]+1] = 0;
			
				// Convert from UTF8 string to wide character string
		
		tempUnicodeCharacterStringPtr = 
							ConvertMultibyteStringToUnicodeString (&titleStringPtr[1]);

		documentCPtr->SetTitle (&tempUnicodeCharacterStringPtr[1]);
		
		}	// end "if (documentCPtr != NULL)"
		
}	// end "MSetWindowTitle"
#endif	// defined multispec_win  

#if defined multispec_lin
void MSetWindowTitle (
				wxDocument*		documentCPtr,
				UCharPtr			titleStringPtr)

{
   
	if (documentCPtr != NULL)
		{
   		// Make sure there is a C string terminator at the end.
                                                  		
		titleStringPtr[titleStringPtr[0]+1] = 0;
      wxString ntitle =  wxString::FromUTF8 ((char*)&titleStringPtr[1]);
		//documentCPtr->SetTitle ((CharPtr)&titleStringPtr[1]);
      documentCPtr->SetTitle (ntitle);
		
		}	// end "if (documentCPtr != NULL)"
		
}	// end "MSetWindowTitle"
#endif


//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void NumToString
//
//	Software purpose:	This routine converts the input 32-bit unsigned integer
//							to a string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/26/2010
//	Revised By:			Larry L. Biehl			Date: 08/27/2010	
							               	                
void	NumToString (
				UInt32					numberValue, 
				UCharPtr					stringPtr) 
				
{                            		
	sprintf ((CharPtr)&stringPtr[1], 
					"%lu",
					numberValue);
					
	stringPtr[0] = (UInt8)strlen ((CharPtr)&stringPtr[1]);

}	// end "NumToString" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void NumToString
//
//	Software purpose:	This routine converts the input 64-bit signed integer
//							to a string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/26/2010
//	Revised By:			Larry L. Biehl			Date: 08/26/2010	
							               	                
void	NumToString (
				SInt64					numberValue, 
				UCharPtr					stringPtr) 
				
{                            		
	sprintf ((CharPtr)&stringPtr[1], 
					"%lld",
					numberValue);
					
	stringPtr[0] = (UInt8)strlen ((CharPtr)&stringPtr[1]);

}	// end "NumToString"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean OutputString
//
//	Software purpose:	The purpose of this routine is to put the
//							input string into the output window and/or the
//							results disk file as requested.
//							This is an overload routine for OutputString
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/05/2017
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

Boolean OutputString (
				CMFileStream*						resultsFileStreamPtr, 
				HPtr	 								stringPtr, 
				UInt32								stringLength, 
				SInt16								outputCode, 
				Boolean								continueFlag)

{
	if (continueFlag)
		return (OutputString2 (resultsFileStreamPtr, 
										stringPtr, 
										stringLength, 
										outputCode, 
										continueFlag,
										-1,
										kASCIICharString));
										
	return (FALSE);		
	
}	// end "OutputString"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean OutputString
//
//	Software purpose:	The purpose of this routine is to put the
//							input string into the output window and/or the
//							results disk file as requested.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/01/1989
//	Revised By:			Larry L. Biehl			Date: 03/13/2012

Boolean OutputString (
				CMFileStream*						resultsFileStreamPtr, 
				HPtr	 								stringPtr, 
				UInt32								stringLength, 
				SInt16								outputCode, 
				Boolean								continueFlag,
				SInt16								charFormatCode)

{
	if (continueFlag)
		return (OutputString2 (resultsFileStreamPtr, 
										stringPtr, 
										stringLength, 
										outputCode, 
										continueFlag,
										-1,
										charFormatCode));
										
	return (FALSE);		
	
}	// end "OutputString"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean OutputString2
//
//	Software purpose:	The purpose of this routine is to put the input string into the 
//							output window (up to the requested limit) and/or the results disk 
//							file as requested.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2012
//	Revised By:			Larry L. Biehl			Date: 03/13/2012

Boolean OutputString2 (
				CMFileStream*						resultsFileStreamPtr, 
				HPtr	 								stringPtr, 
				UInt32								stringLength,
				SInt16								outputCode, 
				Boolean								continueFlag,
				SInt32								stringLimit,
				SInt16								charFormatCode)

{
	SInt16								errCode;
	
	
	if (!continueFlag)
																					return (FALSE);
																					
			// Get the string length if needed.												
		
	if (stringLength == 0)
		stringLength = StringLength ((void*)stringPtr, (charFormatCode!=kUnicodeCharString));
		
	if (stringLength > 0)
		{
				// Put string into results disk file if requested.						
			
		if ((outputCode & kAsciiFormatCode) && resultsFileStreamPtr != NULL)
			{
			errCode = MWriteData (resultsFileStreamPtr, 
										&stringLength, 
										stringPtr, 
										kErrorMessages);
				
			if (errCode != noErr)
				{
				gOutputCode = (gOutputCode & 0xfffd);
				gOutputForce1Code = (gOutputForce1Code & 0xfffd);
				
				}	// end "if (errCode != noErr)" 
			
			}	// end "if ((outputCode & kAsciiFormatCode) && ..." 
			
				// Put string in output text window if requested but only put
				// in the requested limit.
			
		if (outputCode & kOutputWindowFormatCode)
			{
			if (stringLimit > 0 && stringLength > (UInt32)stringLimit)
				{
				sprintf (&stringPtr[stringLimit], "%s", gEndOfLine);
				stringLength = stringLimit + gNumberOfEndOfLineCharacters;
				
				}	/// end "if (stringLimit > 0 && stringLength > stringLimit)"
			
			if (!ListString (stringPtr, stringLength, gOutputTextH, charFormatCode))
				{
				gOutputCode = (gOutputCode & 0xfffe);
				gOutputForce1Code = gOutputCode;
				gOutputTextOKFlag = FALSE;
				
				}	// end "if (!continueFlag)" 
			
			}	// end "if (!ListString (..." 
			
		}	// end "if (stringLength > 0)" 
			
	return (gOutputForce1Code & 0x0003);

}	// end "OutputString2"  


		
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pstr
//
//	Software purpose:	This routine moves a pascal string to the 
//							specified memory location and returns the number
//							of characters that were moved.  A termination 
//							character is added at the end to make it a 'C'
//							string.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Ravi Budruk				Date:  ?/??/????
//	Revised By:			Larry L. Biehl			Date: 06/19/2001
	
void pstr (
				char* 								sPtr,
				char* 								tPtr, 
				SInt16*	 							lenPtr)

		
{
	SInt16								i;
	
	
	*lenPtr = (SInt16)((UInt8)(*tPtr));
	for (i=1; i<=*lenPtr; i++)
		{
		*sPtr = *(++tPtr);
		sPtr++;
		}
		
	*sPtr = 0;
		
}	// end "pstr"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CharPtr PtoCstring
//
//	Software purpose:	This routine copies the input pascal string to an output
//							C string. The input and output string pointers
//							can represent the same string.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/08/1997
//	Revised By:			Larry L. Biehl			Date: 01/08/1997	
							               	   
CharPtr 	PtoCstring (
				CharPtr								inPStringPtr, 
				CharPtr								outCStringPtr)
				
{
	if (inPStringPtr != NULL && outCStringPtr != NULL)
		{
		size_t bytesToMove = inPStringPtr[0];
		
		bytesToMove = MIN (bytesToMove, 254);

		if (bytesToMove > 0)
			memmove (outCStringPtr, &inPStringPtr[1], bytesToMove);
			
		outCStringPtr[bytesToMove] = 0;

		}	// end "if (inCStringPtr != NULL && ...)"

	return outCStringPtr;

}	// end "CtoPstring"


		
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RemoveCharsAddVersion
//
//	Software purpose:	This routine checks if the characters in the 'removeString'
//							plus number characters are at the end of 'string'.  The
//							'removeString' and 'numbers' are removed if they exist.
//							If number characters are at the end then '_' and the 
//							first character in 'remove string' are added plus the number
//							characters.  This allows for the number characters or version
//							to stay with the input string.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1992
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void RemoveCharsAddVersion (
				UCharPtr								removeStringPtr, 
				FileStringPtr						stringPtr)

{
	UInt8									tempString[2];
	FileStringPtr						tempPtr,
											temp2Ptr;
	
	SInt16								index,
											lengthString,
											numberCharacters;
	
	UInt16								lengthRemoveString;
	
	//char									asciiCharacter;
	
	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

	lengthRemoveString = *removeStringPtr;
	if (lengthRemoveString == 0) 
		{
				// This must be a C string.  Get the length.
				
		lengthRemoveString = (UInt16)strlen ((char*)&removeStringPtr[1]);
		lengthRemoveString = MIN (lengthRemoveString, 255);
		
		}	// end "if (lengthRemoveString == 0)"
		
	lengthString = *stringPtr;
	
			// Get number of numbers at the end of the input string.	
	
	numberCharacters = 0;
	tempString[1] = 0;
	for (index=lengthString; index>0; index--)
		{
		//tempString[0] = stringPtr[index];
		//asciiCharacter = (char)T2A((LPCWSTR)tempString);

		if (stringPtr[index] < 0x30 || stringPtr[index] > 0x39)
		//if (stringPtr[index] < 0x0030 || stringPtr[index] > 0x0039)
			break;
			
		numberCharacters++;
		
		}	// end "for (index=lengthString; index>0; index--)"
		
			// Now make stringPtr a pascal string without the version information.
			
	*stringPtr -= (UInt8)numberCharacters;
	 
	 		// Now determine if input suffix characters can be removed.
	 		
	if (CompareSuffixNoCase ((char*)removeStringPtr, stringPtr, &lengthRemoveString))
		{
		index = lengthString - lengthRemoveString - numberCharacters + 1;
		tempPtr = &stringPtr[index];
		
		*stringPtr -= (UInt8)lengthRemoveString;
			
		if (numberCharacters > 0)
			{
					// Replace the '.' with an underscore.							

			*tempPtr = '_';
			
					// Keep the First character after the period.				
					
			tempPtr += 2;
			
			temp2Ptr = &tempPtr[lengthRemoveString-2];
			
			BlockMoveData (removeStringPtr, tempPtr, numberCharacters);
			
			*stringPtr += (UInt8)(2 + numberCharacters);
			
			}	// end "if (numberCharacters > 0)"
			
		}	// end "if (CompareSuffixNoCase (..."
		
	else	// !CompareSuffixNoCase (...
		*stringPtr += (UInt8)numberCharacters; 

}	// end "RemoveCharsAddVersion"  

		
/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		RemoveCharsNoCase
//
//	Software purpose:	This routine removes the characters in
//							"removeString" from the end of "string" if they exist.
//							This routine was developed specifically to remove
//							.lan (ERDAS convention) from the end of filenames to
//							be ready to add .sta for image statistics file names
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1988
//	Revised By:			Larry L. Biehl			Date: 03/14/2017

void RemoveCharsNoCase (
				const CharPtr						removeStringPtr, 
				wchar_t*								stringPtr)

{  
	UInt16		numChars;
	
	#if defined multispec_win
		if (CompareSuffixNoCase (removeStringPtr, stringPtr, &numChars))
		//wchar_t					wideStringPtr[256];

		//numChars = mbstowcs (wideStringPtr, &removeStringPtr[1], 255);
		//int stringLength = wcslen (wideStringPtr);
		//if (_wcsicmp (wideStringPtr, &stringPtr[stringLength-numChars]) == 0)
			{
			*stringPtr -= numChars;
			
					// Also make sure this is a valid c-string
					
			stringPtr[stringPtr[0]+1] = 0;
			
			}	// end "if (CompareSuffixNoCase (removeStringPtr, ..."
	#else
		if (CompareSuffixNoCase (removeStringPtr, stringPtr, &numChars))
			{
			*stringPtr -= (UInt8)numChars;
			
					// Also make sure this is a valid c-string
					
			stringPtr[stringPtr[0]+1] = 0;
			
			}	// end "if (CompareSuffixNoCase (removeStringPtr, ..."
	#endif

}	// end "RemoveCharsNoCase"    
*/
		

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		RemoveCharsNoCase
//
//	Software purpose:	This routine removes the characters in
//							"removeString" from the end of "string" if they exist.
//							This routine was developed specifically to remove
//							.lan (ERDAS convention) from the end of filenames to
//							be ready to add .sta for image statistics file names
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/13/2017
//	Revised By:			Larry L. Biehl			Date: 03/13/2017

void RemoveCharsNoCase (
				const CharPtr						removeStringPtr, 
				UCharPtr								stringPtr)

{  
	UInt16		numChars;
	
	
	if (CompareSuffixNoCase (removeStringPtr, stringPtr, &numChars))
		{
		*stringPtr -= (UInt8)numChars;
		
				// Also make sure this is a valid c-string
				
		stringPtr[stringPtr[0]+1] = 0;
		
		}	// end "if (CompareSuffixNoCase (removeStringPtr, ..."

}	// end "RemoveCharsNoCase"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetActiveImageWindowTitle
//
//	Software purpose:	The purpose of this routine is to set the specified
//							title to the active image window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1995
//	Revised By:			Larry L. Biehl			Date: 03/13/2017 
 
void SetActiveImageWindowTitle (
				UCharPtr								titleStringPtr)

{           
	#if defined multispec_mac  	   
	   if (gActiveImageWindow != NULL)                                                
			MSetWindowTitle (gActiveImageWindow, titleStringPtr);		
	#endif	// defined multispec_mac 
		
	#if defined multispec_win	   
	   if (gActiveImageViewCPtr != NULL)
	   	{                      
   		titleStringPtr[titleStringPtr[0]+1] = 0;
			CMImageDoc* documentCPtr = gActiveImageViewCPtr->GetDocument ();                   
			documentCPtr->SetTitle ((LPCTSTR)&titleStringPtr[1]);
			
			}	// end " if (gActiveImageViewCPtr != NULL)" 		
	#endif	// defined multispec_win  

	#if defined multispec_lin
		if (gActiveImageViewCPtr != NULL) 
			{
			titleStringPtr[titleStringPtr[0] + 1] = 0;
			CMImageFrame* frameCPtr = (CMImageFrame *)(gActiveImageViewCPtr->GetFrame ());
			wxString frametitle (& titleStringPtr[1], wxConvUTF8);
			frameCPtr->SetTitle (frametitle);
			CMImageDoc* documentCPtr = (CMImageDoc *)(gActiveImageViewCPtr->GetDocument ());
			documentCPtr->SetTitle (frametitle);
			
			} // end " if (gActiveImageViewCPtr != NULL)"
   #endif

}	// end "SetActiveImageWindowTitle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetImageWindowTitle
//
//	Software purpose:	The purpose of this routine is to set the specified
//							title to the specified image window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2013
//	Revised By:			Larry L. Biehl			Date: 04/27/2017 
 
void SetImageWindowTitle (
				Handle								windowInfoHandle,
				UCharPtr								titleStringPtr)

{         
	WindowPtr							windowPtr;
	
	
	windowPtr = GetWindowPtr (windowInfoHandle);
	  
	if (windowPtr != NULL)
		{
		#if defined multispec_mac   
			MSetWindowTitle (windowPtr, titleStringPtr);
		#endif	// defined multispec_mac 
		
		#if defined multispec_win
			//if (windowPtr != NULL)
				//{                      
				titleStringPtr[titleStringPtr[0]+1] = 0;
				CMImageDoc* documentCPtr = gActiveImageViewCPtr->GetDocument ();                   
				//documentCPtr->SetTitle ((LPCTSTR)&titleStringPtr[1]);
				MSetWindowTitle (documentCPtr, titleStringPtr);
				
				//}	// end " if (windowPtr != NULL)" 
		#endif	// defined multispec_win  	
			 
		#if defined multispec_lin
			titleStringPtr[titleStringPtr[0]+1] = 0;
			wxString ntitle =  wxString::FromUTF8 ((char*)&titleStringPtr[1]);
			(windowPtr->m_frame)->SetTitle (ntitle);
		#endif
		}	// end "if (windowPtr != NULL)"
		
}	// end "SetImageWindowTitle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetOutputWTitle
//
//	Software purpose:	This routine set the title for the output text window.
//
//	Parameters in:		Pointer to title string.
//
//	Parameters out:	None.
//
// Value Returned:	None.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1995
//	Revised By:			Larry L. Biehl			Date: 12/13/1995

void SetOutputWTitle (
				UCharPtr								titleStringPtr)
   
{		       
	#if defined multispec_mac 
		SetWTitle (gOutputWindow, titleStringPtr);
	#endif	// defined multispec_mac    
		       
	#if defined multispec_win 
		if (gOutputViewCPtr != NULL)                           
			MSetWindowTitle ((CDocument*)gOutputViewCPtr->GetDocument (),
									(UCharPtr)gTextString);
	#endif	// defined multispec_win  
	
}	// end "SetOutputWTitle"

   

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetPascalStringLengthCharacter
//
//	Software purpose:	This routine is a helper function to handle the conversion of
//							code from character base to unicode base. The routine will use
//							strlen when character string is being used and wcrlen when 
//							unicode strings are being used.
//
//	Parameters in:		Pointer to title string.
//
//	Parameters out:	None.
//
// Value Returned:	None.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2017
//	Revised By:			Larry L. Biehl			Date: 03/09/2017

void SetPascalStringLengthCharacter (
				void* 								stringPtr,
				int									length,
				SInt16								charWidthCode)
	
{				
	if (charWidthCode == kReturnASCII)
		{
		UCharPtr	charStringPtr = (UCharPtr)stringPtr;
		charStringPtr[0] = length;
		
		}	// end "if (charWidthCode == kReturnASCII)"
	
	else // charWidthCode != ReturnASCII
		{
				// Do according to the use_wide_character directive
		wchar_t*	wideStringPtr = (wchar_t*)stringPtr;
		wideStringPtr[0] = length;

		}	// end "else !wideCharStringFlag"
	
}	// end "StringLength"

   

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 StringCompare
//
//	Software purpose:	This routine is a helper function to handle the conversion of
//							code from character base to unicode base. The routine will use
//							strcmp when character string is being used and wcrcmp when 
//							unicode strings are being used.
//
//	Parameters in:		Pointer to title string.
//
//	Parameters out:	None.
//
// Value Returned:	None.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2017
//	Revised By:			Larry L. Biehl			Date: 03/03/2017

SInt16 StringCompare (
				UInt8* 								str1,
				UInt8* 								str2)
	
{											
	return strcmp ((char*)str1, (char*)str2);
	
}	// end "StringCompare"

   

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void* StringCopy
//
//	Software purpose:	This routine is a helper function to handle the conversion of
//							code from character base to wide character base. The routine will use
//							strcpy when character string is being used and wcrcpy when 
//							wide character strings are being used.
//							The string starting at location str2 is copy to the location 
//							given by str1.
//
//	Parameters in:		Pointers string 1 and string 2
//							Code indicating whether single character copy should be forced.
//
//	Parameters out:	None.
//
// Value Returned:	Memory location for the for the new c terminator at the end
//							of str1.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2017
//	Revised By:			Larry L. Biehl			Date: 03/20/2017

void* StringCopy (
				void* 								str1, 
				void* 								str2,
				UInt16								startInputStringIndex,
				UInt16								charWidthCode)
	
{		
	char*									charString1Ptr;
	wchar_t*								wideCharString1Ptr;


	if (charWidthCode == kASCIICharString)
		{
		charString1Ptr = (char*)str1;
		return (void*)strcpy ((char*)&charString1Ptr[startInputStringIndex], 
										(char*)str2);

		}	// end "if (charWidthCode == kASCIICharString)"
	
	else	// charWidthCode == kUnicodeCharString
		{
		wideCharString1Ptr = (wchar_t*)str1;
		return (void*)wcscpy ((wchar_t*)&wideCharString1Ptr[startInputStringIndex], 
										(wchar_t*)str2);
 
		}	// end "else charWidthCode == kUnicodeCharString"
	
}	// end "StringCopy"

   

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		int StringLength
//
//	Software purpose:	This routine is a helper function to handle the conversion of
//							code from character base to unicode base. The routine will use
//							strlen when character string is being used and wcrlen when 
//							unicode strings are being used.
//
//	Parameters in:		Pointer to title string.
//
//	Parameters out:	None.
//
// Value Returned:	None.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2017
//	Revised By:			Larry L. Biehl			Date: 03/10/2017

int StringLength (
				void* 								stringPtr,
				Boolean								asciiCharStringFlag)
	
{				
	if (asciiCharStringFlag)
		return ((int)strlen ((char*)stringPtr));
	
	else // !asciiCharStringFlag
		{
				// Do according to the use_wide_character directive
		return ((int)wcslen ((wchar_t*)stringPtr));

		}	// end "else !asciiCharStringFlag"
	
}	// end "StringLength"
				
				

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void StringToNumber
//
//	Software purpose:	The purpose of this routine is to convert the input string
//							to a number. This will be using CFStringGetIntValue for Mac OS X.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/24/2012
//	Revised By:			Larry L. Biehl			Date: 09/11/2017

void StringToNumber (
				char*									stringPtr,
				SInt32*								theNumPtr)

{
#if defined multispec_mac
	CFStringRef							cfStringPtr;

	cfStringPtr = CFStringCreateWithCString (kCFAllocatorDefault, 
															stringPtr, 
															kCFStringEncodingMacRoman);

	*theNumPtr = CFStringGetIntValue (cfStringPtr);
	
	CFRelease (cfStringPtr);
#endif	// defined multispec_mac

#if defined multispec_mac_swift
		// To be done
#endif	// defined multispec_mac_swift

#if defined multispec_win | defined multispec_lin
	*theNumPtr = atol (stringPtr);
#endif	// defined multispec_win

}	// end "StringToNumber"

				

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		char* StrStrNoCase
//
//	Software purpose:	The purpose of this routine is to find string 2 within string 1
//							ignoring the case of the characters in the string.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	NULL:		string 2 was not found within string 1
//							Address:	location within string 1 where string 2 starts
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 10/19/2017

char* StrStrNoCase (
				const char* 						str1, 
				const char* 						str2)
	
{
	const	unsigned char* 			p1 = (unsigned char*)str1;
	const	unsigned char* 			p2 = (unsigned char*)str2;
	char*									stringStartPtr;
	
	UInt32								c1, 
											c2;
											
	
	c1 = toupper (*p1);
	c2 = toupper (*p2);
		
	while (c1 != 0)
		{
		if (c1 == c2)
			{
			stringStartPtr = (char*)p1;
			while (c1 == c2)
				{
				p1++;
				p2++;
			
				c1 = toupper (*p1);
				c2 = toupper (*p2);
				
				if (c2 == 0)
																		return (stringStartPtr);
				
				}	// end "while (c1 == c2)"
				
					// Substring not found. Check again. 
				
			p1 = (unsigned char*)stringStartPtr;
				
			p2 = (unsigned char*)str2;
			c2 = toupper (*p2);
				
			}	// end "if (c1 == c2)"
				
		p1++;
		c1 = toupper (*p1);
			
		}	// end "while (c1 != 0)"
	
	return (NULL);
	
}	// end "StrStrNoCase"

				

//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		char* StrStrNoCase
//
//	Software purpose:	The purpose of this routine is to find the first 'numberCharacters 
//							of string 2 within string 1 ignoring the case of the characters 
//							in the string.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	NULL:		first n characters of string 2 was not found within 
//												string 1
//							Address:	location within string 1 where string 2 starts
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 10/19/2017

char* StrStrNoCase (
				const char* 						str1, 
				const char* 						str2,
				UInt32								numberCharacters)
	
{
	const	unsigned char* 			p1 = (unsigned char*)str1;
	const	unsigned char* 			p2 = (unsigned char*)str2;
	char*									stringStartPtr;
	
	UInt32								c1, 
											c2,
											index;
											
	
	c1 = toupper (*p1);
	c2 = toupper (*p2);
		
	while (c1 != 0)
		{
		if (c1 == c2)
			{
			index = 1;
			stringStartPtr = (char*)p1;
			do
				{
				if (index >= numberCharacters)
																		return (stringStartPtr);
																					
				p1++;
				p2++;
			
				c1 = toupper (*p1);
				c2 = toupper (*p2);
				
				index++;
				
				}	while (c1 == c2);
				
					// Substring not found. Check again. 
				
			p1 = (unsigned char*)stringStartPtr;
				
			p2 = (unsigned char*)str2;
			c2 = toupper (*p2);
				
			}	// end "if (c1 == c2)"
			
		p1++;
		c1 = toupper (*p1);
			
		}	// end "while (c1 != 0)"
	
	return (NULL);
	
}	// end "StrStrNoCase"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void VerifyDOSFileName
//
//	Software purpose:	The purpose of this routine for the Windows version of
//							MultiSpec is to verify that the input file name does
//							not break the 8 plus 3 character limits for DOS. If
//							it does, the first part of the name is truncated to 8
//							charaters and the extension is removed so that the file
//							name is ready for a new extension to be added.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/23/2017

void VerifyDOSFileName (
				FileStringPtr						filePathPtr)
                     
{			                 
	#if defined multispec_win
		#if !defined _WIN32
			UCharPtr							inFileNamePtr,
												outFileNamePtr;
			
			UInt16							directoryLength,
												nameLength,
												pathLength;
			
			
					// Find the start of the file name. I.E after the directory
					// info.
			
			pathLength = filePathPtr[0];		                        
			inFileNamePtr = (UCharPtr)&filePathPtr[pathLength];
			
			nameLength = 0;
			while ((nameLength < pathLength) && (*inFileNamePtr != '\\'))
				{
				inFileNamePtr--;
				nameLength++;
				
				}	// end "while ((nameLength < pathLength) && ..."
				
			if (nameLength > 0)
				{
				directoryLength = pathLength - nameLength;
				inFileNamePtr++;               
				
						// The start of the file name has been found. Now find the
						// '.' if it exists. If it does, then strip the '.' and the
						// extension off. Also truncate the name to 8 charaters.
						// Also make sure that the name does not have a blank character.                               
				
				nameLength = 0;
				outFileNamePtr = inFileNamePtr;		 
				while ((nameLength < 8) &&
							(*inFileNamePtr != '.') && 
							(*inFileNamePtr != 0))
					{
					if (*inFileNamePtr != ' ')
						{
						if (outFileNamePtr != inFileNamePtr)
							*outFileNamePtr = *inFileNamePtr;
											 
						outFileNamePtr++;              
						nameLength++;
						
						}	// end "if (*inFileNamePtr != ' ')"
						
					inFileNamePtr++; 
					
					}	// end "while ((nameLength < 8) && ..."
					
				pathLength = MIN (directoryLength+nameLength, pathLength); 
				pathLength = MIN (gFileNameLengthLimit-2, pathLength);
				
				filePathPtr[0] = (UInt8)pathLength;
				filePathPtr[pathLength+1] = 0;
				
				}	// end "if (nameLength > 0)"
		#endif	// !defined _WIN32
	#endif	// defined multispec_win
		
}	// end "VerifyDOSFileName"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteSpecifiedStringNumber
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file and writes it out to the specifie file.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was loaded.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/13/2007
//	Revised By:			Larry L. Biehl			Date: 04/13/2007	

Boolean WriteSpecifiedStringNumber (
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								strListID, 
				SInt16								index, 
				char*									resourceStringPtr,
				Boolean								continueFlag)
													
{
	UInt32								stringLength;
	
	SInt16								errCode;
	
	
	continueFlag = GetSpecifiedStringNumber (
							strListID, index, (UCharPtr)resourceStringPtr, continueFlag);
	
	if (continueFlag)
		{
		stringLength = resourceStringPtr[0];
		errCode = MWriteData (resultsFileStreamPtr, 
									 &stringLength, 
									 &resourceStringPtr[1], 
									 kErrorMessages);
		
		if (errCode != noErr)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)"
		
	return (continueFlag);

}	// end "WriteSpecifiedStringNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteSpecifiedStringNumber
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file and writes it out to the specifie file.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was written to file okay.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/13/2007
//	Revised By:			Larry L. Biehl			Date: 04/13/2007	

Boolean WriteSpecifiedStringNumber (
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								strListID, 
				SInt16								index,
				char*									finalStringPtr,
				char*									resourceStringPtr,
				char*									inputStringPtr,
				Boolean								continueFlag)
													
{
	UInt32								stringLength;
	
	SInt16								errCode;
	
	
	continueFlag = LoadSpecifiedStringNumberStringP (
														strListID, 
														index, 
														finalStringPtr,
														resourceStringPtr,
														continueFlag,
														inputStringPtr);
	
	if (continueFlag)
		{
		stringLength = finalStringPtr[0];
		errCode = MWriteData (resultsFileStreamPtr, 
									 &stringLength, 
									 &finalStringPtr[1], 
									 kErrorMessages);
		
		if (errCode != noErr)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)"
		
	return (continueFlag);

}	// end "WriteSpecifiedStringNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteSpecifiedStringNumber
//
//	Software purpose:	This routine loads the specified string number in the
//							resource file and writes it out to the specifie file.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	TRUE if string was written to file okay.
//							FALSE if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2007
//	Revised By:			Larry L. Biehl			Date: 04/25/2007	

Boolean WriteSpecifiedStringNumber (
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								strListID, 
				SInt16								index,
				char*									finalStringPtr,
				char*									resourceStringPtr,
				double								inputDoubleValue1,
				double								inputDoubleValue2,
				double								inputDoubleValue3,
				double								inputDoubleValue4,
				double								inputDoubleValue5,
				Boolean								continueFlag)
													
{
	UInt32								stringLength;
	
	SInt16								errCode;
	
	
	continueFlag = LoadSpecifiedStringNumberDouble (
														strListID, 
														index, 
														finalStringPtr,
														resourceStringPtr,
														continueFlag,
														inputDoubleValue1,
														inputDoubleValue2,
														inputDoubleValue3,
														inputDoubleValue4,
														inputDoubleValue5);
	
	if (continueFlag)
		{		
		stringLength = (UInt32)strlen (finalStringPtr);
		errCode = MWriteData (resultsFileStreamPtr, 
									 &stringLength, 
									 finalStringPtr, 
									 kErrorMessages);
		
		if (errCode != noErr)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)"
		
	return (continueFlag);

}	// end "WriteSpecifiedStringNumber"

