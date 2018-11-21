/*
 *	LMultiSpec.cpp
 *      
 * Copyright 2017 multispec
 *	Revised by Larry Biehl		on 11/09/2018
 *	Revised by Tsung Tai Yeh	on 09/10/2015
 *      
 */

// multspec_lin.cpp : Defines the class behaviors for the application.
// Modified for Linux
/* Template for debugging for MultiSpec Online on mygeohub.org.
	int numberChars = sprintf ((char*)&gTextString3,
								" LMultiSpec::GetUserInputFilePath (countBytes, errCode) %d, %d%s", 
								countBytes,
								errCode,
								gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/


//#include "SMultiSpec.h"

//#include "wx/filedlg.h"
#include "wx/cshelp.h"
#include "LMultiSpec.h"
#include "SDeclareGlobals.h"
#include "CFileStream.h"
//#define multispec_lin
#if !defined multispec_wxmac
	#define production_multiSpec_tool
#endif

CMainFrame *pMainFrame = (CMainFrame *) NULL;
void DisplayDescriptionInformation(FileInfoPtr localFileInfoPtr);

time_t				LinGetTime (void);


CMultiSpecApp::CMultiSpecApp ()

{
    //LOGPEN 				logpen;
    //int 					numChars;
    //Boolean 				continueFlag;


    		// Initialize some global variables here.
	
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
    /***Temporary
    gNonProjProcessorSpecs.biPlotDataSpecsH = NULL;
    gNonProjProcessorSpecs.clusterSpecsH = NULL;
    gNonProjProcessorSpecs.listDataSpecsH = NULL;
    gNonProjProcessorSpecs.listResultsSpecsH = NULL;
    gNonProjProcessorSpecs.principalCompSpecsH = NULL;
    gNonProjProcessorSpecs.statisticsImageSpecsH = NULL;
    gNonProjProcessorSpecs.evalTransformationSpecsH = NULL;
     */
    gResultsFileSummary.numberFiles = 0;
    gResultsFileSummary.diskFileListH = NULL;
    gResultsFileSummary.diskFileListPtr = NULL;

    // Initialize the conversion factors from one distance unit to
    // another. This is used in displaying the coordinates in the coordinate
    // view. Everything is relative to 1 mm.

    gDistanceFileConversionFactors[0] = 1000000.; // km to mm
    gDistanceFileConversionFactors[1] = 1000.; // m  to mm
    gDistanceFileConversionFactors[2] = 10.; // cm to mm
    gDistanceFileConversionFactors[3] = 1.; // mm to mm
    gDistanceFileConversionFactors[4] = 0.001; // um to mm
    gDistanceFileConversionFactors[5] = 1609344.; // mi to mm
    gDistanceFileConversionFactors[6] = 914.4; // yd to mm
    gDistanceFileConversionFactors[7] = 304.800609601219; // survery ft to mm
    gDistanceFileConversionFactors[8] = 304.8; // ft to mm
    gDistanceFileConversionFactors[9] = 25.4; // in to mm

    // Initialize the conversion factors from one distance unit to
    // another. This is used in displaying the coordinates in the coordinate
    // view. Everything is relative to 1 mm.

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
			// This overrides the settings in SDecGlob.h. Not sure what the correct values
			// should be yet.

	gMaxRowBytes = 32767;
	gMaxRowBytesFor24Bits = 32767;
	/*
			// Initialize location parameters for first graph window,
	   			
	gNextGraphicsWindowStart.h = 0;
	gNextGraphicsWindowStart.v = 0;
	 
	gUseGWorldFlag = TRUE;

	gBlinkProcedure = kBlink2;

	m_dwSplashTime = 0;


	m_graphDocTemplatePtr = NULL;
	m_imageDocTemplatePtr = NULL;
	m_statisticsDocTemplatePtr = NULL;
	*/
	m_openImageFileInfoHandle = NULL;
	m_nextControlTime = 0;
	//m_imageZoomCode = 0;
    
		// Initialize some standard color pens
	gBlackPen.SetColour(*wxBLACK);
	gBluePen.SetColour(*wxBLUE);
	gCyanPen.SetColour(*wxCYAN);
	gGreenPen.SetColour(*wxGREEN);
	gRedPen.SetColour(*wxRED);
	gWhitePen.SetColour(*wxWHITE);
	gYellowPen.SetColour(*wxYELLOW);
    /*
    m_controlDelayFlag = TRUE;

                    // Get the separator to use for thousands and decimals
                    // in numbers.

    numChars = GetNumberFormat(
                    LOCALE_USER_DEFAULT, // locale for which string is to be formatted
                    0,							// bit flag that controls the function's operation
                    "1000.12",           // pointer to input number string
                    NULL,						// pointer to a formatting information structure
                    (char*)&gTextString, // pointer to output buffer; gTextStrOnUpdateWindowNewSelectionGraphing
                    100);						// size of output buffer

    if (numChars == 9)
            {
            gThousandsSeparator = gTextString[1];
            gDecimalSeparator = gTextString[5];

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
     * */
    InitializeDateVersionStrings ();
     
} // end "CMultiSpecApp"

/////////////////////////////////////////////////////////////////////////////
// The one and only CMspecApp object

IMPLEMENT_APP(CMultiSpecApp)
/////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CMultiSpecApp, wxApp)
	EVT_QUERY_END_SESSION(CMultiSpecApp::OnQueryEndSession) 
	//EVT_MENU(ID_FILE_OPEN, CMultiSpecApp::OnFileOpen)
	EVT_MENU(ID_IMAGE_OPEN, CMultiSpecApp::OnFileOpen)
			// The following event handler is in CMainFrame
	EVT_MENU(ID_PROC_UTIL_LIST_IMAGE_DESC, CMainFrame::OnProcUtilListImageDesc)
END_EVENT_TABLE()


wxDocument* CMultiSpecApp::ActivateGraphView ()
	{
   //wxDocManager* m_docManager_new;
   //m_docManager_new = new wxDocManager();
   //wxDocTemplate* pGraphDocTemplate_new;
   
   //pGraphDocTemplate_new = new wxDocTemplate(m_docManager_new, wxT("Selection Graph"), 
   //         wxT(".lan;*.img;*.bil;*.tif;*.hdf"), wxT(""), wxT(""), 
	//			wxT("Graph Doc"), wxT("Graph View"), CLASSINFO(CMGraphDoc), CLASSINFO(CMGraphView));
   
   wxDocument* init_doc = pGraphDocTemplate->CreateDocument (
															wxEmptyString, wxDOC_NEW | wxDOC_SILENT);

   return init_doc;
	
}		// end "ActivateGraphView"


void CMultiSpecApp::ActivateProjectView ()

{
   wxDocument* init_doc = pStatisticsDocTemplate->CreateDocument(wxEmptyString, wxDOC_NEW | wxDOC_SILENT);
	
}		// end "ActivateProjectView"



wxDocument* CMultiSpecApp::ActivateListDataView ()

{
	wxDocument* init_doc = pListDataDocTemplate->CreateDocument (
															wxEmptyString, wxDOC_NEW | wxDOC_SILENT);
															
	return init_doc;
	
}	// end "ActivateListDataView"



void CMultiSpecApp::GetUserInputFilePath (
				wxString								toolParameterFilePath)
{
	wxString								wxImageFilePath;
	wxWCharBuffer						wideCharBuffer;
	
	UInt8									toolParametersString[5000];
	
	wchar_t								wideImageFilePathPtr[256];
	
	CMFileStream*						toolParametersFileStreamPtr;
	
	FileStringPtr						imageFilePathPtr,
											toolParametersFilePathPtr;
											
	size_t								wxImageFilePathLength;
	
	UInt32								count;
	
	SInt16								errCode;
	
	
	toolParametersFileStreamPtr = InitializeFileStream ((CMFileStream*)NULL);
	
	if (toolParametersFileStreamPtr != NULL && toolParameterFilePath.Length() > 0)
		{
		toolParametersFilePathPtr = (FileStringPtr)toolParametersFileStreamPtr->GetFilePathPPtr();
		strncpy ((char*)&toolParametersFilePathPtr[1], 
					(const char*)(toolParameterFilePath.mb_str(wxConvUTF8)), 
					toolParameterFilePath.Length()); 
		toolParametersFilePathPtr[0] = toolParameterFilePath.Length();
		toolParametersFilePathPtr[toolParametersFilePathPtr[0]+1] = 0;			
		
		int numberChars = sprintf ((char*)&gTextString3,
												" LMultiSpec::GetUserInputFilePath: (TOOL_PARAMETERS file): %s%s",
												toolParametersFilePathPtr,
												gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);

		errCode = OpenFileReadOnly (toolParametersFileStreamPtr,
											  kResolveAliasChains,
											  kLockFile,
											  kVerifyFileStream);
										
		if (errCode == noErr) 
			{
			count = 4995;
			errCode = toolParametersFileStreamPtr->MReadData (toolParametersString, &count, kErrorMessages); 

			if (count > 0 && (errCode == noErr || errCode == eofErr))
				{
						// Now find the image file to be used.
						
				toolParametersString[count] = 0;
						
				//imageFilePathPtr = (StringPtr)strstr (toolParametersString, "file(path):");
				imageFilePathPtr = (FileStringPtr)strstr ((char*)toolParametersString, "file:");
				
				if (imageFilePathPtr != NULL)
					{
					Ptr		nextLinePtr;
					
							// The file path was found. Now find the next end of line character.
							// And set it to the string terminator.
							// Note that the SkipToNextCarriageReturn returns the first character after
							// the end of line character.
							
					nextLinePtr = (Ptr)SkipToNextCarriageReturn ((UCharPtr)imageFilePathPtr);
					if (nextLinePtr != NULL)
						{
						nextLinePtr--;
						*nextLinePtr = 0;
						
						}		// end "if (nextLinePtr != NULL)"
					
							// The resulting string needs to be a pascal string. Use the ":" character as the 
							// character count.
							
					imageFilePathPtr += 4;
					imageFilePathPtr[0] = strlen ((char*)&imageFilePathPtr[1]);
					
					int numberChars2 = sprintf ((char*)&gTextString3,
												" LMultiSpec::GetUserInputFilePath: (selected file): %s%s",
												&imageFilePathPtr[1],
												gEndOfLine);
					ListString ((char*)&gTextString3, numberChars2, gOutputTextH);
					
							// Note that the unicode file path will be needed for OpenImageFileLin below
				
					wxImageFilePath = wxString::FromAscii((char*)&imageFilePathPtr[1]);
					wideCharBuffer = wxImageFilePath.wc_str();
					wxImageFilePathLength = wideCharBuffer.length();
					
					wmemcpy (&wideImageFilePathPtr[1], 
								wideCharBuffer.data(), 
								wxImageFilePathLength);		
								
					wideImageFilePathPtr[0] = (wchar_t)wxImageFilePathLength;			
					
					CheckSomeEvents(0);
					
					gProcessorCode = kOpenImageFileProcessor;
					gFromToolParameterFileFlag = TRUE;
					gCallProcessorDialogFlag = FALSE;
					OpenImageFileLin (wideImageFilePathPtr, FALSE, 0);
					gFromToolParameterFileFlag = FALSE;
					gCallProcessorDialogFlag = TRUE;
					gProcessorCode = 0;
					
					}		// end "if (imageFilePathPtr != NULL)"
			
				}		// end "if (errCode != eofErr || count == 0)"
				
			toolParametersFileStreamPtr->MCloseFile();
			
			}	// end "if (errCode == noErr)"
		
		}		// end "if (toolParametersFileStreamPtr != NULL)"
		
}	// end "GetUserInputFilePath" 


#if !defined NetBeansProject
int CMultiSpecApp::OnExit(void) 
{
	#ifndef multispec_wxmac
		//wxDELETE(m_docManager);
		
		#ifdef production_multiSpec_tool
			wxMessageBox (wxString::Format(_("Select Terminate button in upper right to finish exiting this MultiSpec session.")),
								wxTheApp->GetAppDisplayName(),
								wxCENTRE | wxSTAY_ON_TOP);
		#endif

		delete pGraphDocTemplate;
		delete pimageDocTemplate;
		delete pOutputDocTemplate;
		delete pStatisticsDocTemplate;
		delete m_docManager;
		delete pMainFrame;
   #endif
	
	return 1;
	 
}		// end "OnExit"
#endif


void CMultiSpecApp::OnFileOpen(wxCommandEvent& event)
{
	gProcessorCode = kOpenImageFileProcessor;
	OpenImageFileLin (NULL, FALSE, 0);
	gProcessorCode = 0;

}		// end "OnFileOpen" 


		// CMultiSpecApp initialization
bool CMultiSpecApp::OnInit ()

{
	//wxInitAllImageHandlers();
	m_docManager = new wxDocManager();

			// Register the application's document templates.  Document templates
			// serve as the connection between documents, frame windows and views.

			// Image Document Template
	
	pStatisticsDocTemplate = new wxDocTemplate (m_docManager,
																wxT("Statistics Document"),
																wxT("*.stat"),
																wxT(""),
																wxT(""),
        														wxT("Stat Doc"),
        														wxT("Stat View"),
        														CLASSINFO (CMStatisticsDoc),
        														CLASSINFO (CMStatisticsView));

	pOutputDocTemplate = new wxDocTemplate (m_docManager,
															wxT("Text Document"),
															wxT("*.txt"),
															wxT(""),
															wxT(""),
															wxT("Text Doc"),
															wxT("Text View"),
															CLASSINFO (CMTextDoc),
															CLASSINFO (CMTextView));

	pimageDocTemplate = new wxDocTemplate (m_docManager,
														wxT("Image Document"),
														wxT("*.lan;*.img;*.bil;*.tif;*.hdf"),
														wxT(""),
														wxT(""),
														wxT("Image Doc"),
														wxT("Image View"),
														CLASSINFO (CMImageDoc),
														CLASSINFO (CMImageView));

			// Graph Document Template
			
	pGraphDocTemplate = new wxDocTemplate (m_docManager, 
														wxT("Selection Window"), 
														wxT(".lan;*.img;*.bil;*.tif;*.hdf"), 
														wxT(""), 
														wxT(""), 
														wxT("Graph Doc"), 
														wxT("Graph View"), 
														CLASSINFO(CMGraphDoc), 
														CLASSINFO(CMGraphView));
   
			// List data graph
			
	pListDataDocTemplate = new wxDocTemplate (m_docManager, 
															wxT("Graph"), 
															wxT(".lan;*.img;*.bil;*.tif;*.hdf"), 
															wxT(""), 
															wxT(""),
															wxT("Graph Doc"), 
															wxT("Graph View"), 
															CLASSINFO (CMGraphDoc), 
															CLASSINFO (CMGraphView));
	
			// Create main doc Frame window
	
	#if defined multispec_wxmac
		pMainFrame = new CMainFrame (m_docManager,
												(wxDocParentFrame*)NULL,
												wxT(""),
												wxDefaultPosition,
												//wxSize (0, 0),
												wxSize (1100, 700),
												wxSYSTEM_MENU);
	#else
		pMainFrame = new CMainFrame (m_docManager,
												(wxDocParentFrame*)NULL,
												wxT("MultiSpec"),
												wxDefaultPosition,
												wxSize (1100, 700),
												wxDEFAULT_FRAME_STYLE);
	#endif
	
	#if !defined multispec_wxmac
		pMainFrame->Centre (wxBOTH);
		pMainFrame->Show (TRUE);
		//pMainFrame->ShowWithoutActivating();
	#endif
	
	SetTopWindow (pMainFrame);
	SetAppName ("MultiSpec");
	
	wxDocument* init_doc = pOutputDocTemplate->CreateDocument (wxEmptyString, wxDOC_NEW | wxDOC_SILENT);
	
			// Determine if a file has already been selected for opening and set the flag for that
			// in the MainFrame. It will be acted upon during the first idle period.
		
	bool returnFlag = wxGetEnv (wxT("TOOL_PARAMETERS"), NULL);
	pMainFrame->SetToolParametersFlag (returnFlag);

	#ifdef NetBeansProject
		gDefaultDataDirectory = wxGetHomeDir();
	#else
		#ifdef multispec_wxmac
			gDefaultDataDirectory = wxGetHomeDir();
		#else
			//dialog.SetDirectory("/apps/multispec/current/data");
			//gDefaultDataDirectory = wxString::FromUTF8((char*)getenv ("DATA_PATH"));
			gDefaultDataDirectory = wxString::FromUTF8("/data/tools/multispec");
		#endif
	#endif

	return true;

}	// end "OnInit"


void CMultiSpecApp::OnQueryEndSession (wxCloseEvent& event) 
{ 
	//wxLogMessage("Query end session"); 
	/*
	int numberChars = sprintf ((char*)&gTextString3,
												" CMultiSpecApp:OnQueryEndSession (gProjectInfoPtr): %ld%s", 
												gProjectInfoPtr,
												gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
	*/
	if (!CloseTheProject())
		event.Veto();
 
}		// end "OnQueryEndSession"


//-----------------------------------------------------------------------------
//				                    Copyright (1988-2016)
//                          (c) Purdue Research Foundation
//                             	All rights reserved.
//
//	Function name:		SInt32 OpenImageFile
//
//	Software purpose:	The purpose of this routine is to open an image file
//				either multispectral type or Thematic (classification)
//				type.
//
//	Parameters in:		Pointer to structure giving information about the image
//				to use as the selected image file. If this pointer is
//				NULL, then present a dialog box to the use to allow
//				he/she to select the image file.
//
//	Parameters out:	None
//
//	Value Returned:	0 if selected file is not a project file
//			value > 0 indicating the project file version if the
//			selected file is not a project file.
//
// Called By:		OnFileOpen					in MultiSpec2.cpp
//						OpenProjectImageWindow	in SProject.cpp
//
//	Coded By:	Larry L. Biehl			Date: 12/18/1989
//	Revised By:	Larry L. Biehl			Date: 09/19/2017

SInt32 CMultiSpecApp::OpenImageFileLin (
				LocalAppFile*						localAppFilePtr,
				Boolean								fromOpenProjectImageWindowFlag,
				UInt16								hdfDataSetSelection)
{
	SInt32					projectFileCode;
	
	
	projectFileCode = OpenImageFile (localAppFilePtr,
													fromOpenProjectImageWindowFlag,
													hdfDataSetSelection);

	return (projectFileCode);

} // end "OpenImageFileLin"


//------------------------------------------------------------------------------------
//											Copyright (1988-2016)
//                          (c) Purdue Research Foundation
//											All rights reserved.
//
//	Function name:		void ListDescriptionInformation
//
//	Software purpose:	The purpose of this routine is to list the descriptive
//							information for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/01/1993
//	Revised By:			Larry L. Biehl			Date: 11/26/2008

wxFrame *CMultiSpecApp::CreateChildFrame(wxDocument *doc, wxView *view) {
			// create a child frame of image display frame
	wxFrame *subframe;

	subframe = new CMImageFrame(doc, view, wxStaticCast(GetTopWindow(), wxDocParentFrame));

	return subframe;
}

/*
SInt16 CMultiSpecApp::GetZoomCode(void) {
    return (m_imageZoomCode);

} // end "GetZoomCode"


void CMultiSpecApp::SetZoomCode(SInt16 zoomCode) 
{
	m_imageZoomCode = zoomCode;

	if (m_imageZoomCode == 0)
		m_nextControlTime = 0;

	else // m_imageZoomCode != 0
		{
		m_controlDelayFlag = TRUE;
		
				// Currently for linux, if user holds the ctrl key down and holds the zoom button
				// down, zoom will continue for 1/10 increments. Have not found a way to handle
				// for left clicking on the button. Only event returned is when mouse button goes up
				// after a mouse down.
		
		if (wxGetKeyState(WXK_SHIFT))
			m_controlDelayFlag = FALSE;
	  
		else		// !wxGetKeyState(WXK_SHIFT)
			m_controlDelayFlag = TRUE;
		
	} // end "else m_imageZoomCode != 0"

} // end "SetZoomCode"
*/
/*
void CMultiSpecApp::SetControlDelayFlag(Boolean delayFlag) {
    m_controlDelayFlag = delayFlag;

    if (m_imageZoomCode == 0)
        m_controlDelayFlag = TRUE;

} // end "SetControlDelayFlag"
*/

Handle CMultiSpecApp::GetOpenImageFileInfoHandle() {
    return m_openImageFileInfoHandle;
} // end "GetOpenImageFileInfoHan


Handle CMultiSpecApp::SetUpNewImageDocument (
				Handle								fileInfoHandle,
				SInt16								fileImageType,
				SInt16								windowType)
				
{   
	wxDocument* 							documentPtr = NULL;
	CMFileStream*							fileStreamPtr;
	FileInfoPtr								fileInfoPtr;

	Handle									windowInfoHandle;
	
	SignedByte								handleStatus;

	Boolean									continueFlag;


	m_openImageFileInfoHandle = fileInfoHandle;
	
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (fileInfoHandle,
																				&handleStatus,
																				kNoMoveHi);
											
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr != NULL)
		{
		FileStringPtr filePathPtr = (FileStringPtr)fileStreamPtr->GetFilePathPPtr(); 
      		
		wxString filepathstr = wxString::FromUTF8 ((char*)&filePathPtr[1]);
      documentPtr = pimageDocTemplate->CreateDocument (filepathstr, wxDOC_NEW | wxDOC_SILENT);
		
		//documentPtr = m_imageDocTemplatePtr->OpenDocumentFile ((char*)&filePathPtr[1]);
													
		}	// end "if (fileStreamPtr != NULL)"
		
	MHSetState (fileInfoHandle, handleStatus);
	
	m_openImageFileInfoHandle = NULL;
	
	continueFlag = (documentPtr != NULL);
	
	if (continueFlag)
		{															
		CMImageWindow*	imageWindowCPtr = 
						((CMImageDoc*)documentPtr)->GetDocImageWindowCPtr();
		windowInfoHandle = imageWindowCPtr->GetWindowInfoHandle();
		continueFlag = (windowInfoHandle != NULL);
		
		}		// end "if (continueFlag)"

	if (continueFlag)
		{					
		SetImageType (windowInfoHandle, fileImageType);
		SetWindowType (windowInfoHandle, windowType);

		}		// end "if (continueFlag)"

	return (windowInfoHandle);
	
}	// end "SetUpNewImageDocument"


/*
//----------------------------------------------------------------------------
// wxFileDialog convenience functions
//----------------------------------------------------------------------------

wxString wxFileSelector(const wxString& title,
                        const wxString& defaultDir,
                        const wxString& defaultFileName,
                        const wxString& defaultExtension,
                        const wxString& filter,
                        int flags,
                        wxWindow *parent,
                        int x, int y)
{
	flags |= wxSTAY_ON_TOP;
	
    // The defaultExtension, if non-empty, is
    // appended to the filename if the user fails to type an extension. The new
    // implementation (taken from wxFileSelectorEx) appends the extension
    // automatically, by looking at the filter specification. In fact this
    // should be better than the native Microsoft implementation because
    // Windows only allows *one* default extension, whereas here we do the
    // right thing depending on the filter the user has chosen.

    // If there's a default extension specified but no filter, we create a
    // suitable filter.

    wxString filter2;
    if ( !defaultExtension.empty() && filter.empty() )
        filter2 = wxString(wxT("*.")) + defaultExtension;
    else if ( !filter.empty() )
        filter2 = filter;

    wxFileDialog fileDialog(parent, title, defaultDir,
                            defaultFileName, filter2,
                            flags, wxPoint(x, y));

    // if filter is of form "All files (*)|*|..." set correct filter index
    if ( !defaultExtension.empty() && filter2.find(wxT('|')) != wxString::npos )
    {
        int filterIndex = 0;

        wxArrayString descriptions, filters;
        // don't care about errors, handled already by wxFileDialog
        (void)wxParseCommonDialogsFilter(filter2, descriptions, filters);
        for (size_t n=0; n<filters.GetCount(); n++)
        {
            if (filters[n].Contains(defaultExtension))
            {
                filterIndex = n;
                break;
            }
        }

        if (filterIndex > 0)
            fileDialog.SetFilterIndex(filterIndex);
    }

    wxString filename;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        filename = fileDialog.GetPath();
    }

    return filename;
}

//----------------------------------------------------------------------------
// wxFileSelectorEx
//----------------------------------------------------------------------------

wxString wxFileSelectorEx(const wxString& title,
                          const wxString& defaultDir,
                          const wxString& defaultFileName,
                          int*            defaultFilterIndex,
                          const wxString& filter,
                          int             flags,
                          wxWindow*       parent,
                          int             x,
                          int             y)

{
	flags |= wxSTAY_ON_TOP;
	
    wxFileDialog fileDialog(parent,
                            title,
                            defaultDir,
                            defaultFileName,
                            filter,
                            flags, wxPoint(x, y));

    wxString filename;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        if ( defaultFilterIndex )
            *defaultFilterIndex = fileDialog.GetFilterIndex();

        filename = fileDialog.GetPath();
    }

    return filename;
}
*/


wxFrame* GetActiveFrame(void) 
{ 
	wxFrame* frame = NULL;
	int		height,
				width;
	
	
	if (gOutputViewCPtr != NULL && (WindowPtr)gOutputViewCPtr == gTheActiveWindow) {
		gOutputViewCPtr->m_frame->GetClientSize (&width, &height);
		if (width > 0 && height > 0)
			frame = gOutputViewCPtr->m_frame;
			
		}
	
	if (frame == NULL && gActiveImageViewCPtr != NULL) {
		gActiveImageViewCPtr->m_frame->GetClientSize (&width, &height);
		if (width > 0 && height > 0)
			frame = gActiveImageViewCPtr->m_frame;
			
		}
		
	if (frame == NULL)
		frame = pMainFrame;
		
	return (frame);
		
}	// end "GetActiveChildFrame"


CMainFrame *GetMainFrame(void) 
{
    return pMainFrame;
}


time_t LinGetTime ()
{
	time_t		returnValue = 1;
	
	timespec		tp;
	if (clock_gettime (CLOCK_MONOTONIC, &tp) == 0)
		{
		returnValue = (time_t)(tp.tv_sec*1000 + tp.tv_nsec/1000000);
		
		}		// end "if (clock_gettime (CLOCK_MONOTONIC, &tp) == 0)"
	
	return (returnValue);
}


void FileUploadProcess::OnTerminate(int pid, int status)
{
	int numberChars;
	
	
//	wxLogStatus(m_parent, wxT("Process %u ('%s') terminated with exit code %d."),
//					pid, m_cmd.c_str(), status);

	if (status == 0)
		numberChars = sprintf ((char*)&gTextString3,
										"%sSelected file was uploaded successfully%s",
										gEndOfLine,
										gEndOfLine);
	else		// status != 0
		numberChars = sprintf ((char*)&gTextString3,
										"%sSelected file was not uploaded successfully%s",
										gEndOfLine,
										gEndOfLine);
	
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);
	
	pMainFrame->OnAsyncTermination (this);

}		// end "OnTerminate"