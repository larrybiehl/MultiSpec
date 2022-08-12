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
// File:                xMultiSpec.cpp : class implementation file
// Class Definition:		xMultiSpec.h
//
// Authors:					Abdur Rahman Maud, Larry L. Biehl
//
// Revision date:			09/10/2015 by Tsung Tai Yeh
//								05/29/2020 by Larry Biehl
//
// Language:				C++
//
// System:              Linux and MacOS Operating Systems
//
// Brief description:   This file contains functions that relate to the
//                  		CMultiSpecApp class.
//
/* Template for debugging for MultiSpec Online on mygeohub.org.
	int numberChars = sprintf ((char*)&gTextString3,
					" xMultiSpec::GetUserInputFilePath (countBytes, errCode) %d, %d%s",
					countBytes,
					errCode,
					gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "wx/cshelp.h"

#include "SFileStream_class.h"

#include "xMultiSpec.h"
#include "xStatusDialog.h"
#include "wx/splash.h"
#include "SDeclareGlobals.h"

#if !defined multispec_wxmac
	#define production_multiSpec_tool
#endif

#if defined multispec_wxmac
	#include "wx/sysopt.h"
#endif

CMainFrame *pMainFrame = (CMainFrame*)NULL;

//void DisplayDescriptionInformation (FileInfoPtr localFileInfoPtr);

time_t LinGetTime (void);

BEGIN_EVENT_TABLE (CMultiSpecApp, wxApp)
	EVT_CHAR_HOOK (CMultiSpecApp::OnCharHook)
	EVT_MENU (ID_IMAGE_OPEN, CMultiSpecApp::OnFileOpen)
			// The following event handler is in CMainFrame
	//EVT_MENU (ID_PROC_UTIL_LIST_IMAGE_DESC, CMainFrame::OnProcUtilListImageDesc)
	EVT_QUERY_END_SESSION (CMultiSpecApp::OnQueryEndSession)
END_EVENT_TABLE ()


wxIMPLEMENT_APP (CMultiSpecApp);


CMultiSpecApp::CMultiSpecApp ()

{
	
}	// end "CMultiSpecApp"


wxDocument* CMultiSpecApp::ActivateGraphView (void)

{
   wxDocument* init_doc = pGraphDocTemplate->CreateDocument (
															wxEmptyString, wxDOC_NEW | wxDOC_SILENT);

   return init_doc;
	
}	// end "ActivateGraphView"



void CMultiSpecApp::ActivateProjectView (void)

{
   pStatisticsDocTemplate->CreateDocument (wxEmptyString, wxDOC_NEW | wxDOC_SILENT);
	
}	// end "ActivateProjectView"



wxDocument* CMultiSpecApp::ActivateListDataView (void)

{
	wxDocument* init_doc = pListDataDocTemplate->CreateDocument (
															wxEmptyString, wxDOC_NEW | wxDOC_SILENT);
															
	return init_doc;
	
}	// end "ActivateListDataView"



wxFrame *CMultiSpecApp::CreateChildFrame (
				wxDocument*							doc,
				wxView*								view)

{
			// create a child frame of image display frame
	
	wxFrame *subframe;

	subframe = new CMImageFrame (
								doc, view, wxStaticCast (GetTopWindow(), wxDocParentFrame));

	return subframe;
	
}	// end "CreateChildFrame"


#if defined multispec_wxlin
void CMultiSpecApp::GetUserInputFilePath (
				wxString								toolParameterFilePath)

{
	wxString								wxImageFilePath;
	wxWCharBuffer						wideCharBuffer;
	
	UInt8									toolParametersString[5000];
	
	wchar_t								wideImageFilePathPtr[_MAX_PATH];
	
	CMFileStream*						toolParametersFileStreamPtr;
	
	FileStringPtr						imageFilePathPtr,
											toolParametersFilePathPtr;
											
	size_t								wxImageFilePathLength;
	
	UInt32								count;
	
	SInt16								errCode;
	
	
	toolParametersFileStreamPtr = InitializeFileStream ((CMFileStream*)NULL);
	
	if (toolParametersFileStreamPtr != NULL && toolParameterFilePath.Length() > 0)
		{
		toolParametersFilePathPtr =
							(FileStringPtr)toolParametersFileStreamPtr->GetFilePathPPtr ();
		strncpy ((char*)&toolParametersFilePathPtr[2],
					(const char*)(toolParameterFilePath.mb_str (wxConvUTF8)),
					toolParameterFilePath.Length());
		//toolParametersFilePathPtr[0] = toolParameterFilePath.Length ();
		//toolParametersFilePathPtr[toolParametersFilePathPtr[0]+1] = 0;
		SetFileStringLength (toolParametersFilePathPtr,
										toolParameterFilePath.Length ());
		
		int numberChars = sprintf (
						(char*)&gTextString3,
						" xMultiSpec::GetUserInputFilePath: (TOOL_PARAMETERS file): %s%s",
						&toolParametersFilePathPtr[2],
						gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
		
		errCode = OpenFileReadOnly (toolParametersFileStreamPtr,
											  kResolveAliasChains,
											  kLockFile,
											  kVerifyFileStream);
										
		if (errCode == noErr) 
			{
			count = 4995;
			errCode = toolParametersFileStreamPtr->MReadData (toolParametersString,
																				&count,
																				kErrorMessages);

			if (count > 0 && (errCode == noErr || errCode == eofErr))
				{
						// Now find the image file to be used.
						
				toolParametersString[count] = 0;
						
				imageFilePathPtr = (FileStringPtr)strstr (
															(char*)toolParametersString, "file:");
				
				if (imageFilePathPtr != NULL)
					{
					Ptr		nextLinePtr;
					
							// The file path was found. Now find the next end of line
							// character. And set it to the string terminator.
							// Note that the SkipToNextCarriageReturn returns the first
							// character after the end of line character.
							
					nextLinePtr =
									(Ptr)SkipToNextCarriageReturn ((UCharPtr)imageFilePathPtr);
					if (nextLinePtr != NULL)
						{
						nextLinePtr--;
						*nextLinePtr = 0;
						
						}	// end "if (nextLinePtr != NULL)"
					
							// The resulting string needs to be a pascal string. Use the ":"
							// character as the character count.
							
					imageFilePathPtr += 4;
					imageFilePathPtr[0] = strlen ((char*)&imageFilePathPtr[1]);
					
					int numberChars2 = sprintf (
									(char*)&gTextString3,
									" xMultiSpec::GetUserInputFilePath: (selected file): %s%s",
									&imageFilePathPtr[1],
									gEndOfLine);
					ListString ((char*)&gTextString3, numberChars2, gOutputTextH);
					
							// Note that the unicode file path will be needed for
							// OpenImageFileLin below
				
					wxImageFilePath = wxString::FromAscii ((char*)&imageFilePathPtr[1]);
					wideCharBuffer = wxImageFilePath.wc_str ();
					wxImageFilePathLength = wideCharBuffer.length ();
					
					wmemcpy (&wideImageFilePathPtr[1], 
								wideCharBuffer.data (),
								wxImageFilePathLength);		
								
					wideImageFilePathPtr[0] = (wchar_t)wxImageFilePathLength;			
					
					CheckSomeEvents (0);
					
					gProcessorCode = kOpenImageFileProcessor;
					gFromToolParameterFileFlag = TRUE;
					gCallProcessorDialogFlag = FALSE;
					OpenImageFileLin (wideImageFilePathPtr, FALSE, 0);
					gFromToolParameterFileFlag = FALSE;
					gCallProcessorDialogFlag = TRUE;
					gProcessorCode = 0;
					
					}	// end "if (imageFilePathPtr != NULL)"
			
				}	// end "if (errCode != eofErr || count == 0)"
				
			toolParametersFileStreamPtr->MCloseFile ();
	
			MInitCursor ();
			
			}	// end "if (errCode == noErr)"
			
		}	// end "if (toolParametersFileStreamPtr != NULL)"
		
}	// end "GetUserInputFilePath"
#endif	// defined multispec_wxlin

#if defined multispec_wxmac
void CMultiSpecApp::MacOpenFiles (
				const wxArrayString& 			fileNames)

{
	wxWCharBuffer						wideCharBuffer;
	
	wchar_t								wideImageFilePathPtr[_MAX_PATH];
	
	size_t 								i,
											fileCount,
											wxImageFilePathLength;
	
	
	fileCount = fileNames.GetCount ();
	for (i=0; i<fileCount; i++)
		{
		wideCharBuffer = fileNames[i].wc_str ();
		wxImageFilePathLength = wideCharBuffer.length ();

		wmemcpy (&wideImageFilePathPtr[1],
					wideCharBuffer.data (),
					wxImageFilePathLength);

		wideImageFilePathPtr[0] = (wchar_t)wxImageFilePathLength;

		gProcessorCode = kOpenImageFileProcessor;
		gFromToolParameterFileFlag = TRUE;
		gCallProcessorDialogFlag = FALSE;
		OpenImageFileLin (wideImageFilePathPtr, FALSE, 0);
		gFromToolParameterFileFlag = FALSE;
		gCallProcessorDialogFlag = TRUE;
		gProcessorCode = 0;

		}	// end "for (i=0; i<fileCount; i++)"
	
	MInitCursor ();
}
#endif	// defined multispec_wxmac


void CMultiSpecApp::OnCharHook (
				wxKeyEvent& 						event)

{
	if (gProcessorCode == kDisplayProcessor ||
											gProcessorCode == kSelectionGraphStatsProcess)
		{
				// Note that the command (control) period is not caught here. One only
				// gets the command key not the period. Because of this, the command
				// period is caught using an accel entry in CMImageFrame class
				// (1/2/2019; Larry Biehl)
		
		if (event.GetKeyCode () == WXK_ESCAPE) // ||
				//(event.GetKeyCode() == wxKeyCode ('.') &&
				//											event.GetModifiers () == wxMOD_CONTROL))
			{
			gOperationCanceledFlag = TRUE;
			
			}	// end "if (event.GetKeyCode () == WXK_ESCAPE || ..."
		
		event.Skip ();
		
		}	// end "if (gProcesorCode == kDisplayProcessor)"
	
	else if (gProcessorCode == kClusterProcessor)
		{
				// Note that the command (control) period is not caught here. One only
				// gets the command key not the period. Because of this, the command
				// period is caught using an accel entry in CMImageFrame class
				// (1/2/2019; Larry Biehl)
		
		if (event.GetKeyCode () == WXK_ESCAPE)
			{
			if (gStatusDialogPtr != NULL)
				((CShortStatusDlg*)gStatusDialogPtr)->
													m_canceledCommandInitiatedFlag = TRUE;
			
			}	// end "if (event.GetKeyCode () == WXK_ESCAPE || ..."
		
		event.Skip ();
		
		}	// end "if (gProcesorCode == kDisplayProcessor)"

	else
		event.Skip ();
	
}	// end "OnCharHook"


#if !defined NetBeansProject
int CMultiSpecApp::OnExit (void)

{
	#ifndef multispec_wxmac
		#ifdef production_multiSpec_tool
			wxMessageBox (wxString::Format (_("Select Terminate button in upper right to finish exiting this MultiSpec session.")),
								wxTheApp->GetAppDisplayName (),
								wxCENTRE | wxSTAY_ON_TOP);
		#endif

		delete pGraphDocTemplate;
		delete pImageDocTemplate;
		delete pOutputDocTemplate;
		delete pStatisticsDocTemplate;
		delete m_docManager;
		delete pMainFrame;
   #endif
	
	return 1;
	 
}	// end "OnExit"
#endif


void CMultiSpecApp::OnFileOpen (
				wxCommandEvent& 					event)

{
	gProcessorCode = kOpenImageFileProcessor;
	OpenImageFileLin (NULL, FALSE, 0);
	gProcessorCode = 0;

}	// end "OnFileOpen"



		// CMultiSpecApp initialization

bool CMultiSpecApp::OnInit ()

{
	m_systemTerminationFlag = FALSE;
	
    		// Call the base class initialization method; currently it only parses a
    		// few common command-line options but it could be do more in the future
	
    if (!wxApp::OnInit ())
																							return false;
	
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
	
			// This structure was used for U2U workflow processing. It
			// is currently not implemented in the 64 bit version.
			
	gMultiSpecWorkflowInfo.workFlowCode = 0;

			// Maximum number of columns possible.
			// Note that the MaxRowBytes actually represents columns
			// This overrides the settings in SDecGlob.h. Not sure what the correct
			// values should be yet.

	#if defined multispec_wxmac
				// Not sure what the max is. I have had as many as 240,000 columns work
				// okay
		gMaxRowBytes = 1000000;
		gMaxRowBytesFor24Bits = 1000000;
		
				// Set limit for maximum number of open files.

		rlimit								resourceLimits;
		//SInt32								numberOfOpenFilesHardLimit;
		if (getrlimit (RLIMIT_NOFILE, &resourceLimits) == 0)
			{
			gNumberOfOpenFilesLimit = (SInt32)resourceLimits.rlim_cur;
			//numberOfOpenFilesHardLimit = (SInt32)MIN (resourceLimits.rlim_max, SInt32_MAX);
			
			if (resourceLimits.rlim_cur < 4096)
				{
				resourceLimits.rlim_cur = 4096;
				resourceLimits.rlim_max = MAX (8192, resourceLimits.rlim_max);
				
				if (setrlimit (RLIMIT_NOFILE, &resourceLimits) == 0)
					gNumberOfOpenFilesLimit = (SInt32)resourceLimits.rlim_cur;
				
				}	// end "if (resourceLimits.rlim_cur < 4096)"
			
			}	// end "if (getrlimit (RLIMIT_NOFILE, &resourceLimits) == 0)"
		
				// Indicate a few less than the limit. Going to the limit has
				// caused MultiSpec to crash.
				
		gNumberOfOpenFilesLimit -= 20;
		
				// Adjust the maximum number of files that can be linked if needed.
				
		gMaximumNumberOfLinkedFiles = MIN (gMaximumNumberOfLinkedFiles,
														gNumberOfOpenFilesLimit/2);
	#endif
	#if defined mulitispec_wxlin
				// It appears that he limiting factor is really the numer of lines by
				// columns by 4; This needs to be less than 2,147,483,648. (SInt32_Max).
		gMaxRowBytes = 65535;
		gMaxRowBytesFor24Bits = 66535;
	#endif
	
	m_openImageFileInfoHandle = NULL;
	
			// Initialize some standard color pens
	
	gBlackPen.SetColour (*wxBLACK);
	gBluePen.SetColour (*wxBLUE);
	gCyanPen.SetColour (*wxCYAN);
	gGreenPen.SetColour (*wxGREEN);
	gMagentaPen.SetColour (magentaColor);
	gRedPen.SetColour (*wxRED);
	gWhitePen.SetColour (*wxWHITE);
	gYellowPen.SetColour (*wxYELLOW);
	
	InitializeDateVersionStrings ();
	
	#if defined multispec_wxmac
		//wxSystemOptions::SetOption (wxT("osx.openfiledialog.always-show-types"), 1);
		wxSystemOptions::SetOption (wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES, 1);
	#endif
	
	wxInitAllImageHandlers ();
	
	m_docManager = new wxDocManager ();

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

	pImageDocTemplate = new wxDocTemplate (m_docManager,
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
		//pMainFrame->ShowWithoutActivating ();
	#endif
	/*
			// Was able to get the splash screen to work, but the image being displayed
			// is scaled to twice the size. Probably related to Retina displays. It did
			// not look good. An attempt was make to load as a wxImage and the convert
			// to wxBitmap with scaling but did not get it working.
			// Decided on 12/12/2019 to no use a splashscreen; notice that many apps
			// do not use them now.
	#if defined multispec_wxmac
		wxBitmap splashScreenBitmap;
		//wxImage splashScreenImage;
		//splashScreenImage.LoadFile (wxT("Resources/MultiSpec_Splashscreen.png"), 
												wxBITMAP_TYPE_PNG);
		//if (splashScreenImage.IsOk ())
		//	{
		//	splashScreenBitmap = wxBitmap (splashScreenImage, -1, 2);
			
		//	}

		splashScreenBitmap.LoadFile (wxT("MultiSpec_Splashscreen"), 
												wxBITMAP_TYPE_PNG_RESOURCE);
		if (splashScreenBitmap.IsOk ())
			{
			wxSize bitmapSize = splashScreenBitmap.GetSize ();
    		new wxSplashScreen (splashScreenBitmap,
										wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
										5000,
										NULL,
										-1,
										wxDefaultPosition,
										bitmapSize,		// wxDefaultSize,
										wxBORDER_SIMPLE|wxSTAY_ON_TOP);
		
			}	// end "if (splashScreenBitmap.IsOk ())"
	
		wxYield ();
	#endif
	*/
	SetTopWindow (pMainFrame);
	SetAppName ("MultiSpec");
	
	pOutputDocTemplate->CreateDocument (wxEmptyString, wxDOC_NEW | wxDOC_SILENT);
	
	bool returnFlag = FALSE;
	#if defined multispec_wxlin
				// Determine if a file has already been selected for opening and set the
				// flag for that in the MainFrame. It will be acted upon during the first
				// idle period.
	
		returnFlag = wxGetEnv (wxT("TOOL_PARAMETERS"), NULL);
	#endif
	pMainFrame->SetToolParametersFlag (returnFlag);

			// Initialize the default directory to read from.
	
	#ifdef NetBeansProject
		gDefaultDataDirectory = wxGetHomeDir ();
	#else
		#ifdef multispec_wxmac
			gDefaultDataDirectory = wxGetHomeDir ();
		#else
			//dialog.SetDirectory ("/apps/multispec/current/data");
			//gDefaultDataDirectory = wxString::FromUTF8 ((char*)getenv ("DATA_PATH"));
			gDefaultDataDirectory = wxString::FromUTF8 ("/data/tools/multispec");
		#endif
	#endif

	return true;

}	// end "OnInit"



void CMultiSpecApp::OnQueryEndSession (
				wxCloseEvent& 						event)

{ 
	if (!CloseTheProject ())
		{
		event.Veto ();
		
		}
	
	else	// CloseTheProject ()
		m_systemTerminationFlag = TRUE;
 
}	// end "OnQueryEndSession"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		SInt32 OpenImageFile
//
//	Software purpose:	The purpose of this routine is to open an image file
//							either multispectral type or Thematic (classification)
//							type.
//
//	Parameters in:		Pointer to structure giving information about the image
//							to use as the selected image file. If this pointer is
//							NULL, then present a dialog box to the use to allow
//							he/she to select the image file.
//							This code was adapted from the Windows version
//
//	Parameters out:	None
//
//	Value Returned:	0 if selected file is not a project file
//							value > 0 indicating the project file version if the
//							selected file is not a project file.
//
// Called By:			OnFileOpen
//							OpenProjectImageWindow	in SProject.cpp
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

}	// end "OpenImageFileLin"



Handle CMultiSpecApp::GetOpenImageFileInfoHandle (void)

{
    return m_openImageFileInfoHandle;
	
}	// end "GetOpenImageFileInfoHandle



Handle CMultiSpecApp::SetUpNewImageDocument (
				Handle								fileInfoHandle,
				SInt16								fileImageType,
				SInt16								windowType)
				
{   
	wxDocument* 						documentPtr = NULL;
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
		FileStringPtr filePathPtr = (FileStringPtr)fileStreamPtr->GetFilePathPPtr ();
      		
		wxString filepathstr = wxString::FromUTF8 ((char*)&filePathPtr[2]);
      documentPtr = pImageDocTemplate->CreateDocument (
      													filepathstr, wxDOC_NEW | wxDOC_SILENT);
		
		}	// end "if (fileStreamPtr != NULL)"
		
	MHSetState (fileInfoHandle, handleStatus);
	
	m_openImageFileInfoHandle = NULL;
	
	continueFlag = (documentPtr != NULL);
	
	if (continueFlag)
		{															
		CMImageWindow*	imageWindowCPtr = 
						((CMImageDoc*)documentPtr)->GetDocImageWindowCPtr ();
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

/*
//------------------------------------------------------------------------------------
// wxFileDialog convenience functions
//------------------------------------------------------------------------------------

wxString wxFileSelector (const wxString& title,
									const wxString& defaultDir,
									const wxString& defaultFileName,
									const wxString& defaultExtension,
									const wxString& filter,
									int flags,
									wxWindow *parent,
									int x,
									int y)

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
	if (!defaultExtension.empty() && filter.empty())
		filter2 = wxString (wxT("*.")) + defaultExtension;
 
	else if (!filter.empty())
		filter2 = filter;

	wxFileDialog fileDialog (parent,
										title,
										defaultDir,
										defaultFileName,
										filter2,
                            	flags,
                            	wxPoint (x, y));

   		// if filter is of form "All files (*)|*|..." set correct filter index
 
	if (!defaultExtension.empty() && filter2.find (wxT('|')) != wxString::npos)
		{
		int filterIndex = 0;

		wxArrayString descriptions, filters;
 
				// don't care about errors, handled already by wxFileDialog
 
		(void)wxParseCommonDialogsFilter (filter2, descriptions, filters);
		for (size_t n=0; n<filters.GetCount (); n++)
			{
			if (filters[n].Contains (defaultExtension))
				{
				filterIndex = n;
				break;
 
				}
 
			}

		if (filterIndex > 0)
			fileDialog.SetFilterIndex (filterIndex);
 
		}	// end "if (!defaultExtension.empty() && ..."

	wxString filename;
	if (fileDialog.ShowModal() == wxID_OK)
    	{
		filename = fileDialog.GetPath ();
 
    	}

	return filename;
 
}	// end "wxFileSelector"



//------------------------------------------------------------------------------------
// wxFileSelectorEx
//------------------------------------------------------------------------------------

wxString wxFileSelectorEx (const wxString& title,
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
	
	wxFileDialog fileDialog (parent,
                            title,
                            defaultDir,
                            defaultFileName,
                            filter,
                            flags, wxPoint (x, y));

	wxString filename;
	if (fileDialog.ShowModal() == wxID_OK)
		{
		if (defaultFilterIndex)
			*defaultFilterIndex = fileDialog.GetFilterIndex ();

		filename = fileDialog.GetPath ();
 
    	}	// end "if (fileDialog.ShowModal() == wxID_OK)"

	return filename;
 
}	// end "wxFileSelectorEx"
*/



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CheckSomeEvents
//
//	Software purpose:	This routine is a stub for the Windows version.
//
//	Parameters in:
//
//	Parameters out:
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1995
//	Revised By:			Larry L. Biehl			Date: 01/29/2019

Boolean CheckSomeEvents (
				UInt16								eventCode)

{
	CShortStatusDlg* 					statusDialogPtr = NULL;
	
	long									yieldEventCode;
	
	UInt32								count = 0;
	
	Boolean								returnFlag = TRUE,
											yieldForReturnFlag = true;


	yieldEventCode = wxEVT_CATEGORY_UI + wxEVT_CATEGORY_USER_INPUT;
	if (eventCode == (osMask+updateMask))
		yieldEventCode = wxEVT_CATEGORY_UI;
	
				// Handle other user interface events in the queue
	
	wxEventLoopBase* eventLoopBasePtr = wxEventLoopBase::GetActive ();
	if (eventLoopBasePtr != NULL)
		{
		while (count<3 && yieldForReturnFlag)
			{
			//yieldForReturnFlag = eventLoopBasePtr->YieldFor (wxEVT_CATEGORY_UI);
			yieldForReturnFlag = eventLoopBasePtr->YieldFor (yieldEventCode);
			
			count++;
				
			}	// end "while (count<5 && returnFlag)"
		/*
		int numberChars = sprintf ((char*)&gTextString3,
											" xMultiSpec:CheckSomeEvents (count): %d%s",
											count,
											gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
		*/
		}	// end "if (eventLoopBasePtr != NULL)"
	
	statusDialogPtr = (CShortStatusDlg*)gStatusDialogPtr;
	if (statusDialogPtr != NULL)
		{
		returnFlag = !statusDialogPtr->m_canceledCommandInitiatedFlag;
		
		if (!returnFlag || gOperationCanceledFlag)
			{
			if (gAlertId != 0 && !gOperationCanceledFlag)
				{
				//eventLoopBasePtr->Yield (false);
				gAlertReturnCode = DisplayAlert (gAlertId,
															 kCautionAlert,
															 gAlertStrID,
															 gAlertStringNumber,
															 0,
															 NULL);
				
				}	// end "if (gAlertId != 0 && !gOperationCanceledFlag"

			gOperationCanceledFlag = TRUE;
			if (gAlertId == 0 || gAlertReturnCode == 3)
						// Quit immediately.
																							return (FALSE);

			gOperationCanceledFlag = FALSE;

			if (gAlertReturnCode == 2)
						// Cancel the quit request.
				gAlertReturnCode = 0;

			returnFlag = TRUE;
			
			if (statusDialogPtr != NULL)
				statusDialogPtr->m_canceledCommandInitiatedFlag = FALSE;

			}	// end "if (!returnFlag || gOperationCanceledFlag)"
		
		}	// end "if (statusDialogPtr != NULL)"
	
	else	// statusDialogPtr == NULL
		{
		returnFlag = !gOperationCanceledFlag;
		
		}	// end "else statusDialogPtr == NULL"

	gNextTime = GetTickCount () + gTimeOffset;
	
	return (returnFlag);

}	// end "CheckSomeEvents"



wxFrame* GetActiveFrame (void)

{ 
	wxFrame* 							frame = NULL;
	int									height,
											width;
	
	
	if (gOutputViewCPtr != NULL && (WindowPtr)gOutputViewCPtr == gTheActiveWindow)
		{
		gOutputViewCPtr->m_frame->GetClientSize (&width, &height);
		if (width > 0 && height > 0)
			frame = gOutputViewCPtr->m_frame;
			
		}	// end "if (gOutputViewCPtr != NULL && ..."
	
	if (frame == NULL && gActiveImageViewCPtr != NULL)
		{
		gActiveImageViewCPtr->m_frame->GetClientSize (&width, &height);
		if (width > 0 && height > 0)
			frame = gActiveImageViewCPtr->m_frame;
			
		}	// end "if (frame == NULL && ..."
		
	if (frame == NULL)
		frame = pMainFrame;
		
	return (frame);
		
}	// end "GetActiveChildFrame"



CMainFrame* GetMainFrame (void)

{
    return pMainFrame;
	
}	// end "GetMainFrame"



time_t LinGetTime (void)

{
	time_t		returnValue = 1;
	
	timespec		tp;
	if (clock_gettime (CLOCK_MONOTONIC, &tp) == 0)
		{
		returnValue = (time_t)(tp.tv_sec*1000 + tp.tv_nsec/1000000);
		
		}	// end "if (clock_gettime (CLOCK_MONOTONIC, &tp) == 0)"
	
	return (returnValue);
	
}	// end "LinGetTime"



void FileUploadProcess::OnTerminate (
				int 									pid,
				int 									status)

{
	int 									numberChars;
	
	/*
	wxLogStatus (m_parent,
						wxT("Process %u ('%s') terminated with exit code %d."),
						pid,
						m_cmd.c_str (),
						status);
	*/
	if (status == 0)
		numberChars = sprintf ((char*)&gTextString3,
										"%sSelected file was uploaded successfully%s",
										gEndOfLine,
										gEndOfLine);
	
	else	// status != 0
		numberChars = sprintf ((char*)&gTextString3,
										"%sSelected file was not uploaded successfully%s",
										gEndOfLine,
										gEndOfLine);
	
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);
	
	pMainFrame->OnAsyncTermination (this);

}	// end "OnTerminate"
