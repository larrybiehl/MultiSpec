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
//	File:						SDefines.h
//
//	Authors:					Larry L. Biehl
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	Header file for MultiSpec which contains variable and
//								structure definitions used by MultiSpec.
//
//	Written By:				Larry L. Biehl			Date: 03/29/1988
//	Revised By:				Abdur Maud				Date: 06/24/2013
//	Revised By:				Tsung Tai Yeh			Date: 09/25/2015
//	Revised By:				Larry L. Biehl			Date: 08/25/2020
//	
//------------------------------------------------------------------------------------

#pragma once

#if include_gdal_capability
	#if defined multispec_wxlin
		#include "dbfopen.h"
	#endif	// multispec_wxlin

	#if defined multispec_wxmac
		#include "shapefil.h"
	#endif	// multispec_mac
	
	#ifdef multispec_mac
		#include "shapefil.h"
		//#include "dbfopen.h"
	#endif	// multispec_mac
	
	#ifdef multispec_win
		#include "shapefil.h"
	#endif	// multispec_win
#endif	// include_gdal_capability
	
		// Forward definition of classes
#if defined multispec_wx
	#include "wx/wx.h"
	#include "wx/listbox.h"
	#include "wx/dc.h"
	#include "wx/listctrl.h"
	//#include "SGraphView.h"
	#include "SConstants.h"

	class CMDialog;
	class CMPalette;
	class CMTextView;
	class CMDisplay;
	class CMImageWindow;
	class CMWindowInfo;
	class CMImageDoc;
	class CMImageFrame;
	class CMImageView;
	class CMLegendView;
	class CMLegendList;
	class CMGraphDoc;
	class CMGraphFrame;
	class CMGraphView;
	class CMStatisticsDoc;
	class CMStatisticsView;
	class CMStatisticsFrame;
	
	#ifndef _T
		#define _T(x)     x
	#endif
	#define A2T(x)		x
	#define T2A(x)		x

	#define LPSTR		char*
	#define LPTSTR		char*
	#define LPCSTR		char*
	#define LPCWSTR	wchar_t*
	
	//#include "dlib/svm_threaded.h"
	//typedef dlib::matrix<double,0,1> column_vector;
	//typedef dlib::matrix<double,0,1> sample_type;
#endif

#ifndef multispec_wx
	class CCmdUI;

	class	CDocument;
		
	// class CMDataFile; 
	class	CMDialog;
	class	CMDisplay;
	class CMFileDialog;
	class CMHistogram; 
	class CMImageWindow;
	class CMMemoryUtilities;
	class CMOutlineArea;
	class CMPalette;   
	class CMWindowInfo;

	class CMImageDoc; 
	class	CMImageFrame;
	class	CMImageView;

	class CMStatisticsForm; 

	class CMTextDoc;
	class CMTextView;

	class CMGraphDoc;
	class CMGraphView;
#endif
		// The forward definitions for the structures.
                                      
typedef struct BlockFormat BlockFormat, *BlockFormatPtr;
typedef struct ChannelStatistics ChannelStatistics, *ChannelStatisticsPtr; 
typedef struct ClusterType ClusterType;
typedef struct ControlPoints ControlPoints, *ControlPointsPtr;                             
typedef struct DataToDisplayLevel DataToDisplayLevel, *DisplayLevelPtr;
typedef struct DialogSelectArea DialogSelectArea, *DialogSelectAreaPtr; 
typedef struct DisplaySpecs DisplaySpecs, *DisplaySpecsPtr;
typedef struct DistancesSummary DistancesSummary, *DistancesSummaryPtr;
typedef struct DoubleRect DoubleRect, *DoubleRectPtr;
typedef struct FeatureExtractionSpecs FeatureExtractionSpecs, *FeatureExtractionSpecsPtr;
typedef struct MFileInfo MFileInfo, *FileInfoPtr, **FileInfoHandle;
typedef struct FileIOBuffer FileIOBuffer;
typedef struct GeodeticModelInfo GeodeticModelInfo, *GeodeticModelInfoPtr; 
typedef struct GridCoordinateSystemInfo GridCoordinateSystemInfo, *GridCoordinateSystemInfoPtr;
typedef struct HierarchalFileFormat HierarchalFileFormat, *HierarchalFileFormatPtr;
typedef struct HistogramSpecs HistogramSpecs, *HistogramSpecsPtr;
typedef struct HistogramSummary HistogramSummary, *HistogramSummaryPtr;
typedef struct LayerInfo LayerInfo, *LayerInfoPtr; 
typedef struct MaskInfo MaskInfo, *MaskInfoPtr; 
typedef struct	PlanarCoordinateSystemInfo	PlanarCoordinateSystemInfo, *PlanarCoordinateSystemInfoPtr;  
typedef struct ProjectClassNames ProjectClassNames, *PClassNamesPtr;
typedef struct ProjectFieldIdentifiers ProjectFieldIdentifiers, *PFieldIdentifiersPtr;
typedef struct ProjectFieldPoints ProjectFieldPoints, *PFieldPointsPtr;
typedef struct ProjectInfo ProjectInfo, *ProjectInfoPtr; 
typedef struct ProjectionPursuitSpecs ProjectionPursuitSpecs, *ProjectionPursuitSpecsPtr;
typedef struct RecodeThematicImage RecodeThematicImage, *RecodeThematicImagePtr;
typedef struct TransformationSpecs TransformationSpecs, *TransformationSpecsPtr;
typedef struct TransMapMatrix TransMapMatrix, *TransMapMatrixPtr;
typedef struct TypicalitySummary TypicalitySummary, *TypicalitySummaryPtr;
typedef struct WindowInfo WindowInfo, *WindowInfoPtr;
typedef struct WorkFlowInfo WorkFlowInfo, *WorkFlowInfoPtr;


typedef char*				CharPtr;

typedef double*			CovarianceStatisticsPtr;
	
typedef double				CovarianceStatistics;
	
// typedef long int 			CType;
typedef double 			CType;

typedef double 			DoubleVector[1];
	
typedef double				FldLik;

typedef long int 			LongIntVector[1];

typedef short int	 		ShortIntVector[1];
	
typedef double				SumSquaresStatistics;
	
typedef double*			SumSquaresStatisticsPtr;

typedef unsigned char*	UCharPtr; 

typedef double				CMeanType;
typedef double				CMeanType2;


#if defined multispec_mac_swift
	typedef struct CMFileStream CMFileStream, *CMFileStreamPtr;
	#define _T(x)			x
	#define A2T(x)			x
	#define T2A(x)			x

	#define LPSTR			char*
	#define LPTSTR			char*
	#define LPCSTR			char*
	#define LPCWSTR		wchar_t*

	enum
		{
		blackColor                    = 33,   /* colors expressed in these mappings */
		whiteColor                    = 30,
		redColor                      = 205,
		greenColor                    = 341,
		blueColor                     = 409,
		cyanColor                     = 273,
		magentaColor                  = 137,
		yellowColor                   = 69
		};

	typedef int							CMPaletteInfo;
	typedef Ptr							CursPtr;
	typedef Ptr							DlgHookUPP;
	typedef double						SDouble;
	typedef Handle						CListBox;
	typedef UInt32						SFReply;
	typedef UInt32						THPrint;
	typedef SInt32						CDC;
	typedef struct HParamBlockRec
		{
		UInt32				ioPosOffset;
		CMFileStream*		fileStreamPtr;
		UCharPtr				ioBuffer;
		UInt32				ioReqCount;
		UInt32				ioActCount;
		SInt16				ioPosMode;
		
		} HParamBlockRec;

			typedef Ptr							HPtr;

	typedef char*						HCharPtr;

	typedef unsigned char*			HUCharPtr; 
	
	typedef SInt8*						HSInt8Ptr; 
	
	typedef UInt8*						HUInt8Ptr;
	
	typedef SInt16*					HSInt16Ptr; 
	
	typedef UInt16*					HUInt16Ptr;
	
	typedef SInt32*					HSInt32Ptr; 
	
	typedef UInt32*					HUInt32Ptr;
	
	typedef SInt64*					HSInt64Ptr;
	
	typedef float*						HFloatPtr;
	
	typedef FileIOBuffer*			HFileIOBufferPtr;
	
	typedef double*					DoublePtr;	
	
	typedef double*					HDoublePtr;	
	
	typedef double*					HFldLikPtr;	

	typedef double*					HCovarianceStatisticsPtr; 
	
	typedef double*					HSumSquaresStatisticsPtr;
	
	typedef ChannelStatisticsPtr	HChannelStatisticsPtr;

	typedef int							PaletteHandle;

	typedef ListHandle				LegendListHandle;
	
	typedef PClassNamesPtr			HPClassNamesPtr;
	
	typedef PFieldIdentifiersPtr	HPFieldIdentifiersPtr;
	
	typedef PFieldPointsPtr			HPFieldPointsPtr;
	
	typedef CMeanType*				HCMeanTypePtr;
	
	typedef CType*						HCTypePtr;
	
	typedef unsigned char 			UChar; 

	typedef UInt8*						FileStringPtr;

	typedef wchar_t					LocalAppFile;
	typedef LocalAppFile*			LocalAppFilePtr;
	
	typedef pascal void				PascalVoid;
	
	typedef WindowPtr					StatisticsWindowPtr;
	
	typedef Str255						CString;

	typedef Rect						tagRECT;
	
	typedef UInt32						SFTypeList;
	
	typedef struct LongPoint 
		{
		SInt32			v;
		SInt32			h;
		
		} LongPoint; 
	

	typedef struct ULongRect
		{                                           
		UInt32		top;			// "left" of rectangle
		UInt32		left; 			// "top" of rectangle 
		UInt32		bottom;		// "right" of rectangle 
		UInt32		right;		// "bottom" of rectangle
	
		} ULongRect, *ULongRPtr;
		
	typedef struct CMFileStream
		{
		HFSUniStr255			uniFileName;
		UniChar					uniPascalFileName[256];
		Str255					fileName;
		FSRef						fsRef;
		FSRef						parentFSRef;
		SInt64					fileSize;
		SInt32					parID;
		SInt32					creator;
		SInt32					type;
		UInt16					pathLength;
		SInt16					refNum;
		SInt16					vRefNum;
		Boolean					fSSpecFlag;
		Boolean					fSRefFlag;
		
		} CMFileStream, *CMFileStreamPtr, **CMFileStreamHandle;
#endif	// defined multispec_mac_swift

#if defined multispec_mac
	typedef struct CMFileStream CMFileStream, *CMFileStreamPtr;
	class	CMMemoryDialog;
	class	CMMenus;    

	#if !defined PP_Target_Carbon
		#define fabs(x) std::fabs(x)
	#endif

	#define _T(x)      x
	#define A2T(x)		x
	#define T2A(x)		x

	#define LPSTR		char*
	#define LPTSTR		char*
	#define LPCSTR		char*
	#define LPCWSTR		wchar_t*

			// These are a holder variables for Windows structures.

	typedef PaletteHandle			CMPaletteInfo;

	typedef SInt32						CDC;

	typedef Rect						tagRECT;
	
	typedef Handle						HDC;
	
	typedef unsigned char 			UChar; 

	typedef UInt8*						FileStringPtr;

	#if defined __MWERKS__
		typedef short double 			SDouble;
	#else
		typedef double						SDouble;
	#endif	// defined __MWERKS__, else... 
	
	typedef pascal void				PascalVoid;
	
	typedef WindowPtr					StatisticsWindowPtr;
	
			// Equivalence to huge pointers in the MS Windows system
					
	typedef Ptr							HPtr;

	typedef char*						HCharPtr;

	typedef unsigned char*			HUCharPtr; 
	
	typedef SInt8*						HSInt8Ptr; 
	
	typedef UInt8*						HUInt8Ptr;
	
	typedef SInt16*					HSInt16Ptr; 
	
	typedef UInt16*					HUInt16Ptr;
	
	typedef SInt32*					HSInt32Ptr; 
	
	typedef UInt32*					HUInt32Ptr;
	
	typedef SInt64*					HSInt64Ptr;
	
	typedef float*						HFloatPtr;
	
	typedef FileIOBuffer*			HFileIOBufferPtr;
	
	typedef double*					DoublePtr;	
	
	typedef double*					HDoublePtr;	
	
	typedef double*					HFldLikPtr;	

	typedef double*					HCovarianceStatisticsPtr; 
	
	typedef double*					HSumSquaresStatisticsPtr;
	
	typedef ChannelStatisticsPtr	HChannelStatisticsPtr;  
	
	typedef PClassNamesPtr			HPClassNamesPtr;
	
	typedef PFieldIdentifiersPtr	HPFieldIdentifiersPtr;
	
	typedef PFieldPointsPtr			HPFieldPointsPtr;
	
	typedef CMeanType*				HCMeanTypePtr;
	
	typedef CType*						HCTypePtr;

	typedef ListHandle				LegendListHandle;
	
	typedef Str255						CString;
	
	#ifndef _LongCoords_
		#include "LongCoords.h"
	#endif
	
	typedef struct LongPoint 
		{
		SInt32			v;
		SInt32			h;
		
		} LongPoint; 
	

	typedef struct ULongRect
		{                                           
		UInt32		top;			// "left" of rectangle
		UInt32		left; 			// "top" of rectangle 
		UInt32		bottom;		// "right" of rectangle 
		UInt32		right;		// "bottom" of rectangle
	
		} ULongRect, *ULongRPtr; 
		
		
				// This structure contains the table to convert the image data		
				// values to the palette display values.  The DataToDisplayLevel	
				//	structure describes this structure.										
		
	typedef struct DataDisplay
		{
		union {
				int				twobyte[1500];	// Vector for a line of 16 bit data 
				unsigned	char	onebyte[3000];	// Vector for a line of  8 bit data 
				} toLevel;
		} DataDisplay, *DataDisplayPtr; 
	

	//#if PRAGMA_ALIGN_SUPPORTED
	#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
	#endif
		
			// The following is for documentation. The beginning of the 
			// CMFileStream structure should be like the FSSpec structure.

			//	struct FSSpec {
			//			short							vRefNum;
			//			long							parID;
			//			Str63							name; };
	/*
			// The following was used before the conversion to using Unicode.
	typedef struct CMFileStream
		{
		SInt16					vRefNum;
		SInt32					parID;
		Str255					fileName;
		SInt32					creator;
		SInt32					type;
		SInt64					fileSize;
		SInt16					refNum;
		Boolean					fSSpecFlag;
		
		FSRef						fsRef;
		FSRef						parentFSRef;
		HFSUniStr255			uniFileName;
		UniChar					uniPascalFileName[256];
		UInt16					pathLength;
		Boolean					fSRefFlag;
		
		} CMFileStream, *CMFileStreamPtr, **CMFileStreamHandle;
	*/
	typedef struct CMFileStream
		{
		HFSUniStr255			uniFileName;
		UniChar					uniPascalFileName[256];
		Str255					fileName;
		FSRef						fsRef;
		FSRef						parentFSRef;
		SInt64					fileSize;
		SInt32					parID;
		SInt32					creator;
		SInt32					type;
		UInt16					pathLength;
		SInt16					refNum;
		SInt16					vRefNum;
		Boolean					fSSpecFlag;
		Boolean					fSRefFlag;
		
		} CMFileStream, *CMFileStreamPtr, **CMFileStreamHandle;
	#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
	#endif 
	

	#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
	#endif
	/*
			// The following was used before the conversion to using Unicode.
	typedef struct LocalAppFile
		{
		SInt16					vRefNum;
		SInt32					parID;
		UInt8						fileName[256];
		SInt32					creator;
		SInt32					type;
		SInt64					fileSize;
		SInt16					refNum;
		Boolean					fSSpecFlag;
		UInt8*					fNamePtr;
		
		FSRef						fsRef;
		FSRef						parentFSRef;
		HFSUniStr255			uniFileName;
		UniChar					uniPascalFileName[256];
		UInt16					pathLength;
		Boolean					fSRefFlag;
		
		} LocalAppFile, *LocalAppFilePtr;
	*/
	typedef struct LocalAppFile
		{
		HFSUniStr255			uniFileName;
		UniChar					uniPascalFileName[256];
		UInt8						fileName[256];
		FSRef						fsRef;
		FSRef						parentFSRef;
		SInt64					fileSize;
		UInt8*					fNamePtr;
		SInt32					parID;
		SInt32					creator;
		SInt32					type;
		UInt16					pathLength;
		SInt16					refNum;
		SInt16					vRefNum;
		Boolean					fSSpecFlag;
		Boolean					fSRefFlag;
		
		} LocalAppFile, *LocalAppFilePtr;

	#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
	#endif
	

		// Macro definitions.												
		
		// 	Draw a drop shadow box.																		

	#define DrawDropShadowBox(box)																\
		{																									\
																											\
		PenSize (1,1);																					\
		box.right--;																					\
		box.bottom--;																					\
		FrameRect (&box);																				\
		MoveTo (box.right, box.top+2);															\
		LineTo (box.right, box.bottom);															\
		LineTo (box.left+2, box.bottom);															\
																											\
		}		// end Macro "DrawDropShadowBox(boxPtr)"	
		
	#if !defined PP_Target_Carbon
		typedef UInt16			PMOrientation;
		#define kPMPortrait	1
	#endif	// !defined PP_Target_Carbon
		
		typedef struct CGInfo
			{
			CGContextRef			contextRef;
			CGImageRef				imageRef;
			UInt32					contextRowBytes;
			
			} CGInfo, *CGInfoPtr;
			
		typedef CGColorSpaceRef 	(*CGColorSpaceCreateDeviceGrayPtr)(void);
																  
		typedef CGColorSpaceRef 	(*CGColorSpaceCreateDeviceRGBPtr)(void);
																  
		typedef CGColorSpaceRef 	(*CGColorSpaceCreateIndexedPtr)(
												CGColorSpaceRef        			baseSpace,
  												size_t                 			lastIndex,
  												const unsigned char *  			colorTable);
																  
		typedef void 					(*CGColorSpaceReleasePtr)(
												CGColorSpaceRef					colorSpace);
												
		typedef void					(*CGContextAddLineToPointPtr)(
											  CGContextRef   						ctx,
											  float          						x,
											  float          						y);
												
		typedef void					(*CGContextBeginPathPtr)(
											  CGContextRef   						ctx);
		
		typedef void 					(*CGContextClipToRectPtr)(
												CGContextRef   					ctx,
												CGRect         					rect);
		
		typedef void 					(*CGContextDrawImagePtr)(
												CGContextRef   					ctx,
												CGRect         					rect,
												CGImageRef     					image);
											  
		typedef void					(*CGContextFlushPtr)(
												CGContextRef   					context);
												
		typedef void					(*CGContextMoveToPointPtr)(
											  CGContextRef   						ctx,
											  float          						x,
											  float          						y);
											  
		typedef void					(*CGContextReleasePtr)(
												CGContextRef   					context);
												
		typedef void					(*CGContextRestoreGStatePtr)(
												CGContextRef 						context);
												
		typedef void					(*CGContextSaveGStatePtr)(
												CGContextRef 						context);
												
		typedef void					(*CGContextSetLineWidthPtr)(
											  CGContextRef   						ctx,
											  float          						width);
															
		typedef void					(*CGContextScaleCTMPtr)(
												CGContextRef   					context,
												float          					sx,
												float          					sy);
															
		typedef void					(*CGContextSetAlphaPtr)(
											  CGContextRef   						ctx,
											  float         						alpha);
															
		typedef void					(*CGContextSetInterpolationQualityPtr)(
											  CGContextRef             		c,
											  CGInterpolationQuality   		quality);
															
		typedef void					(*CGContextSetRGBFillColorPtr)(
											  CGContextRef   						ctx,
											  float          						red,
											  float          						green,
											  float          						blue,
											  float          						alpha);
															
		typedef void					(*CGContextSetShouldAntialiasPtr)(
												CGContextRef   					context,
												int            					shouldAntialias);
												
		typedef void					(*CGContextStrokePathPtr)(
											  CGContextRef   						ctx);
			
		typedef void					(*CGContextTranslateCTMPtr)(
												CGContextRef   					ctx,
												float          					tx,
												float          					ty);
		
		#if !TARGET_RT_MAC_MACHO														  
			typedef CGDataProviderRef 	(*CGDataProviderCreateWithDataPtr)(
													void*                 			info,
													const void*           			data,
													size_t                 			size,
													CGReleaseDataProcPtr  			releaseData);
		#else														  
			typedef CGDataProviderRef 	(*CGDataProviderCreateWithDataPtr)(
													void*                 			info,
													const void*           			data,
													size_t                 			size,
													CGDataProviderReleaseDataCallback  			releaseData);
		#endif
												
		typedef void 					(*CGDataProviderReleasePtr)(
												CGDataProviderRef					provider);
											  
		typedef CGImageRef			(*CGImageCreatePtr)(
												size_t                   		width,
												size_t                   		height,
												size_t                   		bitsPerComponent,
												size_t                   		bitsPerPixel,
												size_t                  		bytesPerRow,
												CGColorSpaceRef          		colorspace,
												CGImageAlphaInfo         		alphaInfo,
												CGDataProviderRef        		provider,
												const float              		decode[],
												int                      		shouldInterpolate,
												CGColorRenderingIntent   		intent);
																  
		typedef size_t 				(*CGImageGetBytesPerRowPtr)(
												CGImageRef							cgImage);
																  
		typedef size_t 				(*CGImageGetHeightPtr)(
												CGImageRef							cgImage);
																  
		typedef size_t 				(*CGImageGetWidthPtr)(
												CGImageRef							cgImage);
											  
		typedef CGImageRef			(*CGImageMaskCreatePtr)(
												size_t                   		width,
												size_t                   		height,
												size_t                   		bitsPerComponent,
												size_t                   		bitsPerPixel,
												size_t                  		bytesPerRow,
												CGDataProviderRef        		provider,
												const float              		decode[],
												int                      		shouldInterpolate);
																  
		typedef void 					(*CGImageReleasePtr)(
												CGImageRef							cgImage);
							
		typedef CGRect					(*CGRectMakePtr)(
												float									x,
												float									y,
												float									width,
												float									height);
																  
		typedef OSStatus				(*QDBeginCGContextPtr)(
											  CGrafPtr        					inPort,
											  CGContextRef*  						outContext);
																  
		typedef OSStatus				(*QDEndCGContextPtr)(
											  CGrafPtr        					inPort,
											  CGContextRef*  						inoutContext);
											  
#endif	// defined multispec_mac

#if defined multispec_wx
	#define  huge
	#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
	#define kPMPortrait	1
	#define _MAX_PATH 1024
	#define _MAX_FILE 256
      
			// The forward definitions for linux specific classes.

	class CMFileStream;

			// Other definitions.

	typedef CMPalette*			CMPaletteInfo;

	typedef unsigned char		Byte, UChar, UInt8;

	typedef unsigned char		*StringPtr; //, **StringHandle;
	typedef UInt8*					FileStringPtr;
	typedef wchar_t				TBYTE;
	typedef wchar_t*				WideFileStringPtr;

	typedef char					SignedByte, SInt8;
	//typedef long unsigned int 	UINT;
	typedef unsigned short		UInt16;
	typedef signed short			SInt16;
	typedef unsigned int			UInt32;
	typedef signed int			SInt32;
	//typedef __int64				SInt64;
	typedef long long				SInt64;
	typedef unsigned long long UInt64;
	typedef char*					Ptr;
	typedef double					SDouble;
	typedef void					PascalVoid;
	typedef unsigned int			DWORD;
	typedef unsigned short		WORD;

			// Define some variables that are defined in the Macintosh Toolbox and not
			// in the Linux compiler

	typedef SInt16					Boolean; // BOOLean (0 or !=0)

	typedef wxSize					CSize;
	//typedef wxListView			CListBox;	// original
	//typedef wxListBox				CListBox;

			// Define some variables for MultiSpec
			//**** Using GLOBALHANDLE type to be void for now
	typedef void*					GLOBALHANDLE;
	#ifndef multispec_wxmac
		typedef UInt32					CGContextRef;
	#endif
	typedef UInt32					CGRect;
	typedef UInt32					AEAddressDesc;
	typedef UInt32					AEDesc;
	typedef UInt32					AppFile;
	typedef UInt32					SFTypeList;
	typedef Ptr						CGrafPtr;
	typedef Ptr						ControlActionUPP;
	typedef GLOBALHANDLE			ControlHandle; // Windows global handle
	typedef GLOBALHANDLE			ControlRef; 
	typedef Ptr						CursPtr;
	typedef Ptr						DlgHookUPP;
	//	typedef Ptr					DialogPtr;
	typedef CMDialog*				DialogPtr;

	typedef Ptr						DragGrayRgnUPP;
	typedef Ptr						GrafPtr; // Pointer
	typedef Ptr						GWorldPtr; // Pointer
	typedef GLOBALHANDLE			Handle; // Windows global handle
	typedef wxListView*			LegendListHandle;
	typedef wxListBox*			ListHandle;
	typedef GLOBALHANDLE			MenuHandle;
	typedef UInt16					MenuItemIndex;
	#ifndef multispec_wxmac
		typedef GLOBALHANDLE			MenuRef;
	#endif
	typedef GLOBALHANDLE			PixMapHandle;
	typedef UInt16					PMOrientation;
	typedef GLOBALHANDLE			StringHandle;
	typedef Ptr						ModalFilterUPP;
	typedef Handle					WSHandle;
	typedef Handle					WEReference;
	typedef wxDC					CDC;
	typedef wxUpdateUIEvent		CCmdUI;

	typedef struct EventRecord 
		{
		SInt16 modifiers;
		SInt16 event;

		} EventRecord;

	typedef struct FSRef 
		{
		//UInt8    hidden[_MAX_PATH];
		wchar_t    hidden[_MAX_PATH];
		} FSRef;

	typedef struct Point 
		{
		SInt16 h;
		SInt16 v;
		} Point;

	typedef struct Cell 
		{
		SInt16 h;
		SInt16 v;
		} Cell;

	typedef struct LongPoint 
		{
		SInt32 v;
		SInt32 h;
		} LongPoint;

	struct Rect 
		{ // rc
		int left;
		int top;
		int right;
		int bottom;
		};
	typedef Rect tagRECT;
	typedef Rect RECT;

	typedef struct LongRect 
		{
		SInt32 top; // "top" of rectangle
		SInt32 left; // "left" of rectangle
		SInt32 bottom; // "bottom" of rectangle
		SInt32 right; // "right" of rectangle
		} LongRect, *LongRPtr;

	typedef struct ULongRect 
		{
		UInt32 top; // "top" of rectangle
		UInt32 left; // "left" of rectangle
		UInt32 bottom; // "bottom" of rectangle
		UInt32 right; // "right" of rectangle
		} ULongRect, *ULongRPtr;

	struct RGBColor 
		{
		UInt16 red;
		UInt16 green;
		UInt16 blue;
		};
	typedef struct RGBColor RGBColor, *RGBColorPtr, **RGBColorHdl;

	struct RGB8BitColor
		{
		UInt8 red;
		UInt8 green;
		UInt8 blue;
		};
	typedef struct RGB8BitColor RGB8BitColor, *RGB8BitColorPtr;

	struct ColorSpec 
		{
		SInt16 value;
		RGBColor rgb;
		};
	typedef ColorSpec CSpecArray[1];

	struct ColorTable 
		{
		long ctSeed; /*unique identifier for table*/
		short ctFlags; /*high bit: 0 = PixMap; 1 = device*/
		short ctSize; /*number of entries in CTTable*/
		CSpecArray ctTable; /*array [0..0] of ColorSpec*/
		};
	typedef struct ColorTable ColorTable, *CTabPtr;

	struct Palette 
		{
		short pmEntries; /*entries in pmTable*/
		short pmDataFields[7]; /*private fields*/
		RGBColor pmInfo[1];
		};
	typedef struct Palette Palette, *PalettePtr;

			//	The following structure is filler for text windows. This is the structure
			//	that is used in the Mac OS

	typedef struct 
		{
		char string[48];
		} TextBlock, TextArray[1];

	typedef struct 
		{
		char string[24];
		} LineRec, LineArray[1];

	typedef GLOBALHANDLE CTabHandle;
	/********************************************/
			// Setting PaletteHandle to type int for Linux
			// This is just used to check if Palette exists so will give this a number of 0 or 1
	typedef int PaletteHandle;

	typedef wxRegion* RgnHandle;
	typedef unsigned char Str255[256], Str63[64], Str31[32], Str15[16];
	typedef const unsigned char *ConstStr255Param;
	typedef ConstStr255Param ConstStr63Param, ConstStr32Param, ConstStr31Param, ConstStr27Param, ConstStr15Param;

	typedef Ptr UserItemProcPtr;
	typedef Ptr UserItemUPP;
	typedef Ptr DlgHookYDUPP;
	typedef CMImageView*		WindowPtr;
	
	typedef WindowPtr			StatisticsWindowPtr;


	typedef char LWindow;

	typedef SInt16 OSErr;

	typedef UInt32 FourCharCode;
	typedef UInt32 DateTimeRec;
	typedef UInt32 FInfo;
	typedef UInt32 FSSpec;
	typedef UInt32 IOCompletionUPP;
	typedef UInt32 ListClickLoopUPP;
	typedef wchar_t LocalAppFile;
	typedef UInt32 Pattern;
	typedef UInt32 SFReply;
	typedef UInt32 THPrint;
	typedef UInt32 ThreadID;
	typedef UInt32 ThreadTaskRef;
	typedef UInt32 WSEScrollActionUPP;
	typedef UInt32 WEScrollUPP;

	typedef FourCharCode OSType;

			// Declare huge pointers which were used by older version of Windows

	typedef char huge * HPtr;
	typedef char huge * HCharPtr;
	typedef UChar huge * HUCharPtr;
	typedef SInt8 huge * HSInt8Ptr;
	typedef UInt8 huge * HUInt8Ptr;
	typedef SInt16 huge * HSInt16Ptr;
	typedef UInt16 huge * HUInt16Ptr;
	typedef SInt32 huge * HSInt32Ptr;
	typedef UInt32 huge * HUInt32Ptr;
	typedef SInt64 huge * HSInt64Ptr;
	typedef float huge * HFloatPtr;
	typedef FileIOBuffer* HFileIOBufferPtr;

	typedef double huge * DoublePtr;
	typedef double huge * HDoublePtr;
	typedef double huge * HFldLikPtr;
	typedef double huge * HCovarianceStatisticsPtr;
	typedef double huge * HSumSquaresStatisticsPtr;
	typedef ChannelStatistics huge * HChannelStatisticsPtr;
	typedef ProjectClassNames huge * HPClassNamesPtr;
	typedef ProjectFieldIdentifiers huge * HPFieldIdentifiersPtr;
	typedef ProjectFieldPoints huge * HPFieldPointsPtr;
	typedef CMeanType huge * HCMeanTypePtr;
	typedef CType huge * HCTypePtr;

	typedef LocalAppFile*	LocalAppFilePtr; 
	typedef FSRef *			FSRefPtr;

	typedef struct HParamBlockRec 
		{
		UInt32				ioPosOffset;
		CMFileStream*		fileStreamPtr;
		UCharPtr				ioBuffer;
		UInt32				ioReqCount;
		UInt32				ioActCount;
		SInt16				ioPosMode;
		} HParamBlockRec, *ParmBlkPtr;
#endif	// defined multispec_wx

#if defined multispec_win
	#define  huge

	#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a)) 

	#define kPMPortrait	1
	#define _MAX_FILE 256

	//#define _ITERATOR_DEBUG_LEVEL 1
	#include <vector>

			// The forward definitions for Windows specific classes. 
                  
	class CBitmapButton; 
	class CEditView;
	class CMFileStream;    
	class CMImageFrame;          
	class LGWorld;
	
			// Other definitions.                  
	
	typedef CMPalette*		CMPaletteInfo;

	typedef unsigned char 	Byte, UChar, UInt8; 
	
	typedef unsigned char	*StringPtr;	//, **StringHandle;

	typedef UInt8*				FileStringPtr;

	typedef TBYTE*				WideFileStringPtr;
	
	typedef char 				SignedByte, SInt8;
	
	typedef unsigned short 	UInt16;
	
	typedef signed short 	SInt16;
	
	typedef unsigned long 	UInt32;
	
	typedef signed long 		SInt32; 
	
	typedef __int64	 		SInt64;  
	
	typedef char*				Ptr; 
	
	typedef double				SDouble; 
	
	typedef void				PascalVoid;
	
			// Define some variables that are defined in the Macintosh Toolbox and not
			// in the Windows compiler  
	
	typedef SInt16			  	Boolean;   		// BOOLean (0 or !=0)
	                               
	
			// Define some variables for MultiSpec
	
	typedef UInt32				CGContextRef;
	typedef UInt32				CGRect;
	typedef UInt32				AEAddressDesc;
	typedef UInt32				AEDesc; 
	typedef UInt32				AppFile;
	typedef UInt32				SFTypeList;
	typedef Ptr					CGrafPtr;
	typedef Ptr					ControlActionUPP;
	typedef GLOBALHANDLE  	ControlHandle; // Windows global handle
	typedef GLOBALHANDLE		ControlRef;
	typedef Ptr					CursPtr;
	typedef Ptr					DlgHookUPP;
	typedef CMDialog*			DialogPtr;
	typedef Ptr					DragGrayRgnUPP;                  
	typedef Ptr					GrafPtr;   	// Pointer                       
	typedef Ptr					GWorldPtr;   	// Pointer
	typedef GLOBALHANDLE  	Handle;   		// Windows global handle
	typedef CListBox*	  		ListHandle;   	// Windows global handle 
	typedef CListBox*			LegendListHandle;
	typedef GLOBALHANDLE  	MenuHandle; 
	typedef UInt16				MenuItemIndex;
	typedef GLOBALHANDLE		MenuRef;
	typedef GLOBALHANDLE  	PixMapHandle; 
	typedef UInt16				PMOrientation;
	typedef GLOBALHANDLE  	StringHandle;
	typedef Ptr					ModalFilterUPP;
	typedef Handle				WSHandle;   
	typedef Handle				WEReference;   
	typedef Handle				wxDC;

	typedef struct EventRecord
		{            
		SInt16 		modifiers;
		SInt16 		event;
		
		} EventRecord;   

	typedef struct FSRef
		{
		//UInt8			hidden[_MAX_PATH];
		TBYTE				hidden[_MAX_PATH];

		}	FSRef;

	typedef struct Point 
		{            
		SInt16 		h;
		SInt16 		v;
		
		} Point;  

	typedef struct Cell 
		{            
		SInt16 		h;
		SInt16 		v;
		
		} Cell;  
		
	typedef struct LongPoint 
		{                 
		SInt32			v;
		SInt32			h; 
		
		} LongPoint; 

   struct Rect 
	   {    // rc  
		int			left; 
		int			top; 
		int			right; 
		int			bottom;
		 
      };
	
	typedef struct LongRect
		{                                              
		SInt32		top; 			// "top" of rectangle
		SInt32		left;			// "left" of rectangle 
		SInt32		bottom;		// "bottom" of rectangle
		SInt32		right;		// "right" of rectangle 
	
		} LongRect, *LongRPtr;  
	
	typedef struct ULongRect
		{                                             
		UInt32		top; 			// "top" of rectangle 
		UInt32		left;			// "left" of rectangle 
		UInt32		bottom;		// "bottom" of rectangle
		UInt32		right;		// "right" of rectangle  
	
		} ULongRect, *ULongRPtr;
		
	struct RGBColor
		{
		UInt16			red;
		UInt16			green;
		UInt16			blue;
		};              
		
	typedef struct RGBColor RGBColor, *RGBColorPtr, **RGBColorHdl; 
	  
	struct ColorSpec
		{
		SInt16			value;
		RGBColor			rgb;
		
		};   

	typedef ColorSpec CSpecArray[1];

	struct ColorTable 
		{
		long							ctSeed;					/*unique identifier for table*/
		short							ctFlags;					/*high bit: 0 = PixMap; 1 = device*/
		short							ctSize;					/*number of entries in CTTable*/
		CSpecArray					ctTable;					/*array [0..0] of ColorSpec*/
		};
		
	typedef struct ColorTable ColorTable, *CTabPtr;

	struct Palette 
		{
		short							pmEntries;					/*entries in pmTable*/
		short							pmDataFields[7];			/*private fields*/
		RGBColor						pmInfo[1];
		};
		
	typedef struct Palette Palette, *PalettePtr;
	
			//	The following structure is filler for text windows. This is the structure
			//	that is used in the Mac OS

	typedef struct 
		{
	   char					string[48];
		
		} TextBlock, TextArray[1];
		
	typedef struct 
		{
		char					string[24];
		
		} LineRec, LineArray[1];
	
	
	typedef GLOBALHANDLE  	CTabHandle; 
	typedef GLOBALHANDLE  	PaletteHandle;
		
	typedef CRgn*				RgnHandle;
			// Str255 is set for 512 characters to allow for usage with UNICODE strings.
	typedef unsigned char	Str255[512], Str63[64], Str31[32], Str15[16];
	typedef const unsigned char *ConstStr255Param;
	typedef ConstStr255Param ConstStr63Param, ConstStr32Param, ConstStr31Param, ConstStr27Param, ConstStr15Param;

	typedef Ptr					UserItemProcPtr;
	typedef Ptr					UserItemUPP;
	typedef Ptr					DlgHookYDUPP;
	typedef CMImageView*		WindowPtr;
	typedef CMStatisticsForm*	StatisticsWindowPtr;
	                                
	typedef char				LWindow;
	
	typedef SInt16				OSErr;
	
	typedef UInt32				FourCharCode;
	typedef UInt32				DateTimeRec;
	typedef UInt32				FInfo;      
	typedef UInt32				FSSpec; 
	typedef UInt32				IOCompletionUPP;     
	typedef UInt32				ListClickLoopUPP;
	typedef TBYTE				LocalAppFile;  
	typedef UInt32				Pattern;        
	typedef UInt32				SFReply;        
	typedef UInt32				THPrint; 
	typedef UInt32				ThreadID; 
	typedef UInt32				ThreadTaskRef;        
	typedef UInt32				WSEScrollActionUPP; 
	typedef UInt32				WEScrollUPP;
	 
	typedef FourCharCode		OSType;

	typedef LocalAppFile*	LocalAppFilePtr;  

			// Declare huge pointers
	
	typedef char huge *						HPtr;

	typedef char huge *						HCharPtr;
	
	typedef UChar huge *						HUCharPtr; 
	
	typedef SInt8 huge *						HSInt8Ptr; 
	
	typedef UInt8 huge *						HUInt8Ptr;
	
	typedef SInt16 huge *					HSInt16Ptr; 
	
	typedef UInt16 huge *					HUInt16Ptr;
	
	typedef SInt32 huge *					HSInt32Ptr; 
	
	typedef UInt32 huge *					HUInt32Ptr;
	
	typedef SInt64 huge *					HSInt64Ptr;
	
	typedef float huge *						HFloatPtr;
	
	typedef FileIOBuffer huge *			HFileIOBufferPtr;

	typedef FSRef *							FSRefPtr;
	
	typedef double huge *					DoublePtr;	
	
	typedef double huge *					HDoublePtr;	
	
	typedef double huge *					HFldLikPtr;	

	typedef double huge *					HCovarianceStatisticsPtr; 
	
	typedef double huge *					HSumSquaresStatisticsPtr;
	
	typedef ChannelStatistics huge *		HChannelStatisticsPtr;  
	
	typedef ProjectClassNames huge *		HPClassNamesPtr;
	
	typedef ProjectFieldIdentifiers huge *		HPFieldIdentifiersPtr;
	
	typedef ProjectFieldPoints	huge *	HPFieldPointsPtr;
	
	typedef CMeanType huge *				HCMeanTypePtr;
	
	typedef CType huge *						HCTypePtr;										
				
	
	typedef struct HParamBlockRec
		{
		UInt32				ioPosOffset;
		CMFileStream*		fileStreamPtr;
		UCharPtr				ioBuffer;
		UInt32				ioReqCount;
		UInt32				ioActCount;
		SInt16				ioPosMode;
		
		} HParamBlockRec, *ParmBlkPtr;
#endif	// defined multispec_win 

typedef UInt8					FileStringName255[256];


typedef struct AlgebraicTransformationFunction
	{
	Str255							denominatorString;
	Str255							numeratorString;
	double							transformFactor;
	double							transformOffset;
	SInt16							instrumentCode;
	SInt16							numberChannels;
	
	} AlgebraicTransformationFunction,  *AlgebraicTransformationFunctionPtr;
	

typedef struct ArcViewDoublePoint 
	{
	SDouble 			x;
	SDouble	 		y;
	
	} ArcViewDoublePoint,  *ArcViewDoublePointPtr;
	

typedef struct DoublePoint 
	{
	double 			v;
	double	 		h;
	
	} DoublePoint; 
	

		// Structure that need to be declared first because they are used in later
		// structures.
			
typedef struct ProjectionPursuitSpecs
	{
	double							buThreshold;
	double							tdThreshold;
	double							optimizationThreshold;
	
			// Handle to eigenvalues and eigenvectors that were computed for		
			// preprocessing step.														
	Handle							bandGroupingHandle;
	Handle							transformValueHandle;
	Handle							transformVectorHandle;
	
	UInt32							minimumNumberSamples;
	
	SInt16							algorithm;
	SInt16							firstStageMethod;
	SInt16							initialGroupingCode;
	
	UInt16							bothOddChoicesNumberFeatures;
	UInt16							finalNumberFeatures;
	UInt16							initialNumberFeatures;
	UInt16							maximumNumberFeatures;
	UInt16							numberFeatureChannels;
	
	Boolean							numericalOptimizationFlag;
	Boolean							savePPTransformationFlag;
	Boolean							startWithCurrentBandGroupFlag;
	
	} ProjectionPursuitSpecs, *ProjectionPursuitSpecsPtr;
	

typedef struct RecodeThematicImage
	{
	Handle				thresholdFileInfoHandle;
	SInt32				newRecodedValue;
	SInt32				thresholdValue;
	SInt16				compareParameterCode;
	SInt16				thresholdImageSelection;
	
	} RecodeThematicImage, *RecodeThematicImagePtr;
	

typedef struct TransMapMatrix
	{
	double 		map[3][2];
	
	} TransMapMatrix;
	
		
		// Now declaration of the rest of the structures.

typedef struct AreaDescription
	{	
			// Temporary storage for the number of samples per channels for
			// use when reading data values from the disk file.
	SInt64					numSamplesPerChan;
	
			// Pointer to mask info structure
	MaskInfoPtr				maskInfoPtr;
			
			// Storage for a rgnHandle which is used for polygonal type areas.								
	RgnHandle				rgnHandle;
	 
	SInt32					line;  
	 
	 		// The following define an area within an image.
	SInt32					lineStart;
	SInt32					lineEnd;
	SInt32					columnStart; 
	SInt32					columnEnd;
	SInt32					lineInterval;
	SInt32					columnInterval;
	
			// The following are variables to be used if applying to an
			// associated image that does not have the same start line and
			// column.
	SInt32					columnOffset;
	SInt32					lineOffset;
	SInt32					maxNumberColumns;
	SInt32					maxNumberLines;
	
			// Number of lines in the area description
	SInt32					numberLines;
	
			// Currently is not used.
	SInt32					notPolygonPoints;
	
			// Start line and column for the area relative to the image that it
			// was defined in.
	SInt32					startLine;
	SInt32					startColumn;
	
			// The line and column offset within the image that the mask is being
			// applied.
	UInt32					maskColumnStart;
	UInt32					maskLineStart;
	
			// The mask value that is associated with the specific class and field
			// for this area discription.
	UInt32					maskValueRequest;
	
			// The class and field numbers that this area represents.
	SInt16					classNumber;
	SInt16					fieldNumber;
	
			// "Temporary" storage for the type of disk file format.
	SInt16					diskFileFormat;
	
			// Code indicating whether the area description is a rectangular, 
			// cluster or mask type of description.
	SInt16					pointType;
	
			// Flag indicating whether the offset should be applied. This may be
			// needed the field area for a project is being applied to an
			// associated image which does not have the same start line and column 
			// as the base image for the project.
	Boolean					applyOffsetFlag;
	
			// Flag indicating whether this area is defined as a polygonal area.
	Boolean					polygonFieldFlag;
	
	} AreaDescription, *AreaDescriptionPtr;

	
typedef struct BiPlotDataSpecs
	{
	double					probabilityThreshold;
	
			// Pointer to the channels to be listed. 										
	Handle	 				channelsHandle;
	
			// Pointer to the list of classes to be listed. 							
	Handle	 				classHandle;
	
	Handle					symbolsHandle;
	
	Handle					windowInfoHandle;
	
	SInt16*					classPtr;
	
	UCharPtr					symbolsPtr;
	
	SInt32					lineStart;
	SInt32					lineEnd;
	SInt32					lineInterval;
	SInt32					columnStart;
	SInt32					columnEnd;
	SInt32					columnInterval;
	UInt32					numberClasses;
	
	SInt16					fileInfoVersion;
	SInt16					classSet;
	SInt16					displayPixelCode;
	SInt16					numberChannels;
	SInt16					outlineClassCode;
	SInt16					plotDataCode;
	SInt16					probabilityThresholdCode;
	SInt16					symbolSet;
	SInt16					axisFeaturePtr[2];
	
	UInt16					numberFields;

	Boolean					createNewGraphicsWindowFlag;
	
			// Flag indicating (if true) that the list data specifications have	
			// not been loaded the first time.												
	Boolean					firstTime;
	Boolean					projectFlag;
	Boolean					thresholdFlag;
	Boolean					featureTransformationFlag;
	
	} BiPlotDataSpecs, *BiPlotDataSpecsPtr;
		
		
		//	This structure defines the block offset and size for those file
		// formats for which data is stored in blocks in the file. The block
		// may not be randomly positioned in the file										
	
typedef struct BlockFormat
	{
	UInt32							blockOffsetBytes;
	UInt32							blockSize;
	
	} BlockFormat, *BlockFormatPtr;


typedef struct 	CEMParameters 
	{
	Handle					correlationMatrixClassHandle;	
		
			// First line to use to compute  second order image statistics. 					
	UInt32					lineStart;
		
			// Last line to use to compute  second order image statistics. 						
	UInt32					lineEnd;
		
			// Line interval to use to compute second order image statistics.				
	UInt32					lineInterval;
		
			// First column to use to compute second order image statistics. 					
	UInt32					columnStart;
		
			// Last column to use to compute second order image statistics. 					
	UInt32					columnEnd;
		
			// Column interval to use to compute second order image statistics.				
	UInt32					columnInterval;
	
			// Number of classes to use to compute the correlation matrix
	UInt32					numbercorrelationMatrixClasses;
	
			// Code indicating whether all classes are to be used to compute the
			// correlation matrix.
	SInt16					correlationMatrixClassAreaSet;
	
			// Code indicating whether to compute the correlation matrix
			// 	= 1 use training field for the selected classes
			//		= 2 use selected area
	SInt16					correlationMatrixCode;
   
	}	CEMParameters, *CEMParametersPtr;
	
	
typedef struct ChannelDescription
	{
	unsigned char		description[24];
	
	} ChannelDescription, *ChannelDescriptionPtr;
	
	
typedef struct ChannelStatistics
	{
	double				mean;
	double				sum;
	double				standardDev;
	double				minimum;
	double				maximum;
	
	} ChannelStatistics, *ChannelStatisticsPtr;

	
typedef struct ChannelSubsets
	{
	SInt32					nextIndex;
	Str31						subsetName;
	SInt32					numberChannels;
	UInt16					channelList[2];
	
	} ChannelSubsets, *ChannelSubsetsPtr;
	
	
typedef struct CharBuffer
	{
	char					charBuffer[1];
	
	} CharBuffer, *CharBufferPtr;

	
typedef struct ClassifySpecs
	{
	double					parallelPipedStanDevFactor;
	double					probabilityThreshold;
	double					cemThreshold;
	double					correlationAngleThreshold;
	double					correlationCoefficientThreshold;
	Handle 					channelsHandle;
	Handle	 				classHandle;
	Handle	 				classAreaHandle;
	
			// CEM Parameters.
	Handle					cemParametersH;
	
			// Decision Tree Parameters.													
	Handle					decisionTreeVarH;
	
			// Echo Parameters.																
	Handle					echoClassifierVarH;
	
	Handle	 				featureHandle;
	Handle		 			symbolsHandle;
	Handle					targetWindowInfoHandle;
	Handle 					thresholdProbabilityHandle;
	Handle 					thresholdTableHandle;
	double*					thresholdTablePtr;
	
			// This vector is a list of the class numbers being used in the
			// classification
	SInt16*					classPtr;
	
			// This vector contains a 1 for classes being used in the classification
			// and 0 for classes which are not being used.
	SInt16*					classVectorPtr;
	
	SInt16*					thresholdProbabilityPtr;
	unsigned char*			symbolsPtr;
	
	SInt32					imageLineStart;
	SInt32					imageLineEnd;
	SInt32					imageLineInterval;
	SInt32					imageColumnStart;
	SInt32					imageColumnEnd;
	SInt32					imageColumnInterval;
	SInt32					outputBufferOffset;
	SInt32					probabilityBufferOffset;
	UInt32					numberClassAreas;
	UInt32					numberClasses;
	UInt32					numberProbabilityClasses;
	
	SInt16					channelSet;
	SInt16					classAreaSet;
	SInt16					classSet;
	SInt16					correlationCovarianceCode;
	SInt16					diskFileFormat;
	
			// Index for active image overlay to be used for output
			// of offscreen classification results
	SInt16					imageOverlayIndex;
	
			// Values for Support Vector Machine classifier
			//		See information in Ssvm.h file for more information
	int						svm_type;
	int						svm_kernel_type;
	int						svm_degree;
	double					svm_gamma;
 	double					svm_coef0;
   double					svm_cache_size;
   double					svm_eps;
	double					svm_cost;
   double					svm_nu;
   double					svm_p;
   int						svm_shrinking;
   int						svm_probability;
	
			// K value to be used for K Nearest Neighbor classifier
	SInt16					nearestNeighborKValue;
	SInt16					knnThreshold;
	
	SInt16					mode;
	SInt16					parallelPipedCode;
	SInt16					numberFeatures;
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16					outputStorageType;
	SInt16					probabilityThresholdCode;
	SInt16					symbolSet;
	SInt16					pixelThresholdDegreesOfFreedom;
	SInt16					cellThresholdDegreesOfFreedom;
	SInt16					numberChannels;
	
	Boolean					createThresholdTableFlag;
	Boolean					featureTransformationFlag;
	
	Boolean					supportVectorMachineModelAvailableFlag;
	
	Boolean					imageAreaFlag;
	Boolean					testFldsFlag;
	Boolean					thresholdFlag;
	Boolean					trainingFldsResubstitutionFlag;
	Boolean					trainingFldsLOOFlag;
	
	}		ClassifySpecs, *ClassifySpecsPtr;
	
	
typedef struct ClassifierVar
	{
	AreaDescription	maskAreaDescription;		// Use for special case.
	double				variable1;
	double				variable2;
	double				variable3;
	HDoublePtr			chanMeanPtr;
	double*				classConstantPtr;
	double*				classConstant2Ptr;
	double*				classConstantLOO1Ptr;
	double*				classConstantLOO2Ptr;
	double*				classConstantLOO3Ptr;
	double*				classConstantLOO4Ptr;
	double*				classConstantLOO5Ptr;
	HDoublePtr			covariancePtr;
	HDoublePtr			workVectorPtr;
	HDoublePtr			workVector2Ptr;
	HUInt16Ptr			symbolToClassPtr;
	HSInt64Ptr			countVectorPtr;
	HSInt32Ptr			countClassIndexPtr;
	HUInt32Ptr 			columnWidthVectorPtr;
	SInt64				totalCorrectSamples;
	SInt64				totalNumberSamples;
	SInt64				totalSameDistanceSamples;
	UInt32				numberKappaColumns;
	UInt32				tableWidth;
	UInt32				totalCountIndexStart;
	SInt16				areaCode; 
	SInt16				areaNumberWidth; 
	SInt16				backgroundIndexOffset;    
	SInt16				classGroupNameWidth;
	SInt16				numberWidth;
	SInt16				numberPrecision;
	SInt16				sampleNumberWidth;
	SInt16				summaryCode;
	SInt16				tableType;
	SInt16				unitsCode;
	SInt16				summaryUnitsCode;
	Boolean				thresholdFlag;
	Boolean				useLeaveOneOutMethodFlag;
	
	} ClassifierVar, *ClassifierVarPtr;
	
	
typedef struct ClassInfo 
	{
	SInt64				no_samples;
	
	double				determinant;
	double				logDeterminant;
	
	HDoublePtr			covariancePtr;
	HDoublePtr			dataValuesPtr;
	HDoublePtr			inversePtr;
	HDoublePtr			meanPtr;
	HDoublePtr			transformedCovPtr;
	HDoublePtr			transformedMeanPtr;
	
	} ClassInfo, *ClassInfoPtr;
	

		// Following structure is used in Feature Extraction and Statistics Image
		// processors.
typedef struct class_info_str 
	{
	double				log_det;	
	SInt64				no_sample;	
	UInt32				wave_length_default;
	
	HDoublePtr			data_values;

	HDoublePtr			mean;	
		
	HDoublePtr			cov;
	HDoublePtr			icov;			
	
	unsigned char*		half_image;
	HDoublePtr			fmean;
	HDoublePtr			fvar;
	
	HFloatPtr			mah_dis;
	SInt16				stat_name_storage;	// -1 indicates that this is a selected area.
	Boolean				mah_disLoadedFlag;
	
	} CLASS_INFO_STR;
	
	
typedef struct ClusterSpecs
	{
	double							criticalDistance1;		// for single pass algorithm
	double							criticalDistance2;		// for single pass algorithm
	double							probabilityThreshold;
	HChannelStatisticsPtr		totalChanStatsPtr;
	ClusterType*       			clusterEigenCenterHead;
	ClusterType*       			clusterMemoryHead;
	ClusterType*       			clusterHead;
	HCovarianceStatisticsPtr	totalSumSquaresStatsPtr;
	SInt64							totalNumberClusterPixels;
	Handle							fileInfoHandle;
	Handle							totalSumSquaresStatsHandle;
	Handle							totalChanStatsHandle;
	Handle		 					featureHandle;
	Handle 							channelsHandle;
	Handle 							symbolsHandle;
	Handle		 					clusterClassHandle;
	Handle		 					initializationClassHandle;
	Handle							probabilityFileInfoHandle;
	float								convergence;
	double							classifyThreshold;
	HUInt16Ptr	 					dataClassPtr;
	SInt32							imageLineStart;
	SInt32							imageLineEnd;
	SInt32							imageLineInterval;
	SInt32							imageColumnStart;
	SInt32							imageColumnEnd;
	SInt32							imageColumnInterval;
	SInt32							clusterLineStart;
	SInt32							clusterLineEnd;
	SInt32							clusterLineInterval;
	SInt32							clusterColumnStart;
	SInt32							clusterColumnEnd;
	SInt32							clusterColumnInterval;
	UInt32							numberClusterClasses;
	UInt32							numberInitializationClasses;
	SInt16*							clusterClassToFinalClassPtr;
	
	SInt16							saveStatisticsCode;
	SInt16							fileInfoVersion;
			// Classification area																		
			//		=0, no cluster classification to be done.								
			//		=1, training areas only.													
			//		=2, selected image area only.												
	SInt16							classificationArea;
			// Clusters are to come from:														
			//		=1, training areas only.													
			//		=2, selected image area only.												
	SInt16							clustersFrom;
	
	SInt16							channelSet;
	SInt16							clusterClassSet;
	
			// File format to be used to save the cluster map output.
	SInt16							diskFileFormat;
	
			// Index for active image overlay to be used for output
			// of offscreen cluster results
	SInt16							imageOverlayIndex;
	
	SInt16							initializationClassSet;
	SInt16							symbolSet;
	SInt16							minNumberClusters;
	SInt16							maxNumberClusters;
	SInt16							numberClusters;
	SInt16							numberFinalClusters;
	SInt16							minClusterSize;
	SInt16							mode;
	
			// Initialization option to use.
			//		=1, Along first eigenvectors.
			//		=2, Within eigenvector volume.
			//		=3, One pass cluster
	SInt16							initializationOption;
	
			// Variable to indicate where cluster output is to be placed.			
			//		=0, no output; 																
			//		=1, to output window; 														
			//		=2, to disk file; 															
			//		=3, both output window and disk file; 									
			//		=4, to classification results image file;								
			//		=5, to output window and classification results disk file;		
			//		=6, to disk file and classification results disk file;			
			//		=7, to output window, disk file and classification file;
			//		=8, to image window as an overlay			
	SInt16							outputStorageType;
	
	SInt16							probabilityThresholdCode;
	
	SInt16							totalNumberAreas;
	
			// Code for using current project class means as substitutes
			// for some (all) of eigenvector centers.
			//		=0, Do not include class means
			//		=1, Include class means
	SInt16							projectClassMeansCode;
	
	SInt16							transparency;
	
	UInt16							numberChannels;
	
	Boolean							thresholdFlag;
	Boolean							useCorrelationMatrixFlag;
	
	} ClusterSpecs, *ClusterSpecsPtr; 


typedef struct ClusterType
	{
   SInt32	    			numPixels;				// Number of pixels in cluster. 	
   SInt32	  				numPixInCluster;		// # of pixels from TOTAL 			
   														// classification in cluster. 	
   HCMeanTypePtr   		meanPtr; 				// Pointer to start of means		
   struct ClusterType*	next;						// Ptr to next cluster. 			
   HDoublePtr				sumPtr; 					// Pointer to start of sums.		
   HDoublePtr				variancePtr;			// Pointer to start of variances	
   HDoublePtr				sumSquarePtr1; 		// Pointer to start of sum of		
   														// channel squares.					
   HDoublePtr				sumSquarePtr2; 		// Pointer to start of sum of		
   														// covariance squares.				
	SInt16					clusterNumber;			// Cluster class identifier.
	SInt16					projectStatClassNumber;	// Index to project class info in needed.		
   Boolean					varianceComputed;		// Flag indicating whether			
   														// variance vector is computed.	
   
	} ClusterType;
	

typedef struct ControlPoints
	{
	double*					easting1Ptr;
	double*					northing1Ptr;
	double*					easting2Ptr;
	double*					northing2Ptr;
	SInt16*					statusPtr;
	SInt32					count;
	
	} ControlPoints, *ControlPointsPtr;
		
	
		// This structure contains the information to convert the image data		
		// value to the palette display value.  The image histogram and the		
		// display palette is used to create this vector								
			
typedef struct DataToDisplayLevel	
	{
	Handle		vectorHandle;
	 
	#if defined multispec_mac || defined multispec_mac_swift
		WindowPtr		window;
	#endif	// defined multispec_mac || defined multispec_mac_swift
	#if defined multispec_win  
		CMImageView* 	window;  
	#endif	// defined multispec_win
	#if defined multispec_wx
    CMImageView* window;
   #endif	// defined multispec_wx
      
	double		gaussianStretch;
	SInt32		vectorBytes;
	SInt32		bytesOffset;
	UInt32	 	numberLevels;			// Number if display levels				
	UInt16		numberBytes;
	SInt16		backgroundValueCode;
	SInt16		channel1;
	SInt16		channel2;
	SInt16		channel3;
	SInt16		minMaxCode;
	SInt16		enhancementCode;
	SInt16		percentTailsClipped;
	SInt16		pixelSize;
	Boolean 		upToDateFlag;
		
	}	DataToDisplayLevel, *DisplayLevelPtr, **DisplayLevelHandle;


typedef struct DialogSelectArea
	{
	Rect								toSelectionBox;
	WindowInfoPtr					imageWindowInfoPtr;
	WindowPtr						windowPtr;
	UInt32							columnStart;
	UInt32							columnEnd;
	UInt32							columnInterval;
	UInt32							lineStart;
	UInt32							lineEnd;
	UInt32							lineInterval;
	SInt16							baseItem;
	SInt16							entireIconItem;
	Boolean							adjustToBaseImageFlag;
	Boolean							drawRoutineLoadedFlag;
		
	} DialogSelectArea, *DialogSelectAreaPtr, **DialogSelectAreaHandle;
	
	
		// This stucture is used to store information about output results		
		// disk files and input disk results disk file.									
		
typedef struct DiskFileList	
	{
	Handle				fileInfoH;
	FileInfoPtr			fileInfoPtr;
	SInt32				refCon;
	
	} DiskFileList, *DiskFileListPtr;
	
	
		// This stucture contains information about the output and input result	
		// file information.																		
		
typedef struct DiskFileSummary	
	{
	DiskFileListPtr	diskFileListPtr;
	Handle				diskFileListH;
	SInt16				numberFiles;
	
	} DiskFileSummary, *DiskFileSummaryPtr;
	

		// This structure is used specify the variable to be used to display	
		// an image in the image window													
		
typedef struct DisplaySpecs
	{
			// The background color.															
	RGBColor					backgroundColor;
	
			// The edited background color.													
	RGBColor					editedBackgroundColor;
	
			// User minimum and maximum bin indices to be used for image enhancement.		
	double					minMaxValues[3][2];
	
			// Gaussian stretch
	double					gaussianStretch;
	
			// Magnification to use to copy bit/pix image map to the display		
			// window.																				
	double					magnification;  

		// Maximum magnification that can be used for the image stored in
		// the offscreen map.																
	double					maxMagnification; 		
	
			// Location in bit/pix map of image to use as origin in display		
			// window. double is being used to allow for a scrolled condition
			// where only a portion of a pixel is being displayed. This can happen
			// in wxWidgets based version.
	double					origin[2];

		// The scale of the currently displayed image (in x or column direction).	
		//		The value will be -1. if scale cannot be computed.															
	double					scale; 

			// Thematic bin width.		
	double					thematicBinWidth;
	
			// Factor to use to adjust data values to desired thematic value units
			// to be displayed in the legend.
	double					thematicValueFactor;
	
			// Minimum and maximum values used for thematic type multispectral
			// image displays.		
	double					thematicTypeMinMaxValues[2];

			// Color palette handle for spectral classes for image window. 		
	CTabHandle				savedClassCTableHandle;

			// Color palette handle for information groups for image window.		
	CTabHandle				savedGroupCTableHandle;
	
			// List of channels to be displayed.											
	Handle					channelsHandle;
	
			// List indicating the groups or classes to be displayed.  All other	
			// groups/classes will be displayed as the background color/pattern.	
	Handle					displayClassGroupsHandle;
	
			// Handle to local group names in temporary list.  In other words 	
			// this list hasn't been accepted yet.											
	Handle					localGroupNameHandle;
	
			// Handle for vector to convert thematic symbols to palette entries.	
	Handle					symbolToPaletteEntryH;

			// Handle for color palette for image window.
	CMPaletteInfo			paletteObject;   
             
	#if defined multispec_win
				// Handle for color palette for image window when in the background.
				// This is required for the windows version. 
		CMPaletteInfo			backgroundPaletteObject; 
	#endif	// defined multispec_win
	
	#if defined multispec_wx
				// Handle for color palette for image window when in the background.
				// This is required for the windows version.
		CMPaletteInfo 			backgroundPaletteObject;
	
		 		// Begin and end lines to use when displaying portions of the image
				// during update events for thematic images
	
		int						updateEndLine;
		int						updateStartLine;
	#endif	// defined multispec_wx
      
      
			// Value that is to be considered a background value rather than a data
			// value.
	SInt32					backgroundDataValue;
	
			// User minimum and maximum bin indices to be used for image enhancement.		
	SInt32					minMaxValuesIndex[3][2];
	
			// Smallest interval between display bins. Use for thematic type
			// multispectral displays.
	SInt32					smallestIndexInterval;
	
			// Minimum and maximum indices used for thematic type multispectral
			// image displays.		
	SInt32					thematicTypeMinMaxIndices[2];
	
			// Number of display levels.															
	UInt32					numberLevels;
	
			// Last column to be displayed. 													
	UInt32					columnEnd;
	
			// Column interval to be displayed. 											
	UInt32					columnInterval;
	
			// First column to be displayed. 												
	UInt32					columnStart;		
	
			// Last column displayed in current image.									
	UInt32					displayedColumnEnd;
	
			// Column interval displayed in current image.								
	UInt32					displayedColumnInterval;
	
			// First column displayed in current image.									
	UInt32					displayedColumnStart;
	
			// Last line displayed in current image.														
	UInt32					displayedLineEnd;
	
			// Line interval displayed in current image.									
	UInt32					displayedLineInterval;
	
			// First line displayed in current image. 									
	UInt32					displayedLineStart;
	
			// Image dimensions																	
	UInt32					imageDimensions[2];
	
			// Last line to be displayed. 																	
	UInt32					lineEnd;
	
			// Line interval to be displayed. 												
	UInt32					lineInterval;
	
			// First line to be displayed. 													
	UInt32					lineStart; 
	                                    
			// Number of classes to be displayed in thematic type images.			
	UInt32					numberDisplayClasses;
	
			// Number of groups to be displayed in thematic type images.			
	UInt32					numberDisplayGroups;		
	
			// Number of display levels.															
	UInt32					numberPaletteLevels;
	
			// Width of one channel in offscreen units.	Used for side-by-side	
			// displays																				
	UInt32					offscreenChannelWidth;
	
			// Code indicating whether all or subset of classes are to be			
			// displayed in thematic type images.											
	SInt16					allSubsetClassSet;
	
			// Code indicating whether all or subset of groups are to be			
			// displayed in thematic type images.											
	SInt16					allSubsetGroupSet;	
	
			// Code indicating whether a specified data value (min data value such
			// as '0' for unsigned data) is to be treated as:
			// 	0 = data, 
			//		1 = black
			//		2 = white	
			// The default background value is '0' or data.									
	UInt16					backgroundValueCode;	
			
			// The channel to be used for the blue portion of 'true color' image 
			// displays.																							
	SInt16					blueChannelNumber;
	
			// Last selection in the bits of color pop up menu.						
	SInt16					bitsOfColorSet;
	
			// Blink procedure to use for the thematic image display.						
	SInt16					blinkProcedure;
			
			// The channel number to be display for one channel images.				
	SInt16					channelNumber;				
	
			// Indication of whether all or a subet of the channels have been		
			// selected.																			
	SInt16					channelSet;		
	
			// Indicates whether spectral classes or information groups are to	
			// be displayed.																		
	SInt16					classGroupCode;		
			
			// Color to be used to display a one channel image on the screen.		
			// 	=1 Grey																			
			// 	=2 Red
			//		=3 Green
			//		=4 Blue
			//		=5 Yellow
			//		=6 Aqua
			// 	=7 Violet																											
	SInt16					color;	
	
			// Number of channels in the list.												
	UInt16					displayedNumberChannels;								

			// Type of channel display.														
			// 	=1 1-channel thematic display													
			//		=2 1-channel color display													
			//		=3 3-channel color display													
			//		=4 2-channel color display													
	UInt16					displayType;	
			
			// Number of horizontal display elements per image pixel.
			// Needed for 1-bit displays such as Mac Pluses, SE's etc.
			// Removed in 2001				
	//SInt16					displayHPixelsPerImagePixel;
	
			// Last selection in the display type pop up menu.							
	SInt16					displaySet;	
			
			// Number of vertical display elements per image pixel.
			// Needed for 1-bit displays such as Mac Pluses, SE's etc.
			// Removed in 2001									
	//SInt16					displayVPixelsPerImagePixel;	
	
			// Last selection in the enhancement pop up menu.							
	SInt16					enhancementCode;
			
			// The channel to be used for the green portion of 'true color'  		
			// image displays.																	 
	SInt16					greenChannelNumber;	
	
			// Flag indicating method of histogram development.						
			//		= 0 image statistics already available.								
			//		= 1 compute image statistics.												
			//		= 2 load image statistics from default disk file.					
	SInt16					histogramCompute;
	
			// Pixel depth of color image windows to be used.							
	SInt16					lastColorPixelSize;
	
			// Code for the type of min-max values to use in the display.			
			//		= 1, min-max per individual channels to be displayed.				
			//		= 2, min-max for the entire data range.								
			// 	= 3, user supplied min-max.												
	SInt16					minMaxCode;
	
			// Pointers to order of channels being displayed.							
	SInt16					minMaxPointers[3];
	
			// Number of entries in the palette to be used in drawing the image 	
			// on the screen.																		
	UInt16					numPaletteEntriesUsed;
	
			// Code indicating whether to add offset to image values when			
			// converting to palette values in the image display section for		
			// Thematic type images.															
			// 	value of paletteOffset is the amount of offset to add.			
	SInt16					paletteOffset;	
	
			// Specifies the percent of the histogram tails that are to be			
			// clipped when determining the minimum and maximum data value.		
	SInt16					percentTailsClipped;
	
			// Pixel depth of image window to be drawn into.							
	SInt16					pixelSize;
			
			// The channel to be used for the red portion of 'true color' image 	
			// displays.																			
	SInt16					redChannelNumber;			
			
			// Code for colors to be used for displays.									
			//	  =3 gb 2-channel displays														
			//	  =5 rb 2-channel displays														
			//	  =6 rg 2-channel displays														
			//	  =7 rgb 3-channel displays													
			//   =8 1-channel type displays																		
	SInt16					rgbColors;
	
			// Type of palette to be used in Thematic displays.						
			//		=1 Application default.														
			//		=2 default file palette, such as ERDAS TRL file, etc				
			//		=3 user defined file.														
			//		=4 no change.																	
	SInt16					thematicClassPaletteType;
	
	SInt16					thematicGroupPaletteType;
	
			// Local number of groups in temporary list.  In other words this		
			// list hasn't been accepted yet.												
	UInt16					localNumberGroups;
	
			// Number of channels in the list.												
	UInt16					numberChannels;			
			
			// Flag indicating whether a better 8-bit palette has been generated
			// from the 16 or 24 bit offscreen bit map.						
			//		false - better 8-bit palette has not been generated.											
			//		true  - better 8-bit palette has been generated.														
	Boolean 					better8BitPaletteFlag;
	
			// Flag indicating whether the class to group table has changed and	
			// needs to have its color table updated.										
	Boolean					classToGroupChangeFlag;
												
			// Flag indicating whether computation of data level to display		
			// is to be forced																	
	Boolean					dataToDisplayCompute;
	
			// Flag indicating whether default histogram statistics disk file		
			// exists.																				
	Boolean					defaultHistogramFileFlag;
	
			// Flag indicating whether the title should be drawn for side by
			// side image displays. This is used when one is drawing a portion
			// of an image at a time, i.e. around every second or so for large
			// images. The title should only be drawn once.
	Boolean					drawSideBySideTitleFlag;
	
			// Flag indicating whether default file palette exists.					
	Boolean					filePaletteFlag;
	
			// Flag indicating if the load Specifications is the first time.  	
			// This flag is if it is.  The flag will be set false after the 		
			// first load.  See LoadDisplaySpecs function.								
	Boolean					firstTime;			
	
			// Flag indicating that the image file correctly reflects the data
			// in the supporting disk file.								
	Boolean					imageWindowIsUpToDateFlag;
	
			// Flag indicating whether the currectly loaded vector overlays 
			// should be displayed over the image window
	Boolean					includeVectorOverlaysFlag;			
		
			// Flag indicating whether the structure should be initialized.																				
	Boolean					initializeStructureFlag;		
			
			// Flag indicating the order that the palette should be defined. 		
			//   true = 'white' to 'black'													
			//   false = 'black' to 'white'																				
	Boolean					invertValues;				
			
			// Flag indicating the order for the enhancement for red, green
			// and blue.  If one channel or side by side the first element, [0]
			// will be used. This will eventually replace the above variable.											
			//   false = 'black' to 'white'		
			//   true = 'white' to 'black'																						
	Boolean					invertValuesFlag[3];				
			
			// Flag indicating whether the palette is up to date.						
			//		false - palette needs updating.											
			//		true  - palette does not need updating.														
	Boolean 					paletteUpToDateFlag;

			// Flag indicating whether the palette needs to be read from disk.	
			//		false - palette needs updating.											
			//		true  - palette does not need updating.														
	Boolean 					readPaletteFromDiskFlag;
	
			// Use threshold file flag.														
	Boolean					thresholdFileFlag;
	
			// Flag indicating whether any vector overlays that are loaded
			// overlap with the image file.
	Boolean					vectorOverlaysExistFlag;
												
	}	DisplaySpecs, *DisplaySpecsPtr;
	

		// This structure is used specify some of the variables to be used as
		// default items for the DisplaySpec structures when windows are opened.
		// This structure will store the values for the some settings which the use
		// last used.  The purpose is to reduce mouse clicks when making settings
		// in the display dialog box.												
		
typedef struct DisplaySpecsDefault
	{						
			// Magnification to use to copy bit/pix image map to the display		
			// window.																				
	double							magnification; 

			// Minimum and maximum data values used for thematic type multispectral
			// image displays.		
	double							thematicTypeMinMaxValues[2];
	
			// Factor to use to adjust data values to desired thematic value units
			// to be displayed in the legend.
	double							thematicValueFactor;
	
			// Value that is to be considered a background value rather than a data
			// value.
	SInt32							backgroundDataValue;
	
			// Number of display levels.
	UInt32							numberLevels;	
	
			// Number of display levels.															
	UInt32							numberThematicLevels;
			
			// The channel to be used for the blue portion of 'true color' image 
			// displays.																							
	SInt16							blueChannelNumber;
	
			// Code indicating whether the channels are in wavelength order
	SInt16							channelsInWavelengthOrderCode;
	
			// The channel number to be display for one channel images.				
	SInt16							channelNumber;	
	
			// Last selection in the display type pop up menu.							
	SInt16							displaySet;	
	
			// Last selection in the enhancement pop up menu.							
	SInt16							enhancementCode;
			
			// The channel to be used for the green portion of 'true color'  		
			// image displays.																	 
	SInt16							greenChannelNumber;	
	
			// Code for the type of min-max values to use in the display.			
			//		= 1, min-max per individual channels to be displayed.				
			//		= 2, min-max for the entire data range.								
			// 	= 3, user supplied min-max.												
	SInt16							minMaxCode;	
			
			// The channel to be used for the red portion of 'true color' image 	
			// displays.																			
	SInt16							redChannelNumber;			
			
			// Code for colors to be used for displays.									
			//	  =3 gb 2-channel displays														
			//	  =5 rb 2-channel displays														
			//	  =6 rg 2-channel displays														
			//	  =7 rgb 3-channel displays													
			//   =8 1-channel type displays																		
	SInt16							rgbColors;		
	
			// Code to be used for the class palette for thematic 1-channel displays
	SInt16							oneChannelThematicClassPaletteType;
	
			// Specifies the percent of the histogram tails that are to be			
			// clipped when determining the minimum and maximum data value.		
	SInt16							percentTailsClipped;
	
			// Pixel depth of image window to be drawn into.
	SInt16							pixelSize;
	
			// Code indicating whether a specified data value (min data value such
			// as '0' for unsigned data) is to be treated as:
			// 	0 = data, 
			//		1 = black
			//		2 = white	
			// The default background value is '0' or data.									
	UInt16							backgroundValueCode;
	
			// Type of channel display.														
			// 	=1 1-channel thematic display													
			//		=2 1-channel color display													
			//		=3 3-channel color display													
			//		=4 2-channel color display													
	UInt16							lastDisplayType;	
	
			// The following variables are used to describe the data type for
			// the setting for above.
	
			// Code indicating whether the data values are integer or float
			//		= 0, kIntegerType
			//		= 1, kRealType
	UInt16							dataTypeCode;
	
	UInt16							numberBytes;
	
	UInt16							numberChannels;				
			
			// Flag indicating the order for the enhancement for red, green
			// and blue.  If one channel or side by side the first element, [0]
			// will be used.											
			//   false = 'black' to 'white'		
			//   true = 'white' to 'black'																			
	Boolean							invertValuesFlag[3];	
	
	Boolean							signedDataFlag;
	
	Boolean							structureLoadedFlag;	
												
	}	DisplaySpecsDefault, *DisplaySpecsDefaultPtr;
	

typedef struct DoubleRect
	{
	double 		top;
	double 		left;
	double 		bottom;
	double 		right;
	
	} DoubleRect;


typedef struct EchoClassifierVar 
	{		
   double							annexationThreshold;
   double							annexationThreshold_derived; 
   double							homogeneityThreshold; 		// THD1	 
   
	#if defined multispec_mac || defined multispec_mac_swift
		CMFileStream					*tempFileStreamPtr;
	#endif	// defined multispec_mac || defined multispec_mac_swift
	
	#if defined multispec_win
		CMFileStream					*tempFileStreamPtr;
	#endif	// defined multispec_win

   #if defined multispec_wx
      CMFileStream               *tempFileStreamPtr;
   #endif
      
	HPtr								fieldLikeFlagsPtr;
   double							*thresholds_phase1;	// Number of class long	   
   double							*work1;
   HDoublePtr						work2;
	HSInt32Ptr						epix_ibufPtr;
	HSInt32Ptr						epix_ibuf2Ptr;
	HSInt16Ptr						cellClassPtr;
	HSInt16Ptr						fieldClassNumberPtr;
	HUInt32Ptr						fieldLikelihoodTableIndexPtr;
   HUInt32Ptr						field_number_table;
   HFldLikPtr						fldlikPtr;
   HFldLikPtr*						fieldLikeIndicesPtr;
		
	SInt64							ipixels;
   SInt64							number_of_homogeneous_cells;
			   
   SInt32							field_size;
   SInt32							number_of_fields;
   SInt32							current_max_field_number; 
   SInt32							ncl_fldlik;
   SInt32							ncl_icel;
   SInt32							ncl_rem_icel;
   SInt32							nrw_fldlik;
   SInt32							nrw_icel;
   SInt32							nrw_rem_icel;
			   
   UInt32							icel_linlen;
   UInt32							lastcol_echo;
   UInt32							lastrow_echo;
   UInt32							line_fldlik; 
   UInt32							ncls; 
   UInt32							num_col;
   UInt32							num_row;
   
   SInt16							algorithmCode;
   SInt16							cell_size;				// cell_width * cell_width 
   SInt16							cell_width;
   
   UInt16							nband;  
   UInt16							nfeature; 
   
   Boolean							combineLikeFieldsFlag;
   Boolean							createHomogeneousFilesFlag;
   Boolean							fixed_homogeneityThreshold_option;
  	Boolean							mixCellsFlag;
	Boolean							tempDiskFileWasUsedFlag;
	Boolean							useTempDiskFileFlag;
   
	}	EchoClassifierVar, *EchoClassifierVarPtr;


typedef struct EvaluateCovarianceSpecs
	{
	SInt16*		 					featurePtr;
	SInt16*	 						channelsPtr;
	SInt16*		 					classPtr;
	
			// Pointer to the list of pointers to the channels to be listed. 		
	Handle	 						featureHandle;
	
			// Pointer to the channels to be listed. 										
	Handle	 						channelsHandle;
	
			// Pointer to the list of classes to be listed. 							
	Handle		 					classHandle;
	
	UInt32							numberClasses;
	SInt16							classSet;
	SInt16							channelSet;
	UInt16							numberChannels;
	SInt16							numberFeatures;
	Boolean							featureTransformationFlag;
	Boolean							listOriginalMatrixFlag;
	Boolean							listInvertedMatrixFlag;
	Boolean							listOriginalXInvertedMatrixFlag;
	Boolean							listInvertedInvertedMatrixFlag;
	
	} EvaluateCovarianceSpecs, *EvaluateCovarianceSpecsPtr;
	
	
typedef struct EvaluateTransformSpecs
	{
	Boolean							checkTransformationFlag;
	Boolean							listEigenvaluesFlag;
	Boolean							listTransformFlag;
	Boolean							listTransformXTposedTransformFlag;
	
	} EvaluateTransformSpecs, *EvaluateTransformSpecsPtr;
	
	
typedef struct FeatureExtractionSpecs
	{
	ProjectionPursuitSpecs		projectionPursuitParams;
	double							threshold1;
	double							threshold2;
	
			// Handle to the channels to be listed. 										
	Handle	 						channelsHandle;
	
			// Handle to the list of classes to be listed. 								
	Handle		 					classHandle;
	
			// Handle to eigenvalues and eigenvectors that were computed for		
			// the reduced features set.														
	Handle							eigenValueHandle;
	Handle							eigenVectorHandle;
	
			// Handle to the list of pointers to the channels to be listed. 		
	Handle	 						featureHandle;
	Handle							windowInfoHandle;
	
	float								optimizeThreshold;
	float								outlierThreshold1;
	float								outlierThreshold2;
	
	UInt32							maximumPixelsPerClass;
	UInt32							minimumThresholdNumber;
	UInt32							numberClasses;
	
			// Code for the algorithm to be used.  2 = discriminant analysis,		
			// 3 = decision boundary.															
	SInt16							algorithmCode;
	
	SInt16							channelSet;
	SInt16							classSet;
	SInt16							fileInfoVersion;
	SInt16							preprocessCode;
	
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16							outputStorageType;
	
	UInt16							numberChannels;
	
			// Flag indicating (if true) that the list data specifications have	
			// not been loaded the first time.												
	Boolean							firstTime;
	Boolean							listTransformationFlag;
	Boolean							optimizeClassFlag;
	Boolean							specialOptionsFlag;
	
	} FeatureExtractionSpecs, *FeatureExtractionSpecsPtr;
                        

typedef struct MFileInfo
	{
	#if defined multispec_mac || defined multispec_mac_swift
		CMFileStream					fileStream;
	#endif	// defined multispec_mac || defined multispec_mac_swift
	
	#if defined multispec_win
		CMFileStream					*fileStreamCPtr;
	#endif	// defined multispec_win
	
   #if defined multispec_wx
      CMFileStream *fileStreamCPtr;
   #endif	// defined multispec_wx

	HierarchalFileFormatPtr		hfaPtr;
	BlockFormatPtr					blockFormatPtr;
	void*								gdalDataSetH;
		
			// Used to store the hdf5 file_id when using the hdf routines to read
			// lines of the image file.
	void*								hdf5FileID;
	
	Str15								channelDescriptionUnitString;
	
	double							maxDataValue;
	double							minDataValue;
	
	double							maxUsableDataValue;
	double							minUsableDataValue;
		
	double							noDataValue;
	
			// Pointer to the vector that relates the channel index to hdf the index for 
			// the data set information.	
	UInt16*							channelToHdfDataSetPtr;
	
	Handle							channelDescriptionH;
	
	Handle							channelValuesHandle;
	
	Handle							classDescriptionH;
	
			// Handle for the vector that relates the channel index to hdf the index for 
			// the data set information.	
	Handle							channelToHdfDataSetHandle;
	
	Handle							controlPointsHandle;
	
			// List indicating the relationship of group and palette index and
			// class number and group number.
	Handle							groupTablesHandle;
	
			// Handle to group names.
	Handle							groupNameHandle;
	
			// Handle for hdf data set information
	Handle							hdfHandle;
	
			// Handle for hierarchal format structure
	Handle							hfaHandle;
	
			// Handle to map projection information.
	Handle							mapProjectionHandle;
		
			// Used to store the hdf file_id when using the hdf routines to read
			// lines of the image file.
	SInt32							hdf4FileID;
	
	SInt32							signedValueOffset;
	
	UInt32							blockFormatIndex;
	UInt32							bytesPer1chanAllLines;
	UInt32							bytesPer1lineAllChans;
	UInt32							bytesPer1line1chan;
	UInt32							colorTableOffset;
	UInt32							dataConversionCode;
	UInt32							maxClassNumberValue;
	UInt32							numberBins;		// 256, 1024, 4096, etc
	
			// Number classes will be used for Thematic type files.
			
	UInt32							numberClasses;
	UInt32							numberColumns;
	UInt32							numberHeaderBytes;
	UInt32							numberLines;
	UInt32							numberPreChannelBytes;
	UInt32							numberPreLineBytes;
	UInt32							numberPostChannelBytes;
	UInt32							numberPostLineBytes;
	UInt32							numberTrailerBytes;
	UInt32							startColumn;
	UInt32							startLine;
	
			// Info from gdal libraries
			
	UInt32							blockHeight;
	UInt32							blockWidth;
	
			// File format for the ancillary information, include the class names and
			// colors. Usually this is the same format as that used for 'format' above
			// but the user may want to load the class names and colors for the tiff format
			// from a .trl file.
	SInt16							ancillaryInfoformat;
	
	SInt16							bandInterleave;	// 1=BIL,   2=BSQ,	 3=BIS
	
			// Code indicating whether the channels are in wavelength order
			// 0=no applicable; 1=in wavelength order; 2=not in wavelength order
			// This variable is only used when creating new image files to indicate
			// whether the output image files are in wavelength order.
	SInt16							channelsInWavelengthOrderCode;
	
			// For Thematic image files, this variable indicates how the classes were
			// collapsed if any. In otherwords, a classSymbolPtr vector was created to
			// indicate which classes actually exist according the user request.
			//		=1, not collapse; 
			//		=2, collapse to indicate just the classes that exist;
			// 	=3, collapsed down to just the high class value.
	SInt16							collapseClassSelection;
	
	SInt16							colorTableValueBytes;
	
			// fileExists indicates whether file exists on the volume.
			// -1=don't know, 0=no, 1=yes
	SInt16							fileExists;	
	
			// File format: 1=Erdas73, 2=MultiSpec, 3=Aviris, 4=Default,
			// 5=SunScreenDump, 6=MultiSpecClassification, 7=Erdas74
	SInt16							format;
	
			// Stores the type of band interleave that file being read by gdal routines
			//	is in. MultiSpec treats gdal read data as BSQ since that is how the
			// data are returned as for MultiSpec to use.
	SInt16							gdalBandInterleave;
	
			// Used for gdal library of routines to read data.
	SInt16							gdalDataTypeCode;
	
	SInt16							maxNumberDescriptionCharacters;
	
			// Code indicating the type of data compression used
			//		= 0, no compression
			//		= 1, RLE (run length encoding
			//		= 2, NBIT compression
			//		= 3, Skipping Huffman compression
			//		= 4, GZIP compression
			//		= 5, Szip compression
	UInt16							dataCompressionCode;
	
			// Code indicating whether the data values are integer or float
			//		= 0, kIntegerType
			//		= 1, kRealType
	UInt16							dataTypeCode;
	
			// Code providing information about the description for the channels.
			//		Bit 0 = 0x0: no description information for the channels
			//		Bit 0 = 0x1: description information exists
			//		Bit 1,2 = 0x00: no information exists whether reflective or thermal
			//		Bit 1,2 = 0x01: Reflective data only
			//		Bit 1,2 = 0x10: Thermal data only
			//		Bit 1,2 = 0x11: Both reflective and thermal data exists
			//		Bit 3 = 0x0: no band width information
			//		Bit 3 = 0x1: band width information exists
	UInt16							descriptionCode;
	
	UInt16							hdfDataSetSelection;
	UInt16							numberHdfDataSets;
	
			// Code for system that data represents: 1=Landsat MSS, 2=Landsat MSS5
			// 3=Landsat TM, 4=Landsat TM7, 5=SPOT, 6=SPOT4, 
	UInt16							instrumentCode;
	UInt16							maxNumberEDecimalDigits;
	UInt16							maxNumberFDecimalDigits;
	UInt16							signedBitMask;
	UInt16							signedValueMask;
	
	UInt16							numberBits;			// Common is 8 or 12 bits
	
	UInt16							numberBytes;
	
	UInt16							numberChannels;
	
			// Number of groups in thematic type images.
	UInt16							numberGroups;
	
	Boolean							asciiSymbols;
	Boolean							blockedFlag;
	Boolean							callGetHDFLineFlag;
	Boolean							classChangedFlag;
	Boolean							classesComputedFlag;
	
			// This flag is used to indicate whether the class names for the 1-channel thematic display
			// have been loaded into the classDescriptionH structure.
	Boolean							classNamesLoadedFlag;
	Boolean							descriptionsFlag;
	Boolean							treatLinesAsBottomToTopFlag;
	Boolean							groupChangedFlag;
	Boolean							nonContiguousStripsFlag;
	Boolean							noDataValueFlag;
	Boolean							signedDataFlag;
	Boolean							swapBytesFlag;					// 0=No		1=Yes
	Boolean							swapColorTableBytes;			// 0=No		1=Yes
	Boolean							swapHeaderBytesFlag;			// 0=No		1=Yes
	Boolean							thematicType;
	
	//SInt32							filler;			// to force structure to be even multiple of 8
	
	} MFileInfo, *FileInfoPtr, **FileInfoHandle;


typedef struct FileIOBuffer
	{
	union {
		unsigned short int	twobyte[1];	// Vector for a line of 16 bit data
		unsigned	char			onebyte[1];	// Vector for a line of  8 bit data
		} data;
			
	} FileIOBuffer, *FileIOBufferPtr;
	

typedef struct FileIOInstructions
	{
	ThreadID								asyncIOThread;
	ThreadID								wakeUpThread;
	ThreadTaskRef						theAppRef;
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	HUInt16Ptr							channelListPtr;
	HUCharPtr							inputBufferPtrs[2];
	HUCharPtr							outputBufferPtrs[2];
	HUCharPtr							tiledBufferPtrs[2];
	int*									gdalChannelListPtr;
	LayerInfoPtr						layerInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	SInt32								columnEnd;
	SInt32								columnInterval;
	SInt32								columnStart;
	SInt32								lineEnd;
	SInt32								lineInterval;
	SInt32								lineStart;
	SInt32								nextLineToRead;
	SInt32								numberColumns;
	SInt32								numberLinesAtOnce;
	
	UInt32								bilSpecialNumberChannels;
	UInt32								bufferOffset;
	UInt32								channelEnd;
	UInt32								channelStart;
	UInt32								count;
	UInt32								numberChannels;
	UInt32								numberGdalChannels;
	UInt32								numberOutputBufferSamples;
	UInt32								posOff;
	
	SInt16								bufferUsedForIO;
	SInt16								bufferUsedForApp;
	SInt16								errCode;
	
			// Code to indicate how many bytes the input data are to be forced to
			// in the output buffer to be used. The choices are:
			//		kDoNotForceBytes - leave as originally stored in the image file.
			//		kForce1Byte
			//		kForce2Bytes
			//		kForce4Bytes
			//		kForceFloat4Bytes
			//		kForceFloat8Bytes
	UInt16								forceByteCode;
	
	Boolean								bufferReadyFlag;
	Boolean								doneFlag;
	Boolean								forceBISFormatFlag;
	Boolean								oneReadFlag;
	Boolean								packDataFlag;
	Boolean								fileIOThreadStartedFlag;
	
	Boolean								callPackLineOfDataFlag;
	Boolean								differentBuffersFlag;
	
			// Mask Information.
			
	HUInt16Ptr							maskBufferPtr;
	UInt32								maskColumnStart;
	UInt32								maskValueRequest;
	UInt32								numberMaskColumnsPerLine;
	
	} FileIOInstructions, *FileIOInstructionsPtr;
	
	
typedef struct GeodeticModelInfo
	{
			// Radius of spheroid.						
	double					radiusSpheroid;
	
			// The semi major axis of the ellipsoid.						
	double					semiMajorAxis;
	
			// The semi major axis of the ellipsoid.						
	double					semiMinorAxis;
	
			// The eccentricity.						
	double					e;
	
			// The eccentricity squared.						
	double					eSquared;
	
			// A constant used in the Polar Stereogrphic projection.						
	double					e4;
	
			// Flattening of ellipsoid. Used in Lambert Conformal Conic.						
	double					f0;
	
			// Sign variable used in the Polar Stereogrphic projection.						
	double					fac;
	
			// Small m constant used in the Polar Stereogrphic projection.						
	double					mcs;
	
			// Distance from equator to latitude of origin.						
	double					M0;
	
			// Ratio of angle between meridian. Used in Lambert Conformal Conic.						
	double					ns;
	
			// Height above ellipsoid. Used in Lambert Conformal Conic.					
	double					rh;
	
			// Small t constant used in the Polar Stereogrphic projection.						
	double					tcs;
	
			// Flag used in the Polar Stereogrphic projection.						
	SInt32					ind;
	
			// Code for datum.											
	SInt16					datumCode;
	
			// Code for spheroid name.											
	SInt16					spheroidCode;
	
	} GeodeticModelInfo, *GeodeticModelInfoPtr;
	
	
typedef struct GridCoordinateSystemInfo
	{
			// EPSG Name
	Str63						epsgName;
	
			// Datum Name
	Str63						datumName;
	
			// Ellipsoid Name
	Str63						ellipsoidName;
	
			// Variable For Map Projection
			// False Easting (ProjFalseEastingGeoKey)									
	double					falseEasting;
	
			// False Northing (ProjFalseNorthingGeoKey)								
	double					falseNorthing;
	
			// False Origin Easting (ProjFalseOriginEastingGeoKey)									
	double					falseOriginEasting;
	
			// False Origin Northing (ProjFalseOriginNorthingGeoKey)								
	double					falseOriginNorthing;
	
			// False Origin Latitude (ProjFalseOriginLatGeoKey)									
	double					falseOriginLatitude;
	
			// False Origin Longitude (ProjFalseOriginLongGeoKey)								
	double					falseOriginLongitude;
	
			// Latitude of origin (ProjNatOriginLatGeoKey or ProjCenterLatGeoKey)								
	double					latitudeOrigin;
	
			// Longitude of central meridian (ProjNatOriginLongGeoKey or ProjCenterLongGeoKey)				
	double					longitudeCentralMeridian;
	
			// Scale factor at origin (ProjScaleAtNatOriginGeoKey)			
	double					scaleFactorOfCentralMeridian;
	
			// Standard parallel1 (ProjStdParallel1GeoKey)				
	double					standardParallel1;
	
			// Standard parallel2 (ProjStdParallel2GeoKey)				
	double					standardParallel2;
	
			// Azimuth of the center line (ProjAzimuthAngleGeoKey)				
	double					projAzimuthAngle;
	
			// Code for the grid reference system									
			//  1=Greek Geodetic Reference System								
			//  2=State Plane											
			//  3=Universal Transverse Mercator (UTM)										
	SInt16					referenceSystemCode;
	
			// Code for the map projection.									
			//  1=Alaska Conformal								
			//  2=Albers Conical Equal Area												
	SInt16					projectionCode;
	
			// Coordinate grid zone.
			//    < 0 is northern hemisphere. > 0 is southern hemisphere											
	SInt16					gridZone;
	
	} GridCoordinateSystemInfo, *GridCoordinateSystemInfoPtr;
	

typedef struct HdfDataSets 
	{
	UInt8									name[80];
			// HDF Data SD data set identifier.
	void*									sdid;
	SInt32								vRefNum;
	SInt32								dirID;
			// HDF Data SD data set identifier.
	//SInt32								sdid;
	UInt32								dataSetOffsetBytes;
	SInt16								dataSet; 
			// 1 indicates that it is an 8 or 24 bit image
			// 3 indicates that it is a scientific data set
	SInt16								dataSetType;
	SInt16								groupedNumber;
	SInt16								instrumentChannelNumber;
	
	}	HdfDataSets;
		
		
		//	This structure defines the hierarchal file format components for
		// those file formats which use this type of file structure.												
	
typedef struct HierarchalFileFormat
	{
	SInt64							layerOffsetBytes;
	
	HUCharPtr						tiledBufferPtr;
	
	UInt32							blockHeight;
	UInt32							blocksPerChannel;
	UInt32							blockSize;
	UInt32							blockSizeRead;
	UInt32							blockWidth;
	UInt32							blockWidthRead;
	UInt32							numberBlockHeights;
	UInt32							numberBlockWidths;
	UInt32							blockOffset;
	
	UInt32							firstLineRead;
	UInt32							lastLineRead;
	
	UInt32							firstColumnRead;
	UInt32							firstColumnStartByte;
	UInt32							lastColumnRead;
	UInt32							numberBlocksRead;
	
	} HierarchalFileFormat, *HierarchalFileFormatPtr;
		
		
		//	This structure defines the histogram block data to be used in 			
		//	computing the histogram statistics												
	
typedef struct HistogramSpecs
	{
			// Number of pixels used for histogram statistics.							
	SInt64					totalPixels;
	
			// Overall min and max values used when listing histogram values.		
	double					overallMaxValue;
	double					overallMinValue;
		
			// Handle for the histogram array information.								
	Handle					histogramArrayH;
		
			// List of channels to be histogrammed.										
	Handle					channelsHandle;
		
			// Buffer to use to read the statistics information from disk.			
	CharPtr					statBufferPtr;
	
			// Pointer to the histogram summary structure.
	HistogramSummaryPtr		histogramSummaryPtr;
		
			// Pointer for the histogram array information.								
	HUInt32Ptr				histogramArrayPtr;
		
			// First line to use to compute histogram statistics. 					
	UInt32					lineStart;
		
			// Last line to use to compute histogram statistics. 						
	UInt32					lineEnd;
		
			// Line interval to use to compute histogram statistics.					
	UInt32					lineInterval;
		
			// First column to use to compute histogram statistics. 					
	UInt32					columnStart;
		
			// Last column to use to compute histogram statistics. 					
	UInt32					columnEnd;
		
			// Column interval to use to compute histogram statistics.				
	UInt32					columnInterval;
	
			// Maximum number of bins to be used.
	UInt32					maxNumberBins;
		
			// Number of bad lines in the image.											
	SInt32					numBadLines;
		
			// Indicates whether all (=0) or a subset (=1) of the channels are	
			// to be histogrammed.																
	SInt16					channelSet;
		
			//	Variable indicating method desired to determine the histogram		
			// statistics.  "Maximum range" = 1. "Compute" = 2. "Read from file	
			// = 3.																					
	SInt16					method;
		
			//	Number of channels histogramed.												
	SInt16					numberChannels;
		
			//	Number of channels histogramed.												
	SInt16					numberStatBufferChannels;
		
			//	Number of channels histogramed.												
	SInt16					numberSummaryChannels;
		
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16					outputStorageType;
	
			// Variable to indicate the format used for the current default histogram
			// statistics file.
	SInt16					statFileFormat;
		
			// Flag indicating whether all channels can be processed at once		
			// when getting the histogram information.									
	Boolean					allChannelsAtOnceFlag;
		
			// Flag indicating if histogram is to be computed.							
	Boolean					computeFlag;
		
			// Flag indicating whether the histogram is based on every line and column.												
	Boolean					everyLineFlag;
		
			// Flag indicating (if true) that the histogram specifications have	
			// not been loaded the first time.												
	Boolean					firstTime;
		
			// Flag indicating whether histogram information has changed since	
			// the last image display.															
	Boolean					histogramChangedSinceDisplayFlag;
		
			// Flag indicating if empty bins are to be listed. 						
	Boolean					includeEmptyBinsFlag;
		
			// Flag indicating if this structure needs to be initialized. 			
	Boolean					initializeStructureFlag;
		
			// Flag indicating if the histogram is to be listed in a line 			
			// format. 																				
	Boolean					lineFormatHistFlag;
		
			// Flag indicating if histogram is to be listed. 							
	Boolean					listHistFlag;
		
			// Flag indicating if histogram summary is to be listed. 				
	Boolean					listHistSummaryFlag;
		
			// Flag indicating that the histogram is loaded into the structure.	
	Boolean					loadedFlag;
		
			// Flag indicating if only min, max, mean and standard deviation is	
			// to be computed. 																	
	Boolean					minMaxMeanSDOnlyFlag;
				
	}	HistogramSpecs, *HistogramSpecsPtr; 
	
	
			// This structure defines the histogram for a channel in the image	
			
typedef struct HistogramSummary
	{
	double 					averageValue;	// Average value for channel
	
	double					binFactor;		// Value used to convert data value
													//		to bin number		
		
	double				 	maxNonSatValue; // Maximum nonsaturated value for 	
													 // 	channel
	
	double				 	maxValue;		// Maximum value for channel		
		
	double					medianValue;	// Median value for channel
		
	double				 	minNonSatValue; // Minimum nonsaturated value for 	
													 //	channel							
															
	double 					minValue;		// Minimum value for channel				
		
	double 					stdDeviation;	// Standard deviation of data 		
													// 	values for channel					
		
	SInt32					badValues;		// Number of bad values for channel	
													 
	UInt32					numberBins;		// Number bins used for histogram
													// 	including the min and max
													// 	saturated bins.	
													
	SInt16					binType;			// = 0 kBinWidthOfOne
													// = 1 kBinWidthNotOne indicates that 
													// 	values are packed into the bin. 
													// 	One will need to computethe bin width.
													// = 2 kDataValueIsBinIndex or bin index is the
													//    data value.	
													
	UInt16					numberEDecimalDigits;	// number of decimal places to be
													// used when print out the values for this
													// channel for E format.
													
	UInt16					numberFDecimalDigits;	// number of decimal places to be
													// used when print out the values for this
													// channel for F format.					
		
	Boolean					availableFlag;	// Indicates that information is		
													// available for the channel.			
		
	} HistogramSummary, *HistogramSummaryPtr;
	
	
typedef struct PlanarCoordinateSystemInfo
	{
			// Storage for coefficients if needed.
			
	double*					easting1CoefficientsPtr;
	double*					northing1CoefficientsPtr;
	double*					easting2CoefficientsPtr;
	double*					northing2CoefficientsPtr;
	
			// Conversion factor from meters to native map units
	double					metersToNativeFactor;
	
			// Horizontal Pixel Size.	(For Map Projection.)							
	double					horizontalPixelSize;
	
			// Orientation of the image from north in degrees.
	double					mapOrientationAngle;
	
			// Area of each pixel in 'areaUnitCode' units.								
	double					pixelArea;
	
			// Vertical Pixel Size.	(For Map Projection.)								
	double					verticalPixelSize;
	
			// x-map coordinate origin for orientation angle 							
	double					xMapOrientationOrigin;
	
			// x-Map Coordinate.	(For Map Projection.)									
	double					xMapCoordinate11;
	
			// x-map coordinate origin for orientation angle 							
	double					yMapOrientationOrigin;
	
			// y-Map Coordinate.	(For Map Projection.)									
	double					yMapCoordinate11;
	
			// Code for the area units.											
			// 0=None.																				
			// 1=Acre.																				
			// 2=Hectare.																			
			// 3=Other.																				
	SInt16					areaUnitsCode;
	
			// Code for the distance units.											
			// 0=None.																				
			// 1=meters.																				
			// 2=feet.																			
			// 3=yards.																			
			// 3=kilometers.																				
	SInt16					mapUnitsCode;
	
			// Polynomial Model Order.  0 implies no polynomial model used											
	SInt16					polynomialOrder;
	
			// Code for the z distance units. In other words DEM data.											
			// 0=None.																				
			// 1=meters.																				
			// 2=feet.																			
			// 3=yards.																			
			// 3=kilometers.																				
	SInt16					zMapUnitsCode;
	
			// This is a code to indicate the direction that the map units go.
			// Don't know if this will be used much but it is here just in case.
			// For the x-direction +1 indicates increasing from left to right.
			// For the y-direction +1 indicates increasing from bottom to top.
			// -1 one indicates the opposite.
	//SInt16					xDirectionCode;
	//SInt16					yDirectionCode;
	
	} PlanarCoordinateSystemInfo, *PlanarCoordinateSystemInfoPtr;
	
	
typedef struct ImageOverlayInfo
	{
	Str255								overlayName;
	
	#if defined multispec_mac
		CGInfo								cgInfo;
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		HDC									overlayDC;
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		wxBitmap*								overlayBitmapPtr;
	#endif

			// Handle for color palette for overlay image.
	CMPaletteInfo						paletteObject;
	
	PlanarCoordinateSystemInfo		planarCoordinate;
	
	DoubleRect							boundingMapRectangle;
	
	SInt32								colorTable[256];
	
	LongRect								lineColumnRect;
												
	GWorldPtr							offscreenGWorld;
	
	Handle								offScreenMapHandle;
	
	Handle								offscreenStorageHandle;
	
	UInt32								offscreenMapSize;
	UInt32								rowBytes;
	
			// Code indicating whether to draw the base image when a image overalay
			// is to be drawn over the base image. One does not need to draw the base
			// image in general when:
			//		- the overlay covers the entire base image (bit 1)
			//		- the column and line interval for the image overlay is 1 (bit 2)
			//		- the transparency of the overlay is 0  (bit 3)
			//		- an overlay is selected to be drawn	(bit 4)
			// A code value of 15 indicates the base image is not to be drawn
	UInt16								drawBaseImageCode;
	
	Boolean								usePlanarCoordinateInfoFlag;
	
	} ImageOverlayInfo, *ImageOverlayInfoPtr;
	
	
typedef struct ImageOverlaySpecs
	{
	#if defined multispec_wx
		double					opacityLoaded;
	#endif
	double					opacity;
	
	char						index;
	
	} ImageOverlaySpecs, *ImageOverlaySpecsPtr;
	
	
			// This structure defines the second order statistics for the image	
			
typedef struct ImageSecondOrderStats
	{
	// Number of pixels used for second order image statistics.							
	SInt64					totalPixels;
	
	Handle					sumsHandle;
	
	Handle					sumSquaresHandle;
		
			// First line to use to compute  second order image statistics. 					
	UInt32					lineStart;
		
			// Last line to use to compute  second order image statistics. 						
	UInt32					lineEnd;
		
			// Line interval to use to compute second order image statistics.				
	UInt32					lineInterval;
		
			// First column to use to compute second order image statistics. 					
	UInt32					columnStart;
		
			// Last column to use to compute second order image statistics. 					
	UInt32					columnEnd;
		
			// Column interval to use to compute second order image statistics.				
	UInt32					columnInterval;
					
			//	Number of channels statistics were computed for.												
	SInt16					numberChannels;	
		
	} ImageSecondOrderStats, *ImageSecondOrderStatsPtr;
	
	
typedef struct InverseMatrixMemory
	{
	HDoublePtr				inversePtr;
	HDoublePtr				pivotPtr;
	
	SInt16					*indexColPtr;
	SInt16					*indexRowPtr;
	SInt16					*ipvotPtr;
	
	} InverseMatrixMemory, *InverseMatrixMemoryPtr;
	
		
		
			// This structure defines the relationship between window channels or	
			// layers and the file information structures for that channel or 		
			// layer.  In general the LayerInfo structure will be a vector of 		
			// structures with one for each channel or layer.								
	
typedef struct LayerInfo
	{
			// Index into the FileInfo structure for the given channel number.	
	UInt16					fileInfoIndex;
	
			// The channel number within the defined FileInfo structure which		
			// corresponds "Window" file channel. This value is 0 based.
	UInt16					fileChannelNumber;
	
			// Value indicating the channel which is in wavelength order for the
			// respective index in LayerInfo. This would be the channel for
			// a layer info structure for case when the channels are in wavelength
			// order. This value is 0 based. This is used when using change image
			// file format to put a new file in wavelength order.
	UInt16					wavelengthOrder;
	
			// Value indicating the index of this channel within the wavelength
			// order of all channels associated with the window. By default the
			// value will just be the index in the LayerInfo structure. This value
			// is 0 based. This is used when putting the wavelengths in proper order
			// for selection and list data graphs.
	UInt16					wavelengthIndex;
	
	} LayerInfo, *LayerInfoPtr, **LayerInfoHandle;
	
	
typedef struct LCToWindowUnitsVariables
	{
			// display magnification
	double								magnification;
	
			// displayed offscreen origin.
	SInt32								xOrigin;
	SInt32								yOrigin;
	
			// displayed line and column interval
	SInt32								columnInterval;
	SInt32								lineInterval;
	
			// displayed line/column offsets.
	SInt32								columnOffset;
	SInt32								lineOffset;
	
			// line and column scroll offsets. Used for Windows version.
	SInt32								columnScrollOffset;
	SInt32								lineScrollOffset;
	
			// Location of top of image in window units
	SInt32								imageTopOffset;
	
			// Location of left of image in window units
	SInt32								imageLeftOffset;
													
			// Offset in window units for the first channel	
			// that has at least part of it being displayed. Needed to allow
			// for side by side displays.
	SInt32								channelWindowOffset;				
	
	} LCToWindowUnitsVariables, *LCToWindowUnitsVariablesPtr;
	
	
typedef struct ListDataSpecs
	{
			// Pointer to the list of pointers to the channels to be listed. 		
	Handle		 			featureHandle;
	
			// Pointer to the channels to be listed. 										
	Handle	 				channelsHandle;
	
			// Pointer to the list of classes to be listed. 							
	Handle	 				classHandle;
	
			// Pointer to the list of fields to be listed. 								
	Handle					fieldHandle;
	
	Handle					windowInfoHandle;
	SInt32					lineStart;
	SInt32					lineEnd;
	SInt32					lineInterval;
	SInt32					columnStart;
	SInt32					columnEnd;
	SInt32					columnInterval;
	
	UInt32					numberClasses;
	
	SInt16					fileInfoVersion;
	
			// Variable indicating the format of the output.
			//		=0 channel data to be listed in lines
			//		=1 channel data to be listed in columns
	SInt16					formatCode;
	SInt16					classSet;
	SInt16					channelSet;
	SInt16					fieldSet;
	SInt16					fieldTypeCode;
	SInt16					latLongUnitsCode;
	
			// Number of decimal places to use for listing real type data.
			// The user has a chance to set this to something different than a
			// computed default.
	UInt16					numberFDecimalDigits;
	
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16					outputStorageType;
	
			// Variable indicating the histogram output format. 						
			//		(outputFormatCode & 0x0001) = group channels.						
			//		(outputFormatCode & 0x0002) = group columns.							
	SInt16					outputFormatCode;
	
	UInt16					numberChannels;
	UInt16					numberFields;
	
			// Flag indicating (if true) that the list data specifications have	
			// not been loaded the first time.												
	Boolean					firstTime;
	Boolean					includeLineColumnValuesFlag;
	Boolean					includeLatLongValuesFlag;
	Boolean					includeClassFieldFlag;
	Boolean					listClassesDataFlag;
	Boolean					listSelectedAreaDataFlag;
	Boolean					projectFlag;
	Boolean					graphDataFlag;
	
	} ListDataSpecs, *ListDataSpecsPtr;
	
	
typedef struct ListResultsSpecs
	{
	double					probabilityThreshold;
	double					areaConversionFactor;
	Handle					classAreaHandle;
	Handle					classHandle;
	Handle					fileInfoHandle;
	Handle					probabilityWindowInfoHandle;
	SInt32					lineStart;
	SInt32					lineEnd;
	SInt32					lineInterval;
	SInt32					columnStart;
	SInt32					columnEnd;
	SInt32					columnInterval;
	SInt32					probabilityBufferOffset;
	UInt32					numberClassAreas;
	UInt32					numberClasses;
	SInt16					fileInfoVersion;
			// Variable indicating which areas the result are to be listed for.	
			//		& 0x0001, training; & 0x0002, test; & 0x0004, selected area;	
	SInt16					areaCode;
	
			// Variable indicating the type of summary to be listed.					
			//		& 0x0001, by field; & 0x0002, by classes; & 0x0004, by groups.	
	SInt16					summaryCode;
	
			// Variable indicating the units to be used in the listing.				
			//		& 0x0001, samples; & 0x0002, percent; & 0x0004, area.				
	SInt16					unitsCode;
	SInt16					summaryUnitsCode;
	
			// Variable indicating the type of table to be listed.					
			// = 1 is class type (columns of table are classes;						
			// = 2 is group type (columns of table are groups.							
	SInt16					tableType;
	SInt16					classAreaSet;
	SInt16					classSet;
			// Variable to indicate where list results output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16					outputStorageType;
	SInt16					probabilityThresholdCode;
	SInt16					thresholdTypeCode;
	Boolean					thresholdFlag;
	
	} ListResultsSpecs, *ListResultsSpecsPtr;
	
	
typedef struct MapProjectionInfo
	{
			// Structure for geodetic model description.
	GeodeticModelInfo					geodetic;
	
			// Structure for grid coordianate system information.
	GridCoordinateSystemInfo		gridCoordinate;
	
			// Structure for planar coordianate system information.
	PlanarCoordinateSystemInfo		planarCoordinate;
	
			// Handle to memory to store polynomial coefficients in.
	Handle								coefficientsHandle;
	
			// Number coefficients stored in the coefficient vectors.
	UInt16								numberCoefficients;
	
			// Key used by the GeoTIFF format.
	UInt16								projectedCSTypeGeoKey;
	
	} MapProjectionInfo, *MapProjectionInfoPtr;
	
	
typedef struct MapToWindowUnitsVariables
	{
			// display magnification
	double								magnification;
	
			// x and y origin of the image in the window in map units
	double								mapXOrigin;
	double								mapYOrigin;
	
			// Map units for upper left pixel of the image.  This is need
			// if the map orientation is not 0.
	double								xMap11;
	double								yMap11;
	
			// x and y pixel size in map units taking into account the
			// display interval
	double								xMapPixelSize;
	double								yMapPixelSize;
	
			// This offset allows one to handle the case where the pen hangs
			// from the upper left point. One can subtract 0.5 window units 
			// from calculation to get the closest fit. For just calculating a
			// window rectangle, one does not want to subtract a half unit but
			// go to the nearest window unit.
	double								offset;
	
			// cosine and sine of the map orientation angle.  These variables
			// are only needed when the map orientation angle is not 0.
	double								cosOrAngle;
	double								sinOrAngle;
	
			// Pointers to the coefficients to be used to convert from map
			// units to line/column values.
	double*								easting2CoefficientsPtr;
	double*								northing2CoefficientsPtr;
	
			// Location of top of image in window units
	SInt32								imageTopOffset;
	
			// Location of left of image in window units
	SInt32								imageLeftOffset;
													
			// Offset in window units for the first channel	
			// that has at least part of it being displayed. Needed to allow
			// for side by side displays.
	SInt32								channelWindowOffset;
	
			// window column and line display origin.
	SInt32								windowColumnOrigin;
	SInt32								windowLineOrigin;
	
			// displayed column and line intervals.
	SInt32								displayedColumnInterval;
	SInt32								displayedLineInterval;
	
			// displayed offscreen origin.
	SInt32								xOrigin;
	SInt32								yOrigin;
	
			// Order for polynomial transformations between line/column and
			// map units and vice versa. 
	SInt16								polynomialOrder;	
	
			// Flag indicating whether the map orientation angle needs to be
			// taken into account.
	Boolean								useMapOrientationAngleFlag;		
	
	} MapToWindowUnitsVariables, *MapToWindowUnitsVariablesPtr;
	
	
typedef struct MaskInfo
	{
	CMFileStream*					fileStreamPtr;
	HUInt16Ptr						maskPointer;
	HUInt16Ptr						maskValueToFieldPtr;
	
	Handle							fileStreamHandle;
	Handle							maskHandle;
	Handle							maskValueToFieldHandle;
	
	UInt32							fileLayer;
	UInt32							maxMaskValue;
	UInt32							numberColumns;
	UInt32							numberLines;
	UInt32							numberLayers;
	UInt32							startColumn;
	UInt32							startLine;
	
	} MaskInfo, *MaskInfoPtr;
	

typedef struct NonProjProcessorSpecs
	{
		// Handle to last settings of biplot data specifications.					
	Handle					biPlotDataSpecsH;
	
		// Handle to last settings of cluster specifications.							
	Handle					clusterSpecsH;
	
		// Handle to last settings of list data specifications.						
	Handle					listDataSpecsH;
	
		// Handle to last settings of list results specifications.					
	Handle					listResultsSpecsH;
	
		// Handle to last settings of principal components specifications.		
	Handle					principalCompSpecsH;
	
		// Handle to last settings of reformat processor specifications.		
	Handle					reformatOptionsSpecsH;
	
			// Handle to last settings of the create statistics images				
			// specifications.																	
	Handle					statisticsImageSpecsH;
	
			// Handle to last settings of the transformation matrix checks.																	
	Handle					evalTransformationSpecsH;
	
	} NonProjProcessorSpecs, *NonProjProcessorSpecsPtr;
	
	
typedef struct OverlaySpecs
	{
	RGBColor					overlayColor;
	
	UInt16					lineThickness;
	
	char						index;
	
	} OverlaySpecs, *OverlaySpecsPtr;
	
	
typedef struct PrincipalCompSpecs
	{
			// Pointer to the list of pointers to the channels to be listed. 		
	Handle	 						featureHandle;
	
			// Pointer to the channels to be listed. 										
	Handle	 						channelsHandle;
	
			// Pointer to the list of classes to be listed. 							
	Handle		 					classHandle;
	
	Handle							eigenValueHandle;
	Handle							eigenVectorHandle;
	Handle							totalSumSquaresStatsHandle;
	Handle							totalChanStatsHandle;
	Handle							windowInfoHandle;
	SInt32							columnStart;
	SInt32							columnEnd;
	SInt32							columnInterval;
	SInt32							lineStart;
	SInt32							lineEnd;
	SInt32							lineInterval;
	UInt32							numberClasses;
	SInt16							fileInfoVersion;
	SInt16							classSet;
	SInt16							channelSet;
	SInt16							dataCode;
	SInt16							eigenvectorCode;
	SInt16							fieldTypeCode;
	
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16							outputStorageType;
	
			// Variable indicating the histogram output format. 						
			//		(outputFormatCode & 0x0001) = group channels.						
			//		(outputFormatCode & 0x0002) = group columns.							
	SInt16							outputFormatCode;
	
	UInt16							numberChannels;
	
	Boolean							equalVariancesFlag;
			// Flag indicating (if true) that the specifications have				
			// not been loaded the first time.												
	Boolean							firstTime;
	Boolean							listEigenvectorFlag;
	Boolean							projectFlag;
	Boolean							saveEigenvectorFlag;
	
	} PrincipalCompSpecs, *PrincipalCompSpecsPtr;
	
	
typedef struct ProjectClassNames
	{
	UInt8								name[32];
	double							looCovarianceValue;
	double							userMixingParameter;

	SInt64							numberStatisticsPixels;
	SInt64							numberTrainPixels;
	
	// priorWeights[0] = unequal weights for original statistics
			// priorWeights[1] = unequal weights for enhanced statistics
			// priorWeights[2] = equal weights value
			// priorWeights[3] = weighting for computation of common covariance
	float								priorWeights[4];
	
	SInt16							classNumber;
	SInt16							firstFieldNumber;
	SInt16							lastFieldNumber;
	
			// Code indicating the requested project mixing parameter type.
			//		= 1, computed optimum looc mixing parameter to be used.
			//		= 2, user set mixing parameter to be used.
			//		= 3, identity matrix to be used for covariance matrix.
	SInt16							mixingParameterCode;
	
	SInt16							modifiedStatsStorage;
	SInt16							numberOfFields;
	SInt16							numberOfTestFields;
	SInt16							numberOfTrainFields;
	
			// Covariance stats to be used for the class.
			// 	kOriginalStats					0x0001
			// 	kLeaveOneOutStats				0x0002
			// 	kEnhancedStats					0x0004
	UInt16							covarianceStatsToUse;
	
	Boolean							listMessageFlag;
	Boolean							modifiedStatsFlag;
	Boolean							statsUpToDate;
	
	} ProjectClassNames, *PClassNamesPtr;
	
	
typedef struct ProjectFieldIdentifiers
	{
	Str31					name;
	LongPoint			labelPoint;
	SInt64				numberPixels;
	SInt64				numberPixelsUsedForStats;
	SInt16				classStorage;
	
			// Type of field: 
			//		training field=1, kTrainingType
			//		testing field=2, kTestingType
	SInt16				fieldType;
	SInt16				firstPolygonPoint;
	SInt16				nextField;
	SInt16				numberOfPolygonPoints;
	
			// Type of coordinates defining the field: 							
			// 	rectangle=1, kRectangleType 
			//		polygon=2, kPolygonType 
			//		cluster=3, kClusterType 
			//		mask=4, kMaskType									
	SInt16				pointType;
	SInt16				previousField;
	SInt16				trainingStatsNumber;
	UInt16				maskValue;
	Boolean				loadedIntoClassStats;
	Boolean				statsUpToDate;
	
	} ProjectFieldIdentifiers, *PFieldIdentifiersPtr;
	

typedef struct ProjectFieldPoints
	{
	SInt32				line;
	SInt32				col;
	
	} ProjectFieldPoints, *PFieldPointsPtr;


typedef struct knnPoint
	{
   //int 		label;
   int 		index;
   double 	distance;
	
	} knnType;


//#if PRAGMA_ALIGN_SUPPORTED
#if defined PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#endif

typedef struct ProjectInfo
	{
	#if defined multispec_mac || defined multispec_mac_swift
		CMFileStream					fileStream;
	#endif	// defined multispec_mac || defined multispec_mac_swift
	
	#if defined multispec_win
		CMFileStream*					fileStreamCPtr;
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		CMFileStream*					fileStreamCPtr;
	#endif	// defined multispec_wx

				// used in SVM classify training
	
	struct svm_model*				svmModel;
	struct svm_node*				svm_x;

				// used in KNN classify training

	knnType*							knnDistancesPtr;
	UInt16*							knnLabelsPtr;
	double*							knnDataValuesPtr;
	int 								knnCounter;

	UInt8								imageFileName[256];
	
			// Parameters for train and/or test mask files.
			
	MaskInfo							testMask;
	MaskInfo							trainingMask;
	
	Rect								listTitleBox;
	Rect								popUpClassBox;
	Rect								promptClassBox;
	Rect								popUpCovarianceToUseBox;
	Rect								promptCovarianceToUseBox;
	Rect								popUpFieldBox;
	Rect								promptFieldBox;
	Rect								projectNameBox;
	Rect								projectWindowModeTitleBox;
	  
	double							userMixingParameter;
	double							zeroVarianceFactor;
	
	ControlHandle					toFieldControlH;
	ControlHandle					updateControlH;
	ControlHandle					newFieldControlH;
	ControlHandle					polygonTypeControlH;
	ControlHandle					toClassControlH;
	ControlHandle					addToControlH;
	ControlHandle					toProjectControlH;
	ControlHandle					listControlH;
	ControlHandle					histogramControlH;
	ControlHandle					editNameControlH;
	
	ControlRef						projectNameControlRef;
	
	Handle							projectInfoHandle;
	Handle							classNamesHandle;
	Handle							classChanStatsHandle;
	Handle							classSumSquaresStatsHandle;
	Handle							fieldIdentifiersHandle;
	Handle							fieldCoordinatesHandle;
	Handle							fieldChanStatsHandle;
	Handle							fieldSumSquaresStatsHandle;
	Handle							channelHandle;
	
				// = 1, rectangles/polygons/cluster only
				// = 2, train/test mask files only
				// = 3, both of above
	UInt32							statSourceDefinitionCode;
	
			// Handle to group names.															
	Handle							groupNameHandle;
	
			// Handle to last settings of classify selections.								
	Handle							classifySpecsH;
	
			// Handle for class pair weights.													
	Handle 							classPairWeightsHandle;
	Handle 							classPairWeightsListHandle;
	
		// Handle to the common covariance statistics
	Handle							commonChannelStatsHandle;
	Handle							commonCovarianceStatsHandle;
	
		// Handle for class list to be used for common covariance
	//Handle	 						commonCovarianceClassListHandle;

		// Handle to last setting of evaluate covariance selections;
	Handle							evalCovarianceSpecsHandle;
	
		// Handle to last settings of feature extraction specifications.			
	Handle							featureExtractionSpecsH;
	
		// Handle to current mask value to field number vector.
	Handle							maskValueToFieldHandle;
	
		// Handles to the enhanced class statistics
	Handle							modifiedClassChanStatsHandle;
	Handle							modifiedClassCovStatsHandle;
	
		// Handle to the overlay image window information
	Handle							overlayImageWindowInfoHandle;
	
		// Handle to last settings of separability selections.						
	Handle							separabilitySpecsH;
	
		// Handle to last settings of statistics histogram selections.				
	Handle							statHistogramSpecsH;
	
		// Handle to last settings of statistics enhancement selections.
	Handle							statisticsEnhanceSpecsH;
	
		// Handle to class number to structure storage order vector.
	Handle							storageClassHandle;
	
	Handle							windowInfoHandle;
	
	HPClassNamesPtr				classNamesPtr;
	HChannelStatisticsPtr		classChanStatsPtr;
	HChannelStatisticsPtr		modifiedClassChanStatsPtr;
	HCovarianceStatisticsPtr	modifiedClassCovStatsPtr;
	HSumSquaresStatisticsPtr	classSumSquaresStatsPtr;
	HPFieldIdentifiersPtr		fieldIdentPtr;
	HPFieldPointsPtr				fieldPointsPtr;
	HChannelStatisticsPtr		fieldChanStatsPtr;
	HSumSquaresStatisticsPtr	fieldSumSquaresStatsPtr;
	
			// Current mask value to field number vector.
	SInt16*							maskValueToFieldPtr;
	 
	UInt16*							channelsPtr;
	
			// List indicating the relation of class number and group number.		
	SInt16*							classGroupPtr;
	
			// Class number to structure storage order vector.
	SInt16*							storageClass;
	
	SInt32							classPairWeightSum;
	SInt32							otherInformationPosition;
	SInt32							version;
	
	UInt32							startLine;
	UInt32							startColumn;
	UInt32							numberHeaderBytes;
	UInt32							numberTrailerBytes;
	UInt32							numberPreLineBytes;
	UInt32							numberPostLineBytes;
	UInt32							numberPreChannelBytes;
	UInt32							numberPostChannelBytes;
	UInt32							numberCovarianceEntries;
	UInt32							numberProjectClassPairs;
	
			// Project window information.
	SInt16							currentClass;
	SInt16							currentCoordinate;
	SInt16							currentField;
	SInt16							editPointNumber;
	SInt16							editFieldNumber;
	SInt16							editClassStorageNumber;
	SInt16							lastClassForAddFieldSelection;
	SInt16							lastFieldTypeForAddFieldSelection;
	
			// Variables defining which channels to be used for red, green
			// and blue to create a false color palette.
	SInt16							falseColorPaletteBlue;
	SInt16							falseColorPaletteGreen;
	SInt16							falseColorPaletteRed;
	
			// Code indicating whether the histograms are to be created
			// for the classes or the fields.
			//		=1: classes
			//		=2: fields
	SInt16							histogramClassFieldCode;
	SInt16							histogramClassListClassFieldCode;
	SInt16							histogramFieldListClassFieldCode;
	
			// User specified palette popup menu selection.
	SInt16							imagePalettePopupMenuSelection;
	
			// Variable to contain last setting of the type of coordinates to		
			// define the field: rectangle = 1, polygon = 2.							
	SInt16							lastPointType;
			// Variable to indicate which matrices are to be listed.					
			//		=0, no matrices; =1, correlation; =2, covariance; 					
			//		=3, both correlation and covariance.									
	UInt16							listMatrixType;
	UInt16							listMeanStdPrecision;
	UInt16							listCovCorPrecision;
	
	UInt16							listResultsTestCode;
	UInt16							listResultsTrainingCode;
	
			// Number of groups in thematic type images.									
	SInt16							numberGroups;
	SInt16							numberModifiedStorageClasses;
	SInt16							numberStatTrainClasses;
	SInt16							numberStatisticsFields;
	SInt16							numberStatTrainFields;
	SInt16							numberStatTestFields;
	SInt16							numberTotalPoints;
	SInt16							numberStatisticsChannels;
	SInt16							numberStorageClasses;
	SInt16							numberStorageFields;
	SInt16							numberStoragePoints;
	SInt16							numberStorageStatFields;
			// Variable to indicate which type of field is to be outlined.			
			//		=0, no outlining, =1, outline train, =2, outline test, 			
			//		=3, outline both train and test.											
	SInt16							outlineFieldType;
			// Variable to indicate what labels are to be written with each
			// outlined field.			
			//		=0, no labels, =1, class names, =2, field names, 			
			//		=3, class and field names, =4, train/test text,
			//    =5, class and train/test text, = 6, field name and train/test text,
			//    =7, class name, field name, and train/test text.											
	SInt16							labelFieldCode;
	
			// Code indicating the requested project mixing parameter type.
			//		= 1, computed optimum looc mixing parameter to be used.
			//		= 2, user set mixing parameter to be used.
			//		= 3, identity matrix to be used for covariance matrices.
	SInt16							mixingParameterCode;
	
			// Variable indicating the color to be used for outlining the training
			// and/or test areas in an image and to be used for the text.
			// =1, black
			// =2, white
	SInt16							outlineColorCode;
			// Variable indicates the selelection type being use.
			//	= kRectangleType for rectangular.
			//	= kPolygonType for polygonal.
	SInt16							selectionType;
			// Code indicating the statistics to be calculated for the project.	
			// =1, mean and std deviation only. 											
			// =2, mean, std dev. and covariance.											
	SInt16							statisticsCode;
	SInt16							statsWindowMode;
	
			// Class pair information.															
	SInt16							defaultClassPairWeight;
	SInt16							classPairWeightSet;
	
	SInt16							classWeightSet;
	SInt16							modifiedClassWeightSet;
	
	UInt16							commonCovarianceWeightSet;
	
			// Covariance stats to be used for the project.
			// 	kOriginalStats					0x0001
			// 	kLeaveOneOutStats				0x0002
			// 	kEnhancedStats					0x0004
	UInt16							covarianceStatsToUse;
	
	UInt16							enhancedStatsOrigin;
	
			// This represents the hdf image set that is being used for
			// the project base image file.
	UInt16							hdfDataSetSelection;
	
	UInt16							labelSize;
	
	UInt16							numberCommonCovarianceClasses;
	UInt16							numberStatisticsClasses;
	
	Boolean							localCommonCovarianceLoadedFlag;
	Boolean							enhancedStatsExistFlag;
	Boolean							labelAreasFlag;
	Boolean							listClassFlag;
	Boolean							listFieldFlag;
	Boolean							newProjectFlag;
	Boolean							pixelDataLoadedFlag;
	Boolean							includesStatisticsFromClusterOperationFlag;
	Boolean							statsUpToDate;
	Boolean							statsLoaded;
	Boolean							changedFlag;
	Boolean							handlesLockedFlag;
	Boolean							keepClassStatsOnlyFlag;
	Boolean							moveMemoryFlag;
	Boolean							saveClassMeansCovFlag;
	Boolean							saveClassSumsSquaresFlag;
	Boolean							saveFieldMeansCovFlag;
	Boolean							saveFieldSumsSquaresFlag;
	Boolean							setZeroVarianceFlag;
	Boolean							signedDataFlag;
	Boolean							swapBytesFlag;			// 0=No		1=Yes
	Boolean							useCommonCovarianceInLOOCFlag; 
	
	}	ProjectInfo, *ProjectInfoPtr, **ProjectInfoHandle;

//#if PRAGMA_ALIGN_SUPPORTED
#if defined PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#endif
	
	
typedef struct RectifyImageOptions
	{
	TransMapMatrix			mapMatrix;
	TransMapMatrix			inverseMapMatrix;
	TransMapMatrix			tempMapMatrix;
	double					backgroundValue;
	double					columnScaleFactor;
	double					lineScaleFactor;
	double					rotationAngle;
	double					rotationColumnCenter;
	double					rotationLineCenter;
	SInt16	 				*rectifyChannelPtr;
	Handle					referenceWindowInfoHandle;
	SInt32					columnShift;
	SInt32					lineShift;
	SInt32					nonRectifiedInputOffset;
	SInt32					rectifiedInputOffset;
	
			// Rectification procedure
			//		=1 (shift, rescale, rotate)
			//		=2 reproject to reference image (requires map information)
	SInt16					procedureCode;
	
			// Resampling method to use
			//		=1 nearest neighbor (for Multispectral and Thematic images)
			//		=2 majority rule (for Thematic images)
	SInt16					resampleCode;
	UInt16					numberChannelsToRectify;
	Boolean					blankOutsideSelectedAreaFlag;
	Boolean					shiftOnlyFlag;
	
	} RectifyImageOptions, *RectifyImageOptionsPtr;
	
	
typedef struct ReformatOptions
	{
	RecodeThematicImage			recodeThematicImage;
	Str255							denominatorString;
	Str255							numeratorString;
	double							adjustDivisor;
	double							adjustFactor;
	double							adjustOffset;
	
			// Factors to use for converting radiance to radiant temperature
	double							algebraicTransformK1Value;
	double							algebraicTransformK2Value;
	double							algebraicTransformRadianceMult;
	double							algebraicTransformRadianceOffset;
	
	double							functionFactor;
	double							maximumValue;
	double							minimumValue;
	double							pcScaleFactor;
	double							transformAdjustSelectedChannelsFactor;
	double							transformFactor;
	double							transformOffset;
	SInt64							modifyAppendBytesSkip;
	Handle							channelHandle;
	Handle	 						coefficientsHandle;
	Handle 							transformChannelHandle;
	Handle 							transformOperatorHandle;
	Handle							eigenValueHandle;
	Handle							eigenVectorHandle;
	Handle							inputWindowInfoHandle;
	Handle							outFileInfoHandle;
	Handle							pcChannelHandle;
	Handle							rightBottomMosaicWindowInfoHandle;
	HDoublePtr						eigenValuePtr;
	HDoublePtr						eigenVectorPtr;
	HUCharPtr						ioOutAdjustBufferPtr;
	HUCharPtr						ioOutBufferPtr;
	RectifyImageOptionsPtr		rectifyImageOptionsPtr;
	double*	 						coefficientsPtr;
	SInt32*							fileInfoListPtr;
	SInt16*	 						channelPtr;
	SInt16* 							transformChannelPtr;
	SInt16* 							transformOperatorPtr;
	SInt16*	 						classPtr;
	SInt16*							pcChannelPtr;
	SInt32							backgroundValue;
	SInt32							countOutBytes;
	SInt32							ioOutBufferBytes;
	SInt32							lineStart;
	SInt32							lineEnd;
	SInt32							lineInterval;
	SInt32							columnStart;
	SInt32							columnEnd;
	SInt32							columnInterval;
	SInt32							startLine;
	SInt32							stopLine;
	SInt32							startColumn;
	SInt32							stopColumn;
	
			// Defines the data value conversion to be done. See GetDataConversionCode
			// routine.
	UInt32							convertType;
	UInt32							highSaturationCount;
	UInt32							kthSmallestValue;
	UInt32							lowSaturationCount;
	UInt32							numberClasses;
	UInt32							numberOutputBufferLines;
	
			// Code for the type of algebraic transformation to be done
	SInt16							algebraicTransformOption;
	
	SInt16							bandInterleaveSelection;
	SInt16							channelSet;
	SInt16							classSet;
	SInt16							dataValueTypeSelection;
			
			// The default for the thermal channel if none.
	SInt16							defaultThermalChannel;
	
	SInt16							eigenSource;
	SInt16							fieldType;
	SInt16							fileInfoVersion;
	SInt16							headerFormat;
	
			// The following variables are used to store what some parameters were for
			// the last use of the Reformat processor. They will be used to determine 
			// whether the reformat structure can be reused or whether it is best to
			// clear out and start again.
	SInt16							lastInputFileDataTypeCode;
	SInt16							lastInputFileSignedDataFlag;
	SInt16							lastInputImageType;
	SInt16							lastInputWindowNumberBytes;
	SInt16							lastInputWindowNumberChannels;
	SInt16							lastInputWindowType;
	SInt16							lastReformatProcessorCode;
	SInt16							locationInList;
	
	SInt16							mosaicDirectionCode;
	
	SInt16							numberDenominatorTerms;
	SInt16							numberNumeratorTerms;
	
			// Code indicating whether the output is to:
			// 	= 1 (kNewFileMenuItem) go to a new file
			//		= 2 (kAppendToMenuItem) be appended to an existing file
			//		= 3 (kModifyPartMenuItem) modify (overwrite) a part of an existing file
	SInt16							outputFileCode;
	
			// Channel to use to subtract or add to the selected channels
	SInt16							transformAdjustSelectedChannel;
	
			// Code indicating how to transform the output channel data.  	
			// 0=no transformation; 1=adjust each channel the same; 2=		
			// transform multiple input channels to 1 output channel.		
	SInt16							transformDataCode;
	
	UInt16							convertFromNumberBytes;
	
	UInt16							forceByteCode;
	UInt16							numberChannels;
	UInt16							numberPCChannels;
	UInt16							numberPCComponents;
	UInt16							workingDataTypeCode;
	UInt16							functionCode;
	Boolean							algebraicStringsLoadedFlag;
	Boolean							bottomToTop;
	Boolean							channelDescriptions;
	Boolean							checkForSaturationFlag;
	Boolean							firstTimeFlag;
	Boolean							ignoreBackgroundFlag;
	Boolean							outputInWavelengthOrderFlag;
	Boolean							rightToLeft;
	Boolean							signedOutputDataFlag;
	Boolean							swapBytesFlag;
	Boolean							transformChannelPtrSetFlag;
	Boolean							userSetDataValueTypeSelectionFlag;
	
	} ReformatOptions, *ReformatOptionsPtr;
		
	
struct RGBCharColor
	{
	UChar								red;
	UChar								green;
	UChar								blue;
	
	};              
	
typedef struct RGBCharColor RGBCharColor, *RGBCharColorPtr, **RGBCharColorHdl; 
	
		// The only reason for both ShortDouble and Double rects are for the
		// 680x0/68881 processors. In them short double and double are different.
		// When that processor is not supported, the SDoubleRect can be removed.
		
typedef struct SDoubleRect
	{
	SDouble 		top;
	SDouble 		left;
	SDouble 		bottom;
	SDouble 		right;
	
	} SDoubleRect;
	

typedef struct SelectionInfo
	{
	DoubleRect							coordinateRectangle;
	LongRect								lineColumnRectangle;
	Rect									offScreenRectangle;
	SInt64								numberPixels;
	Handle								polygonCoordinatesHandle;
	SInt16								numberPoints;
	SInt16								typeFlag;
	SInt16								unitsDisplayCode;
	
	} SelectionInfo, *SelectionInfoPtr;
	

typedef struct SelectionIOInfo
	{
	Handle				channelStatsHandle;
	
			// Handle to graphics record.
																	
	Handle				ioBufferHandle;
	Handle				sumSquaresHandle;
	SInt32				bufferSize;
	SInt32				outputBufferOffset;

			// Flag indicating when the memory for the selection graph 				
			// parameters need to be checked.
															
	Boolean				checkIOMemoryFlag;
	
			// This flag will be turned on after a memory warning has been			
			// displayed one time so that the warning will not be displayed		
			// again.								
															
	Boolean				memoryWarningFlag;
	
	} SelectionIOInfo, *SelectionIOInfoPtr;  
	
	
typedef struct SeparabilitySpecs
	{		
	Handle	 							allChanCombinationsHandle;
	Handle	 							channelCombinationsHandle;
	Handle 								channelsHandle;
	Handle	 							classHandle;
	Handle		 						excludeCombinationsH;
	Handle 								featureHandle;
	Handle		 						showCombinationsH;
	Handle 								showExcludeCombinationsH;
	Handle 								symbolsHandle;
	
	Ptr									distancesSummaryPtr;
	float*								distancesPtr;
	SInt16*								channelSetsPtr;
	
	float									distancesLessThanToGroup;
	float									distancesLessThanToList;
	float									maxDistanceRangeToList;
	float									minDistanceRangeToList;
		
	SInt32								combinationSlots;
	SInt32								combinationsToStore;
	SInt32								maxCombinationsToStore;
	SInt32								maxNumberAllChanCombinations;
	SInt32								numberClassCombinations;
	SInt32								numberCombinationsToList;
	SInt32								numberFeatureCombinationsLoaded;
	
	UInt32								maxNumberFeatureCombinations;
	UInt32								meanEnd;
	UInt32								meanStart;
	UInt32								minimumEnd;
	UInt32								minimumStart;
	UInt32								numberClasses;
	
	SInt16								classSet;
	SInt16								channelCombinationSet;
	SInt16								channelSet;
	SInt16								distanceMeasure;
	SInt16								featureSearchCode;
	SInt16								numberContiguousPerGroup;
	SInt16								numberExcludeCombinations;
	SInt16								numberFeatures;
	SInt16								numberShowCombinations;
			// Variable to indicate where separability output is to be placed.	
			//		=0, no output; 																
			//		=1, to output window; 														
			//		=2, to disk file; 															
			//		=3, both output window and disk file; 									
	SInt16								outputStorageType;
			//	Variable indicating how to sort the channel combinations.			
			//		= 0, do not sort. list in order computed.								
			//		= 1, by average class distance.											
			//		= 2, by minimum class distance.											
	SInt16								sortChannelCombinations;
	SInt16								symbolSet;
			//	Variable indicating which tables to list.									
			//		= 1, separability table.													
			//		= 2, class pairs with distance less than threshold.				
			//		= 3, separability and class pairs tables.								
			//		= 4, grouping table.															
			//		= 5, separability and grouping tables.									
			//		= 6, class pairs and grouping tables.									
			//		= 7, separability, class pairs and grouping tables.				
	SInt16								tablesToList;
	
	UInt16								numberAllChanCombinations;
	UInt16								numberChannelGroupCombinations;
	UInt16								numberChannels;
	
	Boolean								featureTransformationFlag;
	Boolean								listClassPairDistances;
	
	} SeparabilitySpecs, *SeparabilitySpecsPtr;
                        

typedef struct ShapeInfo
	{
	#if defined multispec_mac || defined multispec_mac_swift
		CMFileStream					fileStream;
	#endif	// defined multispec_mac || defined multispec_mac_swift
	
	#if defined multispec_win
		CMFileStream					*fileStreamCPtr;
	#endif	// defined multispec_win
			
   #if defined multispec_wx
      CMFileStream					*fileStreamCPtr;
      bool								expandFeatureList;
   #endif	// defined multispec_wx

			// Geodetic model description for vectors
	GeodeticModelInfo					geodetic;
	
			// Grid coordianate system information for vectors
	GridCoordinateSystemInfo		gridCoordinate;			
	
	DoubleRect							boundingXYBox;
	DoubleRect							boundingZMBox;
	
	#if include_gdal_capability
		DBFHandle							dbfInfoPtr;
   #else	// include_gdal_capability
		Handle								dbfInfoPtr;
   #endif	// include_gdal_capability
	
	RGBColor								lastOverlayColor;
	
	Handle								vectorDataHandle;
	Handle								indexRecordHandle;
	
	UInt32								fileLength;
	UInt32								numberRecords;
	UInt32								shapeType;
	
			// =0: lat-long; no conversion done.
			// =1: map units; no conversion done.
			// =2: shape file is converted from lat-long to map units
	SInt16								conversionCode;
	
			// Code for the map units that the shape file was converted to
	SInt16								unitsCodeForConversion;
	
	UInt16								lastLineThickness;
	
			// This is used to indicate version number of the loaded file when
			// the same shape file is loaded more than one time because of differing
			// map projections.
	UInt16								versionNumberLoaded;
	
	} ShapeInfo, *ShapeInfoPtr;
                        

typedef struct ShapeIndexRecords
	{
	SInt32								offset;
	SInt32								contentLength;
	
	} ShapeIndexRecords, *ShapeIndexRecordsPtr;

	
typedef struct StatEnhanceSpecs
	{
	double								hardChiChiThreshold;
	double								hardPercentThreshold;
	double								labelWeight;
	double								logLikeStopPercentChange;
	double								softChiChiThreshold;
	double								softPercentThreshold;
	
	SInt64								numberLabeled;
	SInt64								numberUnlabeled;
	SInt64								unlabeledCount;

	Handle	 							classHandle;
	Handle								targetWindowInfoHandle;
	
	Boolean*								classThresholdFlagPtr;
	
	double*								likePtr;
	double*								priorsPtr;
	double*								totalLogLikePtr;
	
	HDoublePtr							totalLikePtr;
	HDoublePtr							typicalityWeightPtr;
	
	HDoublePtr							labeledDataPtr;
	HDoublePtr							unlabelDataPtr;
	
	TypicalitySummaryPtr				classTypicalityPtr;
	
	SInt32								lineStart;
	SInt32								lineEnd;
	SInt32								lineInterval;
	SInt32								columnStart;
	SInt32								columnEnd;
	SInt32								columnInterval;
	SInt32								iterationMax;
	
	UInt32								numberClasses;

	SInt16								classSet;
	SInt16								covarianceStatsToUse;
	SInt16								fileInfoVersion;
	SInt16								hardThresholdCode;
	SInt16								iterationStopLength;
	SInt16								softThresholdCode;
	SInt16								weightSet;
	
	Boolean								weightLabeledFlag;
	
	}		StatEnhanceSpecs, *StatEnhanceSpecsPtr;
	
	
typedef struct StatHistogramSpecs
	{
	double								maxValue,
											minValue;
	
			// Pointer to memory to be used to determine the project miminums
			// and maximums for the selected channels.  These are used when the
			// data type contain 4 or more bytes.
	double*								channelMaxPtr;
	double*								channelMinPtr;
	
			// Maximum number of pixels in the project classes.  Is used to
			// define the bin width.
	SInt64								maxNumberPixelsInClass;
	
	HChannelStatisticsPtr			channelStatsPtr;
	
			// Pointer to the range information for each histogram column.			
	HistogramSummaryPtr				histogramRangePtr;
	
			// Pointer to the histogram information.										
	HUInt32Ptr							histogramDataPtr;
	
			// Used for pointer to vector stored in feature handle.
	UInt16*								featurePtr;
	
			// Pointer to the list of pointers to project channels to be 			
			// histogrammed. 																		
	Handle		 						featureHandle;
	
			// Pointer to the channels to be histogrammed. 								
	Handle	 							channelsHandle;
	
			// Pointer to the list of classes to be histogrammed. 					
	Handle	 							classHandle;
	
			// Pointer to the list of fields to be histogrammed. 						
	Handle								fieldHandle;
	
			// Handle to histogram data.														
	Handle								histogramDataHandle;
	
	SInt32								columnInterval;
	SInt32								lineInterval;
	
	UInt32								initialNumberHistogramDataBins;
	UInt32								numberClasses;
	
			// Total number of values in histogram.										
	UInt32								totalNumberValues;
	
	SInt16								classSet;
	SInt16								channelSet;
	SInt16								fieldSet;
	
			// Variable indicating the type of histogram output. 						
			//		=1 list values.																
			//		=2 plot values.																
	SInt16								histogramOutputCode;
	
			// Variables storing the last class and field numbers used.
	SInt16								lastClassNumberUsed;
	SInt16								lastFieldNumberUsed;
	
			// Variable containing number of columns of histogram information.	
	SInt16								numberColumns;
	
			// Variable containing the number of fields or classes that data are being
			// stored for for the plot.
	SInt16								numberVectors;
	
			// Variable indicating the histogram output format. 						
			//		(outputFormatCode & 0x0001) = include empty bins.					
			//		(outputFormatCode & 0x0002) = blank values outside of range.	
			//		(outputFormatCode & 0x0010) = matrix form.							
			//		(outputFormatCode & 0x0020) = columnar list form.					
			//		(outputFormatCode & 0x0100) = group channels.						
			//		(outputFormatCode & 0x0200) = group fields/classes.				
	SInt16								outputFormatCode;
	
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16								outputStorageType;
	
			// Variable indicating the maximum number of decimal digits required when
			// listing or ploting data values.
	UInt16								maxNumberEDecimalDigits;
	UInt16								maxNumberFDecimalDigits;
	
	UInt16								numberChannels;
	UInt16								numberFeatures;
	UInt16								numberFields;
	
	Boolean								featureTransformationFlag;
	
	Boolean								overlayDensityFunctionFlag;
	
	} StatHistogramSpecs, *StatHistogramSpecsPtr;
	
	
typedef struct StatisticsImageSpecs
	{
	double							userMinimum;
	double							userMaximum;
	
			// Pointer to the channels to be listed. 										
	Handle	 						channelsHandle;
	
			// Pointer to the list of classes to be listed. 							
	Handle		 					classHandle;
	
			// Pointer to the list of pointers to the channels to be listed. 		
	Handle	 						featureHandle;
	
			// Pointer to the list of fields to be listed. 								
	Handle		 					fieldHandle;
	Handle							windowInfoHandle;
	
	SInt32							columnEnd;
	SInt32							columnInterval;
	SInt32							columnStart;
	SInt32							lineEnd;
	SInt32							lineInterval;
	SInt32							lineStart;
	UInt32							numberClasses;
	
			// Code indicating areas to be used in creating the statistics			
			// images.  =1 classes, =2 selected area in image.							
	SInt16							areaCode;
	SInt16							channelSet;
	SInt16							classSet;
	SInt16							fieldSet;
	
			// Code indicating whether training fields are to be used (=1) and/	
			// or test field are to be used.													
	SInt16							fieldTypeCode;
	SInt16							fileInfoVersion;
	
			// Code indicating whether the minimum and maximum values used in		
			// the images are based on the overal min and max values for 			
			// selected areas, i.e. the same for all images (=1) or are based 	
			// on the min and max values for the area used in the image, i.e.		
			// may be different for each image (=2). 										
	SInt16							minMaxSettingCode;
	SInt16							numberFeatures;
	
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16							outputStorageType;
	
			// Code indicating whether images are to be created for class			
			// statistics (=1) or field statistics (=2).									
	SInt16							perFieldClassCode;
	UInt16							numberChannels;
	UInt16							numberFields;
	
			// Flag indicating (if true) that the list data specifications have	
			// not been loaded the first time.												
	Boolean							firstTime;
	Boolean							createImageWindowFlag;
	Boolean							featureTransformationFlag;
	Boolean							projectFlag;
	
	} StatisticsImageSpecs, *StatisticsImageSpecsPtr;


typedef struct TIFFImageFileDirectory
	{
	UInt16							tag;
	SInt16							type;
	UInt32							count;
	UInt32							value;
	
	} TIFFImageFileDirectory, *TIFF_IFD_Ptr; 
	
	
typedef struct TransformationSpecs
	{
	UInt8								imageFileName[256];
	Handle							eigenValueHandle;
	Handle							eigenVectorHandle;
	Handle	 						eigenFeatureHandle;
	Handle							preProcBandGroupingHandle;
	HDoublePtr						eigenVectorPtr; 
	HDoublePtr						tempMatrixPtr;  
	HDoublePtr						offsetVectorPtr; 
	SInt16*							eigenFeaturePtr;
	
			// Code indicating which processor created that transformation			
			// matrix. (1=principal components; 2=feature extraction.				
	SInt16							createdByCode;
	SInt16							numberChannels;
	SInt16							numberFeatures;
	
	} TransformationSpecs, *TransformationSpecsPtr;
	

typedef struct WindowInfo
	{
			// Pointer to the class structure which defines this image window.
			
   #if defined multispec_wx
		CMImageWindow*				cImageWindowPtr;
      Ptr							windowPtr;
      wxImage						offScreenImage;
   #endif	// defined multispec_wx
	
			// Pointer to the window structure for this image window.
			// Used in Macintosh version only
	#if defined multispec_mac || defined multispec_mac_swift
		CGInfo						cgInfo;
		Ptr							cImageWindowPtr;
		WindowPtr					windowPtr;
	#endif	// defined multispec_mac || defined multispec_mac_swift
		
	#if defined multispec_win 
		CMImageWindow*				cImageWindowPtr;
		Ptr							windowPtr;
	#endif	// defined multispec_win
	
	DoubleRect					boundingLatLongRectangle;
	
	DoubleRect					boundingMapRectangle;
	
	ImageSecondOrderStats	imageSecondOrderStats;
	
			// Parameters for mask file.
			
	MaskInfo					mask;
	
	OverlaySpecs			overlayList[32];
	
	ImageOverlaySpecs		imageOverlayList[kMaxNumberImageOverlays];
	
			// Factor to use to convert coordinates to desired units.
	double					coordinateViewAreaFactor;
	
			// Factor to use to convert coordinates to desired units.
	double					coordinateViewFactor;
	
			// Maximum data value possible in 'window' file.
	double					maxUsableDataValue;
	
			// Minimum data value possible in 'window' file.
	double					minUsableDataValue;
	
			// Print scale
	double					printPageScale;

			// Handles to controls for window.  ZoomIn, ZoomOut, Magnify, 			
			//	legendUpContrl and legendDnContrl will be NULL for non image 		
			// windows.								
	ControlHandle			coordinateViewAreaUnitsControl;
	ControlHandle			coordinateViewControl;
	ControlHandle			coordinateUnitsControl;
	ControlHandle			horizontalControl;
	ControlHandle			legendPaletteControl;
	ControlHandle			legendScrollDownControl;
	ControlHandle			legendScrollUpControl;
	ControlHandle			magnificationControl;
	ControlHandle			overlayControl;												
	ControlHandle			verticalControl;
	ControlHandle			zoomInControl;
	ControlHandle			zoomOutControl;

			// Pointer to graph class structure 
	CMGraphView*			graphViewCPtr;
	
			// Pointer to statistics file information for image window. 			
	CMFileStream*			supportFileStreamPtr;
	
			// Pointer to mask file information for image window. 			
	CMFileStream*			maskFileStreamPtr;
	
			// Handle to histogram display levels for image.  Will be NULL for 	
			// non-image type windows.															
	Handle					displayLevelHandle;
	
			// Handle to display specifications for image.  Will be NULL for 		
			// non-image type windows.															
	Handle					displaySpecsHandle;
	
			// Handle to image file information.  Will be NULL for non-image		
			// type window.																		
	Handle					fileInfoHandle;
	
			// Handle to histogram specifications for image.
			
	Handle					histogramSpecsHandle;
	
			// Handle to histogram summary for image.
	
	Handle					histogramSummaryHandle;
	
			// Handle to storage for offscreen image data.  Will be NULL for 		
			// non-image type windows.															
	Handle					imageBaseAddressH;
	
			// Handle to the layer file information for the "window" files.		
	Handle					layerInfoHandle;
	
			// Handle to map projection information.										
	Handle					mapProjectionHandle;
	
			// Handle to offscreen map for image window.  Will be NULL for 		
			// non-image type windows.															
	Handle					offScreenMapHandle;
	
			// Handle to selection area information for image.  Will be NULL for 
			// non-image type windows.															
	Handle					selectionInfoHandle;
	
			// Handle to statistics file information for image window.  Will be 	
			// NULL for non-image type windows.	
	Handle					supportFileStreamHandle;
	
			// Handle to this window information record.
	Handle					windowInfoHandle;
	
			// Handle to the window record.													
	Handle					windowRecordHandle;
	
			// List Handle for legend information in Thematic type image windows.
	LegendListHandle		legendListHandle;
	
			// Pointer to the offscreen GWorld.												
	GWorldPtr				offscreenGWorld;
	
			// Handle to text record for window.  Will be NULL for image and		
			// palette windows.																	
	//WSHandle					textH;
	#if use_mlte_for_text_window
		TXNObject				textH;
	#endif		// use_mlte_for_text_window
	#if !use_mlte_for_text_window
		WEReference				textH; 
	#endif		// !use_mlte_for_text_window
	
			// Coordinate view parameters
			// 	Height of portion of window for displaying the coordinate information.
	UInt32					coordinateHeight;
	
			// 	x-location in coordinate view to start displaying the cursor location.
	UInt32					coordinateViewCursorStart;
	
			// 	Maximum number of character needed for coordinate information.
	UInt32					coordinateViewNumberChars;
	
			// 	x-location in coordinate view to start displaying the selection info.
	UInt32					coordinateViewSelectionStart;
	
			// 	x-location in coordinate view to start displaying the number of pixels.
	UInt32					coordinateViewNumberPixelsStart;
	
			// 	Maximum number of characters needed for number of pixels.
	UInt32					coordinateViewNumberPixelsChars;
	
			// 	x-location in coordinate view to start displaying the number of pixels.
	UInt32					coordinateViewAreaPopupStart;
	
			// 	x-location in coordinate view to start displaying the number of pixels.
	UInt32					coordinateViewScaleStart;
	
			// Offset from the top of the window where the top of the image
			// is displayed at. This includes the coordinate height (mac version only)
			// and any title height.
	UInt32					imageTopOffset;
	
			// The maximum number of columns in the "window" file that can be		
			// accessed.																			
	UInt32					maxNumberColumns;
	
			// The maximum number of lines in the "window" file that can be		
			// accessed.																			
	UInt32					maxNumberLines;
	
			// The maximum number of bins possible in the "window" file.			
	UInt32					numberBins;
	
			// The number of image overlays for this window. The indices are in the 
			// image overlays structure member.			
	UInt32					numberImageOverlays;
	
			// The number of vector overlays for this window. The indices are in the
			// overlays structure member.			
	UInt32					numberVectorOverlays;
	
			// Size of the buffer for the offscreen pixel map.							
			// This is needed for determining whether to unlock offscreen map		
			// memory.  If more than 9 meg, it should not be unlocked because		
			// the Mac OS crashes when moving blocks larger than this.				
			//	1-22-93																				
	UInt32					offscreenMapSize;
	
			// Number of bytes in each row in the offscreen bitmap
	UInt32					rowBytes;
	
			// Code for print page orientation for the window.
	PMOrientation			printPageOrientation;
	
			// The band interleave for the window files.									
			// 1=BIL, 2=BSQ, 3=BIS  -1=Mixed													
	SInt16					bandInterleave;
	
			// Flag indicating whether information in the window has changed 		
			// since the last save.  0=not changed, 1=changed.							
	SInt16					changed;
	
			// Code indicating whether the channels are in wavelength order
			// 0=not applicable; 1=in wavelength order; 2=not in wavelength order
	SInt16					channelsInWavelengthOrderCode;
	
			// Code indicating the units to use to display the number of pixels
			// or the area of the selected area.  The codes are defined by the 
			// area units constants.							
	SInt16					coordinateViewAreaUnitsCode;
	
			// Code indicating the units to use to display the cursor location
			// and selection area.  The codes are defined by the coordinate units
			// constants.							
	SInt16					coordinateViewUnitsCode;
	
			// Number of decimal places to use for the coordinate view unit.
	SInt16					coordinateViewUnitDecimalPlaces;
	
			// This variables is used to keep track of changes in the file			
			// specification (such as number of lines, etc.  It is incremented	
			// by 1 each time the specifications change for one of the image		
			// files used for the window "file".											
	SInt16					fileInfoVersion;
	
			// Flag indicating the type of grafport.  0=GrafPort, 1=CGrafPort.	
	SInt16					grafPortType;
	
			// Legend parameters.  Used for Thematic type windows.	
	SInt16					legendLeftStart;
	SInt16					legendTitleWidth;
	SInt16					legendTopStart;			
	SInt16					legendWidth; 
	
			// Code indicating whether the data values in the 'window' file are
			// integer or real. Real will also indicate a mixture of integer and real.
			//		= 0, kIntegerType
			//		= 1, kRealType
	UInt16					dataTypeCode;
	
			// Maximum number of data bytes represented by the local channel 		
			// list.																					
	UInt16					localMaxNumberBytes;
	
			// Code indicating the image type.
			//		1 = multispectral type data
			//		2 = thematic type data
	UInt16					imageType;
	
			// Layer in the mask file which includes the masking information.
	UInt16					maskFileLayer;
	
			// Maximum number of characters used in any of the channel 				
			// descriptions.																		
	SInt16					maxNumberDescriptionCharacters;
	
			// Code indicating whether the supportFileStreamHandle is for each image	
			// file (=0) or for all the image files (=1).								
	SInt16					supportFileCode;
	
			// Height of title.  Used for side-by-side image windows.				
	SInt16					titleHeight;
	
			// Item number in window menu.													
	SInt16					windowMenuItem;
	
			// Number of window opened for the file.  There may be more than one	
			// window openned for the same file.											
	SInt16					windowNumber;
	
			// 1=text, 2=image, 3=palette, 4=debug, 5=Thematic, 6=Graphics			
			// 0=project																			
	SInt16					windowType;
	
			// Code providing information about the description for the channels.
			//		Bit 0 = 0x0: no description information for the channels
			//		Bit 0 = 0x1: description information exists
			//		Bit 1,2 = 0x00: no information exists whether reflective or thermal
			//		Bit 1,2 = 0x01: Reflective data only
			//		Bit 1,2 = 0x10: Thermal data only
			//		Bit 1,2 = 0x11: Both reflective and thermal data exists
			//		Bit 3 = 0x0: no band width information
			//		Bit 3 = 0x1: band width information exists
	UInt16					descriptionCode;
	
			// The maximum number of bits per data value in the "window" file.	
	UInt16					numberBits;
	
			// The maximumn number of bytes per data value in the "window" file.	
	UInt16					numberBytes;
	
			// Number of files or (layers) being used for the window.				
	UInt16					numberImageFiles;
	
			// Total number of channels accessible for multispectral image			
			// windows or total number of layers accessible for thematic image	
			// windows.																				
	UInt16					totalNumberChannels;
	
			// This flag is used to indicate when the text output window has changed
			// since the last save.
	Boolean					windowChangedFlag;
	
			// This flag controls when the base image should be drawn in the 
			// image window during an update event. This could off when one just
			// wants to draw an overlay image on top of the base image.
	Boolean					drawBaseImageFlag;
	
			// This flag controls when the bitmap should be drawn. It may be turned
			// off during the beginning of the display processor. CopyOffscreen may
			// get called before any data is loaded into the bitmap. This saves
			// time in drawing, particularly with large images.
	Boolean					drawBitMapFlag;
	
			// This flag controls when the any vector overlays for the image window
			// should be drawn.  In one case this flag is set to false when drawing
			// images in the display image processor so that only the image is drawn
			// in steps.  At the end of the display processor function the flag is
			// turn back on again so that any vectors for the window are drawn. 
	Boolean					drawVectorOverlaysFlag;
	
			// Flag indicating whether the number of data bytes differ for the	
			// local channel list.																
	Boolean					localBytesDifferFlag;
	
			// Flag indicating whether this image window belongs to the open		
			// project.																				
	Boolean					projectWindowFlag;
	
			// Flag indicating whether this image window belongs to the base		
			// image file for the project.													
	Boolean					projectBaseImageFlag;
	
			// Flag indicating whether to show the Class legend for Thematic 		
			// type windows.																		
	Boolean					showLegend;
	
			// Flag indicating whether any of the data values in the "window"
			// file are signed.
	Boolean					signedDataFlag;
	
	} WindowInfo, *WindowInfoPtr, **WindowInfoHandle;
	

typedef struct WorkFlowInfo
	{
			// Structure that contains information to be used for a workflow process.
			
			// file information for the last file that was saved to disk.
	LocalAppFile					lastFileSavedInfo;
	
			// Default name to use.
	UInt8								defaultName[256];
	
			// Name of last image file read.
	UInt8								lastFileReadName[256];

			// Code for work flow processing.  This is just for testing now.
	SInt16							workFlowCode;
	
			//Values to use for some special processing.
	SInt16							thresholdValue;
	SInt16							functionCode;
	
	} WorkFlowInfo, *WorkFlowInfoPtr;

   
#if defined multispec_wx
			// The following is a list of structures defined in MFC
			// which may be useful in Linux version

	typedef struct tagBITMAPINFOHEADER 
		{
		DWORD biSize;
		long biWidth;
		long biHeight;
		WORD biPlanes;
		WORD biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		long biXPelsPerMeter;
		long biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
		
		} BITMAPINFOHEADER, *PBITMAPINFOHEADER;


	typedef struct tagPOINT 
		{
		long x;
		long y;
		
		} POINT;


	typedef DWORD COLORREF;
	
	
	typedef DWORD* LPCOLORREF;


	typedef struct tagLOGPEN 
		{
		int lopnStyle;
		POINT lopnWidth;
		COLORREF lopnColor;
		
		} LOGPEN, *PLOGPEN;


	typedef struct tagLOGBRUSH 
		{
		int lbStyle;
		COLORREF lbColor;
		unsigned long* lbHatch;
		
		} LOGBRUSH, *PLOGBRUSH;


	#define BYTE unsigned char
	#define RGB(r,g,b) ((COLORREF)(((Byte)(r)|((WORD)((Byte)(g))<<8))|(((DWORD)(Byte)(b))<<16)))
	#define GetRValue(rgb)   ((BYTE) (rgb))
	#define GetGValue(rgb)   ((BYTE) (((WORD) (rgb)) >> 8))
	#define GetBValue(rgb)   ((BYTE) ((rgb) >> 16))
#endif	// defined multispec_wx
