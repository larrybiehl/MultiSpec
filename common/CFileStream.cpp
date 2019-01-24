//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CFileStream.cpp
//	Class Definition:		CFileStream.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/27/2018
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	This file contains routines which are used to access
//								various disk files.
//
//	Functions in file:	void 					CloseImageFile
//								void 					IOCheck
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

#include "CFileStream.h"
#include "CImageWindow.h"
							  
#if defined multispec_lin
	#define dupFNErr				-48
#endif	// defined multispec_lin


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 06/01/2017	

CMFileStream::CMFileStream (void)
 
{
	#if defined multispec_lin
		mFilePathName[0] = 0;
		mFilePathName[1] = 0; 
		mPascalFileName[0] = 0; 
		mPascalFileName[1] = 0;

		mUTF8PathName[0] = 0;
		mUTF8PathName[1] = 0;
		mUTF8FileName[0] = 0;
		mUTF8FileName[1] = 0;

		mUnicodePathLength = 0;
		mUTF8PathLength = 0;
	
		mCreator = -1;
		mFileType = -1;
	#endif	// defined multispec_lin

	#if defined multispec_win
		mFilePathName[0] = 0;
		mFilePathName[1] = 0; 
		mPascalFileName[0] = 0; 
		mPascalFileName[1] = 0;

		mUTF8PathName[0] = 0;
		mUTF8PathName[1] = 0;
		mUTF8FileName[0] = 0;
		mUTF8FileName[1] = 0;

		mUnicodePathLength = 0;
		mUTF8PathLength = 0;

		mCreator = -1;
		mFileType = -1;
	#endif	// defined win

}		// end "CMFileStream"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 06/01/2017	
		
#if defined multispec_win 
CMFileStream::CMFileStream (
				CMFileStream						*inputFileStreamPtr) : CFile ()

{  
	UInt16								length;
	
	                 
	mFilePathName[0] = 0; 
	mFilePathName[1] = 0;
	mPascalFileName[0] = 0; 
	mPascalFileName[1] = 0;

	mUTF8PathName[0] = 0;
	mUTF8PathName[1] = 0;
	mUTF8FileName[0] = 0;
	mUTF8FileName[1] = 0;

	mUnicodePathLength = 0;
	mUTF8PathLength = 0;

	mCreator = -1;
	mFileType = -1;
	
			// Copy the input file path including pascal character count
			// and the C string terminator
	
	length = inputFileStreamPtr->mFilePathName[0] + 2;
	length = MIN (length, 256);
	
	memcpy (mFilePathName, inputFileStreamPtr->mFilePathName, (size_t)length);

			// Update the UTF8 version of the path name

	SetUTF8FilePath ();

}	// end "CMFileStream"
#endif	// defined multispec_win
 	
#if defined multispec_lin
CMFileStream::CMFileStream (
				CMFileStream						*inputFileStreamPtr) : wxFile ()
{
	UInt16 length;

	mFilePathName[0] = 0;
	mFilePathName[1] = 0;
	mPascalFileName[0] = 0;
	mPascalFileName[1] = 0;

	mUTF8PathName[0] = 0;
	mUTF8PathName[1] = 0;
	mUTF8FileName[0] = 0;
	mUTF8FileName[1] = 0;

	mUnicodePathLength = 0;
	mUTF8PathLength = 0;
	
	mCreator = -1;
	mFileType = -1;

			// Copy the input file path including pascal character count
			// and the C string terminator

	length = inputFileStreamPtr->mFilePathName[0] + 2;
	length = MIN (length, 256);

	wmemcpy ((wchar_t*)&mFilePathName, 
				(wchar_t*)&inputFileStreamPtr->mFilePathName, 
				(size_t)length);

			// Update the UTF8 version of the path name

	SetUTF8FilePath ();

}	// end "CMFileStream"
#endif	// defined for linux


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 06/01/2017	

#if defined multispec_win   
CMFileStream::CMFileStream (
				WideFileStringPtr					inFilePathPtr) : CFile ()
				
{   
	UInt16 								length;
	
	                
	mFilePathName[0] = 0; 
	mFilePathName[1] = 0;
	mPascalFileName[0] = 0; 
	mPascalFileName[1] = 0;

	mUTF8PathName[0] = 0;
	mUTF8PathName[1] = 0;
	mUTF8FileName[0] = 0;
	mUTF8FileName[1] = 0;

	mUnicodePathLength = 0;
	mUTF8PathLength = 0;

	mCreator = -1;
	mFileType = -1;
	
	length = inFilePathPtr[0]; 
	if (length == 0)
		//length = (UInt16)strlen ((char*)&inFilePathPtr[1]);
		length = (UInt16)StringLength ((void*)&inFilePathPtr[1], FALSE); 
		
			// Copy the input file path including pascal character count
			
	length++; 
	length = MIN (length, 256);
	
	memcpy (mFilePathName, inFilePathPtr, (size_t)length);
				
			// Make sure pascal character count is available.
			
	mFilePathName[0] = (UInt8)length;
				
			// Make certain that there is a C style terminator at the end.
			
	mFilePathName[length+1] = 0;

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

#if defined multispec_lin
CMFileStream::CMFileStream (WideFileStringPtr inFilePathPtr) : wxFile ()
{
	UInt16 length;

	mFilePathName[0] = 0;
	mFilePathName[1] = 0;
	mPascalFileName[0] = 0;
	mPascalFileName[1] = 0;

	mUTF8PathName[0] = 0;
	mUTF8PathName[1] = 0;
	mUTF8FileName[0] = 0;
	mUTF8FileName[1] = 0;

	mUnicodePathLength = 0;
	mUTF8PathLength = 0;
	
	mCreator = -1;
	mFileType = -1;

	length = inFilePathPtr[0];
	if (length == 0)
		length = strlen ((char*)&inFilePathPtr[1]);

			// Copy the input file path including pascal character count

	length++;
	length = MIN (length, 256);

	memcpy ((CharPtr)&mFilePathName, inFilePathPtr, (size_t)length);

			// Make sure pascal character count is available.

	mFilePathName[0] = (UInt8)length;

			// Make certain that there is a C style terminator at the end.

	mFilePathName[length+1] = 0;

	if (MOpenFile ((SInt16)(kRead),(SInt16) (kNoErrorMessages)) == noErr) 
		{
		mCreator = -1;
		mFileType = kTEXTFileType;

		}	// end "if (MOpenFile (kRead, kNoErrorMessages) ..."

} // end "CMFileStream"
#endif 	// defined multispec_lin


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
			
			#if defined multispec_lin
					// Do nothing for now as wxwidgets does not return different error 
					// types
				outErrorCode = 0;
			#endif
	   	
	  		}	// end "if (errCode != noErr)"
	  		
	return (outErrorCode); 
	   
}	// end "ConvertFileErrorNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

Boolean CMFileStream::FileOpen ()
                      
{		   
	#if defined multispec_lin
		mFilePathName[mFilePathName[0]+1] = 0;
		//if (wxFile::Exists (wxString::FromAscii ((CharPtr)&mFilePathName[1])))
		if (wxFile::Exists (wxString (&mFilePathName[1])))
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
	
	#if defined multispec_lin
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetFileNameCPtr
//
//	Software purpose:	The purpose of this routine is to return the address of
//							the start of the actual file name not including the
//							path.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/10/1995
//	Revised By:			Larry L. Biehl			Date: 03/20/2017

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

		fileNameCPtr = (void*)&mUTF8PathName[mUTF8PathLength+1];

		}	// end "if (returnCode == kReturnASCII)"

	else	// returnCode != kReturnASCII
		{
		pathLength = mFilePathName[0];  
	
		if (pathLength > 0)
			{         
			if (mUnicodePathLength > 0)
						// The start of the file name has already been computed.
						// Just return the start address for the file name.
				fileNameCPtr = (void*)&mFilePathName[mUnicodePathLength+1];

			else	// mPathLength == 0
				{
				WideFileStringPtr		wFileNameCPtr;

				wFileNameCPtr = &mFilePathName[pathLength];
				
				nameLength = 0;
				#if defined multispec_lin
					while ((nameLength < pathLength) && (*wFileNameCPtr != '/'))
				#endif

				#if defined multispec_win
					while ((nameLength < pathLength) && (*wFileNameCPtr != '\\'))
				#endif
						{
						wFileNameCPtr--;
						nameLength++;
					
						} // end "while ((nameLength < pathLength) && ..."
					
				if (nameLength > 0)
					wFileNameCPtr++;

				mUnicodePathLength = pathLength - nameLength;

				fileNameCPtr = (void*)wFileNameCPtr;

				}	// end "else mPathLength == 0"
				
			}	// end "if (pathLength > 0)"
			
		else	// pathLength == 0
			{ 
					// Make sure that there is a c terminator.
					
			mFilePathName[1] = 0;
			mUnicodePathLength = 0;

			fileNameCPtr = (void*)&mFilePathName[1];
			
			}	// else pathLength == 0	

		}	// end "else returnCode != kReturnASCII"
		
	return (fileNameCPtr);
		
}	// end "GetFileNameCPtr"  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void* GetFileNamePPtr
//
//	Software purpose:	The purpose of this routine is to return the address of
//							the start of the actual file name not including the
//							path. The pointer is returned as a pascal name pointer.
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
//	Revised By:			Larry L. Biehl			Date: 03/16/2017

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
		wideFileNamePtr = (WideFileStringPtr)mPascalFileName;

		if (wideFileNamePtr[0] == 0)
			{
					// Force Pascal File Name to be generated from the full wide (unicode) 
					// path name.

			fileNameCPtr = (wchar_t*)GetFileNameCPtr (kReturnUnicode);
		
			nameLength = (UInt16)wcslen (fileNameCPtr);
			nameLength = MIN (nameLength, 254);
		
			if (nameLength > 0)
				wmemcpy (&mPascalFileName[1], fileNameCPtr, nameLength); 
		
			mPascalFileName[nameLength+1] = 0;
			mPascalFileName[0] = (UInt8)nameLength;
			
			}	// end "if (wideFileNamePtr[0] == 0)"

		fileNamePtr = (void*)mPascalFileName;

		}	// end "else returnCode != kReturnASCII"
		
	return (fileNamePtr); 	
		
}	// end "GetFileNamePPtr"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void* GetFilePathPPtr
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the file name represented by the input file
//							path pointer.
//		
//	Parameters in:		Address to FileInfo structure for file to be closed.
//
//	Parameters out:	None
//
//	Value Returned: 	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/11/1995
//	Revised By:			Larry L. Biehl			Date: 06/19/2017

void* CMFileStream::GetFilePathPPtr (
				SInt16								returnCode)                       
{			
	#if defined multispec_lin
		if (returnCode == kReturnASCII)
			return ((void*)mUTF8PathName); 
		else	// returnCode != kReturnASCII
			return ((void*)mFilePathName);   
	#endif	// ddefined multispec_lin

	#if defined multispec_win
		if (returnCode == kReturnASCII)
			return ((void*)mUTF8PathName); 
		else	// returnCode != kReturnASCII
			return ((void*)mFilePathName);    
	#endif	// defined multispec_win
		
}	// end "GetFilePathPPtr"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void* GetFilePathPtr
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the file name represented by the input file
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
	
#if defined multispec_lin
OSErr	CMFileStream::GetFileType (
				WideFileStringPtr					filePathPtr,
				long int								*fileTypePtr)
{
	SInt16								errCode;
	UInt16								numSuffixChars;
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
		errCode = -1;

	return (errCode);

}	// end "GetFileType"
#endif	// defined multispec_lin


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void* GetFileUTF8PathLength
//
//	Software purpose:	The purpose of this routine is to find the length of just
//							the path name in the full UTF8 formatted file path name.
//		
//	Parameters in:		Address to FileInfo structure for file to be closed.
//
//	Parameters out:	None
//
//	Value Returned: 	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/20/2017
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

UInt16 CMFileStream::GetFileUTF8PathLength ()       
{
	UInt8*								utf8FileNamePtr;
	
	UInt16								fullPathLength,
											nameLength;


	if (mUTF8PathLength == 0)
		{
		utf8FileNamePtr = mUTF8PathName;
		fullPathLength = mUTF8PathName[0];
		utf8FileNamePtr = &mUTF8PathName[fullPathLength];

		nameLength = 0;
		#if defined multispec_lin
			while ((nameLength < fullPathLength) && (*utf8FileNamePtr != '/'))
		#endif
		#if defined multispec_win
			while ((nameLength < fullPathLength) && (*utf8FileNamePtr != '\\'))
		#endif
			{
			utf8FileNamePtr--;
			nameLength++;
			
			} // end "while ((nameLength < pathLength) && ..."
			
		if (nameLength > 0 && fullPathLength >= nameLength)
			mUTF8PathLength = fullPathLength - nameLength;

		else
			mUTF8PathLength = 0;

		}	// end "if (mUTF8PathLength == 0)"
		
	return (mUTF8PathLength);
 
}	// end "GetFileUTF8PathLength"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 09/13/1995
// Todo: For Linux

void CMFileStream:: IOCheck (
				SInt16								errCode)
                
{	
	if (errCode != noErr)
		{            
		#if defined multispec_win
			::IOCheck (errCode, (char*)NULL);
		#endif	// defined multispec_win  
		   	
		}	// end "if (errCode != noErr)" 
		   
}	// end "IOCheck" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
   
	#if defined multispec_lin
		if (IsOpened ())
			Close ();
	#endif
		
}	// end "MCloseFile"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

SInt16 CMFileStream::MCreateNewFile (
				Boolean								replaceFlag)
                      
{			
	SInt16								errCode;
         
   
   		// Make sure the current file is closed.
   		                       
   MCloseFile ();
   
   		// Now open a new file. If the file already exists the size
   		// is changed to 0 bytes.

	#if defined multispec_lin
		if (!replaceFlag && 
			//wxFile::Exists (wxString::FromAscii ((CharPtr)&mFilePathName[1])))
			wxFile::Exists (wxString (&mFilePathName[1])))
																					return (dupFNErr);
	
				// This will overwrite it if it already exists
				
		errCode = MOpenFile ((SInt16)(wxFile::write), (SInt16)kErrorMessages);
		MCloseFile ();
		
				// Now open it in read/write mode
				
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
	
   if (mFilePathName[0] > 0)
		{
				// Make certain that there is a C style terminator at the end.
				
		mFilePathName[mFilePathName[0]+1] = 0;   
		
		TRY
   		{                                           
   		Remove ((LPCTSTR)&mFilePathName[1]);
   	
   		}
   	
		CATCH (CFileException, e)
   		{                     
			errCode = error.m_cause;
				               
			if (messageCode == kErrorMessages)                                      
	   		IOCheck (errCode);
   	
   		errCode = ConvertFileErrorNumber (errCode);
   	
   		}
		END_CATCH 
	           			
		}	// end "if (mFilePathName[0] > 0)"
		
	return (errCode); 
#endif	// defined multispec_win   
   
#if defined multispec_lin 
	SInt16				errCode = noErr;
	
   if (mFilePathName[0] > 0)
		{
				// Make certain that there is a C style terminator at the end.
				
		mFilePathName[mFilePathName[0]+1] = 0;   
		
		                                          
   	bool suc = wxRemoveFile (&mFilePathName[1]);
      if (!suc)
         errCode = -36;
   	 		
		}	// end "if (mFilePathName[0] > 0)"
		
	return (errCode); 
#endif	// defined multispec_lin
		
}	// end "MDeleteFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
			
	#if defined multispec_lin
		 if (!Flush ())
			  continueFlag = FALSE;
	#endif
	
	return (continueFlag);

}	// end "MFlushFile"           



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
		
	#if defined multispec_lin
		 wxFileOffset pos = Tell ();
		 if (pos == wxInvalidOffset)
			  errCode = -1;
			  
		 else
			  *outOffsetPtr=(SInt64)pos;
	#endif
	
	return (errCode);
	
}	// end "MGetMarker"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
		
	#if defined multispec_lin
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 07/05/2017

SInt16 CMFileStream::MOpenFile (
				UInt16								readWriteCode,
				UInt16								messageCode)
                      
{			
	SInt16								errCode = noErr;  
	

	#if defined multispec_lin
				// wxFile does not return any error. It just closes the  file				
				// Make certain that there is a C style terminator at the end.

		mFilePathName[mFilePathName[0]+1] = 0;
		if (readWriteCode == kRead)
			{		
			if (wxFile::Exists (wxString (&mFilePathName[1])))
				{
				if (Open (wxString (&mFilePathName[1]),kRead)) 
					{
					mCreator = -1;
					mFileType = kTEXTFileType;
					open_mode = readWriteCode;
					
					}	// end "if (Open ((CharPtr)inFilePathPtr, ..."

				else // !Open (...
					{
					//if (messageCode == kErrorMessages)
					//    IOCheck (errCode);
					//errCode = ConvertFileErrorNumber (errCode);
					errCode = -1;
					
					}	// end "else !Open (..."
					
				}	// end "if (wxFile::Exists (wxString (&mFilePathName[1])))"
				
			else	// !wxFile::Exists (wxString (&mFilePathName[1]))
				errCode = fnfErr;
				
			}	// end "if (readWriteCode == kRead)"
			
		else if (readWriteCode == kWrite)
			{
			if (Open (wxString (&mFilePathName[1]), kWrite)) 
				{
				mCreator = -1;
				mFileType = kTEXTFileType;
				open_mode = readWriteCode;
				
				}	// end "if (Open ((CharPtr)inFilePathPtr, ..."

			else // !Open (...
				{
				//if (messageCode == kErrorMessages)
				//    IOCheck (errCode);
				//errCode = ConvertFileErrorNumber (errCode);
				errCode = -1;
				
				}	// end "else !Open (..."
				
			}	// end "else if (readWriteCode == kWrite)"
			
		else if (readWriteCode == kReadWrite)
			{
			if (Open (wxString (&mFilePathName[1]), kReadWrite)) 
				{
				mCreator = -1;
				mFileType = kTEXTFileType;
				open_mode = readWriteCode;
			  
				}	// end "if (Open ((CharPtr)inFilePathPtr, ..."

			else	// !Open (...
				{
				//if (messageCode == kErrorMessages)
				//	IOCheck (errCode);
				//errCode = ConvertFileErrorNumber (errCode);
				errCode = -1;
				
				}	// end "else !Open (..."
				
			}	// end "else if (readWriteCode == kReadWrite)"
	#endif	// defined multispec_lin
	
	#if defined multispec_win 	
		CFileException						error;
		
		
		if (m_hFile == CFile::hFileNull)
			{     
					// Make certain that there is a C style terminator at the end.
				
			mFilePathName[mFilePathName[0]+1] = 0;
				  
			if (Open ((LPCTSTR)&mFilePathName[1],    
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		unsigned long int MPeekData
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

	#if defined multispec_lin
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		unsigned long int MReadData
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
	
	#if defined multispec_lin
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
	
	#if defined multispec_lin
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
	
	
	#if defined multispec_lin
		errCode = ftruncate (fd (), countBytes); 
	#endif	// defined multispec_lin

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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		unsigned long int MWriteData
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
      
	#if defined multispec_lin
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
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 06/01/2017

void CMFileStream::SetFileName (
				FileStringPtr						inFileNamePtr)
                     
{			
	WideFileStringPtr					fileNamePtr,
											filePathNamePtr;

	int									sizeNeeded;
	UInt16								inFileNameLength;
	
	#if defined multispec_lin
		wxMBConvUTF8						converter;
	#endif
				
	
	if (inFileNamePtr == NULL)
		{
				// If a path name already exists. Make sure the file stream is
				// fully set up.

		if (mFilePathName[0] == 0 && mUTF8PathName[0] > 0)
			{
					// Need to create the unicode formatted name from the 
					// the existing uft8 formatted string
					
			inFileNamePtr = mUTF8PathName;
			filePathNamePtr = mFilePathName;

			#if defined multispec_lin
				sizeNeeded = converter.ToWChar (NULL, -1, (char*)&mUTF8PathName[1]);
				sizeNeeded = MIN (sizeNeeded, _MAX_PATH-2);
				sizeNeeded = converter.ToWChar (
									&mFilePathName[1], sizeNeeded, (char*)&mUTF8PathName[1]);
			#endif	// end defined multispec_lin

			#if defined multispec_win
				sizeNeeded = MultiByteToWideChar (
										CP_UTF8, 0, (LPCSTR)&mUTF8PathName[1], -1, NULL, 0);
				sizeNeeded = MIN (sizeNeeded, _MAX_PATH-2);
				MultiByteToWideChar (CP_UTF8, 
											0, 
											(LPCSTR)&mUTF8PathName[1], 
											-1, 
											&mFilePathName[1], 
											sizeNeeded);
			#endif	// end defined multispec_win
			
			mFilePathName[0] = (TBYTE)wcslen (&mFilePathName[1]);

					// Force the unicode version of the file name to be generated and
					// the path length (no file name) to be computed.

			GetFileNameCPtr (kReturnUnicode);

					// Now force the multibyte file name to be created if they are 
					// requested.
			
			mPascalFileName[0] = 0;

			mUTF8FileName[0] = 0;
			SetUTF8FileName ();

			}	// end "if (mFilePathName[0] == 0 && mUTF8PathName[0] > 0)"

		}	// end "if (inFileNamePtr == NULL)"
	
	else	// inFileNamePtr != NULL)
		{
		if (mFilePathName[0] == 0 && mUTF8PathName[0] > 0)
			{
					// Add the file to the utf8 path string.

			strcpy ((char*)&mUTF8PathName[mUTF8PathLength+1], (char*)&inFileNamePtr[1]);
			mUTF8PathName[0] = mUTF8PathLength + inFileNamePtr[0];

			}	// end "if (mFilePathName[0] == 0 && mUTF8PathName[0] > 0)"

		else	// Add the file name to the unicode path name.
			{
			fileNamePtr = (WideFileStringPtr)GetFileNameCPtr (kReturnUnicode); 

					// Make sure the file name plus path is less than the max buffer length
			
			inFileNameLength = inFileNamePtr[0];
			inFileNameLength = MIN (inFileNameLength, _MAX_PATH-mUnicodePathLength-2);
			
					// Make sure that the file name is terminated by a c terminator.
					
			inFileNamePtr[inFileNameLength+1] = 0;

					// Need to convert the input file name from multibyte string
					// to wide string to be able to add to the wide (unicoded)
					// full path.

			#if defined multispec_lin
				sizeNeeded = converter.ToWChar (NULL, -1, (char*)&inFileNamePtr[1]);
				sizeNeeded = MIN (sizeNeeded, 256);
				sizeNeeded = converter.ToWChar (
											fileNamePtr, sizeNeeded, (char*)&inFileNamePtr[1]);
			#endif	// defined multispec_lin

			#if defined multispec_win
				sizeNeeded = MultiByteToWideChar (
										CP_UTF8, 0, (LPCSTR)&inFileNamePtr[1], -1, NULL, 0);
				sizeNeeded = MIN (sizeNeeded, 256);
				MultiByteToWideChar (
						CP_UTF8, 0, (LPCSTR)&inFileNamePtr[1], -1, fileNamePtr, sizeNeeded);
			#endif	// defined multispec_win
									
					// Update the length of the file path string.
					
			mFilePathName[0] = (TBYTE)wcslen (&mFilePathName[1]);
					
					// Now force the UTF8 version of the file path name to be updated.

			mUTF8PathName[0] = 0;
			mUTF8FileName[0] = 0;

			}	// end "else add the file name to the unicode path name."

		}	// end "if (inFileNamePtr != NULL)"

	if (mUTF8PathName[0] == 0)
		SetUTF8FilePath ();
		
}	// end "SetFileName"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 11/28/2017

void CMFileStream::SetFilePath (
				#if defined multispec_lin
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
	
	wmemcpy (mFilePathName, 
				inFilePathPtr, 
				(size_t)inFilePathPtr[0]+1);
				
			// Make sure that the path name is terminated by a c terminator.
			
	if (inFilePathPtr[0] < 255)
		mFilePathName[inFilePathPtr[0]+1] = 0;

			// Update the UTF8 version of the path name

	mUTF8PathName[0] = 0;
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

	mUTF8PathName[0] = 0;
	SetUTF8FilePath ();
		
}	// end "SetFilePath"
#endif	// defined multispec_win
*/

#if defined multispec_lin
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 06/23/2017

void CMFileStream::SetFilePathFromCharString (
				StringPtr							inFilePathPtr,
				Boolean								closeFileFlag)
                     
{		
	wxString								wxInFilePath;
	wxWCharBuffer						wideCharBuffer;
	
	size_t								wxInFilePathLength;
	
	wchar_t								wideTempString[256];
	
	
	if (closeFileFlag)
		MCloseFile ();  
		
			// Copy the input file path. Allow for a pascal string length
			
	SInt16 filePathLength = (SInt16)strlen ((char*)inFilePathPtr);
	
	if (filePathLength < 255)
		{
				// Convert the input string to a wide character string.
				
		wxInFilePath = wxString::FromAscii ((char*)inFilePathPtr);
		wideCharBuffer = wxInFilePath.wc_str ();
		wxInFilePathLength = wideCharBuffer.length ();
		
		wmemcpy (&mFilePathName[1], 
					wideCharBuffer.data (), 
					wxInFilePathLength);
					
		mFilePathName[0] = (wchar_t)wxInFilePathLength;
				
			// Make sure that the path name is terminated by a c terminator.
			
		mFilePathName[mFilePathName[0]+1] = 0;

		mUTF8PathName[0] = 0;
		SetUTF8FilePath ();
			
		}	// end "if (filePathLength < 255)"
		
}	// end "SetFilePathFromCharString"  
#endif


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 03/21/2017

void CMFileStream::SetUTF8FileName ()
                     
{			
	FileStringPtr						utf8FileNamePtr;
	
	UInt16								nameLength = 0;


	utf8FileNamePtr = (FileStringPtr)GetFileNameCPtr ();

	if (mUTF8PathName[0] >= mUTF8PathLength)
		nameLength = mUTF8PathName[0] - mUTF8PathLength;

	if (nameLength > 0)
		{
		memcpy (&mUTF8FileName[1], utf8FileNamePtr, nameLength); 

		mUTF8FileName[0] = (UInt8)nameLength;
		mUTF8FileName[nameLength+1] = 0;

		}	// end "if (nameLength > 0)"

	else	// nameLength <= 0
		{
		mUTF8PathLength = 0;
		mUTF8FileName[0] = 0;
		mUTF8FileName[1] = 0;

		}	// end "else nameLength <= 0"

}	// end "SetUTF8FileName"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 06/23/2017

void CMFileStream::SetUTF8FilePath ()
                     
{				
	TBYTE*								filePathCPtr;
	UInt8*								utf8StringPtr;
	
	int									size,
											sizeNeeded;
	
	#if defined multispec_lin
		wxMBConvUTF8						converter;
		wxCharBuffer						utf8PathCharBuffer;
		char*									utf8PathCharBufferPtr;
		size_t								outputSize;
	#endif

	
	if (mUTF8PathName[0] == 0)
		{
		filePathCPtr = &mFilePathName[1];
		utf8StringPtr = &mUTF8PathName[1];
		//std::string utf8 = UTF8FromUTF16 (filePathCPtr);  requires visualc++ 2010
		size = (int)wcslen (filePathCPtr);
		
		#if defined multispec_lin
			UInt8						tempMBPathName[_MAX_PATH+1];
			//UInt8*					mbStringPtr = mUTF8PathName;
			//UInt8						tempMBPathName2[_MAX_PATH+1];
		
			utf8PathCharBuffer = converter.cWC2MB (filePathCPtr, size, &outputSize);
			utf8PathCharBufferPtr = utf8PathCharBuffer.data ();

			if (outputSize > 0)
				{
				memcpy (tempMBPathName, utf8PathCharBufferPtr, outputSize);
				tempMBPathName[outputSize] = 0;
				CtoPstring (tempMBPathName, tempMBPathName);
				strncpy ((char*)mUTF8PathName, (char*)tempMBPathName, outputSize+1);
				mUTF8PathName[outputSize+1] = 0;
				
				}	// end "if (outputSize > 0)"
				
			sizeNeeded = outputSize;
		#endif	// defined multispec_lin

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

				// Add pascal length
				
		mUTF8PathName[0] = sizeNeeded;

				// Add the c termination character
			
		mUTF8PathName[sizeNeeded+1] = 0;

		SetUTF8FileName ();

		}	// end "if (fileStreamPtr->mUTF8PathName[0] == 0)"

}	// end "SetUTF8FilePath"
