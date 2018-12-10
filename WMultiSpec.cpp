// MultiSpec.cpp : Defines the class behaviors for the application.
// Revised By:		Larry L. Biehl		Date: 03/19/2018
//
                  
#include "SMultiSpec.h"

#include "CDisplay.h"      
#include "CFileStream.h"
#include "CImageWindow.h"

#include "WMultiSpec.h"
#include "WGraphView.h"
#include "WImageView.h" 
#include "WImageDoc.h"
#include "WImageFrame.h" 
#include "WStatisticsDoc.h"
#include "WStatisticsFrame.h"
#include "WStatisticsView.h"
#include "WTextDoc.h"
#include "WTextView.h"
#include "WTextFrame.h"
#include "WGraphDoc.h"
#include "WGraphFrame.h"
#include "WMainFrame.h"
#include "WOpenFileDialog.h"
#include	"WStatisticsDoc.h"
#include "WLegendView.h" 
#include "WTools.h"     

#include "SDeclareGlobals.h"

#include "IniReader.h"
#include "IniWriter.h"

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

//extern void						CloseGDALInterface (void);

extern SInt16	 				LoadHDFInformation (
										FileInfoPtr 						fileInfoPtr,
										Boolean								dataSetSpecifiedFlag);
/*
extern void						SetImageType(
										Handle 								windowInfoHandle,
										SInt16								imageType);

extern void						SetWindowType(
										Handle 								windowInfoHandle,
										SInt16								windowType);
*/

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiSpecApp

BEGIN_MESSAGE_MAP(CMultiSpecApp, CWinApp)
	//{{AFX_MSG_MAP(CMultiSpecApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen) 
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
	
		// Standard file based document commands
		
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	
		// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMspecApp construction

CMultiSpecApp::CMultiSpecApp()
{  
	TBYTE										string[256];

   LOGPEN 									logpen;
	
	int										numChars;
   
   Boolean									continueFlag;
   
   	
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
		         
//	#if !defined _WIN32
//		gMaxRowBytes = 8188; 
//		gMaxRowBytesFor24Bits = 21844;
//	#endif	// !defined _WIN32 
		         
//	#if defined _WIN32           
		gMaxRowBytes = 32767; 
		gMaxRowBytesFor24Bits = 32767;
//	#endif	// defined _WIN32 
	
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

	numChars = GetNumberFormat(
			LOCALE_USER_DEFAULT, // locale for which string is to be formatted
			0,							// bit flag that controls the function's operation
			_T("1000.12"),       // pointer to input number string
			NULL,						// pointer to a formatting information structure
			#if defined _UNICODE
				(LPWSTR)string,					// pointer to output buffer
			#endif
			#if !defined _UNICODE
				(LPSTR)string,					// pointer to output buffer
			#endif
			100);						// size of output buffer

	if (numChars == 9)
		{
		gThousandsSeparator = (char)string[1];
		gDecimalSeparator = (char)string[5];

		}		// end "if (numChars == 9)"
	
			// Initialize some standard color pens 
   	               
	logpen.lopnStyle = PS_SOLID;
	logpen.lopnWidth.x = 1;
	logpen.lopnWidth.y = 1;
	
	gRGBColorList[0] = RGB(0, 0, 0);
	logpen.lopnColor = gRGBColorList[0];
	continueFlag = gBlackPen.CreatePenIndirect(&logpen);
		         
	gRGBColorList[1] = RGB(0, 0, 255);
	logpen.lopnColor = RGB(0, 0, 255);
	if (continueFlag)
		continueFlag = gBluePen.CreatePenIndirect(&logpen);
		         
	gRGBColorList[2] = RGB(0, 255, 255);
	logpen.lopnColor = RGB(0, 255, 255);
	if (continueFlag)
		continueFlag = gCyanPen.CreatePenIndirect(&logpen);  
		         
	gRGBColorList[3] = RGB(0, 128, 0);
	logpen.lopnColor = RGB(0, 128, 0);
	if (continueFlag)
		continueFlag = gGreenPen.CreatePenIndirect(&logpen);   
		         
	gRGBColorList[4] = RGB(255, 0, 255);
	logpen.lopnColor = RGB(255, 0, 255);
	if (continueFlag)
		continueFlag = gMagentaPen.CreatePenIndirect(&logpen);    
		         
	gRGBColorList[5] = RGB(255, 0, 0);
	logpen.lopnColor = RGB(255, 0, 0);
	if (continueFlag)
		continueFlag = gRedPen.CreatePenIndirect(&logpen);     
		         
	gRGBColorList[6] = RGB(255, 255, 255);
	logpen.lopnColor = RGB(255, 255, 255);
	if (continueFlag)
		continueFlag = gWhitePen.CreatePenIndirect(&logpen);     
		         
	gRGBColorList[7] = RGB(255, 255, 0);
	logpen.lopnColor = RGB(255, 255, 0);
	if (continueFlag)
		continueFlag = gYellowPen.CreatePenIndirect(&logpen); 

	InitializeDateVersionStrings ();
	
}		// end "CMultiSpecApp"

/////////////////////////////////////////////////////////////////////////////
// The one and only CMspecApp object

CMultiSpecApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CMspecApp initialization

BOOL 
CMultiSpecApp::InitInstance()
{
	Rect								screenRect;

	//char*								initFile;

	
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.

//	SetDialogBkColor();        // Set dialog background color to gray
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	
			// Get spare memory for displaying messages. 
	
	gSpareWarnMemoryH = MNewHandle (gSpareWarnSize);
	if (gSpareWarnMemoryH == NULL)              
																					return FALSE;

	
			// Get the setting for the user interface level.

	//setting = ConfigurationManager::AppSettings("DefaultUserInterface");
	CString iniFileName = LPCTSTR(".\\");
	iniFileName += m_pszProfileName;
	CIniReader iniReader((char*)(LPCTSTR)iniFileName);
	SInt16 userInterfaceValue = iniReader.ReadInteger ("Settings", "DefaultUserInterface", 1);

	gFullVersionFlag = TRUE;
	if (userInterfaceValue == 2)
		gFullVersionFlag = FALSE;
/*																					
	gDebugFileStreamPtr = new CMFileStream("\0mu_debug.txt");   
	if (gDebugFileStreamPtr == NULL)              
																					return FALSE;
																					
	gDebugFileStreamPtr->MCreateNewFile ();
	if (gDebugFileStreamPtr->mFileType == -1)              
																					return FALSE;
	
	SInt16 outputCode = kAsciiFormatCode;																			
	ListProcessorTitleLine(gDebugFileStreamPtr, &outputCode, TRUE);																				
*/
		// Register the application's document templates.  Document templates
		//  serve as the connection between documents, frame windows and views.
		
			// Image Document Template

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MSPECTYPE,                
		RUNTIME_CLASS(CMImageDoc),
		RUNTIME_CLASS(CMImageFrame),        // standard MDI child frame
		RUNTIME_CLASS(CMImageView)); 
	if (pDocTemplate == NULL)
																					return FALSE;
	AddDocTemplate(pDocTemplate);
	m_imageDocTemplatePtr = pDocTemplate;  
		
			// Text Output Document Template

	CMultiDocTemplate* pOutputDocTemplate;
	pOutputDocTemplate = new CMultiDocTemplate(
		IDR_OutType,             
//		IDR_MSPECTYPE,                
		RUNTIME_CLASS(CMTextDoc),    
		RUNTIME_CLASS(CMTextFrame),        // Splitter child frame 
		RUNTIME_CLASS(CMTextView));
	if (pOutputDocTemplate == NULL)
																					return FALSE;
	AddDocTemplate(pOutputDocTemplate);  
		
			// Graph Document Template
                                      
	CMultiDocTemplate* pGraphDocTemplate;    
	pGraphDocTemplate = new CMultiDocTemplate(
		IDR_MSPECTYPE,                
		RUNTIME_CLASS(CMGraphDoc),    
		RUNTIME_CLASS(CMGraphFrame), 
		RUNTIME_CLASS(CMGraphView));
	if (pGraphDocTemplate == NULL)
																					return FALSE;
	AddDocTemplate(pGraphDocTemplate);
	m_graphDocTemplatePtr = pGraphDocTemplate;  
		
			// Statistics Document Template  
                                      
	CMultiDocTemplate* pStatisticsDocTemplate;    
	pStatisticsDocTemplate = new CMultiDocTemplate(
		IDR_MSPECTYPE,                
		RUNTIME_CLASS(CMStatisticsDoc),    
		RUNTIME_CLASS(CMStatisticsFrame), 
		RUNTIME_CLASS(CMStatisticsForm));
	if (pStatisticsDocTemplate == NULL)
																					return FALSE;
	AddDocTemplate(pStatisticsDocTemplate);
	m_statisticsDocTemplatePtr = pStatisticsDocTemplate;    
		
			// Legend Document Template  
/*                                      
	CMultiDocTemplate* pLegendDocTemplate;    
	pLegendDocTemplate = new CMultiDocTemplate(
		IDR_MSPECTYPE,                
		RUNTIME_CLASS(CMImageDoc),    
		RUNTIME_CLASS(CMImageFrame), 
		RUNTIME_CLASS(CMLegendView));
	if (pLegendDocTemplate == NULL)
																					return FALSE;
	AddDocTemplate(pLegendDocTemplate);
//	m_legendDocTemplatePtr = pLegendDocTemplate;
*/	
		// create main MDI Frame window
		
	CMainFrame* pMainFrame = new CMainFrame;
	if (pMainFrame == NULL)
																					return FALSE;
		
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
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
	m_pMainWnd->ShowWindow(m_nCmdShow);
//	m_pMainWnd->ShowWindow(SW_SHOWNORMAL); 
//	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);       
//	m_pMainWnd->UpdateWindow();

		// Show splash screen                       

	if (!m_pMainWnd->IsIconic() && m_lpCmdLine[0] == 0 &&
											m_splash.Create(m_pMainWnd))
		{
		m_splash.ShowWindow(SW_SHOW);
		m_splash.UpdateWindow();
		
		}		// if (!m_pMainWnd->IsIconic() && ...
		
	m_dwSplashTime = ::GetCurrentTime();      

		// enable file manager drag/drop and DDE Execute open
		
	EnableShellOpen();
	RegisterShellFileTypes(); 
	
		// open the output text window 
		
	if (pOutputDocTemplate->OpenDocumentFile(NULL) == NULL)
																					return FALSE;

		// simple command line parsing
/*	
	if (m_lpCmdLine[0] != '\0')
		{                                     
			// open an existing document
			
		pDocTemplate->OpenDocumentFile(m_lpCmdLine);
		
		}	// end "if (m_lpCmdLine[0] != '\0')"
 */
	m_pMainWnd->DragAcceptFiles();
	
		// The main window has been initialized, so show and update it.
	
//	m_pMainWnd->ShowWindow(m_nCmdShow);
//	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);       
	m_pMainWnd->UpdateWindow(); 

	return TRUE;
	
}	// end "InitInstance"



Boolean 
CMultiSpecApp::DoIdle(
				SInt32			lCount)
				
{
			// call base class idle first
			
	Boolean bResult = CWinApp::OnIdle(lCount);

	return bResult;
	
}		// end "DoIdle"



BOOL 
CMultiSpecApp::OnIdle(
				SInt32			lCount)
				
{
			// call base class idle first
			
	BOOL bResult = CWinApp::OnIdle(lCount);

			// then do our work
			
	if (m_splash.m_hWnd != NULL)
		{
		if (::GetTickCount() - m_dwSplashTime > 1500)
			{ 
			
					// timeout expired, destroy the splash window
					
			m_splash.DestroyWindow();
			m_pMainWnd->UpdateWindow();

					// NOTE: don't set bResult to FALSE,
					//  CWinApp::OnIdle may have returned TRUE
					
			}		// end "if (::GetCurrentTime() - m_dwSplashTime > 2500)"
			
		else		// ::GetCurrentTime() - m_dwSplashTime <= 2500 
			{
					// check again later...
					
			bResult = TRUE;
			
			}		// end "else ::GetCurrentTime() - m_dwSplashTime <= 1500"
			
		}		// end "if (m_splash.m_hWnd != NULL)"
		
	if (m_imageZoomCode > 0)
		{                                               
		if (!m_controlDelayFlag || (GetTickCount() > m_nextControlTime) )
			{
			m_nextControlTime = GetTickCount() + gControlOffset;
			                   
			m_pMainWnd->SendMessage(WM_COMMAND, m_imageZoomCode);    // send command 
			
			}		// end "if (!m_controlDelayFlag || ..." 
					
		bResult = TRUE;		// check again later... 
		
		}		// end "if (m_imageZoomCode > 0)"
		
			// Check if a selection is being made in a window.
			
	if (gActiveImageViewCPtr != NULL)
		{
				// Send mouse move message if the mouse is captured to force
				// scrolling in case it is needed.
				
		if (gActiveImageViewCPtr->GetCapture() == gActiveImageViewCPtr)
			{     
			if ( CMTool::c_nonClientAreaFlag )              
				{ 						
				CMTool* pTool = CMTool::FindTool(CMTool::c_toolType); 
			
				if (pTool != NULL)
					{
					pTool->OnMouseMove(gActiveImageViewCPtr, 0, CMTool::c_lastMousePoint);
										
					bResult = TRUE;		// check again later...  

					}		// end "if (pTool != NULL)"
						
				}		// end "if ( CMTool::c_nonClientAreaFlag ) "
			
			}		// end "if (gActiveImageViewCPtr->GetCapture() == ..."   
			
		}		// end "if (gActiveImageViewCPtr != NULL)"

	return bResult;
	
}		// end "OnIdle"

#ifdef _DEBUG
                        
CMultiDocTemplate* 
CMultiSpecApp::GetGraphDocTemplate() // non-debug version is inline
	{
//	ASSERT(m_graphDocTemplatePtr->IsKindOf(RUNTIME_CLASS(CMultiSpecApp)));
	return (CMultiDocTemplate*)m_graphDocTemplatePtr;
	
	}		// end "GetGraphDocTemplate"
	
	
                        
Handle 
CMultiSpecApp::GetOpenImageFileInfoHandle() // non-debug version is inline
	{
//	ASSERT(m_graphDocTemplatePtr->IsKindOf(RUNTIME_CLASS(CMultiSpecApp)));
	return m_openImageFileInfoHandle;
	
	}		// end "GetOpenImageFileInfoHandle"
	
	
                        
CMultiDocTemplate* 
CMultiSpecApp::GetStatisticsDocTemplate() // non-debug version is inline
	{
//	ASSERT(m_graphDocTemplatePtr->IsKindOf(RUNTIME_CLASS(CMultiSpecApp)));
	return (CMultiDocTemplate*)m_statisticsDocTemplatePtr;
	
	}		// end "GetStatisticsDocTemplate"

#endif //_DEBUG   

 
void 
CMultiSpecApp::SetPrintOrientation (
				Handle								windowInfoHandle)
	{
	PRINTDLG						pd;
	UInt16						pageOrientationSetting;
	BOOL							returnFlag;

	
	pd.lStructSize = (DWORD)sizeof (PRINTDLG);
	returnFlag = GetPrinterDeviceDefaults (&pd);
	if (returnFlag)
		{	
		pageOrientationSetting = GetPrintPageOrientation (windowInfoHandle);

		DEVMODE FAR* pDevMode = (DEVMODE FAR*)GlobalLock (m_hDevMode);
		pDevMode->dmOrientation = pageOrientationSetting;
		GlobalUnlock (m_hDevMode);

		}		// end "if (returnFlag)"
	
	}		// end "SetPrintOrientation" 



Handle 
CMultiSpecApp::SetUpNewImageDocument(
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
																				&handleStatus,
																				kNoMoveHi);
											
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr != NULL)
		{
		TBYTE* filePathPtr = (TBYTE*)fileStreamPtr->GetFilePathPPtr (kReturnUnicode); 
		documentPtr = m_imageDocTemplatePtr->OpenDocumentFile(
															(LPCTSTR)&filePathPtr[1]);
															
		}		// end "if (fileStreamPtr != NULL)"
		
	MHSetState (fileInfoHandle, handleStatus);
	
	m_openImageFileInfoHandle = NULL;
	
	continueFlag = (documentPtr != NULL);
	
	if (continueFlag)
		{															
		CMImageWindow*	imageWindowCPtr = 
						((CMImageDoc*)documentPtr)->GetDocImageWindowCPtr();
		if (imageWindowCPtr != NULL)
			windowInfoHandle = imageWindowCPtr->GetWindowInfoHandle();
		continueFlag = (windowInfoHandle != NULL);
		
		}		// end "if (continueFlag)"

	if (continueFlag)
		{					
		SetImageType (windowInfoHandle, fileImageType);
		SetWindowType (windowInfoHandle, windowType);

		}		// end "if (continueFlag)"

	return (windowInfoHandle);
	
	}		// end "SetUpNewImageDocument"


                        
void 
CMultiSpecApp::SetZoomCode(
				SInt16								zoomCode)
				
{                                                                     
	m_imageZoomCode = zoomCode;     
	
	if (m_imageZoomCode == 0) 
		m_nextControlTime = 0;
		
	else		// m_imageZoomCode != 0
		{ 
		if (GetKeyState (VK_RBUTTON) < 0)
			m_controlDelayFlag = FALSE;
			
		else		// GetKeyState (VK_RBUTTON) >= 0    
			m_controlDelayFlag = TRUE; 
		
		}		// end "else m_imageZoomCode != 0"
	
}		// end "SetZoomCode"
 
                        
void 
CMultiSpecApp::SetControlDelayFlag(                
				Boolean								delayFlag)
				
{                              
	m_controlDelayFlag = delayFlag;
	
	if (m_imageZoomCode == 0)  
		m_controlDelayFlag = TRUE; 
	
}		// end "SetControlDelayFlag"
 
                        
SInt16 
CMultiSpecApp::GetZoomCode(void)
				
{                                                                     
	return (m_imageZoomCode);
	
}		// end "SetZoomCode"

/*
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CMDialog
{
	public:
		CAboutDlg(CWnd* pParent = NULL);
	
			// Dialog Data
		//{{AFX_DATA(CAboutDlg)
		enum { IDD = IDD_ABOUTBOX };
		//}}AFX_DATA
	    	
		// Implementation
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
		
		DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL)
	: CMDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}


void 
CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CMDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/
	// App command to run the dialog
void 
CMultiSpecApp::OnAppAbout()
{
	//char								sectionNames[256];
	CAboutDlg* 						aboutDlgPtr = NULL;
	SInt32							//errCode,
										returnCode;
	Boolean							savedFullVersionFlag;

	
	savedFullVersionFlag = gFullVersionFlag;
	TRY
		{ 
		aboutDlgPtr = new CAboutDlg(); 
			
		Boolean continueFlag = aboutDlgPtr->DoDialog (); 
		
		if (aboutDlgPtr != NULL)                       
			delete aboutDlgPtr;

				// Update the full version value in the ini file if the setting has changed.

		if (gFullVersionFlag != savedFullVersionFlag)
			{
			CString iniFileName = LPCTSTR(".\\");
			iniFileName += m_pszProfileName;

					// Save the new setting to the ini file.
			
			if (gFullVersionFlag)
				returnCode = WritePrivateProfileSection (_T("Settings"), _T("DefaultUserInterface=1"), iniFileName);

			else	// !gFullVersionFlag
				returnCode = WritePrivateProfileSection (_T("Settings"), _T("DefaultUserInterface=2"), iniFileName);

//			if (returnCode == 0)
//				errCode = GetLastError();

			}	// end "if (gFullVersionFlag != savedFullVersionFlag)"
		}
			
	CATCH_ALL(e)
		{
		MemoryMessage(0, kObjectMessage);
		}
	END_CATCH_ALL 
	
}
/*
/////////////////////////////////////////////////////////////////////////////
// CMultiSpecApp commands

BOOL CAboutDlg::OnInitDialog()
{                       
	CString str, strFmt;
	
	  
	CDialog::OnInitDialog();    

			// fill available memory
			               
	strFmt.LoadString(IDS_AboutMemory);
	wsprintf(str.GetBuffer(80), strFmt, ::GetFreeSpace(0) / 1024L);
	str.ReleaseBuffer();
	SetDlgItemText(IDC_AboutMemory, str);    

			// fill largest contiguous block of memory parameter
			               
	strFmt.LoadString(IDS_AboutContiguousMemory);
	wsprintf(str.GetBuffer(80), strFmt, ::GlobalCompact(0) / 1024L);
	str.ReleaseBuffer();
	SetDlgItemText(IDC_AboutContiguousMemory, str);

			// fill math coprocessor information
			
	if (GetWinFlags() & WF_80x87)
		str.LoadString(IDS_AboutCoprocessor);
	else
		str.LoadString(IDS_AboutNoCoprocessor);
	SetDlgItemText(IDC_AboutCoprocessor, str);

			// fill disk free information
			
	struct _diskfree_t diskfree;
	if (_dos_getdiskfree(_getdrive(), &diskfree) == 0)
		{
		strFmt.LoadString(IDS_AboutDiskSpace);
		wsprintf(str.GetBuffer(80), strFmt,
			(DWORD)diskfree.avail_clusters *
			(DWORD)diskfree.sectors_per_cluster *
			(DWORD)diskfree.bytes_per_sector / (DWORD)1024L);
		str.ReleaseBuffer();
		
		}
		
	else
		str.LoadString(IDS_AboutNoDiskSpace);
		
	SetDlgItemText(IDC_AboutDisk, str);              
	
			// Set color information about the display 
			
	CDC				pDC;                              
	
	if ( pDC.CreateIC("DISPLAY", NULL, NULL, NULL) )
		{                        
		
		SInt16 bitsPerPixel = pDC.GetDeviceCaps(BITSPIXEL);
		SetDlgItemInt(IDC_AboutBitsColor, bitsPerPixel);     
		
		SInt16 colorPlanes = pDC.GetDeviceCaps(PLANES);
		SetDlgItemInt(IDC_AboutColorPlanes, colorPlanes);
		
		}		// end "if ( pDC->CreateIC("Display", NULL, NULL, NULL) )"
	
	PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"
*/           
  

void 
CMultiSpecApp::OnFileOpen(void)

{  
	CDocument*		documentPtr;
	 
	gProcessorCode = kOpenImageFileProcessor;
	OpenImageFileWin (NULL, &documentPtr, FALSE, 0);
	gProcessorCode = 0;
	
}		// end "OnFileOpen"        
  

void 
CMultiSpecApp::OnFilePrintSetup(void)

{  
//	DMORIENT_LANDSCAPE
// DMORIENT_PORTRAIT
	PRINTDLG						pd;
	Handle						windowInfoHandle;
	UInt16						pageOrientationSetting;
	BOOL							returnFlag;

	
	windowInfoHandle = GetActiveWindowInfoHandle ();

	pd.lStructSize = (DWORD)sizeof (PRINTDLG);
	returnFlag = GetPrinterDeviceDefaults (&pd);
	if (returnFlag)
		{	
		pageOrientationSetting = GetPrintPageOrientation (windowInfoHandle);

//		DEVMODE FAR* pDevMode = (DEVMODE FAR*)GlobalLock (m_hDevMode);
//		pDevMode->dmOrientation = pageOrientationSetting;
//		GlobalUnlock (m_hDevMode);

		}		// end "if (returnFlag)"

	CWinApp::OnFilePrintSetup();

	returnFlag = GetPrinterDeviceDefaults (&pd);
	if (returnFlag)
		{
		DEVMODE FAR* pDevMode = (DEVMODE FAR*)GlobalLock (m_hDevMode);
		pageOrientationSetting = pDevMode->dmOrientation;
		GlobalUnlock (m_hDevMode);

		SetPrintPageOrientation (windowInfoHandle, pageOrientationSetting);

		}		// end "if (returnFlag)"
	
}		// end "OnFilePrintSetup" 
  
  

CDocument* 
CMultiSpecApp::OpenDocumentFile(
				LPCSTR				lpszFileName)

{   
	CDocument* 		documentPtr = NULL;
	

	gProcessorCode = kOpenImageFileProcessor;
	OpenImageFileWin ((LocalAppFile*)lpszFileName, &documentPtr, FALSE, 0);
	gProcessorCode = 0;
	  	            
//	openDocument = m_imageDocTemplatePtr->OpenDocumentFile(lpszFileName);
	
	return (documentPtr); 
	
}		// end "OpenDocumentFile"



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
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

SInt32 
CMultiSpecApp::OpenImageFileWin (
				LocalAppFile*						localAppFilePtr,
				CDocument**							documentPtrPtr,
				Boolean								fromOpenProjectImageWindowFlag,
				UInt16								hdfDataSetSelection)

{
	SInt32								projectFileCode;

/*	FileInfoPtr							fileInfoPtr;
										
	CMFileStream*						fileStreamPtr;

	FSRef									fileAsFSRef[500];
	
	Handle								fileInfoHandle,
											windowInfoHandle;
	
	UInt32								formatVersionCode,
											itemCount;
	
	SInt16								errCode,
											fileFormat,
											itemHit,
											numberFileTypes = -1,
											promptString;
	
	Boolean								continueFlag,
											disposeFileInfoHandleFlag,
											fileFlag,
											fileInfoLoadedFlag,
											doneFlag,
											thematicTypeFlag;
*/
	
	projectFileCode = OpenImageFile (localAppFilePtr, FALSE, 0);
/*		
	doneFlag = FALSE;
	disposeFileInfoHandleFlag = FALSE;
	gMultipleImageFileCode = 0;
	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	*documentPtrPtr = NULL;
	
			// Make certain that the cursor has been set for the pointer.  It		
			// is possible for the cursor to be something else if this open 		
			// command is being called directly from the finder.							
				
	MInitCursor();
	
	fileInfoHandle = InitializeFileInfoStructure (NULL, kNotPointer);
	
	if (fileInfoHandle != NULL)
		{
		// oul: added the condition of _AMD64_ to be compatible with x64
		// _AMD64_ is the predefined macro for x64 machines according to MSDN

		#if !defined _X86_ && !defined _AMD64_
		
					// If the option key was down when the 'Open Image' menu item		
					// was selected then include all files in the file list.  			
					// Otherwise just show the selected file types.							
					
			numberFileTypes = gNumberFileTypes;
			if (gEventRecord.modifiers & optionKey)
				numberFileTypes = -1;
				
		#endif	// !defined _X86_ && !defined _AMD64_
			
		while (!doneFlag)
			{
					// Initialize local variables.											
					
			promptString = IDS_SelectImage;
			if (gMultipleImageFileCode != 0)
				promptString = IDS_SelectFileToLink;
		
			fileInfoLoadedFlag = FALSE;
			continueFlag = FALSE;
    		projectFileCode = 0;
    		
			InitializeFileInfoStructure (fileInfoHandle, kNotPointer);
			
			fileInfoPtr = (FileInfoPtr)GetHandlePointer(
													fileInfoHandle,
													kLock,
													kNoMoveHi);
													
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
					// Indicate that the image type is to be determined from the 	
					// header part of the file.												
				
			gGetFileImageType = 0;
		
			errCode = GetFile (fileStreamPtr, 
										numberFileTypes, 
										(SFTypeList*)&gListTypes,
										localAppFilePtr,
										fileAsFSRef,
										&itemCount,
										promptString);
						
    		fileFlag = ((errCode == noErr) & FileExists(fileStreamPtr));
    	
	    			// If localAppFilePtr is not NULL then this is a call from the		
	    			// initialization routine.  This current file may be a project	
	    			// file.  Check for project file case and return if it is.
	    			// The Project file information will be saved and opened as
	    			// the last item in the list.		
	    	
	    	if ( (gGetFileStatus == 1 || localAppFilePtr) && fileFlag)
	    		{
	    		projectFileCode = CheckIfProjectFile (fileStreamPtr, kLoadHeader);
	    		
	    		if (projectFileCode > 0)
	    			{
					// oul: added the condition of _AMD64_ to be compatible with x64
					// _AMD64_ is the predefined macro for x64 machines according to MSDN
					
					#if !defined _X86_ && !defined _AMD64_
		    			if (gGetFileStatus == 1)
		    				{
		    				gLocalAppFile.vRefNum = fileStreamPtr->vRefNum;
		    				gLocalAppFile.parID = fileStreamPtr->parID;
							CopyPToP ( (char*)gAppFile.fName, (char*)fileStreamPtr->fileName);
							gLocalAppFile.type = fileStreamPtr->type;
							gLocalAppFile.refNum = 0;
		    				gLocalAppFile.fSSpecFlag = fileStreamPtr->fSSpecFlag;
		    				gLocalAppFile.fNamePtr = (Str63*)&gAppFile.fName;
		    				
		    				}		// end "if (gGetFileStatus == 1)"
					#endif	// !defined _X86_ && !defined _AMD64_
	    				
	    			fileFlag = FALSE;
					CloseFile (fileStreamPtr);
					
					}		// end "if (projectFileCode > 0)" 
				
				}		// end "if ((gGetFileStatus == 1 || localAppFilePtr) ..." 
				
			CheckAndUnlockHandle (fileInfoHandle);
	    		
	    	if (fileFlag)
	    		{
						// Set the data set specification if given expected hdf or
						// netcdf files.

				if (fromOpenProjectImageWindowFlag && hdfDataSetSelection > 0)
					{
   				fileInfoPtr = (FileInfoPtr)GetHandlePointer (
   																	fileInfoHandle, kNoLock, kNoMoveHi);

					fileInfoPtr->hdfDataSetSelection = hdfDataSetSelection;

//					errCode = LoadHDFInformation (fileInfoPtr, TRUE);	
							
					}		// end "if (fromOpenProjectImageWindowFlag && hdfDataSetSelection > 0" 

	    				// Determine the file format and load the header 				
	    				// information.															
	    				
	    		fileInfoLoadedFlag = 
	    						LoadImageInformation (windowInfoHandle,
																fileInfoHandle, 
																FALSE, 
																&formatVersionCode,
																0);
		
						// If the file information was loaded, add to the active 	
						// image window "file" or create the image window for the 	
						// file.																		
						
			   if (fileInfoLoadedFlag && gGetFileImageType != kVectorImageType)
			   	{
			   	if (gMultipleImageFileCode == 2)
			   		{
   					fileInfoPtr = (FileInfoPtr)GetHandlePointer (
   													fileInfoHandle, kNoLock, kNoMoveHi);
   													
			   		if ( fileInfoPtr->bandInterleave != kBIS )
			   			{
							fileFormat = fileInfoPtr->format;
							continueFlag = AddToImageWindowFile (
																windowInfoHandle, fileInfoHandle);
						
							if (continueFlag && formatVersionCode == 1)
								{
								if (LinkFiles (fileFormat, windowInfoHandle) != noErr)
									continueFlag = FALSE;
								
								}		// end "if (continueFlag && formatVersionCode == 1)"
								
							}		// end "if (fileInfoPtr->bandInterleave != kBIS)"
							
						else		// ...->bandInterleave == kBIS 
									// Can not logically link BIS interleaved files. 	
							{	
							continueFlag = TRUE;
							itemHit = DisplayAlert (
											kErrorAlertID, 2, kAlertStrID, IDS_Alert15, 0, NULL);
							
							}		// end "else ...->bandInterleave == kBIS" 
							
						disposeFileInfoHandleFlag = TRUE;
						
						}		// end "if (gMultipleImageFileCode == 2)" 
			   	
			   	else		// gAddToImageWindowFileFlag != 2 
			   		{
			   		m_openImageFileInfoHandle = fileInfoHandle;
			   		
			   		StringPtr filePathPtr = fileStreamPtr->GetFilePathPtr(); 
						*documentPtrPtr = m_imageDocTemplatePtr->OpenDocumentFile(
																				(char*)&filePathPtr[1]);
						
			   		m_openImageFileInfoHandle = NULL;
						
						continueFlag = (*documentPtrPtr != NULL);
						
						if (continueFlag)
							{															
							CMImageWindow*	imageWindowCPtr = 
											((CMImageDoc*)*documentPtrPtr)->GetDocImageWindowCPtr();
							windowInfoHandle = imageWindowCPtr->GetWindowInfoHandle();
							continueFlag = (windowInfoHandle != NULL);
							
							}		// end "if (continueFlag)"

						if (continueFlag)
							{					
							SetImageType (windowInfoHandle, gGetFileImageType);
							SetWindowType (windowInfoHandle, gGetFileImageType);

							}		// end "if (continueFlag)"
						
						if (continueFlag && formatVersionCode == 1)
							{
							fileFormat = GetFileFormat (windowInfoHandle);
							if (LinkFiles (fileFormat, windowInfoHandle) != noErr)
								continueFlag = FALSE;
							
							}		// end "if (continueFlag && formatVersionCode == 1)"
						
						if (continueFlag)
							{													
							fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
							thematicTypeFlag = GetThematicType (fileInfoHandle);
					
							if (!thematicTypeFlag)
								continueFlag = SetUpImageWindow (windowInfoHandle);
									
							else		// thematicTypeFlag
								continueFlag = SetUpThematicImageWindow (windowInfoHandle);
								
							}		// end "if (continueFlag)"
						
						if (gMultipleImageFileCode == 0)
							doneFlag = TRUE;
						disposeFileInfoHandleFlag = FALSE;
			   		
			   		}		// end "else gMultipleImageFileCode != 2" 
						
					}		// end "if (fileInfoLoadedFlag && gGetFileImageType != kVectorType)" 
				
	      	}		// end "if (fileFlag)"
 	    		
      	if (!continueFlag || gGetFileImageType == kVectorImageType)
      		{
      		DisposeFileInfoHandle (fileInfoHandle);
      		fileInfoHandle = NULL;
				doneFlag = TRUE;
				
				}		// end "if (!continueFlag)"  
				
			}		// end "while (!doneFlag)" 
	   
	   		// Note only dispose of the file info handle. Do not dispose of
	   		// handles within the file info structure since they are now
	   		// being used in a copy of the file info structure for linked
	   		// files.
	   		
		if (disposeFileInfoHandleFlag)
			UnlockAndDispose (fileInfoHandle);
      										
      }		// end "if (fileInfoHandle != NULL)"
		
	gMultipleImageFileCode = 0;
*/      
	return (projectFileCode);
	
}		// end "OpenImageFileWin"

/*                  
#ifndef _WIN32
int _matherr(
				_exception   			*except)

{
   if (except->type == _OVERFLOW)
   	{
   	except->retval = DBL_MAX; 
		return 1;
		
		}		// end "if (except->type == _OVERFLOW)"
		
	else
		return 0;

}		// end "_matherr"
#endif 
*/


int 
CMultiSpecApp::ExitInstance()

{  
	if (m_splash.m_hWnd != NULL)
		{
				// Problem occurred during start up. Close the splash window.
				                                            
		m_splash.DestroyWindow();
		m_pMainWnd->UpdateWindow();
		
		}	// end "if (m_splash.m_hWnd != NULL)"

			// Release memory for non-project processor structures
			
	gProcessorCode = 0;
	ReleaseNonProjProcessorStructures ();
			
			// Dispose of the overlays
	
	while (gNumberShapeFiles > 0)
		{
		CloseOverlayFile (gNumberShapeFiles-1); 
		
		}		// end "while (count > 0)"
			
		UnlockAndDispose (gShapeFilesHandle);
		                                                             
	if (gDebugFileStreamPtr != NULL) 
		delete gDebugFileStreamPtr;
	
			// Close any GDAL interface classes.
#if include_gdal_capability
	CloseGDALInterface ();
#endif		// include_gdal_capability
		
	gDebugFileStreamPtr = NULL;
	
	gBlackPen.DeleteObject();
	gBluePen.DeleteObject();
	gCyanPen.DeleteObject();
	gGreenPen.DeleteObject();
	gMagentaPen.DeleteObject();
	gRedPen.DeleteObject();
	gWhitePen.DeleteObject();
	gYellowPen.DeleteObject();
	
	return ( CWinApp::ExitInstance() );
		
}		// end "ExitInstance"



void CMultiSpecApp::OnAppExit ()

{
	CMImageView*					window;

	Handle							windowInfoHandle;

	UInt32							offset;

	SInt16							itemHit;

	Boolean							doneFlag;

	
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
			 
			windowInfoHandle = GetWindowInfoHandle(window);
				 
			if ( SaveIfWindowChanged (windowInfoHandle, FALSE) )
				{
				doneFlag = FALSE;
				break;
				
				}		// end "if ( SaveIfWindowChanged (..."
		   	
		   }		// end "for (offset=0; offset<..." 
		
		}		// end "if (doneFlag)"  
	
	if (doneFlag)
		{
				// Request user to save the output text window if it has not been saved
				// recently. If this is a forced quit by the application then the user
				// will have a chance to save the output window later on.
			
		Boolean windowChangedFlag = GetOutputWindowChangedFlag ();
		if (windowChangedFlag && gOutputViewCPtr->GetTextBufferLength() > 2)
			{	
			if (LoadSpecifiedStringNumberStringP (
												kAlertStrID, 
												IDS_Alert54, 
												(char*)gTextString,
												(char*)gTextString2,
												TRUE,
												(char*)&gTextString3[1] ) )
				itemHit = DisplayAlert (kSaveAlertID, kStopAlert, 0, 0, 0, gTextString);
				
			if (itemHit == 1)
				doneFlag = gOutputViewCPtr->GetDocument()->SaveModified();
				
			else if (itemHit == 2)
				doneFlag = FALSE;
				
			else
				gOutputViewCPtr->GetEditCtrl().SetModify(FALSE);
			
			}		// end "if (windowChangedFlag && ...->GetTextBufferLength() > 2)"
			
		}		// end "if (doneFlag)"
   
	if (doneFlag)
		{
//		_CrtDumpMemoryLeaks ();

   	gProcessorCode = -1;
   
   	CWinApp::OnAppExit ();
   	
   	}		// end "OnAppExit"

}		// end "OnAppExit"


void 
CMultiSpecApp::ExitApplication ()

{  		
	OnAppExit ();

}		// end "ExitApplication"
