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
//	File:						SExternalGlobals.h
//
//	Authors:					Larry L. Biehl
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contain globals declared as external
//
//	Revised By:				Abdur Maud				Date: 06/24/2013
//	Revised By:				Larry L. Biehl			Date: 05/03/2020
//	
//------------------------------------------------------------------------------------

#pragma once

#if defined multispec_wx
			// Storage for default (last used directory) for file input
   extern wxString		gDefaultDataDirectory;
	
			// Storage for file output. This is used when the output directory is
			// changed because the original directory is not writeable.
			// This is used for PutFile. If the parameter is empty, then one does not
			// need to change the output directory for PutFile.
   extern wxString		gOutputDirectory;
	
	extern int				g4BitUnsignedIntegerMenuItem;
	extern int				g1ByteSignedIntegerMenuItem;
	extern int				g1ByteUnsignedIntegerMenuItem;
	extern int				g2ByteSignedIntegerMenuItem;
	extern int				g2ByteUnsignedIntegerMenuItem;
	extern int				g4ByteSignedIntegerMenuItem;
	extern int				g4ByteUnsignedIntegerMenuItem;
	extern int				g4ByteRealMenuItem;
	extern int				g8ByteRealMenuItem;
	extern int				gTIFFGeoTIFFMenuItem;
	extern int				gNoneMenuItem;
	extern int				gArcViewMenuItem;
	extern int				gERDAS74MenuItem;
	extern int				gGAIAMenuItem;
	extern int				gMatlabMenuItem;

	//extern Boolean			gCallProcessorDialogFlag;
	//extern SInt16			gOutputForce1CodeSetting;

	#if defined multispec_wxmac
		extern int 			gFontSize;
	#else
		extern int			gFontSize;
	#endif
#endif	// defined multispec_wx


// Externally Declared Global Variables 

		// The MultiSpec processor address used to send Apple Events to itself.	
extern AEAddressDesc					gSelfAddressDescription;

		// Used as an empty container when generating Apple Event.					
extern AEDesc							gNullDesc;

		// Used to store the most recent algebraic transformation function.
extern AlgebraicTransformationFunction		gAlgebraicTransformationFunction;

		//	structure to use for obtaining information about a field or area to	
		// be used to compute statistics, classify, etc.								
extern AreaDescription				gAreaDescription;

		// Structure containing the specifications for the biplot processor.		
extern BiPlotDataSpecsPtr			gBiPlotDataSpecsPtr;

		// true when off screen bit/pix map exists for the active window			
		// This is used to determine the type of cursor over the active window	
extern Boolean							gActiveOffscreenMapExists;

		// If true, alias manager is present. 															
extern Boolean							gAliasMgrPresentFlag;
				
		// If true, appearance manager is present.	
extern Boolean							gAppearanceManagerFlag;

		// If true, apple events is present. 															
extern Boolean							gAppleEventsFlag;

		// If true, the system architecture is that for the 680x0 and PowerPC computers
		// If false, the system architecture is that for the Intel computers 
extern Boolean							gBigEndianFlag; 

		// TRUE if the processor dialog is to be called.
		// FALSE if the processor dialog is not to be called.
		// This flag is currently used for case when arrow keys are used to display the
		// the next channel in the list
extern Boolean							gCallProcessorDialogFlag;

		// Flag indicating whether this is a carbon application.
extern Boolean							gCarbonFlag;

		// if true, application is being run on Mac with a color screen. 						
extern Boolean							gColorQDflag;

		// This is used for the save dialog box.		
extern Boolean							gConfirmReplacementAlertFlag;

		// Flag indicating whether signed data needs to be shifted so that it	
		// is positive.																			
extern Boolean							gConvertSignedDataFlag;

		// when true drag gray rgn coordinate diolog box will be displayed		
extern Boolean							gCoordinateFlag;

		// Flag indicating whether the cpp new operator has been called to
		// load in the block of memory that it uses. This is used to try
		// to make sure that the block of memory does not fragment the heap.
extern Boolean							gCPPNewCalledFlag;

		// when true debug output will be printed 										
extern Boolean							gTestFlag;

		// Flag indicating whether the memory message should be displayed when
			// running under OS X. This flag helps in determining how much memory
				// is available without alerting the user with the memory short message.
extern Boolean							gDisplayOSXMemoryMessageFlag;

		// true if user has chosen quit item 															
extern Boolean							gDoneFlag;

		// True if user double clicked on a list item. 										
extern Boolean							gDoubleClickFlag;

		// Flag indicating whether the drag manager is present.
extern Boolean							gDragMgrPresentFlag;

		// Flag indicating whether the selected lines and columns in the dialog	
		// box represent the entire image.													
extern Boolean							gEntireImageFlag;

		// Flag indicating whether the alert has already been displayed indicating
		// that the epsg_csv folder was not found.
extern Boolean							gEPSG_csv_FolderAlertDisplayedFlag;

		// Flag to indicate whether a call in a list click loop is for the		
		// first time.																				
extern Boolean							gFirstTimeClikLoop;

		// Flag to indicate whether the Font Manager is available.
extern Boolean							gFontMgrPresentFlag;

	// This is used in the Online (linux) version to automatially display a file on start up.
extern Boolean							gFromToolParameterFileFlag;

		// Flag indicating whether project capability is allowed in MultiSpec. This
		// flag when false, allows a simpler version of MultiSpec.
extern Boolean							gFullVersionFlag;

		// This flag indicates when the user has displayed the about box such that 
		// the simpler version checkbox is displayed.
extern Boolean							gFullVersionSettingFlag;

		// true if the Gestalt Manager is available.										
extern Boolean							gGestaltAvailableFlag;

		// Flag indicating whether the drag manager is present.
extern Boolean							gHasDragManager;

		// if true, the operating system has FSSpec call available.  This			
		// began with system 7.0 or thereabouts.													
extern Boolean							gHasFSSpecCalls;

		// Flag indicating whether the icon utilities are present.
extern Boolean							gHasIconUtilitiesFlag;

		// Flag indicating whether the thread manager is present.
extern Boolean							gHasThreadManager;

		// If true, WaitNextEvent trap is available.  If it is false, the			
		// GetNextEvent must be called.																	
extern Boolean							gHasWaitNextEvent;

		// Flag indicating whether the hdf data set selection flag has been 
		// displayed for the first time since MultiSpec started up.  It will only
		// be displayed one time for each time the MultiSpec application is started.
extern Boolean							gHDFDataSetSelectionAlertDisplayedFlag;

		// If true, application is running in the background.							
extern Boolean							gInBackground;

		// If TRUE, include the test menu item in the Option menu.							
extern Boolean							gIncludeTestMenuItemFlag; 

		// If TRUE, then messages like "variance was 0 ...." will only be listed
		// once per class. This is needed for Feature Selection when one goes
		// through many channel combinations. Otherwize the message could be 
		// listed thousands of times.
extern Boolean							gListOnlyOneMessagePerClassFlag;

		// Flag indicating whether map coordinates should be displayed in the
		// coordinate dialog window.															
extern Boolean							gMapCoordinateFlag;

		// Parameter that indicates if a memory alert dialog box has been displayed
		// for the current memory request in a call to NewPtr or NewHandle.			
extern Boolean							gMemoryAlertDisplayFlag;

		// Flag indicating whether Navigation Services are available.
extern Boolean							gNavServicesExistsFlag;

		// Flag indicating whether this window was just selected with a mouse	
		// down in the content.  This flag is used to help know whether to		
		// process a control event or make a selection.  Usually if the mouse	
		// down happened to be over an inactive window, then the mouse down is	
		// to just select the window, not for any other action.						
extern Boolean							gNewSelectedWindowFlag;

		// Flag indicating whether the last operation was cancelled by the user 
extern Boolean							gOperationCanceledFlag;

		// Flag indicating whether OSX core graphics should be used.
extern Boolean							gOSXCoreGraphicsFlag;

		// Flag indicating whether MultiSpec is being run under OSX
extern Boolean							gOSXFlag;

		// Flag indicating whether MultiSpec is being run under an OSX version
		// before 10.1. If so printing and copying are not available.
extern Boolean							gOSXPre10_1Flag;

		// Flag indicating whether output can be listed in the output text		
		// window for the current processor.  It will always be set true at the	
		// beginning of the processor and set false if memory is too full to		
		// list something in the text window. 												
extern Boolean							gOutputTextOKFlag;

		// Flag indicating whether the print driver is loaded.						
extern Boolean 						gPrinterOK;

		// Flag indicating if 32 bit quick draw is implemented on the Mac.		
extern Boolean							gQD32IsImplemented;

		// Flag indicating whether the menu bar needs to be redrawn.				
extern Boolean							gRedrawMenuBar;

		// Flag indicating whether the selection rectangle exists.  It is used	
		// in various dialog boxes.															
extern Boolean							gSelectionRectangleFlag;

		// Flag indicating whether StretchDIBits is supported by the video
		// driver. Only used in the Windows version.															
extern Boolean							gStretchDIBitsFlag; 

		// Flag indicating whether HFS Plus volumes are support. In others words
		// are the calls available to access files larger than 2 gigabytes.
extern Boolean							gHasHFSPlusAPIs;

		// Flag indicating whether specified two and four byte integers are to be
		// swapped.  This depends upon the file type and the computer
		// architecture.													
extern Boolean							gSwapBytesFlag;

		// Flag indicating whether the processor menu items need to be updated.	
extern Boolean							gUpdateEditMenuItemsFlag;

		// Flag indicating whether the processor menu items need to be updated.	
extern Boolean							gUpdateFileMenuItemsFlag;

		// Flag indicating whether the project menu items need to be updated.	
extern Boolean							gUpdateOptionsMenuItemsFlag;

		// Flag indicating whether the project menu items need to be updated.	
extern Boolean							gUpdatePaletteMenuItemsFlag;

		// Flag indicating whether the processor menu items need to be updated.	
extern Boolean							gUpdateProcessorMenuItemsFlag;

		// Flag indicating whether the project menu items need to be updated.	
extern Boolean							gUpdateProjectMenuItemsFlag;

		// Flag indicating whether the project menu items need to be updated.	
extern Boolean							gUpdateWindowsMenuItemsFlag;

		// Flag indicating whether the GWorld capability is to be used.			
extern Boolean							gUseGWorldFlag;

		// Flag indicating whether the thread manager is present.
extern Boolean							gUseThreadedIOFlag;

		// Pointer to temporary buffer to be used for computation of a 			
		// class channel statistics vector.													
extern HChannelStatisticsPtr		gTempChannelStatsPtr;

		// pointer to classification specification structure							
extern ClassifySpecsPtr				gClassifySpecsPtr;

		// pointer to classification specification structure							
extern ClusterSpecsPtr				gClusterSpecsPtr;

		// pointer to file stream for debug file.
extern CMFileStream*					gDebugFileStreamPtr;

		// Pointer to the selection graph window.											
extern CMGraphView*					gSelectionGraphViewCPtr;

		// Pointer to the active image view class. This is
		// is holder for a window's variable.											
extern CMImageView*					gActiveImageViewCPtr;

		// output grafPort window record pointer 											
extern CMTextView*					gOutputViewCPtr; 

		// UniversalProcPtr for the control action routines.					
extern ControlActionUPP				gGraphChangeSetControlActionPtr;						
extern ControlActionUPP				gImageWScrollActionPtr;
extern ControlActionUPP				gImageWThumbScrollActionPtr;
extern ControlActionUPP				gImageZoomControlActionPtr;
extern ControlActionUPP				gOutputWScrollActionPtr;
extern ControlActionUPP				gThematicLegendControlActionPtr;

#if defined multispec_mac
				// Control definition pointer storage for carbon applications.
	extern ControlDefSpec				gControlDefinitionSpec;
#endif

extern ControlHandle					gCurrentControlHandle; 
extern ControlHandle					gStatusProgressControlHandle;

		// Pointers for cursors
extern CursPtr 						gCursorPtr[16];

		// image data values to image display levels 									
extern DataToDisplayLevel 			gToDisplayLevels;

		// Structure to be used to store the date and time in.						
extern DateTimeRec					gDateTimeRecord;

		// Dialog pointer to the status dialog window.  This is used by several	
		// of the processors.																	
extern DialogPtr						gStatusDialogPtr;

		// Information about list of currently open disk files being used for 	
		// results input or output.															
extern DiskFileSummary				gResultsFileSummary;

		// Structure used to store some of the user settings for the last 
		// displayed image which may be used for the next image that is opened.
extern DisplaySpecsDefault			gDisplaySpecsDefault;

		// UniversalProcPtr for the drag grey region routines.						
extern DragGrayRgnUPP				gDragScrollListPtr;

		// Pointer to the echo classifier variable structure.							
extern EchoClassifierVarPtr 		gEchoClassifierVariablePtr;

		//	Declare event record																	
extern EventRecord					gEventRecord;

			// Pointer to the feature extraction information structure.				
extern FeatureExtractionSpecsPtr	gFeatureExtractionSpecsPtr;

		// Active image file information pointer.  If it is NULL then the 		
		// file information handle does not exist or it is not locked.				
extern FileInfoPtr					gImageFileInfoPtr;

		//Image file information pointer.  If it is NULL then the 		
		// file information handle does not exist or it is not locked.		
		// This is usually used when also reading probability data along with
		// associated classified data.			
extern FileInfoPtr					gImageFileInfo2Ptr;

		// Pointer to buffer to be used for reading data from disk to memory.	
extern HUCharPtr						gInputBufferPtr;

		// Pointer to second buffer to be used for reading data from disk to memory.
		// This is usually used when also reading probability data along with
		// associated classified data.	
extern HUCharPtr						gInputBuffer2Ptr;

		// Pointer to buffer to use for packing the data just read from disk 	
		// into memory into the format to be used by the processor.					
extern HUCharPtr						gOutputBufferPtr;

		// Pointer to buffer to use for packing the data just read from disk 	
		// into memory into the format to be used by the processor.		
		// This is usually used when also reading probability data along with
		// associated classified data.				
extern HUCharPtr						gOutputBuffer2Ptr;

		// Structure containing instructions needed to read data from a disk		
		// file.																						
extern FileIOInstructions			gFileIOInstructions[2];

		// Pointer to structure containing instructions for what data to read	
		// from an input file.																	
extern FileIOInstructionsPtr		gFileIOInstructionsPtr;							
extern FileIOInstructionsPtr		gFileIOInstructions2Ptr;

		// Structure which contains finder information about a disk file.			
extern FInfo							gFinderInfo;

		// Structure which contains an FSSpec record.  This global structure		
		// is shared by several of the Apple Event routines.							
extern FSSpec							gFileFSSpec;

		// Pointer the the graph record currently being used.							
extern GraphPtr						gGraphRecordPtr;

		// Handle to active image file information 										
extern Handle							gActiveImageFileInfoH;

		// active image window info handle													
extern Handle							gActiveImageWindowInfoH;

		// active image layer info handle.													
extern Handle							gActiveImageLayerInfoH;

		// active window info handle															
extern Handle							gActiveWindowInfoH;

		// Handle to saved channel subsets.																					
extern Handle							gChannelSubsetsH;

		// Handle to character buffer 1.														
extern Handle							gCharBufferHandle1;

		// Handle to character buffer 2.														
extern Handle							gCharBufferHandle2;

		// Handle to character buffer 3.														
extern Handle							gCharBufferHandle3;

		// Structure to store information for available image overlays. 										
//extern ImageOverlayInfo				gImageOverlayInfoList[kMaxNumberImageOverlays];

		// Handle to list of image overlay info handles that the user has loaded in.
extern Handle							gImageOverlayHandle;

		// Available window structure handle list. 										
extern Handle							gImageWindowHandleList[kMaxNumberIWindows];

		// Handle to memory to be used for reading data from disk to memory.		
extern Handle							gIOBufferHandle;

		// Handle to output window information structure.
extern Handle							gOutputWindowInfoH;

		// Handle to output window record.													
extern Handle							gOutputWindowRecordHandle;

		// Handle to palette window record.													
extern Handle							gPaletteWindowHandle;

		// Handle to information about memory for IO related to graphing			
		// data represented by area selections.											
//extern Handle							gSelectionIOInfoHandle;

		// Handle to list of shape file handles that the user has loaded in.
extern Handle							gShapeFilesHandle;

		// Handle to statistics window record.												
extern Handle							gProjectWindowHandle;

		// Handle to spare memory to use to load code resources in.					
extern Handle							gSpareCodeMemoryH;

		// Handle to spare memory to use for presenting warning message if		
		// needed.																					
extern Handle							gSpareWarnMemoryH;

		// Memory pointers to use for matrix inversion calculations.				
extern InverseMatrixMemory			gInverseMatrixMemory;

		// UniversalProcPtr for the FileIO completion routine.
extern IOCompletionUPP				gFileIOCompletionPtr;

		// Pointer to the image file (layer) information.								
extern LayerInfoPtr					gImageLayerInfoPtr;

		// Pointer to the image file (layer) information.		
		// This is usually used when also reading probability data along with
		// associated classified data.										
extern LayerInfoPtr					gImageLayerInfo2Ptr;

		// UniversalProcPtr for the legend list click loop routine.					
extern ListClickLoopUPP				gLegendCClickLoopPtr;

		// handle to list in dialog window													
extern ListHandle						gDialogListHandle;

		// handle to list in dialog window													
extern ListHandle						gDialogListHandle2;

		// handle to list in statistics window for Macintosh
		// or CListBox Class Pointer for Windows
#if defined multispec_mac || defined multispec_mac_swift
	extern ListHandle						gStatisticsListHandle;
#endif	// defined multispec_mac || defined multispec_mac_swift
	                        
#if defined multispec_win 
	extern CListBox*						gStatisticsListHandle;
#endif	// defined multispec_win 
#if defined multispec_wx 
   extern wxListBox* gStatisticsListHandle;    
#endif
		// Structure used by the MultiSpec to pass finder information about 		
		// files are to be handled by MultiSpec.	This structure was set up		
		// to handle both FSSpec and pre-FSSpec type information.					
extern LocalAppFile					gLocalAppFile;

		// Storage for a temporary long point.												
extern LongPoint						gTempLongPoint;

		// Storage for a temporary long rectangle.										
extern LongRect						gTempLongRect;

		// Handle to All Available - Subset... popup menu.								
extern MenuHandle						gPopUpAllAvailableSubsetMenu;

		// Handle to All Possible - Subset... popup menu.								
extern MenuHandle						gPopUpAllPossibleSubsetMenu;

		// Handle to channels popup menu.													
extern MenuHandle						gPopUpAllSubsetMenu;

		// Handle to enhancement treat 0's as popup menu.												
extern MenuHandle						gPopUpAreaUnitsMenu;

		// Handle to channels popup menu.													
extern MenuHandle						gPopUpBandInterleaveMenu;

		// Handle to bits of color popup menu.												
extern MenuHandle						gPopUpBitsOfColorMenu;

		// Handle to data value type popup menu.													
extern MenuHandle						gPopUpDataValueTypeMenu;

		// Handle to bits per data value popup menu.													
extern MenuHandle						gPopUpBitsPerDataValueMenu;

		// Handle to channel description popup menu.										
extern MenuHandle						gPopUpChanDescriptionMenu;

		// Handle to Spectral class/information group popup menu.					
extern MenuHandle						gPopUpClassGroupMenu;

		// Handle to Spectral class/information group popup menu.					
extern MenuHandle						gPopUpClassGroupMenu2;

		// Handle to classify procedure popup menu.										
extern MenuHandle						gPopUpClassifyProcedureMenu;

		// Handle to class popup menu.														
extern MenuHandle						gPopUpClassMenu;

		// Handle to bytes per data value popup menu.													
extern MenuHandle						gPopUpClusterMaskDiskFileMenu;

		// Handle to class popup menu.														
extern MenuHandle						gPopUpCollapseClassMenu;

		// Handle to compare parameter popup menu.														
extern MenuHandle						gPopUpCompareParameterMenu;

		// Handle to coordinates unit popup menu.														
extern MenuHandle						gPopUpCoordinateUnitsMenu;

		// Handle to covariance estimate popup menu.												
extern MenuHandle						gPopUpCovarianceEstimateMenu;

		// Handle to channels popup menu.													
extern MenuHandle						gPopUpDiskFileMenu;

		// Handle to channels popup menu.													
extern MenuHandle						gPopUpDiskFileFormatMenu;

		// Handle to display type popup menu.												
extern MenuHandle						gPopUpDisplayTypeMenu;

		// Variable indicating what the current echo algorithm is.				
		// It is used by echo classify dialog.  =1 is maximum likelihood, 				
		// =2 is fisher.		
extern MenuHandle						gPopUpEchoAlgorithmMenu;
	
		// Handle to display enhancement popup menu.										
extern MenuHandle						gPopUpFEAlgorithmMenu;
	
		// Handle to reformat transform function popup menu.										
extern MenuHandle						gPopUpFunctionMenu;

		// Handle to mosaic direction popup menu.														
extern MenuHandle						gPopUpGraphAxisLabelMenu;

		// Handle to hard threshold popup menu. Used in Enhance Statistics.								
extern MenuHandle						gPopUpHardThresholdMenu;

		// Handle to symbols popup menu.														
extern MenuHandle						gPopUpHeaderOptionsMenu;

		// Handle to display type popup menu.												
extern MenuHandle						gPopUpHistogramMethodMenu;

		// Handle to horizontal datum popup menu.														
extern MenuHandle						gPopUpHorizontalDatumMenu;

		// Handle to image overlay popup menu used in dialog boxes.												
extern MenuHandle						gPopUpImageOverlayMenu;
	
		// Handle to reformat transform function popup menu.										
extern MenuHandle						gPopUpLinkOptionsMenu;

		// Handle to statistics type popup menu.												
extern MenuHandle						gPopUpListDataFormatMenu;

		// Handle to map units popup menu.														
extern MenuHandle						gPopUpMapUnitsMenu;

		// Handle to mosaic direction popup menu.														
extern MenuHandle						gPopUpMosaicDirectionMenu;

		// Handle to enhancement min-max popup menu.														
extern MenuHandle						gPopUpMinMaxMenu;

		// Handle to output file popup menu.												
extern MenuHandle						gPopUpOutputFileMenu;

		// Handle to palette popup menu.														
extern MenuHandle						gPopUpPaletteMenu;

		// Handle to display enhancement popup menu.										
extern MenuHandle						gPopUpPreprocessMenu;

		// Handle to project changes popup menu.											
extern MenuHandle						gPopUpProjectChangesMenu;

		// Handle to projection ellipsoid popup menu.														
extern MenuHandle						gPopUpEllipsoidMenu;

		// Handle to projection popup menu.														
extern MenuHandle						gPopUpProjectionMenu;

		// Handle to projection type popup menu.														
extern MenuHandle						gPopUpReferenceSystemMenu;

		// Handle for resample code popup menu.								
extern MenuHandle						gPopUpResampleMenu;

		// Handle to save cluster statistics popup menu.								
extern MenuHandle						gPopUpSaveStatsMenu;

		// Handle for selection display units popup menu.								
extern MenuHandle						gPopUpSelectionDisplayUnitsMenu;

		// Handle to select test mask image popup menu.												
extern MenuHandle						gPopUpSelectTestMaskImageMenu;

		// Handle to threshold image popup menu.														
extern MenuHandle						gPopUpSelectThresholdImageMenu;

		// Handle to select train mask image popup menu.												
extern MenuHandle						gPopUpSelectTrainMaskImageMenu;

		// Handle to separability distance popup menu.									
extern MenuHandle						gPopUpSeparabilityDistanceMenu;

		// Handle to soft threshold popup menu. Used in Enhance Statistics.								
extern MenuHandle						gPopUpSoftThresholdMenu;

		// Handle to statistics type popup menu.												
extern MenuHandle						gPopUpStatisticsTypeMenu;

		// Handle to enhancement stretch type popup menu.												
extern MenuHandle						gPopUpStretchMenu;

		// Handle to symbols popup menu.														
extern MenuHandle						gPopUpSymbolsMenu;

		// Handle to display type popup menu.												
extern MenuHandle						gPopUpTemporaryMenu;

		// Handle to enhancement treat 0's as popup menu.												
extern MenuHandle						gPopUpTreatZeroAsMenu;

		// Handle to weights popup menu.														
extern MenuHandle						gPopUpWeightsMenu;

		// menu handle array 																	
extern MenuHandle						gMultiSpecMenus[kNumMenuItems];

		// UniversalProcPtr for the common dialog filter routines.					
extern ModalFilterUPP				gDialogListFilterPtr;
extern ModalFilterUPP				gProcessorDialogFilterPtr;

		// Structure containing handles to specifications that may be used for	
		// non-project processor specifications.											
extern NonProjProcessorSpecs		gNonProjProcessorSpecs;

		// List of file types that MultiSpec is allowed to recognize.				
extern OSType							gListTypes[9];

		// Pattern to be used for oulining selected areas.								
extern Pattern							gOutlinePenPattern;
	
		// patterns to be used for pattern images											
extern Pattern							gPatterns[kNumPatterns];

		// Location of the upper left corner of the next graph window.				
extern Point							gNextGraphicsWindowStart;

		// Location of the last mouse location when doing dynamic
		// scrolling using the thumb control.
extern Point							gOldThumbScrollPoint;

		// The process serial number for the currently executing MultiSpec.
#if defined multispec_mac
	extern ProcessSerialNumber			gProcessSerialNumber;
#endif	// defined multispec_mac

		// pointer to project file information												
extern ProjectInfoPtr				gProjectInfoPtr;

		// Temporary memory pointers used to hold a location							
		// in memory for GWorld pointers.  These are used to try to not			
		// fragment the heap when using GWorld routines for offscreen pixmaps.	
extern Ptr								gGWorldPlaceHolderList[kMaxNumberIWindows];

		// structure to coordinates of drag gray rectangle 							
extern Rect								gGrayRect;

		// Rectangle for the graphics box in the graphics status dialog.			
extern Rect								gStatusGraphicsBox;

		// rectangle to be used for storage for temporary rectangles.				
extern Rect								gTempRect; 

		// rectangle to be used for the view area of a window.						
extern Rect								gViewRect;   

		// Pointer to the current reformat options structure
extern ReformatOptionsPtr			gReformatOptionsPtr;

#if defined multispec_win
			// pen structure to be used for storing the color of the pen.
	extern CPen								gBlackPen;
	extern CPen								gBluePen;
	extern CPen								gCyanPen;
	extern CPen								gGreenPen;
	extern CPen								gMagentaPen;
	extern CPen								gRedPen;
	extern CPen								gWhitePen;
	extern CPen								gYellowPen;
	
	extern CRgn								gMFC_Rgn;
	
	extern COLORREF						gRGBColorList[8];
#endif	// defined multispec_win

#if defined multispec_wx
			// pen structure to be used for storing the color of the pen.
	extern wxPen								gBlackPen;
	extern wxPen								gBluePen;
	extern wxPen								gCyanPen;
	extern wxPen								gGreenPen;
	extern wxPen								gMagentaPen;
	extern wxPen								gRedPen;
	extern wxPen								gWhitePen;
	extern wxPen								gYellowPen;
	
	extern wxRegion							gMFC_Rgn;
	
	//extern COLORREF						gRGBColorList[8];
#endif	// defined multispec_wx

extern RGBColor						gCoordinateViewBackgroundColor;

extern RGBColor						gCurrentSelectedColor;

extern RGBColor						gInactiveForeGroundColor;

extern RGBColor						gOverlayColorList[7];

		// Temporary region handle.  The region is allocated in 						
		// InitializeMultiSpec.c																
extern RgnHandle						gSelectionClipRgn;

		// Temporary region handle.  The region is allocated in 						
		// InitializeMultiSpec.c																
extern RgnHandle						gTempRegion1;

		// Temporary region handle.  The region is allocated in 						
		// InitializeMultiSpec.c																
extern RgnHandle						gTempRegion2;

		// Pointer to the statistics enhancement information structure.			
extern StatEnhanceSpecsPtr			gStatEnhanceSpecsPtr;

			// Pointer to statistics histogram information.								
extern StatHistogramSpecsPtr		gStatHistogramSpecsPtr;

			// Pointer to the create statistics images information structure.		
extern StatisticsImageSpecsPtr	gStatisticsImageSpecsPtr;

		// Location to store the name of the file to be handled by MultiSpec.													
extern Str255							gAppFileName;

		// Buffer to be used to manipulate menu item strings.							
extern Str255							gMenuItemString;

		// buffer to be used to manipulate text strings									
extern Str255							gTextString;								
extern Str255							gTextString2;									
extern Str255							gTextString3;

		// Date version string
		// Application identifier string
extern char								gDateVersionString[64];
extern char								gApplicationIdentifierString[64];

		// buffer to be used to manipulate text strings		
extern wchar_t							gWideTextString[256];									
extern wchar_t							gWideTextString2[256];								
extern wchar_t							gWideTextString3[256];

		// Structure containing information for a transformation matrix.			
extern TransformationSpecs			gTransformationMatrix;

#if defined multispec_mac
	#if use_mlte_for_text_window
		extern TXNObject						gTextObject;
	#endif	// use_mlte_for_text_window
#endif	// multispec_mac

#if defined multispec_mac
	#if !TARGET_CPU_68K
				// Universal procedure pointer to the bridge routine to the word		
				// solution engine code resource.  This is only used for powerpc.		
		extern UniversalProcPtr				gWSEBridgePtr;
	#endif		// !TARGET_CPU_68K
#endif		// multispec_mac

		// UniversalProcPtr for dialog user item routines that stay loaded		
		// in memory.																				

extern UserItemUPP					gCreateOneColumnList1Ptr;
extern UserItemUPP					gCreateOneColumnList2Ptr;
extern UserItemUPP					gDrawBandInterleavePopUpPtr;
extern UserItemUPP					gDrawChannelsPopUpPtr;
extern UserItemUPP					gDrawDialogClassPopUpPtr;
extern UserItemUPP					gDrawGraphicsBoxPtr;
extern UserItemUPP					gDrawImageFilePopUpPtr;
extern UserItemUPP					gDrawSymbolsPopUpPtr;
extern UserItemUPP					gDrawWeightsPopUpPtr;
extern UserItemUPP					gHiliteOKButtonPtr;
extern UserItemUPP					gSetEntireImageIconPtr;

		// The image window information pointer.  If it is NULL,						
		// then an active image window does not exist or the 							
		// WindowInfoHandle is not locked.													
		// It is usually set up near the beginning of a processor function and	
		// "turned off" at the end of a function.											
extern WindowInfoPtr					gImageWindowInfoPtr;	

		// The image window information pointer.  If it is NULL,						
		// then an active image window does not exist or the 							
		// WindowInfoHandle is not locked.													
		// It is usually set up near the beginning of a processor function and	
		// "turned off" at the end of a function.			
		// This is usually used when also reading probability data along with
		// associated classified data.											
extern WindowInfoPtr					gImageWindowInfo2Ptr;

		// current active image window														
extern WindowPtr						gActiveImageWindow;  

		// current windows CDC pointer. This is only used by Windows.														
#ifndef multispec_wx
	extern CDC*								gCDCPointer;
#else
	extern wxDC*							gCDCPointer;
#endif
		// output grafPort window record pointer 											
extern WindowPtr						gOutputWindow;

		// window pointer to palette window													
extern WindowPtr						gPaletteWindow;

		// Image window associated with the palette window.							
extern WindowPtr						gPaletteImageWindow;

		// window pointer to project base window that was last used to make an	
		// area selection.																		
extern WindowPtr						gProjectSelectionWindow;

		// Pointer to the selection graph window.											
//extern WindowPtr						gSelectionGraphWindow;

		// window pointer to statistics window												
extern WindowPtr						gProjectWindow;

		// the active window																		
extern WindowPtr						gTheActiveWindow;

		// window list 																			
extern WindowPtr						gWindowList[
						kNumberBaseWindows+kMaxNumberIWindows+kMaxNumberGWindows];
				
#if defined multispec_mac				
#if !use_mlte_for_text_window		
			// UniversalProcPtr for the WASTE line break procedure.	
	extern WELineBreakUPP				gWENoWrapLineBreakProcPtr;

			// UniversalProcPtr for the word solution engine action routines.			
	//extern WSEScrollActionUPP			gOutputAutoScrollActionPtr;		
	extern WEScrollUPP					gOutputAutoScrollActionPtr;
#endif	// !use_mlte_for_text_window
#endif 	// defined multispec_mac						

		// handle to output text record 			
#if use_mlte_for_text_window
	extern TXNObject						gOutputTextH;
#endif
#if !use_mlte_for_text_window
	extern WEReference					gOutputTextH;
#endif

		// Declare errno for math library 													
// int							errno; 

		// Pointer to character buffer 1.													
extern HPtr								gCharBufferPtr1;									
extern HPtr								gCharBufferPtr2;									
extern HPtr								gCharBufferPtr3;

		// Pointer to the dialog description table.										
extern UInt16							gDialogItemListTable[3*kDialogItemTableLength];

		// Pointer to the dialog description table.										
extern UInt16*							gDialogItemDescriptorPtr;

		// Define the decimal separator for numbers
extern char								gDecimalSeparator;
                      
   	// End of line character(s). This is initialized for Macintosh
		// end of line characters.
extern char								gEndOfLine[3];
extern wchar_t							gWideEndOfLine[3];

		// Tab character.																			
extern char								gTextTab;

		// String of two carriage return characters.										
extern char								gTextCR[3];

		// Define the thousands separator for numbers
extern char								gThousandsSeparator;

		// Pointer to temporary buffer to be used for computation of a 			
		// class sums of squares matrix.														
extern HDoublePtr						gTempSumsSquaresPtr;

		// Pointer to temporary buffer to be used for computation of a 			
		// covariance or correlation matrices.												
extern HDoublePtr						gTempMatrixPtr;

		// Factors to use to convert from kilometers to meters to feet, etc.
extern double							gDistanceDisplayConversionFactors[12];

		// Factors to use to convert from kilometers to meters to feet, etc.
extern double							gDistanceFileConversionFactors[10];

		// Last coordinate line value.														
extern double							gDoubleCoordinateColumnValue;

		// Last coordinate line value.														
extern double							gDoubleCoordinateLineValue;

		// Maximum natural log value for double values;
extern double							gMaximumNaturalLogValue;

		// Minimum natural log value for double values;
extern double							gMinimumNaturalLogValue;

		// Variable containing the minimum magnification that is allowed.			
extern double							gMinMagnification;

		// Variable that is used by a processor for temporary storage.				
extern double							gTempDoubleVariable1;

		// Smallest determinant that is allowed for a valid matrix inversion.	
extern double							gLowerDeterminantOffsetAllowed;

		// Increment to be used when updating the graphics status box.				
extern double							gStatusBoxIncrement;

		// Right coordinates of the graphics status box which represents the		
		// portion of the work that has been completed.									
extern double							gStatusGraphicsRight;
            
#if defined multispec_win || defined multispec_wx
	extern int								gCrossCursorID;
#endif	// defined multispec_win

//extern int 								szip_allow_encoding;		// For HDF library

		// Variables used when converting between line-column units and window	
		// units.																					
extern SInt32							gChannelWindowInterval;
extern SInt32							gChannelWindowOffset;
extern SInt32							gChannelWindowWidth;
extern SInt32							gSideBySideChannels;
extern SInt32							gStartChannel;
extern SInt32							gStartChannelWindow;

		// Last coordinate line value.														
extern SInt32							gCoordinateColumnValue;

		// Last coordinate line value.														
extern SInt32							gCoordinateLineValue;

		// Holds the 4-letter code indicating that MultiSpec is the creator = 	
		// 'MLAR'.																					
extern SInt32							gCreator;

			// Variable that contains the maximum number of characters that		
			// are allowed in one line of text data.  This is used by				
			// 'ListString'. 																		
extern SInt32							gMaxCharsAllowedInLine;

			// Variable that contains the number of characters in the longest		
			// line of output text. 															
extern SInt32							gMaxCharsInLine;

		// The maximum number of files that can be linked.
		// This may be adjusted lower at start up if the system does not allow
		//	this many files to be open at a given time.
extern SInt32							gMaximumNumberOfLinkedFiles;

		// Maximum size of block of memory that can be moved.  The Mac OS 		
		// as a bug such that if a block of memory more than 9.5 meg or so		
		// is move then heap is corrupted.													
extern SInt32							gMaxMoveableBlockSize;

		// Minimum memory needed for drawing a dialog or alert box.					
extern SInt32							gMinMemoryForDrawingDialog;

		// Number of open files.					
extern SInt32							gNumberOfOpenFiles;

		// Number of open files.					
extern SInt32							gNumberOfOpenFilesLimit;

		// Minimum time offset allowed between scroll bar and zoom control		
		// operations.																				
extern UInt32							gControlOffset;

			// Variable for storage of the current number of selections in
			// the open image list. Will be used for knowing when to allow
			// for vector files in this list. They are only allowed for 
			// single selection lists.
extern SInt32							gOpenImageSelectionListCount;

			// Version of quickdraw that is available.									
extern SInt32							gQuickDrawVersion;

#if defined multispec_mac                                       		
#if TARGET_CPU_68K
		// Step value to use when scrolling output text window. This value
		// is used instead of GetControlProperty/SetControlProperty in
		// 68k version.									
	extern SInt32							gOutputScrollValue;

#endif	// TARGET_CPU_68K							      
#endif	// defined multispec_mac						

		// Offset to use for output selection buffer.									
extern SInt32							gSelectionOutputBufOffset;

		// Use for determining when to check for command-. for stopping.			
extern SInt32							gTimeOffset;

		// Maximum number of channels combinations to be considered in				
		// the Separability processor.														
extern UInt32							gMaxNumberChannelCombinations;

		// Maximum number of row bytes possible.  For non-32 bit QD systems, it	
		// is 0x2000.  For 32bit QD system it is 0x4000; actually it could be	
		// 0x8000 if the origin was change each time for the offscreen bit maps.
extern UInt32							gMaxRowBytes;   
                      
#if defined multispec_win || defined multispec_wx
			// Maximum number of row bytes possible for 24 bit offscreen maps. 
	extern UInt32							gMaxRowBytesFor24Bits; 
#endif	// defined multispec_win									

		// Uses for storage of next time to list minutes left information.
extern UInt32							gNextMinutesLeftTime;

		// Used for determining when to list the next minutes
		// left message.	
extern UInt32							gNextMinutesLeftTimeOffset;

		// Uses for storage of next time to list status information.				
#if defined multispec_mac || defined multispec_mac_swift
	extern UInt32							gNextStatusTime;
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	extern UInt32							gNextStatusTime;
#endif	// defined multispec_win

#if defined multispec_wx
	extern time_t							gNextStatusTime;
#endif	// defined multispec_wx

		// Used for determining when to list status information.						
extern UInt32							gNextStatusTimeOffset;

		// Uses for storage of next time to check for command-. for stopping.	
#if defined multispec_mac || defined multispec_mac_swift
	extern UInt32							gNextTime;
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	extern UInt32							gNextTime;
#endif	// defined multispec_win

#if defined multispec_wx
	extern time_t							gNextTime;
#endif	// defined multispec_wx

		// Count of the number of shape files that have been loaded in.
extern UInt32							gNumberImageOverlayFiles;

		// Count of the number of shape files that have been loaded in.
extern UInt32							gNumberShapeFiles;

		// Count of the length of the overlay image handle list.
extern UInt32							gImageOverlayHandleListLength;

		// Count of the length of the shape file handle list.
extern UInt32							gShapeHandleListLength;

			// Amount of spare memory to keep stored in a handle to be used to	
			// load code resources in when main memory runs out.						
extern UInt32							gSpareCodeSize;

			// Amount of spare memory to keep stored in a handle to be used to	
			// load code resources in when main memory runs out.						
extern UInt32							gSpareWarnSize;

		// Variable indicating the item number in the status dialog box that		
		// is to be used for the title.														
extern UInt32							gStatusIDNumber;

		// Flag indicating the last memory condition for DataPak's word			
		// solution engine.																		
extern UInt32							gTextMemoryMinimum;

			// Variable containing the time to yield to background applications.	
extern UInt32							gYieldTime;

		// Variable containing the legend width for the active window.				
extern SInt16							gActiveLegendWidth;

		// Variable indicating the type of the active window.							
extern SInt16							gActiveWindowType;

		// ID of alert to be used when the user cancels a process.					
extern SInt16							gAlertId;

		// Return code from alert used when the user cancels a process.			
extern SInt16							gAlertReturnCode;

		// ID of string to use in the alert when the user cancels a process.		
extern SInt16							gAlertStrID;

		// String number to use in the alert when the user cancels a process.	
extern SInt16							gAlertStringNumber;

		// Variable indicating what the current band interleave selection is.	
extern SInt16							gBandInterleaveSelection;

		// Identifies the routine to use for blinking classes/groups in			
		// Thematic type images.																
extern SInt16							gBlinkProcedure;

		// Variable indicating what the current bits per data value selection 	
		// is.																						
extern SInt16							gBitsPerDataValueSelection;

		// Variable indicating what the current channel description selection 	
		// is.																						
extern SInt16							gChanDescriptionSelection;

		// Variable indicating what the current channel selection type is.		
		// It is used by processor dialogs.  =1 is all channels, =2 is subset.	
extern SInt16							gChannelSelection;

		// variable indicating which area units menu item is currently checked.
extern SInt16							gCheckedAreaUnitsMenuItem;

		// variable indicating which window menu item is currently checked.
extern SInt16							gCheckedWindowMenuItem;

		// Variable indicating the limit of the number of classes for which		
		// a class list or legend will be created.										
extern UInt16							gClassListLimit;

		// Variable indicating what the current class area selection type is.	
		// It is used by processor dialogs.  =1 is all classes, =2 is subset.	
extern SInt16							gClassAreaSelection;

		// Number of entries in the palette that are uses for displaying 			
		// classes or groups in thematic type images.									
extern SInt16							gClassPaletteEntries;

		// Variable indicating what the current class selection type is.			
		// It is used by processor dialogs.  =1 is all classes, =2 is subset.	
extern SInt16							gClassSelection;

		// Variable indicating what the current class or group selection is.		
		// it is used by processor dialogs.  =1 is classes; =2 is information	
		// groups.																					
extern SInt16							gClassGroupSelection;

		// Variable indicating what the current classify procedure is.				
		// It is used by classify dialog.  =1 is maximum likelihood, 				
		// =2 is L1 minimum distance, =3 is L2 minimum distance, =4 is ECHO.		
extern SInt16							gClassificationProcedure;

		// Variable indicating what the current data value type selection 	
		// is.																						
extern SInt16							gDataValueTypeSelection;

		// Default units to be used for areas.
extern SInt16							gDefaultAreaUnits;

		// Variable containing the working default legend width.						
extern SInt16							gDefaultLegendWidth;

		// Variable containing the working default titleHeight.						
extern SInt16							gDefaultTitleHeight;

		// Variable containing the default palette to use for the TRL file		
		// associated with new classification images.									
extern SInt16							gDefaultImagePalette;

		// Variable containing the default palette to use for the TRL file		
		// associated with new probability image.											
extern SInt16							gDefaultProbabilityPalette;

		// Indicates which level of the dialog description table to use			
		// if the dialog filter is called.													
extern SInt16							gDialogItemTableSet;

		// Variable containing the time interval to be used in displaying a		
		// portion of an image on the screen. The units are ticks (1/60 second)	
extern SInt16							gDisplayIntervalTime;

#if defined multispec_win | defined multispec_wx
			// File filter index to use in the windows version for opening
			// files.
	extern SInt16							gFileFilterIndex;
#endif	// defined multispec_win

		// Variable indicating the file name length limit.
extern UInt16							gFileNameLengthLimit;

		// Variable indicating the file name length limit.
extern UInt16							gFilePathNameLengthLimit;

		// Variable indicating what the current file name selection is.			
extern SInt16							gFileNamesSelection;

		// Running total of the number of graphics windows that have been 		
		// created.																					
extern SInt16							gGraphicsWindowCount;

		// Location offset for the next graphics windows.								
extern SInt16							gGraphicsWindowOffset;

		// Variable containing the minimum grow area for a window.					
extern SInt16							gGrowAreaMinimum;

		// Variable containing the type of image window that was last selected.	
		// Thematic type or Multispectral type.											
extern SInt16							gGetFileImageType;

		// Variable containing the status of the file hook routine for GetFile.	
extern SInt16							gGetFileStatus;

		// Variable which contains the number of tries to get more memory to		
		// satisfy a memory request for code that is used by "MyGrowZone".		
extern SInt16							gGetMemoryTries;

		// Variable indicating what the current header options selection type 	
		// is. It is used by processor dialogs.  =1 is no header,					
		// =2 is 73 Erdas header, =3 is 74 Erdas header.								
extern SInt16							gHeaderOptionsSelection;

		// Variable indicating which method should be used to obtain the			
		// histogram information to be used.												
extern SInt16							gHistogramMethod;

			// File filter index to use for opening
			// image or shape files.
extern SInt16							gImageFileFilterIndex;

		// Variable indicating what the current interclass weight selection 		
		// type is.  It is used by processor dialogs.  =1 is equal weights, 		
		// =2 is unequal.																			
extern SInt16							gInterClassWeightsSelection;

		// Variable indicating what the last hdf data set selection was.  This
		// is used to help when users are making repeated accesses to similar
		// hdf files and want the same data set each time.  It is used with
		// gLastHDFNumberDataSets
extern SInt16							gLastHDFDataSetSelection;

		// Variable indicating the number of hdf data sets in the last hdf file
		// that was selected.  This is used to help when users are making repeated 
		// accesses to similar hdf files and want the same data set each time. It 
		// is used with gLastHDFDataSetSelection
extern SInt16							gLastHDFNumberDataSets;

		// Variable indicating the current selection in the link options popup menu.
extern SInt16							gLinkOptionsSelection;

		// Storage for the last scrap count that was loaded into the TEScrap.	
extern SInt16							gLastScrapCount;

		// Variable containing the maximum magnification that is allowed.			
extern UInt16							gMaxMagnification;

		// Maximum print page width.  Defined by QMS ColorScript 100.				
extern SInt16							gMaxPrintPageWidth;

		// Maximum print page height.  Defined by QMS ColorScript 100.				
extern SInt16							gMaxPrintPageHeight;

		// Parameter that when set to one indicates that not enough memory was	
		// available for a request.  It is set in "MyGrowZone" and is used in	
		// various loops in MultiSpec to allow one to exit from the loop.			
extern SInt16							gMemoryError;

			// Variable that indicates which type if is needed. This variable is	
			// used in the MyGrowZone routine.  =1 indicates Code resources; =2	
			// indicates non-Code resources and data, =3 indicates for spare		
			// memory.																				
extern SInt16							gMemoryTypeNeeded;


		// Code indicating whether the selected image is the first file for a 	
		// multiple linked set of image files = 1; or to add the selected image 
		// file to the active image window "file" = 2.							 		
extern SInt16							gMultipleImageFileCode;

		// Number of available "place holders" for GWorld pointers.					
extern SInt16							gNumberAvailableGWorldPtrs;

		// Number of available window handles. 											
extern SInt16							gNumberAvailableImageWindowHandles;
	
		// Number of file types in the 'gListTypes' array that MultiSpec			
		// can recognize.																			
extern SInt16							gNumberFileTypes;

		// Number of End of Line characters for OS.    						
extern SInt16							gNumberOfEndOfLineCharacters;

			// Current number of open graphics windows. 									
extern SInt16							gNumberOfGWindows;

			// Current number of open image windows. 										
extern SInt16							gNumberOfIWindows;

		// Variable containing the number of probability classes.					
extern UInt16							gNumberProbabilityClasses;

		// variable indicating which window menu item is currently checked.
extern SInt16							gOutlinedWindowMenuItem;

		// Variable indicating what the current output file selection is.			
extern SInt16							gOutputFileSelection;

		// Variable containing the factor to use in computing the horizontal		
		// scroll bar control values for the output text window.						
extern SInt16							gOutputHScrollFactor;

		// Variable containing the line height for the output text window.		
extern SInt16							gOutputLineHeight;

		// Variable containing the factor to use in computing the control			
		// values for the output text window.												
extern SInt16							gOutputVScrollFactor;

		// Variable indicating what the current data output request is.			
		// It is used by processor dialogs.  =1 is all text window, =2 is 		
		// 1's column on = output messages/results to text window.					
		// 2's column on = output messages/results to ascii disk file.				
		// 4's column on = output results to disk file in defined format.			
		//							see gOutputFormatCode.										
		// 8's column on = output probabality results disk file.						
		// 16's column on = output ECHO fields results disk file.					
		// 32's column on = output ECHO classes results disk file.					
		// Combinations of 1+2, 1+4, etc. can also be used.							
extern SInt16							gOutputCode;

		// Variable indicating what the current data output request is with		
		// the text output window being forced.  (See gOutputCode).					
extern SInt16							gOutputForce1Code;

		// Variable indicating what gOutputForce1Code will be set to. This is
		// being used to to turn off text output list for cases such as when
		// one is using the arrow keys to display the next or previous channel.
		// The output text window is not updated. This saves on time in updating
		// the output text window.					
extern SInt16							gOutputForce1CodeSetting;

		// Variable indicating what the current data output format request is	
		// for the results disk file.															
		// 1 = kErdas73Type																		
		// 2 = kMultiSpecType (not used)														
		// 3 = kAVIRISType (not used)															
		//	etc.																						 
extern SInt16							gOutputFormatCode;

		// Palette offset to use for displaying thematic type image legends.		
extern SInt16							gPaletteOffset;
      
      // Variable indicating the current selection of the palette menu
      // in the Display Thematic Dialog box.
extern SInt16							gPaletteSelection;

		// Variable indicating what the current principal component channel 		
		// selection type is.	It is used by processor dialogs.  =1 is all 		
		// PC channels, =2 is subset.															
extern SInt16							gPCChannelSelection;

		// Variable indicating which preprocess menu selection is the current
		// selection.
extern SInt16							gPreprocessSelection;

			// ID of present cursor																
extern SInt16							gPresentCursor;

		// Code indicating which processor is currently being used.					
		//		=0 none; =10 Display; =20 Histogram; =30 Reformat; =40 Cluster;	
		//		=50 Statistics; =60 Separability; =70 Classify;							
		//		=80 Display Results; =1010 Check Covariances; =1030 List Results;	
		//		=1040 Principal Components.													
extern SInt16							gProcessorCode;

extern SInt16							gListDataCode;

		// Variable indicating what the current save cluster statistics			
		// selection type is. It is used by processor dialogs.  =1 do not save, 
		// =2 save to new project, =3 save to current project, .						
extern SInt16							gSaveStatisticsSelection;

		// Variable indicating the currection selection in the select image
		// overlay popup menu that is used in dialog boxes. The first one in the
		// list is to create a new overlay.
extern SInt16							gSelectImageOverlaySelection;

		// Variable indicating what the current separability distance measure 	
		// is.  =1 is transformed divergence, =2 is divergence, =3 is 				
		// bhattacharyya, =4 is 'noncovariance bhattacharyya, =5 is error			
		// function bhattacharyya.																
extern SInt16							gSeparabilityDistance;

			// Index used with spinning cursor
extern SInt16							gSpinCursorIndex;

			// Variable which mode the statistics window is currently in.			
extern SInt16							gStatisticsMode;

		// Variable indicating the item number in the status dialog box that		
		// is to be used for the title.														
extern SInt16							gStatusTitleItemNumber;

		// Variable indicating what the current symbol selection type is.			
		// It is used by processor dialogs.  =1 is default symbols,					
		// =2 is user symbols.																	
extern SInt16							gSymbolSelection;

		// Code for macintosh system architecture
		//		1 = 68K
		//		2 = PowerPC
		//		3 = Intel
extern UInt16							gSystemArchitectureCode;

			// Code to indicate that memory for text window operation is short.	
extern SInt16							gTextMemoryShortCode;

		// Variable containing the threshold value for which any rectangle 		
		// selection in an image must be larger than to be considered a 			
		// selected ares.  This will allow a user to click in an image window 	
		// to select the window or to zoom in at that point and not consider 	
		// it a rectangle selection.															
extern SInt16							gThresholdSize;

			// Used in 'DragScrollList' routine for scrolling lists.
extern SInt16							gVerticalCellOffset;
extern SInt16							gSelectedCell;

			// Variable used as storage for number of viewable text lines in the	
			// text window.																		
extern SInt16							gViewLines;

			// Variable used as storage for number of viewable pixels in a line	
			// of characters in the text window.											
extern SInt16							gViewHPixels;

		// Variable indicating what the current weight selection type is.			
		// It is used by processor dialogs.  =1 is equal weights, 					
		// =2 is unequal.																			
extern SInt16							gWeightsSelection;

		// Font number to be used in the windows.
extern SInt16							gWindowTextFont;

#if defined multispec_wx                        
			// Disable other windows when executing list Data graph window
	extern wxWindowDisabler				*m_windowDisabler;
#endif

			// Structure contain information used for workflow processing. Currently
			// this is for testing and for a very specific project.
extern WorkFlowInfo					gMultiSpecWorkflowInfo;

#if defined multispec_wx
// Data to assign to each entry in wxCombo in coordinatebar
extern SInt16 gLineColumnUnitsMenuItem;
extern SInt16 gUnknownUnitsMenuItem;
extern SInt16 gDecimalLatLongUnitsMenuItem;
extern SInt16 gDMSLatLongUnitsMenuItem;
extern SInt16 gKilometersUnitsMenuItem;
extern SInt16 gMetersUnitsMenuItem;
extern SInt16 gCentimetersUnitsMenuItem;
extern SInt16 gMillimetersUnitsMenuItem;
extern SInt16 gMicrometersUnitsMenuItem;
extern SInt16 gMilesUnitsMenuItem;
extern SInt16 gYardsUnitsMenuItem;
extern SInt16 gFeetUnitsMenuItem;
extern SInt16 gInchesUnitsMenuItem;

// Area Units Popup Menu data variables

extern SInt16 gNumberPixelsUnitsMenuItem;
extern SInt16 gAreaUnknownUnitsMenuItem;
extern SInt16 gSqKilometersUnitsMenuItem;
extern SInt16 gHectareUnitsMenuItem;
extern SInt16 gSqMetersUnitsMenuItem;
extern SInt16 gSqCentimetersUnitsMenuItem;
extern SInt16 gSqMillimetersUnitsMenuItem;
extern SInt16 gSqMicrometersUnitsMenuItem;
extern SInt16 gSqMilesUnitsMenuItem;
extern SInt16 gAcresUnitsMenuItem;
extern SInt16 gSqYardsUnitsMenuItem;
extern SInt16 gSqFeetUnitsMenuItem;
extern SInt16 gSqInchesUnitsMenuItem;


// Legend Palette popup menu data variables
extern int	gNoPaletteColorsDefined;
extern int  gDefaultColors;
extern int  gDefaultColors;
extern int  gDefaultGrays;
extern int  gCorrelationMatrixColors;
extern int  gAVHRR_NDVI_Colors;
extern int  gMODIS_NDVI_Colors;
extern int  gFalseColors;
extern int  gImageDefaultColorTable;
extern int  gUserDefinedColors;
extern int  gCopyColorsFromClassTable;

extern int  gProbablilityColors;
extern int  gComputedGrays;
extern int  gPaletteHandle;
extern int  gProbablilityColors2;
#endif

#if defined multispec_mac
			// Parameter relative to carbon applications.
			
	extern ListDefSpec							gListDefinitionSpec;
	
	extern CFBundleRef 							gApplicationServicesBundle;
	
	extern CGColorSpaceCreateDeviceGrayPtr	gCGColorSpaceCreateDeviceGrayPtr;
	
	extern CGColorSpaceCreateDeviceRGBPtr	gCGColorSpaceCreateDeviceRGBPtr;
	
	extern CGColorSpaceCreateIndexedPtr		gCGColorSpaceCreateIndexedPtr;
	
	extern CGColorSpaceReleasePtr				gCGColorSpaceReleasePtr;
	
	extern CGContextAddLineToPointPtr		gCGContextAddLineToPointPtr;
	
	extern CGContextBeginPathPtr				gCGContextBeginPathPtr;
	
	extern CGContextClipToRectPtr				gCGContextClipToRectPtr;
	
	extern CGContextDrawImagePtr				gCGContextDrawImagePtr;
	
	extern CGContextFlushPtr					gCGContextFlushPtr;
	
	extern CGContextMoveToPointPtr			gCGContextMoveToPointPtr;
	
	extern CGContextReleasePtr					gCGContextReleasePtr;
	
	extern CGContextRestoreGStatePtr			gCGContextRestoreGStatePtr;
	
	extern CGContextSaveGStatePtr				gCGContextSaveGStatePtr;
	
	extern CGContextScaleCTMPtr				gCGContextScaleCTMPtr;
	
	extern CGContextSetAlphaPtr				gCGContextSetAlphaPtr;
	
	extern CGContextSetInterpolationQualityPtr	gCGContextSetInterpolationQualityPtr;
	
	extern CGContextSetLineWidthPtr			gCGContextSetLineWidthPtr;
	
	extern CGContextSetRGBFillColorPtr		gCGContextSetRGBFillColorPtr;
	
	extern CGContextSetShouldAntialiasPtr	gCGContextSetShouldAntialiasPtr;
	
	extern CGContextStrokePathPtr				gCGContextStrokePathPtr;
	
	extern CGContextTranslateCTMPtr			gCGContextTranslateCTMPtr;
	
	extern CGDataProviderCreateWithDataPtr	gCGDataProviderCreateWithDataPtr;
	
	extern CGDataProviderReleasePtr			gCGDataProviderReleasePtr;
	
	extern CGImageCreatePtr						gCGImageCreatePtr;
	
	extern CGImageGetBytesPerRowPtr			gCGImageGetBytesPerRowPtr;
	
	extern CGImageGetHeightPtr					gCGImageGetHeightPtr;
	
	extern CGImageGetWidthPtr					gCGImageGetWidthPtr;
	
	extern CGImageMaskCreatePtr				gCGImageMaskCreatePtr;
	
	extern CGImageReleasePtr					gCGImageReleasePtr;
	
	extern CGRectMakePtr							gCGRectMakePtr;
	
	extern QDBeginCGContextPtr					gQDBeginCGContextPtr;
	
	extern QDEndCGContextPtr					gQDEndCGContextPtr;
#endif	// defined multispec_mac
