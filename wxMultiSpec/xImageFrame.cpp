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
//	File:						xImageFrame.cpp : class implementation file
//	Class Definition:		xImageFrame.h
//
//	Authors:					Larry L. Biehl, Wei-Kang Hsu, Tsung Tai Yeh
//
//	Revision date:			04/08/2022
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMImageFrame class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" xImageFrame:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "xMultiSpec.h"
#include "xImageFrame.h"
#include "xImageDoc.h"

#include "wx/display.h"

#ifdef multispec_wxlin
	#include "xToolBar_images.cpp"
	#include "xCursor_images.cpp"
#endif

#define kDefaultMaxLegendWidth 170

Boolean CMImageFrame::s_forcePaletteBackgroundFlag = FALSE;


IMPLEMENT_DYNAMIC_CLASS(CMImageFrame, wxDocChildFrame)


BEGIN_EVENT_TABLE (CMImageFrame, wxDocChildFrame)
	EVT_CHAR_HOOK (CMImageFrame::OnCharHook)
   EVT_KEY_DOWN (CMImageFrame::OnKeyDown)
	EVT_MAXIMIZE (CMImageFrame::OnMaximizeWindow)

	EVT_MENU (ID_DOWN_ARROW, CMImageFrame::DoKeyDownDownArrow)
	EVT_MENU (ID_LEFT_ARROW, CMImageFrame::DoKeyDownLeftArrow)
	EVT_MENU (ID_RIGHT_ARROW, CMImageFrame::DoKeyDownRightArrow)
	EVT_MENU (ID_UP_ARROW, CMImageFrame::DoKeyDownUpArrow)
	EVT_MENU (ID_FILE_SAVE_AS, CMImageFrame::OnFileSaveAs)
	EVT_MENU (ID_VIEW_COORDINATES_BAR, CMImageFrame::OnViewCoordinatesBar)
	EVT_MENU (ID_WINDOW_NEW_SELECTION_GRAPH, CMImageFrame::OnWindowNewSelectionGraph)
	EVT_MENU (ID_CANCEL_DRAW, CMImageFrame::OnCancelDraw)

	EVT_SASH_DRAGGED_RANGE (ID_WINDOW_TOP, ID_WINDOW_LEFT, CMImageFrame::OnSashDrag)

	EVT_SIZE (CMImageFrame::OnSize)

	EVT_UPDATE_UI (ID_FILE_SAVE_AS, CMImageFrame::OnUpdateFileSaveAs)
	EVT_UPDATE_UI (ID_VIEW_COORDINATES_BAR, CMImageFrame::OnUpdateViewCoordinatesBar)
	EVT_UPDATE_UI (ID_WINDOW_NEW_SELECTION_GRAPH, CMImageFrame::OnUpdateWindowNewSelectionGraph)
	EVT_UPDATE_UI (wxID_PRINT, CMImageFrame::OnUpdateFilePrint)
	EVT_UPDATE_UI (wxID_PREVIEW, CMImageFrame::OnUpdateFilePrintPreview)
	EVT_UPDATE_UI (wxID_PAGE_SETUP, CMImageFrame::OnUpdateFilePrintSetup)
	EVT_UPDATE_UI (ID_OVERLAY, CMImageFrame::OnUpdateOverlay)
END_EVENT_TABLE ()


CMImageFrame::CMImageFrame ()

{
	m_imageViewCPtr = NULL;
	m_imageFrameActiveFlag = FALSE;
	m_forcePaletteBackgroundUpdateFlag = FALSE;
	m_coordinatesBar = NULL;
	m_frameMaximized = FALSE;
	
}	// end "CMImageFrame"



CMImageFrame::CMImageFrame (
				wxDocument*							doc,
				wxView*								view,
				wxDocParentFrame*					parent)
		: wxDocChildFrame (doc,
									view,
									parent,
									wxID_ANY,
									wxT("Image Frame"),
									wxDefaultPosition,
									wxDefaultSize,
									#if defined multispec_wxmac
										wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
									#else
										wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP)
									#endif
{
			// Initialize variables
	
	#if defined multispec_wxmac
		wxBitmap overlayi = wxBITMAP_PNG (overlay);
		wxBitmap zoom1 = wxBITMAP_PNG (zoomx1);
		wxBitmap zoomout = wxBITMAP_PNG (zoom_out);
		wxBitmap zoomin = wxBITMAP_PNG (zoom_in);
	#else
		wxBitmap overlayi = wxBITMAP_PNG_FROM_DATA (overlay);
		wxBitmap zoom1 = wxBITMAP_PNG_FROM_DATA (zoomx1);
		wxBitmap zoomout = wxBITMAP_PNG_FROM_DATA (zoom_out);
		wxBitmap zoomin = wxBITMAP_PNG_FROM_DATA (zoom_in);
	#endif
	
			// Get blink open and shut cursors for thematic images
	
	#if defined multispec_wxmac
		wxBitmap blinkOpen = wxBITMAP_PNG (blink_open);
		wxBitmap blinkShut = wxBITMAP_PNG (blink_shut);
	#else
		wxBitmap blinkOpen = wxBITMAP_PNG_FROM_DATA (blink_open);
		wxBitmap blinkShut = wxBITMAP_PNG_FROM_DATA (blink_shut);
	#endif
	
	wxImage blink_open_image = blinkOpen.ConvertToImage ();
	blink_open_image.SetOption (wxIMAGE_OPTION_CUR_HOTSPOT_X, 16);
	blink_open_image.SetOption (wxIMAGE_OPTION_CUR_HOTSPOT_Y, 16);
	m_blinkOpenCursor = wxCursor (blink_open_image);
	
	wxImage blink_shut_image = blinkShut.ConvertToImage ();
	blink_shut_image.SetOption (wxIMAGE_OPTION_CUR_HOTSPOT_X, 16);
	blink_shut_image.SetOption (wxIMAGE_OPTION_CUR_HOTSPOT_Y, 16);
	m_blinkShutCursor = wxCursor (blink_shut_image);
	
	m_imageViewCPtr = (CMImageView*)view;
	m_imageFrameActiveFlag = FALSE;
	m_forcePaletteBackgroundUpdateFlag = FALSE;
	m_coordinatesBar = NULL;
	m_frameMaximized = FALSE;

	SetMinSize (wxSize (150, 150));

			// CMCoordinatebar window
	
	wxSashLayoutWindow* win = new wxSashLayoutWindow (this,
																		ID_WINDOW_TOP,
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxNO_BORDER | wxSW_3D);
	win->SetDefaultSize (wxSize (1000, 40));
	win->SetOrientation (wxLAYOUT_HORIZONTAL);
	win->SetAlignment (wxLAYOUT_TOP);
	
			// Hide it after creation
	
	win->Show (false);
	m_topWindow = win;

			// CMTitle window to be above image and below coordinate bar
	
	win = new wxSashLayoutWindow (this,
											ID_WINDOW_TITLE,
											wxDefaultPosition,
											wxDefaultSize,
											wxNO_BORDER | wxSW_3D);
	win->SetDefaultSize (wxSize (1000, 15));
	win->SetOrientation (wxLAYOUT_HORIZONTAL);
	win->SetAlignment (wxLAYOUT_TOP);
	
			// Hide it after creation
	
	win->Show (false);
	m_titleWindow = win;

			// A window to the left of the client window for CMLegendview
	
	win = new wxSashLayoutWindow (this,
											ID_WINDOW_LEFT,
											wxDefaultPosition,
											wxSize (kDefaultMaxLegendWidth, 30),
											wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
	win->SetDefaultSize (wxSize (0, 0));
	win->SetOrientation (wxLAYOUT_VERTICAL);
	win->SetAlignment (wxLAYOUT_LEFT);
	win->SetSashVisible (wxSASH_RIGHT, true);
	
			// Hide it after creation
	
	win->Show (false);
	m_leftWindow = win;

			// Main window for displaying image
	
	win = new wxSashLayoutWindow (this,
											ID_WINDOW_RIGHT,
											wxDefaultPosition,
											wxSize (200, 30),
											wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
	win->SetDefaultSize (wxSize (120, 1000));
	win->SetOrientation (wxLAYOUT_VERTICAL);
	win->SetAlignment (wxLAYOUT_RIGHT);
	m_mainWindow = win;
	
	#if defined multispec_wxmac
				// Note: found that tool tip has to be set after the control is added to the
				// tool bar. If not it will not show.
	
		m_toolBar = CreateToolBar (wxTB_HORIZONTAL, wxID_ANY);
		m_toolBar->SetToolBitmapSize (wxSize (16, 16));

   	wxBitmapButton* bpButtonX1 = new wxBitmapButton (m_toolBar,
																			ID_MAGNIFICATION,
																			zoom1,
																			wxDefaultPosition,
																			//wxDefaultSize,
																			wxSize (16, 16),
																			wxBORDER_NONE+wxBU_EXACTFIT);
		bpButtonX1->Bind (wxEVT_LEFT_DOWN, &CMImageFrame::DoZoomToOne, this);
		m_toolBar->AddControl (bpButtonX1);
   	bpButtonX1->SetToolTip (wxT("Zoom image to X1.0 magnification"));

   	wxBitmapButton* bpButtonZoomIn = new wxBitmapButton (
   																		m_toolBar,
																			ID_ZOOM_IN,
																			zoomin,
																			wxDefaultPosition,
																			wxSize (16, 16),
																			wxBORDER_NONE+wxBU_EXACTFIT);
		bpButtonZoomIn->Bind (wxEVT_LEFT_DOWN, &CMImageFrame::DoZoomIn, this);
		bpButtonZoomIn->Bind (wxEVT_LEFT_DCLICK, &CMImageFrame::DoZoomIn, this);
		m_toolBar->AddControl (bpButtonZoomIn);
   	bpButtonZoomIn->SetToolTip (
   				wxT("Zoom into image. Use shift key to zoom as fast as system can."));

   	wxBitmapButton* bpButtonZoomOut = new wxBitmapButton (
   																		m_toolBar,
																			ID_ZOOM_OUT,
																			zoomout,
																			wxDefaultPosition,
																			wxSize (16, 16),
																			wxBORDER_NONE+wxBU_EXACTFIT);
		//SetUpToolTip (bpButton, IDS_ToolTip40);
		bpButtonZoomOut->Bind (wxEVT_LEFT_DOWN, &CMImageFrame::DoZoomOut, this);
		bpButtonZoomOut->Bind (wxEVT_LEFT_DCLICK, &CMImageFrame::DoZoomOut, this);
		m_toolBar->AddControl (bpButtonZoomOut);
   	bpButtonZoomOut->SetToolTip (
				wxT("Zoom out of image. Use shift key to zoom as fast as system can."));
	
		m_zoomText = new wxStaticText (m_toolBar,
													ID_ZOOMINFO,
													wxT(""),
													wxDefaultPosition,
													wxSize (50, 16));
		m_toolBar->AddControl (m_zoomText, wxT("add a fixed text"));
		wxFont currentFont = m_toolBar->GetFont ();
		currentFont.SetPointSize (gFontSize+1);
		currentFont.SetWeight (wxFONTWEIGHT_LIGHT);
		m_zoomText->SetFont (currentFont);
   	m_zoomText->SetToolTip (wxT("Displays current image magnification"));

   	wxBitmapButton* bpButtonOverlay = new wxBitmapButton (
   																		m_toolBar,
																			ID_OVERLAY,
																			overlayi,
																			wxDefaultPosition,
																			wxSize (16, 16),
																			wxBORDER_NONE+wxBU_EXACTFIT);
		bpButtonOverlay->Bind (wxEVT_BUTTON,
										&CMainFrame::OnToolBarShowOverlay,
										GetMainFrame());
		m_toolBar->AddControl (bpButtonOverlay);
		m_toolBar->EnableTool (ID_OVERLAY, false);
   	bpButtonOverlay->SetToolTip (wxT("Control image and vector overlays on image"));
	
		m_toolBar->Realize ();
	#endif	// defined multispec_wxmac

	Layout ();

			// Set default location for image window.
	
	int							menuHeight,
									xLocation,
									yLocation,
									xSize,
									offsetCount;
	 
	GetMainFrame()->m_menubar1->GetSize (&xSize, &menuHeight);
	offsetCount = div (gNumberOfIWindows-1, 5).rem;
	xLocation = 3 + offsetCount * 10;
	#if defined NetBeansProject
		yLocation = 110;
	#elif defined multispec_wxmac
		yLocation = menuHeight + 3;
	#else
   	int		toolBarHeight, toolBarWidth;
	
		GetMainFrame()->m_toolBar1->GetSize (&toolBarWidth, &toolBarHeight);
	
				// Allow 2 pixel space below toolbar
	
		yLocation = 20 + menuHeight + toolBarHeight + 2;
	#endif
	yLocation += offsetCount * 10;
	Move (xLocation, yLocation);
	
			// SetSize ... use this eventually when one knows the size of the image.

			// Set global variables since this is now the active image window
	
	gActiveImageWindow = m_imageViewCPtr;
	gActiveImageViewCPtr = m_imageViewCPtr;
	gActiveWindowType = kImageWindowType;
	gTheActiveWindow = (WindowPtr)m_imageViewCPtr;

			// Set the image window name

    Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
    Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
    FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
    GetImageWindowName (NULL,
								fileInfoPtr,
								gTextString,
								FALSE);
    wxString ntitle = wxString::FromUTF8 ((char*)&gTextString[1]);
    SetLabel (ntitle);
    
    m_frameMaximized = false;
    m_frameSizeSaved = GetSize ();
	 
			// Set Accelerator Table
	
	wxAcceleratorEntry entries[39];
	entries[0].Set (wxACCEL_CTRL, (int) 'O', ID_IMAGE_OPEN);
	entries[1].Set (wxACCEL_CTRL, (int) ';', ID_FILE_OPEN_PROJECT);
	entries[2].Set (wxACCEL_CTRL, (int) 'W', ID_FILE_CLOSE_WINDOW);
	entries[3].Set (wxACCEL_CTRL, (int) 'S', ID_FILE_SAVE_AS);
	entries[4].Set (wxACCEL_CTRL, (int) 'P', ID_FILE_PRINT);
	entries[5].Set (wxACCEL_CTRL, (int) 'Q', wxID_EXIT);
	
	entries[6].Set (wxACCEL_CTRL, (int) 'Z', ID_EDIT_UNDO);	// wxID_UNDO
	entries[7].Set (wxACCEL_CTRL, (int) 'X', wxID_CUT);
	entries[8].Set (wxACCEL_CTRL, (int) 'C', wxID_COPY);
	entries[9].Set (wxACCEL_CTRL, (int) 'V', wxID_PASTE);
	entries[10].Set (wxACCEL_NORMAL, (int) WXK_DELETE, wxID_CLEAR);
	entries[11].Set (wxACCEL_CTRL, (int) 'A', ID_EDIT_SELECT_ALL);
	entries[12].Set (wxACCEL_CTRL, (int) ',', ID_EDIT_IMAGE_DESCRIPTION);
	entries[13].Set (wxACCEL_CTRL, (int) '-', ID_EDIT_IMAGE_MAP_PARAMETERS);
	
	entries[14].Set (wxACCEL_CTRL, (int) '1', ID_VIEW_COORDINATES_BAR);
	
	entries[15].Set (wxACCEL_CTRL, (int) 'D', ID_PROC_DISPLAY_IMAGE);
	entries[16].Set (wxACCEL_CTRL, (int) 'H', ID_PROC_HISTOGRAM_IMAGE);
	entries[17].Set (wxACCEL_CTRL, (int) 'J', ID_PROC_LISTDATA);
	entries[18].Set (wxACCEL_CTRL, (int) 'R', ID_PROC_REFORMAT_CHANGE_IMAGE);
	entries[19].Set (wxACCEL_CTRL, (int) 'L', ID_PROC_CLUSTER);
	entries[20].Set (wxACCEL_CTRL, (int) 'T', ID_PROC_STATISTICS);
	entries[21].Set (wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	entries[22].Set (wxACCEL_CTRL, (int) 'F', ID_PROC_FEATURE_EXTRACTION);
	entries[23].Set (wxACCEL_CTRL, (int) 'B', ID_PROC_FEATURE_SELECTION);
	entries[24].Set (wxACCEL_CTRL, (int) 'M', ID_PROC_CLASSIFY);
	entries[25].Set (wxACCEL_CTRL, (int) 'Y', ID_PROC_LISTRESULTS);
	entries[26].Set (wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	
	entries[27].Set (wxACCEL_CTRL, (int) 'K', ID_PROC_UTIL_PRIN_COMP_ANALYSIS);
	entries[28].Set (wxACCEL_CTRL, (int) 'I', ID_PROC_UTIL_CREATE_STAT_IMAGE);
	entries[29].Set (wxACCEL_CTRL, (int) 'G', ID_PROC_UTIL_BIPLOTS_OF_DATA);
	entries[30].Set (wxACCEL_CTRL, (int) 'U', ID_PROC_UTIL_LIST_IMAGE_DESC);
	entries[31].Set (wxACCEL_CTRL, (int) '[', ID_PROC_UTIL_CHECK_COVARIANCES);
	entries[32].Set (wxACCEL_CTRL, (int) ']', ID_PROC_UTIL_CHECKTRANS_MATRIX);
	
	entries[33].Set (wxACCEL_CTRL, (int) '2', ID_WINDOW_NEW_SELECTION_GRAPH);
	
	entries[34].Set (wxACCEL_NORMAL, (int) WXK_LEFT, ID_LEFT_ARROW);
	entries[35].Set (wxACCEL_NORMAL, (int) WXK_RIGHT, ID_RIGHT_ARROW);
	entries[36].Set (wxACCEL_NORMAL, (int) WXK_UP, ID_UP_ARROW);
	entries[37].Set (wxACCEL_NORMAL, (int) WXK_DOWN, ID_DOWN_ARROW);
	entries[38].Set (wxACCEL_CTRL, (int) '.', ID_CANCEL_DRAW);
	wxAcceleratorTable accel (39, entries);
	SetAcceleratorTable (accel);

}	// end "CMImageFrame"



CMImageFrame::~CMImageFrame () 

{

}	// end "~CMImageFrame"



void CMImageFrame::ActivateImageWindowItems (
				Boolean 								activateFlag,
				Boolean 								changeWindowFlag)

{
	double								magnification;
	
   
	gNewSelectedWindowFlag = FALSE;
	
	if ((activateFlag != m_imageFrameActiveFlag))
		{
		m_forcePaletteBackgroundUpdateFlag = TRUE;

				// If this image window is going inactive indicate that a palette
			  // change must be force if one does not occur so that the
			  // thematic palette will be realized in the background. It may
			  // not be if the new active image window is of multispectral type
			  // and the colors do not change.

		if (!activateFlag)
			s_forcePaletteBackgroundFlag = TRUE;

		}	// end "if ((activateFlag != m_imageFrameActiveFlag) && ..."

	if (activateFlag && changeWindowFlag) 
		{
		m_imageFrameActiveFlag = TRUE;
		m_imageLegendViewCPtr->GetLegendListCPtr()->SetLegendListActiveFlag (TRUE);
		
				// Force the cursor to be reselected
		
		gPresentCursor = -1;
		
				// Update the zoom value in the status bar to reflect this active
				// image window
		
		magnification = -1;
		if (m_imageViewCPtr->ImageWindowIsAvailable ())
			{
			CMDisplay* displayMultiCPtr = m_imageViewCPtr->m_displayMultiCPtr;
			magnification = displayMultiCPtr->GetMagnification ();

			}	// end "if (m_imageViewCPtr->ImageWindowIsAvailable ())"
      
		GetMainFrame()->UpdateStatusBar (this, magnification);
      
				// Handle project related items.
				
		Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

		if (windowInfoPtr->projectWindowFlag) 
			{
			gProjectSelectionWindow = m_imageViewCPtr;

			LoadNewFieldListBox ();

			}	// end "if (windowInfoPtr->projectWindowFlag)"

		ShowGraphSelection ();

      if (gSelectionGraphViewCPtr != NULL && gProcessorCode != kDisplayProcessor)
      			// If the display processor is active, the information needed has not
      			// been read yet.
			UpdateSelectionWindowList ();
      
				// This flag is used in selections to try to differentiate between a user
				// making a selection or just selecting the image to work on.

		gNewSelectedWindowFlag = TRUE;
		
		}	// end "if (bActivate && changeWindowFlag)"

	if (!activateFlag && changeWindowFlag) 
		{
		m_imageFrameActiveFlag = FALSE;
		m_imageLegendViewCPtr->GetLegendListCPtr()->SetLegendListActiveFlag (FALSE);

		}	// end "if (!activateFlag && changeWindowFlag)"

		// Make sure that the flag indicating that the control key is down
		// is set to FALSE. It may be TRUE if a menu command was processed
		// using quick keys.

	if (m_imageViewCPtr != NULL)
		m_imageViewCPtr->SetControlKeyFlag (FALSE);

}	// end "ActivateImageWindowItems"



void CMImageFrame::ChangeClassGroupDisplay (
				SInt16								newClassGroupCode)
				 
{
	SInt16 currentClassGroupCode = m_imageViewCPtr->GetClassGroupCode ();

	if (currentClassGroupCode != 0 && newClassGroupCode != currentClassGroupCode)
		{
		UpdateActiveImageLegend (newClassGroupCode, kCallCreatePalette);
		
		if (currentClassGroupCode == kClassDisplay ||
												newClassGroupCode == kClassDisplay)
			{
      	Handle displaySpecsHandle = m_imageViewCPtr->GetDisplaySpecsHandle ();
      	DisplaySpecsPtr displaySpecsPtr =
										(DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
		
			displaySpecsPtr->paletteObject->SetPaletteLoadedFlag (FALSE);
			
			}	// end "if (currentClassGroupCode != kGroupDisplay && ..."

		}	// end "if (classGroupCode != m_classGroupCode + 1)"
	
}	// end "ChangeClassGroupDisplay"
 
 

void CMImageFrame::DoKeyDownDownArrow (
				wxCommandEvent& 					event)

{
	char  theChar = 0x28;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownDownArrow"
 


void CMImageFrame::DoKeyDownLeftArrow (
				wxCommandEvent& 					event)

{
	char  theChar = 0x25;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownLeftArrow"
 


void CMImageFrame::DoKeyDownRightArrow (
				wxCommandEvent& 					event)

{
	char  theChar = 0x27;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownRightArrow"
 


void CMImageFrame::DoKeyDownUpArrow (
				wxCommandEvent& 					event)

{
	char  theChar = 0x26;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownUpArrow"



void CMImageFrame::DoZoomIn (
				wxMouseEvent&						event)

{
			// Verify that the global variable for this image view are
			// correct. It seems at times the activate is not called if the zoom
			// buttons are selected for a zoom operation when the image window is
			// not the active window.
	
	if (gActiveImageViewCPtr != m_imageViewCPtr)
		UpdateActiveImageWindowInfo ();
		
	GetMainFrame()->SetZoomCode (ID_ZOOM_IN);
	m_imageViewCPtr->ZoomIn ();
	
   m_frameMaximized = false;
	
			// Prepare for case when user holds mouse button down.
	
	GetMainFrame()->SetNextControlTime (gControlOffset);
	
	event.Skip ();

}	// end "DoZoomIn"



void CMImageFrame::DoZoomOut (
				wxMouseEvent&						event)

{
			// Verify that the global variable for this image view are
			// correct. It seems at times the activate is not called if the zoom
			// buttons are selected for a zoom operation when the image window is
			// not the active window.
	
	if (gActiveImageViewCPtr != m_imageViewCPtr)
		UpdateActiveImageWindowInfo ();
	
	GetMainFrame()->SetZoomCode (ID_ZOOM_OUT);
	m_imageViewCPtr->ZoomOut ();
	
   m_frameMaximized = false;
	
			// Prepare for case when user holds mouse button down.
	
	GetMainFrame()->SetNextControlTime (gControlOffset);
	
	event.Skip ();

}	// end "DoZoomOut"



void CMImageFrame::DoZoomToOne (
				wxMouseEvent&						event)

{
	GetMainFrame()->SetZoomCode (ID_MAGNIFICATION);
	m_imageViewCPtr->ZoomSize ();
	
   m_frameMaximized = false;
	
	GetMainFrame()->SetZoomCode (0);
	
	event.Skip ();

}	// end "DoZoomToOne"



Boolean CMImageFrame::GetActiveWindowFlag (void)

{
	return (m_imageFrameActiveFlag);

}	// end "GetActiveWindowFlag"



void CMImageFrame::GetMinimumDisplaySizeForImage (
				Handle 								windowInfoHandle,
				LongPoint* 							minimumWindowSizePointPtr)

{
	LongPoint 							offScreenPoint;
	
   double 								savedOrigin[2];

   DisplaySpecsPtr 					displaySpecsPtr = NULL;
   WindowInfoPtr 						windowInfoPtr = NULL;

	

	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	displaySpecsPtr =
				(DisplaySpecsPtr)GetHandlePointer (GetDisplaySpecsHandle (windowInfoPtr));
	
   if (displaySpecsPtr != NULL)
   	{
      savedOrigin[kVertical] = displaySpecsPtr->origin[kVertical];
      savedOrigin[kHorizontal] = displaySpecsPtr->origin[kHorizontal];

      displaySpecsPtr->origin[kVertical] = 0;
      displaySpecsPtr->origin[kHorizontal] = 0;

				 // Note that this will include the height of the coordinate bar if
				 // it is showing since the coordinate bar is part of the client area
				 // of the window.
		
      offScreenPoint.v = displaySpecsPtr->imageDimensions[kVertical];
      offScreenPoint.h = displaySpecsPtr->imageDimensions[kHorizontal];
      ConvertOffScreenPointToWinPoint (
      					windowInfoHandle, &offScreenPoint, minimumWindowSizePointPtr);

      displaySpecsPtr->origin[kVertical] = savedOrigin[kVertical];
      displaySpecsPtr->origin[kHorizontal] = savedOrigin[kHorizontal];

      		// Adjust the size needed to allow for scroll bars and
      		// one pixel extra.
	
      int 		coordinateBarHeight = 0,
      	 		titleBarHeight = 0;
		
      UInt16 	amountToAllowForHStuff,
      			amountToAllowForVStuff;
	
				// Get the total of the title and toolbar height
		
		wxRect wxFrameRect = GetRect ();
		wxRect wxFrameClientRect = GetClientRect ();
		int titleToolBarHeight = wxFrameRect.height - wxFrameClientRect.height;
		
      		// Get coordinate window height
		
      coordinateBarHeight = GetCoordinateHeight (windowInfoHandle);
      titleBarHeight = GetTitleHeight (windowInfoHandle);

      #ifdef NetBeansProject
			amountToAllowForHStuff = 7;
			amountToAllowForVStuff = 33;
      #else	// mygeohub
			#if defined multispec_wxmac
						// Allow 2 pixels left/top and 4 pixels right/bottom around
						// edge for border
		
				titleToolBarHeight += coordinateBarHeight + titleBarHeight;
		
				amountToAllowForHStuff = 2 + 4 + wxFrameRect.width -
																				wxFrameClientRect.width;
				amountToAllowForVStuff = 2 + 4 + titleToolBarHeight;
			#else
				titleToolBarHeight += coordinateBarHeight + titleBarHeight;
		
				amountToAllowForHStuff = 2 * 2 + wxFrameRect.width -
																				wxFrameClientRect.width;
				amountToAllowForVStuff = 2 * 2 + titleToolBarHeight;
			#endif
		#endif

      minimumWindowSizePointPtr->v += amountToAllowForVStuff;
		
      minimumWindowSizePointPtr->h += amountToAllowForHStuff;
		
      		// The image size needed has to be larger than the minimum.

      minimumWindowSizePointPtr->v = MAX (minimumWindowSizePointPtr->v,
      												gGrowAreaMinimum);
      minimumWindowSizePointPtr->h = MAX (minimumWindowSizePointPtr->h,
      												gGrowAreaMinimum + windowInfoPtr->legendWidth);
	
   	}	// end "if (displaySpecsPtr != NULL)"
	
   else // displaySpecsPtr == NULL
   	{
      minimumWindowSizePointPtr->h = gGrowAreaMinimum;
      minimumWindowSizePointPtr->v = gGrowAreaMinimum;
		
   	}	// end "else displaySpecsPtr == NULL"

}	// end "GetMinimumDisplaySizeForImage"



int CMImageFrame::GetTitleAndToolBarHeight (void)

{
	wxRect wxFrameRect = GetRect ();
	wxRect wxFrameClientRect = GetClientRect ();

	return (wxFrameRect.height - wxFrameClientRect.height);

}	// end "GetTitleAndToolBarHeight"



void CMImageFrame::InitialUpdate (CSize imageViewSize) 

{
	/*
			// For Linux after displaying image
			// Tweak this routine for optimal display
			
	CRect rectFrame;

	GetWindowRect (rectFrame);
	CSize size = rectFrame.Size ();

			// This represents just the size allowed for the image. This was
			// set by the ResizeParentToFit call in CMImageView::OnInitialUpdate
			// routine. Add the amount needed for the legend and the split window bar.

	size.cx += kDefaultMaxLegendWidth;
	SetWindowPos (NULL, 0, 0, size.cx, size.cy,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	m_wndSplitter.SetColumnInfo (0, 170, 0);
	m_wndSplitter.SetColumnInfo (1, imageViewSize.cx, 0);
	m_wndSplitter.SetRowInfo (0, imageViewSize.cy, 0);

	m_wndSplitter.RecalcLayout ();

	Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer (
																			windowInfoHandle);

	if (windowInfoPtr != NULL) 
		{
		windowInfoPtr->showLegend = TRUE;
		windowInfoPtr->legendWidth = kDefaultMaxLegendWidth;

		}	// end "if (windowInfoPtr != NULL)"

	m_imageLegendViewCPtr->SetLegendWidthSetFlag ();

	m_imageLegendViewCPtr->AdjustLegendListLength ();
	*/
}	// end "InitialUpdate"



void CMImageFrame::OnActivate (
				wxActivateEvent&					event)

{
	m_imageFrameActiveFlag = true;
	if (event.GetActive () && m_mainWindow)
		m_mainWindow->SetFocus ();
	
}	// end "OnActivate"



void CMImageFrame::OnCancelDraw (
				wxCommandEvent& 					event)

{
	if (gProcessorCode == kDisplayProcessor)
		gOperationCanceledFlag = TRUE;

}	// end "OnCancelDraw"



void CMImageFrame::OnChar (
				wxKeyEvent& 						event)

{
	/*
	int numberChars = sprintf ((char*)gTextString3,
				" xImageFrame:OnChar (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
	*/
	if (event.GetEventType() == wxEVT_KEY_DOWN && gProcessorCode != 0)
		{
		
		}	// end "if (event.GetEventType() == wxEVT_KEY_DOWN)"
	
	event.Skip ();
	
}	// end "OnChar"



void CMImageFrame::OnCharHook (
				wxKeyEvent&							event)

{
	/*
	int numberChars = sprintf ((char*)gTextString3,
				" xImageFrame:OnCharHook (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
	*/
	int keyCode = event.GetKeyCode ();
	int keyUP = WXK_UP;
	int keyDOWN = WXK_DOWN;
	if ((keyCode == WXK_UP || keyCode == WXK_DOWN) && event.ShiftDown())
		{
		char	theChar;
		if (keyCode == WXK_UP)
			theChar = 0x26;
			
		else	// keyCode == WXK_DOWN
			theChar = 0x28;
			
		DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);
		event.Skip (false);
		
		}	// if ((event.GetKeyCode () == WXK_UP || ...
	
	event.Skip ();
	
}	// end "OnCharHook"



void CMImageFrame::OnFileSave (
				wxCommandEvent& 					event)

{
	gProcessorCode = kSaveProcessor;
	WriteThematicInfo (NULL, 0);
	gProcessorCode = 0;

}	// end "OnFileSave"



void CMImageFrame::OnFileSaveAs (
				wxCommandEvent& 					event)

{
	gProcessorCode = kSaveOutputAsProcessor;
	SaveImageWindowAs ();
	gProcessorCode = 0;

}	// end "OnFileSaveAs"



void CMImageFrame::OnKeyDown (
				wxKeyEvent& 						event)

{
	SInt16								windowType;
	
	Boolean								hasCaptureFlag;
	
	
			// Only handle if this window is not captured and a processor is not
			// in operation.
	
	hasCaptureFlag = HasCapture ();
	
   if (!hasCaptureFlag && gProcessorCode == 0)
		{
				// Also only handle if cursor is over image portion of a thematic image
				// window.
		
		windowType = m_imageViewCPtr->GetWindowType ();
		
		}	// end "if (!hasCaptureFlag && gProcessorCode == 0)"

}	// end "OnKeyDown"



void CMImageFrame::OnMaximizeWindow (
				wxMaximizeEvent& 					event)

{
   wxRect 								tempArea;
	
	wxPoint 								imgNewOrig = GetPosition (),
   	 									mainOrig;
	
   WindowInfoPtr 						windowInfoPtr = NULL;
	
   int									clientWidth,
											clientHeight,
         								legendHeight = 0,
         								legendWidth = 0,
         								coordinateBarHeight = 0;
	
	
	#if defined multispec_wxlin
   	Maximize (false);
	#endif
	
	if (m_frameMaximized)
		{
				// currently maximized, set to the saved values
		
		SetSize (m_frameSizeSaved.x, m_frameSizeSaved.y);
		m_frameMaximized = false;
		Restore ();
		SetPosition (imgNewOrig);
		
				// Adjusting the scrollbar is not effective now, not sure why @ 07.11.16
		
		m_imageViewCPtr->m_Canvas->SetScrollPos (wxVERTICAL, m_scrollPos.y, true);
		m_imageViewCPtr->m_Canvas->SetScrollPos (wxHORIZONTAL, m_scrollPos.x, true);
		
      }	// end "if (m_frameMaximized)"
	
	else	// Currently unmaximized
		{
		UInt16 					mainFrameBarHeight = 0,
									otherLegendStuffHeight = 0;
		
		m_frameSizeSaved = GetSize ();
		m_scrollPos = m_imageViewCPtr->m_Canvas->GetScrollPosition ();
		
		Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
		
		GetMinimumDisplaySizeForImage (windowInfoHandle, &m_TempLongPoint);
		m_frameMaximized = true;
		
		int titleToolBarHeight = GetTitleAndToolBarHeight ();
		
		#ifdef NetBeansProject
			GetMainFrame()->GetClientSize (&clientWidth, &clientHeight);
			GetMainFrame()->GetPosition (&mainOrig.x, &mainOrig.y);
		
			mainFrameBarHeight = 150;
			otherLegendStuffHeight = 130;
		#else	// mygeohub or multispec_wxmac
			#if defined multispec_wxlin
   			int		menuHeight,
   						menuWidth,
   						toolBarHeight,
   						toolBarWidth;
	
				GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
				GetMainFrame()->m_toolBar1->GetSize (&toolBarWidth, &toolBarHeight);
		
				GetMainFrame()->GetClientSize (&clientWidth, &clientHeight);
				GetMainFrame()->GetPosition (&mainOrig.x, &mainOrig.y);
		
						// Allow a some space for the window border
		
				mainOrig.x = 2;

						// Need toallow for MultiSpec Title line in MyGeoHub Workspace
		
				mainFrameBarHeight = 27 + menuHeight + toolBarHeight;
				otherLegendStuffHeight = titleToolBarHeight;
				clientHeight -= 33;
		
						// Allow for space around right edge of image frame within
						// the display area
		
				clientWidth -= 10;
			#endif
			#if defined multispec_wxmac
				wxRect	clientRect;
		
				clientRect = wxDisplay().GetClientArea ();
		
				clientWidth = clientRect.GetWidth ();
				clientHeight = clientRect.GetHeight ();
				mainOrig.x = 2;
				mainOrig.y = 2;
		
				mainFrameBarHeight = clientRect.GetTop ();
		
						// Set space needed for Title/Tool bar and
						// Classes & Palette dropdown menus
		
				otherLegendStuffHeight = titleToolBarHeight;
		
						// Allow for 2 pixel space left/top and 4 pixels right/bottom
						// around image frame within the display area
		
				clientHeight -= 6;
				clientWidth -= 6;
			#endif
		#endif
		
				// Set the window height to max (legend height, image size)
		
		//if (legendWidth > 0)
		if (windowInfoPtr->windowType == kThematicWindowType)
			{
			legendWidth = windowInfoPtr->legendWidth;
			
			if (GetCoordinateHeight (windowInfoHandle) > 0)
				{
						// Has coordinate bar
				
				tempArea = (m_imageViewCPtr->m_frame)->m_topWindow->GetClientRect ();
				coordinateBarHeight = tempArea.GetBottom ();
				
				}	// end "if (GetCoordinateHeight (windowInfoHandle) > 0)"
			
			if (legendWidth > 1)
				{
						// Get height for class and palette controls for thematic images
						// Note that if the legend width is less than or equal to 1, the
						// legend is not being displayed. Ignore it.
			
				int			hx,
								hy;
				
				m_imageLegendViewCPtr->FindWindow (IDC_COMBO1)->GetSize (&hx, &hy);
				otherLegendStuffHeight += hy;
				
				m_imageLegendViewCPtr->FindWindow (IDC_PaletteCombo)->GetSize (&hx, &hy);
				otherLegendStuffHeight += hy;
				
						// Get the height of one row
				
				#if defined multispec_wxlin
					int legendItemHeight = 23;
				#endif
				#if defined multispec_wxmac
					int legendItemHeight = 21;
				#endif
				
				legendHeight = m_imageViewCPtr->m_frame->m_imageLegendViewCPtr->
								m_legendListBox->m_ilist->GetImageCount () * legendItemHeight;
				
						// Allow for extra space around the combo boxes
				
				#if defined multispec_wxlin
					int extraSpace = 32;
				#endif
				#if defined multispec_wxmac
					int extraSpace = 10;
				#endif

				legendHeight += otherLegendStuffHeight + coordinateBarHeight + extraSpace;
				
				m_TempLongPoint.v = MAX (m_TempLongPoint.v, legendHeight);
				
				}	// end "if (legendWidth > 1)"
			
			}	// end "if (windowInfoPtr->windowType == kThematicWindowType)"
		
				// set the image frame height no larger than the client height
		
		if (m_TempLongPoint.v > clientHeight)
			{
			m_TempLongPoint.v = MIN (m_TempLongPoint.v, clientHeight);
			imgNewOrig = wxPoint (imgNewOrig.x, mainOrig.y + mainFrameBarHeight);
			
			}	// end "if (m_TempLongPoint.v > clientHeight)"
		
				// Set the image frame width no larger than the client width
		
		if (m_TempLongPoint.h > clientWidth)
			{
			m_TempLongPoint.h = MIN (m_TempLongPoint.h, clientWidth);
			imgNewOrig = wxPoint (mainOrig.x, imgNewOrig.y);
			
			}	// end "if (m_TempLongPoint.h > clientWidth)"
		
				// Realize the modification, adjust scrollbar later
		
		#if defined multispec_wxlin
			Restore ();
		#endif
		
		SetSize (m_TempLongPoint.h, m_TempLongPoint.v);
		SetPosition (imgNewOrig);
		
				// Increase width to hide unnecessary horizontal scroll bars
		
		int scrollRangeH = m_imageViewCPtr->m_Canvas->GetScrollRange (wxHORIZONTAL);
		int scrollRangeV = m_imageViewCPtr->m_Canvas->GetScrollRange (wxVERTICAL);
		
		if (m_TempLongPoint.h < clientWidth && scrollRangeH > 1)
			{
			#if defined multispec_wxlin
				int extraSpace = 15;
			#endif
			#if defined multispec_wxmac
				int extraSpace = 11;
			#endif
			m_TempLongPoint.h +=
							(m_TempLongPoint.h - scrollRangeH) - legendWidth + extraSpace;
			
					// Minimum width is 150
			
			m_TempLongPoint.h = MAX (150, m_TempLongPoint.h);
			
			}	// end "if (m_TempLongPoint.h < clientWidth && scrollRangeH > 1)"
		
		if (m_TempLongPoint.v < clientHeight && scrollRangeV > 1)
			{
			m_TempLongPoint.v += 20;
			
			}	// end "if (m_TempLongPoint.v < clientHeight && scrollRangeV > 1)"
		
		SetSize (m_TempLongPoint.h, m_TempLongPoint.v);
		
		 		// Check if the right border has exceeded the window limit
		
		if (imgNewOrig.x + m_TempLongPoint.h > mainOrig.x + clientWidth)
		 	{
			imgNewOrig.x =  MAX (2, mainOrig.x+clientWidth-m_TempLongPoint.h);
			SetPosition (imgNewOrig);

			}	// end "if (ImgNewOrig.x + m_TempLongPoint.h > MainOrig.x + clientWidth)"
		
		 		// Check if the lower border has exceeded the window limit
		
		if (imgNewOrig.y + m_TempLongPoint.v > mainOrig.y + clientHeight)
			{
			imgNewOrig.y =  MAX (mainOrig.y+mainFrameBarHeight, mainOrig.y+clientHeight-m_TempLongPoint.v);
			SetPosition (imgNewOrig);
			
		 	}	// end "if (ImgNewOrig.y + m_TempLongPoint.v > MainOrig.y + clientHeight)"
		
		}	// end "else currently unmaximized"
	
	m_imageViewCPtr->UpdateOffscreenMapOrigin ();
	
}	// end "OnMaximizeWindow"



void CMImageFrame::OnRefresh (
				wxCommandEvent& 					WXUNUSED (event))

{
	GetView()->OnUpdate (NULL, NULL);
	
}	// end "OnRefresh"



void CMImageFrame::OnSashDrag (
				wxSashEvent& 						event)

{
	if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        																							return;

	switch (event.GetId ())
    	{
		case ID_WINDOW_TOP:
			{
			m_topWindow->SetDefaultSize (wxSize (1000, event.GetDragRect().height));
			break;
			}
			
		case ID_WINDOW_LEFT:
			{
			m_leftWindow->SetDefaultSize (wxSize (event.GetDragRect().width, 1000));
			break;
			}

		}	// end "switch (event.GetId ())"

	wxLayoutAlgorithm layout;
	layout.LayoutFrame (this, m_mainWindow);

}	// end "OnSashDrag"



void CMImageFrame::OnSize (
				wxSizeEvent& 						event)

{
	wxLayoutAlgorithm 				layout;
	wxPoint 								scrollPos;


	layout.LayoutFrame (this, m_mainWindow);
	
	if (m_imageViewCPtr->m_Canvas != NULL)
		{
		wxPoint scrollPos = m_imageViewCPtr->m_Canvas->GetScrollPosition ();
		wxRect displayRect = m_imageViewCPtr->m_Canvas->GetImageDisplayRect (scrollPos);
		m_imageViewCPtr->m_Canvas->Refresh (false, &displayRect);
		
		}	// end "if (m_imageViewCPtr->m_Canvas != NULL)"
	
	else	// m_imageViewCPtr->m_Canvas == NULL
		{
		scrollPos.x = 0;
		scrollPos.y = 0;
		
		}	// end "else m_imageViewCPtr->m_Canvas == NULL"

	if (!m_frameMaximized)
		{
		m_frameSizeSaved = GetSize ();
		m_scrollPos = scrollPos;
		
		if (m_imageViewCPtr->m_Canvas != NULL)
			m_imageViewCPtr->ScrollChanged ();
		
		}	// end "if (!m_frameMaximized)"

}	// end "OnSize"



void CMImageFrame::OnUpdateFilePrint (
				CCmdUI& 								pCmdUI)

{
	Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																						kLock);

    UpdateFileImagePrint (&pCmdUI, windowInfoPtr);

    CheckAndUnlockHandle (windowInfoHandle);

    		// Disable until implemented.
	
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrint"



void CMImageFrame::OnUpdateFilePrintPreview (
				CCmdUI& 								pCmdUI)

{
    		// Disable until implemented
	
    //pCmdUI->Enable (gActiveImageViewCPtr->CheckIfOffscreenImageExists ());
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintPreview"



void CMImageFrame::OnUpdateFilePrintSetup (
				CCmdUI& 								pCmdUI)

{
    		// Set to false until set up.
	
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintSetup"


 
void CMImageFrame::OnUpdateFileSaveAs (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean 								enableFlag = FALSE;


	Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																						kLock);

	enableFlag = UpdateFileImageSaveAs (&pCmdUI, windowInfoPtr);

	CheckAndUnlockHandle (windowInfoHandle);

	pCmdUI.Enable (enableFlag);
	
}	// end "OnUpdateFileSaveAs"



void CMImageFrame::OnUpdateOverlay (
				wxUpdateUIEvent& 					event)

{
	WindowInfoPtr 						windowInfoPtr;
	Handle 								windowInfoHandle;
	Boolean 								enableFlag = FALSE;


	windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
	windowInfoPtr = (WindowInfoPtr) GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL && (windowInfoPtr->numberVectorOverlays > 0 ||
            										windowInfoPtr->numberImageOverlays > 0))
		enableFlag = TRUE;

	event.Enable (enableFlag);

}	// end "OnUpdateOverlay"



void CMImageFrame::OnUpdateViewCoordinatesBar (
				wxUpdateUIEvent& 					event) 

{
    Boolean m_displayCoordinatesFlag =
    									((CMImageDoc*)GetDocument())->GetCoordinateFlag ();
    event.Enable (true);
    event.Check (m_displayCoordinatesFlag);

}	// end "OnUpdateViewCoordinatesBar"



void CMImageFrame::OnUpdateWindowNewSelectionGraph (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (UpdateWindowSelectionGraph ());

}	// end "OnUpdateWindowNewSelectionGraph"



void CMImageFrame::OnViewCoordinatesBar (
				wxCommandEvent& 					event)

{
			// Just for the coordinate view to be toggled.
	
	ShowCoordinateView (2);

}	// end "OnViewCoordinatesBar"



void CMImageFrame::OnWindowNewSelectionGraph (
				wxCommandEvent& 					event)

{
	gProcessorCode = kSelectionGraphProcessor;
	
   ((CMultiSpecApp*)wxTheApp)->ActivateGraphView ();
	
	gProcessorCode = 0;

}	// end "OnWindowNewSelectionGraph"



void CMImageFrame::OnWindowShowCoordinateView (
				wxCommandEvent& 					event)

{
			// Toggle the coordinates bar

	ShowCoordinateView (2);

}	// end "OnWindowShowCoordinateView"



void CMImageFrame::SetActiveWindowFlag (
				Boolean 								setting)

{
	m_imageFrameActiveFlag = setting;

}	// end "SetActiveWindowFlag"



void CMImageFrame::SetImageViewCPtr (
				CMImageView* 						imageViewCPtr)

{
	m_imageViewCPtr = imageViewCPtr;

}	// end "SetImageViewCPtr"



void CMImageFrame::SetLegendWidth (
				SInt16 								newLegendWidth)

{
	wxSize								legendsize;
	Handle								windowInfoHandle;
	int									currentWidth;
	SInt16 								windowType;


			// If the input legend width is less than 0, then get the
			// current legend width to use as the new legend width. If
			// the new legend width is less than the threshold then force
			// the width to be 0 and set show legend width to FALSE.

	if (newLegendWidth < 0 || m_imageLegendViewCPtr == NULL)
																									return;

	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	windowType = GetWindowType (windowInfoHandle);

			// Get the current settings.
	
	legendsize = m_leftWindow->GetSize ();
	currentWidth = legendsize.GetWidth ();

	if (windowType == kThematicWindowType) 
		{
		if (newLegendWidth < 7)
			{
					// Force the legend width to be 0;
			
			m_leftWindow->Show (false);
			newLegendWidth = 0;

			}	// end "if (newLegendWidth < 7)"

		else if (newLegendWidth < 145)
			{
					// Force the legend width to be 145;

			newLegendWidth = 145;

					// Force the current legend width to be different.
			m_leftWindow->Show (true);
			currentWidth = 144;

			}	// end "else if (newLegendWidth < 145)"

		else
			newLegendWidth = kDefaultMaxLegendWidth;

		}	// end "windowType == kThematicWindowType"

	else	// windowType != kThematicWindowType
		{
				// Legend is not allowed for multispectral image windows.
		
		m_leftWindow->Show (false);
		newLegendWidth = 0;

		}	// end "else windowType != kThematicWindowType"

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL)
		{
		windowInfoPtr->showLegend = (newLegendWidth > 0);
		windowInfoPtr->legendWidth = newLegendWidth;

		}	// end "if (windowInfoPtr != NULL)"

	if (newLegendWidth != currentWidth)
		{
		legendsize.SetWidth (newLegendWidth);
		m_leftWindow->SetDefaultSize (legendsize);
		wxLayoutAlgorithm layout;
		layout.LayoutFrame (this, m_mainWindow);

		}	// end "if (newLegendWidth != currentWidth)"

}	// end "SetLegendWidth"


// Just enable or disable Legendview window
void CMImageFrame::SetSplitterParameters (
				SInt16 								windowType)

{
			// Set the splitter parameters such that no splitter is
			// available for image type windows and a splitter is
			// available for thematic type windows.

	if (windowType == kImageWindowType)
		{
		if (m_leftWindow->IsShown ())
			m_leftWindow->Show (false);

		}	// end "if (windowType == kImageType)"

	else // windowType == kThematicWindowType
		{
		if (!m_leftWindow->IsShown ())
			m_leftWindow->Show (true);

		}	// end "else windowType == kThematicWindowType"

	wxLayoutAlgorithm layout;
	layout.LayoutFrame (this, m_mainWindow);

}	// end "SetSplitterParameters"


void CMImageFrame::ShowCoordinateView (
				SInt16								inputCode)

{
	wxChoice*							displayUnitsCtrl;
	
	Handle								windowInfoHandle;
	
	int									coordinateBarHeight = 0;
	
	Boolean								cflag = false,
											setCoordinateHeightFlag = false;
	

	windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);

			// Toggle the coordinates bar
	
	if (inputCode == 1)
		{
				// From an new image being opened. Request is to force displayed
				
		cflag = true;
		((CMImageDoc*)GetDocument())->SetDisplayCoordinatesFlag (true);
		setCoordinateHeightFlag = true;
		
		}	// end "if (inputCode == 1)"
	
	else if (inputCode == 2)
		{
				// Toggle the coordinate view bar on or off
		
		cflag = ((CMImageDoc*)GetDocument())->OnViewCoordinatesBar ();
		setCoordinateHeightFlag = true;
		
		}	// end "else if (inputCode == 2)"
	
	else	// inputCode == 3, update when the coordinates bar is already showed
		{
		if (GetCoordinateHeight (windowInfoHandle) > 0)
					// Coordinates bar is shown
			cflag = true;
			
		}	// end "else inputCode == 3, update when the coordinates bar
			// is already showed"
	  
	if (cflag)
		UpdateSelectionCoordinates ();

	if (cflag && windowInfoHandle != NULL)
		{
		m_coordinatesBar->Show (true);
		m_topWindow->Show (true);

		SetUpCoordinateUnitsPopUpMenu (NULL,
													windowInfoHandle,
													m_coordinatesBar);

		m_coordinatesBar->m_displayUnitsCode =
													GetCoordinateViewUnits (windowInfoHandle);

		m_coordinatesBar->m_displayUnitsListSelection =
							 GetComboListSelection (m_coordinatesBar,
															 IDC_DisplayUnitsCombo,
															 m_coordinatesBar->m_displayUnitsCode);

		displayUnitsCtrl =
							(wxChoice*)m_coordinatesBar->FindWindow (IDC_DisplayUnitsCombo);
		if (displayUnitsCtrl != NULL)
			displayUnitsCtrl->SetSelection (
											(int)m_coordinatesBar->m_displayUnitsListSelection);

		SetUpAreaUnitsPopUpMenu (NULL,
										 windowInfoHandle,
										 m_coordinatesBar);

		m_coordinatesBar->m_areaUnitsCode =
				 GetCoordinateViewAreaUnits (windowInfoHandle);

		m_coordinatesBar->m_areaUnitsListSelection =
								 GetComboListSelection (m_coordinatesBar,
																IDC_AreaUnitsCombo,
																m_coordinatesBar->m_areaUnitsCode);

		#if defined multispec_wxlin
			wxComboBox*		areaUnitsCtrl;
			areaUnitsCtrl = (wxComboBox*)m_coordinatesBar->FindWindow (IDC_AreaUnitsCombo);
		#endif
		#if defined multispec_wxmac
			wxChoice*		areaUnitsCtrl;
			areaUnitsCtrl = (wxChoice*)m_coordinatesBar->FindWindow (IDC_AreaUnitsCombo);
		#endif

		if (areaUnitsCtrl != NULL)
			areaUnitsCtrl->SetSelection ((int)m_coordinatesBar->m_areaUnitsListSelection);
		
		if (setCoordinateHeightFlag)
			{
					// Set the coordinate bar height variable
		
			wxRect coordinateRect = m_coordinatesBar->GetRect ();
			coordinateBarHeight = coordinateRect.GetHeight ();
			
			}	// end "if (setCoordinateHeightFlag)"
			
		}	// end "if (cflag && ...)"

	else	// !displayCoordinatesFlag
		{
		m_coordinatesBar->Show (false);
		m_topWindow->Show (false);
		
		}	// end "else !displayCoordinatesFlag"
	
	if (setCoordinateHeightFlag)
		{
		SetCoordinateHeight (windowInfoHandle, coordinateBarHeight);
		m_frameMaximized = false;
	
		}	// end "if (setCoordinateHeightFlag)"
	
	if (cflag)
		{
		::UpdateScaleInformation (windowInfoHandle);

		}	// end "if (cflag)"
	
	wxLayoutAlgorithm layout;
	layout.LayoutFrame (this, m_mainWindow);

}	// end "ShowCoordinateView"



void CMImageFrame::ShowTitleView (
				SInt16								inputCode)

{
	if (inputCode == 0)
		{
		m_titleBar->Show (false);
		m_titleWindow->Show (false);
		
		}	// end "if (inputCode == 0)"
	
	else	// inputCode != 0
		{
		m_titleBar->Show (true);
		m_titleWindow->Show (true);
		
		}	// end "else inputCode != 0"

	wxLayoutAlgorithm layout;
	layout.LayoutFrame (this, m_mainWindow);

}	// end "ShowTitleView"



void CMImageFrame::UpdateActiveImageWindowInfo (void)

{
			// The active image window class pointer will only change when
			// there is no active processing funtion running.

	if (gProcessorCode == 0)
		{
		gActiveWindowType = kImageWindowType;
		gTheActiveWindow = m_imageViewCPtr;
		
      if (gSelectionGraphViewCPtr != NULL)
			gSelectionGraphViewCPtr->m_frame->UpdateExpandFlag ();
			
		if (gActiveImageViewCPtr != m_imageViewCPtr)
			{
			gActiveImageViewCPtr = m_imageViewCPtr;
			gActiveImageWindow = m_imageViewCPtr;
			
			gActiveImageWindowInfoH = GetActiveImageWindowInfoHandle ();
			gActiveImageFileInfoH = GetActiveImageFileInfoHandle ();
			gActiveImageLayerInfoH = GetActiveImageLayerInfoHandle ();
			
			gActiveLegendWidth = 0;
			if (GetShowLegendFlag (gActiveImageWindowInfoH))
				gActiveLegendWidth = GetLegendWidth (gActiveImageWindowInfoH);
				
			}	// end "if (gActiveImageViewCPtr != m_imageViewCPtr)"

		}	// end "if (gProcessorCode == 0)"

}	// end "UpdateActiveImageWindowInfo"



void CMImageFrame::UpdateCursorCoordinates (
        		char* 								lineValueStringPtr,
				char* 								columnValueStringPtr)

{
	((wxStaticText*)m_coordinatesBar->FindWindow (IDC_CursorColumn))->Show (true);
	((wxStaticText*)m_coordinatesBar->FindWindow (IDC_CursorLine))->Show (true);
	
	((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_CursorLine)))->
											SetLabel (wxString::FromUTF8 (lineValueStringPtr));
	((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_CursorColumn)))->
											SetLabel (wxString::FromUTF8 (columnValueStringPtr));
	
	m_coordinatesBar->Layout ();

}	// end "UpdateCursorCoordinates"



void CMImageFrame::UpdateCursorCoordinates (void)

{
	((wxStaticText*)m_coordinatesBar->FindWindow (IDC_CursorColumn))->Show (false);
	((wxStaticText*)m_coordinatesBar->FindWindow (IDC_CursorLine))->Show (false);
	
}	// end "UpdateCursorCoordinates"



void CMImageFrame::UpdateScaleInformation (
				double 								scale,
				char* 								scaleStringPtr)

{		
	if (scale <= 0)
		((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_ScalePrompt)))->
																				SetLabel (wxT(" "));
	
	else	// scale > 0
		((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_ScalePrompt)))->
																				SetLabel (wxT("Scale"));
	
	((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_Scale)))->
													SetLabel (wxString::FromUTF8(scaleStringPtr));

}	// end "UpdateScaleInformation"



void CMImageFrame::UpdateSelectedAreaInformation (
				char* 								areaDescriptionStringPtr,
				char* 								areaValueStringPtr)
		  
{
	((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_NumberPixels)))->
											SetLabel (wxString::FromUTF8(areaValueStringPtr));
		
}	// end "UpdateSelectedAreaInformation"



void CMImageFrame::UpdateSelectionCoordinates (void)

{
	DoubleRect							coordinateRectangle;
	LongRect								lineColumnRectangle;
	
	SInt64								numberPixels;
	
	Handle								windowInfoHandle;
	

			// Show the coordinates

	if (GetSelectionCoordinates (m_imageViewCPtr,
											&lineColumnRectangle,
											&coordinateRectangle,
											&numberPixels))
		{
		windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
		DrawSelectionCoordinates (windowInfoHandle,
											 &lineColumnRectangle,
											 &coordinateRectangle,
											 numberPixels);

		}	// end "if (GetSelectionCoordinates (m_imageViewCPtr,..."

	else		// !GetSelectionCoordinates (&selectionRectangle)
		{
		((wxStaticText*)m_coordinatesBar->FindWindow (IDC_SelectionLine))->Show (false);
		((wxStaticText*)m_coordinatesBar->FindWindow (IDC_SelectionColumn))->Show (false);
		
		((wxStaticText*)m_coordinatesBar->FindWindow (IDC_NumberPixels))->Show (false);
		((wxWindow*)m_coordinatesBar->FindWindow (IDC_AreaUnitsCombo))->Show (false);

		}	// end "else !GetSelectionCoordinates (&selectionRectangle)"
	
}	// end "UpdateSelectionCoordinates"



void CMImageFrame::UpdateSelectionCoordinates (
				char*									lineValueStringPtr,
				char*									columnValueStringPtr)

{
	((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_SelectionLine)))->
											SetLabel (wxString::FromUTF8(lineValueStringPtr));
	((wxStaticText*)(m_coordinatesBar->FindWindow (IDC_SelectionColumn)))->
											SetLabel (wxString::FromUTF8(columnValueStringPtr));
	
}	// end "UpdateSelectionCoordinates"



void CMImageFrame::UpdateSelectionWindowList ()

{
	wxCommandEvent						event;
	
	
	
   Handle fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
   FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
   gSelectionGraphViewCPtr->ResetListControls ();

   if (m_imageViewCPtr->m_Canvas->m_dataListShowFlag) 
		{
      gSelectionGraphViewCPtr->m_frame->m_checkBoxData->SetValue (true);
      gSelectionGraphViewCPtr->m_frame->OnData (event);
		
		}	// end "if (m_imageViewCPtr->m_Canvas->m_dataListShowFlag == true)"
	
	else	// !m_imageViewCPtr->m_Canvas->m_dataListShowFlag
      gSelectionGraphViewCPtr->m_frame->m_checkBoxData->SetValue (false);

   if (m_imageViewCPtr->m_Canvas->m_featureListShowFlag) 
		{
      gSelectionGraphViewCPtr->m_frame->m_checkBoxFeature->SetValue (true);
      gSelectionGraphViewCPtr->m_frame->OnFeature (event);
		
		}	// end "if (m_imageViewCPtr->m_Canvas->m_featureListShowFlag == true)"
		
	else	// !m_imageViewCPtr->m_Canvas->m_featureListShowFlag
      gSelectionGraphViewCPtr->m_frame->m_checkBoxFeature->SetValue (false);
   
   if (fileInfoPtr == NULL || fileInfoPtr->channelValuesHandle == NULL) 
		gSelectionGraphViewCPtr->m_frame->m_comboXlabel->Hide ();
			
	else	// fileInfoPtr != NULL && fileInfoPtr->channelValuesHandle != NULL
		{
		gSelectionGraphViewCPtr->m_frame->m_comboXlabel->Show ();   
		gSelectionGraphViewCPtr->m_frame->m_panel2->Layout ();
		
		}	// end "else fileInfoPtr != NULL && ..."
   
   gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();
   gSelectionGraphViewCPtr->m_frame->UpdateSplitterWindowLayout ();
	
}	// end "UpdateSelectionWindowList"
