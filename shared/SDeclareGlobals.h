//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
// File:						SDeclareGlobals.h
//
//	Authors:					Larry L. Biehl
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains the declaration of the globals use by
//								MultiSpec.
//
//	Revised By:				Abdur Maud				Date: 01/24/2013
//	Revised By:				Larry L. Biehl			Date: 01/04/2019
//
//------------------------------------------------------------------------------------

#ifndef __SDECGLOBAL__
#define __SDECGLOBAL__

#if defined multispec_lin
	#include "wx/pen.h"
	#include "wx/region.h"
	#include <math.h>
	#include <float.h>
	#define CPen wxPen
	#define CRgn wxRegion

			// current windows CDC pointer. This is only used by Windows and Linux.
	wxDC*			gCDCPointer = NULL;
	
			// Storage for default (last used directory) for file input
   wxString		gDefaultDataDirectory;

			// Data to assign to each entry in wxCombo in coordinatebar
	SInt16      gLineColumnUnitsMenuItem = 1;
	SInt16      gUnknownUnitsMenuItem = 2;
	SInt16      gDecimalLatLongUnitsMenuItem = 3;
	SInt16      gDMSLatLongUnitsMenuItem = 4;
	SInt16      gKilometersUnitsMenuItem = 5;
	SInt16      gMetersUnitsMenuItem = 6;
	SInt16      gCentimetersUnitsMenuItem = 7;
	SInt16      gMillimetersUnitsMenuItem = 8;
	SInt16      gMicrometersUnitsMenuItem = 9;
	SInt16      gMilesUnitsMenuItem = 10;
	SInt16      gYardsUnitsMenuItem = 11;
	SInt16      gFeetUnitsMenuItem = 12;
	SInt16      gInchesUnitsMenuItem = 13;

			// Area Units Popup Menu data variables
	SInt16		gNumberPixelsUnitsMenuItem = 1;
	SInt16		gAreaUnknownUnitsMenuItem = 2;
	SInt16		gSqKilometersUnitsMenuItem = 3;
	SInt16		gHectareUnitsMenuItem = 4;
	SInt16		gSqMetersUnitsMenuItem = 5;
	SInt16		gSqCentimetersUnitsMenuItem = 6;
	SInt16		gSqMillimetersUnitsMenuItem = 7;
	SInt16		gSqMicrometersUnitsMenuItem = 8;
	SInt16		gSqMilesUnitsMenuItem = 9;
	SInt16		gAcresUnitsMenuItem = 10;
	SInt16		gSqYardsUnitsMenuItem = 11;
	SInt16		gSqFeetUnitsMenuItem = 12;
	SInt16		gSqInchesUnitsMenuItem	= 13;

	int			g4BitUnsignedIntegerMenuItem	= k4BitUnsignedIntegerMenuItem;
	int			g1ByteSignedIntegerMenuItem = k1ByteSignedIntegerMenuItem;
	int			g1ByteUnsignedIntegerMenuItem = k1ByteUnsignedIntegerMenuItem;
	int			g2ByteSignedIntegerMenuItem = k2ByteSignedIntegerMenuItem;
	int			g2ByteUnsignedIntegerMenuItem = k2ByteUnsignedIntegerMenuItem;
	int			g4ByteSignedIntegerMenuItem = k4ByteSignedIntegerMenuItem;
	int			g4ByteUnsignedIntegerMenuItem = k4ByteUnsignedIntegerMenuItem;
	int			g4ByteRealMenuItem	= k4ByteRealMenuItem;
	int			g8ByteRealMenuItem	= k8ByteRealMenuItem;

	int			gNoneMenuItem = kNoneMenuItem;
	int			gArcViewMenuItem = kArcViewMenuItem;
	int			gERDAS74MenuItem = kERDAS74MenuItem;
	int			gGAIAMenuItem = kGAIAMenuItem;
	int			gTIFFGeoTIFFMenuItem = kTIFFGeoTIFFMenuItem;
	int			gMatlabMenuItem = kMatlabMenuItem;

	int			gNoPaletteColorsDefined = 0;
	int			gDefaultColors = 1;
	int			gDefaultGrays = 2;
	int			gCorrelationMatrixColors = 3;
	int			gAVHRR_NDVI_Colors = 4;
	int			gMODIS_NDVI_Colors = 5;
	int			gFalseColors = 6;
	int			gImageDefaultColorTable = 7;
	int			gUserDefinedColors = 8;
	int			gCopyColorsFromClassTable = 9;
	int			gProbablilityColors = 10;
	int			gComputedGrays = 11;
	int			gPaletteHandle = 12;
	int			gProbablilityColors2 = 13;

	#if defined multispec_wxmac
		int 			gFontSize = 12;
	#endif
	#if !defined multispec_wxmac
		int			gFontSize = 9;
	#endif
#endif   // defined multispec_lin

typedef struct 	DecisionTreeVar 
	{
	double				startThreshold;
	double				minimumThreshold;
	double				thresholdIncrement;
	
	SInt16				classifyAlgorithm;
	SInt16				distanceAlgorithm;
	SInt16				nodeProcedure;
   
	}	DecisionTreeVar, *DecisionTreeVarPtr;


// Declare Global Variables 

		// The MultiSpec processor address used to send Apple Events to itself.	
AEAddressDesc					gSelfAddressDescription;

		// Used as an empty container when generating Apple Event.					
AEDesc							gNullDesc;

		// Used to store the most recent algebraic transformation function.
AlgebraicTransformationFunction		gAlgebraicTransformationFunction;

		//	structure to use for obtaining information about a field or area to	
		// be used to compute statistics, classify, etc.								
AreaDescription				gAreaDescription;

		// Structure containing the specifications for the biplot processor.		
BiPlotDataSpecsPtr			gBiPlotDataSpecsPtr = NULL;

		// TRUE when off screen bit/pix map exists for the active window			
		// This is used to determine the type of cursor over the active window
Boolean							gActiveOffscreenMapExists = FALSE;

		// If TRUE, alias manager is present. 															
Boolean							gAliasMgrPresentFlag = FALSE;
				
		// If TRUE, appearance manager is present.	
Boolean							gAppearanceManagerFlag = FALSE;

		// If TRUE, apple events is present. 															
Boolean							gAppleEventsFlag = FALSE; 

		// If TRUE, the system architecture is that for the 680x0 and PowerPC computers
		// If FALSE, the system architecture is that for the Intel computers 
#if defined multispec_mac || defined multispec_mac_swift
	#if TARGET_CPU_X86 || TARGET_CPU_X86_64
		Boolean							gBigEndianFlag = FALSE;
	#else		// !TARGET_CPU_X86
		Boolean							gBigEndianFlag = TRUE;
	#endif		// TARGET_CPU_X86 ... else 
#endif	// defined multispec_mac || defined multispec_mac_swift
                                
#if defined multispec_win 
	Boolean							gBigEndianFlag = FALSE;
#endif	// defined multispec_win
   
#if defined multispec_lin
   Boolean							gBigEndianFlag = FALSE;
#endif

		// TRUE if the processor dialog is to be called.
		// FALSE if the processor dialog is not to be called.
		// This flag is currently used for case when arrow keys are used to display the
		// the next channel in the list
Boolean							gCallProcessorDialogFlag = TRUE;

		// Flag indicating whether this is a carbon application.
Boolean							gCarbonFlag = TRUE;

		// if TRUE, application is being run on Mac with a color screen. 						
Boolean							gColorQDflag = FALSE;

		// This is used for the put dialog box.
Boolean							gConfirmReplacementAlertFlag = TRUE;

		// Flag indicating whether signed data needs to be shifted so that it	
		// is positive.																			
Boolean							gConvertSignedDataFlag = FALSE;
	
		// Flag to be used to tell if user wants to show coordinate information 
		// from drag gray rgn printed to the dialog box 						
Boolean							gCoordinateFlag = FALSE;

		// Flag indicating whether the cpp new operator has been called to
		// load in the block of memory that it uses. This is used to try
		// to make sure that the block of memory does not fragment the heap.
Boolean							gCPPNewCalledFlag = FALSE;
	
		//	Flag to be used to tell if user wants debug information listed to 
		// the screen												
Boolean							gTestFlag = FALSE;

		// Flag indicating whether the memory message should be displayed when
			// running under OS X. This flag helps in determining how much memory
				// is available without alerting the user with the memory short message.
Boolean							gDisplayOSXMemoryMessageFlag = TRUE;

		// TRUE if user has chosen quit item 															
Boolean							gDoneFlag = FALSE;

		// True if user double clicked on a list item. 										
Boolean							gDoubleClickFlag = FALSE;

		// Flag indicating whether the drag manager is present.
Boolean							gDragMgrPresentFlag = FALSE;

		// Flag indicating whether the selected lines and columns in the dialog	
		// box represent the entire image.													
Boolean							gEntireImageFlag = FALSE;

		// Flag indicating whether the alert has already been displayed indicating
		// that the epsg_csv folder was not found.
Boolean							gEPSG_csv_FolderAlertDisplayedFlag = FALSE;

		// Flag to indicate whether a call in a list click loop is for the		
		// first time.																				
Boolean							gFirstTimeClikLoop = FALSE;

		// Flag to indicate whether the Font Manager is available.
Boolean							gFontMgrPresentFlag = FALSE;

		// This is used in the Online (linux) version to automatially display a file on start up.
Boolean							gFromToolParameterFileFlag = FALSE;

		// Flag indicating whether project capability is allowed in MultiSpec. This
		// flag when false, allows a simpler version of MultiSpec.
Boolean							gFullVersionFlag = TRUE;

		// This flag indicates when the user has displayed the about box such that 
		// the simpler version checkbox is displayed.
Boolean							gFullVersionSettingFlag = FALSE;

		// TRUE if the Gestalt Manager is available.										
Boolean							gGestaltAvailableFlag = FALSE;

		// Flag indicating whether the drag manager is present.
Boolean							gHasDragManager = FALSE;

		// if TRUE, the operating system has FSSpec call available.  This			
		// began with system 7.0 or thereabouts.													
Boolean							gHasFSSpecCalls = FALSE;

		// Flag indicating whether the icon utilities are present.
Boolean							gHasIconUtilitiesFlag = FALSE;

		// Flag indicating whether the thread manager is present.
Boolean							gHasThreadManager = FALSE;
                                                    
		// If TRUE, WaitNextEvent trap is available.  If it is FALSE, the			
		// GetNextEvent must be called.																	
Boolean							gHasWaitNextEvent = FALSE;

		// Flag indicating whether the hdf data set selection flag has been 
		// displayed for the first time since MultiSpec started up.  It will only
		// be displayed one time for each time the MultiSpec application is started.
Boolean							gHDFDataSetSelectionAlertDisplayedFlag = FALSE;

		// If TRUE, application is running in the background.							
Boolean							gInBackground = FALSE; 

		// If TRUE, include the test menu item in the Option menu.							
Boolean							gIncludeTestMenuItemFlag = FALSE; 

		// If TRUE, then messages like "variance was 0 ...." will only be listed
		// once per class. This is needed for Feature Selection when one goes
		// through many channel combinations. Otherwize the message could be 
		// listed thousands of times.
Boolean							gListOnlyOneMessagePerClassFlag = FALSE;

		// Flag indicating whether the file IO error message is to be given.
//Boolean							gIOErrorMessageFlag = TRUE; 

		// Flag indicating whether map coordinates should be displayed in the	
		// coordinate dialog window.															
Boolean							gMapCoordinateFlag = FALSE;

		// Parameter that indicates if a memory alert dialog box has been displayed
		// for the current memory request in a call to NewPtr or NewHandle.			
Boolean							gMemoryAlertDisplayFlag = FALSE;

		// Flag indicating whether Navigation Services are available.
Boolean							gNavServicesExistsFlag = FALSE;

		// Flag indicating whether this window was just selected with a mouse	
		// down in the content.  This flag is used to help know whether to		
		// process a control event or make a selection.  Usually if the mouse	
		// down happened to be over an inactive window, then the mouse down is	
		// to just select the window, not for any other action.						
Boolean							gNewSelectedWindowFlag = FALSE;

		// Flag indicating whether the last operation was cancelled by the user 
Boolean							gOperationCanceledFlag = FALSE;

		// Flag indicating whether OSX core graphics should be used.
Boolean							gOSXCoreGraphicsFlag = FALSE;

		// Flag indicating whether MultiSpec is being run under OSX
Boolean							gOSXFlag = FALSE;

		// Flag indicating whether MultiSpec is being run under an OSX version
		// before 10.1. If so printing and copying are not available. Note
		// that this flag only has meaning if being run under OSX.
Boolean							gOSXPre10_1Flag = FALSE;

		// Flag indicating whether output can be listed in the output text		
		// window for the current processor.  It will always be set TRUE at the	
		// beginning of the processor and set FALSE if memory is too full to		
		// list something in the text window. 												
Boolean							gOutputTextOKFlag = TRUE;

		// Flag indicating whether the print driver is loaded.						
Boolean 							gPrinterOK = FALSE;

		// Flag indicating if 32 bit quick draw is implemented on the Mac.		
Boolean							gQD32IsImplemented = TRUE;

		// Flag indicating whether the menu bar needs to be redrawn.				
Boolean							gRedrawMenuBar = FALSE;

		// Flag indicating whether the selection rectangle exists.  It is used	
		// in various dialog boxes.															
Boolean							gSelectionRectangleFlag = FALSE; 

		// Flag indicating whether StretchDIBits is supported by the video
		// driver. Only used in the Windows version.															
Boolean							gStretchDIBitsFlag = FALSE; 

		// Flag indicating whether HFS Plus volumes are support. In others words
		// are the calls available to access files larger than 2 gigabytes.
Boolean							gHasHFSPlusAPIs = FALSE;

		// Flag indicating whether specified two and four byte integers are to be
		// swapped.  This depends upon the file type and the computer
		// architecture.													
Boolean							gSwapBytesFlag = FALSE;

		// Flag indicating whether the processor menu items need to be updated.	
Boolean							gUpdateEditMenuItemsFlag = TRUE;

		// Flag indicating whether the processor menu items need to be updated.	
Boolean							gUpdateFileMenuItemsFlag = TRUE;

		// Flag indicating whether the project menu items need to be updated.	
Boolean							gUpdateOptionsMenuItemsFlag = TRUE;

		// Flag indicating whether the project menu items need to be updated.	
Boolean							gUpdatePaletteMenuItemsFlag = TRUE;

		// Flag indicating whether the processor menu items need to be updated.	
Boolean							gUpdateProcessorMenuItemsFlag = TRUE;

		// Flag indicating whether the project menu items need to be updated.	
Boolean							gUpdateProjectMenuItemsFlag = TRUE;

		// Flag indicating whether the project menu items need to be updated.	
Boolean							gUpdateWindowsMenuItemsFlag = TRUE;

		// Flag indicating whether the GWorld capability is to be used.			
Boolean							gUseGWorldFlag = FALSE;

		// Flag indicating whether the thread manager is present.
Boolean							gUseThreadedIOFlag = FALSE;

		// Pointer to temporary buffer to be used for computation of a 			
		// class channel statistics vector.													
HChannelStatisticsPtr		gTempChannelStatsPtr = NULL;

		// pointer to classification specification structure							
ClassifySpecsPtr				gClassifySpecsPtr = NULL;

		// pointer to classification specification structure							
ClusterSpecsPtr				gClusterSpecsPtr = NULL;

		// pointer to file stream for debug file.
CMFileStream*					gDebugFileStreamPtr = NULL;

		// Pointer to the selection graph window.											
CMGraphView*					gSelectionGraphViewCPtr = NULL;

		// Pointer to the active image view class. This is
		// is holder for a window's variable.																
CMImageView*					gActiveImageViewCPtr = NULL;

		// output grafPort window record pointer 											
CMTextView*						gOutputViewCPtr = NULL; 

		// UniversalProcPtr for the control action routines.							
ControlActionUPP				gGraphChangeSetControlActionPtr = NULL;						
ControlActionUPP				gImageWScrollActionPtr = NULL;
ControlActionUPP				gImageWThumbScrollActionPtr = NULL;
ControlActionUPP				gImageZoomControlActionPtr = NULL;
ControlActionUPP				gOutputWScrollActionPtr = NULL;
ControlActionUPP				gThematicLegendControlActionPtr = NULL;

#if defined multispec_mac
			// Control definition pointer storage for carbon applications.
	ControlDefSpec					gControlDefinitionSpec;
#endif	// defined multispec_mac

ControlHandle					gCurrentControlHandle = NULL;
ControlHandle					gStatusProgressControlHandle = NULL;

		// Pointers for cursors
CursPtr 							gCursorPtr[16];		

		// image data values to image display levels 									
DataToDisplayLevel 			gToDisplayLevels;

		// Structure to be used to store the date and time in.						
DateTimeRec						gDateTimeRecord;

		// Pointer to the decision tree classifier variable structure.				
DecisionTreeVarPtr 			gDecisionTreeVarPtr = NULL;

		// dialog pointer to grag gray rgn coordinate dialog box
// DialogPtr						gCoordinatePtr = NULL;

		// Dialog pointer to the status dialog window.  This is used by several	
		// of the processors.																	
DialogPtr						gStatusDialogPtr = NULL;

		// Information about list of currently open disk files being used for 	
		// results input or output.															
DiskFileSummary				gResultsFileSummary;

		// Structure used to store some of the user settings for the last 
		// displayed image which may be used for the next image that is opened.
DisplaySpecsDefault			gDisplaySpecsDefault;

		// UniversalProcPtr for the drag grey region routines.						
DragGrayRgnUPP					gDragScrollListPtr = NULL;

		// Pointer to the echo classifier variable structure.							
EchoClassifierVarPtr 		gEchoClassifierVariablePtr = NULL;

		//	Declare event record																	
EventRecord						gEventRecord;

			// Pointer to the feature extraction information structure.				
FeatureExtractionSpecsPtr	gFeatureExtractionSpecsPtr = NULL;

		// Active image file information pointer.  If it is NULL then the 		
		// file information handle does not exist or it is not locked.				
FileInfoPtr						gImageFileInfoPtr = NULL;

		//Image file information pointer.  If it is NULL then the 		
		// file information handle does not exist or it is not locked.		
		// This is usually used when also reading probability data along with
		// associated classified data.			
FileInfoPtr						gImageFileInfo2Ptr = NULL;

		// Pointer to buffer to be used for reading data from disk to memory.	
HUCharPtr						gInputBufferPtr = NULL;

		// Pointer to second buffer to be used for reading data from disk to memory.
		// This is usually used when also reading probability data along with
		// associated classified data.	
HUCharPtr						gInputBuffer2Ptr = NULL;

		// Pointer to buffer to use for packing the data just read from disk 	
		// into memory into the format to be used by the processor.					
HUCharPtr						gOutputBufferPtr = NULL;

		// Pointer to buffer to use for packing the data just read from disk 	
		// into memory into the format to be used by the processor.		
		// This is usually used when also reading probability data along with
		// associated classified data.				
HUCharPtr						gOutputBuffer2Ptr = NULL;

		// Structure containing instructions needed to read data from a disk		
		// file.																						
FileIOInstructions			gFileIOInstructions[2];

		// Pointer to structure containing instructions for what data to read	
		// from an input file.																	
FileIOInstructionsPtr		gFileIOInstructionsPtr = NULL;							
FileIOInstructionsPtr		gFileIOInstructions2Ptr = NULL;

		// Structure which contains finder information about a disk file.			
FInfo								gFinderInfo;

		// Structure which contains an FSSpec record.  This global structure		
		// is shared by several of the Apple Event routines.							
FSSpec							gFileFSSpec;

		// Pointer the the graph record currently being used.							
GraphPtr							gGraphRecordPtr = NULL;

		// Handle to active image file information 										
Handle							gActiveImageFileInfoH = NULL;

		// active image window info handle													
Handle							gActiveImageWindowInfoH = NULL;

		// active image layer info handle.													
Handle							gActiveImageLayerInfoH = NULL;

		// active window info handle															
Handle							gActiveWindowInfoH = NULL;

		// Handle to saved channel subsets.																					
Handle							gChannelSubsetsH = NULL;

		// Handle to character buffer 1.														
Handle							gCharBufferHandle1 = NULL;

		// Handle to character buffer 2.														
Handle							gCharBufferHandle2 = NULL;

		// Handle to character buffer 3.														
Handle							gCharBufferHandle3 = NULL;

		// Handle to store open file custom dialog items
Handle 							gCustomNavOpenList = NULL;

		// Handle to store put file custom dialog items
Handle 							gCustomNavPut = NULL;

		// Structure to store information for available image overlays. 										
// ImageOverlayInfo				gImageOverlayInfoList[kMaxNumberImageOverlays];

		// Handle to list of image overlay info handles that the user has loaded in.
Handle							gImageOverlayHandle = NULL;

		// Available window structure handle list. 										
Handle							gImageWindowHandleList[kMaxNumberIWindows];

		// Handle to memory to be used for reading data from disk to memory.		
Handle							gIOBufferHandle = NULL;

		// Handle to output window information structure.
Handle							gOutputWindowInfoH = NULL;

		// Handle to output window record.													
Handle							gOutputWindowRecordHandle = NULL;

		// Handle to palette window record.													
Handle							gPaletteWindowHandle = NULL;

		// Handle to list of shape file handles that the user has loaded in.
Handle							gShapeFilesHandle = NULL;

		// Handle to information about memory for IO related to graphing			
		// data represented by area selections.											
//Handle							gSelectionIOInfoHandle = NULL;

		// Handle to statistics window record.												
Handle							gProjectWindowHandle = NULL;

		// Handle to spare memory to use to load code resources in.					
Handle							gSpareCodeMemoryH = NULL;

		// Handle to spare memory to use for presenting warning message if		
		// needed.																					
Handle							gSpareWarnMemoryH = NULL;

		// Memory pointers to use for matrix inversion calculations.				
InverseMatrixMemory			gInverseMatrixMemory;

		// UniversalProcPtr for the FileIO completion routine.
IOCompletionUPP				gFileIOCompletionPtr = NULL;

		// Pointer to the image file (layer) information.								
LayerInfoPtr					gImageLayerInfoPtr = NULL;

		// Pointer to the image file (layer) information.		
		// This is usually used when also reading probability data along with
		// associated classified data.										
LayerInfoPtr					gImageLayerInfo2Ptr = NULL;

		// UniversalProcPtr for the legend list click loop routine.					
ListClickLoopUPP				gLegendCClickLoopPtr = NULL;

		// handle to list in dialog window													
ListHandle						gDialogListHandle = NULL;

		// handle to list in dialog window													
ListHandle						gDialogListHandle2 = NULL;

		// handle to list in statistics window for Macintosh
		// or CListBox Class Pointer for Windows
#if defined multispec_mac || defined multispec_mac_swift
	ListHandle						gStatisticsListHandle = NULL;
#endif	// defined multispec_mac || defined multispec_mac_swift
	                        
#if defined multispec_win 
	CListBox*						gStatisticsListHandle = NULL;
#endif	// defined multispec_win

#if defined multispec_lin
   wxListBox*		gStatisticsListHandle = NULL;  
#endif

		// Structure used by the MultiSpec to pass finder information about 		
		// files are to be handled by MultiSpec.	This structure was set up		
		// to handle both FSSpec and pre-FSSpec type information.					
LocalAppFile					gLocalAppFile;

		// Storage for a temporary long point.												
LongPoint						gTempLongPoint;

		// Storage for a temporary long rectangle.										
LongRect							gTempLongRect;

		// Handle to All Available - Subset... popup menu.								
MenuHandle						gPopUpAllAvailableSubsetMenu = NULL;

		// Handle to All Possible - Subset... popup menu.								
MenuHandle						gPopUpAllPossibleSubsetMenu = NULL;

		// Handle to channels popup menu.													
MenuHandle						gPopUpAllSubsetMenu = NULL;

		// Handle to enhancement treat 0's as popup menu.												
MenuHandle						gPopUpAreaUnitsMenu = NULL;

		// Handle to channels popup menu.													
MenuHandle						gPopUpBandInterleaveMenu = NULL;

		// Handle to bits of color popup menu.												
MenuHandle						gPopUpBitsOfColorMenu = NULL;

		// Handle to data value type popup menu.													
MenuHandle						gPopUpDataValueTypeMenu = NULL;

		// Handle to channels popup menu.													
MenuHandle						gPopUpBitsPerDataValueMenu = NULL;

		// Handle to channel description popup menu.										
MenuHandle						gPopUpChanDescriptionMenu = NULL;

		// Handle to Spectral class/information group popup menu.					
MenuHandle						gPopUpClassGroupMenu = NULL;

		// Handle to Spectral class/information group popup menu.					
MenuHandle						gPopUpClassGroupMenu2 = NULL;

		// Handle to classify procedure popup menu.										
MenuHandle						gPopUpClassifyProcedureMenu = NULL;

		// Handle to channels popup menu.													
MenuHandle						gPopUpClusterMaskDiskFileMenu = NULL;

		// Handle to covariance estimate popup menu.												
MenuHandle						gPopUpCovarianceEstimateMenu = NULL;

		// Variable indicating what the current echo algorithm is.				
		// It is used by echo classify dialog.  =1 is maximum likelihood, 				
		// =2 is fisher.		
MenuHandle						gPopUpEchoAlgorithmMenu = NULL;

		// Handle to class popup menu.														
MenuHandle						gPopUpClassMenu = NULL;

		// Handle to class popup menu.														
MenuHandle						gPopUpCollapseClassMenu = NULL;

		// Handle to compare parameter popup menu.														
MenuHandle						gPopUpCompareParameterMenu = NULL;

		// Handle to coordinate units popup menu.														
MenuHandle						gPopUpCoordinateUnitsMenu = NULL;

		// Handle to channels popup menu.													
MenuHandle						gPopUpDiskFileMenu = NULL;

		// Handle to channels popup menu.													
MenuHandle						gPopUpDiskFileFormatMenu = NULL;

		// Handle to display type popup menu.												
MenuHandle						gPopUpDisplayTypeMenu = NULL;
	
		// Handle to display enhancement popup menu.										
MenuHandle						gPopUpFEAlgorithmMenu = NULL;
	
		// Handle to reformat transform function popup menu.										
MenuHandle						gPopUpFunctionMenu = NULL;

		// Handle to mosaic direction popup menu.														
MenuHandle						gPopUpGraphAxisLabelMenu = NULL;

		// Handle to hard threshold popup menu. Used in Enhance Statistics.								
MenuHandle						gPopUpHardThresholdMenu = NULL;

		// Handle to symbols popup menu.														
MenuHandle						gPopUpHeaderOptionsMenu = NULL;

		// Handle to display type popup menu.												
MenuHandle						gPopUpHistogramMethodMenu = NULL;

		// Handle to horizontal datum popup menu.														
MenuHandle						gPopUpHorizontalDatumMenu = NULL;

		// Handle to image overlay popup menu used in dialog boxes.												
MenuHandle						gPopUpImageOverlayMenu = NULL;
	
		// Handle to reformat transform function popup menu.										
MenuHandle						gPopUpLinkOptionsMenu = NULL;

		// Handle to statistics type popup menu.												
MenuHandle						gPopUpListDataFormatMenu = NULL;

		// Handle to map units popup menu.														
MenuHandle						gPopUpMapUnitsMenu = NULL;

		// Handle to mosaic direction popup menu.														
MenuHandle						gPopUpMosaicDirectionMenu = NULL;

		// Handle to output file popup menu.												
MenuHandle						gPopUpOutputFileMenu = NULL;

		// Handle to palette popup menu.														
MenuHandle						gPopUpPaletteMenu = NULL;
	
		// Handle to display enhancement popup menu.										
MenuHandle						gPopUpPreprocessMenu = NULL;

		// Handle to project changes popup menu.											
MenuHandle						gPopUpProjectChangesMenu = NULL;

		// Handle to projection ellipsoid popup menu.														
MenuHandle						gPopUpEllipsoidMenu = NULL;

		// Handle to enhancement min-max popup menu.														
MenuHandle						gPopUpMinMaxMenu = NULL;

		// Handle to projection popup menu.														
MenuHandle						gPopUpProjectionMenu = NULL;

		// Handle to projection type popup menu.														
MenuHandle						gPopUpReferenceSystemMenu = NULL;

		// Handle for resample code popup menu.								
MenuHandle						gPopUpResampleMenu = NULL;

		// Handle to save cluster statistics popup menu.								
MenuHandle						gPopUpSaveStatsMenu = NULL;

		// Handle for selection display units popup menu.								
MenuHandle						gPopUpSelectionDisplayUnitsMenu = NULL;

		// Handle to soft threshold popup menu. Used in Enhance Statistics.								
MenuHandle						gPopUpSoftThresholdMenu = NULL;

		// Handle to select test mask image popup menu.												
MenuHandle						gPopUpSelectTestMaskImageMenu = NULL;

		// Handle to threshold image popup menu.														
MenuHandle						gPopUpSelectThresholdImageMenu = NULL;

		// Handle to select train mask image popup menu.												
MenuHandle						gPopUpSelectTrainMaskImageMenu = NULL;

		// Handle to separability distance popup menu.									
MenuHandle						gPopUpSeparabilityDistanceMenu = NULL;

		// Handle to statistics type popup menu.												
MenuHandle						gPopUpStatisticsTypeMenu = NULL;

		// Handle to enhancement stretch type popup menu.												
MenuHandle						gPopUpStretchMenu = NULL;

		// Handle to symbols popup menu.														
MenuHandle						gPopUpSymbolsMenu = NULL;

		// Handle to display type popup menu.												
MenuHandle						gPopUpTemporaryMenu = NULL;

		// Handle to enhancement treat 0's as popup menu.												
MenuHandle						gPopUpTreatZeroAsMenu = NULL;

		// Handle to weights popup menu.														
MenuHandle						gPopUpWeightsMenu = NULL;

		// menu handle array 																	
MenuHandle						gMultiSpecMenus[kNumMenuItems];

		// UniversalProcPtr for the common dialog filter routine.					
ModalFilterUPP					gDialogListFilterPtr = NULL;
ModalFilterUPP					gProcessorDialogFilterPtr = NULL;

		// Structure containing handles to specifications that may be used for	
		// non-project processor specifications.											
NonProjProcessorSpecs		gNonProjProcessorSpecs;

		// List of file types that MultiSpec is allowed to recognize.				
OSType							gListTypes[9];

		// Pattern to be used for oulining selected areas.								
Pattern							gOutlinePenPattern;
	
		// patterns to be used for pattern images											
Pattern							gPatterns[kNumPatterns];

		// Location of the upper left corner of the next graph window.				
Point								gNextGraphicsWindowStart;

		// Location of the last mouse location when doing dynamic
		// scrolling using the thumb control.
Point								gOldThumbScrollPoint;

		// The process serial number for the currently executing MultiSpec.
#if defined multispec_mac || defined multispec_mac_swift
	ProcessSerialNumber			gProcessSerialNumber;
#endif	// defined multispec_mac || defined multispec_mac_swift

		// pointer to project file information												
ProjectInfoPtr					gProjectInfoPtr = NULL;

		// Temporary memory pointers used to hold a location							
		// in memory for GWorld pointers.  These are used to try to not			
		// fragment the heap when using GWorld routines for offscreen pixmaps.	
Ptr								gGWorldPlaceHolderList[kMaxNumberIWindows];

		// structure to coordinates of drag gray rectangle 							
Rect								gGrayRect;

		// Rectangle for the graphics box in the graphics status dialog.			
Rect								gStatusGraphicsBox;

		// rectangle to be used for storage for temporary rectangles.				
Rect								gTempRect; 

		// rectangle to be used for the view area of a window.						
Rect								gViewRect;  

		// Pointer to the current reformat options structure
ReformatOptionsPtr			gReformatOptionsPtr = NULL; 

#if defined multispec_win
			// pen structure to be used for storing some standard color pens.
	CPen								gBlackPen;
	CPen								gBluePen;
	CPen								gCyanPen;
	CPen								gGreenPen;
	CPen								gMagentaPen;
	CPen								gRedPen;
	CPen								gWhitePen;
	CPen								gYellowPen;
	
	CRgn								gMFC_Rgn;
	
	COLORREF							gRGBColorList[8];
#endif	// defined multispec_win

#if defined multispec_lin
   // pen structure to be used for storing some standard color pens.
        // Later on also check whether to use default wxwidgets or not
	wxPen								gBlackPen;
	wxPen								gBluePen;
	wxPen								gCyanPen;
	wxPen								gGreenPen;
	wxPen								gMagentaPen;
	wxPen								gRedPen;
	wxPen								gWhitePen;
	wxPen								gYellowPen;

	wxRegion							gMFC_Rgn;
        // The following is used in windows to specify RGB.
        // TODO: Find suitable replacement for Linux
	//COLORREF							gRGBColorList[8];

#endif
RGBColor							gCoordinateViewBackgroundColor;

RGBColor							gCurrentSelectedColor;

RGBColor							gInactiveForeGroundColor;

RGBColor							gOverlayColorList[7];

		// Temporary region handle.  The region is allocated in 						
		// InitializeMultiSpec.c																
RgnHandle						gSelectionClipRgn = NULL;

		// Temporary region handle.  The region is allocated in 						
		// InitializeMultiSpec.c																
RgnHandle						gTempRegion1 = NULL;

		// Temporary region handle.  The region is allocated in 						
		// InitializeMultiSpec.c																
RgnHandle						gTempRegion2 = NULL;

		// Pointer to the statistics enhancement information structure.
StatEnhanceSpecsPtr			gStatEnhanceSpecsPtr = NULL;

			// Pointer to statistics histogram information.								
StatHistogramSpecsPtr		gStatHistogramSpecsPtr = NULL;

			// Pointer to the create statistics images information structure.		
StatisticsImageSpecsPtr		gStatisticsImageSpecsPtr = NULL;

		// Location to store the name of the file to be handled by MultiSpec.													
Str255							gAppFileName;

		// Buffer to be used to manipulate menu item strings.							
Str255							gMenuItemString;

		// buffer to be used to manipulate text strings									
Str255							gTextString;									
Str255							gTextString2;								
Str255							gTextString3;

		// Date version string
		// Application identifier string
//#if defined multispec_mac
	char								gDateVersionString[64];
	char								gApplicationIdentifierString[64];
//#else
//	wchar_t							gDateVersionString[16];
//	wchar_t							gApplicationIdentifierString[64];
//#endif

		// buffer to be used to manipulate text strings		
//TBYTE								gWideTextString[256];							
wchar_t							gWideTextString[256];									
wchar_t							gWideTextString2[256];								
wchar_t							gWideTextString3[256];


		// Structure containing information for a transformation matrix.
TransformationSpecs			gTransformationMatrix;

#if defined multispec_mac || defined multispec_mac_swift
	#if use_mlte_for_text_window
		TXNObject						gTextObject = NULL;
	#endif	// use_mlte_for_text_window
#endif	// multispec_mac || defined multispec_mac_swift

#if defined multispec_mac || defined multispec_mac_swift
	#if !TARGET_CPU_68K
				// Universal procedure pointer to the bridge routine to the word		
				// solution engine code resource.  This is only used for powerpc.		
		UniversalProcPtr				gWSEBridgePtr = NULL;
	#endif		// !TARGET_CPU_68K
#endif		// multispec_mac || defined multispec_mac_swift

		// UniversalProcPtr for dialog user item routines that stay loaded		
		// in memory.																				

UserItemUPP						gCreateOneColumnList1Ptr = NULL;
UserItemUPP						gCreateOneColumnList2Ptr = NULL;
UserItemUPP						gDrawBandInterleavePopUpPtr = NULL;
UserItemUPP						gDrawChannelsPopUpPtr = NULL;
UserItemUPP						gDrawDialogClassPopUpPtr = NULL;
UserItemUPP						gDrawGraphicsBoxPtr = NULL;
UserItemUPP						gDrawImageFilePopUpPtr = NULL;
UserItemUPP						gDrawSymbolsPopUpPtr = NULL;
UserItemUPP						gDrawWeightsPopUpPtr = NULL;
UserItemUPP						gHiliteOKButtonPtr = NULL;
UserItemUPP						gSetEntireImageIconPtr = NULL;

		// The image window information pointer.  If it is NULL,						
		// then an active image window does not exist or the 							
		// WindowInfoHandle is not locked.													
		// It is usually set up near the beginning of a processor function and	
		// "turned off" at the end of a function.											
WindowInfoPtr					gImageWindowInfoPtr = NULL;	

		// The image window information pointer.  If it is NULL,						
		// then an active image window does not exist or the 							
		// WindowInfoHandle is not locked.													
		// It is usually set up near the beginning of a processor function and	
		// "turned off" at the end of a function.			
		// This is usually used when also reading probability data along with
		// associated classified data.											
WindowInfoPtr					gImageWindowInfo2Ptr = NULL;

		// current active image window														
WindowPtr						gActiveImageWindow = NULL;  

#if defined multispec_win
		// current windows CDC pointer. This is only used by Windows.														
	CDC*								gCDCPointer = NULL; 
#endif

		// output grafPort window record pointer 											
WindowPtr						gOutputWindow = NULL;

		// window pointer to palette window													
WindowPtr						gPaletteWindow = NULL;

		// Image window associated with the palette window.							
WindowPtr						gPaletteImageWindow = NULL;

		// window pointer to project base window that was last used to make an	
		// area selection.																		
WindowPtr						gProjectSelectionWindow = NULL;

		// Pointer to the selection graph window.											
//WindowPtr						gSelectionGraphWindow = NULL;

		// window pointer to statistics window												
WindowPtr						gProjectWindow = NULL;

		// the active window																		
WindowPtr						gTheActiveWindow = NULL;

		// window list 																			
WindowPtr						gWindowList[
						kNumberBaseWindows+kMaxNumberIWindows+kMaxNumberGWindows];
						
#if defined multispec_mac || defined multispec_mac_swift
	#if !use_mlte_for_text_window
				// UniversalProcPtr for the WASTE line break procedure.	
		WELineBreakUPP					gWENoWrapLineBreakProcPtr = NULL;

				// UniversalProcPtr for the WASTE scroll action routines.			
		//WSEScrollActionUPP			gOutputAutoScrollActionPtr = NULL;			
		WEScrollUPP						gOutputAutoScrollActionPtr = NULL;
	#endif	// !use_mlte_for_text_window	
#endif 	// defined multispec_mac || defined multispec_mac_swift

		// handle to output text record 			
#if use_mlte_for_text_window
	TXNObject						gOutputTextH = NULL;
#endif
#if !use_mlte_for_text_window
	WEReference						gOutputTextH = NULL;
#endif														

		// Declare errno for math library 													
// int							errno; 

		// Pointer to character buffer 1.													
HPtr								gCharBufferPtr1 = NULL;									
HPtr								gCharBufferPtr2 = NULL;									
HPtr								gCharBufferPtr3 = NULL;

		// Pointer to the dialog description table.	
		// Allow for 3 layers of dialog boxes.									
UInt16							gDialogItemListTable[3*kDialogItemTableLength];

		// Pointer to the dialog description table.										
UInt16*							gDialogItemDescriptorPtr = NULL;
                      
   	// End of line character(s). This is initialized for Macintosh
		// end of line characters.
#if defined multispec_mac || defined multispec_mac_swift
	char								gEndOfLine[3] = {'\015', '\000', '\000'};
	wchar_t							gWideEndOfLine[3] = {'\015', '\000', '\000'};
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win     
	char								gEndOfLine[3] = {'\015', '\012', '\000'};
	wchar_t							gWideEndOfLine[3] = {'\015', '\012', '\000'};
#endif	// defined multispec_win  

#if defined multispec_lin     
	char								gEndOfLine[3] = {'\012', '\000', '\000'};
	wchar_t							gWideEndOfLine[3] = {'\012', '\000', '\000'};
#endif	// defined multispec_lin  

		// Define the decimal separator for numbers
char								gDecimalSeparator = '.';

		// Tab character.																			
char								gTextTab = '\t';

		// String of two carriage return characters.										
char								gTextCR[3];

		// Define the thousands separator for numbers
char								gThousandsSeparator = ',';

		// Pointer to temporary buffer to be used for computation of a 			
		// class sums of squares matrix.														
HDoublePtr						gTempSumsSquaresPtr = NULL;

		// Pointer to temporary buffer to be used for computation of a 			
		// covariance or correlation matrices.												
HDoublePtr						gTempMatrixPtr = NULL;

		// Factors to use to convert from sq. kilometers to sq. meters to sq. feet, etc.
double							gDistanceDisplayConversionFactors[12];

		// Factors to use to convert from kilometers to meters to feet, etc.
double							gDistanceFileConversionFactors[10];

		// Last coordinate line value.														
double							gDoubleCoordinateColumnValue = 0.0;

		// Last coordinate line value.														
double							gDoubleCoordinateLineValue = 0.0;

		// Maximum natural log value for double values;
double							gMaximumNaturalLogValue = 
											floor(DBL_MAX_10_EXP * (double)log((double)10));

		// Minimum natural log value for double values;
double							gMinimumNaturalLogValue = 
												ceil(DBL_MIN_10_EXP * (double)log((double)10));

		// Variable containing the minimum magnification that is allowed.			
double							gMinMagnification = .010101;

		// Variable that is used by a processor for temporary storage.				
double							gTempDoubleVariable1 = 0.;

		// Smallest log determinant that is allowed for a valid matrix 
		// inversion.	
double							gLowerDeterminantOffsetAllowed = -35.0;

		// Increment to be used when updating the graphics status box.				
double							gStatusBoxIncrement = 0.;

		// Right coordinates of the graphics status box which represents the		
		// portion of the work that has been completed.									
double							gStatusGraphicsRight = 0.;

//int								szip_allow_encoding = 0;		// Added 10/23/2007
            
#if defined multispec_win                              						
	int							gCrossCursorID = IDC_CROSS_CURSOR;
#endif	// defined multispec_win
   
#if defined multispec_lin                              						
	int							gCrossCursorID = IDC_CROSS_CURSOR;
#endif	// defined multispec_lin

		// Variables used when converting between line-column units and window	
		// units.																					
SInt32							gChannelWindowInterval = 1;
SInt32							gChannelWindowOffset = 0;
SInt32							gChannelWindowWidth = 0;
SInt32							gSideBySideChannels = 1;
SInt32							gStartChannel = 1;
SInt32							gStartChannelWindow = 1;

		// Last coordinate line value.														
SInt32							gCoordinateColumnValue = 1;

		// Last coordinate line value.														
SInt32							gCoordinateLineValue = 1;

		// Holds the 4-letter code indicating that MultiSpec is the creator = 	
		// 'MLAR'.																					
SInt32							gCreator = 0;

			// Variable that contains the maximum number of characters that		
			// are allowed in one line of text data.  This is used by				
			// 'ListString'. 																		
SInt32							gMaxCharsAllowedInLine = SInt16_MAX - 3;

			// Variable that contains the number of characters in the longest		
			// line of output text. 															
SInt32							gMaxCharsInLine = 1;

		// Maximum size of block of memory that can be moved.  The Mac OS 		
		// as a bug such that if a block of memory more than 9.5 meg or so		
		// is move then heap is corrupted.													
SInt32							gMaxMoveableBlockSize = 8000000;

		// Minimum memory needed for drawing a dialog or alert box.					
SInt32							gMinMemoryForDrawingDialog = 4000;

		// Number of open files.					
SInt32							gNumberOfOpenFiles = 0;

		// The limit for the number of open files.
		// The limit will be set to a high value to start with in case we
		// are not able to determine what the limit is.					
SInt32							gNumberOfOpenFilesLimit = 32000;

		// Minimum time offset allowed between scroll bar and zoom control		
		// operations. Units are in ticks or 1/60's of a second.
#if defined multispec_mac || defined multispec_mac_swift
	UInt32							gControlOffset = 8;
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win                                       												
	UInt32							gControlOffset = 150;
#endif	// defined multispec_win	

#if defined multispec_wxlin
	UInt32							gControlOffset = CLOCKS_PER_SEC/4;
#endif	// defined multispec_wxlin

#if defined multispec_wxmac
			// Looks like ticks are 1000 per second
	UInt32							gControlOffset = 250;
#endif	// defined multispec_wxmac

			// Variable for storage of the current number of selections in
			// the open image list. Will be used for knowing when to allow
			// for vector files in this list. They are only allowed for 
			// single selection lists.
SInt32							gOpenImageSelectionListCount = 0;

			// Version of quickdraw that is available.									
SInt32							gQuickDrawVersion;

		// Offset to use for output selection buffer.									
SInt32							gSelectionOutputBufOffset = 0;

		// Use for determining when to check for command-. for stopping.							
		// Units are in ticks or 1/60's of a second.					
SInt32							gTimeOffset = kFrontTimeOffset;   

		// Maximum number of channels combinations to be considered in				
		// the Separability processor.														
UInt32							gMaxNumberChannelCombinations = UInt32_MAX;

		// Maximum number of row bytes possible.  For non-32 bit QD systems, it	
		// is 0x2000.  For 32bit QD system it is 0x4000; actually it could be	
		// 0x8000 if the origin was change each time for the offscreen bit maps. 
UInt32							gMaxRowBytes = kMaxRowBytes;   
                      
#if defined multispec_win || defined multispec_lin 
			// Maximum number of row bytes possible for 24 bit offscreen maps.
			// This variable is only used for the Windows system.  
	UInt32							gMaxRowBytesFor24Bits = kMaxRowBytes * 3; 
#endif	// defined multispec_win || multispec_lin									

		// Uses for storage of next time to list minutes left information.
UInt32							gNextMinutesLeftTime = 0;

		// Used for determining when to list the next minutes
		// left message. Units are in ticks or 1/60's of a second
		// for Mac OS and milliseconds for Windows OS and microseconds
		// for linux		
									
#if defined multispec_mac || defined multispec_mac_swift
	UInt32							gNextMinutesLeftTimeOffset = 180;
#endif	// defined multispec_mac || defined multispec_mac_swift
                                
#if defined multispec_win                                    
	UInt32							gNextMinutesLeftTimeOffset = 3000;
#endif	// defined multispec_win									

#if defined multispec_lin                                    
	UInt32							gNextMinutesLeftTimeOffset = 3000;
#endif	// defined multispec_lin		
		// Uses for storage of next time to list status information.				
UInt32							gNextStatusTime = 0;

		// Used for determining when to list status information.			
		// operations. Units are in ticks or 1/60's of a second
		// for Mac OS and milliseconds for Windows OS and microseconds
		// for linux.
#if defined multispec_mac || defined multispec_mac_swift
	UInt32							gNextStatusTimeOffset = 20;
#endif	// defined multispec_mac || defined multispec_mac_swift
                                
#if defined multispec_win                                    
	UInt32							gNextStatusTimeOffset = 330;
#endif	// defined multispec_win									   
   
#if defined multispec_lin                                    
	UInt32							gNextStatusTimeOffset = 333;
#endif	// defined multispec_lin									   

		// Uses for storage of next time to check for command-. for stopping.	
#if defined multispec_mac || defined multispec_mac_swift
	UInt32							gNextTime = 20;
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	UInt32							gNextTime = 330;
#endif	// defined multispec_win

#if defined multispec_lin
	time_t							gNextTime = 333;
#endif	// defined multispec_lin

		// Count of the number of shape files that have been loaded in.
UInt32							gNumberImageOverlayFiles = 0;

		// Count of the number of shape files that have been loaded in.
UInt32							gNumberShapeFiles = 0;

		// Count of the length of the overlay image handle list.
UInt32							gImageOverlayHandleListLength = 0;

		// Count of the length of the shape file handle list.
UInt32							gShapeHandleListLength = 0;

			// Amount of spare memory to keep stored in a handle to be used to	
			// load code resources in when main memory runs out.						
UInt32							gSpareCodeSize = 44000;

			// Amount of spare memory to keep stored in a handle to be used to	
			// load alert resources in when main memory runs out.						
UInt32							gSpareWarnSize = 25000;

		// Variable indicating the item number in the status dialog box that		
		// is to be used for the title.														
UInt32							gStatusIDNumber = 0;

		// Flag indicating the last memory condition for DataPak's word			
		// solution engine.																		
UInt32							gTextMemoryMinimum= 12000;

			// Variable containing the time to yield to background applications.	
UInt32							gYieldTime = kFrontYieldTime;

		// Variable containing the legend width for the active window.				
SInt16							gActiveLegendWidth = 0;

		// Variable indicating the type of the active window.							
SInt16							gActiveWindowType = 0;

		// ID of alert to be used when the user cancels a process.					
SInt16							gAlertId = 0;

		// Return code from alert used when the user cancels a process.			
SInt16							gAlertReturnCode = 0;

		// ID of string to use in the alert when the user cancels a process.		
SInt16							gAlertStrID = 0;

		// String number to use in the alert when the user cancels a process.	
SInt16							gAlertStringNumber = 0;

		// Variable indicating what the current band interleave selection is.	
SInt16							gBandInterleaveSelection = 0;

		// Identifies the routine to use for blinking classes/groups in			
		// Thematic type images.																
SInt16							gBlinkProcedure = kBlink1;

		// Variable indicating what the current bits per data value selection 	
		// is.																						
SInt16							gBitsPerDataValueSelection = 0;

		// Variable indicating what the current bytes per data value selection 	
		// is.																						
// SInt16							gBytesPerDataValueSelection = 0;

		// Variable indicating what the current channel description selection 	
		// is.																						
SInt16							gChanDescriptionSelection = 0;

		// Variable indicating what the current channel selection type is.		
		// It is used by processor dialogs.  =1 is all channels, =2 is subset.	
SInt16							gChannelSelection = kAllMenuItem;

		// variable indicating which area units menu item is currently checked.
SInt16							gCheckedAreaUnitsMenuItem = 0;

		// variable indicating which window menu item is currently checked.
SInt16							gCheckedWindowMenuItem = 0;

		// Variable indicating the limit of the number of classes for which		
		// a class list or legend will be created.										
UInt16							gClassListLimit = 2000;

		// Variable indicating what the current class area selection type is.	
		// It is used by processor dialogs.  =1 is all classes, =2 is subset.	
SInt16							gClassAreaSelection = kAllMenuItem;

		// Number of entries in the palette that are uses for displaying 			
		// classes or groups in thematic type images.									
SInt16							gClassPaletteEntries = 0;

		// Variable indicating what the current class selection type is.			
		// It is used by processor dialogs.  =1 is all classes, =2 is subset.	
SInt16							gClassSelection = kAllMenuItem;

		// Variable indicating what the current class or group selection is.		
		// it is used by processor dialogs.  =1 is classes; =2 is information	
		// groups.																					
SInt16							gClassGroupSelection = kAllMenuItem;

		// Variable indicating what the current classify procedure is.				
		// It is used by classify dialog.  =1 is maximum likelihood, 				
		// =2 is fisher, =3 is L2 minimum distance, =4 is ECHO.		
SInt16							gClassificationProcedure = 0;

		// Variable indicating what the current data value type selection 	
		// is.																						
SInt16							gDataValueTypeSelection = 0;

		// Default units to be used for areas.
SInt16							gDefaultAreaUnits = kHectareUnitsMenuItem;

		// Variable containing the working default legend width.
#if defined multispec_mac || defined multispec_mac_swift
	SInt16						gDefaultLegendWidth = 136; 
#endif	// defined multispec_mac || defined multispec_mac_swift
						
#if defined multispec_win		                               	
	SInt16						gDefaultLegendWidth = 170;
#endif	// defined multispec_win									
   
#if defined multispec_lin		                               	
	SInt16						gDefaultLegendWidth = 170;
#endif	// defined multispec_lin									

		// Variable containing the working default titleHeight.
#if defined multispec_mac || defined multispec_mac_swift
	SInt16						gDefaultTitleHeight = 15;
#endif	// defined multispec_mac || defined multispec_mac_swift
						
#if defined multispec_win		
	SInt16						gDefaultTitleHeight = 15;
#endif	// defined multispec_win										
   
#if defined multispec_lin		
	SInt16						gDefaultTitleHeight = 15;
#endif	// defined multispec_lin										

		// Variable containing the default palette to use for the TRL file		
		// associated with new classification images.									
SInt16							gDefaultImagePalette = 1001;

		// Variable containing the default palette to use for the TRL file		
		// associated with new probability image.											
SInt16							gDefaultProbabilityPalette = 1005;

		// Indicates which level of the dialog description table to use			
		// if the dialog filter is called. -1 indicates that no set is to be		
		// used.																				
SInt16							gDialogItemTableSet = -1;
            
#if defined multispec_win 
 			// Indicates what the bits per pixel is for the display. This is
 			// used in the windows version only.
	UInt16							gDisplayBitsPerPixel = 8; 
#endif	// defined multispec_win

#if defined multispec_lin 
 			// Indicates what the bits per pixel is for the display. This is
 			// Defining for linux for now. See later if necessary
	UInt16							gDisplayBitsPerPixel = 8; 
#endif	// defined multispec_lin
   
		// Variable containing the time interval to use to copy portions
		// of an offscreen bit/pix map to the screen as the offscreen map
		// is being loaded. The units are ticks (1/60 second)
SInt16							gDisplayIntervalTime = 120;

#if defined multispec_win || defined multispec_lin
			// File filter index to use in the windows version for opening
			// files.
	SInt16							gFileFilterIndex = 1;
#endif	// defined multispec_win | defined multispec_lin
                   
		// File filter index to use for opening
		// image or shape files.
#if defined multispec_win || defined multispec_lin			
	SInt16							gImageFileFilterIndex = 2;
#else
	SInt16							gImageFileFilterIndex = 0;
#endif	// defined multispec_win, else...

		// Variable indicating the file name length limit.
UInt16							gFileNameLengthLimit = 254;

		// Variable indicating what the current file name selection is.			
SInt16							gFileNamesSelection = 0;

		// Running total of the number of graphics windows that have been 		
		// created.																					
SInt16							gGraphicsWindowCount = 0;

		// Location offset for the next graphics windows.								
SInt16							gGraphicsWindowOffset = 10;

		// Variable containing the minimum grow area for a window.					
SInt16							gGrowAreaMinimum = 150;

		// Variable containing the type of image window that was last selected.	
		// Thematic type or Multispectral type.											
SInt16							gGetFileImageType = 0;

		// Variable containing the status of the file hook routine for GetFile.	
SInt16							gGetFileStatus;

		// Variable which contains the number of tries to get more memory to		
		// satisfy a memory request for code that is used by "MyGrowZone".		
SInt16							gGetMemoryTries = 0;

		// Variable indicating what the current header options selection type 	
		// is. It is used by processor dialogs.  =1 is no header,					
		// =2 is 73 Erdas header, =3 is 74 Erdas header.								
SInt16							gHeaderOptionsSelection = 0;

		// Variable indicating which method should be used to obtain the			
		// histogram information to be used.												
SInt16							gHistogramMethod = 0;

		// Variable indicating what the current interclass weight selection 		
		// type is.  It is used by processor dialogs.  =1 is equal weights, 		
		// =2 is unequal.																			
SInt16							gInterClassWeightsSelection = 0;

		// Variable indicating what the last hdf data set selection was.  This
		// is used to help when users are making repeated accesses to similar
		// hdf files and want the same data set each time.  It is used with
		// gLastHDFNumberDataSets
SInt16							gLastHDFDataSetSelection = 0;

		// Variable indicating the number of hdf data sets in the last hdf file
		// that was selected.  This is used to help when users are making repeated 
		// accesses to similar hdf files and want the same data set each time. It 
		// is used with gLastHDFDataSetSelection
SInt16							gLastHDFNumberDataSets = 0;

		// Storage for the last scrap count that was loaded into the TEScrap.	
SInt16							gLastScrapCount = 0;

		// Variable indicating the current selection in the link options popup menu.
SInt16							gLinkOptionsSelection = 1;

		// Variable containing the maximum magnification that is allowed.			
UInt16							gMaxMagnification = 99;

		// Maximum print page width.  Defined by QMS ColorScript 100.				
SInt16							gMaxPrintPageWidth = 0;

		// Maximum print page height.  Defined by QMS ColorScript 100.				
SInt16							gMaxPrintPageHeight = 0;

		// Parameter that when set to one indicates that not enough memory was	
		// available for a request.  It is set in "MyGrowZone" and is used in	
		// various loops in MultiSpec to allow one to exit from the loop.			
SInt16							gMemoryError = noErr;

			// Variable that indicates which type if is needed. This variable is	
			// used in the MyGrowZone routine.  =1 indicates Code resources; =2	
			// indicates non-Code resources and data, =3 indicates for spare		
			// memory.																				
SInt16							gMemoryTypeNeeded = 0;

		// Code indicating whether the selected image is: 
		//  = 0: not applicable
		//  = 1: the first file for a multiple linked set of image files
		//  = 2: to be added the active image window "file"
		//  = 3: multiple image selected to be linked together							 		
SInt16							gMultipleImageFileCode = 0;

		// Number of available "place holders" for GWorld pointers.					
SInt16							gNumberAvailableGWorldPtrs = 0;

		// Number of available window handles. 											
SInt16							gNumberAvailableImageWindowHandles = 0;
	
		// Number of file types in the 'gListTypes' array that MultiSpec			
		// can recognize.																			
SInt16							gNumberFileTypes = 8;

		// Number of End of Line characters for OS.  			
#if defined multispec_mac || defined multispec_mac_swift
	SInt16							gNumberOfEndOfLineCharacters = 1;
#endif	// defined multispec_mac || defined multispec_mac_swift
                                
#if defined multispec_win                              						
	SInt16							gNumberOfEndOfLineCharacters = 2;
#endif	// defined multispec_win
   
#if defined multispec_lin
   // Number of End of Line characters for OS. (Linux only has Line Feed)
	SInt16				gNumberOfEndOfLineCharacters = 1;
#endif

			// Current number of open graphics windows. 									
SInt16							gNumberOfGWindows = 0;

			// Current number of open image windows. 										
SInt16							gNumberOfIWindows = 0;

		// Variable containing the number of probability classes.					
UInt16							gNumberProbabilityClasses = 121;

		// variable indicating which window menu item is currently checked.
SInt16							gOutlinedWindowMenuItem = 0;

		// Variable indicating what the current output file selection is.			
SInt16							gOutputFileSelection = 0;

		// Variable containing the factor to use in computing the horizontal		
		// scroll bar control values for the output text window.						
SInt16							gOutputHScrollFactor = 1;

		// Variable containing the line height for the output text window.		
SInt16							gOutputLineHeight = 0;

		// Variable containing the factor to use in computing the control			
		// values for the output text window.												
SInt16							gOutputVScrollFactor = 1;

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
SInt16							gOutputCode = 0;

		// Variable indicating what the current data output request is with		
		// the text output window being forced.  (See gOutputCode).					
SInt16							gOutputForce1Code = 0x0001;

		// Variable indicating what gOutputForce1Code will be set to. This is
		// being used to to turn off text output list for cases such as when
		// one is using the arrow keys to display the next or previous channel.
		// The output text window is not updated. This saves on time in updating
		// the output text window.					
SInt16							gOutputForce1CodeSetting = 0x0001;

		// Variable indicating what the current data output format request is	
		// for the results disk file.															
		// 1 = kErdas73Type																		
		// 2 = kMultiSpecType (not used)														
		// 3 = kAVIRISType (not used)															
		//	etc.																						 
SInt16							gOutputFormatCode = 0;

		// Palette offset to use for displaying thematic type image legends.		
SInt16							gPaletteOffset = 0;
      
      // Variable indicating the current selection of the palette menu
      // in the Display Thematic Dialog box.
SInt16							gPaletteSelection = 0;

		// Variable indicating what the current principal component channel 		
		// selection type is.	It is used by processor dialogs.  =1 is all 		
		// PC channels, =2 is subset.															
SInt16							gPCChannelSelection = 0;

		// Variable indicating which preprocess menu selection is the current
		// selection.
SInt16							gPreprocessSelection = 0;

			// Index of present cursor																
SInt16							gPresentCursor = kArrow;

		// Code indicating which processor is currently being used.					
		//		=0 none; =10 Display; =20 Histogram; =30 Reformat; =40 Cluster;	
		//		=50 Statistics; =60 Separability; =70 Classify;							
		//		=80 Display Results; =1010 Check Covariances; =1030 List Results;	
		//		=1040 Principal Components.													
SInt16							gProcessorCode = 0;

SInt16                     gListDataCode = 0;

		// Variable indicating what the current save cluster statistics			
		// selection type is. It is used by processor dialogs.  =1 do not save, 
		// =2 save to new project, =3 save to current project, .						
SInt16							gSaveStatisticsSelection = 0;

		// Variable indicating the currection selection in the select image
		// overlay popup menu that is used in dialog boxes. The first one in the
		// list is to create a new overlay.
SInt16							gSelectImageOverlaySelection = 0;

		// Variable indicating what the current separability distance measure 	
		// is.  =1 is transformed divergence, =2 is divergence, =3 is 				
		// bhattacharyya, =4 is 'noncovariance bhattacharyya, =5 is error			
		// function bhattacharyya.																
SInt16							gSeparabilityDistance = 0;

		// Index used with spinning cursor
SInt16							gSpinCursorIndex = kSpin;

			// Variable which mode the statistics window is currently in.			
SInt16							gStatisticsMode = 0;

		// Variable indicating the item number in the status dialog box that		
		// is to be used for the title.														
SInt16							gStatusTitleItemNumber = 0;

		// Variable indicating what the current symbol selection type is.			
		// It is used by processor dialogs.  =1 is default symbols,					
		// =2 is user symbols.																	
SInt16							gSymbolSelection = 0;

		// Code for macintosh system architecture
		//		1 = 68K
		//		2 = PowerPC
		//		3 = Intel
UInt16							gSystemArchitectureCode = 0;

			// Code to indicate that memory for text window operation is short.	
SInt16							gTextMemoryShortCode = 0;

		// Variable containing the threshold value for which any rectangle 		
		// selection in an image must be larger than to be considered a 			
		// selected ares.  This will allow a user to click in an image window 	
		// to select the window or to zoom in at that point and not consider 	
		// it a rectangle selection.															
SInt16							gThresholdSize = 2;

			// Used in 'DragScrollList' routine for scrolling lists.
SInt16							gVerticalCellOffset = 0;
SInt16							gSelectedCell = 0;

			// Variable used as storage for number of viewable text lines in the	
			// text window.																		
SInt16							gViewLines = 0;

			// Variable used as storage for number of viewable pixels in a line	
			// of characters in the text window.											
SInt16							gViewHPixels = 0;

		// Variable indicating what the current weight selection type is.			
		// It is used by processor dialogs.  =1 is equal weights, 					
		// =2 is unequal.																			
SInt16							gWeightsSelection = 0;

			// Font number to be used in the windows.										
SInt16							gWindowTextFont = 0;

#if defined multispec_lin 
			// Disable other windows when executing list Data graph window
	wxWindowDisabler							*m_windowDisabler;
#endif

			// Structure contain information used for workflow processing. Currently
			// this is for testing and for a very specific project.
WorkFlowInfo					gMultiSpecWorkflowInfo;



#if defined multispec_mac
			// Parameter relative to carbon applications.
			
	ListDefSpec								gListDefinitionSpec;
	
	CFBundleRef 							gApplicationServicesBundle = NULL;
	
	CGColorSpaceCreateDeviceGrayPtr	gCGColorSpaceCreateDeviceGrayPtr = NULL;
	
	CGColorSpaceCreateDeviceRGBPtr	gCGColorSpaceCreateDeviceRGBPtr = NULL;
	
	CGColorSpaceCreateIndexedPtr		gCGColorSpaceCreateIndexedPtr = NULL;
	
	CGColorSpaceReleasePtr				gCGColorSpaceReleasePtr = NULL;
	
	CGContextAddLineToPointPtr			gCGContextAddLineToPointPtr = NULL;
	
	CGContextBeginPathPtr				gCGContextBeginPathPtr = NULL;
	
	CGContextClipToRectPtr				gCGContextClipToRectPtr = NULL;
	
	CGContextDrawImagePtr				gCGContextDrawImagePtr = NULL;
	
	CGContextFlushPtr						gCGContextFlushPtr = NULL;
	
	CGContextMoveToPointPtr				gCGContextMoveToPointPtr = NULL;
	
	CGContextReleasePtr					gCGContextReleasePtr = NULL;
	
	CGContextRestoreGStatePtr			gCGContextRestoreGStatePtr = NULL;
	
	CGContextSaveGStatePtr				gCGContextSaveGStatePtr = NULL;
	
	CGContextScaleCTMPtr					gCGContextScaleCTMPtr = NULL;
	
	CGContextSetAlphaPtr					gCGContextSetAlphaPtr = NULL;
	
	CGContextSetInterpolationQualityPtr		gCGContextSetInterpolationQualityPtr = NULL;
	
	CGContextSetLineWidthPtr			gCGContextSetLineWidthPtr = NULL;
	
	CGContextSetRGBFillColorPtr		gCGContextSetRGBFillColorPtr = NULL;
	
	CGContextSetShouldAntialiasPtr	gCGContextSetShouldAntialiasPtr = NULL;
	
	CGContextStrokePathPtr				gCGContextStrokePathPtr = NULL;
	
	CGContextTranslateCTMPtr			gCGContextTranslateCTMPtr = NULL;
	
	//#if !TARGET_RT_MAC_MACHO	
		CGDataProviderCreateWithDataPtr	gCGDataProviderCreateWithDataPtr = NULL;
	//#endif
	
	CGDataProviderReleasePtr			gCGDataProviderReleasePtr = NULL;
	
	CGImageCreatePtr						gCGImageCreatePtr = NULL;
	
	CGImageGetBytesPerRowPtr			gCGImageGetBytesPerRowPtr = NULL;
	
	CGImageGetHeightPtr					gCGImageGetHeightPtr = NULL;
	
	CGImageGetWidthPtr					gCGImageGetWidthPtr = NULL;
	
	CGImageMaskCreatePtr					gCGImageMaskCreatePtr = NULL;
	
	CGImageReleasePtr						gCGImageReleasePtr = NULL;
	
	CGRectMakePtr							gCGRectMakePtr = NULL;
	
	QDBeginCGContextPtr					gQDBeginCGContextPtr = NULL;
	
	QDEndCGContextPtr						gQDEndCGContextPtr = NULL;
#endif	// defined multispec_mac
#endif	// ifndef __SDECGLOBAL__
