//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						SFileStream_class.cpp
//	Class Definition:		SFileStream_class.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/11/2020
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	This file contains routines which are used to access
//								various disk files.
//
/* Template for debugging for MultiSpec Online on mygeohub.org.
	int numberChars = sprintf (
							(char*)&gTextString3,
							" CMFileStream::MSetSizeOfFile (countBytes, errCode) %d, %d%s", 
							countBytes,
							errCode,
							gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SFileStream_class.h"
#include "SImageWindow_class.h"
							  
#if defined multispec_wx
	#define dupFNErr				-48
#endif	// defined multispec_wx


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CMFileStream
//
//	Software purpose:	Constructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

CMFileStream::CMFileStream (void)
 
{
	InitializeMembers ();

}	// end "CMFileStream"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CMFileStream
//
//	Software purpose:	Constructor 
//							Contruct a file stream object based on the name and location of 
//							the file represents by the input file stream object.  
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 04/12/2020
		
#if defined multispec_win 
CMFileStream::CMFileStream (
				CMFileStream						*inputFileStreamPtr) : CFile ()

{  
	UInt16								length;
	
	
	InitializeMembers ();

			// Copy the input file path including character count
			// and the C string terminator
	
	length = inputFileStreamPtr->mWideFilePathName[0] + 2;
	length = MIN (length, _MAX_PATH);
	
	memcpy (mWideFilePathName, inputFileStreamPtr->mWideFilePathName, (size_t)length);

			// Update the UTF8 version of the path name

	SetUTF8FilePath ();

}	// end "CMFileStream"
#endif	// defined multispec_win
 	
#if defined multispec_wx
CMFileStream::CMFileStream (
				CMFileStream						*inputFileStreamPtr) : wxFile ()

{
	UInt16 								length;

	
	InitializeMembers ();

			// Copy the input file path including character count
			// and the C string terminator

	length = (UInt16)GetWideFileStringLength (inputFileStreamPtr->mWideFilePathName);
	length += 2;
	length = MIN (length, _MAX_PATH);

	wmemcpy ((wchar_t*)&mWideFilePathName,
				(wchar_t*)&inputFileStreamPtr->mWideFilePathName,
				(size_t)length);

			// Update the UTF8 version of the path name

	SetUTF8FilePath ();

}	// end "CMFileStream"
#endif	// defined for multispec_wx


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CMFileStream
//
//	Software purpose:	Constructor 
//							Contruct a file stream object based on the input file name.  
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 04/12/2020

#if defined multispec_win   
CMFileStream::CMFileStream (
				WideFileStringPtr					inFilePathPtr) : CFile ()
				
{   
	UInt16 								length;
	
	
	InitializeMembers ();

	length = inFilePathPtr[0];
	if (length == 0)
		length = (UInt16)StringLength ((void*)&inFilePathPtr[1], FALSE); 
		
			// Copy the input file path including character count and c
			// terminator
			
	length += 2;
	length = MIN (length, _MAX_PATH);
	
	memcpy (mWideFilePathName, inFilePathPtr, (size_t)length);
				
			// Make sure character count is available.
			
	mWideFilePathName[0] = (UInt8)length;
				
			// Make certain that there is a C style terminator at the end.
			
	mWideFilePathName[length+1] = 0;

			// Update the UTF8 version of the path name

	SetUTF8FilePath ();
	
	if (MOpenFile ((SInt16)(kRead), 
							(SInt16)(kNoErrorMessages)) == noErr)
		{                      
		mCreator = -1;
		mFileType = kTEXTFileType;  
	
		}	// end "if (MOpenFile (kRead, kNoErrorMessages) ..."
   
}	// end "CMFileStream" 
#endif 	// defined multispec_win

#if defined multispec_wx
CMFileStream::CMFileStream (
				WideFileStringPtr inWideFilePathPtr) : wxFile ()

{
	UInt16 								length;

	
	InitializeMembers ();

	length = (UInt16)GetWideFileStringLength (inWideFilePathPtr);
	if (length == 0)
		length = wcslen ((wchar_t*)&inWideFilePathPtr[1]);
			
			// Copy the input file path including character count and c
			// terminator
			
	length += 2;
	length = MIN (length, _MAX_PATH);

	wmemcpy ((wchar_t*)&mWideFilePathName, inWideFilePathPtr, (size_t)length);

			// Make sure character count is available.

	mWideFilePathName[0] = length;

			// Make certain that there is a C style terminator at the end.

	mWideFilePathName[length+1] = 0;

	if (MOpenFile ((SInt16)(kRead),(SInt16) (kNoErrorMessages)) == noErr) 
		{
		mCreator = -1;
		mFileType = kTEXTFileType;

		}	// end "if (MOpenFile (kRead, kNoErrorMessages) ..."

}	// end "CMFileStream"
#endif 	// defined multispec_wx


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		~CMFileStream
//
//	Software purpose:	Destructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMFileStream::~CMFileStream ()

{               

}	// end "~CMFileStream" 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisposeFilePath
//
//	Software purpose:	The purpose of this routine is to dispose of the
//							input file path structure.
//		
//	Parameters in:		Pointer to a file path structure.
//
//	Parameters out:	None
//
//	Value Returned:	Pointer to a copy of the input file path structure.
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/11/1995
//	Revised By:			Larry L. Biehl			Date: 03/16/2017

void CMFileStream::DisposeFilePath (
				WideFileStringPtr					inFilePathPtr) 
			                	
{	   									
	inFilePathPtr = (WideFileStringPtr)CheckAndDisposePtr ((Ptr)inFilePathPtr);
		
}	// end "DisposeFilePath" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ConvertFileErrorNumber
//
//	Software purpose:	The purpose of this routine is to convert the input
//							Windows file error number to an equivalent
//							Mac OS file error number.
//		
//	Parameters in:		File operation error code.
//
//	Parameters out:	None
//
//	Value Returned:	Equivalent MacOS file error code
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1995
//	Revised By:			Larry L. Biehl			Date: 11/01/1995
// TODO: For Linux
SInt16 CMFileStream::ConvertFileErrorNumber (
				SInt16								errCode)
                
{	
	SInt16								outErrorCode = errCode;
		
	if (errCode != noErr)
		{                  
		#if defined multispec_win
			switch (errCode)
				{        
				case CFileException::genericException:
					outErrorCode = ioErr;
		   		break;           
		   		    
				case CFileException::fileNotFound:
					outErrorCode = fnfErr;
		   		break;           
		   		
				case CFileException::badPath:
					outErrorCode = bdNamErr;
		   		break;     
		   		
				case CFileException::tooManyOpenFiles:
					outErrorCode = tmfoErr;
		   		break;
		   		
				case CFileException::accessDenied:
					outErrorCode = fLckdErr;
		   		break;
		   		
				case CFileException::invalidFile:
					outErrorCode = bdNamErr;
		   		break;
		   		
				case CFileException::badSeek:
					outErrorCode = posErr;
		   		break;   
		   		
				case CFileException::hardIO:
					outErrorCode = ioErr;
		   		break;   
		   		
				case CFileException::sharingViolation:
					outErrorCode = ioErr;
		   		break;     
		   		
				case CFileException::lockViolation:
					outErrorCode = fLckdErr;
		   		break;           
		   		
				case CFileException::diskFull:
					outErrorCode = dskFulErr;
		   		break;    
		   		
				case CFileException::endOfFile:
					outErrorCode = eofErr;
		   		break;     
		   		
				default: 
 					outErrorCode = ioErr;
		   		break;
		   		       
		   	}	// end "switch (errCode)"
		                   
			#endif	// defined multispec_win 
			
			#if defined multispec_wx
					// Do nothing for now as wxwidgets does not return different error 
					// types
				outErrorCode = errCode;
			#endif
	   	
	  		}	// end "if (errCode != noErr)"
	  		
	return (outErrorCode); 
	   
}	// end "ConvertFileErrorNumber" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		OSErr MOpenFile
//
//	Software purpose:	The purpose of this routine is to determine if the file
//							is already open.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned: 	TRUE if file is already open
//							FALSE if file is not open
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/16/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

Boolean CMFileStream::FileOpen ()
                      
{		   
	#if defined multispec_wx
		UInt16 wideFilePathNameLength = GetWideFileStringLength (mWideFilePathName);
		mWideFilePathName[wideFilePathNameLength+1] = 0;
		//if (wxFile::Exists (wxString::FromAscii ((CharPtr)&mFilePathName[1])))
		if (wxFile::Exists (wxString (&mWideFilePathName[1])))
			return (wxFile::IsOpened ());
		else
			return FALSE;
	#endif
	
	#if defined multispec_win 
		return (m_hFile != CFile::hFileNull);
	#endif	// defined multispec_win  
}	// end "FileOpen"
  

/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Ptr GetCopyOfFilePath
//
//	Software purpose:	The purpose of this routine is to get a copy of the
//							input file path structure and return a pointer to
//							the copy.
//		
//	Parameters in:		Pointer to a file path structure.
//
//	Parameters out:	None
//
//	Value Returned:  Pointer to a copy of the input file path structure.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/11/1995
//	Revised By:			Larry L. Biehl			Date: 02/23/2017

Ptr CMFileStream::GetCopyOfFilePath (
				FileStringPtr						inFilePathPtr) 
			                	
{	       
	Ptr outCStringPtr = NULL;

	size_t bytesToMove = inFilePathPtr[0] + 2;
	
	#if defined multispec_wx
		outCStringPtr = MNewPointer (getsize (inFilePathPtr));
	#endif
	
	#if defined multispec_win
		outCStringPtr = MNewPointer (_MAX_PATH);
	#endif
	
	if (outCStringPtr != NULL)
		memmove (outCStringPtr, inFilePathPtr, bytesToMove);
	
	return (outCStringPtr);                 
		
}	// end "GetCopyOfFilePath" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Ptr GetCopyOfFilePath
//
//	Software purpose:	The purpose of this routine is to get a copy of the
//							input file path structure and return a pointer to
//							the copy.
//		
//	Parameters in:		Pointer to a file path structure.
//
//	Parameters out:	None
//
// Value Returned:  	Pointer to a copy of the input file path structure.
//
// Called By:				
//
//	Coded By:			Larry L. Biehl			Date: 04/11/1995
//	Revised By:			Larry L. Biehl			Date: 04/11/1995

FileStringPtr CMFileStream::GetCopyOfFilePath (void) 
			                	
{	       
	Ptr			outStringPtr = NULL;
	
	
	outStringPtr = MNewPointer (sizeof (Str255));

	if (outStringPtr != NULL)
		memmove (outStringPtr, &mFilePathName, mFilePathName[0]+2);
	
	return ((FileStringPtr)outStringPtr);                 
		
}	// end "GetCopyOfFilePath"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNameCPtr
//
//	Software purpose:	The purpose of this routine is to return the address of
//							the start of the actual file name not including the
//							path.
//		
//	Parameters in:		Code indicating whether the length of the ASCII or wide
//							character path name should be returned
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/10/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void* CMFileStream::GetFileNameCPtr (
				SInt16								returnCode)
                     
{			
	void*									fileNameCPtr;
	
	UInt16								nameLength,
											pathLength;
	

	if (returnCode == kReturnASCII)
		{
				// This will force the file path name length to be computed if needed.

		GetFileUTF8PathLength ();

				// Return the start address for the file name.

		fileNameCPtr = (void*)&mUTF8FilePathName[mUTF8PathLength+2];

		}	// end "if (returnCode == kReturnASCII)"

	else	// returnCode != kReturnASCII
		{
		//pathLength = mWideFilePathName[0];
		pathLength = GetWideFileStringLength (mWideFilePathName);
	
		if (pathLength > 0)
			{         
			if (mWidePathLength > 0)
						// The start of the file name has already been computed.
						// Just return the start address for the file name.
				fileNameCPtr = (void*)&mWideFilePathName[mWidePathLength+1];

			else	// mPathLength == 0
				{
				WideFileStringPtr		wFileNameCPtr;

				wFileNameCPtr = &mWideFilePathName[pathLength];
				
				nameLength = 0;
				#if defined multispec_wx
					while ((nameLength < pathLength) && (*wFileNameCPtr != '/'))
				#endif

				#if defined multispec_win
					while ((nameLength < pathLength) && (*wFileNameCPtr != '\\'))
				#endif
						{
						wFileNameCPtr--;
						nameLength++;
					
						}	// end "while ((nameLength < pathLength) && ..."
					
				if (nameLength > 0)
					wFileNameCPtr++;

				mWidePathLength = pathLength - nameLength;

				fileNameCPtr = (void*)wFileNameCPtr;

				}	// end "else mPathLength == 0"
				
			}	// end "if (pathLength > 0)"
			
		else	// pathLength == 0
			{ 
					// Make sure that there is a c terminator.
					
			mWideFilePathName[1] = 0;
			mWidePathLength = 0;

			fileNameCPtr = (void*)&mWideFilePathName[1];
			
			}	// else pathLength == 0	

		}	// end "else returnCode != kReturnASCII"
		
	return (fileNameCPtr);
		
}	// end "GetFileNameCPtr"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileNamePPtr
//
//	Software purpose:	The purpose of this routine is to return the address of
//							the start of the actual file name not including the
//							path. The pointer is returned as a pascal name pointer.
//		
//	Parameters in:		Code indicating whether the length of the ASCII or wide
//							character path name should be returned
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/18/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void* CMFileStream::GetFileNamePPtr (
				SInt16								returnCode)
                     
{			
	void*									fileNamePtr;
	wchar_t*								fileNameCPtr;
	WideFileStringPtr					wideFileNamePtr;

	UInt16								nameLength;
							
	
	if (returnCode == kReturnASCII)
		{	
		fileNamePtr = (void*)mUTF8FileName;

		}	// end "if (returnCode == kReturnASCII)"

	else	// returnCode != kReturnASCII
		{
		wideFileNamePtr = (WideFileStringPtr)mWideFileName;

		if (wideFileNamePtr[0] == 0)
			{
					// Force 'Pascal' File Name to be generated from the full wide
					// (unicode) path name. In other words the first element is the
					// the size of the string.

			fileNameCPtr = (wchar_t*)GetFileNameCPtr (kReturnUnicode);
		
			nameLength = (UInt16)wcslen (fileNameCPtr);
			nameLength = MIN (nameLength, _MAX_PATH);
		
			if (nameLength > 0)
				wmemcpy (&mWideFileName[1], fileNameCPtr, nameLength);
		
			mWideFileName[nameLength+1] = 0;
			
			//mWideFileName[0] = (UInt8)nameLength;
			SetWideFileStringLength(mWideFileName, nameLength);
			
			}	// end "if (wideFileNamePtr[0] == 0)"

		fileNamePtr = (void*)mWideFileName;

		}	// end "else returnCode != kReturnASCII"
		
	return (fileNamePtr); 	
		
}	// end "GetFileNamePPtr"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFilePathPPtr
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the specified full file path name.
//		
//	Parameters in:		Code indicating whether the length of the ASCII or wide
//							character path name should be returned
//
//	Parameters out:	None
//
//	Value Returned: 	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/11/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void* CMFileStream::GetFilePathPPtr (
				SInt16								returnCode)                       
{			
	#if defined multispec_wx
		if (returnCode == kReturnASCII)
			return ((void*)mUTF8FilePathName);
		else	// returnCode != kReturnASCII
			return ((void*)mWideFilePathName);
	#endif	// ddefined multispec_wx

	#if defined multispec_win
		if (returnCode == kReturnASCII)
			return ((void*)mUTF8FilePathName);
		else	// returnCode != kReturnASCII
			return ((void*)mWideFilePathName);
	#endif	// defined multispec_win
		
}	// end "GetFilePathPPtr"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFilePathPtr
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input file
//							path pointer.
//		
//	Parameters in:		Address to FileInfo structure for file to be closed.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/11/1995
//	Revised By:			Larry L. Biehl			Date: 03/14/2017

void* CMFileStream::GetFilePathPtr (
				FileStringPtr						filePathPtr)
                     
{			
	return ((void*)filePathPtr);    
		
}	// end "GetFilePathPtr" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		OSErr GetFileType
//
//	Software purpose:	The purpose of this routine is to get the file type
//							for the input file.
//		
//	Parameters in:		Macintosh: 	Pointer to FSSpec record for file
//							Windows:		Path for file
//
//	Parameters out:	the file type
//
//	Value Returned:	OSErr for file IO.			
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/11/1995
//	Revised By:			Larry L. Biehl			Date: 06/22/2017
                     
#if defined multispec_win 
OSErr	CMFileStream::GetFileType (
				TBYTE*								filePathPtr,
				long int								*fileTypePtr)

{
	CFileStatus							status;
	SInt16								errCode;
	UInt16								numSuffixChars;
	Boolean								fileExistsFlag = FALSE;


			// Determine if the file exists
			// Make certain that there is a C style string terminator
			// at the end of the file path. 
	
	filePathPtr[filePathPtr[0]+1] = 0; 

	try
		{
		status.m_size = 0;
		status.m_szFullName[0] = 0;
		fileExistsFlag = CFile::GetStatus ((LPCTSTR)&filePathPtr[1], status);

		}	// end try

	catch (...)
		{
				// Added this code for .NET because GetStatus would return an
				// uncaught error when checking a file on a cd when the time
				// for last accessed for reading was 0. I really don't care what
				// the time was for the last read time.  Does not really matter
				// for files on cd's, etc.

		if (status.m_size > 0 && status.m_szFullName[0] != 0)
			fileExistsFlag = TRUE;

		}	// end catch
   
   if (fileExistsFlag)
   	{  
		if (CompareSuffixNoCase ("\0.STA", filePathPtr, &numSuffixChars))
			*fileTypePtr = kISTAFileType;
			
		else if (CompareSuffixNoCase ("\0.TRL", filePathPtr, &numSuffixChars))
			*fileTypePtr = kITRLFileType;
			
		else if (CompareSuffixNoCase (
								"\0.TransformMatrix", filePathPtr, &numSuffixChars))
			*fileTypePtr = kTRANFileType;
		
		else	
			*fileTypePtr = kTEXTFileType;
			
		errCode = noErr; 
			
		}	// end "if (CFile::GetStatus (..."
		
	else
		errCode = CFileException::badPath;
	
	return (errCode);

}	// end "GetFileType"
#endif	// defined multispec_win
	
#if defined multispec_wx
OSErr	CMFileStream::GetFileType (
				WideFileStringPtr					filePathPtr,
				long int								*fileTypePtr)
				
{
	SInt16								errCode;
	Boolean								fileExistsFlag = FALSE;
	
	
			// Determine if the file exists
			// Make certain that there is a C style string terminator
			// at the end of the file path.

	filePathPtr[filePathPtr[0]+1] = 0;
	wxString strw (&filePathPtr[1]);
	wxFileName wfile = wxFileName (strw);
	if (wfile.FileExists ())
		fileExistsFlag = TRUE;
		
	if (fileExistsFlag) 
		{
		if (CompareSuffixNoCase ((char*)"\0.STA", filePathPtr, NULL))
			*fileTypePtr = kISTAFileType;

		else if (CompareSuffixNoCase ((char*)"\0.TRL", filePathPtr, NULL))
			*fileTypePtr = kITRLFileType;

		else if (CompareSuffixNoCase (
								(char*)"\0.TransformMatrix", filePathPtr, NULL))
			*fileTypePtr = kTRANFileType;

		else
			*fileTypePtr = kTEXTFileType;

		errCode = noErr;

		}	// end "if (CFile::GetStatus (..."

	else
		errCode = -1;

	return (errCode);

}	// end "GetFileType"
#endif	// defined multispec_wx


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileUTF8PathLength
//
//	Software purpose:	The purpose of this routine is to find the length of just
//							the path name in the full UTF8 formatted file path name.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned: 	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/20/2017
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

UInt16 CMFileStream::GetFileUTF8PathLength ()       
{
	UInt8*								utf8FileNamePtr;
	
	UInt16								fullFilePathLength,
											nameLength;


	if (mUTF8PathLength == 0)
		{
		utf8FileNamePtr = mUTF8FilePathName;
		//fullPathLength = mUTF8FilePathName[0];
		fullFilePathLength = GetFileStringLength (mUTF8FilePathName);
		utf8FileNamePtr = &mUTF8FilePathName[fullFilePathLength+1];

		nameLength = 0;
		#if defined multispec_wx
			while ((nameLength < fullFilePathLength) && (*utf8FileNamePtr != '/'))
		#endif
		#if defined multispec_win
			while ((nameLength < fullFilePathLength) && (*utf8FileNamePtr != '\\'))
		#endif
			{
			utf8FileNamePtr--;
			nameLength++;
			
			}	// end "while ((nameLength < pathLength) && ..."
			
		if (nameLength > 0 && fullFilePathLength >= nameLength)
			mUTF8PathLength = fullFilePathLength - nameLength;

		else
			mUTF8PathLength = 0;

		}	// end "if (mUTF8PathLength == 0)"
		
	return (mUTF8PathLength);
 
}	// end "GetFileUTF8PathLength"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetPathFileLength
//
//	Software purpose:	The purpose of this routine is to get the length of the full
//							file path name.
//
//	Parameters in:		Code indicating whether the length of the UTF8 or Unicode (Wide)
//							character path name should be returned
//
//	Parameters out:	None
//
//	Value Returned: 	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2020
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

int CMFileStream::GetPathFileLength (
				SInt16								returnCode)
{
	#if defined multispec_wx
		if (returnCode == kReturnASCII)
			return (GetFileStringLength (mUTF8FilePathName));
		else	// returnCode != kReturnASCII
			return (mWidePathLength);
	#endif	// ddefined multispec_wx

	#if defined multispec_win
		if (returnCode == kReturnASCII)
			return (GetFileStringLength (mUTF8FilePathName));
		else	// returnCode != kReturnASCII
			return (mWidePathLength);
	#endif	// defined multispec_win
		
}	// end "GetPathFileLength"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void IOCheck
//
//	Software purpose:	The purpose of this routine is to check for IO
//							Errors and post a message if one has occured.
//
//	Parameters in:		File operation error code.
//							Address of file name.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/10/2020
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void CMFileStream::InitializeMembers ()

{
	mWideFilePathName[0] = 0;
	mWideFilePathName[1] = 0;
	
	mWideFileName[0] = 0;
	mWideFileName[1] = 0;

	mUTF8FilePathName[0] = 0;
	mUTF8FilePathName[1] = 0;
	mUTF8FilePathName[2] = 0;
	
	mUTF8FileName[0] = 0;
	mUTF8FileName[1] = 0;
	mUTF8FileName[2] = 0;

	//mUTF8PathNameLength = 0;
	//mUTF8FileNameLength = 0;
	mWidePathLength = 0;
	mUTF8PathLength = 0;

	mCreator = -1;
	mFileType = -1;
			
}	// end "GetPathFileLength"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void IOCheck
//
//	Software purpose:	The purpose of this routine is to check for IO 
//							Errors and post a message if one has occured.
//		
//	Parameters in:		File operation error code.
//							Address of file name.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 12/10/2019
// Todo: For Linux

void CMFileStream:: IOCheck (
				SInt16								errCode)
                
{	
	if (errCode != noErr)
		{            
		#if defined multispec_win || defined multispec_wx
			::IOCheck (errCode, (char*)NULL);
		#endif	// defined multispec_win || ...
		   	
		}	// end "if (errCode != noErr)" 
		   
}	// end "IOCheck" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		MCloseFile
//
//	Software purpose:	The purpose of this routine is to close and unlock
//							the file that is described by fileInfo.
//		
//	Parameters in:		Address to FileInfo structure for file to be closed.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1988
//	Revised By:			Larry L. Biehl			Date: 04/10/1995

void CMFileStream::MCloseFile (void)
                      
{			
	#if defined multispec_win 
		TRY
			{ 
			if (m_hFile != CFile::hFileNull)
				Close ();
			
			}
			
		CATCH (CFileException, e)
			{                                                        
			IOCheck (e->m_cause);
			
			}
		END_CATCH
	#endif	// defined multispec_win
   
	#if defined multispec_wx
		if (IsOpened ())
			Close ();
	#endif
		
}	// end "MCloseFile"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MCreateNewFile
//
//	Software purpose:	The purpose of this routine is to create a new data file
//							based on the current object parameters.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/18/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

SInt16 CMFileStream::MCreateNewFile (
				Boolean								replaceFlag)
                      
{			
	SInt16								errCode;
         
   
   		// Make sure the current file is closed.
   		                       
   MCloseFile ();
   
   		// Now open a new file. If the file already exists the size
   		// is changed to 0 bytes.

	#if defined multispec_wx
		if (!replaceFlag && 
			//wxFile::Exists (wxString::FromAscii ((CharPtr)&mFilePathName[1])))
			wxFile::Exists (wxString (&mWideFilePathName[1])))
																					return (dupFNErr);
	
				// This will overwrite it if it already exists
				
		errCode = MOpenFile ((SInt16)(wxFile::write), (SInt16)kErrorMessages);
		MCloseFile ();
		
				// Now open it in read/write mode
	
		if (errCode == noErr)
			errCode = MOpenFile ((SInt16)(wxFile::read_write), (SInt16)kErrorMessages);
	#endif

	#if defined multispec_win
		errCode = MOpenFile (
						(SInt16)(CFile::modeReadWrite|CFile::modeCreate|CFile::typeBinary),
						(SInt16)(kErrorMessages));
	#endif

	return (errCode);
		
}	// end "MCreateNewFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		OSErr MDeleteFile
//
//	Software purpose:	The purpose of this routine is to open the file.
//		
//	Parameters in:		Read/Write code.
//
//	Parameters out:	None
//
//	Value Returned: 	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/16/2010
//	Revised By:			Larry L. Biehl			Date: 08/16/2010

SInt16 CMFileStream::MDeleteFile (
				UInt16								messageCode)
                      
{			
#if defined multispec_win 
	CFileException						error;
	SInt16								errCode = noErr;
	
   if (mWideFilePathName[0] > 0)
		{
				// Make certain that there is a C style terminator at the end.
				
		mWideFilePathName[mWideFilePathName[0]+1] = 0;
		
		TRY
   		{                                           
   		Remove ((LPCTSTR)&mWideFilePathName[1]);
   	
   		}
   	
		CATCH (CFileException, e)
   		{                     
			errCode = error.m_cause;
				               
			if (messageCode == kErrorMessages)                                      
	   		IOCheck (errCode);
   	
   		errCode = ConvertFileErrorNumber (errCode);
   	
   		}
		END_CATCH 
	           			
		}	// end "if (mWideFilePathName[0] > 0)"
		
	return (errCode); 
#endif	// defined multispec_win   
   
#if defined multispec_wx 
	SInt16				errCode = noErr;
	
	UInt16 filePathNameLength = GetWideFileStringLength (mWideFilePathName);
	
   //if (mWideFilePathName[0] > 0)
   if (filePathNameLength > 0)
		{
				// Make certain that there is a C style terminator at the end.
				
		//mWideFilePathName[mWideFilePathName[0]+1] = 0;
		mWideFilePathName[filePathNameLength+1] = 0;
		
   	bool suc = wxRemoveFile (&mWideFilePathName[1]);
      if (!suc)
         errCode = -36;
   	 		
		}	// end "if (filePathNameLength > 0)"
		
	return (errCode); 
#endif	// defined multispec_wx
		
}	// end "MDeleteFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MFlushFile
//
//	Software purpose:	The purpose of this routine is to create a new data file
//							based on the current object parameters.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/18/1995
//	Revised By:			Larry L. Biehl			Date: 05/18/1995

Boolean CMFileStream::MFlushFile (void)
                      
{		
	Boolean								continueFlag = TRUE;
	 	
	#if defined multispec_win  
				// Flush any data in the buffer to disk.

		TRY
			{                                           
			Flush (); 
			
			}
			
		CATCH (CFileException, e)
			{                                                                          
			IOCheck (e->m_cause);
			continueFlag = FALSE;
			
			}
		END_CATCH 
	#endif	// defined multispec_win     
			
	#if defined multispec_wx
		 if (!Flush ())
			  continueFlag = FALSE;
	#endif
	
	return (continueFlag);

}	// end "MFlushFile"           



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MGetMarker
//
//	Software purpose:	The purpose of this routine is to get the current location
//							of the file marker in the current file.
//
//	Parameters in:		None
//
//	Parameters out:	Location of the marker from the beginning of the file.
//
//	Value Returned:	Return error code.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/07/1995 
//	Revised By:			Larry L. Biehl			Date: 12/22/2003  

SInt16 CMFileStream::MGetMarker (
				SInt64*								outOffsetPtr,
				SInt16								messageCode)
                    
{
	SInt16								errCode = noErr;
		
	#if defined multispec_win 
		TRY
			{
			*outOffsetPtr = GetPosition ();
			
			}
			
		CATCH (CFileException, e)
			{  
			errCode = e->m_cause;
			if (messageCode == kErrorMessages)                                                       
				IOCheck (errCode);
			
			errCode = ConvertFileErrorNumber (errCode);
			
			}
			
		END_CATCH
	#endif	// defined multispec_win
		
	#if defined multispec_wx
		 wxFileOffset pos = Tell ();
		 if (pos == wxInvalidOffset)
			  errCode = -1;
			  
		 else
			  *outOffsetPtr=(SInt64)pos;
	#endif
	
	return (errCode);
	
}	// end "MGetMarker"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MGetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to return the size
//							of the file represented by the current object.
//		
//	Parameters in:		None
//
//	Parameters out:	number of bytes in the file
//
//	Value Returned:	Error code
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/05/1995
//	Revised By:			Larry L. Biehl			Date: 12/22/2003

SInt16 CMFileStream::MGetSizeOfFile (
				SInt64*								countPtr)
                      
{			
	SInt16			errCode = noErr;
	
	#if defined multispec_win
		TRY
			{
			*countPtr = (SInt64)GetLength ();
			
			}
			
		CATCH (CFileException, e)
			{  
			errCode = e->m_cause;                                                      
			IOCheck (errCode);
			
			errCode = ConvertFileErrorNumber (errCode);
			
			}
		END_CATCH
	#endif	// defined multispec_win   
		
	#if defined multispec_wx
		 if (IsOpened ())
			{
			wxFileOffset len = Length ();
			*countPtr = (SInt64)len;
			
			}	// end "if (IsOpened ())"
			
		 else	// !IsOpened ()
			errCode = -1;
	#endif	

	return (errCode);
	
}	// end "MGetSizeOfFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		OSErr MOpenFile
//
//	Software purpose:	The purpose of this routine is to open the file.
//		
//	Parameters in:		Read/Write code.
//
//	Parameters out:	None
//
//	Value Returned: 	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/04/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

SInt16 CMFileStream::MOpenFile (
				UInt16								readWriteCode,
				UInt16								messageCode)
                      
{			
	SInt16								errCode = noErr;  
	

	#if defined multispec_wx
		Boolean								checkErrorFlag = FALSE;
	
				// Use GetLastError () to get any errors.

		UInt16 filePathNameLength = GetWideFileStringLength (mWideFilePathName);
		//mWideFilePathName[mFilePathName[0]+1] = 0;
		mWideFilePathName[filePathNameLength+1] = 0;
		if (readWriteCode == kRead)
			{		
			if (wxFile::Exists (wxString (&mWideFilePathName[1])))
				{
				if (Open (wxString (&mWideFilePathName[1]), kRead))
					{
					mCreator = -1;
					mFileType = kTEXTFileType;
					open_mode = readWriteCode;
					
					}	// end "if (Open ((CharPtr)inFilePathPtr, ..."

				else	// !Open (...
					{
					checkErrorFlag = TRUE;
					
					}	// end "else !Open (..."
					
				}	// end "if (wxFile::Exists (wxString (&mFilePathName[1])))"
				
			else	// !wxFile::Exists (wxString (&mFilePathName[1]))
				errCode = fnfErr;
				
			}	// end "if (readWriteCode == kRead)"
			
		else if (readWriteCode == kWrite)
			{
			if (Open (wxString (&mWideFilePathName[1]), kWrite))
				{
				mCreator = -1;
				mFileType = kTEXTFileType;
				open_mode = readWriteCode;
				
				}	// end "if (Open ((CharPtr)inFilePathPtr, ..."

			else	// !Open (...
				{
				checkErrorFlag = TRUE;
				
				}	// end "else !Open (..."
				
			}	// end "else if (readWriteCode == kWrite)"
			
		else if (readWriteCode == kReadWrite)
			{
			if (Open (wxString (&mWideFilePathName[1]), kReadWrite))
				{
				mCreator = -1;
				mFileType = kTEXTFileType;
				open_mode = readWriteCode;
			  
				}	// end "if (Open ((CharPtr)inFilePathPtr, ..."

			else	// !Open (...
				{
				checkErrorFlag = TRUE;
				
				}	// end "else !Open (..."
				
			}	// end "else if (readWriteCode == kReadWrite)"
	
		if (checkErrorFlag)
			{
			errCode = GetLastError ();
					// wxWidgets presents its own error message for this.
			//if (messageCode == kErrorMessages)
			//	::IOCheck (errCode, this);
			errCode = ConvertFileErrorNumber (errCode);
			
			}	// end "checkErrorFlag"
	#endif	// defined multispec_wx
	
	#if defined multispec_win 	
		CFileException						error;
		
		
		if (m_hFile == CFile::hFileNull)
			{     
					// Make certain that there is a C style terminator at the end.
				
			mWideFilePathName[mWideFilePathName[0]+1] = 0;
				  
			if (Open ((LPCTSTR)&mWideFilePathName[1],
							readWriteCode | shareDenyNone,
							&error))
				{ 	                       
				mCreator = -1;
				mFileType = kTEXTFileType;
			
				}	// end "if (Open ((CharPtr)inFilePathPtr, ..."
				
			else	// !Open (...
				{
				errCode = error.m_cause;
				//errCode = (SInt16)GetLastError ();
										
				if (messageCode == kErrorMessages)                                      
					IOCheck (errCode);
			
				errCode = ConvertFileErrorNumber (errCode);
					
				}	// end "else !Open (..."
				
			}	// end "if (m_hFile == CFile::hFileNull)"
	#endif	// defined multispec_win   
		
	return (errCode);
		
}	// end "MOpenFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MPeekData
//
//	Software purpose:	The purpose of this routine is to read the request number
//							of bytes from the file that is in scope.  The file pointer is
//							not moved.
//
//	Parameters in:		Pointer to the buffer to copy the data to
//							Number of bytes to read.
//
//	Parameters out:	None
//
//	Value Returned:	Number of bytes actually read
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/17/1995
//	Revised By:			Larry L. Biehl			Date: 04/17/1995
                      
SInt16 CMFileStream::MPeekData (
				void									*outBufferPtr,
				UInt32								*numberBytesPtr)
                    
{
	SInt32								bytesRead = 0;
   SInt16								errCode = noErr;
   
	#if defined multispec_win 
	   ULONGLONG							currentPosition;
	   
	   
	   TRY
	   	{
	  		currentPosition = GetPosition ();                      
	   	bytesRead = Read (outBufferPtr, *numberBytesPtr);
			currentPosition = Seek (currentPosition, CFile::begin);
	   	
	   	}
	   	
	   CATCH (CFileException, e)
	   	{  
	   	errCode = e->m_cause;                                                      
	   	IOCheck (errCode);
	   	
	   	errCode = ConvertFileErrorNumber (errCode);
	   	
	   	}
	   END_CATCH				                        
	#endif	// defined multispec_win

	#if defined multispec_wx
		if (IsOpened ())
			{
			wxFileOffset currentPosition;
			currentPosition = Tell ();
			bytesRead = (SInt32)Read (outBufferPtr, (size_t)*numberBytesPtr);
			currentPosition = Seek (currentPosition, wxFromStart);
			if (currentPosition == wxInvalidOffset)
				errCode = -1;
				
			}	// end "if (IsOpened ())"
			
		else	// !IsOpened ()
			errCode = -1;
	#endif	// defined lin

	*numberBytesPtr = bytesRead;
		
	return (errCode);
  
}	// end "MPeekData"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MReadData
//
//	Software purpose:	The purpose of this routine is to read the request number
//							of bytes from the file that is in scope.
//
//	Parameters in:		Pointer to the buffer to copy the data to
//							Number of bytes to read.
//
//	Parameters out:	None
//
//	Value Returned:	Number of bytes actually read
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/14/1995
//	Revised By:			Larry L. Biehl			Date: 08/27/2018
                      
SInt16 CMFileStream::MReadData (
				void									*outBufferPtr,
				UInt32								*numberBytesPtr,
				SInt16								messageCode)
                    
{
	UInt32								bytesRead = 0;
	SInt16								errCode = noErr;
	
	
	#if defined multispec_win 
		//if (m_hFile == CFile::hFileNull)
		//	return 1;

		TRY
			{
			bytesRead = Read (outBufferPtr, *numberBytesPtr);

			if (*numberBytesPtr != bytesRead)       
				AfxThrowFileException (CFileException::endOfFile, -1);

			}

		CATCH (CFileException, e)
			{  
			errCode = e->m_cause;

			if (messageCode == kErrorMessages)                                                       
				IOCheck (errCode); 

			errCode = ConvertFileErrorNumber (errCode);

			}
		END_CATCH 				                        
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		if (IsOpened ())
			{
			SInt32								linuxBytesRead;
			//wxFileOffset						flength = Length ();
			//wxFileOffset						cmarker = Tell ();
			
					// First see if the number of bytes left in file is less than bytes to 
					// read
					
			//wxFileOffset bytesinFile = flength - cmarker;
			//UInt32 bytestoread = (UInt32)MIN (bytesinFile, *numberBytesPtr);
			linuxBytesRead = (SInt32)Read	(outBufferPtr, (size_t)*numberBytesPtr);
			//if (linuxBytesRead != bytestoread)
			//	errCode = eofErr;
				
					// Also, Check if end of file has been reached
					
			//if (bytestoread < *numberBytesPtr)
			if (linuxBytesRead != *numberBytesPtr)
				errCode = eofErr;
				
			bytesRead = MAX (linuxBytesRead, 0);
			
			}	// end "if (IsOpened ())"
			
		else	// !IsOpened ()
			errCode = -1;
	#endif
	
	*numberBytesPtr = bytesRead;
					
	return (errCode);
  
}	// end "MReadData"            



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MSetMarker
//
//	Software purpose:	The purpose of this routine is to set the file marker
//							in the current file.
//
//	Parameters in:		Location to set the marker from the beginning.
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/28/1995 
//	Revised By:			Larry L. Biehl			Date: 01/17/1996  

SInt16 CMFileStream::MSetMarker (
				SInt64								inOffset,
				SInt16								fromCode,
				SInt16								messageCode)
                    
{
	SInt16								errCode = noErr;
		
	#if defined multispec_win 
		//if (m_hFile != CFile::hFileNull)
	
	   TRY
	   	{
	   	UInt16 wFromCode = CFile::begin;
	   	
	   	if (fromCode == fsFromMark)
	   		wFromCode = CFile::current;     
	   	
	   	if (fromCode == fsFromLEOF)
	   		wFromCode = CFile::end;
	   		
			inOffset = Seek (inOffset, wFromCode);
	   	
	   	}
	   	
	   CATCH (CFileException, e)
	   	{  
	   	errCode = e->m_cause; 
			if (messageCode == kErrorMessages)                                                      
	   		IOCheck (errCode);
			   	
			errCode = ConvertFileErrorNumber (errCode);
	   	
	   	}
	   	
	   END_CATCH				                        
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		if (IsOpened ())
			{
			wxSeekMode mode = wxFromStart;
			if (fromCode == fsFromMark)
				mode = wxFromCurrent;

			if (fromCode == fsFromLEOF)
            mode = wxFromEnd;

			wxFileOffset offset = Seek (wxFileOffset (inOffset), mode);
			if (offset == wxInvalidOffset)
            errCode = -1;
				
			}	// end "if (IsOpened ())"
	#endif

	return (errCode);
  
}	// end "MSetMarker"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MSetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to set the size
//							of the file represented by the current object.
//		
//	Parameters in:		number of bytes to set the file to
//
//	Parameters out:	none
//
//	Value Returned:	Error code
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 01/10/2017

SInt16 CMFileStream::MSetSizeOfFile (
				SInt64								countBytes)
                      
{		
	SInt16								errCode = noErr;
	
	
	#if defined multispec_wx
		errCode = ftruncate (fd (), countBytes); 
	#endif	// defined multispec_wx

	#if defined multispec_win
	   TRY
	   	{
			SetLength ((UInt32)countBytes);
	   	
	   	}
	   	
	   CATCH (CFileException, e)
	   	{  
	   	errCode = e->m_cause;                                                      
	   	IOCheck (errCode);
				   	
			errCode = ConvertFileErrorNumber (errCode);
	   	
	   	}
	   END_CATCH 	
	#endif	// defined multispec_win

	return (errCode);   
		
}	// end "MSetSizeOfFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 MWriteData
//
//	Software purpose:	The purpose of this routine is to read the request number
//							of bytes from the file that is in scope.
//
//	Parameters in:		Pointer to the buffer to copy the data to
//							Number of bytes to read.
//
//	Parameters out:	None
//
//	Value Returned:	Number of bytes actually read
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/17/1995
//	Revised By:			Larry L. Biehl			Date: 11/01/1995
                      
SInt16 CMFileStream::MWriteData (
				void									*inBufferPtr,
				UInt32								*numberBytesPtr,
				SInt16								messageCode)
                    
{
	SInt16 		errCode = noErr; 
	
	#if defined multispec_win 
		//if (m_hFile == CFile::hFileNull)
		//		return 1;
	   	
	   TRY
	   	{
	   	Write (inBufferPtr, *numberBytesPtr);
	   	
	   	}
	   	
	   CATCH (CFileException, e)
	   	{  
	   	errCode = e->m_cause;
	   	if (messageCode == kErrorMessages)                                                      
	   		IOCheck (errCode); 
					   	
			errCode = ConvertFileErrorNumber (errCode);
	   	
	   	}
	   END_CATCH  				                        
	#endif	// defined multispec_win
      
	#if defined multispec_wx
		size_t written;
		if (IsOpened ())
			{
			written = Write (inBufferPtr, *numberBytesPtr);
			if (written != *numberBytesPtr)
				errCode = -1;
				
			}	// end "if (IsOpened ())"
			
		else	// !IsOpened ()
			errCode = -1;
	#endif
	
	return (errCode); 
  
}	// end "MWriteData"     


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetFileName
//
//	Software purpose:	The purpose of this routine is to set the file name to
//							the input name. It is put after any existing path
//							information.
//		
//	Parameters in:		Pointer to file name in pascal format.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/15/1999
//	Revised By:			Larry L. Biehl			Date: 04/11/2020

void CMFileStream::SetFileName (
				FileStringPtr						inFileNamePtr)
                     
{			
	WideFileStringPtr					fileNamePtr,
											filePathNamePtr;

	int									sizeNeeded;
	UInt16								inFileNameLength,
											wideFilePathNameLength,
											utf8FilePathNameLength;
	
	#if defined multispec_wx
		wxMBConvUTF8						converter;
	#endif
				

	wideFilePathNameLength = GetWideFileStringLength (mWideFilePathName);
	utf8FilePathNameLength = GetFileStringLength (mUTF8FilePathName);
	if (inFileNamePtr == NULL)
		{
				// If a path name already exists. Make sure the file stream is
				// fully set up.

		//if (mFilePathName[0] == 0 && mUTF8PathName[0] > 0)
		if (wideFilePathNameLength == 0 && utf8FilePathNameLength > 0)
			{
					// Need to create the unicode formatted name from the 
					// the existing uft8 formatted string
					
			inFileNamePtr = mUTF8FilePathName;
			filePathNamePtr = mWideFilePathName;

			#if defined multispec_wx
				sizeNeeded = converter.ToWChar (NULL, -1, (char*)&mUTF8FilePathName[2]);
				sizeNeeded = MIN (sizeNeeded, _MAX_PATH-2);
				sizeNeeded = converter.ToWChar (
							&mWideFilePathName[1], sizeNeeded, (char*)&mUTF8FilePathName[2]);
			#endif	// end defined multispec_wx

			#if defined multispec_win
				sizeNeeded = MultiByteToWideChar (
										CP_UTF8, 0, (LPCSTR)&mUTF8FilePathName[2], -1, NULL, 0);
				sizeNeeded = MIN (sizeNeeded, _MAX_PATH-2);
				MultiByteToWideChar (CP_UTF8, 
											0, 
											(LPCSTR)&mUTF8FilePathName[2], 
											-1, 
											&mWideFilePathName[1],
											sizeNeeded);
			#endif	// end defined multispec_win
			
			//mWideFilePathName[0] = (TBYTE)wcslen (&mWideFilePathName[1]);
			SetWideFileStringLength (mWideFilePathName,
												wcslen (&mWideFilePathName[1]));

					// Force the unicode version of the file name to be generated and
					// the path length (no file name) to be computed.

			GetFileNameCPtr (kReturnUnicode);

					// Now force the multibyte file name to be created if they are 
					// requested.
			
			mWideFileName[0] = 0;
			mWideFileName[1] = 0;

			mUTF8FileName[0] = 0;
			mUTF8FileName[1] = 0;
			mUTF8FileName[2] = 0;
			SetUTF8FileName ();

			}	// end "if (mFilePathNameLength == 0 && mUTF8PathNameLength > 0)"

		}	// end "if (inFileNamePtr == NULL)"
	
	else	// inFileNamePtr != NULL)
		{
		//if (mFilePathName[0] == 0 && mUTF8PathName[0] > 0)
		inFileNameLength = GetFileStringLength (inFileNamePtr);
		if (wideFilePathNameLength == 0 && utf8FilePathNameLength > 0)
			{
					// Add the file to the utf8 path string.

			strcpy ((char*)&mUTF8FilePathName[mUTF8PathLength+2],
						(char*)&inFileNamePtr[2]);
			//mUTF8PathName[0] = mUTF8PathLength + inFileNamePtr[0];
			SetFileStringLength (mUTF8FilePathName, mUTF8PathLength + inFileNameLength);

			}	// end "if (mFilePathNameLength == 0 && mUTF8PathNameLength > 0)"

		else	// Add the file name to the unicode path name.
			{
			fileNamePtr = (WideFileStringPtr)GetFileNameCPtr (kReturnUnicode); 

					// Make sure the file name plus path is less than the max buffer length
			
			//inFileNameLength = inFileNamePtr[0];
			inFileNameLength = MIN (inFileNameLength, _MAX_PATH-mWidePathLength-2);
			
					// Make sure that the file name is terminated by a c terminator.
					
			inFileNamePtr[inFileNameLength+2] = 0;

					// Need to convert the input file name from multibyte string
					// to wide string to be able to add to the wide (unicoded)
					// full path.

			#if defined multispec_wx
				sizeNeeded = converter.ToWChar (NULL, -1, (char*)&inFileNamePtr[2]);
				sizeNeeded = MIN (sizeNeeded, _MAX_PATH);
				sizeNeeded = converter.ToWChar (
											fileNamePtr, sizeNeeded, (char*)&inFileNamePtr[2]);
			#endif	// defined multispec_wx

			#if defined multispec_win
				sizeNeeded = MultiByteToWideChar (
										CP_UTF8, 0, (LPCSTR)&inFileNamePtr[2], -1, NULL, 0);
				sizeNeeded = MIN (sizeNeeded, _MAX_PATH);
				MultiByteToWideChar (
						CP_UTF8, 0, (LPCSTR)&inFileNamePtr[2], -1, fileNamePtr, sizeNeeded);
			#endif	// defined multispec_win
									
					// Update the length of the file path string.
					
			//mWideFilePathName[0] = (TBYTE)wcslen (&mFilePathName[1]);
			SetWideFileStringLength (mWideFilePathName, wcslen (&mWideFilePathName[1]));
					
					// Now force the UTF8 version of the file path name to be updated.

			mUTF8FilePathName[0] = 0;
			mUTF8FilePathName[1] = 0;
			mUTF8FilePathName[2] = 0;
			
			mUTF8FileName[0] = 0;
			mUTF8FileName[1] = 0;
			mUTF8FileName[2] = 0;

			}	// end "else add the file name to the unicode path name."

		}	// end "if (inFileNamePtr != NULL)"

	//if (mUTF8FilePathName[0] == 0)
	if (GetFileStringLength (mUTF8FilePathName) == 0)
		SetUTF8FilePath ();
		
}	// end "SetFileName"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetFilePath
//
//	Software purpose:	The purpose of this routine is to set a new file path
//							name. The file represented by the current path is closed
//							if requested.
//		
//	Parameters in:		Input pascal file path and name string
//							Flag indicating whether to close the current file
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/19/1995
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void CMFileStream::SetFilePath (
				#if defined multispec_wx
					wchar_t*								inFilePathPtr, 
				#endif
				#if defined multispec_win
					TBYTE*								inFilePathPtr,  
				#endif
				Boolean								closeFileFlag)
                     
{			
	if (closeFileFlag)
		MCloseFile ();  
		
			// Copy the input file path including pascal character count
	
	wmemcpy (mWideFilePathName,
				inFilePathPtr, 
				(size_t)inFilePathPtr[0]+1);
				
			// Make sure that the path name is terminated by a c terminator.
			
	mWideFilePathName[inFilePathPtr[0]+1] = 0;

			// Update the UTF8 version of the path name

	mUTF8FilePathName[0] = 0;
	mUTF8FilePathName[1] = 0;
	SetUTF8FilePath ();
		
}	// end "SetFilePath"


/*
#if defined multispec_win
void CMFileStream::SetFilePath (
				TBYTE*								inFilePathPtr,  
				Boolean								closeFileFlag)
                     
{
	if (closeFileFlag)
		MCloseFile ();  

			// Copy the input file path including pascal character count

	wmemcpy (mFilePathName, inFilePathPtr, (size_t)inFilePathPtr[0] + 1);

				
			// Make sure that the path name is terminated by a c terminator.
			
	if (inFilePathPtr[0] < 255)
		mFilePathName[inFilePathPtr[0]+1] = 0;

			// Update the UTF8 version of the path name

	mUTF8PathNameLength = 0;
	mUTF8PathName[0] = 0;
	SetUTF8FilePath ();
		
}	// end "SetFilePath"
#endif	// defined multispec_win
*/

#if defined multispec_wx
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetFilePathFromCharString
//
//	Software purpose:	The purpose of this routine is to set a new file path
//							name. The file represented by the current path is closed
//							if requested.
//		
//	Parameters in:		Input c file path and name string
//							Flag indicating whether to close the current file
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/16/2015
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void CMFileStream::SetFilePathFromCharString (
				StringPtr							inFilePathPtr,
				Boolean								closeFileFlag)
                     
{		
	wxString								wxInFilePath;
	wxWCharBuffer						wideCharBuffer;
	
	size_t								wxInFilePathLength;
	
	
	if (closeFileFlag)
		MCloseFile ();  
		
			// Copy the input file path. Allow for a pascal string length
			
	SInt16 filePathLength = (SInt16)strlen ((char*)inFilePathPtr);
	
	if (filePathLength < _MAX_PATH)
		{
				// Convert the input string to a wide character string.
				
		wxInFilePath = wxString::FromAscii ((char*)inFilePathPtr);
		wideCharBuffer = wxInFilePath.wc_str ();
		wxInFilePathLength = wideCharBuffer.length ();
		
		wmemcpy (&mWideFilePathName[1],
					wideCharBuffer.data (), 
					wxInFilePathLength);
					
		mWideFilePathName[0] = (wchar_t)wxInFilePathLength;
				
				// Make sure that the path name is terminated by a c terminator.
		
		UInt16 wideFilePathNameLength = GetWideFileStringLength (mWideFilePathName);
		mWideFilePathName[wideFilePathNameLength+1] = 0;

				// The path length may have changed; force it to be reset.
		
		mWidePathLength = 0;
		mUTF8PathLength = 0;

		mUTF8FilePathName[0] = 0;
		mUTF8FilePathName[1] = 0;
		mUTF8FilePathName[2] = 0;
		SetUTF8FilePath ();
			
		}	// end "if (filePathLength < _MAX_PATH)"
		
}	// end "SetFilePathFromCharString"  
#endif


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUTF8FileName
//
//	Software purpose:	The purpose of this routine is to set the utf8 formatted
//							version of the file name in the class from the full
//							utf8 formatted path name. This is used in places where
//							just the file name is needed.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2017
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void CMFileStream::SetUTF8FileName ()
                     
{			
	FileStringPtr						utf8FileNamePtr;
	
	UInt16								filePathNameLength,
											nameLength = 0;


	utf8FileNamePtr = (FileStringPtr)GetFileNameCPtr ();

	//if (mUTF8FilePathName[0] >= mUTF8PathLength)
	filePathNameLength = GetFileStringLength (mUTF8FilePathName);
	if (filePathNameLength >= mUTF8PathLength)
		nameLength = filePathNameLength - mUTF8PathLength;

	if (nameLength > 0)
		{
		memcpy (&mUTF8FileName[2], utf8FileNamePtr, nameLength);
			
		//mUTF8FileName[0] = (UInt8)nameLength;
		//mUTF8FileName[nameLength+1] = 0;
		SetFileStringLength (mUTF8FileName, nameLength);

		}	// end "if (nameLength > 0)"

	else	// nameLength <= 0
		{
		mUTF8PathLength = 0;
		
		mUTF8FileName[0] = 0;
		mUTF8FileName[1] = 0;
		mUTF8FileName[2] = 0;

		}	// end "else nameLength <= 0"

}	// end "SetUTF8FileName"	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUTF8FilePath
//
//	Software purpose:	The purpose of this routine is to set the utf8 formatted
//							version of the path name in the class from the full
//							unicode (wide character) path name. 
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2017
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void CMFileStream::SetUTF8FilePath ()
                     
{				
	TBYTE*								filePathCPtr;
	UInt8*								utf8StringPtr;
	
	int									size,
											sizeNeeded;
											
	UInt16								filePathNameLength;
	
	#if defined multispec_wx
		wxMBConvUTF8						converter;
		wxCharBuffer						utf8PathCharBuffer;
		char*									utf8PathCharBufferPtr;
		size_t								outputSize;
	#endif

	
	filePathNameLength = GetFileStringLength (mUTF8FilePathName);
	//if (mUTF8PathName[0] == 0)
	if (filePathNameLength == 0)
		{
		filePathCPtr = &mWideFilePathName[1];
		utf8StringPtr = &mUTF8FilePathName[2];
		//std::string utf8 = UTF8FromUTF16 (filePathCPtr);  requires visualc++ 2010
		size = (int)wcslen (filePathCPtr);
		
		#if defined multispec_wx
			UInt8						tempMBPathName[_MAX_PATH+3];
			//UInt8*					mbStringPtr = mUTF8PathName;
			//UInt8						tempMBPathName2[_MAX_PATH+1];
		
			utf8PathCharBuffer = converter.cWC2MB (filePathCPtr, size, &outputSize);
			utf8PathCharBufferPtr = utf8PathCharBuffer.data ();

			if (outputSize > 0)
				{
				memcpy (tempMBPathName, utf8PathCharBufferPtr, outputSize);
				tempMBPathName[outputSize] = 0;
				//CtoPstring (tempMBPathName, tempMBPathName, outputSize);
				strncpy ((char*)&mUTF8FilePathName[2],
							(char*)tempMBPathName,
							outputSize);
				//mUTF8FilePathName[outputSize+1] = 0;
				
				}	// end "if (outputSize > 0)"
				
			sizeNeeded = outputSize;
		#endif	// defined multispec_wx

		#if defined multispec_win
			sizeNeeded = WideCharToMultiByte (
										CP_UTF8, 0, filePathCPtr, size, NULL, 0, NULL, NULL);
										
			WideCharToMultiByte (CP_UTF8, 
										0, 
										filePathCPtr, 
										size, 
										(LPSTR)utf8StringPtr, 
										sizeNeeded, 
										NULL, 
										NULL);
		#endif	// defined multispec_win

				// Add string length & c termination character
				
		//mUTF8FilePathName[0] = sizeNeeded;
		SetFileStringLength (mUTF8FilePathName, sizeNeeded);
			
		//mUTF8PathName[sizeNeeded+1] = 0;

		SetUTF8FileName ();

		}	// end "if (fileStreamPtr->mUTF8PathNameLength == 0)"

}	// end "SetUTF8FilePath"


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetWideFileStringLength
//
//	Software purpose:	The purpose of this routine is to set the input length of the
//							of the input file string in the first 2 bytes of the input
//							file string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/10/2020
//	Revised By:			Larry L. Biehl			Date: 04/11/2020

void CMFileStream::SetWideFileStringLength (
				WideFileStringPtr					wideFileStringPtr,
				int									wideFileStringLength)

{
	UInt16*								wideFileStringLengthPtr;
	
	
	wideFileStringLengthPtr = (UInt16*)wideFileStringPtr;
	wideFileStringLengthPtr[0] = wideFileStringLength;
	
	wideFileStringPtr[wideFileStringLength+1] = 0;

}	// end "SetFileStringLength"

