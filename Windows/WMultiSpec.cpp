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
//	File:						MultiSpec.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/19/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMultiSpecApp class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SDeclareGlobals.h"
#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#include "WGraphDoc.h"
#include "WGraphFrame.h"
#include "WGraphView.h"
#include "WImageDoc.h"
#include "WImageFrame.h"
#include "WImageView.h"
#include "WLegendView.h"
#include "WMainFrame.h"
#include "WMultiSpec.h"
#include "WOpenFileDialog.h"
#include "WStatisticsDoc.h"
#include "WStatisticsFrame.h"
#include "WStatisticsView.h"
#include "WTextDoc.h"
#include "WTextView.h"
#include "WTextFrame.h"
#include "WTools.h"

#include "IniReader.h"
#include "IniWriter.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMultiSpecApp, CWinApp)
	//{{AFX_MSG_MAP (CMultiSpecApp)
	ON_COMMAND (ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND (ID_APP_EXIT, OnAppExit)
	ON_COMMAND (ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	
		// Standard file based document commands
		
	ON_COMMAND (ID_FILE_NEW, CWinApp::OnFileNew)
	
		// Standard print setup command

	ON_COMMAND (ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP ()



CMultiSpecApp::CMultiSpecApp ()

{  
	TBYTE									string[256];

   LOGPEN 								logpen;
	
	int									numChars;
   
   Boolean								continueFlag;
   
   	
			// Initialize some global variables here.

	gAlgebraicTransformationFunction.denominatorString[0] = 0;
	gAlgebraicTransformationFunction.numeratorString[0] = 0;
	gAlgebraicTransformationFunction.transformFactor = 1;
	gAlgebraicTransformationFunction.transformOffset = 0;
	gAlgebraicTransformationFunction.instrumentCode = 0;
	gAlgebraicTransformationFunction.numberChannels = 0;

	gDisplaySpecsDefault.structureLoadedFlag = FALSE;
	gDisplaySpecsDefault.numberThematicLevels = 12;
	
	gTransformationMatrix.imageFileName[0] = 0;
	gTransformationMatrix.eigenValueHandle = NULL;
	gTransformationMatrix.eigenVectorHandle = NULL;
	gTransformationMatrix.eigenFeatureHandle = NULL;
	gTransformationMatrix.createdByCode = 0;
	gTransformationMatrix.numberFeatures = 0;
	gTransformationMatrix.eigenVectorPtr = NULL;
	gTransformationMatrix.tempMatrixPtr = NULL;
	gTransformationMatrix.eigenFeaturePtr = NULL;
	
	gNonProjProcessorSpecs.biPlotDataSpecsH = NULL; 
	gNonProjProcessorSpecs.clusterSpecsH = NULL; 
	gNonProjProcessorSpecs.listDataSpecsH = NULL; 
	gNonProjProcessorSpecs.listResultsSpecsH = NULL; 
	gNonProjProcessorSpecs.principalCompSpecsH = NULL; 
	gNonProjProcessorSpecs.statisticsImageSpecsH = NULL;    
	gNonProjProcessorSpecs.evalTransformationSpecsH = NULL;
	
	gResultsFileSummary.numberFiles = 0;
	gResultsFileSummary.diskFileListH = NULL;
	gResultsFileSummary.diskFileListPtr = NULL;
	
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
	gDistanceFileConversionFactors[7] = 304.800609601219;		// survery ft to mm
	gDistanceFileConversionFactors[8] = 304.8;					// ft to mm
	gDistanceFileConversionFactors[9] = 25.4;						// in to mm
	
			// Initialize the conversion factors from one distance unit to
			// another. This is used in displaying the coordinates in the coordinate
			// view. Everthing is relative to 1 mm.
			
	gDistanceDisplayConversionFactors[0] = 1000000.;				// km to mm
	gDistanceDisplayConversionFactors[1] = 100000.;					// sq root of hectare to mm
	gDistanceDisplayConversionFactors[2] = 1000.;					// m  to mm
	gDistanceDisplayConversionFactors[3] = 10.;						// cm to mm
	gDistanceDisplayConversionFactors[4] = 1.;						// mm to mm
	gDistanceDisplayConversionFactors[5] = 0.001;					// um to mm
	gDistanceDisplayConversionFactors[6] = 1609344.;				// mi to mm
	gDistanceDisplayConversionFactors[7] = 63614.90724;			// acre to mm
	gDistanceDisplayConversionFactors[8] = 914.4;					// yd to mm
	gDistanceDisplayConversionFactors[9] = 304.800609601219;		// survey ft to mm
	gDistanceDisplayConversionFactors[10] = 304.8;					// ft to mm
	gDistanceDisplayConversionFactors[11] = 25.4;					// in to mm
		
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

			// Maximum number of columns possible.
			// Note that the MaxRowBytes actually represents columns
			// Not sure what the max is; 250,000 display columns have worked.
		         
	gMaxRowBytes = 1000000;
	gMaxRowBytesFor24Bits = 1000000;
	
			// Initialize location parameters for first graph window, 
	   			
	gNextGraphicsWindowStart.h = 0; 
	gNextGraphicsWindowStart.v = 0; 
				
	gUseGWorldFlag = TRUE; 
	
	gBlinkProcedure = kBlink2; 
	
	m_dwSplashTime = 0;
	m_nextControlTime = 0;
	
	m_graphDocTemplatePtr = NULL;
	m_imageDocTemplatePtr = NULL;
	m_statisticsDocTemplatePtr = NULL;
	
	m_openImageFileInfoHandle = NULL;
	
	m_imageZoomCode = 0;
	
	m_controlDelayFlag = TRUE;

			// Get the separator to use for thousands and decimals
			// in numbers.

	numChars = GetNumberFormat (
					LOCALE_USER_DEFAULT, // locale for which string is to be formatted
					0,							// bit flag that controls the function's operation
					_T("1000.12"),       // pointer to input number string
					NULL,						// pointer to a formatting information structure
					(LPWSTR)string,		// pointer to output buffer
					100);						// size of output buffer

	if (numChars == 9)
		{
		gThousandsSeparator = (char)string[1];
		gDecimalSeparator = (char)string[5];

		}	// end "if (numChars == 9)"
	
			// Initialize some standard color pens 
   	               
	logpen.lopnStyle = PS_SOLID;
	logpen.lopnWidth.x = 1;
	logpen.lopnWidth.y = 1;
	
	gRGBColorList[0] = RGB (0, 0, 0);
	logpen.lopnColor = gRGBColorList[0];
	continueFlag = gBlackPen.CreatePenIndirect (&logpen);
		         
	gRGBColorList[1] = RGB (0, 0, 255);
	logpen.lopnColor = RGB (0, 0, 255);
	if (continueFlag)
		continueFlag = gBluePen.CreatePenIndirect (&logpen);
		         
	gRGBColorList[2] = RGB (0, 255, 255);
	logpen.lopnColor = RGB (0, 255, 255);
	if (continueFlag)
		continueFlag = gCyanPen.CreatePenIndirect (&logpen);
		         
	gRGBColorList[3] = RGB (0, 128, 0);
	logpen.lopnColor = RGB (0, 128, 0);
	if (continueFlag)
		continueFlag = gGreenPen.CreatePenIndirect (&logpen);
		         
	gRGBColorList[4] = RGB (255, 0, 255);
	logpen.lopnColor = RGB (255, 0, 255);
	if (continueFlag)
		continueFlag = gMagentaPen.CreatePenIndirect (&logpen);
		         
	gRGBColorList[5] = RGB (255, 0, 0);
	logpen.lopnColor = RGB (255, 0, 0);
	if (continueFlag)
		continueFlag = gRedPen.CreatePenIndirect (&logpen);
		         
	gRGBColorList[6] = RGB (255, 255, 255);
	logpen.lopnColor = RGB (255, 255, 255);
	if (continueFlag)
		continueFlag = gWhitePen.CreatePenIndirect (&logpen);
		         
	gRGBColorList[7] = RGB (255, 255, 0);
	logpen.lopnColor = RGB (255, 255, 0);
	if (continueFlag)
		continueFlag = gYellowPen.CreatePenIndirect (&logpen);

	InitializeDateVersionStrings ();
	
}	// end "CMultiSpecApp"


//------------------------------------------------------------------------------------
// The one and only CMspecApp object

CMultiSpecApp NEAR theApp;



Boolean CMultiSpecApp::DoIdle (
				SInt32								lCount)
				
{
			// Call base class idle first
			
	Boolean bResult = CWinApp::OnIdle (lCount);

	return bResult;
	
}	// end "DoIdle"



void CMultiSpecApp::ExitApplication ()

{
	OnAppExit ();

}	// end "ExitApplication"



int CMultiSpecApp::ExitInstance ()

{
	if (m_splash.m_hWnd != NULL)
		{
				// Problem occurred during start up. Close the splash window.
		
		m_splash.DestroyWindow ();
		m_pMainWnd->UpdateWindow ();
		
		}	// end "if (m_splash.m_hWnd != NULL)"

			// Release memory for non-project processor structures
	
	gProcessorCode = 0;
	ReleaseNonProjProcessorStructures ();
	
			// Dispose of the overlays
	
	while (gNumberShapeFiles > 0)
		{
		CloseOverlayFile (gNumberShapeFiles-1);
		
		}	// end "while (count > 0)"
	
	UnlockAndDispose (gShapeFilesHandle);
	
	if (gDebugFileStreamPtr != NULL)
		delete gDebugFileStreamPtr;
	
			// Close any GDAL interface classes.
	
	#if include_gdal_capability
		CloseGDALInterface ();
	#endif    // include_gdal_capability
	
	gDebugFileStreamPtr = NULL;
	
	gBlackPen.DeleteObject ();
	gBluePen.DeleteObject ();
	gCyanPen.DeleteObject ();
	gGreenPen.DeleteObject ();
	gMagentaPen.DeleteObject ();
	gRedPen.DeleteObject ();
	gWhitePen.DeleteObject ();
	gYellowPen.DeleteObject ();
	
	return (CWinApp::ExitInstance ());
	
}	// end "ExitInstance"


		// returns font height for the appropriate screen

long CMultiSpecApp::getFontHeightForDefaultDC (
				double								pointSize)		

{
			// this function can be optimized by using a simple cache on the point size (worth it in some situations!)

	HDC		hdc;

	hdc = ::GetDC (NULL);
	long height = getFontHeight (hdc, pointSize);
	::ReleaseDC (NULL, hdc);

	return (height);

}	// end "getFontHeightForDefaultDC"


long CMultiSpecApp::getFontHeight (
				HDC									hDC,
				double								pointSize)		// returns font height for the appropriate screen

{
	long	pixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
	long	lfHeight = (long)((pointSize * (double)pixPerInch) / 72.0);

	return lfHeight;
}

/*
long CMultiSpecApp::getFontHeight (
				CDC*									dc,			// can be NULL for default screen
				double								pointSize)	// returns font height for the appropriate screen

{
	if (dc == NULL)
		{
		return getFontHeightForDefaultDC (pointSize);
		}

	else
		{
		return getFontHeight (dc->GetSafeHdc(), pointSize);
		}

}	// end "getFontHeight"
*/

#ifdef _DEBUG
		 // Non-debug version is inline

CMultiDocTemplate* CMultiSpecApp::GetGraphDocTemplate ()

{
	return (CMultiDocTemplate*)m_graphDocTemplatePtr;
	
}	// end "GetGraphDocTemplate"


    // Non-debug version is inline

Handle CMultiSpecApp::GetOpenImageFileInfoHandle ()

{
	return m_openImageFileInfoHandle;
	
}	// end "GetOpenImageFileInfoHandle"


       // Non-debug version is inline

CMultiDocTemplate* CMultiSpecApp::GetStatisticsDocTemplate ()

{
	return (CMultiDocTemplate*)m_statisticsDocTemplatePtr;
	
}	// end "GetStatisticsDocTemplate"
#endif    //_DEBUG


SInt16 CMultiSpecApp::GetZoomCode (void)

{
	return (m_imageZoomCode);
	
}	// end "GetZoomCode"



BOOL CMultiSpecApp::InitInstance ()

{
	Rect									screenRect;

	
			// Standard initialization
			// If you are not using these features and wish to reduce the size
			//  of your final executable, you should remove from the following
			//  the specific initialization routines you do not need.

	//SetDialogBkColor ();        // Set dialog background color to gray
	LoadStdProfileSettings ();  	// Load standard INI file options (including MRU)
	
			// Get spare memory for displaying messages.
	
	gSpareWarnMemoryH = MNewHandle (gSpareWarnSize);
	if (gSpareWarnMemoryH == NULL)
																					return FALSE;

	
			// Get the setting for the user interface level.

	//setting = ConfigurationManager::AppSettings ("DefaultUserInterface");
	CString iniFileName = LPCTSTR (".\\");
	iniFileName += m_pszProfileName;
	CIniReader iniReader ((char*)(LPCTSTR)iniFileName);
	SInt16 userInterfaceValue =
							iniReader.ReadInteger ("Settings", "DefaultUserInterface", 1);

	gFullVersionFlag = TRUE;
	if (userInterfaceValue == 2)
		gFullVersionFlag = FALSE;

			// Register the application's document templates.  Document templates
			// serve as the connection between documents, frame windows and views.
	
			// Image Document Template
			// standard MDI child frame

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate (IDR_MSPECTYPE,
														RUNTIME_CLASS (CMImageDoc),
														RUNTIME_CLASS (CMImageFrame),
														RUNTIME_CLASS (CMImageView));
	if (pDocTemplate == NULL)
																						return FALSE;
	
	AddDocTemplate (pDocTemplate);
	m_imageDocTemplatePtr = pDocTemplate;
	
			// Text Output Document Template
			// Splitter child frame

	CMultiDocTemplate* pOutputDocTemplate;
	pOutputDocTemplate = new CMultiDocTemplate (IDR_OutType,
																RUNTIME_CLASS (CMTextDoc),
																RUNTIME_CLASS (CMTextFrame),
																RUNTIME_CLASS (CMTextView));
	if (pOutputDocTemplate == NULL)
																						return FALSE;
	
	AddDocTemplate (pOutputDocTemplate);
	
			// Graph Document Template
	
	CMultiDocTemplate* pGraphDocTemplate;
	pGraphDocTemplate = new CMultiDocTemplate (IDR_MSPECTYPE,
																RUNTIME_CLASS (CMGraphDoc),
																RUNTIME_CLASS (CMGraphFrame),
																RUNTIME_CLASS (CMGraphView));
	if (pGraphDocTemplate == NULL)
																						return FALSE;
	AddDocTemplate (pGraphDocTemplate);
	m_graphDocTemplatePtr = pGraphDocTemplate;
	
			// Statistics Document Template
	
	CMultiDocTemplate* pStatisticsDocTemplate;
	pStatisticsDocTemplate = new CMultiDocTemplate (IDR_MSPECTYPE,
																	RUNTIME_CLASS (CMStatisticsDoc),
																	RUNTIME_CLASS (CMStatisticsFrame),
																	RUNTIME_CLASS (CMStatisticsForm));
	if (pStatisticsDocTemplate == NULL)
																						return FALSE;
	AddDocTemplate (pStatisticsDocTemplate);
	m_statisticsDocTemplatePtr = pStatisticsDocTemplate;
	
			// create main MDI Frame window
	
	CMainFrame* pMainFrame = new CMainFrame;
	if (pMainFrame == NULL)
																						return FALSE;
	
	if (!pMainFrame->LoadFrame (IDR_MAINFRAME))
																						return FALSE;
	
	m_pMainWnd = pMainFrame;
	GetScreenRect (&screenRect);

			// Allow 200 pixels of space at the bottom of the window for other
			// window information like a help window from another application
			// such as that which Indiana State University likes to use.
			// Also limit the screen to no more than a 6 (width) to 4 (height)
			// ration so that the application window does not take up the entire
			// width of a wide screen monitor.

	SInt32				height,
							width;

	width = screenRect.right - screenRect.left + 1;
	height = screenRect.bottom - screenRect.top - 200;
	width = MIN (width, 6*height/4);

	m_pMainWnd->SetWindowPos (NULL,
										screenRect.left,
										screenRect.top,
										width,
										height,
										SWP_NOZORDER);
	m_pMainWnd->ShowWindow (m_nCmdShow);

			// Show splash screen

	if (!m_pMainWnd->IsIconic () && m_lpCmdLine[0] == 0 &&
											m_splash.Create (m_pMainWnd))
		{
		m_splash.ShowWindow (SW_SHOW);
		m_splash.UpdateWindow ();
		
		}	// if (!m_pMainWnd->IsIconic () && ...
	
	m_dwSplashTime = ::GetCurrentTime ();

		// enable file manager drag/drop and DDE Execute open
	
	EnableShellOpen ();
	RegisterShellFileTypes ();
	
			// open the output text window
	
	if (pOutputDocTemplate->OpenDocumentFile (NULL) == NULL)
																						return FALSE;

	m_pMainWnd->DragAcceptFiles ();
	
		 // The main window has been initialized, so show and update it.
	
	m_pMainWnd->UpdateWindow ();

	return TRUE;
	
}	// end "InitInstance"


		// App command to run the dialog

void CMultiSpecApp::OnAppAbout ()

{
	CAboutDlg* 							aboutDlgPtr = NULL;
	SInt32								returnCode;
	Boolean								savedFullVersionFlag;

	
	savedFullVersionFlag = gFullVersionFlag;
	TRY
		{
		aboutDlgPtr = new CAboutDlg ();
			
		Boolean continueFlag = aboutDlgPtr->DoDialog ();
		
		if (aboutDlgPtr != NULL)
			delete aboutDlgPtr;

				// Update the full version value in the ini file if the setting
				// has changed.

		if (gFullVersionFlag != savedFullVersionFlag)
			{
			CString iniFileName = LPCTSTR (".\\");
			iniFileName += m_pszProfileName;

					// Save the new setting to the ini file.
			
			if (gFullVersionFlag)
				returnCode = WritePrivateProfileSection (_T("Settings"),
																		_T("DefaultUserInterface=1"),
																		iniFileName);

			else	// !gFullVersionFlag
				returnCode = WritePrivateProfileSection (_T("Settings"),
																		_T("DefaultUserInterface=2"),
																		iniFileName);

			}	// end "if (gFullVersionFlag != savedFullVersionFlag)"
			
		}	// end "TRY"
	
	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		}
	
	END_CATCH_ALL
	
}	// end "OnAppAbout"



void CMultiSpecApp::OnAppExit ()

{
	CMImageView*						window;

	Handle								windowInfoHandle;

	UInt32								offset;

	SInt16								itemHit;

	Boolean								doneFlag;

	
			// Close the project if one is open.
	
	doneFlag = CloseTheProject ();
	
			// Check if any of the image windows need to have the class and/or
			// group information saved.
	
	if (doneFlag)
		{
				// Now, hide the image and graphics windows.
		
		for (offset=0; offset<(UInt32)gNumberOfIWindows; offset++)
			{
			window = gWindowList[kNumberBaseWindows+offset];
			
			windowInfoHandle = GetWindowInfoHandle (window);
			
			if (SaveIfWindowChanged (windowInfoHandle, FALSE))
				{
				doneFlag = FALSE;
				break;
				
				}	// end "if (SaveIfWindowChanged (..."
			
		   }	// end "for (offset=0; offset<..."
		
		}	// end "if (doneFlag)"
	
	if (doneFlag)
		{
				// Request user to save the output text window if it has not been saved
				// recently. If this is a forced quit by the application then the user
				// will have a chance to save the output window later on.
		
		Boolean windowChangedFlag = GetOutputWindowChangedFlag ();
		if (windowChangedFlag && gOutputViewCPtr->GetTextBufferLength () > 2)
			{
			if (LoadSpecifiedStringNumberStringP (kAlertStrID,
																IDS_Alert54,
																(char*)gTextString,
																(char*)gTextString2,
																TRUE,
																(char*)&gTextString3[1]))
				itemHit = DisplayAlert (kSaveAlertID, kStopAlert, 0, 0, 0, gTextString);
			
			if (itemHit == 1)
				doneFlag = gOutputViewCPtr->GetDocument()->SaveModified ();
				
			else if (itemHit == 2)
				doneFlag = FALSE;
			
			else
				gOutputViewCPtr->GetEditCtrl ().SetModify (FALSE);
			
			}	// end "if (windowChangedFlag && ...->GetTextBufferLength () > 2)"
		
		}	// end "if (doneFlag)"
	
	if (doneFlag)
		{
		//_CrtDumpMemoryLeaks ();

   	gProcessorCode = -1;
		
   	CWinApp::OnAppExit ();
		
   	}	// end "OnAppExit"

}	// end "OnAppExit"



void CMultiSpecApp::OnFileOpen (void)

{
	CDocument*							documentPtr;
	
	
	gProcessorCode = kOpenImageFileProcessor;
	OpenImageFileWin (NULL, &documentPtr, FALSE, 0);
	gProcessorCode = 0;
	
}	// end "OnFileOpen"



void CMultiSpecApp::OnFilePrintSetup (void)

{
	PRINTDLG								pd;
	Handle								windowInfoHandle;
	UInt16								pageOrientationSetting;
	BOOL									returnFlag;

	
	windowInfoHandle = GetActiveWindowInfoHandle ();

	pd.lStructSize = (DWORD)sizeof (PRINTDLG);
	returnFlag = GetPrinterDeviceDefaults (&pd);
	if (returnFlag)
		{
		pageOrientationSetting = GetPrintPageOrientation (windowInfoHandle);

		}	// end "if (returnFlag)"

	CWinApp::OnFilePrintSetup ();

	returnFlag = GetPrinterDeviceDefaults (&pd);
	if (returnFlag)
		{
		DEVMODE FAR* pDevMode = (DEVMODE FAR*)GlobalLock (m_hDevMode);
		pageOrientationSetting = pDevMode->dmOrientation;
		GlobalUnlock (m_hDevMode);

		SetPrintPageOrientation (windowInfoHandle, pageOrientationSetting);

		}	// end "if (returnFlag)"
	
}	// end "OnFilePrintSetup"



BOOL CMultiSpecApp::OnIdle (
				SInt32								lCount)
				
{
			// Call base class idle first
			
	BOOL bResult = CWinApp::OnIdle (lCount);

			// then do our work
			
	if (m_splash.m_hWnd != NULL)
		{
		if (::GetTickCount () - m_dwSplashTime > 1500)
			{ 
			
					// timeout expired, destroy the splash window
					
			m_splash.DestroyWindow ();
			m_pMainWnd->UpdateWindow ();

					// Don't set bResult to FALSE,
					// CWinApp::OnIdle may have returned TRUE
					
			}	// end "if (::GetCurrentTime () - m_dwSplashTime > 2500)"
			
		else	// ::GetCurrentTime () - m_dwSplashTime <= 2500
			{
					// check again later...
					
			bResult = TRUE;
			
			}	// end "else ::GetCurrentTime () - m_dwSplashTime <= 1500"
			
		}	// end "if (m_splash.m_hWnd != NULL)"
		
	if (m_imageZoomCode > 0)
		{
		if ((GetKeyState (VK_SHIFT) & 0x8000))
			m_controlDelayFlag = FALSE;
		else
			m_controlDelayFlag = TRUE;

		if (!m_controlDelayFlag || (GetTickCount () > m_nextControlTime))
			{
			m_nextControlTime = GetTickCount () + gControlOffset;
			                   
			m_pMainWnd->SendMessage (WM_COMMAND, m_imageZoomCode);    // send command
			
			}	// end "if (!m_controlDelayFlag || ..."
					
		bResult = TRUE;	// check again later...
		
		}	// end "if (m_imageZoomCode > 0)"
		
			// Check if a selection is being made in a window.
			
	if (gActiveImageViewCPtr != NULL)
		{
				// Send mouse move message if the mouse is captured to force
				// scrolling in case it is needed.
				
		if (gActiveImageViewCPtr->GetCapture () == gActiveImageViewCPtr)
			{     
			if (CMTool::c_nonClientAreaFlag)
				{ 						
				CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
			
				if (pTool != NULL)
					{
					pTool->OnMouseMove (gActiveImageViewCPtr, 0, CMTool::c_lastMousePoint);
										
					bResult = TRUE;	// check again later...

					}	// end "if (pTool != NULL)"
						
				}	// end "if (CMTool::c_nonClientAreaFlag) "
			
			}	// end "if (gActiveImageViewCPtr->GetCapture () == ..."
			
		}	// end "if (gActiveImageViewCPtr != NULL)"

	return bResult;
	
}	// end "OnIdle"



CDocument* CMultiSpecApp::OpenDocumentFile (
				LPCSTR								lpszFileName)

{
	CDocument* 							documentPtr = NULL;
	

	gProcessorCode = kOpenImageFileProcessor;
	OpenImageFileWin ((LocalAppFile*)lpszFileName, &documentPtr, FALSE, 0);
	gProcessorCode = 0;
	
	return (documentPtr);
	
}	// end "OpenDocumentFile"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 OpenImageFileWin
//
//	Software purpose:	The purpose of this routine is to open an image file
//							either multispectral type or Thematic (classification)
//							type.
//
//	Parameters in:		Pointer to structure giving information about the image
//								to use as the selected image file. If this pointer is
//								NULL, then present a dialog box to the use to allow
//								he/she to select the image file.
//
//	Parameters out:	None
//
//	Value Returned:	0 if selected file is not a project file
//							value > 0 indicating the project file version if the
//								selected file is not a project file.
//
// Called By:			GetImageFile in fileIO.c
//							OpenMultiSpecDocument in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1989
//	Revised By:			Larry L. Biehl			Date: 03/02/2013

SInt32 CMultiSpecApp::OpenImageFileWin (
				LocalAppFile*						localAppFilePtr,
				CDocument**							documentPtrPtr,
				Boolean								fromOpenProjectImageWindowFlag,
				UInt16								hdfDataSetSelection)

{
	SInt32								projectFileCode;


	projectFileCode = OpenImageFile (localAppFilePtr, FALSE, 0);

	return (projectFileCode);
	
}	// end "OpenImageFileWin"



void CMultiSpecApp::SetControlDelayFlag (
				Boolean								delayFlag)

{
	m_controlDelayFlag = delayFlag;
	
	if (m_imageZoomCode == 0)
		m_controlDelayFlag = TRUE;
	
}	// end "SetControlDelayFlag"


 
void CMultiSpecApp::SetPrintOrientation (
				Handle								windowInfoHandle)

{
	PRINTDLG								pd;
	UInt16								pageOrientationSetting;
	BOOL									returnFlag;

	
	pd.lStructSize = (DWORD)sizeof (PRINTDLG);
	returnFlag = GetPrinterDeviceDefaults (&pd);
	if (returnFlag)
		{	
		pageOrientationSetting = GetPrintPageOrientation (windowInfoHandle);

		DEVMODE FAR* pDevMode = (DEVMODE FAR*)GlobalLock (m_hDevMode);
		pDevMode->dmOrientation = pageOrientationSetting;
		GlobalUnlock (m_hDevMode);

		}	// end "if (returnFlag)"
	
}	// end "SetPrintOrientation"



Handle CMultiSpecApp::SetUpNewImageDocument (
				Handle								fileInfoHandle,
				SInt16								fileImageType,
				SInt16								windowType)
				
{   
	CDocument* 							documentPtr = NULL;
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;

	Handle								windowInfoHandle = NULL;
	
	SignedByte							handleStatus;

	Boolean								continueFlag;


	m_openImageFileInfoHandle = fileInfoHandle;
	
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (fileInfoHandle,
																			&handleStatus);
											
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr != NULL)
		{
		TBYTE* filePathPtr = (TBYTE*)fileStreamPtr->GetFilePathPPtr (kReturnUnicode); 
		documentPtr = m_imageDocTemplatePtr->OpenDocumentFile (
																		(LPCTSTR)&filePathPtr[1]);
															
		}	// end "if (fileStreamPtr != NULL)"
		
	MHSetState (fileInfoHandle, handleStatus);
	
	m_openImageFileInfoHandle = NULL;
	
	continueFlag = (documentPtr != NULL);
	
	if (continueFlag)
		{															
		CMImageWindow*	imageWindowCPtr = 
									((CMImageDoc*)documentPtr)->GetDocImageWindowCPtr ();
		if (imageWindowCPtr != NULL)
			windowInfoHandle = imageWindowCPtr->GetWindowInfoHandle ();
		continueFlag = (windowInfoHandle != NULL);
		
		}	// end "if (continueFlag)"

	if (continueFlag)
		{					
		SetImageType (windowInfoHandle, fileImageType);
		SetWindowType (windowInfoHandle, windowType);

		}	// end "if (continueFlag)"

	return (windowInfoHandle);
	
}	// end "SetUpNewImageDocument"


                        
void CMultiSpecApp::SetZoomCode (
				SInt16								zoomCode)
				
{                                                                     
	m_imageZoomCode = zoomCode;     
	
	if (m_imageZoomCode == 0) 
		m_nextControlTime = 0;
		
	else	// m_imageZoomCode != 0
		{ 
		if (GetKeyState (VK_RBUTTON) < 0)
			m_controlDelayFlag = FALSE;
			
		else    // GetKeyState (VK_RBUTTON) >= 0    
			m_controlDelayFlag = TRUE; 
		
		}	// end "else m_imageZoomCode != 0"
	
}	// end "SetZoomCode"
