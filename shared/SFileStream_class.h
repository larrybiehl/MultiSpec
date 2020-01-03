//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2020)
//						(c) Purdue Research Foundation
//								All rights reserved.
//
//	File:						SFileStream_class.h
//	Implementation:		SFileStream_class.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux, MacOS and Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMFileStream class
//								Example code from c1993 Metrowerks Inc. All rights reserved.
//
//------------------------------------------------------------------------------------

#if !defined __MFILESTR_H__
	#define __MFILESTR_H__ 
	
#define	kUndefined								-1

#if defined multispec_win 
	#define	kRead									CFile::modeRead|CFile::typeBinary
	#define	kWrite								CFile::modeWrite|CFile::typeBinary
	 
	class	CMFileStream : public CFile  
#endif	// defined multispec_win 
	
#if defined multispec_wx
	#include "SMultiSpec.h"
	#include "wx/file.h"
	#include "wx/string.h"
	#include "wx/filename.h"
	#include "wx/utils.h"
	#include "wx/filefn.h"
	#define	kRead				wxFile::read//|CFile::typeBinary
	#define	kWrite			wxFile::write//|CFile::typeBinary
	#define	kReadWrite     wxFile::read_write

	class CMFileStream : public wxFile
#endif	// defined multispec_wx
		{
		public:
			CMFileStream ();
		
			CMFileStream (
				CMFileStream						*inputFileStreamPtr);
		
			CMFileStream (
				WideFileStringPtr					inFilePathPtr);
									
			virtual ~CMFileStream();
			
			static void DisposeFilePath(
				WideFileStringPtr					inFilePathPtr);
										
			Boolean FileOpen (void);
			
			void* GetCopyOfFilePath(void);
			
			static void* GetCopyOfFilePath(
				WideFileStringPtr					inFilePathPtr);
			
			void* GetFileNameCPtr (
				SInt16			returnCode=kDefaultFileStringCode);
			
			void* GetFileNamePPtr (
				SInt16			returnCode=kDefaultFileStringCode);
			
			void* GetFilePathPPtr (
				SInt16			returnCode=kDefaultFileStringCode);
			
			static void* GetFilePathPtr (
				FileStringPtr						inFilePathPtr);
			
			#if defined multispec_win
				static OSErr GetFileType (
					TBYTE*								filePathPtr,
					long int								*fileTypePtr);
			#endif	// defined multispec_win
			
			#if defined multispec_wx
				static OSErr GetFileType (
					WideFileStringPtr					filePathPtr,
					long int								*fileTypePtr);
			#endif	// defined multispec_wx
			
			UInt16 GetFileUTF8PathLength (void);
			
			void IOCheck (
				SInt16								errCode);
			
			void MCloseFile (void);
			
			SInt16 MCreateNewFile (
				Boolean								replaceFlag);
			
			SInt16 MDeleteFile (
				UInt16								messageCode);
			
			Boolean MFlushFile (void);
			
			SInt16 MGetMarker(
				SInt64*								outOffsetPtr,
				SInt16								messageCode);
			
			SInt16 MGetSizeOfFile (
				SInt64								*countPtr);
										
			SInt16 MOpenFile (
				UInt16								readWriteCode,
				UInt16								messageCode);
										
			SInt16 MPeekData (
				void									*outBuffer,
				UInt32								*numberBytesPtr);
										
			SInt16 MReadData (
				void									*outBuffer,
				UInt32								*numberBytesPtr,
				SInt16								messageCode);
				 
			SInt16 MSetMarker(
				SInt64								inOffset,
				SInt16								nfrom,
				SInt16								messageCode);
										
			SInt16 MSetSizeOfFile (
				SInt64								countBytes);
										            
			SInt16 MWriteData (
				void									*inBufferPtr,
				UInt32								*numberBytesPtr,
				SInt16								messageCode);
										
			void SetFileName (
				FileStringPtr						inFileNamePtr);
										
			void SetFilePath (
				WideFileStringPtr					inFilePathPtr,
				Boolean								closeFileFlag);
										
			void SetFilePathFromCharString (
				StringPtr							inFilePathPtr,
				Boolean								closeFileFlag);

			void SetUTF8FileName (void);

			void SetUTF8FilePath (void);
			
			long int								mCreator,
													mFileType;

			int									mUnicodePathLength,
													mUTF8PathLength;

			UInt32								mFileSize;
			
			#if defined multispec_win
				TBYTE						mFilePathName[_MAX_PATH];

				UInt8						mUTF8PathName[_MAX_PATH];
						// mUTF8FileName is the UTF8 formatted version
						// of just the file name
				UInt8						mUTF8FileName[_MAX_PATH];
			#endif	// defined multispec_win 
									         
			#if defined multispec_wx
				wchar_t					mFilePathName[_MAX_PATH+1];
				long						m_spaceFiller1;
				UInt8						mUTF8PathName[_MAX_PATH+1];
				long						m_spaceFiller2;
				UInt8						mUTF8FileName[_MAX_PATH+1];
			#endif	// defined multispec_wx
									
		protected:
			SInt16 ConvertFileErrorNumber (
				SInt16								errCode);
										
			#if defined multispec_wx
					// buffer pascal version of file             
				wchar_t								mPascalFileName[_MAX_PATH+1];
				unsigned int						open_mode;
			#endif	// defined multispec_wx
		
			#if defined multispec_win
					// buffer pascal version of file  
				TBYTE									mPascalFileName[_MAX_PATH];
			#endif	// defined multispec_win
			
		};	// end "class CMFileStream"

#endif // !defined __MFILESTR_H__             
