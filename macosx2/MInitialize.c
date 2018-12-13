//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						MInitialize.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			08/16/2018
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The following program initializes MultiSpec at startup.
//								This will be the only procedure in this file so that
//								it can be in a separate code resource that can be 
//								unloaded after startup.
//
//	Functions in file:	WindowPtr 			CreateOutputWindow
//								Boolean 				CreateOutputWControls
//								void					InitializeAppleEventParameters
//								void 					InitializeDoubleVariables
//								void 					InitializeMultiSpec
//								Boolean				InitializeTextEngine
//								void	 				InitPrinter
//								void					MakeMonaco
//
//	Diagram of MultiSpec procedure calls for the procedures in the file.
//
//		InitializeMultiSpec
//			CreateOutputWindow (in windows.c)
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

//#define _DARWIN_UNLIMITED_STREAMS
//#include <stdio.h>

#define kNavServicesOpenList			3001
#define kNavServicesCustomPut			3002

extern Handle 			gCustomNavOpenList;
extern Handle 			gCustomNavPut;

#if use_mlte_for_text_window
	extern pascal OSStatus	TextWindowFocusAcquired (
					EventHandlerCallRef				nextHandler,
					EventRef								theEvent, 
					void*									userData);		
	/*
	extern pascal OSStatus TextWindowContentClicked (
					EventHandlerCallRef				nextHandler,
					EventRef								theEvent,
					void*									userData);
	*/
	extern OSStatus SetOutputWindowFont (
					TXNObject							textObject);
	/*
	extern OSStatus TXNSetReadOnly (
					TXNObject							inTextObject,
					Boolean								inIsReadOnly);
	*/
#endif	// use_mlte_for_text_window		
#if !use_mlte_for_text_window											
	extern pascal StyledLineBreakCode _WENoWrapLineBreak (
					const char *						inTextPtr,
					SInt32								inTextLength,
					SInt32								inTextStart,
					SInt32								inTextEnd,
					Fixed *								ioTextWidth,
					SInt32 *								ioTextOffset,
					WEReference							inWE);
#endif	// !use_mlte_for_text_window

extern pascal Boolean LegendCClickLoop (void);
											
extern pascal OSErr HandleAEOpenApplication (
				const AppleEvent*					appleEventPtr,
				AEDescList*							replyPtr, 
				SInt32	 							handlerRefcon);

extern pascal OSErr HandleAEOpenDocument (
				const AppleEvent*					appleEventPtr,
				AEDescList*							replyPtr, 
				SInt32								handlerRefcon);

extern pascal OSErr HandleAEPrintDocument (
				const AppleEvent*					appleEventPtr,
				AEDescList*							replyEventPtr, 
				SInt32	 							handlerRefcon);
					
extern pascal OSErr HandleAEQuitApplication (
				const AppleEvent*					appleEventPtr,
				AEDescList*							replyPtr, 
				SInt32				 				handlerRefcon);

extern Boolean InitializeTextEngine (void);
						

extern void WriteInterfacePreferenceValue (
				Boolean								fullVersionFlag);
										
										

//#if GENERATINGCFM
#if TARGET_RT_MAC_CFM || TARGET_RT_MAC_MACHO
	extern pascal void 				FileIOCompletionRoutine (ParmBlkPtr pbPtr);
#else
	extern pascal void 				FileIOCompletionRoutine (ParmBlkPtr pbPtr : __A0);
#endif

		// For Thumb based action control. Not in Control header. This is from
		// one of the news groups.
		
enum {
	uppThumbActionProcInfo = kPascalStackBased
};

//#if GENERATINGCFM
#if TARGET_RT_MAC_CFM
	typedef UniversalProcPtr ThumbActionProcUPP;
	#define NewThumbActionProc (userRoutine) \
								(ThumbActionProcUPP) \
	NewRoutineDescriptor ((ProcPtr)(userRoutine), uppThumbActionProcInfo, GetCurrentArchitecture ())
#else
	typedef ControlActionProcPtr ThumbActionProcUPP;
	#define NewThumbActionProc (userRoutine) \
								((ThumbActionProcUPP) (userRoutine))
#endif



//enum {
//	kUnresolvedCFragSymbolAddress = 0
//};
enum {
	kUnresolvedSymbolAddress	= kUnresolvedCFragSymbolAddress
};

//#if PRAGMA_ALIGN_SUPPORTED
#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#endif
												
typedef struct
	{
	short int		jmpInstr;
	ListDefUPP		jmpAddr;		// Ptr
	
	}		JmpRec, *JmpPtr, **JmpHandle;
												
typedef struct
	{
	short int			jmpInstr;
	ControlDefUPP		jmpAddr;		// Ptr
	
	}		JmpControlRec, *JmpControlPtr, **JmpControlHandle;

//#if PRAGMA_ALIGN_SUPPORTED
#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#endif
	
// #define  _useMalloc_ 



		// Prototypes for routines in this file that are only called by
		// other routines in this file.													

WindowPtr CreateOutputWindow (Boolean*);

Boolean	InitializeAppleEventParameters (void);
	
void		InitializeDoubleVariables (void);

Boolean 	IsAppearanceManagerPresent (
				Boolean								gestaltAvailableFlag);

Boolean 	LoadCoreGraphicsVariables (void);

//void	MakeMonaco (FormatPtr);

short int NumToolboxTraps (void);

Boolean	TrapAvailable (short int);

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeMultiSpec ()
//
//	Software purpose:	The purpose of this procedure is to initialize
//							MultiSpec at application startup.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/26/1988
//	Revised By:			Larry L. Biehl			Date: 11/27/2017

void InitializeMultiSpec (void)

{
			// Declare local variables	
	
	#ifndef _Gestalt
		#define								_Gestalt		0xa1ad
	#endif
	
	Point									where;
	
	DialogPtr							dialogPtr;
	GrafPtr								savedPort;
	GrowZoneUPP							myGrowZonePtr;
	//WindowInfoPtr						windowInfoPtr;
	
	CursHandle							cursorHndl;
	
	//Handle								dialogHandle;
	//										handle;

	ResType								resType = 'DLOG';
	
	SInt32								feature,
											gestaltResponse,
											osVersion = 0;
												
	SInt16								count,
											errCode = noErr,
											returnCode;
	
	Boolean								longRowBytesFlag,
											returnFlag;
	/*
	char									cmdChar = 0x1b,
											markCharColor = 0x8A,
											markCharUtilities = 0x8C,
											markCharOpenImage = 0x8D;
	*/
	
			// Dispose of the temporary memory.
			
	gTempMatrixPtr = (double*)CheckAndDisposePtr ((Ptr)gTempMatrixPtr);
	
	gSpareCodeMemoryH = UnlockAndDispose (gSpareCodeMemoryH);
	
	gCreator = 'MLAR';
	gQD32IsImplemented = FALSE;
	gQuickDrawVersion = 0;
	gHasFSSpecCalls = FALSE;
	gSystemArchitectureCode = 1;
	
			// Initialize global memory variables.											
			
	gMaxMoveableBlockSize = 8000000;
	gMinMemoryForDrawingDialog = 4000;

	#ifdef _useMalloc_
				// Now force the nonrelocatable code used by malloc to be 			
				// created.	 Otherwise if it is done later by fopen when a 			
				// project is written or read, the application heap will become 	
				// fragremented.  This uses about 12K bytes of space.  				
				
		gTempMatrixPtr = (double*)malloc ((size_t)4);
		
		if (gTempMatrixPtr != NULL)
			free (gTempMatrixPtr);
		
		else	// gTempMatrixPtr == NULL 
																							ExitToShell ();
			
		gTempMatrixPtr = NULL;
	#endif	// end "_useMalloc_"
	/*
	#if !TARGET_API_MAC_CARBON
				// Initialize the graph port														
					
		InitGraf (&qd.thePort);
		
				// Initialize the window manager;												
				
		InitWindows ();

				// Initialize the font manager													
				
		InitFonts ();
		
				// Set up the event manager mask (you should accept responsibility	 
				// for all events to insure that the event queue is flushed; 			
				// some calls such as MENUSELECT will not work properly if there are  
				// extra mouse up events lying around):										

		FlushEvents (everyEvent, 0);

				// Initialize the menu manager;													
				
		InitMenus ();

				// Do text edit initialization here												
				
		TEInit ();
		
				// Initialize the dialog manager													
				
		InitDialogs (NIL);
	
				// Initialize palette manager for Macintosh II								

		InitPalettes ();
	#endif	// !TARGET_API_MAC_CARBON
	*/
			// Initialize the cursor															
			
	MInitCursor ();
	/*
	#if ! TARGET_API_MAC_CARBON
				//	register this application with the TSM
				//	this is not needed for Carbon clients
		if (InitTSMAwareApplication () != noErr)
																							ExitToShell ();
	#endif // ! TARGET_API_MAC_CARBON
	
	#if !TARGET_API_MAC_CARBON	
				// Get a handle to a print record for image and graph windows.												
			
		gHPrint = InitPrinter ();
		if (gHPrint == NULL)
																							ExitToShell ();
	#endif / !TARGET_API_MAC_CARBON
	*/
			// Draw about MultiSpec dialog box												
			// First check whether it is in the open resource files.  If not		
			// then this indicates that the application resource file is not		
			// available. If not available, exit the application.						
	
	if (GetResource (resType, kAboutMultiSpecID) == NULL)		
																							ExitToShell ();
	/*
	#if !TARGET_API_MAC_CARBON		
				// Make certain that a minimum amount of memory space is available.	
				
		SInt32		contiguousSpace,
						totalSpace;
		
		returnFlag = FALSE;
		if ((SInt32)GetApplLimit () - (SInt32)ApplicationZone () < minimumHeapSize)
			returnFlag = TRUE;
			
		PurgeSpace (&totalSpace, &contiguousSpace);
		if (totalSpace < minimumSpaceSize)
			returnFlag = TRUE;
			
		if (returnFlag)
			{
			returnCode = StopAlert (1155, NIL);
																							ExitToShell ();
																					
			}	// end "if (returnFlag)"
	#endif	// !TARGET_API_MAC_CARBON
	*/
			// The next bit of code will force the splash screen to be in the		
			// front most layer if the user is working under Multifinder.			
			
	for (count=1; count<=3; count++)
		EventAvail (everyEvent, &gEventRecord);									
	
			// Determine whether Gestalt is available.	
	
	//#if TARGET_API_MAC_CARBON
		gGestaltAvailableFlag = TRUE;
	//#else
	//	gGestaltAvailableFlag = TrapAvailable (_Gestalt);
	//#endif	// TARGET_API_MAC_CARBON else...

	if (gGestaltAvailableFlag)
		{
				// Check for Navigation Services
				// Do this here so the memory does not become fragmented.
			
		gNavServicesExistsFlag = FALSE;
		
		#if !TARGET_CPU_68K				 
			gNavServicesExistsFlag = NavServicesAvailable ();
			//gNavServicesExistsFlag = FALSE;
			if (gNavServicesExistsFlag)
				NavLoad ();
		#endif	// !TARGET_CPU_68K
		
		}	// end "if (gGestaltAvailableFlag)"
		
			// Put up the about MultiSpec dialog window. Center it.					
			
	GetDlogOrigin (kAboutMultiSpecID, &where);
	
	//#if TARGET_API_MAC_CARBON
		dialogPtr = GetNewDialog (kAboutMultiSpecID, NULL, (WindowPtr)-1);
	//#else
	//	HLock (dialogHandle);
	//	dialogPtr = GetNewDialog (kAboutMultiSpecID, *dialogHandle, (WindowPtr)-1);
	//#endif	// TARGET_API_MAC_CARBON else...
	
	if (dialogPtr == NULL)
																							ExitToShell ();
	
	GetPort (&savedPort);
	SetPortDialogPort (dialogPtr);
	MoveWindow (GetDialogWindow (dialogPtr), where.h, where.v, FALSE);
	HideDialogItem (dialogPtr, 1);
	HideDialogItem (dialogPtr, 15);
	#if use_mlte_for_text_window
		Rect									portRect;
		
		GetWindowPortBounds (GetDialogWindow (dialogPtr), &portRect);
		SizeWindow (GetDialogWindow (dialogPtr), 
							portRect.right-portRect.left, 
							portRect.bottom-portRect.top-35, 
							FALSE);
		HideDialogItem (dialogPtr, 12);
	#endif
						
	#if !include_gdal_capability
		HideDialogItem (dialogPtr, 13);
	#endif	// not include_gdal_capability
						
	ShowWindow (GetDialogWindow (dialogPtr));
	DrawDialog (dialogPtr);
	
	if (QDIsPortBuffered (GetDialogPort (dialogPtr)))
		QDFlushPortBuffer (GetDialogPort (dialogPtr), NULL);
	
	gNextStatusTime = TickCount ();
	
			// Set up system dependent flags		
		 	
			// Determine if WaitNextEvent trap is available.  If it is not 		
			// then display a message to the user stating what the mimimum			
			// system is exit.																	
		
	gHasWaitNextEvent = TRUE;
	
	if (!gHasWaitNextEvent || !gGestaltAvailableFlag)
		{
		returnCode = StopAlert (1163, NIL);
																							ExitToShell ();
																				
		}	// end "if (!gHasWaitNextEvent || !gGestaltAvailableFlag)" 
	
	if (gGestaltAvailableFlag)
		{
		if (Gestalt (gestaltSystemVersion, &gestaltResponse) == noErr)
			osVersion = (gestaltResponse >> 8);
		
		if (Gestalt (gestaltSysArchitecture, &gestaltResponse) == noErr)
			{ 
			if (gestaltResponse == gestaltPowerPC)
				gSystemArchitectureCode = kPPC;
					
			if (gestaltResponse == gestaltIntel)
				gSystemArchitectureCode = kIntel;
				
			}	// end "if (Gestalt (gestaltSysArchitecture...)"
				
						// Allow all block sizes of memory to be moved
						// for Power PC's
		
		if (gSystemArchitectureCode == 2 || gSystemArchitectureCode == 3)
			gMaxMoveableBlockSize = SInt32_MAX;
		
		}	// end "if (gGestaltAvailableFlag)" 
	
	if (errCode != noErr)
																							ExitToShell ();
	
			// Initialize other system configuration flags.								
	
	if (gGestaltAvailableFlag)
		{	
		errCode = Gestalt (gestaltQuickdrawVersion, &gQuickDrawVersion);
		
		if (errCode == noErr)
			{
			if (gQuickDrawVersion & 0x0000ff00)
				gColorQDflag = TRUE;
			if (gQuickDrawVersion & 0x0000fe00)
				gQD32IsImplemented = TRUE;
				
			}	// end "if (errCode == noErr)" 
		
		errCode = Gestalt (gestaltAppleEventsAttr, &gestaltResponse);
		
		if (errCode == noErr)
			{
			if (gestaltResponse & 0x00000001)
				gAppleEventsFlag = TRUE;
			
			}	// end "if (errCode == noErr)" 
		
		errCode = Gestalt (gestaltFSAttr, &gestaltResponse);
		
		if (errCode == noErr)
			{
			if (BitTst (&gestaltResponse, 31-gestaltHasFSSpecCalls))
				gHasFSSpecCalls = TRUE;
			
			if (!gHasFSSpecCalls && osVersion < 0x10)
				{
				returnCode = StopAlert (1163, NIL);
																							ExitToShell ();
																					
				}	// end "if (!gHasFSSpecCalls && osVersion < 0x10)"
			
			}	// end "if (errCode == noErr)" 
		
		errCode = Gestalt (gestaltAliasMgrAttr, &gestaltResponse);
		
		if (errCode == noErr)
			{
			if (BitTst (&gestaltResponse, 31-gestaltAliasMgrPresent))
				gAliasMgrPresentFlag = TRUE;
			
			}	// end "if (errCode == noErr)" 
			
				// Determine if the Font Manager is present for 'FMGetFontFamilyFromName'.
				// The Gestalt test does not work. It is true when 
				// FMGetFontFamilyFromName is not available. OS 9 has to be running
				// and CarbonLib needs to be present.
		
		gFontMgrPresentFlag = FALSE;
		if (osVersion >= 0x09 && gCarbonFlag)
			{
			gFontMgrPresentFlag = TRUE;
			//if (Gestalt (gestaltFontMgrAttr, &gestaltResponse) == noErr)
			
			}	// end "if (osVersion >= 0x09 && gCarbonFlag)" 
						
				// Check if Thread manager is present. 

		if (Gestalt (gestaltThreadMgrAttr, &feature) == noErr)
			{
			#ifdef	powerc
					//	If running on a PowerPC, make sure that we not only have the
					//	68K Thread Manager, but also the PowerPC shared library, too.
					//	Because of the wonders of weak linking and out of memory errors
					//	we need to also check to make sure that an entrypoint in the 
					// library is there, too.
					 
				if (long(NewThread) != kUnresolvedSymbolAddress)
					gHasThreadManager = ((feature & ((1 << gestaltThreadMgrPresent) | 
														(1 << gestaltThreadsLibraryPresent))) != 0);
			#else
				gHasThreadManager = ((feature & (1 << gestaltThreadMgrPresent)) != 0);
				gHasThreadManager = FALSE;
			#endif
			
					// Check if SCSI Manager 4.3 is available for asynchronous I/O.
					// This is from develop, June 1997, page 115.
			/*
			#if !TARGET_API_MAC_CARBON	
				if (!TrapAvailable (0xa089))
					gHasThreadManager = FALSE;
			#endif	// !TARGET_API_MAC_CARBON
			*/
			}	// end "if (Gestalt (gestaltThreadMgrAttr, &feature) == noErr)"
			
				// Currently production versions will not have the thread manager
				// option turned on. Additional testing needs to be done.
			
		gHasThreadManager = FALSE;
		
				// Determine if the drag manager is present.

		errCode = Gestalt (gestaltDragMgrAttr, &gestaltResponse);
		if (errCode == noErr && (gestaltResponse & (1 << gestaltDragMgrPresent)) == 0)
			gDragMgrPresentFlag = TRUE;
		
				// Determine if the icon utilites are present.

		errCode = Gestalt (gestaltIconUtilitiesAttr, &gestaltResponse);
		if (errCode == noErr &&
								(gestaltResponse & (1 << gestaltIconUtilitiesPresent)) != 0)
			gHasIconUtilitiesFlag = TRUE;
			
				// Determine if long row bytes is present
				// Currently not used but would sure like for it to be.
		
		errCode = Gestalt (gestaltQuickdrawFeatures, &gestaltResponse);
		if (errCode == noErr && 
									(gestaltResponse & (1 << gestaltQDHasLongRowBytes)) != 0)
			longRowBytesFlag = TRUE; 
		
				// Check if running under OSX
	
		errCode = Gestalt (gestaltMenuMgrAttr, &gestaltResponse);
		if ((errCode == noErr) && (gestaltResponse & gestaltMenuMgrAquaLayoutMask))
			gOSXFlag = TRUE;
			
				// Determine if HFS Plus calls are available for working with files
				// larger that 2 gigabytes.

		errCode = Gestalt (gestaltFSAttr, &gestaltResponse);
		if (errCode == noErr && 
						(gestaltResponse & (1 << gestaltHasHFSPlusAPIs)) != 0)
			//			(gestaltResponse & (1 << gestaltFSSupportsHFSPlusVols)) != 0)
			//gSupportsHFSPlusVols = TRUE;
			gHasHFSPlusAPIs = TRUE; 
			
				// Determine if the Appearance Manager is available.
			
		gAppearanceManagerFlag = IsAppearanceManagerPresent (gGestaltAvailableFlag);
		//gAppearanceManagerFlag = FALSE;
	
		//if (gAppearanceManagerFlag)
		//	RegisterAppearanceClient ();
		
		}	// end "if (gGestaltAvailableFlag)" 
	/*
	#if !TARGET_API_MAC_CARBON	
		else	// !gGestaltAvailableFlag
			{
					// Determine if color quick draw is present.								
					// If so, determine if 32-bit quickdraw is implemented.				
			
			if (sysEnvRec.hasColorQD)
				{
				gColorQDflag = TRUE;

						// Determine if 32-Bit QuickDraw is implemented.					
						
				gQD32IsImplemented = TrapAvailable (QD32Trap);
				
				if (gColorQDflag)
					gQuickDrawVersion = 0x00000100;
				if (gQD32IsImplemented)
					gQuickDrawVersion = 0x00000200;
				
				}	// end "if (sysEnvRec.hasColorQD)" 
				
			}	// end "else !gGestaltAvailableFlag" 
	#endif	// !TARGET_API_MAC_CARBON
	*/
			// Initialize flag indicating whether the GWorld capability is to		
			// be used for offscreen bit maps.												
			
	gUseGWorldFlag = gQD32IsImplemented;
	
	if (!gUseGWorldFlag)
																							ExitToShell ();
	
			// Get memory for IO threads if needed.
	
	if (gHasThreadManager)
		{		
		if (CreateFileIOThreadPool () != noErr)
																							ExitToShell ();
																					
		}	// end "if (gHasThreadManager)"
																					
		
			// If GWorld routines are to be used for offscreen pix maps, then		
			// get a list of place holder memory segments to be used for the		
			// GWorld pointers.  This is done here so that memory will not			
			// become fragmented later.  I do not know how to use the GWorld		
			// capability now without framented memory.
	
		
	//#if TARGET_API_MAC_CARBON
		gNumberAvailableGWorldPtrs = 0;
	//#else	// !TARGET_API_MAC_CARBON
	//	gNumberAvailableGWorldPtrs = kMaxNumberIWindows;
	//#endif	// TARGET_API_MAC_CARBON, else
	
	for (count=0; count<kMaxNumberIWindows; count++)
		{
		//#if TARGET_API_MAC_CARBON
			gGWorldPlaceHolderList[count] = NULL;
		//#else	// !TARGET_API_MAC_CARBON
		//	gGWorldPlaceHolderList[count] = NewPtr (sizeof (CGrafPort));
		//	if (gGWorldPlaceHolderList[count] == NULL)
		//																					ExitToShell ();
		//#endif	// TARGET_API_MAC_CARBON, else
		
		}	// end "for (count=0; count<kMaxNumberIWindows; count++)" 
	
	gOutputLineHeight = 11;
	
			// Build the menus																	 
	
	gMultiSpecMenus[kAppleM] = GetMenu (kAppleMenuID);
	if (gMultiSpecMenus[kAppleM] == NULL)										
																							ExitToShell ();
	//#if !TARGET_API_MAC_CARBON
	//	AppendResMenu (gMultiSpecMenus[kAppleM], (ResType)'DRVR');
	//#endif	// !TARGET_API_MAC_CARBON
	InsertMenu (gMultiSpecMenus[kAppleM], 0);		// insert at end of list 
	
			// Build the 'File' Menu.															
			
	gMultiSpecMenus[kFileM] = GetMenu (kFileMenuID);
	if (gMultiSpecMenus[kFileM] == NULL)										
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kFileM], 0);			// insert at end of list 
	DisableMenuItem (gMultiSpecMenus[kFileM], kNewProject);
	DisableMenuItem (gMultiSpecMenus[kFileM], kOpenProjImage);
 	DisableMenuItem (gMultiSpecMenus[kFileM], kOpenThematicClassGroup);
 	DisableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
 	
	EnableMenuItem (gMultiSpecMenus[kFileM], kSaveTransformMenuItem);
	
	DisableMenuItem (gMultiSpecMenus[kFileM], kSave);
	DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
	DisableMenuItem (gMultiSpecMenus[kFileM], kSaveProject);
	DisableMenuItem (gMultiSpecMenus[kFileM], kSaveProjectAs);
 	DisableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
 	EnableMenuItem  (gMultiSpecMenus[kFileM], kQuit);
 	
			// Allow for Aqua interface.
			// Delete the quit and quit separator from the File menu.
			
	DeleteMenuItem (gMultiSpecMenus[kFileM], kQuit);
	DeleteMenuItem (gMultiSpecMenus[kFileM], kQuit-1);
		
			// Build the 'Edit' Menu.
	
	gMultiSpecMenus[kEditM] = GetMenu (kEditMenuID);
	if (gMultiSpecMenus[kEditM] == NULL)										
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kEditM], 0);			// insert at end of list 
   DisableMenuItem (gMultiSpecMenus[kEditM], kUndo);
   DisableMenuItem (gMultiSpecMenus[kEditM], kCut);
  	DisableMenuItem (gMultiSpecMenus[kEditM], kCopy);
   DisableMenuItem (gMultiSpecMenus[kEditM], kPaste);
   DisableMenuItem (gMultiSpecMenus[kEditM], kClear);
   DisableMenuItem (gMultiSpecMenus[kEditM], kSelectAll);
	DisableMenuItem (gMultiSpecMenus[kEditM], kEditSelectionRect);
	DisableMenuItem (gMultiSpecMenus[kEditM], kClearSelectionRect);
	DisableMenuItem (gMultiSpecMenus[kEditM], kClearTransformMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kEditM], kChangeSpecs);
	DisableMenuItem (gMultiSpecMenus[kEditM], kCoordinates);	
	DisableMenuItem (gMultiSpecMenus[kEditM], kClearOverlays);									
	
	gMultiSpecMenus[kOverlaysM] = GetMenu (kClearOverlaysMenuID);
	if (gMultiSpecMenus[kOverlaysM] == NULL)										
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kOverlaysM], -1);			// insert submenu in	  
																		// edit menu		 
	
	gMultiSpecMenus[kProjectM] = GetMenu (kProjectMenuID);
	if (gMultiSpecMenus[kProjectM] == NULL)										
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kProjectM], 0);		// insert at end of list 
	DisableMenuItem (gMultiSpecMenus[kProjectM], kUseBaseStatistics);
	DisableMenuItem (gMultiSpecMenus[kProjectM], kUseLOOStatistics);
	DisableMenuItem (gMultiSpecMenus[kProjectM], kUseEnhancedStatistics);
	DisableMenuItem (gMultiSpecMenus[kProjectM], kMixedStatisticsUsed);
	DisableMenuItem (gMultiSpecMenus[kProjectM], kClearStatistics);
	DisableMenuItem (gMultiSpecMenus[kProjectM], kChangeBaseImageFile);
	DisableMenuItem (gMultiSpecMenus[kProjectM], kAssociatedImage);
	
	gMultiSpecMenus[kProcessorM] = GetMenu (kProcessorMenuID);
	if (gMultiSpecMenus[kProcessorM] == NULL)								
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kProcessorM], 0);	// insert at end of list 
 	EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
 	DisableMenuItem (gMultiSpecMenus[kProcessorM], kDisplaysMItem);
 	DisableMenuItem (gMultiSpecMenus[kProcessorM], kHistogramMItem);
 	DisableMenuItem (gMultiSpecMenus[kProcessorM], kListDataMenuItem);
 	EnableMenuItem (gMultiSpecMenus[kProcessorM], kReformatMItem);
 	DisableMenuItem (gMultiSpecMenus[kProcessorM], kClusterMItem);
	DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatisticsMItem);
	DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatEnhanceMItem);
	DisableMenuItem (gMultiSpecMenus[kProcessorM], kFeatureExtractionMItem);
 	DisableMenuItem (gMultiSpecMenus[kProcessorM], kSeparabilityMItem);
 	DisableMenuItem (gMultiSpecMenus[kProcessorM], kClassifyMItem);
 	DisableMenuItem (gMultiSpecMenus[kProcessorM], kListResultsMItem);
 	EnableMenuItem (gMultiSpecMenus[kProcessorM], kUtilitiesMItem);

			// Add the reformat submenu.									
 	
	gMultiSpecMenus[kReformatM] = GetMenu (kReformatMenuID);
	if (gMultiSpecMenus[kReformatM] == NULL)								
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kReformatM], -1);			// insert submenu in	  
																		// processor menu		 
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeHeaderMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeImageMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertMultispectralMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertProjectMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertShapeMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatModifyChannelMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatMosaicImagesMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRecodeThematicMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRectifyImageMenuItem);
 	EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertENVIASCIIMenuItem);

			// Add the utilities submenu.														
			
	//SetItemCmd  (gMultiSpecMenus[kProcessorM], kUtilitiesMItem, cmdChar);
	//SetItemMark (gMultiSpecMenus[kProcessorM], kUtilitiesMItem, markCharUtilities);
	gMultiSpecMenus[kUtilitiesM] = GetMenu (kUtilitiesMenuID);
	if (gMultiSpecMenus[kUtilitiesM] == NULL)								
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kUtilitiesM], -1);			// insert submenu in	  
																		// processor menu		 
 	DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kPrincipalComponentsMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kStatisticsImageMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCovarianceCheckMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kBiPlotDataMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kListDescriptionMenuItem);
	
			// Get the options menu.															
	
	gMultiSpecMenus[kOptionM] = GetMenu (kOptionMenuID);
	if (gMultiSpecMenus[kOptionM] == NULL)									
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kOptionM], 0);		// insert at end of list 
	
			// Remove the test menu item if requested.
											
	if (!gIncludeTestMenuItemFlag)	
		DeleteMenuItem (gMultiSpecMenus[kOptionM], kDebug);
	
			// Get the palette menu.
			// Will not use this menu item from now on (3/28/2014)															
			
	//gMultiSpecMenus[kPaletteM] = GetMenu (kPaletteMenuID);
	//if (gMultiSpecMenus[kPaletteM] == NULL)
	//																						ExitToShell ();
	//InsertMenu (gMultiSpecMenus[kPaletteM], -1);	// insert submenu in options menu
	DeleteMenuItem (gMultiSpecMenus[kOptionM], kPalette);
	
			// Get the area units menu.															
			
	gMultiSpecMenus[kAreaUnitsM] = GetMenu (kDefaultAreaUnitsMenuID);
	if (gMultiSpecMenus[kAreaUnitsM] == NULL)									
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kAreaUnitsM], -1);	// insert submenu in options menu 
	
			// Get the windows menu
	
	gMultiSpecMenus[kWindowM] = GetMenu (kWindowMenuID);
	if (gMultiSpecMenus[kWindowM] == NULL)										
																							ExitToShell ();
	InsertMenu (gMultiSpecMenus[kWindowM], 0);		// insert at end of list 
 	DisableMenuItem (gMultiSpecMenus[kWindowM], kShowHideCoordinateView);
	DisableMenuItem (gMultiSpecMenus[kWindowM], kSelectionGraphMenuItem);
 	DisableMenuItem (gMultiSpecMenus[kWindowM], kPaletteWindow);
 	DisableMenuItem (gMultiSpecMenus[kWindowM], kStatisticsWItem);
	
	DrawMenuBar ();
	
			// Initialize redraw menu bar flag.												
	
	gRedrawMenuBar = TRUE;
	
			// Load in menu resources for the pop menus.									
		
	gPopUpAllSubsetMenu = GetMenu (kPopUpAllSubsetMenuID);
	if (gPopUpAllSubsetMenu == NULL)
																							ExitToShell ();
																					
	gPopUpClassMenu = GetMenu (kPopUpClassMenuID);
	if (gPopUpClassMenu == NULL)
																							ExitToShell ();
																					
	gPopUpCompareParameterMenu = GetMenu (kPopUpCompareParameterMenuID);
	if (gPopUpCompareParameterMenu == NULL)
																							ExitToShell ();
																					
	gPopUpProjectChangesMenu = GetMenu (kPopUpProjectChangesMenuID);
	if (gPopUpProjectChangesMenu == NULL)
																							ExitToShell ();
																					
	gPopUpWeightsMenu = GetMenu (kPopUpWeightsMenuID);
	if (gPopUpWeightsMenu == NULL)
																							ExitToShell ();
	
	gPopUpPaletteMenu = GetMenu (kPopUpPaletteMenuID);
	if (gPopUpPaletteMenu == NULL)
																							ExitToShell ();
	
	gPopUpDiskFileMenu = GetMenu (kPopUpDiskFileMenuID);
	if (gPopUpDiskFileMenu == NULL)
																							ExitToShell ();
	
	gPopUpFEAlgorithmMenu = GetMenu (kPopUpFEAlgorithmMenuMenuID);
	if (gPopUpFEAlgorithmMenu == NULL)
																							ExitToShell ();
	
	gPopUpSymbolsMenu = GetMenu (kPopUpSymbolsMenuID);
	if (gPopUpSymbolsMenu == NULL)
																							ExitToShell ();
	
	gPopUpAllAvailableSubsetMenu = GetMenu (kPopUpAllAvailableSubsetMenuID);
	if (gPopUpAllAvailableSubsetMenu == NULL)
																							ExitToShell ();
	
	gPopUpAllPossibleSubsetMenu = GetMenu (kPopUpAllPossibleSubsetMenuID);
	if (gPopUpAllPossibleSubsetMenu == NULL)
																							ExitToShell ();
	
	gPopUpDataValueTypeMenu = GetMenu (kPopUpDataValueTypeMenuID);
	if (gPopUpDataValueTypeMenu == NULL)
																							ExitToShell ();
	
	gPopUpBitsPerDataValueMenu = GetMenu (kPopUpBitsPerDataValueMenuID);
	if (gPopUpBitsPerDataValueMenu == NULL)
																							ExitToShell ();
	
	gPopUpBandInterleaveMenu = GetMenu (kPopUpBandInterleaveMenuID);
	if (gPopUpBandInterleaveMenu == NULL)
																							ExitToShell ();
	
	gPopUpClassGroupMenu = GetMenu (kPopUpClassGroupMenuID);
	if (gPopUpClassGroupMenu == NULL)
																							ExitToShell ();
	
	gPopUpClusterMaskDiskFileMenu = GetMenu (kPopUpClusterDiskFileMenuID);
	if (gPopUpClusterMaskDiskFileMenu == NULL)
																							ExitToShell ();
	
	gPopUpHeaderOptionsMenu = GetMenu (kPopUpHeaderOptionsMenuID);
	if (gPopUpHeaderOptionsMenu == NULL)
																							ExitToShell ();
	
	gPopUpSaveStatsMenu = GetMenu (kPopUpSaveStatsMenuID);
	if (gPopUpSaveStatsMenu == NULL)
																							ExitToShell ();
	
	gPopUpTemporaryMenu = GetMenu (kColorMenuID);
	if (gPopUpTemporaryMenu == NULL)
																							ExitToShell ();
	
	gPopUpOutputFileMenu = GetMenu (kPopUpOutputFileMenuID);
	if (gPopUpOutputFileMenu == NULL)
																							ExitToShell ();
	
	gPopUpChanDescriptionMenu = GetMenu (kPopUpChannelDescriptionMenuID);
	if (gPopUpChanDescriptionMenu == NULL)
																							ExitToShell ();
	DisableMenuItem (gPopUpChanDescriptionMenu, 1);
	DeleteMenuItem (gPopUpChanDescriptionMenu, 1);
	
	gPopUpSeparabilityDistanceMenu = GetMenu (kPopUpSeparabilityDistanceMenuID);
	if (gPopUpSeparabilityDistanceMenu == NULL)
																							ExitToShell ();
	
	gPopUpClassifyProcedureMenu = GetMenu (kPopUpClassifyProcedureMenuID);
	if (gPopUpClassifyProcedureMenu == NULL)
																							ExitToShell ();
	
			// Remove the Decision Tree option for operational use.
			
	DeleteMenuItem (gPopUpClassifyProcedureMenu, kDecisionTreeMode);
	
	gPopUpEchoAlgorithmMenu = GetMenu (kPopUpEchoAlgorithmMenuID);
	if (gPopUpEchoAlgorithmMenu == NULL)
																							ExitToShell ();
	
	gPopUpPreprocessMenu = GetMenu (kPopUpPreprocessMenuID);
	if (gPopUpPreprocessMenu == NULL)
																							ExitToShell ();
	
	gPopUpClassGroupMenu2 = GetMenu (kPopUpClassGroupMenuID2);
	if (gPopUpClassGroupMenu2 == NULL)
																							ExitToShell ();
	
	gPopUpHistogramMethodMenu = GetMenu (kPopUpHistogramMethodMenuID);
	if (gPopUpHistogramMethodMenu == NULL)
																							ExitToShell ();
	
	gPopUpCovarianceEstimateMenu = GetMenu (kPopUpCovarianceMenuID);
	if (gPopUpCovarianceEstimateMenu == NULL)
																							ExitToShell ();
	
	gPopUpStatisticsTypeMenu = GetMenu (kPopUpStatisticsTypeMenuID);
	if (gPopUpStatisticsTypeMenu == NULL)
																							ExitToShell ();
	
	gPopUpListDataFormatMenu = GetMenu (kPopUpListDataFormatMenuID);
	if (gPopUpListDataFormatMenu == NULL)
																							ExitToShell ();
	
	gPopUpSelectTrainMaskImageMenu = GetMenu (kPopUpSelectMaskImage1MenuID);
	if (gPopUpSelectTrainMaskImageMenu == NULL)
																							ExitToShell ();
	
	gPopUpSelectTestMaskImageMenu = GetMenu (kPopUpSelectMaskImage2MenuID);
	if (gPopUpSelectTestMaskImageMenu == NULL)
																							ExitToShell ();
																					
	gPopUpSelectThresholdImageMenu = GetMenu (kPopUpSelectThresholdImageMenuID);
	if (gPopUpSelectThresholdImageMenu == NULL)
																							ExitToShell ();
	
	gPopUpSoftThresholdMenu = GetMenu (kPopUpSoftThresholdMenuID);
	if (gPopUpSoftThresholdMenu == NULL)
																							ExitToShell ();
	
	gPopUpHardThresholdMenu = GetMenu (kPopUpHardThresholdMenuID);
	if (gPopUpHardThresholdMenu == NULL)
																							ExitToShell ();
	
	gPopUpReferenceSystemMenu = GetMenu (kPopUpReferenceSystemMenuID);
	if (gPopUpReferenceSystemMenu == NULL)
																							ExitToShell ();
	
	gPopUpProjectionMenu = GetMenu (kPopUpProjectionMenuID);
	if (gPopUpProjectionMenu == NULL)
																							ExitToShell ();
	
	gPopUpMapUnitsMenu = GetMenu (kPopUpMapUnitsMenuID);
	if (gPopUpMapUnitsMenu == NULL)
																							ExitToShell ();
	
	gPopUpEllipsoidMenu = GetMenu (kPopUpEllipsoidMenuID);
	if (gPopUpEllipsoidMenu == NULL)
																							ExitToShell ();
	
	gPopUpCoordinateUnitsMenu = GetMenu (kPopUpCoordinateUnitsMenuID);
	if (gPopUpCoordinateUnitsMenu == NULL)
																							ExitToShell ();
	
	gPopUpHorizontalDatumMenu = GetMenu (kPopUpDatumMenuID);
	if (gPopUpHorizontalDatumMenu == NULL)
																							ExitToShell ();
																					
	gPopUpDisplayTypeMenu = GetMenu (kPopUpDisplayTypeMenuID);
	if (gPopUpDisplayTypeMenu == NULL)
																							ExitToShell ();

	gPopUpBitsOfColorMenu = GetMenu (kPopUpBitsOfColorMenuID);
	if (gPopUpBitsOfColorMenu == NULL)
																							ExitToShell ();

	gPopUpStretchMenu = GetMenu (kPopUpStretchMenuID);
	if (gPopUpStretchMenu == NULL)
																							ExitToShell ();

	gPopUpMinMaxMenu = GetMenu (kPopUpMinMaxMenuID);
	if (gPopUpMinMaxMenu == NULL)
																							ExitToShell ();

	gPopUpTreatZeroAsMenu = GetMenu (kPopUpTreatZeroAsMenuID);
	if (gPopUpTreatZeroAsMenu == NULL)
																							ExitToShell ();

	gPopUpAreaUnitsMenu = GetMenu (kPopUpAreaUnitsMenuID);
	if (gPopUpAreaUnitsMenu == NULL)
																							ExitToShell ();

	gPopUpImageOverlayMenu = GetMenu (kPopUpImageOverlayMenuID);
	if (gPopUpImageOverlayMenu == NULL)
																							ExitToShell ();
	
	gPopUpCollapseClassMenu = GetMenu (kPopUpCollapseClassMenuID);
	if (gPopUpCollapseClassMenu == NULL)
																							ExitToShell ();
	
	gPopUpSelectionDisplayUnitsMenu = GetMenu (kPopUpSelectionDisplayUnitsMenuID);
	if (gPopUpSelectionDisplayUnitsMenu == NULL)
																							ExitToShell ();
	
	gPopUpResampleMenu = GetMenu (kPopUpResampleMenuID);
	if (gPopUpResampleMenu == NULL)
																							ExitToShell ();
	
	gPopUpFunctionMenu = GetMenu (kPopUpFunctionMenuID);
	if (gPopUpFunctionMenu == NULL)
																							ExitToShell ();
	
	gPopUpLinkOptionsMenu = GetMenu (kPopUpLinkOptionsMenuID);
	if (gPopUpLinkOptionsMenu == NULL)
																							ExitToShell ();
	
	gPopUpDiskFileFormatMenu = GetMenu (kPopUpDiskFileFormatMenuID);
	if (gPopUpDiskFileFormatMenu == NULL)
																							ExitToShell ();
	
	gPopUpMosaicDirectionMenu = GetMenu (kPopUpMosaicDirectionMenuID);
	if (gPopUpMosaicDirectionMenu == NULL)
																							ExitToShell ();
	
	gPopUpGraphAxisLabelMenu = GetMenu (kPopUpGraphAxisLabelMenuID);
	if (gPopUpGraphAxisLabelMenu == NULL)
																							ExitToShell ();

			// This is the default cursor.

	gCursorPtr[kArrow] = NULL;	

			// Get cross cursor for later use												
			
	cursorHndl = GetCursor (kCrossCursorID);
	gCursorPtr[kCross] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kCross] == NULL)													
																							ExitToShell ();
	
			// Set up waitCursor as watch cursor											

	cursorHndl = GetCursor (watchCursor);
	gCursorPtr[kWait] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kWait] == NULL)													
																							ExitToShell ();
	MSetCursor (kWait);

			// Get I beam cursor for later use												
			
	cursorHndl = GetCursor (iBeamCursor);
	gCursorPtr[kIBeam] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kIBeam] == NULL)													
																							ExitToShell ();

			// Get hand cursor for later use													
	
	cursorHndl = GetCursor (kHandCursorID);
	gCursorPtr[kHand] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kHand] == NULL)													
																							ExitToShell ();

			// Get legend width cursor for later use.										
	
	cursorHndl = GetCursor (kLegendWidthCursorID);
	gCursorPtr[kLegendWidth] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kLegendWidth] == NULL)													
																							ExitToShell ();

			// Get legend width "down" cursor for later use.										
	
	cursorHndl = GetCursor (kLegendWidthDownCursorID);
	gCursorPtr[kLegendWidthDown] = (CursPtr)GetHandlePointer (
																(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kLegendWidthDown] == NULL)													
																							ExitToShell ();

			// Get blink open cursor for later use.												
	
	cursorHndl = GetCursor (kBlinkCursor1ID);
	gCursorPtr[kBlinkOpenCursor1] = (CursPtr)GetHandlePointer (
																(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kBlinkOpenCursor1] == NULL)													
																							ExitToShell ();
	
			// This is the same cursor.
																							
	gCursorPtr[kBlinkOpenCursor2] = gCursorPtr[kBlinkOpenCursor1];

			// Get blink shut cursor for later use.												
	
	cursorHndl = GetCursor (kBlinkCursor2ID);
	gCursorPtr[kBlinkShutCursor] = (CursPtr)GetHandlePointer (
																(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kBlinkShutCursor] == NULL)													
																							ExitToShell ();

			// Get spin cursor1 for later use.												
	
	cursorHndl = GetCursor (kSpinCursor1ID);
	gCursorPtr[kSpin] = (CursPtr)GetHandlePointer ((Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kSpin] == NULL)													
																							ExitToShell ();

			// Get spin cursor2 for later use.												
	
	cursorHndl = GetCursor (kSpinCursor1ID+1);
	gCursorPtr[kSpin+1] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kSpin+1] == NULL)													
																							ExitToShell ();

			// Get spin cursor3 for later use.												
	
	cursorHndl = GetCursor (kSpinCursor1ID+2);
	gCursorPtr[kSpin+2] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kSpin+2] == NULL)													
																							ExitToShell ();

			// Get spin cursor4 for later use.												
	
	cursorHndl = GetCursor (kSpinCursor1ID+3);
	gCursorPtr[kSpin+3] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kSpin+3] == NULL)													
																							ExitToShell ();

			// Get hand closed cursor for later use													
	
	cursorHndl = GetCursor (kHandClosedCursorID);
	gCursorPtr[kHandClosed] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kHandClosed] == NULL)													
																							ExitToShell ();

			// Get cross cursor2 for later use													
	
	cursorHndl = GetCursor (kCrossCursor2ID);
	gCursorPtr[kCross2] = (CursPtr)GetHandlePointer (
															(Handle)cursorHndl, kLock, kMoveHi);
	if (gCursorPtr[kCross2] == NULL)													
																							ExitToShell ();
																					
			// Get resources for Navigation Services custom get dialogs.
			
	gCustomNavOpenList = GetResource ('DITL', kNavServicesOpenList);
	if (gCustomNavOpenList == NULL)													
																							ExitToShell ();
																					
			// Get resources for Navigation Services custom put dialogs.
			
	gCustomNavPut = GetResource ('DITL', kNavServicesCustomPut);
	if (gCustomNavPut == NULL)													
																							ExitToShell ();

	GetIndPattern (&gOutlinePenPattern, kLinePatternID, 1);
	
			// Get the font number to be used for the windows.  Assume the 		
			// monaco font for now.												
	
	if (gFontMgrPresentFlag)		
		gWindowTextFont = FMGetFontFamilyFromName ("\pmonaco");
		
	else	// !gCarbonFlag
		GetFNum ("\pmonaco", &gWindowTextFont);
		
	if (gWindowTextFont == 0)		
		gWindowTextFont = 4;

	gToDisplayLevels.vectorHandle = NULL;

			// Load parameters to handle Core Graphics when running under
			// OSX.
			
	gOSXCoreGraphicsFlag = LoadCoreGraphicsVariables ();
			
	if (!gOSXCoreGraphicsFlag)
		{
		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							kAlertStrID, 
							IDS_Alert93, 
							IDS_Alert94, 
							NULL);
																							ExitToShell ();
																				
		}	// end "if (!gOSXCoreGraphicsFlag)"
		
 	if (gOSXCoreGraphicsFlag)
		gTimeOffset = 30;
		
	//gOSXCoreGraphicsFlag = FALSE;
	
			// Determine the maximum number of row bytes that can exist for a 	
			// bit map/pix map.																	
			
	gMaxRowBytes = kMaxRowBytes;
	if (gQD32IsImplemented)
		gMaxRowBytes = kQD32MaxRowBytes;
	if (gOSXCoreGraphicsFlag)
				// Number of pixRowBytes is a multiple of 8 bytes.
		gMaxRowBytes = (((32*kMaxNumberOSXDisplayColumns + 127 + 32) >> 7) << 4);
	
			// Initialize the window list vector								
			
	gWindowList[0] = NULL;
	gWindowList[1] = NULL;
	gWindowList[2] = NULL;
	
			// Intialize blink procedure to use.											
			
	gBlinkProcedure = kBlink1;
	//if (gQD32IsImplemented)
	//	gBlinkProcedure = kBlink2;
		
			// Initialize image window information.
	
	for (count=0; count<kMaxNumberIWindows; count++)
		gImageWindowHandleList[count] = NULL;
		  	
			// Create the output window. 												
			
	gOutputWindow = CreateOutputWindow (&returnFlag);
		    		
	if (!returnFlag)															
																							ExitToShell ();
																				
			// Initialize the Waste (text engine).
			
	if (!InitializeTextEngine ())
																							ExitToShell ();
	
	#if use_mlte_for_text_window	
		returnCode = TXNAttachObjectToWindowRef (gTextObject, gOutputWindow);
		if (returnCode != noErr)															
																							ExitToShell ();
																				
		returnCode = TXNSetScrollbarState (gTextObject, kScrollBarsAlwaysActive);
		if (returnCode != noErr)															
																							ExitToShell ();
																				
		TXNFocus (gTextObject, TRUE);			
	#endif	// use_mlte_for_text_window

			// Now that window variables are initialized, force an update 
			// in the palette menu.																	

	//gUpdatePaletteMenuItemsFlag = TRUE;
	
			// Set outputWindow as default start start up window. 					
			
	SetPortWindowPort (gOutputWindow);

	gCoordinateLineValue = -1;	
	gCoordinateColumnValue = -1;
	
			// Initialize coordinate view background color.
	
	gCoordinateViewBackgroundColor.red = 0xDDDD;
	gCoordinateViewBackgroundColor.green = 0xDDDD;
	gCoordinateViewBackgroundColor.blue = 0xDDDD;
	
			// Initialize inactive foreground color.
	
	gInactiveForeGroundColor.red = 0x8888;
	gInactiveForeGroundColor.green = 0x8888;
	gInactiveForeGroundColor.blue = 0x8888;
	
			// Initialize the default overlay colors
			//		blackColor
			
	gOverlayColorList[0].red = 0x0000;
	gOverlayColorList[0].green = 0x0000;
	gOverlayColorList[0].blue = 0x0000;
	
			//		cyanColor
			
	gOverlayColorList[1].red = 0x0000;
	gOverlayColorList[1].green =0xFFFF;
	gOverlayColorList[1].blue = 0xFFFF;
	
			//		greenColor
			
	gOverlayColorList[2].red = 0x0000;
	gOverlayColorList[2].green = 0xFFFF;
	gOverlayColorList[2].blue = 0x0000;
	
			//		blueColor
			
	gOverlayColorList[3].red = 0x0000;
	gOverlayColorList[3].green = 0x0000;
	gOverlayColorList[3].blue = 0xFFFF;
	
			//		magentaColor
			
	gOverlayColorList[4].red = 0xFFFF;
	gOverlayColorList[4].green = 0x0000;
	gOverlayColorList[4].blue = 0xFFFF;
	
			//		redColor
			
	gOverlayColorList[5].red = 0xFFFF;
	gOverlayColorList[5].green = 0x0000;
	gOverlayColorList[5].blue = 0x0000;
	
			//		yellowColor
			
	gOverlayColorList[6].red = 0xFFFF;
	gOverlayColorList[6].green = 0xFFFF;
	gOverlayColorList[6].blue = 0x0000;	
		
	gNextGraphicsWindowStart.v	= 0;
	gNextGraphicsWindowStart.h = 0;
	/*
	#if !TARGET_API_MAC_CARBON	
				// Create storage for palette window.											
				
		if (gColorQDflag)
			gPaletteWindowHandle = MNewHandle (sizeof (CWindowRecord));
			
		else	// !gColorQDflag 
			gPaletteWindowHandle = MNewHandle (sizeof (WindowRecord));
		
		if (gPaletteWindowHandle == NULL)
																							ExitToShell ();
		HLockHi (gPaletteWindowHandle);
		
						
				// Create storage for statistics window.										
				
		gProjectWindowHandle = MNewHandle (sizeof (WindowRecord));
		if (gProjectWindowHandle == NULL)
																							ExitToShell ();
		HLockHi (gProjectWindowHandle);
	#endif	// !TARGET_API_MAC_CARBON	
	*/
	gTextTab = '\t';
	gTextCR[0]  = '\r';
	gTextCR[1]  = '\r';
	gTextCR[2]  = 0;
	
	//#if TARGET_API_MAC_CARBON
				// Set up the list definition for carbon applications.
				
		gListDefinitionSpec.defType = kListDefUserProcType;
		gListDefinitionSpec.u.userProc = NewListDefUPP (LegendListDefinition);
		if (gListDefinitionSpec.u.userProc == NULL)
																				ExitToShell ();
	
				// Set up the control definition for carbon applications.
				
		gControlDefinitionSpec.defType = kControlDefProcPtr;
		gControlDefinitionSpec.u.defProc = NewControlDefUPP (ControlDefinition);
		if (gControlDefinitionSpec.u.defProc == NULL)
																							ExitToShell ();
	/*#else	// !TARGET_API_MAC_CARBON
				// Set up the code resources for the legend list definition				
				// procedure.				
						
		legendListDefinitionHandle = (JmpHandle)Get1Resource ('LDEF', 131);
		if (legendListDefinitionHandle == NULL)
																							ExitToShell ();
																				
		HNoPurge ((Handle)legendListDefinitionHandle);	
		//	HLock ((Handle)legendListDefinitionHandle);
		(**legendListDefinitionHandle).jmpInstr = 0x4EF9;

		(**legendListDefinitionHandle).jmpAddr = NewListDefProc (LegendListDefinition);
														
		if ((**legendListDefinitionHandle).jmpAddr == NULL)
																							ExitToShell ();
	
				// Set up the code resources for the legend control popup 
				// definition procedure.																			
				
		controlDefinitionHandle = (JmpControlHandle)Get1Resource ('CDEF', 131);
		if (controlDefinitionHandle == NULL)
																							ExitToShell ();
																				
		HNoPurge ((Handle)controlDefinitionHandle);	
		//	HLock ((Handle)controlDefinitionHandle);
		(**controlDefinitionHandle).jmpInstr = 0x4EF9;

		(**controlDefinitionHandle).jmpAddr = NewControlDefProc (ControlDefinition);
														
		if ((**controlDefinitionHandle).jmpAddr == NULL)
																							ExitToShell ();
	#endif	// TARGET_API_MAC_CARBON, else...
	*/
	
			// Set global variables containing the maximum print page size to		
			// use.  This is defined by the QMS colorscript 100.  Hopefully in	
			// the future this parameter will be set properly by the printer		
			// drives.																				
			
	gMaxPrintPageWidth = 583;
	gMaxPrintPageHeight = 638;
	
			// Initialize structure used for file I/O.
			
	InitializeFileIOInstructions (&gFileIOInstructions[0]);
	InitializeFileIOInstructions (&gFileIOInstructions[1]);
	
			// Get the process serial number.
			
	::GetCurrentProcess (&gProcessSerialNumber);
			
			// Initialize information in workflow structure.
		
	gMultiSpecWorkflowInfo.lastFileSavedInfo.fileName[0] = 0;
	gMultiSpecWorkflowInfo.lastFileReadName[0] = 0;
	gMultiSpecWorkflowInfo.defaultName[0] = 0;
	gMultiSpecWorkflowInfo.workFlowCode = 0;
	gMultiSpecWorkflowInfo.thresholdValue = 26;	
	
			// Initialize handles for processor specifications that may be used	
			// in non-project specifications.												
			
	gNonProjProcessorSpecs.biPlotDataSpecsH = NULL;
	gNonProjProcessorSpecs.clusterSpecsH = NULL;
	gNonProjProcessorSpecs.listDataSpecsH = NULL;
	gNonProjProcessorSpecs.listResultsSpecsH = NULL;
	gNonProjProcessorSpecs.principalCompSpecsH = NULL;
	gNonProjProcessorSpecs.reformatOptionsSpecsH = NULL;
	gNonProjProcessorSpecs.statisticsImageSpecsH = NULL;
	gNonProjProcessorSpecs.evalTransformationSpecsH = NULL;
	
			// Initialize variables for results input and output files.				
			
	gResultsFileSummary.numberFiles = 0;
	gResultsFileSummary.diskFileListH = NULL;
	gResultsFileSummary.diskFileListPtr = NULL;
	
			// Intialize structure containing transformation matrix.					
			
	gTransformationMatrix.imageFileName[0] = 0;
	gTransformationMatrix.preProcBandGroupingHandle = NULL;
	gTransformationMatrix.eigenValueHandle = NULL;
	gTransformationMatrix.eigenVectorHandle = NULL;
	gTransformationMatrix.eigenFeatureHandle = NULL;
	gTransformationMatrix.createdByCode = 0;
	gTransformationMatrix.numberChannels = 0;
	gTransformationMatrix.numberFeatures = 0;
	gTransformationMatrix.eigenVectorPtr = NULL; 
	gTransformationMatrix.tempMatrixPtr = NULL; 
	gTransformationMatrix.eigenFeaturePtr = NULL;
	gTransformationMatrix.offsetVectorPtr = NULL;

			// Initialize structure containing pointers for matrix
			// inversion.

	gInverseMatrixMemory.inversePtr = NULL;
	gInverseMatrixMemory.pivotPtr = NULL;
	gInverseMatrixMemory.indexRowPtr = NULL;
	gInverseMatrixMemory.indexColPtr = NULL;
	gInverseMatrixMemory.ipvotPtr = NULL;
	
			// Initialize the file types that MultiSpec is allowed to recognize.
			
	gListTypes[0] = 'TEXT';
	gListTypes[1] = '???\?';	// Allow for ANSI trigraph 
	gListTypes[2] = 'BINA';
	gListTypes[3] = 'Gcls';
	gListTypes[4] = 'rIMG';
	gListTypes[5] = 'TPIC';
	gListTypes[6] = 'TIFF';
	gListTypes[7] = 'IMGg';
	gListTypes[8] = 'DATA';
	
	gNumberFileTypes	= 9;
	
			// Place c-string terminators at the end of the global
			// textstrings to try to catch any pascal string that are treated
			// as c-strings which did not get the terminator added.
			
	gTextString[255] = kNullTerminator;
	gTextString2[255] = kNullTerminator;
	gTextString3[255] = kNullTerminator;
	
			// Indicate that the default display structure has not been loaded.
			
	gDisplaySpecsDefault.structureLoadedFlag = FALSE;
	gDisplaySpecsDefault.numberThematicLevels = 12;
	
			// Allocate space for some temporary region handles.						
			
	gTempRegion1 = NewRgn ();
	if (gTempRegion1 == NULL)
																							ExitToShell ();
	
	gTempRegion2 = NewRgn ();
	if (gTempRegion2 == NULL)
																							ExitToShell ();
	
	gSelectionClipRgn = NewRgn ();
	if (gSelectionClipRgn == NULL)
																							ExitToShell ();
	
			// Set procedure to handle memory shortage.									
			
	myGrowZonePtr = NewGrowZoneUPP (MyGrowZone);
	if (myGrowZonePtr == NULL)
																							ExitToShell ();
	
	SetGrowZone (myGrowZonePtr);
	
			// Get the pointers to the rountines that are called indirectly.		
	/*
	#if !TARGET_API_MAC_CARBON	
				// Standard File Hook routines.													
		
		gGetFileHookPtr = NewDlgHookProc (GetFileHook);
		if (gGetFileHookPtr == NULL)
																							ExitToShell ();
																					
		gPutFileHookPtr = NewDlgHookProc (PutFileHook);
		if (gPutFileHookPtr == NULL)
																							ExitToShell ();
	#endif	// !TARGET_API_MAC_CARBON
	*/
			// FileIO routines.
	
	if (gHasThreadManager)
		{
		gFileIOCompletionPtr = NewIOCompletionUPP (FileIOCompletionRoutine);
		if (gFileIOCompletionPtr == NULL)
																							ExitToShell ();
																				
		}	// end "if (gHasThreadManager)"
			
			// Modal Dialog Filter routines.													

	gProcessorDialogFilterPtr = NewModalFilterUPP (ProcessorDialogFilter);
	if (gProcessorDialogFilterPtr == NULL)
																							ExitToShell ();
																				
	gDialogListFilterPtr = NewModalFilterUPP (DialogListFilter);
	if (gDialogListFilterPtr == NULL)
																							ExitToShell ();
			
			// Control Action routines.			
																				
	gGraphChangeSetControlActionPtr = 
									NewControlActionUPP (GraphChangeSetControlEvent);
	if (gGraphChangeSetControlActionPtr == NULL)
																							ExitToShell ();
	
	gImageWScrollActionPtr = NewControlActionUPP (ImageWScroll);
	if (gImageWScrollActionPtr == NULL)
																							ExitToShell ();

	gImageWThumbScrollActionPtr = NewControlActionUPP (ImageWThumbScrollAction);
	if (gImageWThumbScrollActionPtr == NULL)
																							ExitToShell ();
																				
	gImageZoomControlActionPtr = NewControlActionUPP (ImageZoomControlEvent);
	if (gImageZoomControlActionPtr == NULL)
																							ExitToShell ();
																				
	gThematicLegendControlActionPtr = 
								NewControlActionUPP (ThematicLegendControlEvent);
	if (gThematicLegendControlActionPtr == NULL)
																							ExitToShell ();
																				
	gOutputWScrollActionPtr = NewControlActionUPP (OutputWScroll);
	if (gOutputWScrollActionPtr == NULL)
																							ExitToShell ();

			// Word Solution Engine routines.												
	/*
	gOutputAutoScrollActionPtr =  NewWSEScrollActionProc (OutputAutoScroll);
	if (gOutputAutoScrollActionPtr == NULL)
																							ExitToShell ();
	*/
			// List Click Loop routines.

	gLegendCClickLoopPtr = NewListClickLoopUPP (LegendCClickLoop);
	if (gLegendCClickLoopPtr == NULL)
																							ExitToShell ();
	
			// Drag Gray Region routines.														
			// This routine pointer will be intialized when used in the				
			// GroupsDialog routine.  This routine may move in memory.				
	
	gDragScrollListPtr = NewDragGrayRgnUPP (DragScrollList);
	if (gDragScrollListPtr == NULL)
																							ExitToShell ();
			
			// Dialog User Item routines.														
	
	gCreateOneColumnList1Ptr = NewUserItemUPP (CreateOneColumnList1);
	if (gCreateOneColumnList1Ptr == NULL)
																							ExitToShell ();
	
	gCreateOneColumnList2Ptr = NewUserItemUPP (CreateOneColumnList2);
	if (gCreateOneColumnList2Ptr == NULL)
																							ExitToShell ();
																				
	gDrawBandInterleavePopUpPtr = NewUserItemUPP (DrawBandInterleavePopUp);
	if (gDrawBandInterleavePopUpPtr == NULL)
																							ExitToShell ();

	gDrawChannelsPopUpPtr = NewUserItemUPP (DrawChannelsPopUp);
	if (gDrawChannelsPopUpPtr == NULL)
																							ExitToShell ();
																				
	gDrawDialogClassPopUpPtr = NewUserItemUPP (DrawDialogClassPopUp);
	if (gDrawDialogClassPopUpPtr == NULL)
																							ExitToShell ();
																				
	gDrawGraphicsBoxPtr = NewUserItemUPP (DrawGraphicsBox);
	if (gDrawGraphicsBoxPtr == NULL)
																							ExitToShell ();
																				
	gDrawImageFilePopUpPtr = NewUserItemUPP (DrawImageFilePopUp);
	if (gDrawImageFilePopUpPtr == NULL)
																							ExitToShell ();
																			
	gHiliteOKButtonPtr = NewUserItemUPP (HiliteOKButton);
	if (gHiliteOKButtonPtr == NULL)
																							ExitToShell ();
																				
	gSetEntireImageIconPtr = NewUserItemUPP (SetEntireImageIcon);
	if (gSetEntireImageIconPtr == NULL)
																							ExitToShell ();
																				
	gDrawSymbolsPopUpPtr = NewUserItemUPP (DrawSymbolsPopUp);
	if (gDrawSymbolsPopUpPtr == NULL)
																							ExitToShell ();
																				
	gDrawWeightsPopUpPtr = NewUserItemUPP (DrawWeightsPopUp);
	if (gDrawWeightsPopUpPtr == NULL)
																							ExitToShell ();
	
			// Allocate memory to use as spare for loading code resources into	
			// and allocate memory as spare to present memory warning message.	
			
	gMemoryTypeNeeded = 2;
	
	gSpareCodeMemoryH = MNewHandle ((Size)gSpareCodeSize);
		
	if (gSpareCodeMemoryH == NULL)
		gDoneFlag = TRUE;
		
	if (!gDoneFlag)
		gSpareWarnMemoryH = MNewHandle ((Size)gSpareWarnSize);
	if (gSpareWarnMemoryH == NULL)
		gDoneFlag = TRUE;											
			
	gMemoryTypeNeeded = 0;
	
			// Exit routine if memory is too short for spare handles.				
			
	if (gDoneFlag)
																									return;
	
			// Initialize double variables.													
			
	InitializeDoubleVariables ();
	
	#ifndef __XCODE__
				// Get the characters to use for the thousands separator and the decimal
				// separator.
		
		NumberPartsPtr		numberPartsPtr;
		Ptr					ptr;
		Handle				itlHandle;
		SInt32 				offset;
		SInt32 				length;	
		GetIntlResourceTable (iuSystemScript,
									 smNumberPartsTable,
									 &itlHandle,
									 &offset,
									 &length);
									 
		ptr = (Ptr)GetHandlePointer (itlHandle);
		
		if (ptr != NULL)
			{
			numberPartsPtr = (NumberPartsPtr)&ptr[offset];
		
			gThousandsSeparator = numberPartsPtr->data[tokThousands-1].a[1];
			gDecimalSeparator = numberPartsPtr->data[tokDecPoint-1].a[1];
			
			}	// end "if (ptr != NULL)"
	#endif	// ndef __XCODE__
	
			// Insert entries for the required Apple events.							
			
	if (gAppleEventsFlag)
		gDoneFlag = InitializeAppleEventParameters ();
		
	
			// Get a list of place holder memory segments to be used for the 		
			// image window records.  This is done here so that memory will not	
			// become fragmented later.  One may 'waste' a few hundred bytes		
			// for unused window records but it saves several K bytes later;		
			// especially if projects are opened and closed.							
			
	if (!gDoneFlag)
		{
		for (count=0; count<kMaxNumberIWindows; count++)
			{
			//#if TARGET_API_MAC_CARBON
				gImageWindowHandleList[count] = NULL;
			/*#else
				handle = MNewHandle (sizeof (WindowRecord));
				if (handle != NULL)
					{
					MoveHHi (handle);
					HLock (handle);				
					gNumberAvailableImageWindowHandles++;
					gImageWindowHandleList[count] = handle;
																				
					}	// end "if (handle != NULL)" 
				
				else	// handle == NULL 
					{
					gDoneFlag = TRUE;
					break;
					
					}	// end "else handle == NULL" 
			#endif	// TARGET_API_MAC_CARBON else...
			*/
			}	// end "for (count=0; count<kMaxNumberIWindows; count++)" 
		
		}	// end "if (!gDoneFlag)" 
				
			// Make certain that the About MultiSpec dialog box has been
			// displayed for 1.25 seconds before hiding it.								
	
	gNextStatusTime += 75;
	while (TickCount () < gNextStatusTime)
		{
		}
	/*
	if (errCode == noErr)
		{
		gNextStatusTime = TickCount () + 275;
		while (TickCount () < gNextStatusTime)
			{
			}
		}
	*/
			// Get the dialog list record handle.											
			
	HideWindow	(GetDialogWindow (dialogPtr));
	
	//#if TARGET_API_MAC_CARBON
		DisposeDialog (dialogPtr);
	/*#else
		handle = ((DialogPeek)dialogPtr)->items;
		CloseDialog (dialogPtr);
		UnlockAndDispose (dialogHandle);
		UnlockAndDispose (handle);
	#endif	// TARGET_API_MAC_CARBON else...
	*/
			// Show output text window															
	
	ShowWindow (gOutputWindow);
	
			// Read the simpler interface preference value
			
	SInt16 interfaceValue = 0;
	CFStringRef userInterfaceKey = CFSTR ("DefaultUserInterface");
	CFNumberRef userInterfaceValue = (CFNumberRef)CFPreferencesCopyAppValue (
											userInterfaceKey, kCFPreferencesCurrentApplication); 
	
	returnFlag = FALSE;
	if (userInterfaceValue != NULL)
      {
		returnFlag = CFNumberGetValue (
									userInterfaceValue, kCFNumberSInt16Type, &interfaceValue);
		
      CFRelease (userInterfaceValue);
         
      }	// end "if (userInterfaceValue != NULL)"
	
	gFullVersionFlag = TRUE;
	if (returnFlag)
		{
				// Interface preference value exists. Set the gFullVersionFlag global.
				
		if (interfaceValue == 2)
			gFullVersionFlag = FALSE;
		
		}	// end "if (returnFlag)"
		
	else	// The preference does not exist yet. Create it.
		WriteInterfacePreferenceValue (gFullVersionFlag);
			
	CFRelease (userInterfaceKey);
   
}	// end "InitializeMultiSpec" 

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeAppleEventParameters
//
//	Software purpose:	This routine intializes parameters that involve apple
//							events.  Currently, if there is an error the apple event
//							flag is turned off so that apple events are not used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			InitializeMultiSpec
//
//	Coded By:			Larry L. Biehl			Date: 08/23/93
//	Revised By:			Larry L. Biehl			Date: 09/09/93	

Boolean InitializeAppleEventParameters (void)

{
	ProcessSerialNumber				processSN;
	
	AEEventHandlerUPP					eventHandlerUPP;
	SInt16								returnCode;
	
	
			// Get Routine Description for Open Application.							
			
	//eventHandlerUPP = NewAEEventHandlerProc (HandleAEOpenApplication);
	eventHandlerUPP = NewAEEventHandlerUPP (HandleAEOpenApplication);
	
	if (eventHandlerUPP != NULL)
		returnCode = AEInstallEventHandler (kCoreEventClass,
														kAEOpenApplication,
														eventHandlerUPP,
														0,
														FALSE);
	
	if (eventHandlerUPP != NULL && returnCode == noErr)
		{								
		//eventHandlerUPP = NewAEEventHandlerProc (HandleAEOpenDocument);
		eventHandlerUPP = NewAEEventHandlerUPP (HandleAEOpenDocument);
	
		if (eventHandlerUPP != NULL)
			returnCode = AEInstallEventHandler (kCoreEventClass,
															kAEOpenDocuments,
															eventHandlerUPP,
															0,
															FALSE);
											
		}	// end "if (eventHandlerUPP != NULL && returnCode == noErr)" 
							
	if (eventHandlerUPP != NULL && returnCode == noErr)
		{		
		//eventHandlerUPP = NewAEEventHandlerProc (HandleAEPrintDocument);
		eventHandlerUPP = NewAEEventHandlerUPP (HandleAEPrintDocument);
	
		if (eventHandlerUPP != NULL)
			returnCode = AEInstallEventHandler (kCoreEventClass,
															kAEPrintDocuments,
															eventHandlerUPP,
															0,
															FALSE);
											
		}	// end "if (eventHandlerUPP != NULL && returnCode == noErr)" 
	
	if (eventHandlerUPP != NULL && returnCode == noErr)
		{									
		//eventHandlerUPP = NewAEEventHandlerProc (HandleAEQuitApplication);
		eventHandlerUPP = NewAEEventHandlerUPP (HandleAEQuitApplication);
	
		if (eventHandlerUPP != NULL)
			returnCode = AEInstallEventHandler (kCoreEventClass,
															kAEQuitApplication,
															eventHandlerUPP,
															0,
															FALSE);
											
		}	// end "if (eventHandlerUPP != NULL && returnCode == noErr)" 
	  
   		// Create a "null descriptor" to serve as a default container.			
   		
	gNullDesc.descriptorType = typeNull;
	gNullDesc.dataHandle = NIL;
	
			// Create an address descriptor for sending things to MultiSpec.		
			
	processSN.highLongOfPSN = 0;
	processSN.lowLongOfPSN = kCurrentProcess;
  
	if (eventHandlerUPP != NULL && returnCode == noErr)	
		returnCode = AECreateDesc (typeProcessSerialNumber,
											(Ptr)&processSN,
											sizeof (ProcessSerialNumber),
											&gSelfAddressDescription);
											
	if (returnCode != noErr)
		gAppleEventsFlag = FALSE;
		
	return (eventHandlerUPP == NULL);
	
}	// end "InitializeAppleEventParameters" 


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeDoubleVariables ()
//
//	Software purpose:	The purpose of this procedure is to initialize
//							the double variables in MultiSpec.  This routine is
// 						needed so that the coprocessor registers are not saved
//							on the stack at the beginning of "InitializeMultiSpec"
//							before the check is made if a coprocessor is available.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/04/1990
//	Revised By:			Larry L. Biehl			Date: 11/27/2000	

void InitializeDoubleVariables (void)
 
{
	
			// Initialize minimum magnification that is allowed.						
			// This is done here, because putting this line of code in 				
			// 'InitializeMultiSpec' causes coprocessor calls to be made before	
			// the check is made to determine if a coprocessor is available.		
			// The calls made are those to place the coprocessor registers on		
			// the stack upon entry into 'InitializeMultiSpec'.						

	gMinMagnification = 1/(double)gMaxMagnification;
	
	gLowerDeterminantOffsetAllowed = -35.;
	
			// Initialize the conversion factors from one distance unit to
			// another. This is used in displaying the coordinates in the coordinate
			// view. Everthing is relative to 1 mm.
			
	gDistanceFileConversionFactors[0] = 1000000.;				// km to mm
	gDistanceFileConversionFactors[1] = 1000.;					// m  to mm
	gDistanceFileConversionFactors[2] = 10.;						// cm to mm
	gDistanceFileConversionFactors[3] = 1.;						// mm to mm
	gDistanceFileConversionFactors[4] = 0.001;					// um to mm
	gDistanceFileConversionFactors[5] = 1609344.;				// mi to mm
	gDistanceFileConversionFactors[6] = 914.4;					// yd to mm
	gDistanceFileConversionFactors[7] = 304.800609601219;		// survey ft to mm
	gDistanceFileConversionFactors[8] = 304.8;					// ft to mm
	gDistanceFileConversionFactors[9] = 25.4;						// in to mm
	
			// Initialize the conversion factors from one distance unit to
			// another. This is used in displaying the coordinates in the coordinate
			// view. Everthing is relative to 1 mm.
			
	gDistanceDisplayConversionFactors[0] = 1000000.;		// km to mm
	gDistanceDisplayConversionFactors[1] = 100000.;			// sq root of hectare to mm
	gDistanceDisplayConversionFactors[2] = 1000.;			// m  to mm
	gDistanceDisplayConversionFactors[3] = 10.;				// cm to mm
	gDistanceDisplayConversionFactors[4] = 1.;				// mm to mm
	gDistanceDisplayConversionFactors[5] = 0.001;			// um to mm
	gDistanceDisplayConversionFactors[6] = 1609344.;		// mi to mm
	gDistanceDisplayConversionFactors[7] = 63614.90724;	// acre to mm
	gDistanceDisplayConversionFactors[8] = 914.4;			// yd to mm
	gDistanceDisplayConversionFactors[9] = 304.8;			// ft to mm
	gDistanceDisplayConversionFactors[10] = 25.4;			// in to mm
	
}	// end "InitializeDoubleVariables" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CreateOutputWindow
//
//	Software purpose:	The purpose of this routine is to create an output
//							window (text only) and allocate a structure that 
//							contains handles to other structures which will be
//							connected with the window and create the scroll bars
//							for the window.
//
//	Parameters in:		None
//
//	Parameters out:	Boolean variable indicating whether setup was okay.
//
// Value Returned:	Window pointer			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1988
//	Revised By:			Larry L. Biehl			Date: 02/16/2012	

WindowPtr CreateOutputWindow (
				Boolean*								windowSetupOK)

{
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							theWindow;
	
	Handle								fileInfoHandle,
											outputWindowRecordHandle,
											windowInfoHandle;
	
	Boolean								abortedFlag;
	
	
			// Start with aborted flag set to FALSE.										
	
	abortedFlag = FALSE;	
	
			// Initialize other parameters.													
	
	windowInfoHandle = NULL;
	
			// Create an output text window		
	
	//#if TARGET_API_MAC_CARBON
		outputWindowRecordHandle = NULL;
		theWindow = GetNewCWindow (kOutputWindowID, NULL, (WindowPtr)-1);
	/*#else
				// Get handle to window record.
					
		outputWindowRecordHandle = MNewHandle (sizeof (WindowRecord));
		if (outputWindowRecordHandle == NULL)
			abortedFlag = TRUE;
		
		else	// outputWindowRecordHandle != NULL 
			{
			MoveHHi (outputWindowRecordHandle);
			HLock (outputWindowRecordHandle);											
			
			theWindow = GetNewWindow (
									kOutputWindowID, *outputWindowRecordHandle, (WindowPtr)-1);
			
			}	// end "else outputWindowRecordHandle != NULL" 	
	#endif	// TARGET_API_MAC_CARBON else...	
	*/
			// Check if the window pointer is NULL.  If it is null then there		
			// isn't enought memory to store the window structure or the window	
			// resource couldn't be found.  If it is NULL, then exit the			
			// procedure.																			
	
	if (theWindow == NULL)		
		abortedFlag = TRUE;
		
			// Allocate WindowInfo record to store handle to structures and		
			// variables which provide additional information about the 			
			// output window																		
	
	if (!abortedFlag)
		{
		SetPortWindowPort (theWindow);
	
		windowInfoHandle = (Handle)MNewHandle (sizeof (WindowInfo));
		SetWRefCon (theWindow, (SInt32)windowInfoHandle);
		gOutputWindowInfoH = windowInfoHandle;
		
				// If windowInfoHandle is NULL then there isn't enough memory.		
				// Clean up at end and exit the procedure if there isn't enough	
				// memory.																			
		
		if (windowInfoHandle == NULL)		
			abortedFlag = TRUE;
		
		}	// end "if (!abortedFlag)" for windowInfoHandle 
	
			// Get a new handle to a block of memory to be used for file 			
			// information for the new image window.										
	
	if (!abortedFlag)
		{	
		fileInfoHandle = MNewHandle (sizeof (MFileInfo));
		if (fileInfoHandle == NULL)		
			abortedFlag = TRUE;
			
		}	// end "if (!abortedFlag)" 
		
	if (!abortedFlag)
		{
				// Initialize the variables and handles in the structure.			
		
		InitializeFileInfoStructure (fileInfoHandle, kNotPointer);
		
				// Save the handle to the text for the window, set 					
				// window type and the window menu item number.							
		
		HLock (windowInfoHandle);
		windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
	
				// Initialize the window information record.
			
		InitializeWindowInfoStructure ((Handle)windowInfoPtr,
													kPointer,
													fileInfoHandle,
													kNoImageType,
													kOutputWindowType);
			
				// Initialize items specific for text window information structure.
		
		windowInfoPtr->windowRecordHandle = outputWindowRecordHandle;
		windowInfoPtr->grafPortType = kGrafType;
		windowInfoPtr->windowMenuItem = kTextOutput;
		//windowInfoPtr->textH  = gOutputTextH;
	
		HUnlock (windowInfoHandle);
		
		#if !use_mlte_for_text_window
					// Create the scroll bar controls for the output window.	
			
			abortedFlag = !CreateOutputWControls (theWindow, FALSE);
		#endif	// !use_mlte_for_text_window
		
		}	// end "if (!abortedFlag)" 
		
			// Move the text window to the proper location
				
	//#if TARGET_API_MAC_CARBON
				// for OSX
		
		OSStatus 							status;
				
		Rect									rect,
												windowRect;
												
		GDHandle								currentDevice;
		
		SInt16								savedScreenRectLeft;
		
		
		GetWindowPortBounds (theWindow, &windowRect);				
		
		currentDevice = GetDeviceList ();	
					
		if (currentDevice != NULL)
			{
			status = GetAvailableWindowPositioningBounds (currentDevice, &rect);
			
			if (status == noErr)
				{
						// Allow for window title bar.
						
				rect.top += 20;
				
						// Allow for a few pixels around the window.
						
				InsetRect (&rect, 2, 4);
				savedScreenRectLeft = rect.left;
				rect.left = rect.right - (windowRect.right - windowRect.left) - 5;
				rect.left = MAX (rect.left, savedScreenRectLeft);
				
				MoveWindow (theWindow, rect.left, rect.top, FALSE);
				
				}	// end "if (status == noErr)"
			
			}	// end "if (currentDevice != NULL)"
			
		#if use_mlte_for_text_window
			EventTypeSpec commandEventType;
			
			status = InstallStandardEventHandler (GetWindowEventTarget (theWindow));
							
					// Install a handler to update the UI when this window comes to the front
			
			commandEventType.eventClass = kEventClassWindow;
			commandEventType.eventKind = kEventWindowFocusAcquired;
			
			if (status == noErr)
				status = InstallEventHandler (GetWindowEventTarget (theWindow), 
														NewEventHandlerUPP (TextWindowFocusAcquired), 
														1, 
														&commandEventType, 
														NULL, 
														NULL);
			/*
					// Install a handler to update the UI when this window has a mouse 
					// click in the content portion of the window.
			
			commandEventType.eventClass = kEventClassWindow;
			commandEventType.eventKind = kEventWindowHandleContentClick;
					
			if (status == noErr)
				status = InstallEventHandler (
													GetWindowEventTarget (theWindow),
													NewEventHandlerUPP (TextWindowContentClicked),
													1,
													&commandEventType,
													NULL,
													NULL);
			*/
			
			if (status != noErr)
				abortedFlag = TRUE;
		#endif	// use_mlte_for_text_window
	//#endif		// TARGET_API_MAC_CARBON
		
			// Set text font information.														
	
	if (!abortedFlag)
		{
		TextFont (gWindowTextFont); 			// monaco  
		TextSize (gOutputLineHeight-2);		// 9 point 
		
				// Reset the application window global variables. 						
		
		gTheActiveWindow = theWindow;
		gActiveWindowInfoH = windowInfoHandle;
		
		gWindowList[0] = theWindow;
		
		}	// end "if (!abortedFlag)" 
		
	if (abortedFlag)
		{
		UnlockAndDispose (windowInfoHandle);
		if (theWindow != NULL)	
			//#if TARGET_API_MAC_CARBON
				DisposeWindow (theWindow);
			//#else
			//	CloseWindow (theWindow);
			//#endif	// TARGET_API_MAC_CARBON else...
			
		outputWindowRecordHandle = UnlockAndDispose (outputWindowRecordHandle);
		theWindow = NULL;
		
		}	// end "if (abortedFlag)" 
	
	*windowSetupOK = !abortedFlag;
	
	gOutputWindowRecordHandle = outputWindowRecordHandle;
	
	return (theWindow);
	
}	// end "CreateOutputWindow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateOutputWControls
//
//	Software purpose:	This routine draws the scroll controls for the 
//							output window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateOutputWindow ()
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 07/26/2001			

Boolean CreateOutputWControls (
				WindowPtr							theWindow, 
				Boolean								visible)

{
			// Local Declarations	
			
	Rect									portRect;															 
			
	WindowInfoPtr						windowInfoPtr;
  
  	ControlHandle						theRootControl;
	Handle								windowInfoHandle;
	OSStatus								errorCode;
	SInt16								procID;
  	Boolean								returnFlag;
	SignedByte							handleStatus;
	
	
			// Initialize local variables.  Assume procedure finishies				
			// abnormally.																			
			
	returnFlag = FALSE;
	
	if (theWindow != NULL)
		{
		if (gAppearanceManagerFlag)
			{
					// Create the Root control		
					
			errorCode = CreateRootControl (theWindow, &theRootControl);
			if (errorCode != noErr)
																							return (FALSE);
																					
			}	// end "if (gAppearanceManagerFlag)"
																					
		windowInfoHandle = (Handle)GetWRefCon (theWindow);
		
		if (windowInfoHandle != NULL)
			{
			handleStatus = HGetState (windowInfoHandle);
			HLock (windowInfoHandle);
			windowInfoPtr = (WindowInfoPtr)*windowInfoHandle;
	
					// Compute clip area of window.													
					
			GetWindowClipRectangle (theWindow, kNoScrollBarArea, &gViewRect);
	
					// Create the vertical scroll bar control.							
					// Scale the vertical scroll bar to fit the window.				

			GetWindowPortBounds (theWindow, &portRect);
			gTempRect.top    = portRect.top - 1;
			gTempRect.left   = portRect.right - kSBarWidth;
			gTempRect.bottom = portRect.bottom - kSBarWidth + 1;
			gTempRect.right  = portRect.right + 1;	
					
					// Get the procedure id to be used.
					
			procID = scrollBarProc;
			if (gAppearanceManagerFlag)
				procID = kControlScrollBarLiveProc;
		
					// Make the vertical scroll bar											
					
			windowInfoPtr->verticalControl = NewControl (theWindow,
																		&gTempRect, 
																		"\p", 
																		visible, 
																		0, 
																		0, 
																		0, 
																		procID, 
																		kVerticalControl);
			if (windowInfoPtr->verticalControl == NULL)
																							return (FALSE);

					//	Attach a LongControl record to the vertical scroll bar. This allows 
					// us to use long settings and thus scroll text taller than 32,767 
					// pixels. LCAttach will not do anything if the appearance manager
					// is present. The appearance manager allows for 32 bit control 
					// values.
										
			if (LCAttach (windowInfoPtr->verticalControl) != noErr)
																							return (FALSE);
																					
					// Unhilite the control since there are no characters in the	
					// window yet.																	
					
			HiliteControl (windowInfoPtr->verticalControl, 255);
															
					// Create the horizontal scroll bar control							
					// Scale the horizontal scroll bar to fit the window.				
		
			gTempRect.top    = portRect.bottom - kSBarWidth;
			gTempRect.left   = portRect.left - 1;
			gTempRect.bottom = portRect.bottom + 1;
			gTempRect.right  = portRect.right - kSBarWidth + 1;
	
					// Set number of characters in longest line to the number that	
					// will fit in the output window width.  This assumes that the	
					// character width is 'defaultCharWidth' pixels.					
			
			gMaxCharsInLine = 1;
			
					// Set the maximum number of characters that are allowed in		
					// one line of text data.													
					
			gMaxCharsAllowedInLine = SHRT_MAX - 3;
			
					// Make the horizontal scroll bar.	
			
			windowInfoPtr->horizontalControl = NewControl (
																theWindow,
																&gTempRect,
																"\p", 
																visible, 
																0, 
																0,
																0, 
																//gMaxCharsInLine*kDefaultCharWidth,
																procID, 
																kHorizontalControl);
			if (windowInfoPtr->horizontalControl == NULL)
																							return (FALSE);

					//	Attach a LongControl record to the vertical scroll bar. This allows 
					// us to use long settings and thus scroll text taller than 32,767 
					// pixels. LCAttach will not do anything if the appearance manager
					// is present. The appearance manager allows for 32 bit control 
					// values.
						
			if (LCAttach (windowInfoPtr->horizontalControl) != noErr)
																							return (FALSE);
																					
					// Unhilite the control since there are no characters in the	
					// window yet.																	
					
			HiliteControl (windowInfoPtr->horizontalControl, 255);
	
					//	Show window controls.
			
			ShowControl (windowInfoPtr->verticalControl);	
			ShowControl (windowInfoPtr->horizontalControl);	
		
					// set clip region of window.  I.E. clip at scroll bar edges.	
		
			ClipRect (&gViewRect);
			
			returnFlag = TRUE;
	
			HSetState (windowInfoHandle, handleStatus);
			
			}	// end "if (windowInfoHandle != NULL)" 
			
		}	// end "if (theWindow != NULL)" 
		
	return (returnFlag);
	
}	// end "CreateOutputWControls"


/*
#if !TARGET_API_MAC_CARBON	
short int NumToolboxTraps ()

{
	if (NGetTrapAddress (_InitGraf, ToolTrap) == NGetTrapAddress (0xaa6e,ToolTrap))
		return (0x0200);
		
	else
		return (0x0400);

}	// end "NumToolboxTraps"



TrapType GetTrapType (
				short int							trapNumber)

{
	if ((trapNumber & 0x0800) > 0)
		return (ToolTrap);
		
	else
		return (OSTrap);

}	// end "GetTrapType" 
#endif	// !TARGET_API_MAC_CARBON
*/


#if use_mlte_for_text_window
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean InitializeTextEngine
//
//	Software purpose:	This routine initializes the Multilingual Text Engine that will
//							be used in the Universal version of MultiSpec.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2008
//	Revised By:			Larry L. Biehl			Date: 02/24/2012			

Boolean InitializeTextEngine (void)

{
	OSStatus								status;
	TXNControlTag						iControlTags[1];
	TXNControlData						iControlData[1];
	TXNMargins							txnMargins;
	//TXNTab								txnTab;
																  
	status = TXNCreateObject (
							NULL,
							//kTXNReadOnlyMask+kTXNMonostyledTextMask+kTXNWantHScrollBarMask+
							kTXNMonostyledTextMask+kTXNWantHScrollBarMask +
														kTXNWantVScrollBarMask+kTXNDrawGrowIconMask,
							&gTextObject);

	if (status == noErr)
		{
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)*gActiveWindowInfoH;
		windowInfoPtr->textH = gTextObject;
	
		gOutputTextH = gTextObject;
					
		iControlTags[0] = kTXNWordWrapStateTag;
		iControlData[0].uValue = kTXNNoAutoWrap;
		status = TXNSetTXNObjectControls (
											gTextObject, FALSE, 1, iControlTags, iControlData);
		
		}	// end "if (status == noErr)"

	if (status == noErr)
		{
		txnMargins.topMargin = 10;
		txnMargins.leftMargin = 10;
		txnMargins.bottomMargin = 0;
		txnMargins.rightMargin = 10;
		
		iControlTags[0] = kTXNMarginsTag;
		iControlData[0].marginsPtr = &txnMargins;
		status = TXNSetTXNObjectControls (
											gTextObject, FALSE, 1, iControlTags, iControlData);
		
		}	// end "if (status == noErr)"

	if (status == noErr)
		{
		iControlTags[0] = kTXNSingleLevelUndoTag;
		iControlData[0].uValue = TRUE;
		status = TXNSetTXNObjectControls (
											gTextObject, FALSE, 1, iControlTags, iControlData);
		
		}	// end "if (status == noErr)"

			// Tabs caused problems in the output text window. After there are so many,
			// the text in the window. It is truncated when displaying although the data
			// are still there for copying. They will reappear if one scrolls with that
			// text hidden. The text will go away again as soon as one changes the size
			// of the text window.
			//	Found note about this problem on the web. It is a problem in mlte. It has
			// a problem after 20 or number of tabs. Have not found a solution other than
			// converting to cocoa. 2/25/2012

	if (status == noErr)
		{		
		iControlTags[0] = kTXNTabSettingsTag;
		iControlData[0].tabValue.value = 20;		// 8 what what we started with here.
		iControlData[0].tabValue.tabType = kTXNLeftTab;		// kTXNLeftTab
		iControlData[0].tabValue.filler = 0;
		status = TXNSetTXNObjectControls (
											gTextObject, FALSE, 1, iControlTags, iControlData);
		
		}	// end "if (status == noErr)"

	if (status == noErr)
		{
		iControlTags[0] = kTXNDrawSelectionWhenInactiveTag;
		status = TXNGetTXNObjectControls (gTextObject, 1, iControlTags, iControlData);
		
		}	// end "if (status == noErr)"

	if (status == noErr)
		{
		iControlTags[0] = kTXNSelectionStateTag;
		status = TXNGetTXNObjectControls (gTextObject, 1, iControlTags, iControlData);
		
		}	// end "if (status == noErr)"
		
				// Set the font for the text object to an equal spaced font.
				
	if (status == noErr)
		status = SetOutputWindowFont (gTextObject);
	
	return (status == noErr);

}	// end "InitializeTextEngine"
#endif	// use_mlte_for_text_window


#if !use_mlte_for_text_window
#if !TARGET_RT_MAC_MACHO
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean InitializeTextEngine
//
//	Software purpose:	This routine initializes the word solution engine
//							software that is used in the output text window.
//							This code is not included in the 'InitializeMultiSpec'
//							routine because it framents the heap.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/13/91
//	Revised By:			Larry L. Biehl			Date: 07/19/2001			

Boolean InitializeTextEngine (void)

{
	PageMarginRec						pageMargins;
	LongRect								viewRect;
	Rect 									portRect;
	
	Handle								hPageMargins = NULL;
	
	//#if !TARGET_API_MAC_CARBON
	//	THPrint								hPrintRecord = NULL;
	//#endif	// TARGET_API_MAC_CARBON, else...
	
	OSStatus								errorReturn;
	
	
	#ifdef powerc
		/*
				// Load in word solution engine code resources for the power pc	
			
		wsBridgeHandle = (JmpHandle)Get1Resource ('wseb', 128);
		if (wsBridgeHandle == NULL)
																							return (FALSE);
													
		MoveHHi ((Handle)wsBridgeHandle);
		HLock ((Handle)wsBridgeHandle);
		
		gWSEBridgePtr = NewRoutineDescriptor (
											(ProcPtr)*wsBridgeHandle, uppWSEProcInfo, kM68kISA);
													
		if (gWSEBridgePtr == NULL)
																							return (FALSE);
		*/
	#endif
	

			// Initialize DataPak's Word Solution Engine.								
				
	//WSStartup (128, (ProcPtr)NULL);
	
	#ifdef powerc
		/*
		UniversalProcPtr errProccPtr = NewRoutineDescriptor (
					(ProcPtr)MyWSErrorProc,
					kPascalStackBased
		 					| RESULT_SIZE (SIZE_CODE (sizeof (Boolean)))
							| STACK_ROUTINE_PARAMETER (1, SIZE_CODE (sizeof (ProcPtr))),
					kPowerPCISA);
							
		WSSetErrProc ((ProcPtr)errProccPtr);
		*/
	#endif
	
	#ifndef powerc
		//WSSetErrProc ((ProcPtr)MyWSErrorProc);
	#endif
	
	gOutputAutoScrollActionPtr = NewWEScrollUPP (TextScrolled);
	
	if (gOutputAutoScrollActionPtr == NULL)
																							return (FALSE);
																				
	//_weNoWrapLineBreakProc = NewWELineBreakProc (_WENoWrapLineBreak);
	gWENoWrapLineBreakProcPtr = NewWELineBreakUPP (_WENoWrapLineBreak);
	
	
			// Resset routine to handle memory shortage.									
	
	SetGrowZone ((GrowZoneUPP)MyGrowZone);
	
	//MakeMonaco (&formatRecord);
	
			// Get a copy of the globals and set the minimum memory variable		
			// needed to do a text operation.  For now we will specify the			
			// maximum of 7 times the maximum text buffer size and 12000 bytes.	
	
	gTextMemoryMinimum = 0;		// For WASTE
	gTextMemoryMinimum = MAX (gTextMemoryMinimum, 12000);
	
			// Adjust default tab space to 1 character space width.					
			
	//wsGlobals.TabSpace = 6;
	//WSSetGlobals (&wsGlobals);
		
			// Set destination and view rectangles for text. This is temporary
			// for now.													
	/*
	viewRect.top = 0;
	viewRect.left = 0;
	viewRect.right = 100;
	viewRect.bottom = 100;
	
	bounds.top = 0;
	bounds.left = 5;
	bounds.right = gViewRect.right;
	bounds.bottom = gViewRect.bottom;
	*/
	//portRect = gOutputWindow->portRect;
	GetWindowPortBounds (gOutputWindow, &portRect);

	viewRect.top = 3;
	viewRect.left = 3;
	viewRect.bottom = portRect.bottom - kSBarWidth - 3;
	viewRect.right = portRect.right - kSBarWidth - 3;
	
	SetPortWindowPort (gOutputWindow);
	if (WENew (&viewRect,
					&viewRect,
					weDoAutoScroll |
						weDoOutlineHilite |
						weDoUndo |
						weDoMultipleUndo |
						weDoIntCutAndPaste |
						weDoDragAndDrop |
						weDoUseTempMem |
						weDoDrawOffscreen,
					&gOutputTextH))
																							return (FALSE);

			// If gOutputTextH is NULL, then there isn't enough memory.	Clean				
			// up at end and exit the routine if there isn't enough memory.		
		
	if (gOutputTextH == NULL)		
																							return (FALSE);

			//	Save a reference to the window in the WE instance
			
	errorReturn = WESetUserInfo (kWindowTag, 
											(SInt32)gOutputWindow, 
											gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);

			//	Set up our scroll callback
			
	errorReturn = WESetInfo (weScrollProc, 
										&gOutputAutoScrollActionPtr, 
										gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);

			//	Associate a default page margins record with the WE reference
			
	pageMargins.top = 72 << 16 ;			//	one inch
	pageMargins.bottom = 72 << 16 ;
	pageMargins.left = 72 << 16 ;
	pageMargins.right = 72 << 16 ;
	
	errorReturn = PtrToHand (&pageMargins, &hPageMargins, sizeof (pageMargins));
	if (errorReturn != noErr)
																							return (FALSE);
	
	errorReturn = WESetUserInfo (kPageMarginsTag, 
											(SInt32)hPageMargins, 
											gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);
	hPageMargins = NULL;

	errorReturn = WESetInfo (weLineBreakHook, 
										&gWENoWrapLineBreakProcPtr, 
										gOutputTextH);
	//errorReturn = WESetInfo (weLineBreakHook, NULL, gOutputTextH);
	if (errorReturn != noErr)
																							return (FALSE);
	/*
	#if !TARGET_API_MAC_CARBON		
				// Get print record for text window.
				
		hPrintRecord = InitPrinter ();
		if (hPrintRecord == NULL)		
																							return (FALSE);
																						
		if (WESetUserInfo ('PREC', (SInt32)hPrintRecord, gOutputTextH) != noErr)
																							return (FALSE);
	#endif	// !TARGET_API_MAC_CARBON
	*/
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)*gActiveWindowInfoH;
	windowInfoPtr->textH  = gOutputTextH;
	
	return (TRUE);
		
}	// end "InitializeTextEngine" 
#endif	// !TARGET_RT_MAC_MACHO
#endif	// !use_mlte_for_text_window

/*
#if !TARGET_API_MAC_CARBON	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitPrinter
//
//	Software purpose:	The purpose of this procedure is to open the printer 
//							driver and initialize the print globals.
//
//							NOTE.  Most of the code for this procedure came from
//							THINK's CAPPS editor construction kit.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			THINK C					Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 07/18/2001			

THPrint InitPrinter (void)

{
	THPrint								hPrint;
	
	
	gPrinterOK = FALSE;
		
	hPrint = (THPrint)NewHandleClear (sizeof (TPrint));
		
	if (gHPrint != NULL)
		{
		PrOpen ();
		
		if (PrError () == noErr)
			{
			PrintDefault (gHPrint);
			
			if (PrError () == noErr)
				gPrinterOK = TRUE;
		
			}	// end "if (PrError () == noErr)" 
		
		PrClose ();
		
		}	// end "if (hPrint != NULL)"
		
	return (hPrint); 
	
}	// end "InitPrinter" 
#endif	// !TARGET_API_MAC_CARBON
*/
	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean IsAppearanceManagerPresent
//
//	Software purpose:	The purpose of this routine is to determine if the appearance
//							manager is present.
//
//	Parameters in:		Flag indicating if gestalt is available
//
//	Parameters out:	None
//
// Value Returned:	Flag indicating if appearance manager is present.			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/31/2001
//	Revised By:			Larry L. Biehl			Date: 01/31/2001	

Boolean IsAppearanceManagerPresent (
				Boolean								gestaltAvailableFlag)

{
	OSStatus 							err = noErr;
	
	SInt32								response;
    
	Boolean								appearanceManagerFlag = FALSE;
    
    
	if (gestaltAvailableFlag)
		{

	    		// Attempt to call Gestalt; if we succeed, test for presence of 
	    		// Appearance Mgr
	    		
	   if (!(err = Gestalt (gestaltAppearanceAttr, &response)))
			appearanceManagerFlag = 
	        					response & (1 << gestaltAppearanceExists) ? TRUE : FALSE;
	        
	    		// If the Appearance Mgr selector is undefined, the Appearance Mgr is 
	    		// not present
	    		
	   else if (err == gestaltUndefSelectorErr)
	      appearanceManagerFlag = FALSE;
		
		}	// end "if (gestaltAvailableFlag)"

	return (appearanceManagerFlag);
    
}	// end "IsAppearanceManagerPresent"

	
/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MakeMonaco
//
//	Software purpose:	The purpose of this routine is to initialize
//							the format structure for DataPak's Word Solution
//							engine.  This code is from DataPak's example program.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/90
//	Revised By:			Larry L. Biehl			Date: 08/23/91	

void MakeMonaco (
				FormatPtr							formatRecordPtr)

{
	SInt16								x;
	

   formatRecordPtr->Pos = 0;
   formatRecordPtr->UserProcs = 0L;
   formatRecordPtr->fReal = 1;
   for (x=0; x<31; x++) 
   	formatRecordPtr->fName[x] = 0;
   	
   formatRecordPtr->fName[0] = 6;
   formatRecordPtr->fName[1] = 'M';
   formatRecordPtr->fName[2] = 'o';
   formatRecordPtr->fName[3] = 'n';
   formatRecordPtr->fName[4] = 'a';
   formatRecordPtr->fName[5] = 'c';
   formatRecordPtr->fName[6] = 'o';
   formatRecordPtr->fStyle = 0;
   formatRecordPtr->fPoint = FixRatio (gOutputLineHeight-2,1);
   formatRecordPtr->fgColor.red = 0;
   formatRecordPtr->fgColor.green = 0;
   formatRecordPtr->fgColor.blue = 0;
   formatRecordPtr->Txr = 0;
   formatRecordPtr->cExtra = (long) 0;
   formatRecordPtr->Leading = 0;
   formatRecordPtr->UserSpace = 0;
   
}	// end "MakeMonaco" 
*/

#if use_mlte_for_text_window	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetOutputWindowFont
//
//	Software purpose:	The purpose of this routine is to set the font and font size
//							for the MLTE style Output Window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2008
//	Revised By:			Larry L. Biehl			Date: 12/30/2008	

OSStatus SetOutputWindowFont (
				TXNObject							textObject)

{
	ATSUFontID							fontID;
	OSStatus								status;
	TXNTypeAttributes					attributes[] = { {kATSUFontTag, sizeof (ATSUFontID), {NULL}},
																  {kATSUSizeTag, sizeof (Fixed), {NULL}} };

		
				// Set the font for the text object to an equal spaced font.
		
	status = ATSUFindFontFromName ("Courier",		// Monaco or Courier
												strlen ("Courier"),
												kFontFullName,
												kFontMacintoshPlatform,	// kFontNoPlatformCode
												kFontRomanScript,			// kFontNoScriptCode
												kFontEnglishLanguage,	// kFontNoLanguageCode
												&fontID);
	
	if (status == noErr)
		{	
		attributes[0].data.dataValue = fontID;
		attributes[1].data.dataValue = Long2Fix (11);		
					
		//TXNSetReadOnly (textObject, FALSE);
		status = TXNSetTypeAttributes (textObject, 
													2, 
													attributes,
													kTXNStartOffset, 
													kTXNEndOffset);
		//TXNSetReadOnly (textObject, TRUE);
		
		}	// end "if (status == noErr)"
		
	return (status);
   
}	// end "SetOutputWindowFont"
#endif	// use_mlte_for_text_window
/*
#if !TARGET_API_MAC_CARBON	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Bolean TrapAvailable
//
//	Software purpose:	The purpose of these routines are to determine if
//							the given input trap is available.  These routines
//							came from inside Macintosh Volume VI.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/29/91
//	Revised By:			Larry L. Biehl			Date: 10/29/91			

Boolean TrapAvailable (
				SInt16								trapNumber)

{
	TrapType								tType;
	

	tType = GetTrapType (trapNumber);
	
	if (tType == ToolTrap)
		{
		trapNumber = (trapNumber & 0x07ff);
		if (trapNumber >= NumToolboxTraps ())
			trapNumber = _Unimplemented;
		
		}	// end "if (tType == ToolTrap)" 
		
	return (NGetTrapAddress (trapNumber, tType) != 
														NGetTrapAddress (_Unimplemented,ToolTrap));

}	// end "TrapAvailable" 
#endif	// !TARGET_API_MAC_CARBON	
*/
/*
#if use_mlte_for_text_window
// Got this routine from Multilingual Text Engine information at:
// http://www.carbondev.com/site/?page=Multilingual+Text+Engine+information+
//	Coded By:			xxxxxxxxxxxx			Date: xxxxxxxxxx
//	Revised By:			Larry L. Biehl			Date: 12/23/2008

OSStatus TXNSetReadOnly (
				TXNObject							inTextObject,
				Boolean								inIsReadOnly)
{
	TXNControlTag					tags[] = { kTXNNoUserIOTag };
	TXNControlData					data[] = { { inIsReadOnly ? kTXNReadOnly : kTXNReadWrite } };

	return TXNSetTXNObjectControls (inTextObject, FALSE, 1, tags, data);

	return (noErr);
	
}	// end "TXNSetReadOnly"
#endif	// use_mlte_for_text_window	
*/

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Bolean LoadCoreGraphicsVariables
//
//	Software purpose:	The purpose of this routine is to load the core graphics 
//							variables needed to run under the carbon version.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/14/2001
//	Revised By:			Larry L. Biehl			Date: 02/06/2007		

Boolean LoadCoreGraphicsVariables (void)

{
	Boolean 								isLoadedFlag = FALSE;
	
	//#if TARGET_API_MAC_CARBON
		OSErr 								theErr;
		

				// Create a bundle reference based on its name
				
		theErr = CreateFrameworkBundleFromName (CFSTR ("ApplicationServices.framework"),
																&gApplicationServicesBundle);
		
		if ((theErr == noErr) && (gApplicationServicesBundle != NULL))
			{
			isLoadedFlag = CFBundleLoadExecutable (gApplicationServicesBundle);
			
			if (isLoadedFlag)
				{
						// Lookup the function in the bundle by name
				
				gCGColorSpaceCreateDeviceGrayPtr =
						(CGColorSpaceCreateDeviceGrayPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle,
															CFSTR ("CGColorSpaceCreateDeviceGray"));
				
				if (gCGColorSpaceCreateDeviceGrayPtr != NULL)
					gCGColorSpaceCreateDeviceRGBPtr =
						(CGColorSpaceCreateDeviceRGBPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGColorSpaceCreateDeviceRGB"));
				
				if (gCGColorSpaceCreateDeviceRGBPtr != NULL)
					gCGColorSpaceReleasePtr =
							(CGColorSpaceReleasePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGColorSpaceRelease"));
				
				if (gCGColorSpaceReleasePtr != NULL)
					gCGColorSpaceCreateIndexedPtr =
							(CGColorSpaceCreateIndexedPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGColorSpaceCreateIndexed"));
				
				if (gCGColorSpaceCreateIndexedPtr != NULL)
					gCGContextAddLineToPointPtr =
							(CGContextAddLineToPointPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextAddLineToPoint"));
				
				if (gCGContextAddLineToPointPtr != NULL)
					gCGContextBeginPathPtr =
							(CGContextBeginPathPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextBeginPath"));
				
				if (gCGContextBeginPathPtr != NULL)
					gCGContextClipToRectPtr =
							(CGContextClipToRectPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextClipToRect"));
				
				if (gCGContextClipToRectPtr != NULL)
					gCGContextDrawImagePtr =
							(CGContextDrawImagePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextDrawImage"));
				
				if (gCGContextDrawImagePtr != NULL)
					gCGContextFlushPtr =
							(CGContextFlushPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextFlush"));
				
				if (gCGContextFlushPtr != NULL)
					gCGContextMoveToPointPtr =
							(CGContextMoveToPointPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextMoveToPoint"));
				
				if (gCGContextMoveToPointPtr != NULL)
					gCGContextScaleCTMPtr =
							(CGContextScaleCTMPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextScaleCTM"));
				
				if (gCGContextScaleCTMPtr != NULL)
					gCGContextReleasePtr =
							(CGContextReleasePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextRelease"));

				if (gCGContextReleasePtr != NULL)
					gCGContextRestoreGStatePtr =
							(CGContextRestoreGStatePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextRestoreGState"));
				
				if (gCGContextRestoreGStatePtr != NULL)
					gCGContextSaveGStatePtr =
							(CGContextSaveGStatePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextSaveGState"));
				
				if (gCGContextSaveGStatePtr != NULL)
					gCGContextTranslateCTMPtr =
							(CGContextTranslateCTMPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextTranslateCTM"));
				
				if (gCGContextTranslateCTMPtr != NULL)
					gCGContextSetAlphaPtr =
							(CGContextSetAlphaPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextSetAlpha"));
												
				if (gCGContextSetAlphaPtr != NULL)
					gCGContextSetInterpolationQualityPtr = 
							(CGContextSetInterpolationQualityPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextSetInterpolationQuality"));
												
				if (gCGContextSetInterpolationQualityPtr != NULL)
					gCGContextSetLineWidthPtr = 
							(CGContextSetLineWidthPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextSetLineWidth"));
												
				if (gCGContextSetLineWidthPtr != NULL)
					gCGContextSetRGBFillColorPtr = 
							(CGContextSetRGBFillColorPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextSetRGBFillColor"));
				
				if (gCGContextSetRGBFillColorPtr != NULL)
					gCGContextSetShouldAntialiasPtr =
							(CGContextSetShouldAntialiasPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextSetShouldAntialias"));
						
				if (gCGContextSetShouldAntialiasPtr != NULL)
					gCGContextStrokePathPtr = 
						(CGContextStrokePathPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGContextStrokePath"));
						
				if (gCGContextStrokePathPtr != NULL)
					gCGDataProviderCreateWithDataPtr = 
						(CGDataProviderCreateWithDataPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGDataProviderCreateWithData"));
						
				if (gCGDataProviderCreateWithDataPtr != NULL)
					gCGDataProviderReleasePtr =
							(CGDataProviderReleasePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGDataProviderRelease"));
				
				if (gCGDataProviderReleasePtr != NULL)
					gCGImageCreatePtr =
							(CGImageCreatePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGImageCreate"));
				
				if (gCGImageCreatePtr != NULL)
					gCGImageGetBytesPerRowPtr =
							(CGImageGetBytesPerRowPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGImageGetBytesPerRow"));
				
				if (gCGImageGetBytesPerRowPtr != NULL)
					gCGImageGetHeightPtr =
							(CGImageGetHeightPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGImageGetHeight"));
				
				if (gCGImageGetHeightPtr != NULL)
					gCGImageGetWidthPtr =
							(CGImageGetWidthPtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGImageGetWidth"));
				
				if (gCGImageGetWidthPtr != NULL)
					gCGImageMaskCreatePtr =
							(CGImageMaskCreatePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGImageMaskCreate"));
				
				if (gCGImageMaskCreatePtr != NULL)
					gCGImageReleasePtr =
							(CGImageReleasePtr)CFBundleGetFunctionPointerForName (
															gApplicationServicesBundle, 
															CFSTR ("CGImageRelease"));
																
				if (gCGImageReleasePtr == NULL)
					isLoadedFlag = FALSE;
				
				if (isLoadedFlag)
					{
							// Check if some 10.1 version routines are available
							
					gQDBeginCGContextPtr =
							(QDBeginCGContextPtr)CFBundleGetFunctionPointerForName (
																			gApplicationServicesBundle, 
																			CFSTR ("QDBeginCGContext"));
				
					gQDEndCGContextPtr =
							(QDEndCGContextPtr)CFBundleGetFunctionPointerForName (
																			gApplicationServicesBundle, 
																			CFSTR ("QDEndCGContext"));
							
					if (gQDBeginCGContextPtr == NULL || gQDEndCGContextPtr == NULL)
						gOSXPre10_1Flag = TRUE;
						
					}	// end "if (isLoadedFlag)"
												
				/*
				if (gCGImageReleasePtr != NULL)
					gCGRectMakePtr =
							(CGRectMakePtr)CFBundleGetFunctionPointerForName (
																			gApplicationServicesBundle,
																			CFSTR ("CGRectMake"));
				*/
				}	// end "if (isLoadedFlag)"
				
			}	// end "if ((theErr == noErr) && (theBundle != NULL))"
	//#endif	// TARGET_API_MAC_CARBON
		
	return (isLoadedFlag);

}	// end "TrapAvailable" 
