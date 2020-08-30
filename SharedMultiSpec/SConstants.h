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
//	File:						SConstants.h
//
//	Authors:					Larry L. Biehl
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	Header file which contains the constants used by MultiSpec.
//
//	Written By:				Larry L. Biehl			Date: 03/29/1988
//	Revised By:				Abdur Maud				Date: 06/18/2013
//	Revised By:				Larry L. Biehl			Date: 08/29/2020
//	
//------------------------------------------------------------------------------------

#pragma once

#if defined multispec_wx
	#include <time.h>
	#include <string>
	#include <cstring>
	#include <iostream>

	#define	activMask						0x0100
	#define	keyDownMask						0x0008
	#define	mDownMask						0x0002
	#define	mUpMask							0x0004
	#define	updateMask						0x0040
	#define	osMask							0x0000
	#define	GetTickCount()           LinGetTime()
	#define	TickCount()					(time_t)GetTickCount()
	#define	noErr				0
	#define	fsFromStart		1
	#define	fsFromLEOF	   2
	#define	fsFromMark		3

	#define	dskFulErr       -34
	#define	ioErr		       -36
	#define	bdNamErr	       -37
	#define	eofErr		    -39
	#define	posErr		    -40
	#define	tmfoErr		    -42
	#define	fnfErr		    -43
	#define	wPrErr		    -44
	#define	fLckdErr	       -45

	#define	kFrontTimeOffset		500		// in milliseconds
	#define	kDisplayTimeOffset	1000		// in milliseconds
	#define	kNumMenuItems				1
	#define	kAllMenuItem				0
	#define	kFrontYieldTime	      0 
	#define	kReadyThreadState			0
	#define	kSubsetMenuItem			1
	#define	kAppendToMenuItem			2
	#define	kNewFileMenuItem			1
	#define	kAppendToMenuItem			2
	#define	kModifyPartMenuItem		3 

	#define	kDefaultMenuItem			1
	#define	kUserMenuItem				2

	#define	kEqualWeightMenuItem		0
	#define	kUnequalWeightMenuItem	1
	#define	kNumberSampleWeightMenuItem  2
	
	#define	kNoneMenuItem					0
	#define	kArcViewMenuItem				1
	#define	kERDAS74MenuItem				2
	#define	kGAIAMenuItem					3 
	#define	kTIFFGeoTIFFMenuItem			4
	#define	kMatlabMenuItem				5

	#define	kBILMenuItem					0
	#define	kBSQMenuItem					1
	#define	kBISMenuItem					2 
	#define	kBNonSQMenuItem				3 
	#define	kBNonSQBlockedMenuItem		4 
	#define	kBIBlockMenuItem				5

	#define	k4BitUnsignedIntegerMenuItem		0
	#define	k1ByteSignedIntegerMenuItem		1
	#define	k1ByteUnsignedIntegerMenuItem		2
	#define	k2ByteSignedIntegerMenuItem		3
	#define	k2ByteUnsignedIntegerMenuItem		4
	#define	k4ByteSignedIntegerMenuItem		5
	#define	k4ByteUnsignedIntegerMenuItem		6
	#define	k4ByteRealMenuItem					7
	#define	k8ByteRealMenuItem					8

	#define	ID_CLEAROVERLAYMENUITEMSTART		30001
	#define	ID_CLEAROVERLAYMENUITEMEND			30100

	#define	ID_SHOWOVERLAYMENUITEMSTART		30101
	#define	ID_SHOWOVERLAYMENUITEMEND			30200

	#define	ID_GRAPHOVERLAYMENUITEMSTART		30201
	#define	ID_SELECTBINWIDTHMENUITEMSTART	30401
	
	#define ID_SELECTBINWIDTHMENUITEMEND		30500
	#define	ID_SELECTVECTORMENUITEMSTART		30601

	#define	kTicksPerMinute						60000

	#define StripAddress(x) ((Ptr)(x))
	#define	true32b	 1
	// The following was properly redefined in SDefines.h
	//#define RGB(a,b,c) 0x0000

	#define	blackColor						RGB(0,0,0)		// 33
	#define	redColor							RGB(255,0,0)	//205
	#define	greenColor						RGB(0,128,0)	//341
	#define	blueColor						RGB(0,0,255)	//409
	#define	cyanColor						RGB(0,255,255)	//273
	#define	magentaColor					RGB(255,0,255)	//137 
	#define	whiteColor						RGB(255,255,255)	//30   
	#define	yellowColor						RGB(255,255,0)	//69 
	#define LMGetTicks()		(UInt32)GetTickCount() 
	#define	kSample 1
#endif	// defined multispec_wx

#if defined multispec_mac || defined multispec_mac_swift
	#pragma once
	
	#undef _80BitDoubles_
	#ifndef powerc
		#define _80BitDoubles_
	#endif
	
			// Set up register double parameter
	/*
	#ifndef _MC68881_
		#define	DOUBLE		double
	#endif	// _MC68881_
	
	#ifdef _MC68881_
		#define	DOUBLE		register double
	#endif	// _MC68881_
	*/

	#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

			// Menu indices 																		
	#define kAppleM			0
	#define kFileM				1
	#define kEditM				2
	#define kProjectM			3
	#define kProcessorM		4
	#define kPaletteM			5
	#define kOptionM			6
	#define kUtilitiesM		7
	#define kWindowM			8
	#define kOverlaysM		9
	#define kReformatM		10
	#define kAreaUnitsM		11
	
			// Number of menu items																
	#define 	kNumMenuItems			  12
		
			// Declare Menu Item Selections													
			// "Apple" menu selection															
	#define	kAboutMultiSpec		1
		
			// "File" menu selections															
	#define	kNewProject						1
	#define	kOpenImage						2
	#define	kOpenProject					3
	#define	kOpenProjImage					4
	#define	kOpenThematicClassGroup		5
	#define	kCloseImage						6
	#define	kSaveTransformMenuItem		8
	#define	kSave								10
	#define	kSaveOutputAs					11
	#define	kSaveProject					12
	#define	kSaveProjectAs					13
	#define	kPageSetUp						15
	#define	kPrintOutput					16
	#define	kQuit								18
	
			// "Edit" menu selections															
	#define	kUndo								1
	#define	kCut								3
	#define	kCopy								4
	#define	kPaste							5
	#define	kClear							6
	#define  kSelectAll						7
	#define	kEditSelectionRect			9
	#define	kClearSelectionRect			10
	#define	kClearTransformMenuItem		12
	#define	kChangeSpecs					14
	#define	kCoordinates					15
	#define	kClearOverlays					17
	
			// "Clear Overlays" submenu selection														
	#define	kAllImageOverlaysMenuItem		1
	#define	kAllVectorOverlaysMenuItem		2
	
			// "Project" menu selections														
	#define	kUseBaseStatistics			1
	#define	kUseLOOStatistics				2
	#define	kUseEnhancedStatistics		3
	#define	kMixedStatisticsUsed			4
	#define	kClearStatistics				6
	#define	kChangeBaseImageFile			7
	#define	kAssociatedImage				9
	
			// "Processor" menu selection														
	#define	kDisplaysMItem						1
	#define	kHistogramMItem					2
	#define	kListDataMenuItem					3
	#define	kReformatMItem						4
	#define  kClusterMItem						6
	#define  kStatisticsMItem					7
	#define  kStatEnhanceMItem					8
	#define  kFeatureExtractionMItem			9
	#define	kSeparabilityMItem				10
	#define  kClassifyMItem						11
	#define	kListResultsMItem 				12
	#define	kUtilitiesMItem					14
	#define	kUser1MItem							15
	
			// "Reformat" menu selection	
	#define	kReformatChangeHeaderMenuItem				1
	#define	kReformatChangeImageMenuItem				2
	#define	kReformatConvertMultispectralMenuItem	3
	#define	kReformatConvertProjectMenuItem			4
	#define	kReformatConvertShapeMenuItem				5
	#define	kReformatModifyChannelMenuItem			6
	#define	kReformatMosaicImagesMenuItem				7
	#define	kReformatRecodeThematicMenuItem			8
	#define	kReformatRectifyImageMenuItem				9
	#define	kReformatConvertENVIASCIIMenuItem		10
	
			// "Utilities" menu selection														
	#define	kPrincipalComponentsMenuItem	1
	#define	kStatisticsImageMenuItem		2
	#define	kBiPlotDataMenuItem				3
	#define	kListDescriptionMenuItem		4
	#define	kCovarianceCheckMenuItem		5
	#define	kCheckTransformMenuItem			6
	
			// "OpenImage" menu selection														
	#define	kMultispectral		1
	#define	kThematic			2
	
	/*
			// "Palette" menu selection
			// This menu is not used any more; 3/28/2014														
	#define	kSpectralClasses			1
	#define	kInfoGroups					2
	#define	kInvertPalette				4
	#define	kShowClosePaletteW		6
	#define	kBlink1MenuItem			8
	#define	kBlink2MenuItem			9
	#define	k8BitPaletteMenuItem		11
	*/
			// "Option" menu selection															
	#define	kPalette							1	// This menu was removed 3/28/2014 
	#define	kMemoryStatus					1
	#define	kDefaultAreaUnits				2
	#define	kSwitchCrossCursor			3
	#define	kDebug							5
	
			// "Window" menu selections													
	#define	kHideTheWindow					1
	#define	kShowHideCoordinateView		3
	#define	kSelectionGraphMenuItem 	5
	#define	kTextOutput						7
	#define  kPaletteWindow					8
	#define	kStatisticsWItem				9
	
			// Data Value Type Popup Menu
	#define	k4BitUnsignedIntegerMenuItem		1
	#define	k1ByteSignedIntegerMenuItem		2
	#define	k1ByteUnsignedIntegerMenuItem		3
	#define	k2ByteSignedIntegerMenuItem		4
	#define	k2ByteUnsignedIntegerMenuItem		5
	#define	k4ByteSignedIntegerMenuItem		6
	#define	k4ByteUnsignedIntegerMenuItem		7
	#define	k4ByteRealMenuItem					8 
	#define	k8ByteRealMenuItem					9
	
			// Header Popup Menu
	#define	kNoneMenuItem					1
	#define	kArcViewMenuItem				2
	#define	kERDAS74MenuItem				3
	#define	kGAIAMenuItem					4 
	#define	kTIFFGeoTIFFMenuItem			5
	#define	kMatlabMenuItem				6
	
			// Band Interleave Popup Menu
	#define	kBILMenuItem					1
	#define	kBSQMenuItem					2
	#define	kBISMenuItem					3 
	#define	kBNonSQMenuItem				4 
	#define	kBNonSQBlockedMenuItem		5 
	#define	kBIBlockMenuItem				6 
	
			// Output File Popup Menu
	#define	kNewFileMenuItem				1
	#define	kAppendToMenuItem				2
	#define	kModifyPartMenuItem			3 
	
			// Channels Popup Menu
	#define	kAllMenuItem					1
	#define	kSubsetMenuItem				2
	
			// Symbols Popup Menu
	#define	kDefaultMenuItem				1
	#define	kUserMenuItem					2
	
			// Weights Popup Menu
	#define	kEqualWeightMenuItem					1
	#define	kUnequalWeightMenuItem				2
	#define	kNumberSampleWeightMenuItem		3
	
			// System Architecture codes
	#define  k68K							1
	#define  kPPC							2
	#define  kIntel						3

			// Constants for suspend/resume and mouse moved events sent by 			
			// Multifinder.																			
	#define	kBackTimeOffset			5
	#define	kFrontTimeOffset			60
	#define	kDisplayTimeOffset		30
	#define	kBackYieldTime				10
	#define	kFrontYieldTime			2
	#define	kMouseMovedMessage		0xfa	// High byte of mouse-moved message.	
	#define	kResumeMask					1
	#define	kSuspendResumeMessage	1
	
			// Number of ticks returned by LMGetTicks() which represent 1 minute.
	#define	kTicksPerMinute			3600
	
			// Cursor Macros
	#define	WaitCursor()					::SetCursor (gWaitCursorPtr)
	#define	DefaultCursor()				::InitCursor ()
#endif	// defined multispec_mac

#if defined multispec_win 
	#undef _80BitDoubles_
	
	#if !defined DOUBLE
		#define	DOUBLE		double
	#endif		// !defined DOUBLE

			// Number of menu items																
	#define 	kNumMenuItems			  1 	// Not currently used in the Windows version

			// Menu indices 																		
	#define kAppleM						0
	#define kFileM							1
	#define kEditM							2
	#define kWindowM						3
	#define kProjectM						4
	#define kProcessorM					5
	#define kPaletteM						6
	#define kOptionM						7
	#define kUtilitiesM					8
	#define kOpenImageM					9        
	
			// Data Value Type Popup Menu
	#define	k4BitUnsignedIntegerMenuItem		0
	#define	k1ByteSignedIntegerMenuItem		1
	#define	k1ByteUnsignedIntegerMenuItem		2
	#define	k2ByteSignedIntegerMenuItem		3
	#define	k2ByteUnsignedIntegerMenuItem		4
	#define	k4ByteSignedIntegerMenuItem		5
	#define	k4ByteUnsignedIntegerMenuItem		6
	#define	k4ByteRealMenuItem					7 
	#define	k8ByteRealMenuItem					8
	
			// Header Popup Menu
	#define	kNoneMenuItem					0
	#define	kArcViewMenuItem				1
	#define	kERDAS74MenuItem				2
	#define	kGAIAMenuItem					3 
	#define	kTIFFGeoTIFFMenuItem			4
	#define	kMatlabMenuItem				5 
	
			// Band Interleave Popup Menu
	#define	kBILMenuItem					0
	#define	kBSQMenuItem					1
	#define	kBISMenuItem					2  
	#define	kBNonSQMenuItem				3 
	#define	kBNonSQBlockedMenuItem		4 
	#define	kBIBlockMenuItem				5
	
			// Output File Popup Menu
	#define	kNewFileMenuItem				1
	#define	kAppendToMenuItem				2
	#define	kModifyPartMenuItem			3
	
			// Channels Popup Menu
	#define	kAllMenuItem					0
	#define	kSubsetMenuItem				1
	
			// Symbols Popup Menu
	#define	kDefaultMenuItem				0
	#define	kUserMenuItem					1
	
			// Weights Popup Menu
	#define	kEqualWeightMenuItem					0
	#define	kUnequalWeightMenuItem				1
	#define	kNumberSampleWeightMenuItem		2

	#define	noErr								0
	#define	true32b							1
	 
	 		// Masks used by Macintosh OS
	 		// 	These will not be used by the Windows OS
	#define	activMask						0x0100
	#define	keyDownMask						0x0008
	#define	mDownMask						0x0002
	#define	mUpMask							0x0004
	#define	updateMask						0x0040
	#define	osMask							0x0000
	
	//#define	LONG_MAX							2147483647
	
			// Replacement for StripAddress on the Mac.
	#define StripAddress(x) ((Ptr)(x))
	
			// Replacement for LMGetTicks()
	//#define LMGetTicks()		(UInt32)GetTickCount() 
	#define TickCount()		(UInt32)GetTickCount() 
	
			// Cursor Macros
	#define	WaitCursor()					BeginWaitCursor()
	#define	DefaultCursor()				EndWaitCursor()
	
	#define	topLeft(r)	(((Point*)&(r))[0])
	#define	botRight(r)	(((Point*)&(r))[1])
	
	#define	blackColor						RGB(0,0,0)		// 33
	#define	redColor							RGB(255,0,0)	//205
	#define	greenColor						RGB(0,128,0)	//341
	#define	blueColor						RGB(0,0,255)	//409
	#define	cyanColor						RGB(0,255,255)	//273
	#define	magentaColor					RGB(255,0,255)	//137 
	#define	whiteColor						RGB(255,255,255)	//30   
	#define	yellowColor						RGB(255,255,0)	//69 
	
	#define	fsFromStart						1
	#define	fsFromLEOF						2
	#define	fsFromMark						3
	
	#define	dskFulErr           			-34
	#define	ioErr								-36
	#define	bdNamErr							-37
	#define	eofErr							-39
	#define	posErr							-40
	#define	tmfoErr							-42
	#define	fnfErr							-43
	#define	wPrErr							-44
	#define	fLckdErr							-45
	
	#define	kFrontTimeOffset				500
	#define	kDisplayTimeOffset			500
 
	#define 	RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
	#define 	RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top) 
	
	#define WM_DOREALIZE   (WM_USER + 0)   
	
	#define	kFrontYieldTime			0 
	
	#define	kReadyThreadState			0
	
			// Number of ticks returned by LMGetTicks() which represent 1 minute.
	#define	kTicksPerMinute			60000
	
	#define	ID_CLEAROVERLAYMENUITEMSTART	30001
	#define	ID_CLEAROVERLAYMENUITEMEND		30100
	
	#define	ID_SHOWOVERLAYMENUITEMSTART	30101
	#define	ID_SHOWOVERLAYMENUITEMEND		30200
	
	#define	ID_GRAPHOVERLAYMENUITEMSTART	30201
	#define	ID_GRAPHOVERLAYMENUITEMEND		30300
	
	#define	ID_SELECTVECTORMENUITEMSTART	30301
	#define	ID_SELECTVECTORMENUITEMEND		30400
	
	#define	ID_SELECTBINWIDTHMENUITEMSTART	30401
	#define	ID_SELECTBINWIDTHMENUITEMEND		30500	
#endif // defined multispec_win   

		// The following are constants that are shared in the linux, mac and windows 
		// versions.
		
#define SIGN2(a,b) ((b) >= 0.0 ? a : -a)

		// Image and output window constants

		//		Window type codes
#define	kProjectWindowType			0
#define  kOutputWindowType				1
#define	kImageWindowType				2
#define  kPaletteWindowType			3
#define	kStatisticsWindowType		4
#define	kThematicWindowType			5
#define	kGraphicsWindowType			6												
		
#define	kVerticalControl				0
#define	kHorizontalControl			1
#define	kCoordinateViewControl		2
#define	kZoomInControl					3
#define	kZoomOutControl				4
#define	kMagControl						5
#define	kOverlayControl				6
#define	kLegendScrollUpControl 		7
#define	kLegendScrollDownControl 	8
#define	kLegendTextControl 			9		// Currently not used.
#define	kLegendPaletteControl 		10
#define	kCoordinateUnitsControl 	11
#define	kAreaUnitsControl				12

#define	kBottomButtonWidth 			24
#define	kLegendScrollWidth 			16
#define	kLegendWidthChanger			4
#define	kMagWidth						42
#define	kZoomHeight						16

#define	kDisplayNoMessage				0
#define	kDisplayMessage				1

#define	kMaxNumberGWindows			30
#define	kMaxNumberIWindows			50
#define	kMaxNumberImageOverlays		16
#define	kNumberBaseWindows			3
#define	kImageWindowStart				3

		// Channel wavelength order codes
#define	kNotApplicable					0
#define	kInOrder							1
#define	kNotInOrder						2

#define	kChannelDescriptionLength		24

		// Graph window constants
#define	kNextGraphSetControl				0
#define	kPreviousGraphSetControl		1	
#define	kGraphVectorControl				2
#define	kGraphOverlayControl				3
#define	kGraphBinWidthControl			4
#define	kGraphXAxisLabelControl			5

		// Overlay codes
#define	kImageOverlay					1
#define	kVectorOverlay					2

		// Cursor indices
#define	kArrow						0
#define	kCross						1
#define	kWait							2
#define	kIBeam						3
#define	kHand							4
#define	kLegendWidth				5
#define	kLegendWidthDown			6
#define	kBlinkOpenCursor1			7
#define	kBlinkOpenCursor2			8
#define	kBlinkShutCursor1			9
#define	kBlinkShutCursor2			10
#define	kSpin							11
#define	kSpinEnd						14
#define	kHandClosed					15
#define	kCross2						16
 
		// Image format structure constants												
#define	kMixed						-1
#define	kBIL							1
#define	kBSQ							2
#define	kBIS							3 
#define	kBNonSQ						4 
#define	kBNonSQBlocked				5 
#define	kBIBlock						6
//#define	kBNonSQBlockedGeneral	7


#define	kWriteERDAS								1
#define	kModifyImageFormat					2
#define	kMosaicImages							3
#define	kFieldsToThematicFile				4
#define	kRectifyImage							5
#define	kMultispecToThematic					6
#define	kENVIROIToThematic 					7
#define	kModifyWDescriptions 				8
#define	kRecodeThematicImage 				9

		// File types.																			
#define	kNoneType							0
#define	kErdas73Type						1
#define	kMultiSpecType						2
#define	kAVIRISType							3
#define	kDefaultType						4
#define  kSunScreenDumpType				5
#define	kMultiSpecClassificationType	6
#define	kErdas74Type						7
#define  kVICARType							8
#define  kGAIAType							9
#define  kSADIEType							10
#define	kLARSYSMISTType					11
#define	kMatlabType							12
#define	kTGAType								13
#define	kGAIA2Type							14
#define	kTIFFType							15
#define	kLASType1							16
#define	kLASType2							17
#define	kHDF4Type							18
#define	kPDSType								19 
#define	kImagineType						20
#define	kLGSOWGType							21	
#define	kFastL7AType						22
#define	kENVIType							23
#define	kArcViewType						24
#define	kGeoTIFFType						25
#define	kWindowsBitMapType				26
#define	kArcViewShapeType					27
#define	kNETCDFType							28
#define	kNdfType								29
#define	kJPEG2000Type						30
#define	kPNGType								31
#define	kJPEGType							32
#define	kHDF5Type							33
#define	kArcInfoBinaryGridType			34
#define	kECWType								35
#define	kGRIBType							36
#define	kNETCDF2Type						37
#define	kArcGISASCIIGridType				38
#define	kGRASSASCIIGridType				39
#define	kHDF4Type2							40
#define	kGDALVRTType						41
#define	kNITFType							42
#define	kPCIDSKType							43
#define	kSRTMHGTType						44

#define	kArcViewDefaultSupportType		1024

		// GetFileHeaderString routine flags
#define	kNoSubstringAllowed				0
#define	kSubstringAllowed					1

		// Instrument Codes 
#define	kLandsatMSS							1
#define	kLandsatMSS5						2
#define	kLandsatTM							3
#define	kLandsatTM7							4
#define	kSPOT									5
#define	kSPOT4								6
#define	kSPOT5								7
#define	kASTER								8
#define	kMODIS								9
#define	kHYPERION							10
#define	kTANSO_FTS							11
#define	kTANSO_CAI							12
#define	kLandsatLC8_OLI_TIRS				13
#define	kLandsatLC8_OLI					14
#define	kLandsatLC8_TIRS					15
#define	kSentinel2A_MSI					16
#define	kSentinel2B_MSI					17
#define	kPeruSat								18

#define	kNumberInstrumentCodes			18

		// Image type codes
#define	kNoImageType						0
#define	kMultispectralImageType			2
#define	kThematicImageType				5
#define	kVectorImageType					7

		// Open Image constants														
#define	kDoNotSkipEqual				0
#define	kSkipEqual						1	
	
		// Open Image Filter popup constants
#define	kLoadHeader						0
#define	kMultiThematicShapeFiles	101
#define	kMultiThematicFiles			102
#define	kMultiFiles						103
#define	kThematicFiles					104
#define	kShapeFiles						105
#define	kProbablilityFiles			106
#define	kSTAFiles						107
#define	kTRLFiles						108
#define	kProjectFiles					109
#define	kTransformFiles				110
#define	kAllReadableFiles				111
#define	kAllReadableImageFiles		112
#define	kThematicGroupInfoFiles		113
#define	kAllTextFiles					114

		// Create New File constants
#define	kDoNotReplaceFlag			0
#define	kReplaceFlag				1

		// Date compression constants
		
#define	kNoCompression					0
#define	kRLECompression				1
#define	kNBITCompression				2
#define	kSkipHuffmanCompression		3
#define	kGZIPCompression				4
#define	kSzipCompression				5
#define	kJPEG2000Compression			6
#define	kPNGCompression				7
#define	kJPEGCompression				8
#define	kLZWCompression				9
#define	kPackBitsCompression			10
#define	kCCITTGroup3Compression		11
#define	kCCITTGroup4Compression		12
#define	kCCITT_1D_Compression		13
#define	kShuffleCompression			14
#define	kFletcher32Compression		15
#define	kScaleOffsetCompression		16
#define	kDeflateCompression			17
#define	kYCbCr_JPEGCompression		18
#define	kUnknownCompression			19

		// Data type constants
#define	kIntegerType				0
#define	kRealType					1

#define	kMaxValueToListWith_f	(9999999999.)
#define	kMinValueToListWith_f	(.000000001)

		// Thematic Display constants														
#define	kClassDisplay				1
#define	kGroupDisplay				2
#define	kGroupClassDisplay		3
#define	kBlink1						1
#define	kBlink2						2

		// Offscreen copy constants
#define	kClipboardCopy				1
#define	kSourceCopy					2
#define	kScrollCopy					3
#define	kDestinationCopy			4
#define	kPrinterCopy				5

#define	kNotFromImageUpdate		0
#define	kFromImageUpdate			1

		// Collapse class options
#define	kCollapseClass				1
#define	kDoNotCollapseClass		2
#define	kCollapseToHighest		3

		// Destination window codes
#define	kToImageWindow				1
#define	kToPrintWindow				2
#define	kToClipboardWindow		3
#define	kWindowsUseOrigin			4	// Only used in WTools polygon draw.

		// Processor Codes.																	
#define	kAboutMultiSpecProcessor				1
#define	kPolygonSelectionProcessor				5		
#define	kDisplayProcessor							10	
#define	kDisplayOverlayProcessor				15
#define	kHistogramProcessor						20
#define	kHistogramInDisplayProcessor			21
#define	kReformatProcessor						30
#define	kRefChangeHeaderProcessor				3010
#define	kRefChangeFileFormatProcessor			3020
#define	kRefMosaicImagesProcessor				3030
#define	kRefModifyChanDescriptionsProcessor	3040
#define	kRefFieldsToThematicFileProcessor	3050
#define	kRefShapeToThematicFileProcessor		3060
#define	kRectifyImageProcessor					3070
#define	kMultispecToThematicProcessor			3080
#define	kENVIROItoThematicProcessor 			3090
#define	kRecodeThematicImageProcessor			3100
#define	kClusterProcessor							40
#define	kStatisticsProcessor						50
#define	kListStatsProcessor						51
#define	kHistogramStatsProcessor				52
#define	kComputeStatsProcessor					53
#define	kSeparabilityProcessor					60
#define	kClassifyProcessor						70
#define	kListResultsProcessor					80
#define	kFeatureExtractionProcessor			90
#define	kStatEnhanceProcessor					95
#define	kCovarianceCheckProcessor				1010
#define	kListDataProcessor						1020
#define	kPrincipalComponentsProcessor			1030
#define	kStatisticsImageProcessor				1040
#define	kListDescriptionProcessor				1050
#define	kTransformCheckProcessor				1060
#define	kBiPlotDataProcessor						1070

#define	kOpenImageFileProcessor					10002
#define	kOpenProjectFileProcessor				10003
#define	kOpenProjectImageProcessor				10004
#define	kOpenThematicInfo							10005
#define	kLoadSaveTransformationMatrix			10006
#define	kSaveProcessor								10004
#define	kSaveOutputAsProcessor					10007
#define	kPrintProcessor							10008
#define	kSaveProjectProcessor					10009
#define	kSaveProjectAsProcessor					10009

#define	kChangeImageDescriptionProcessor		11005

#define	kChangeBaseImageProcessor				13009

#define	kBetter8BitPaletteProcessor			16011

#define	kSelectionGraphProcessor				17004
#define	kSelectionGraphStatsProcess			17005

#define	kZoomButtonControl						20001

		// Output Results Disk File Types.												
#define	kOutputWindowFormatCode 	1
#define	kAsciiFormatCode				2
#define	kClassifyFileCode				4
#define	kProbFormatCode				8
#define	kEchoFieldsCode				16
#define	kEchoClassesCode				32
#define	kClusterMaskCode				64
#define	kCreateImageOverlayCode		128
#define	kResultsFilesCode				0x000E
#define	kFromDescriptionCode			256
#define	kFromShapeToThematicCode	0x0200

	// Source for creation of TIFF image files.
#define	kFromImageWindow				1
#define	kFromReformat					2
#define	kFromClassification			3

		// Conversions done when reading lines of data.  They are
		// simple copies of data.  No information is changed.
#define k8BitTo8Bit									0x00800080
#define k8BitIntSignedTo16BitIntSigned			0x00810101
#define k8BitIntUnsignedTo16BitIntUnsigned	0x00800100
#define k8BitIntUnsignedTo16BitIntSigned		0x00800101
#define k8BitIntSignedTo32BitIntSigned			0x00810201
#define k8BitIntUnsignedTo32BitIntUnsigned	0x00800200
#define k8BitIntUnsignedTo32BitIntSigned		0x00800201
#define k8BitIntSignedTo32BitReal				0x00810203
#define k8BitIntUnsignedTo32BitReal				0x00800203
#define k8BitIntSignedTo64BitReal				0x00810403
#define k8BitIntUnsignedTo64BitReal				0x00800403
#define k16BitTo16Bit								0x01000100
#define k16BitIntSignedTo32BitIntSigned		0x01010201
#define k16BitIntUnsignedTo32BitIntUnsigned	0x01000200
#define k16BitIntUnsignedTo32BitIntSigned		0x01000201
#define k16BitIntSignedTo32BitReal				0x01010203
#define k16BitIntUnsignedTo32BitReal			0x01000203
#define k16BitIntSignedTo64BitReal				0x01010403
#define k16BitIntUnsignedTo64BitReal			0x01000403
#define k32BitTo32Bit								0x02000200
#define k32BitIntSignedTo32BitReal				0x02010203
#define k32BitIntUnsignedTo32BitReal			0x02000203
#define k32BitIntSignedTo64BitReal				0x02010403
#define k32BitIntUnsignedTo64BitReal			0x02000403
#define k32BitRealTo64BitReal						0x02030403
#define k64BitTo64Bit								0x04000400

		// Conversion of data in the Reformat processor.  They may involve changes in 
		// data values.
#define k8BitIntSignedTo8BitIntUnsigned		0x00810080
#define k8BitIntUnsignedTo8BitIntSigned		0x00800081
#define k8BitIntSignedTo16BitIntUnsigned		0x00810100
#define k8BitIntSignedTo32BitIntUnsigned		0x00810200
#define k16BitIntSignedTo8BitIntSigned			0x01010081
#define k16BitIntUnsignedTo8BitIntUnsigned	0x01000080
#define k16BitIntSignedTo8BitIntUnsigned		0x01010080
#define k16BitIntUnsignedTo8BitIntSigned		0x01000081
#define k16BitIntSignedTo16BitIntUnsigned		0x01010100
#define k16BitIntUnsignedTo16BitIntSigned		0x01000101
#define k16BitIntSignedTo32BitIntUnsigned		0x01010200
#define k32BitIntSignedTo32BitIntUnsigned		0x02010200
#define k32BitIntUnsignedTo32BitIntSigned		0x02000201
#define k32BitIntSignedTo16BitIntSigned		0x02010101
#define k32BitIntUnsignedTo16BitIntUnsigned	0x02000100
#define k32BitIntSignedTo16BitIntUnsigned		0x02010100
#define k32BitIntUnsignedTo16BitIntSigned		0x02000101
#define k32BitIntSignedTo8BitIntSigned			0x02010081
#define k32BitIntUnsignedTo8BitIntUnsigned	0x02000080
#define k32BitIntSignedTo8BitIntUnsigned		0x02010080
#define k32BitIntUnsignedTo8BitIntSigned		0x02000081
#define k32BitRealTo32BitIntSigned				0x02030201
#define k32BitRealTo32BitIntUnsigned			0x02030200
#define k32BitRealTo16BitIntSigned				0x02030101
#define k32BitRealTo16BitIntUnsigned			0x02030100
#define k32BitRealTo8BitIntSigned				0x02030081
#define k32BitRealTo8BitIntUnsigned				0x02030080
#define k64BitRealTo32BitReal						0x04030203
#define k64BitRealTo32BitIntSigned				0x04030201
#define k64BitRealTo32BitIntUnsigned			0x04030200
#define k64BitRealTo16BitIntSigned				0x04030101
#define k64BitRealTo16BitIntUnsigned			0x04030100
#define k64BitRealTo8BitIntSigned				0x04030081
#define k64BitRealTo8BitIntUnsigned				0x04030080

		// Dialog Boxes
#define kDoNotInitializeLineColumnValues		0
#define kInitializeLineColumnValues				1

#define kIntervalEditBoxesDoNotExist			0
#define kIntervalEditBoxesExist					1

		// BiPlot Processor Constants.													
#define	kDisplayPixels			1
#define	kPixelsAsSymbols		2
#define	kPixelsAsColor			4

#define	kOutlineClasses		1
#define	kClassesAsEllipses	2
#define	kClassesAsBorders		4

		// Popup Control Constants.
#define	kNoCheckItem			0		
#define	kCheckItem				1

		// List Data Processor Constants.
#define	kChannelLineList		1
#define	kChannelColumnList	2	

		// List Results Processor Constants.											
#define	kFieldSummary			1		// Field performance table
#define	kClassSummary			2		// Class performance table
#define	kGroupSummary			4		// Group performance table
#define	kAreasUsed				8		// List of coordinates for areas used.
#define	kAreasSummary			0x10	// Only used by classify processor

#define	kClassTable				1
#define	kGroupTable				2

#define	kDoNotCallCreatePalette		0
#define	kCallCreatePalette			1

#define	kSampleUnits			1
#define	kPercentUnits			2
#define	kAreaUnits				4 

		// Channels dialog box constants.
#define	kAllChannels					1

#define	kSelectItemsList				1
#define	kItemsListOnly					2
#define	kSelectedItemsListOnly		3
#define	kSelectPCList					4

		// Classifier Processor Constants.											
#define	kMaxLikeMode						1
#define	kMahalanobisMode					2
#define	kFisherMode							3
#define	kEchoMode							4
#define	kSupportVectorMachineMode		5
#define	kKNearestNeighborMode			6
#define	kEuclideanMode						7
#define	kCorrelationMode					8
#define	kCEMMode								9
#define	kParallelPipedMode				10

		// Support Vector Machine Classifier Constants.
#define	kC_SVC_Type						0
#define	kNU_SVC_Type					1
#define	kOne_Class_Type				2
#define	kEpsilon_SVR_Type				3
#define	kNU_SVR_Type					4

#define	kLinearKernel					0
#define	kPolynomialKernel				1
#define	kRBFKernel						2
#define	kSigmoidKernel					3

		// Correlation Classifier Constants.
#define	kNoCovarianceUsed				1
#define	kCommonCovariance				2
#define	kSampleCovariance				3

		// Parallel Piped Classifier Constants.
#define	kPPMinMaxCode							1
#define	kPPStandardDeviationCode			2
	
		// Structure Creation Constants
#define	kDoNoteCreateStructure		0 
#define	kCreateStructure				1 
	
		// Classify Output Disk File Types
#define	kClassifyASCIIOutputFormat				1
#define	kClassifyERDAS74OutputFormat			2
#define	kClassifyGAIAOutputFormat				3
#define	kClassifyTIFFGeoTIFFOutputFormat		4 

		// Cluster Processor Constants
#define	kSinglePass						1
#define	kISODATA							2

		// Display Processor Constants 
		
		//   Display min-max enhancement codes.											
#define	kPerChannelMinMaxNoClip		1
#define	kPerChannelMinMax				2
#define	kEntireDataRange				3
#define	kUserSpecified					4
#define	kThematicDefault				5

		//		Display Specifications Structure Constants
#define	kVertical						0
#define	kHorizontal						1 

		//	  Enhancement Constants
#define	kLinearStretch						1
#define	kEqualAreaStretch					2
#define	kGaussianStretch					3

		//		Display types
#define k1_ChannelThematicDisplayType	1
#define k1_ChannelGrayLevelDisplayType	2
///#define k2_ChannelDisplayType				4
#define k3_ChannelDisplayType				3
#define k3_2_ChannelDisplayType			5
#define kSideSideChannelDisplayType		4

		// Feature Extraction Processor Constants.
		
		//		Preprocess Code
#define	kNoPreprocess						0	
#define	kComputeNew							1	
#define	kUseLast								2

		// 	Feature Extraction Algorithm
#define	kNonparametricWeighted			1
#define	kDecisionBoundary					2
#define	kDiscriminantAnalysis			3
#define	kPreprocessingOnly				4

		//		Projection Pursuit Algorithm										
#define	kDoNotPreprocess					0
#define	kProjectionPursuit				1
#define	kPPFeatureSelection				2

		//		Preprocess initial grouping
#define	kUniformInitGrouping				1
#define	kLastGrouping						2

		//		First stage method			
#define	kUniformGrouping					1
#define	kTopDown								2
#define	kTopDownBottomUp					3

		// Feature Selection Processor Constants.										
#define	kBhattacharyya						1
#define	kEFBhattacharyya					2
#define	kMeanBhattacharyya				3
#define	kCovarianceBhattacharyya		4
#define	kNonCovarianceBhattacharyya	5
#define	kTransformedDivergence			6
#define	kDivergence							7
	
		// "Reformat" request	
#define	kReformatChangeHeaderRequest				1
#define	kReformatChangeImageRequest				2
#define	kReformatConvertMultispectralRequest	3
#define	kReformatConvertProjectRequest			4
#define	kReformatConvertShapeRequest				5
#define	kReformatModifyChannelRequest				6
#define	kReformatMosaicImagesRequest				7
#define	kReformatRecodeThematicRequest			8
#define	kReformatRectifyImageRequest				9
#define	kReformatConvertENVIASCIIRequest			10

		// Status Dialog Item Number Set Constants.									
#define	kStatusTitle1			1
#define	kStatusTitle2			2
#define	kStatusClassA			3
#define	kStatusClassB			4
#define	kStatusField			5
#define	kStatusLine				6
#define	kStatusCluster			7
#define	kStatusMinutes			8
#define	kStatusCommand			9
#define	kStatusTitle3			10

		// TIFF Header Constants.															
#define	kTIFFByte			1
#define	kTIFFASCII			2
#define	kTIFFShort			3
#define	kTIFFLong			4
#define	kTIFFRational		5
#define	kTIFFDouble			12

		// Map Coordinate Constants.														
#define	kCentralMeridian			-100.000
#define	kEarthRadius1				(6370997.0)
#define	kFalseEasting				0.000
#define	kFalseNorthing				0.000
#define	kXMapCoordinate11			1.0
#define	kYMapCoordinate11			1.0
#define	kOrigin						45.000
#define	kHorizontalPixelSize		1.0
#define	kVerticalPixelSize		1.0

#define	kUSSurveyFeetToMetersConversion 0.304800609601219
#define	kFeetToMetersConversion 0.3048

#define	kNoEllipsoidDefinedMenuItem			1
#define	kUserDefinedEllipsoidMenuItem			2
#define	kAiryEllipsoidMenuItem					3
#define	kAustralianEllipsoidMenuItem			4
#define	kBesselEllipsoidMenuItem				5
#define	kBesselModifiedEllipsoidMenuItem		6
#define	kClarke1866EllipsoidMenuItem			7
#define	kClarke1880EllipsoidMenuItem			8
#define	kEverestEllipsoidMenuItem				9
#define	kGRS1967ModifiedEllipsoidMenuItem	10
#define	kGRS80EllipsoidMenuItem					11
#define	kInternational1909EllipsoidMenuItem	12
#define	kKrassovskyEllipsoidMenuItem			13
#define	kSphereEllipsoidMenuItem				14
#define	kWGS72EllipsoidMenuItem					15
#define	kWGS84EllipsoidMenuItem					16

#define	kNoEllipsoidDefinedCode					0
#define	kUserDefinedEllipsoidCode				1
#define	kAiryEllipsoidCode						2
#define	kAustralianEllipsoidCode				3
#define	kBesselEllipsoidCode						4
#define	kBesselModifiedEllipsoidCode			5
#define	kClarke1866EllipsoidCode				6
#define	kClarke1880EllipsoidCode				7
#define	kEverestEllipsoidCode					8
#define	kGRS1967ModifiedEllipsoidCode			9
#define	kGRS80EllipsoidCode						10
#define	kInternational1909EllipsoidCode		11
#define	kKrassovskyEllipsoidCode				12
#define	kSphereEllipsoidCode						13
#define	kWGS72EllipsoidCode						14
#define	kWGS84EllipsoidCode						15

#define	kNoRSDefinedMenuItem							1
#define	kUserDefinedRSMenuItem						2
#define	kGeographicRSMenuItem						3
#define	kGaussKrugerRSMenuItem						4
#define	kGDA94RSMenuItem								5
#define	kGGRS87RSMenuItem								6
#define	kStatePlaneNAD27RSMenuItem					7
#define	kStatePlaneNAD83RSMenuItem					8
#define	kUTM_NAD27RSMenuItem							9
#define	kUTM_NAD83RSMenuItem							10
#define	kUTM_SAD69RSMenuItem							11
#define	kUTM_WGS72RSMenuItem							12
#define	kUTM_WGS84RSMenuItem							13
#define	kUTMRSMenuItem									14
#define	kByEPSGCodeMenuItem							15

#define	kNoRSDefinedCode								0
#define	kUserDefinedRSCode							1
#define	kGeographicRSCode								2
#define	kGaussKrugerRSCode							3
#define	kGDA94RSCode									4
#define	kGGRS87RSCode									5
#define	kStatePlaneNAD27RSCode						6
#define	kStatePlaneNAD83RSCode						7
#define	kUTM_NAD27RSCode								8
#define	kUTM_NAD83RSCode								9
#define	kUTM_SAD69RSCode								10
#define	kUTM_WGS72RSCode								11
#define	kUTM_WGS84RSCode								12
#define	kUTMRSCode										13
#define	kByEPSGCodeCode								14

#define	kNoneDefinedMenuItem							1
#define	kAlaskaConformalMenuItem					2
#define	kAlbersConicalEqualAreaMenuItem			3
#define	kAzimuthalEquidistantMenuItem				4
#define	kCylindricalEqualAreaMenuItem				5
#define	kEquidistantConicMenuItem					6
#define	kEquirectangularMenuItem					7
#define	kGeneralVerticalNearSideMenuItem			8
#define	kGnomonicMenuItem								9
#define	kHammerMenuItem								10
#define	kIntegerizedSinusoidalMenuItem			11
#define	kInterruptedGoodeHomolosineMenuItem		12
#define	kInterruptedMollweideMenuItem				13
#define	kKrovakMenuItem								14
#define	kLambertAzimuthalEqualAreaMenuItem		15
#define	kLambertConformalConicMenuItem			16
#define	kMercatorMenuItem								17
#define	kMillerCylindricalMenuItem					18
#define	kModifiedTransverseMercatorMenuItem		19
#define	kMollweideMenuItem							20
#define	kNewZealandMapGridMenuItem					21
#define	kOblatedEqualAreaMenuItem					22
#define	kObliqueMercatorMenuItem					23
#define	kOrthographicMenuItem						24
#define	kPolarStereographicMenuItem				25
#define	kPolyconicMenuItem							26
#define	kRobinsonMenuItem								27
#define	kSinusoidalMenuItem							28
#define	kSpaceObliqueMercatorMenuItem				29
#define	kStereographicMenuItem						30
#define	kTransverseMercatorMenuItem				31
#define	kTransvMercatorSouthOrientedMenuItem	32
#define	kVanderGrintenIMenuItem						33
#define	kWagnerIVMenuItem								34
#define	kWagnerVIIMenuItem							35

#define	kGeographicCode							-1
#define	kNotDefinedCode							0
#define	kAlaskaConformalCode						1
#define	kAlbersConicalEqualAreaCode			2
#define	kAzimuthalEquidistantCode				3
#define	kCylindricalEqualAreaCode				4
#define	kEquidistantConicCode					5
#define	kEquirectangularCode						6
#define	kGeneralVerticalNearSideCode			7
#define	kGnomonicCode								8
#define	kHammerCode									9
#define	kIntegerizedSinusoidalCode				10
#define	kInterruptedGoodeHomolosineCode		11
#define	kInterruptedMollweideCode				12
#define	kKrovakCode									13
#define	kLambertAzimuthalEqualAreaCode		14
#define	kLambertConformalConicCode				15
#define	kMercatorCode								16
#define	kMillerCylindricalCode					17
#define	kModifiedTransverseMercatorCode		18
#define	kMollweideCode								19
#define	kNewZealandMapGridCode					20
#define	kOblatedEqualAreaCode					21
#define	kObliqueMercatorCode						22
#define	kOrthographicCode							23
#define	kPolarStereographicCode					24
#define	kPolyconicCode								25
#define	kRobinsonCode								26
#define	kSinusoidalCode							27
#define	kSpaceObliqueMercatorCode				28
#define	kStereographicCode						29
#define	kTransverseMercatorCode					30
#define	kTransvMercatorSouthOrientedCode		31
#define	kVanderGrintenICode						32
#define	kWagnerIVCode								33
#define	kWagnerVIICode								34

		// Area Units Popup Menu Constants
#define	kNumberPixelsUnitsMenuItem				1
#define	kAreaUnknownUnitsMenuItem				2
#define	kSqKilometersUnitsMenuItem				3
#define	kHectareUnitsMenuItem					4
#define	kSqMetersUnitsMenuItem					5
#define	kSqCentimetersUnitsMenuItem			6
#define	kSqMillimetersUnitsMenuItem			7
#define	kSqMicrometersUnitsMenuItem			8
#define	kSqMilesUnitsMenuItem					9
#define	kAcresUnitsMenuItem						10
#define	kSqYardsUnitsMenuItem					11
#define	kSqUSSurveyFeetUnitsMenuItem			12
#define	kSqFeetUnitsMenuItem						13
#define	kSqInchesUnitsMenuItem					14

		// Coordinate Units Popup Menu Constants
#define	kLineColumnUnitsMenuItem				1
#define	kUnknownUnitsMenuItem					2
#define	kDecimalLatLongUnitsMenuItem			3
#define	kDMSLatLongUnitsMenuItem				4
#define	kKilometersUnitsMenuItem				5
#define	kMetersUnitsMenuItem						6
#define	kCentimetersUnitsMenuItem				7
#define	kMillimetersUnitsMenuItem				8
#define	kMicrometersUnitsMenuItem				9
#define	kMilesUnitsMenuItem						10
#define	kYardsUnitsMenuItem						11
#define	kUSSurveyFeetUnitsMenuItem				12
#define	kFeetUnitsMenuItem						13
#define	kInchesUnitsMenuItem						14

		// Coordinate Map Units Code
#define	kUnknownCode								0
#define	kDecimalDegreesCode						1
#define	kKilometersCode							2
#define	kMetersCode									3
#define	kCentimetersCode							4
#define	kMillimetersCode							5
#define	kMicrometersCode							6
#define	kMilesCode									7
#define	kYardsCode									8
#define	kUSSurveyFeetCode							9
#define	kFeetCode									10
#define	kInchesCode									11

		// Datum Menu Items
#define 	kNoDatumDefinedMenuItem						1
#define	kBeijing1954MenuItem							2
#define	kCampoMenuItem									3
#define	kGeocentricDatumAustralia1994MenuItem	4
#define	kGGRS87DatumMenuItem							5
#define 	kNAD27MenuItem									6
#define	kNAD83MenuItem									7
#define	kNAD83_CSRS_MenuItem							8	
#define	kNewZealandGeodeticDatum1949MenuItem	9
#define	kNGO1948_OslowMenuItem						10
#define	kDatum_OSGB_1936MenuItem					11
#define	kPulkovo1942DatumMenuItem					12
#define	kPulkovo1995DatumMenuItem					13
#define	kSAD69DatumMenuItem							14
#define	kWGS72DatumMenuItem							15
#define	kWGS84MenuItem									16
#define	kSphereDatumMenuItem							17

		// Datum Codes
#define 	kNoDatumDefinedCode						0
#define	kBeijing1954Code							1
#define	kCampoCode									2	// GeoTIFF Code 4221 or 6221
#define	kGeocentricDatumAustralia1994Code	3	// GeoTIFF Code 4283 or 6283
#define	kGGRS87DatumCode							4  // GeoTIFF Code 4121 or 6121
#define 	kNAD27Code									5	// GeoTIFF Code 4267
#define	kNAD83Code									6	// GeoTIFF Code 4269
#define	kNAD83_CSRSCode							7
#define	kNewZealandGeodeticDatum1949Code		8	// GeoTIFF Code 4272 or 6272
#define	kNGO1948_OslowCode						9
#define	kDatum_OSGB_1936Code						10	// GeoTIFF Code 4277 or 6277
#define	kPulkovo1942DatumCode					11	// GeoTIFF Code 4284
#define	kPulkovo1995DatumCode					12	// GeoTIFF Code 4284
#define	kSAD69DatumCode							13	// GeoTIFF Code 4291
#define	kWGS72DatumCode							14	// GeoTIFF Code 4322
#define	kWGS84Code									15	// GeoTIFF Code 4326
#define	kSphereDatumCode							16	// GeoTIFF Code 4035

		// Project constants.																
#define	kNoProject						0
#define	kProject							1

#define	kDoNotDisposePointers		0
#define	kDisposePointers				1

		// Project Statistics types.														
#define	kMeanStdDevOnly				1
#define	kMeanCovariance				2
#define	kPixelValuesOnly				3

#define	kFieldStatsOnly				1
#define	kClassStatsOnly				2

#define	kBothClassWtVectors			3
#define	kModifiedClassWtVector		2
#define	kNonModifiedClassWtVector	1

		// Covariance stats to be used for class or project. This is based on
		// how the statistics were computed.
#define	kNoStatisticsUsed				0x0000
#define	kOriginalStats					0x0001
#define	kLeaveOneOutStats				0x0002
#define	kEnhancedStats					0x0004
#define	kMixedStats						0x0008

#define	kAllCovarianceStats			0x0007

		// List Covariance Inversion Messages
#define	kDoNotUseListOneMessagePerClassFlag		0
#define	kUseListOneMessagePerClassFlag			1

		// Mixing parameter codes.
#define	kComputedOptimum				0x0001
#define	kUserSet							0x0002
#define	kIdentityMatrix				0x0003

		// Include/Don't include cluster field constants.
#define	kDontIncludeClusterFields		0
#define	kIncludeClusterFields			1

		// Matrix multiply constants.														
#define	kMat1ByMat2 					1
#define	kTransposeMat1ByMat2 		2
#define	kMat1ByTransposeMat2 		3 

		// Matrix inverse constants.														
#define	kGetDeterminantOnly	 		0
#define	kReturnMatrixInverse 		1 

		// Statistics window control codes.													
#define  kAddToListControl				13
#define	kEditNameControl				22
#define	kStatHistogramControl		23
#define	kStatHistogramClassControl	24
#define	kStatHistogramFieldControl	25
#define	kNewFieldControl				15
#define	kPolygonEnterControl			18
#define	kStatPrintControl				19
#define	kStatPrintClassControl		20
#define	kStatPrintFieldControl		21
#define	kToClassControl				17
#define	kToFieldControl				12
#define	kToProjectControl				16
#define	kUpdateStatsControl			14

		// Transform constants.																
#define	kNoTransformation				0
#define	kUseTransformation			1
		
#define	kDoNotLoadMatricesVectors	0
#define	kLoadMatricesVectors			1

		// Histogram processor compute codes.											
#define	kDoNotComputeHistogram		0
#define	kComputeHistogram				1
#define	kLoadHistogramFromDisk		2

		// Method codes.												
#define	kStoredInMemory				1
#define	kComputeFromImage				2
#define	kReadFromSupportFile			3

#define	kNoSetUpHistogramArray		0
#define	kSetUpHistogramArray			1

		// Histogram Bin Type Codes
#define	kBinWidthOfOne					0
#define	kBinWidthNotOne				1
#define	kDataValueIsBinIndex			2

		// List Header constants.															
#define	kLImageInfo						1
#define	kLProjectName					2
#define	kLProjectImageName			4
#define	kLStatType						8
#define	kLInputParametersLineOnly	16

		// Statistics Histogram constants												
#define	kChannelGrouping				1
#define	kClassFieldGrouping			2
#define	kColumnList						2

#define	kListData						1
#define	kPlotData						2

#define	kHistogramClass				1
#define	kHistogramField				2

#define	kListMatrixFormat				1
#define	kListSingleColumnFormat		2

#define	kGroupByChannels				1
#define	kGroupByFieldsClasses		2

		// Class to Group constants														
#define	kNewGroup						1
#define	kEditGroup						2
#define	kEditClass						3
#define	kMaxClassGroupNameLength	31

		// Dialog filter function constants.											
#define	kDialogItemTableLength			60
#define	kDItemReal							0x0001
#define	kDItemMinus							0x0002
#define	kDItemString						0x0004
#define	kDItemSType1						0x0008
#define	kDItemSType2						0x0010
#define	kSetUpDFilterTable				1
#define	kDoNotSetUpDFilterTable			0

		// Dialog setup constants.															
#define	kCopyScrap							1
#define	kDoNotCopyScrap					0

		// Dialog routine constants.
#define	kZeroNotAllowed					0
#define	kZeroAllowed						1
		
#define	kDoNotDisplayRangeAlert			0
#define	kDisplayRangeAlert				1

		// Variable lengths													
#define	k68881Bytes							12 			// Length of 68881 double value.

		// Variable minimums and maximums
#define	SInt64_MAX							9223372036854775807LL
#define	UInt32_MAX							4294967295U
#define	SInt32_MAX							2147483647L
#define	SInt32_MIN							-2147483648L
#define	SInt16_MAX							32767
#define	SInt16_MIN							-32768
#define	UInt16_MAX                 	65535

		// Class table constants.															
#define	kListNoWeights							-1
#define	kListNoSymbols							0L

		// Algorithm constants to count number of pixels.							
#define	kDefaultAlgorithm					0x0000
#define	kSPClusterCase						0x0002

		// Memory Constants
#define	kNoLock									0
#define	kLock										1
#define	kNoMoveHi								0
#define	kMoveHi									1

		// Project area type constants.	     												
#define	kTrainingType							1 
#define	kTestingType							2
#define	kAreaType								4
		
#define	kBothFieldTypes						3		// Both Training and Testing Types

		// Selection area constants.														
#define	kClearSelectionArea					1
#define	kDontClearSelectionArea				0

#define	kUseThreshold							1
#define	kDontUseThreshold						0

#define	kAdjustToBaseImage					1 
#define	kDontAdjustToBaseImage				0 

#if defined multispec_wx
    #define	kLineColumnUnits						0
    #define	kMapUnits                        1
    #define	kLatLongUnits							2
#else
    #define	kLineColumnUnits						1
    #define	kMapUnits								2
    #define	kLatLongUnits							3
#endif
		// File information structure initialization constants.					
#define	kNotPointer								0
#define	kPointer									1  
	
		// Reformat Transform Codes
#define	kNoTransform							0
#define	kAdjustChannel							1 
#define	kCreatePCImage							2
#define	kTransformChannels					3
#define	kFunctionOfChannels					4
#define	kAdjustChannelsByChannel			5

#define	kAlgebraicTransformGeneral			0
#define	kAlgebraicTransformNDVI				1
#define	kAlgebraicTransformRadiance		2
#define	kAlgebraicTransformThermal_K		1
#define	kAlgebraicTransformThermal_C		2
#define	kAlgebraicTransformThermal_F		3

#define	kNoFunction								0
#define	kFunctionMin							1
#define	kFunctionMax							2
#define	kFunctionSum							3
#define	kFunctionAverage						4
#define	kFunctionMedian						5
#define	kFunctionKthSmallestElement		6

#define	kDoNotKeepSameFormatAsInput		0
#define	kKeepSameFormatAsInput				1

		// Reformat Rectify Codes
#define	kTranslateScaleRotate				1
#define	kReprojectToReferenceImage			2

#define	kIMAGCode								0x494D4147	// 'IMAG'

		// Reformat Mosaic Codes
#define	kMosaicLeftRight						1
#define	kMosaicTopBottom						2

	// MultiSpec file format constants      
#define	kBINAFileType							0x42494E41	// 'BINA'
#define	kISTAFileType							0x49535441	// 'ISTA'
#define	kITRLFileType							0x4954524C	// 'ITRL'
#define	kTRANFileType							0x5452414E	// 'TRAN'
#define	kTEXTFileType							0x54455854	// 'TEXT' 
#define	kGAIAFileType							0x47636C73	// 'Gcls'
#define	kMacSADIEFileType						0x72494D47	// 'rIMG'
#define	kMatlabFileType						0x4D415457	// 'MATW'
#define	kTGAFileType							0x54504943	// 'TPIC'
#define	kTIFFFileType							0x54494646  // 'TIFF'
#define	kICLRFileType							0x49434C52  // 'ICLR'

		// MultiSpec creator constants     
#define	kGAIACreator							0x47414941	// 'GAIA'
#define	kMacSADIECreator						0x474D3839	// 'GM89'
#define	kMatlabCreator							0x4D41544C	// 'MATL'

		// File IO Constants
#define	kDoNotForceBISFormat					0
#define	kDoNotPackData							0
#define	kForceBISFormat						1
#define	kNoSpecialFlag							0
#define	kPackData								1 

#define	kDoNotForceBytes						0
#define	kForce1Byte								1
#define	kForce2Bytes							2
#define	kForce4Bytes							4 
#define	kForceReal4Bytes						8
#define	kForceReal8Bytes						16
	
#define	kErrorMessages							1
#define	kNoErrorMessages						0  
	
#define	kLockFile								1
#define	kNoLockFile								0

#define	kDoNotVerifyFileStream				0
#define	kVerifyFileStream						1
	
#define	kDontResolveAliasChains				0
#define	kResolveAliasChains					1

#define	kSetSpecialBILFlagFalse				0
#define	kSetSpecialBILFlagTrue				1
#define	kDetermineSpecialBILFlag			2

		// GetWindowClipRectangle constants
#define	kNoScrollBarArea						0
#define	kImageArea								1
#define	kFrameArea								2
#define	kGraphArea								3
#define	kImageScrollArea						4
#define	kCoordinateArea						5
#define	kCoordinateCursorArea 				6
#define	kCoordinateSelectionArea 			7
#define	kImageFrameArea 						8
#define	kCoordinateAreaUnitsArea			9
#define	kGraphBinWidth							10
#define	kTitleArea								11
#define	kBottomScrollBar						12
	
		// Alert styles
#define	kAlert									0
#define	kNoteAlert								1
#define	kCautionAlert							2
#define	kStopAlert								3

		// Exception Warnings
#define	kMemoryMessage							1
#define	kObjectMessage							2 

		// Dialog Constants
#define	kHide										0
#define	kShow										1

		// Project Information Constants
#define 	kDoNotSetupGlobalInfoPointers		0
#define 	kSetupGlobalInfoPointers			1
#define	kSetupGlobalInfoPointersIfCan		2

#define	kDoNotPrompt							0
#define	kPrompt									1

#define	kEqualWeights							1
#define	kUnequalWeights						2
#define	kNumberSampleWeights					3

		// "Thematic Popup Palette"															
#define	kNoPaletteColorsDefined		0
#define  kDefaultColors					1
#define  kDefaultGrays					2
#define  kCorrelationMatrixColors	3
#define  kAVHRR_NDVI_Colors			4
#define	kMODIS_NDVI_Colors			5
#define  kFalseColors					6
#define  kImageDefaultColorTable		7
#define  kUserDefinedColors			8
#define  kCopyColorsFromClassTable	9

#define  kProbablilityColors			10
#define  kComputedGrays					11
#define  kPaletteHandle					12
#define	kProbablilityColors2			13
#define	kClassify_Info_Colors		14

		// "TRL support file constants"
#define	kPaletteOnly							1
#define	kHistogram								2
#define	kClassNames								4
#define	kPaletteHistogramClassNames		7 

		// ChannelMenu type
#define	kImageChannelType						1
#define	kProjectChannelType					2

		// Update Statistics Constants
#define	kUpdateProject							2
#define	kUpdateClass							3
#define	kUpdateField							4

		// Statistics Window Modes
#define	kSelectFieldMode						1
#define	kClassListMode							2
#define	kFieldListMode							3
#define	kCoordinateListMode					4

		// Flag indicating whether to clear area for project area outlines
#define	kDoNotClearArea						0
#define	kDoClearArea							1

		// Threaded IO constants
#define	kDoNotAllowForThreadedIO			0
#define	kAllowForThreadedIO					1

		// Class Info Structure constants
#define	kAssignMemory					1
#define	kAssignNoMemory 				0
#define	kNumberChannels				2
#define	kNumberFeatures				0
#define	kSquare							4
#define	kTriangular						0
#define	kAllowLoadingSquare			8
#define	kNotAllowLoadingSquare		0

		// Contants for "SetUpActiveImageInformationGlobals" routine
#define	kDoNotUseProject				0
#define	kUseProject						1

		// Matrix identifies constants
#define	kTriangleInputMatrix					0
#define	kSquareInputMatrix					1

#define	kTriangleOutputMatrix				0
#define	kSquareOutputMatrix					1

#define	kTriangularMatrices					0
#define	kTriangularInputMatrix1				0
#define	kTriangularInputMatrices			0

#define	kNotSquareMatrix						0

		// Option codes for GetClassDataValues & GetFieldDataValues
#define	kNoTrainingSamples					1
#define	kComputeColumnInterval				2

		// List Statistics option constants
#define	kListCorrelation						1
#define	kListCovariance						2
#define	kListMeansStdDev						4
#define	kListTransformedStats				8

		// Mask constants
#define	kTrainingMaskSet						1
#define	kTestMaskSet							2
#define	kMaskSet									3

#define	kNewMaskFields							1
#define	kCheckCurrentFields					2

		// Point types
#define	kRectangleType							1
#define	kPolygonType							2
#define	kClusterType							3
#define	kMaskType								4

		// Overlay constants
#define	kAllCorners								1
#define	kUpperLeftLowerRightCorners		2

		// Constants related to SetMapToWindowUnitVariables
#define	kNotCoreGraphics						0
#define	kCoreGraphics							1

		// File IO Constants
#define	kSkipLine								2 

		// Other constants																	
#define	kCopyInterval							25
#define  kDragMargin								10
#define  kEditStatistics						2
#define	kfsPosMode								1
#define	kGraphStatistics						4
#define	kHighLevelEvent						23
#define	kBlank									' '
#define	kCarriageReturn 						'\r' 
#define	kClassNumberOnly						1
#define	kCGrafType								1
#define  kColorLevelsMax 						256
#define  kColor2LevelsMax 						15
#define  kColor3LevelsMax 						6
#define	kDefaultCharWidth						6
#define	kDefaultLegendWidth					40
#define	kDefaultTitleHeight					15
#define	kDegreesFreedomCrossover			31
#define 	kDegreesToRadians						(0.0174532925199432958)
#define 	kEPSLN									1.0e-10
#define	kGrafType								0
#define 	kHALF_PI 								(1.57079632679489662)
#define 	kLN2										(.693147180559945286)
#define	kLineFeed		 						'\n'
#define	kListStatistics						3
#define	kMaxFieldsPerClass					512
#define	kMaxClassFieldNameLength			31
#define	kMaxNumberClasses						65536
#define	kMaxNumberDisplayClasses			65536
#define	kMaxNumberChannels					16384
#define	kMaxNumberColumns						300000
#define	kMaxNumberOSXDisplayColumns		32767

#if defined multispec_wx
	#define	kMaxNumberDisplayLines				100000
#endif
#if defined multispec_mac
	#define	kMaxNumberDisplayLines				32767
#endif
#if defined multispec_win
	#define	kMaxNumberDisplayLines				100000
#endif

#define	kMaxNumberLines						300000
#define	kMaxNumberStatChannels				16384
#define	kMaxNumberStatClasses				256
#define	kMaxNumberStatFields					32767
#define	kMaxNumberStatPoints					32767
#define	kMaxRowBytes 							8190		// 0x1ffe
#define	kQD32MaxRowBytes 						16364		// 0x3fec, documentation says 0x3ffe
#define	kMaxStartColumn						2147483647
#define	kMaxStartLine							2147483647
#define	kMaxThreshold							100
#define	kMinFreeMemory							20000
#define	kNewLineCharacter						'\012'
#define	kNewLinePosMode						0x0d81
#define	kNoForceClose							0
#define	kNullTerminator						'\0'
#define  kMaxColorValue							65535
#define	kMaxLength								10
#define  kNumPatterns							18
#define	kPaletteWidth							30
#define	kPI										(3.14159265358979323846)
#define 	kRadiansToDegrees						(57.2957795130823209)
#define 	kSBarWidth								15
#define	kStatClassListMode					320
#define	kStatFieldListMode					330
#define	kStatNewFieldPolyMode				312
#define	kStatNewFieldRectMode				311
#define	kStatPolygonListMode					340
#define	kSQRTPI 									(1.77245385090551603)
#define	kSQRT2PI									(2.5066282746310005)
#define  kTextWMargin							4 
#define 	kTWO_PI 									(6.28318530717958647692)

#define  NIL										0L

#define	kNoSwap									0

		// Color combination constants
#define	kGBColor									3
#define  kRBColor         						5
#define  kRGColor			 						6
#define  kRGBColor			 					7
#define  kGrayColor			 					8

#define 	kStatusBarHeight    					45

		// Use to request file name be returned in ASCII, Unicode (mac), or 
		// Wide character
#define	kReturnASCII							1
#define	kReturnUTF8								2
#define	kReturnUnicode							3

#define	kDefaultFileStringCode				kReturnASCII

		// Used to identify that the input string is in wide character format.
#define	kASCIICharString						1
#define	kUTF8CharString						2
#define	kUnicodeCharString					3

		// Code for GDAL library being used.
#define	kHDF4_Library							1
#define	kGDAL_Library							2

		// Code for specifying file name types to be zeroed out
#define	kDoNotKeepUTF8CharName				0
#define	kKeepUTF8CharName						1
#define	kKeepWideCharName						2

		// Mask codes for WindowInfo.descriptionCode variable
#define	kDescriptionExists					0x0001
#define	kReflectiveData						0x0002
#define	kThermalData							0x0004
#define	kBothReflectiveThermalData			0x0006
#define	kBandWidthInfoExists					0x0008

		// x Axis plot choices for selection graphs
#define	kChannels								1
#define	kWavelengths							2
#define	kBandWidths								3
#define	kReflectiveWavelengths				4
#define	kReflectiveBandWidths				5
#define	kThermalWavelengths					6
#define	kThermalBandWidths					7

		// Macros 
#define	MAX(a, b) (a > b ? a : b) 
#define	MIN(a, b) (a < b ? a : b)

#define	FS_DOUBLE		sizeof (double)
#define	FS_FLOAT			sizeof (float)
