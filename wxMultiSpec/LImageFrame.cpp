//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LImageFrame.cpp : class implementation file
//	Class Definition:		LImageFrame.h
//
//	Authors:					Larry L. Biehl, Wei-Kang Hsu
//
//	Revision date:			11/20/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMImageFrame class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" LImageFrame:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "LMultiSpec.h"
#include "LImageFrame.h"
#include "LImageDoc.h"

//#ifdef multispec_wxmac
	//#include "res/overlay_24.png"
	//#include "res/zoomx1_24.png"
	//#include "res/zoom_out_24.png"
	//#include "res/zoom_in_24.png"
//#else
	#include "LToolbar_img.cpp"
//#endif

#define kDefaultMaxLegendWidth 170

BEGIN_EVENT_TABLE (CMImageFrame, wxDocChildFrame)
	EVT_MAXIMIZE (CMImageFrame::OnMaximizeWindow)

	EVT_MENU (ID_DOWN_ARROW, CMImageFrame::DoKeyDownDownArrow)
	EVT_MENU (ID_LEFT_ARROW, CMImageFrame::DoKeyDownLeftArrow)
	EVT_MENU (ID_RIGHT_ARROW, CMImageFrame::DoKeyDownRightArrow)
	EVT_MENU (ID_UP_ARROW, CMImageFrame::DoKeyDownUpArrow)
	EVT_MENU (ID_FILE_SAVE_AS, CMImageFrame::OnFileSaveAs)
	EVT_MENU (ID_VIEW_COORDINATES_BAR, CMImageFrame::OnViewCoordinatesBar)
	EVT_MENU (ID_WINDOW_NEW_SELECTION_GRAPH, CMImageFrame::OnWindowNewSelectionGraph)
	EVT_MENU (ID_WINDOW_SHOW_COORDINATE_VIEW, CMImageFrame::OnWindowShowCoordinateView)

	EVT_SASH_DRAGGED_RANGE (ID_WINDOW_TOP, ID_WINDOW_LEFT, CMImageFrame::OnSashDrag)

	EVT_SIZE (CMImageFrame::OnSize)

	EVT_UPDATE_UI (ID_FILE_SAVE_AS, CMImageFrame::OnUpdateFileSaveAs)
	EVT_UPDATE_UI (ID_VIEW_COORDINATES_BAR, CMImageFrame::OnUpdateViewCoordinatesBar)
	EVT_UPDATE_UI (ID_WINDOW_NEW_SELECTION_GRAPH, CMImageFrame::OnUpdateWindowNewSelectionGraph)
	EVT_UPDATE_UI (ID_WINDOW_SHOW_COORDINATE_VIEW, CMImageFrame::OnUpdateWindowShowCoordinateView)
	EVT_UPDATE_UI (wxID_PRINT, CMImageFrame::OnUpdateFilePrint)
	EVT_UPDATE_UI (wxID_PREVIEW, CMImageFrame::OnUpdateFilePrintPreview)
	EVT_UPDATE_UI (wxID_PAGE_SETUP, CMImageFrame::OnUpdateFilePrintSetup)
	EVT_UPDATE_UI (ID_OVERLAY, CMImageFrame::OnUpdateOverlay)
END_EVENT_TABLE ()

Boolean CMImageFrame::s_forcePaletteBackgroundFlag = FALSE;


IMPLEMENT_DYNAMIC_CLASS(CMImageFrame, wxDocChildFrame)



CMImageFrame::CMImageFrame ()

{
	m_imageViewCPtr = NULL;
	m_imageFrameActiveFlag = FALSE;
	m_forcePaletteBackgroundUpdateFlag = FALSE;
	m_coordinatesBar = NULL;
	
}	// end "CMImageFrame"



CMImageFrame::CMImageFrame (
				wxDocument*							doc,
				wxView*								view,
				wxDocParentFrame*					parent)
		: wxDocChildFrame (doc, view, parent, wxID_ANY, wxT("Image Frame"),
					#if defined multispec_wxmac
						wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
					#else
						wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP)
					#endif
{
			// Initialize variables
	
	//#if defined multispec_wxmac
		/*
		wxBitmap overlayi = wxBITMAP_PNG(Resources/overlay_24);
		wxBitmap zoom1 = wxBITMAP_PNG(Resources/zoomx1_24);
		wxBitmap zoomout = wxBITMAP_PNG(Resources/zoom_out_24);
		wxBitmap zoomin = wxBITMAP_PNG(Resources/zoom_in_24);
		*/
	//#else
		wxBitmap overlayi = wxBITMAP_PNG_FROM_DATA(overlay);
		wxBitmap zoom1 = wxBITMAP_PNG_FROM_DATA(zoomx1);
		wxBitmap zoomout = wxBITMAP_PNG_FROM_DATA(zoom_out);
		wxBitmap zoomin = wxBITMAP_PNG_FROM_DATA(zoom_in);
	//#endif
	
	m_imageViewCPtr = (CMImageView*)view;
	m_imageFrameActiveFlag = FALSE;
	m_forcePaletteBackgroundUpdateFlag = FALSE;
	m_coordinatesBar = NULL;

	SetMinSize (wxSize (150,150));

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
		wxToolBar* l_toolBar = this->CreateToolBar (wxTB_HORIZONTAL, wxID_ANY);
		l_toolBar->SetToolBitmapSize (wxSize (16, 16));
		l_toolBar->AddTool (ID_MAGNIFICATION,
									wxT("tool"),
									zoom1,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Zoom to X1.0 magnification"),
									wxEmptyString);
	
		//wxToolBarToolBase* zoomInTool = l_toolBar->AddTool (ID_ZOOM_IN,
		l_toolBar->AddTool (ID_ZOOM_IN,
									wxT("tool"),
									zoomin,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Zoom in for active image window"),
									wxEmptyString);
	
		//wxToolBarToolBase* zoomOutTool = l_toolBar->AddTool (ID_ZOOM_OUT,
		l_toolBar->AddTool (ID_ZOOM_OUT,
									wxT("tool"),
									zoomout,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Zoom out for active image window"),
									wxEmptyString);
	
		m_zoomText = new wxStaticText (l_toolBar,
													ID_ZOOMINFO,
													wxT(""),
													wxDefaultPosition,
													wxSize (50, 16));
		l_toolBar->AddControl (m_zoomText, wxT("add a fixed text"));
		wxFont currentFont = l_toolBar->GetFont ();
		currentFont.SetPointSize (gFontSize+2);
		currentFont.SetWeight (wxFONTWEIGHT_LIGHT);
		m_zoomText->SetFont (currentFont);
	
		//l_toolBar->AddSeparator ();
		l_toolBar->AddTool (ID_OVERLAY,
									wxT("tool"),
									overlayi,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Control overlays on active image window"),
									wxEmptyString);
		l_toolBar->EnableTool (ID_OVERLAY, false);
		l_toolBar->Realize ();
	#endif

	Layout ();

			// Set default location for image window.
	
	int							xLocation,
									yLocation,
									offsetCount;
	 
	offsetCount = div (gNumberOfIWindows-1, 5).rem;
	xLocation = 5 + offsetCount * 20;
	#ifdef NetBeansProject
		yLocation = 110;
	#else	// mygeohub
		yLocation = 80; 
	#endif
	yLocation += offsetCount * 20;
	Move(xLocation, yLocation);
	
			//SetSize ... use this eventually when one knows the size of the image.

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
	 
    m_numberFDecimalDigits = fileInfoPtr->maxNumberFDecimalDigits;
    
    m_frameMaximized = false;
    m_frameSizeSaved = this->GetSize ();
	 
			// Set Accelerator Table
	
	wxAcceleratorEntry entries[36];
	entries[0].Set(wxACCEL_CTRL, (int) 'O', ID_IMAGE_OPEN);
	entries[1].Set(wxACCEL_CTRL, (int) ';', ID_FILE_OPEN_PROJECT);
	entries[2].Set(wxACCEL_CTRL, (int) 'W', ID_FILE_CLOSE_WINDOW);
	entries[3].Set(wxACCEL_CTRL, (int) 'S', ID_FILE_SAVE_AS);
	entries[4].Set(wxACCEL_CTRL, (int) 'P', ID_FILE_PRINT);
	entries[5].Set(wxACCEL_CTRL, (int) 'Q', wxID_EXIT);
	
	entries[6].Set(wxACCEL_CTRL, (int) 'Z', wxID_UNDO);
	entries[7].Set(wxACCEL_CTRL, (int) 'X', wxID_CUT);
	entries[8].Set(wxACCEL_CTRL, (int) 'C', wxID_COPY);
	entries[9].Set(wxACCEL_CTRL, (int) 'V', wxID_PASTE);
	entries[10].Set(wxACCEL_NORMAL, (int) WXK_DELETE, wxID_CLEAR);
	entries[11].Set(wxACCEL_CTRL, (int) 'A', ID_EDIT_SELECT_ALL);
	entries[12].Set(wxACCEL_CTRL, (int) ',', ID_EDIT_IMAGE_DESCRIPTION);
	entries[13].Set(wxACCEL_CTRL, (int) '`', ID_EDIT_IMAGE_MAP_PARAMETERS);
	
	entries[14].Set(wxACCEL_CTRL, (int) 'D', ID_PROC_DISPLAY_IMAGE);
	entries[15].Set(wxACCEL_CTRL, (int) 'H', ID_PROC_HISTOGRAM_IMAGE);
	entries[16].Set(wxACCEL_CTRL, (int) 'J', ID_PROC_LISTDATA);
	entries[17].Set(wxACCEL_CTRL, (int) 'R', ID_PROC_REFORMAT_CHANGE_IMAGE);
	entries[18].Set(wxACCEL_CTRL, (int) 'L', ID_PROC_CLUSTER);
	entries[19].Set(wxACCEL_CTRL, (int) 'T', ID_PROC_STATISTICS);
	entries[20].Set(wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);	
	entries[21].Set(wxACCEL_CTRL, (int) 'F', ID_PROC_FEATURE_EXTRACTION);	
	entries[22].Set(wxACCEL_CTRL, (int) 'B', ID_PROC_FEATURE_SELECTION);	
	entries[23].Set(wxACCEL_CTRL, (int) 'M', ID_PROC_CLASSIFY);	
	entries[24].Set(wxACCEL_CTRL, (int) 'Y', ID_PROC_LISTRESULTS);	
	entries[25].Set(wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);	
	
	entries[26].Set(wxACCEL_CTRL, (int) 'K', ID_PROC_UTIL_PRIN_COMP_ANALYSIS);
	entries[27].Set(wxACCEL_CTRL, (int) 'I', ID_PROC_UTIL_CREATE_STAT_IMAGE);
	entries[28].Set(wxACCEL_CTRL, (int) 'G', ID_PROC_UTIL_BIPLOTS_OF_DATA);
	entries[29].Set(wxACCEL_CTRL, (int) 'U', ID_PROC_UTIL_LIST_IMAGE_DESC);
	entries[30].Set(wxACCEL_CTRL, (int) '[', ID_PROC_UTIL_CHECK_COVARIANCES);
	entries[31].Set(wxACCEL_CTRL, (int) ']', ID_PROC_UTIL_CHECKTRANS_MATRIX);	
	
	entries[32].Set(wxACCEL_NORMAL, (int) WXK_LEFT, ID_LEFT_ARROW);
	entries[33].Set(wxACCEL_NORMAL, (int) WXK_RIGHT, ID_RIGHT_ARROW);
	entries[34].Set(wxACCEL_NORMAL, (int) WXK_UP, ID_UP_ARROW);
	entries[35].Set(wxACCEL_NORMAL, (int) WXK_DOWN, ID_DOWN_ARROW);	
	wxAcceleratorTable accel(36, entries);
	SetAcceleratorTable (accel);

}	// end "CMImageFrame"



CMImageFrame::~CMImageFrame () 

{

}	// end "~CMImageFrame"



void CMImageFrame::OnMaximizeWindow(wxMaximizeEvent& event)
{
   wxPoint ImgNewOrig = GetPosition();
   wxPoint MainOrig;
   int	clientWidth,
			clientHeight,
         legendHeight = 0,
         legendWidth = 0,
         coordinateBarHeight = 0;
   wxRect tempArea;
   WindowInfoPtr windowInfoPtr = NULL;
   
   //   gActiveImageViewCPtr->m_frame->Refresh(false, NULL);
   Maximize(false);
   
//   m_imageViewCPtr->m_frame->GetSize();
   
//   if (gActiveImageViewCPtr != NULL) {
		
//      // Update the standard state to be the size of						
//				// the window that allows the entire image to						
//      		// be viewed or if that is larger than the 							
//      		// screen then the entire													
//      		// screen for the device that the cursor is in. 
//				
////		GetWindowStandardState (window, &idealStandardState);
////		gTempRect = idealStandardState;
////		GetWindowUserState (window, &userStateRect);
//				
//				// Determine whether the top left corner of the user				
//				// rectangle is within the current device.  If so use				
//				// this as the top left for the standard state.  If not			
//				// use the upper left of the 'new' current device.					
//						
////		if (PtInRect (topLeft(userStateRect), &gViewRect))
////			topLeft(idealStandardState) = topLeft(userStateRect);
////			
////		else		// !PtInRect (&topLeft(... 
////			topLeft(idealStandardState) = topLeft(gViewRect);
//      
//      windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
//																			kNoLock,
//																			kNoMoveHi);
//	if (windowInfoPtr != NULL)
//			displaySpecsPtr = GetActiveDisplaySpecsPtr();
//
//      
////      CMImageFrame* activechild = (CMImageFrame*) (gActiveImageViewCPtr->GetFrame());
//   if (displaySpecsPtr != NULL)
//    {
//         GetWindowClipRectangle(gActiveImageWindow, kImageFrameArea, &gViewRect);
//					// Adjust down to the size needed to enclose the image.						
//			idealStandardState.right = MIN(gViewRect.right,
//									origin.y + (SInt16)gTempLongPoint.h);
//											
//			idealStandardState.bottom = MIN(gViewRect.bottom,
//									origin.x + (SInt16)gTempLongPoint.v);
//											
//					// Now force the upper left if possible to enclose the		
//					// image if needed.														
//				
//			idealStandardState.left = MAX(gViewRect.left,
//									origin.y - (SInt16)gTempLongPoint.h);
//											
//			idealStandardState.top = MAX(gViewRect.top,
//									origin.x - (SInt16)gTempLongPoint.v);
//			
//	}	// end "if (displaySpecsPtr)" 
////      int width_max = idealStandardState.right-idealStandardState.left;
////      int height_max = idealStandardState.bottom-idealStandardState.top;
//      int width_max = gViewRect.right-gViewRect.left;
//      int height_max = gViewRect.bottom-gViewRect.top;

//   }      
   
//   gActiveImageWindow->m_frame->SetSize(1200, 800);
	
	if (m_frameMaximized == true)
		{
				// currently maximized, set to the saved values
		
		SetSize (m_frameSizeSaved.x, m_frameSizeSaved.y);
		m_frameMaximized = false;
		Restore ();
		SetPosition (ImgNewOrig);
		
				// Adjusting the scrollbar is not effective now, not sure why @ 07.11.16
			
		m_imageViewCPtr->m_Canvas->SetScrollPos (wxVERTICAL, m_scrollPos.y, true);
		m_imageViewCPtr->m_Canvas->SetScrollPos (wxHORIZONTAL, m_scrollPos.x, true);
		
      }
	
	else
		{
				// Currently unmaximized
		
		m_frameSizeSaved = GetSize();
		m_scrollPos = m_imageViewCPtr->m_Canvas->GetScrollPosition ();
		
		Handle windowInfoHandle = GetWindowInfoHandle(gActiveImageViewCPtr);
		if (windowInfoHandle != NULL)
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer(windowInfoHandle,kNoLock,kNoMoveHi);
		GetMinimumDisplaySizeForImage (windowInfoHandle, &m_TempLongPoint);  
		m_frameMaximized = true;
		
		GetMainFrame()->GetClientSize(&clientWidth, &clientHeight);
		GetMainFrame()->GetPosition(&MainOrig.x, &MainOrig.y);
		UInt16 MainFrameBarHeight, OtherLegendStuffHeight;
			#ifdef NetBeansProject
					MainFrameBarHeight = 150;
					OtherLegendStuffHeight = 130;
			#else	// mygeohub
					MainFrameBarHeight = 80;
					OtherLegendStuffHeight = 90;
					clientHeight -= 30;
			#endif
		
				// Set the window height to max(legend height, image size)
		
		if (windowInfoPtr->legendWidth > 0)
			{
			if (GetCoordinateHeight(windowInfoHandle) > 0)
				{ // has coordinate bar
				tempArea = (m_imageViewCPtr->m_frame)->m_topWindow->GetClientRect();
				coordinateBarHeight = tempArea.GetBottom();                  
				}
			
			legendHeight = m_imageViewCPtr->m_frame->m_imageLegendViewCPtr->m_legendListBox->m_ilist->GetImageCount()*22;
			legendHeight += OtherLegendStuffHeight + coordinateBarHeight;
			m_imageViewCPtr->m_frame->m_imageLegendViewCPtr->GetClientSize(&legendWidth, &legendHeight);
			#ifndef multispec_wxmac
				legendWidth = m_imageViewCPtr->m_frame->m_imageLegendViewCPtr->m_clientWidth;
			#endif
			m_TempLongPoint.v = MAX(m_TempLongPoint.v, legendHeight);
			
			}
					
				// set the image frame height no larger than the client height
		if(m_TempLongPoint.v > clientHeight)
			{
			m_TempLongPoint.v = MIN(m_TempLongPoint.v, clientHeight);           
			ImgNewOrig = wxPoint(ImgNewOrig.x, MainOrig.y+ MainFrameBarHeight);
		}
		// set the image frame width no larger than the client width
		if(m_TempLongPoint.h > clientWidth){
			m_TempLongPoint.h = MIN(m_TempLongPoint.h, clientWidth);
			ImgNewOrig = wxPoint(MainOrig.x, ImgNewOrig.y);
		}
		
		// realize the modification, adjust scrollbar later
		Restore();
		SetSize(m_TempLongPoint.h, m_TempLongPoint.v); 
		SetPosition(ImgNewOrig);                
		
			// Increase width to hide unnecessary horizontal scroll bars
		
		int scrollRangeH = m_imageViewCPtr->m_Canvas->GetScrollRange(wxHORIZONTAL);
		int scrollRangeV = m_imageViewCPtr->m_Canvas->GetScrollRange(wxVERTICAL);
		 
		if (m_TempLongPoint.h < clientWidth)
			if(scrollRangeH > 1)
				{
				m_TempLongPoint.h += (m_TempLongPoint.h -scrollRangeH) - windowInfoPtr->legendWidth + 11;
				//m_TempLongPoint.h += legendWidth - 170;
				//m_TempLongPoint.h += (legendWidth -scrollRangeH)  + 11;
				}
		if (m_TempLongPoint.v < clientHeight)
			if(scrollRangeV > 1)
				{
				m_TempLongPoint.v += 20; //(m_TempLongPoint.v - scrollRangeV);
				
				}
		
		SetSize (m_TempLongPoint.h, m_TempLongPoint.v);
		 
		 		// Check if the right border has exceed the window limit
		
		if (ImgNewOrig.x + m_TempLongPoint.h > MainOrig.x + clientWidth)
		 	{
			ImgNewOrig.x =  MAX(0,MainOrig.x + clientWidth - m_TempLongPoint.h);
			SetPosition(ImgNewOrig);
			}
		
		 		// Check if the lower border has exceed the window limit
		
		if (ImgNewOrig.y + m_TempLongPoint.v > MainOrig.y + clientHeight)
			{
			ImgNewOrig.y =  MAX(MainOrig.y+MainFrameBarHeight, MainOrig.y + clientHeight - m_TempLongPoint.v);
			SetPosition(ImgNewOrig);
			
		 	}
		
		//printf("legend image list height = %d \n",
		//m_imageViewCPtr->m_frame->m_imageLegendViewCPtr->m_legendListBox->m_ilist->GetImageCount()*20);
		//printf("legend image list = %d \n",m_imageViewCPtr->m_frame->m_imageLegendViewCPtr->m_legendListBox->m_ilist->GetImageCount());
		
		}
}



void CMImageFrame::GetMinimumDisplaySizeForImage ( 
   Handle windowInfoHandle,
   LongPoint* minimumWindowSizePointPtr)
{
   LongPoint offScreenPoint;

   DisplaySpecsPtr displaySpecsPtr = NULL;
   WindowInfoPtr windowInfoPtr = NULL;

   SInt16 savedOrigin[2];

   if (windowInfoHandle != NULL)
      windowInfoPtr = (WindowInfoPtr)GetHandlePointer(windowInfoHandle,kNoLock,kNoMoveHi);

   if (windowInfoPtr != NULL)
      displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(GetDisplaySpecsHandle(windowInfoPtr),kNoLock,kNoMoveHi);
   

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
	
      //int titleHeight = gDefaultTitleHeight;
      int coordinateBarHeight = 0; 
      wxRect tempArea;
      UInt16 amountToAllowForHStuff, amountToAllowForVStuff;
	
				// Get the total of the title and toolbar height
	
		wxRect wxFrameRect = this->GetRect ();
		wxRect wxFrameClientRect = this->GetClientRect ();
		int titleToolBarHeight = wxFrameRect.height - wxFrameClientRect.height;
	
      #ifdef NetBeansProject
			amountToAllowForHStuff = 7;
			amountToAllowForVStuff = 33;
      #else	// mygeohub
			#if defined multispec_wxmac
						// Allow 2 pixels around edge for border
						// Do not allow for title height
		
				titleToolBarHeight -= 20;
						
				amountToAllowForHStuff = 2 * 2 + wxFrameRect.width - wxFrameClientRect.width;
				amountToAllowForVStuff = 2 * 2 + titleToolBarHeight;
			#else
				amountToAllowForHStuff = 6;
				amountToAllowForVStuff = 6;	// Allows for border
			#endif
		#endif	

		//coordinateBarHeight = GetCoordinateHeight(windowInfoHandle);
		/*
      if (GetCoordinateHeight(windowInfoHandle) > 0)
      	{ // has coordinate bar
         tempArea = (m_imageViewCPtr->m_frame)->m_topWindow->GetClientRect();
         coordinateBarHeight = tempArea.GetBottom();                  
      	}
		*/
		//minimumWindowSizePointPtr->v += kSBarWidth + coordinateBarHeight + titleHeight + 1 ;
      minimumWindowSizePointPtr->v += amountToAllowForVStuff + coordinateBarHeight;
      
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



void CMImageFrame::ActivateImageWindowItems (
        Boolean activateFlag,
        Boolean changeWindowFlag) 
{
	double		magnification;	
   
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

		}	// end "if ( (activateFlag != m_imageFrameActiveFlag) && ..."

	if (activateFlag && changeWindowFlag) 
		{
		m_imageFrameActiveFlag = TRUE;
		m_imageLegendViewCPtr->GetLegendListCPtr()->SetLegendListActiveFlag(TRUE);
	  
			// Make this the highest child frame in the list. Not used with wxSTAY_ON_TOP
			
		//if (gProcessorCode == 0)
		//	Raise();
		
			// Update the zoom value in the status bar to reflect this active image window
		magnification = -1;
		if (m_imageViewCPtr->CheckIfOffscreenImageExists()) 
			{
			CMDisplay* displayMultiCPtr = m_imageViewCPtr->m_displayMultiCPtr;
			magnification = displayMultiCPtr->GetMagnification();

			}	// end "if ( ...->CheckIfOffscreenImageExists() )"
      
		GetMainFrame()->UpdateStatusBar(magnification);
      
				// Handle project related items.
				
		Handle windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer (windowInfoHandle);

		if (windowInfoPtr->projectWindowFlag) 
			{
			gProjectSelectionWindow = m_imageViewCPtr;

			LoadNewFieldListBox();

			}	// end "if (windowInfoPtr->projectWindowFlag)"

		//ShowGraphSelection ();

      if (gSelectionGraphViewCPtr != NULL)
			UpdateSelectionWindowList ();
      
      ShowGraphSelection ();
      
				// This flag is used in selections to try to differentiate between a user
				// making a selection or just selecting the image to work on.

		gNewSelectedWindowFlag = TRUE;
		
		}	// end "if (bActivate && changeWindowFlag)"

	if (!activateFlag && changeWindowFlag) 
		{
		m_imageFrameActiveFlag = FALSE;
		m_imageLegendViewCPtr->GetLegendListCPtr ()->SetLegendListActiveFlag (FALSE);
		//GetMainFrame()->UpdateStatusBar(-1); // this is to hide the "zoom" info when the imageWin is not active

		}	// end "if (!activateFlag && changeWindowFlag)"

		// Make sure that the flag indicating that the control key is down
		// is set to FALSE. It may be TRUE if a menu command was processed
		// using quick keys.

	m_imageViewCPtr->SetControlKeyFlag (FALSE);

}	// end "ActivateImageWindowItems"



void CMImageFrame::ChangeClassGroupDisplay (
				SInt16								newClassGroupCode,
				Boolean								updateClassGroupListFlag)
				 
{
	SInt16 currentClassGroupCode = m_imageViewCPtr->GetClassGroupCode();

	if (currentClassGroupCode != 0 &&
			newClassGroupCode != currentClassGroupCode)
		{
		//UpdateActiveImagePalette(newClassGroupCode);
		UpdateActiveImageLegend(newClassGroupCode, kCallCreatePalette);

		if (updateClassGroupListFlag)
			{
			m_imageLegendViewCPtr->UpdateClassGroupComboList(newClassGroupCode);

			}	// end "if (updateClassGroupListFlag)"

		}	// end "if (classGroupCode != m_classGroupCode + 1)"
	
}	// end "ChangeClassGroupDisplay"
 
 

void CMImageFrame::DoKeyDownDownArrow (wxCommandEvent& event) 
{
	char  theChar = 0x28;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownDownArrow"
 

void CMImageFrame::DoKeyDownLeftArrow (wxCommandEvent& event) 

{
	char  theChar = 0x25;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownLeftArrow"
 

void CMImageFrame::DoKeyDownRightArrow (wxCommandEvent& event) {
	char  theChar = 0x27;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownRightArrow"
 

void CMImageFrame::DoKeyDownUpArrow (wxCommandEvent& event) {
	char  theChar = 0x26;
	
	DoNextDisplayChannelEvent (m_imageViewCPtr, theChar);

}	// end "DoKeyDownUpArrow"


Boolean CMImageFrame::GetActiveWindowFlag(void) {
    if (this != NULL)
        return (m_imageFrameActiveFlag);

    else // this == NULL
        return (FALSE);

}	// end "GetActiveWindowFlag"

void CMImageFrame::GetCoordinateViewComboText(char* comboItemStringPtr, UInt16 itemNumber) 
{
	if (this != NULL)
		{
		wxComboBox* comboBoxPtr = (wxComboBox*)m_coordinatesBar->FindWindow(itemNumber);
		//wxString selvalue = comboBoxPtr->GetString(comboBoxPtr->GetSelection());
      wxString selvalue = comboBoxPtr->GetString (0);
		comboItemStringPtr[0] = (UInt8) selvalue.Len();
		strncpy(&comboItemStringPtr[1], (const char*)selvalue.mb_str(wxConvUTF8), (int) selvalue.Len());
		(&comboItemStringPtr[1])[(int) selvalue.Len()] = '\0';
		//(UInt8) comboBoxPtr->GetLBText(comboBoxPtr->GetCurSel(), &comboItemStringPtr[1]);
		
		}	// end "if (this != NULL)"

}	// end "GetCoordinateViewComboText"



void CMImageFrame::InitialUpdate (CSize imageViewSize) 

{
	/*
			// For Linux after displaying image
			// Tweak this routine for optimal display
			
	CRect rectFrame;

	GetWindowRect(rectFrame);
	CSize size = rectFrame.Size();

			// This represents just the size allowed for the image. This was
			// set by the ResizeParentToFit call in CMImageView::OnInitialUpdate
			// routine. Add the amount needed for the legend and the split window bar.

	size.cx += kDefaultMaxLegendWidth;
	SetWindowPos(NULL, 0, 0, size.cx, size.cy,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	m_wndSplitter.SetColumnInfo(0, 170, 0);
	m_wndSplitter.SetColumnInfo(1, imageViewSize.cx, 0);
	m_wndSplitter.SetRowInfo(0, imageViewSize.cy, 0);

	m_wndSplitter.RecalcLayout();

	Handle windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer (
																			windowInfoHandle);

	if (windowInfoPtr != NULL) 
		{
		windowInfoPtr->showLegend = TRUE;
		windowInfoPtr->legendWidth = kDefaultMaxLegendWidth;

		}	// end "if (windowInfoPtr != NULL)"

	m_imageLegendViewCPtr->SetLegendWidthSetFlag();

	m_imageLegendViewCPtr->AdjustLegendListLength();
	*/
}	// end "InitialUpdate"



void CMImageFrame::OnActivate (
				wxActivateEvent&								event)
{
	m_imageFrameActiveFlag = true;
	if (event.GetActive () && m_mainWindow)
		m_mainWindow->SetFocus();
	
}	// end "OnActivate"



void CMImageFrame::OnFileSave(wxCommandEvent& event) 
{
	gProcessorCode = kSaveProcessor;
	WriteThematicInfo(NULL, 0);
	gProcessorCode = 0;

}

void CMImageFrame::OnFileSaveAs(wxCommandEvent& event) 
{
	gProcessorCode = kSaveOutputAsProcessor;
	SaveImageWindowAs();
	gProcessorCode = 0;

}	// end "OnFileSaveAs"



void CMImageFrame::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
	GetView()->OnUpdate(NULL, NULL);
}



void CMImageFrame::OnSashDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId()) {
        case ID_WINDOW_TOP:
        {
            m_topWindow->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
            break;
        }
        case ID_WINDOW_LEFT:
        {
            m_leftWindow->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
            break;
        }

    }

    wxLayoutAlgorithm layout;
    layout.LayoutFrame(this, m_mainWindow);

}


void CMImageFrame::OnSize (
				wxSizeEvent& 							event)

{
	wxLayoutAlgorithm 					layout;
	wxPoint 									scrollPos;


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
		
		}	// end "else	// m_imageViewCPtr->m_Canvas == NULL"

	if (!m_frameMaximized)
		{
		m_frameSizeSaved = GetSize ();
		m_scrollPos = scrollPos;
		
		}	// end "if (!m_frameMaximized)"

}	// end "OnSize"


/*
void CMImageFrame::OnUpdateEditClearSelectionRectangle(wxUpdateUIEvent& event) {
    SInt16 selectionTypeCode;
    Boolean enableFlag;
    // Update the menu text.
    //selectionTypeCode = GetSelectionTypeCode(gActiveImageViewCPtr);
    selectionTypeCode = GetSelectionTypeCode(m_imageViewCPtr);
    UpdateEditClearSelection(&event, selectionTypeCode);

    // Determine whether menu item is enabled.

    enableFlag = FALSE;
    //if (gActiveImageViewCPtr != NULL)
    if (m_imageViewCPtr != NULL)
        enableFlag = (selectionTypeCode != 0);

    event.Enable(enableFlag);

}	// end "OnUpdateEditClearSelectionRectangle"


void CMImageFrame::OnUpdateEditCopy(wxUpdateUIEvent& pCmdUI) {

    //wxBell();
    Handle windowInfoHandle = GetActiveImageWindowInfoHandle();

    WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
            windowInfoHandle,
            kLock,
            kNoMoveHi);

    Boolean enableFlag = UpdateEditImageCopy(&pCmdUI, windowInfoPtr);

    CheckAndUnlockHandle(windowInfoHandle);

    // Will set to false until implemented
    pCmdUI.Enable(false);
}
*/

void CMImageFrame::OnUpdateFilePrint(CCmdUI& pCmdUI) {
    //Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
    Handle windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);

    WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
            windowInfoHandle,
            kLock,
            kNoMoveHi);

    Boolean enableFlag = UpdateFileImagePrint (&pCmdUI, windowInfoPtr);

    CheckAndUnlockHandle(windowInfoHandle);

    // disable until implemented.
    pCmdUI.Enable(false);

}

void CMImageFrame::OnUpdateFilePrintPreview(CCmdUI& pCmdUI) {
    // Disable until implemented
    //pCmdUI->Enable(gActiveImageViewCPtr->CheckIfOffscreenImageExists());
    pCmdUI.Enable(false);

}

void CMImageFrame::OnUpdateFilePrintSetup(CCmdUI& pCmdUI)

{
    // Set to false until set up.
    pCmdUI.Enable(false);

}


 
void CMImageFrame::OnUpdateFileSaveAs (wxUpdateUIEvent& pCmdUI)

{
	Boolean enableFlag = FALSE;


	Handle windowInfoHandle = GetActiveImageWindowInfoHandle();

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
			windowInfoHandle,
			kLock,
			kNoMoveHi);

	enableFlag = UpdateFileImageSaveAs(&pCmdUI, windowInfoPtr);

	CheckAndUnlockHandle(windowInfoHandle);

	pCmdUI.Enable(enableFlag);
	
}	// end "OnUpdateFileSaveAs"



void CMImageFrame::OnUpdateOverlay(wxUpdateUIEvent& event) 

{
	WindowInfoPtr windowInfoPtr;
	Handle windowInfoHandle;
	Boolean enableFlag = FALSE;


	//windowInfoHandle = GetWindowInfoHandle(gActiveImageViewCPtr);
	windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);
	windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
            windowInfoHandle, kNoLock, kNoMoveHi);

	if (windowInfoPtr != NULL && (windowInfoPtr->numberOverlays > 0 ||
            windowInfoPtr->numberImageOverlays > 0))
		enableFlag = TRUE;

	event.Enable(enableFlag);

}	// end "OnUpdateOverlay"



void CMImageFrame::OnUpdateViewCoordinatesBar(wxUpdateUIEvent& event) 

{
    Boolean m_displayCoordinatesFlag = ((CMImageDoc *) GetDocument())->GetCoordinateFlag();
    event.Enable(true);
    event.Check(m_displayCoordinatesFlag);

}	// end "OnUpdateViewCoordinatesBar"


void CMImageFrame::OnUpdateWindowShowCoordinateView(wxUpdateUIEvent& pCmdUI) {

    pCmdUI.Enable(UpdateWindowCoordinateView(&pCmdUI));

}	// end "OnUpdateWindowShowCoordinateView"


void CMImageFrame::OnUpdateWindowNewSelectionGraph(wxUpdateUIEvent& pCmdUI) {

    // Disable for now until implemented.
    pCmdUI.Enable(false);
    //pCmdUI.Enable(UpdateWindowSelectionGraph());

}	// end "OnUpdateWindowNewSelectionGraph"



void CMImageFrame::OnViewCoordinatesBar(wxCommandEvent& event)

{
    // Just for the coordinate view to be toggled.
    ShowCoordinateView(2);

}	// end "OnViewCoordinatesBar"


void CMImageFrame::OnWindowNewSelectionGraph(wxCommandEvent& event)

{

}	// end "OnWindowNewSelectionGraph"


void CMImageFrame::OnWindowShowCoordinateView(wxCommandEvent& event) 
{
			// Toggle the coordinates bar

	ShowCoordinateView(2);

}	// end "OnWindowShowCoordinateView"


void CMImageFrame::OnZoom(wxCommandEvent& event)

{
	if (event.GetId() == ID_ZOOM_IN)
		m_imageViewCPtr->ZoomIn();
	else if (event.GetId() == ID_ZOOM_OUT)
		m_imageViewCPtr->ZoomOut();
	else
		m_imageViewCPtr->ZoomSize();
		
   m_frameMaximized = false;   
   m_frameSizeSaved = GetSize();
}	// end "OnZoom"



void CMImageFrame::SetActiveWindowFlag(Boolean setting) 
{
	if (this != NULL)
		m_imageFrameActiveFlag = setting;

}	// end "SetActiveWindowFlag"



void CMImageFrame::SetImageViewCPtr(
        CMImageView* imageViewCPtr)
{
	m_imageViewCPtr = imageViewCPtr;

}	// end "SetImageViewCPtr"



void CMImageFrame::SetLegendWidth(SInt16 newLegendWidth) 
{
	Handle		windowInfoHandle;
	wxSize		legendsize;
	int			currentWidth, 
					minimumWidth;

	SInt16 windowType;


			// If the input legend width is less than 0, then get the
			// current legend width to use as the new legend width. If
			// the new legend width is less than the threshold then force
			// the width to be 0 and set show legend width to FALSE.

	if (newLegendWidth < 0 || m_imageLegendViewCPtr == NULL)
		return;

	windowInfoHandle = GetActiveImageWindowInfoHandle();
	windowType = GetWindowType(windowInfoHandle);

			// Get the current settings.
	legendsize = m_leftWindow->GetSize();

	if (windowType == kThematicWindowType) 
		{
		if (newLegendWidth < 7) {
			// Force the legend width to be 0;
			m_leftWindow->Show(false);
			newLegendWidth = 0;

			}	// end "if (currentWidth < 7)"

		else if (newLegendWidth < 145) {
					// Force the legend width to be 145;

			newLegendWidth = 145;

					// Force the current legend width to be different.
			m_leftWindow->Show(true);
			currentWidth = 144;

			}	// end "else if (newLegendWidth < 145)"

		else
			newLegendWidth = kDefaultMaxLegendWidth;

		}	// end "windowType == kThematicWindowType"

	else { // windowType != kThematicWindowType
				// Legend is not allowed for multispectral image windows.
		m_leftWindow->Show(false);
		newLegendWidth = 0;

		}

	WindowInfoPtr windowInfoPtr = (WindowInfoPtr) GetHandlePointer(
			windowInfoHandle,
			kNoLock,
			kNoMoveHi);

	if (windowInfoPtr != NULL) {
		windowInfoPtr->showLegend = (newLegendWidth > 0);
		windowInfoPtr->legendWidth = newLegendWidth;

		}	// end "if (windowInfoPtr != NULL)"

	if (newLegendWidth != currentWidth)
		{
		legendsize.SetWidth(newLegendWidth);
		m_leftWindow->SetDefaultSize(legendsize);
		wxLayoutAlgorithm layout;
		layout.LayoutFrame(this, m_mainWindow);

		}	// end "if (newLegendWidth != currentWidth)"

}	// end "SetLegendWidth"


// Just enable or disable Legendview window

void CMImageFrame::SetSplitterParameters(SInt16 windowType)
{
			// Set the splitter parameters such that no splitter is
			// available for image type windows and a splitter is
			// available for thematic type windows.

	if (windowType == kImageWindowType)
		{
		if (m_leftWindow->IsShown())
			m_leftWindow->Show(false);

		}	// end "if (windowType == kImageType)"

	else // windowType == kThematicWindowType
		{
		if (!m_leftWindow->IsShown())
			m_leftWindow->Show(true);

		}	// end "else windowType == kThematicWindowType"

	wxLayoutAlgorithm layout;
	layout.LayoutFrame(this, m_mainWindow);

}	// end "SetSplitterParameters"


void CMImageFrame::ShowCoordinateView (
				SInt16								inputCode)
{

	RECT									coordinateRect,
											displayUnitsRect,
											linePromptRect;
	wxComboBox*							comboPtr;
	Handle								windowInfoHandle;
	Boolean								cflag = false;
	

	windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);

			// Toggle the coordinates bar
	if (inputCode == 1)
		{
				// From an new image being opened. Request is to force displayed
				
		cflag = true;
		((CMImageDoc*)GetDocument())->SetDisplayCoordinatesFlag (true);
		
		}
	else if (inputCode == 2) // inputCode == 2
		{
				// Toggle the coordinate view bar on or off
		cflag = ((CMImageDoc*)GetDocument())->OnViewCoordinatesBar();
		
		}
	else 
		{  // inputCode == 3, update when the coordinates bar is already showed
		if (GetCoordinateHeight(windowInfoHandle) > 0)
			{ //coordinates bar is shown
			cflag = true;
			}
		}
	  
	if (cflag)
		UpdateSelectionCoordinates ();

	//windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);

	if (cflag && windowInfoHandle != NULL)
		{
		m_coordinatesBar->Show(true);
		m_topWindow->Show(true);

		SetUpCoordinateUnitsPopUpMenu (NULL,
													windowInfoHandle, //gActiveImageWindowInfoH,
													m_coordinatesBar);

		m_coordinatesBar->m_displayUnitsCode =
				 GetCoordinateViewUnits(windowInfoHandle);
		//GetCoordinateViewUnits(gActiveImageWindowInfoH);

		m_coordinatesBar->m_displayUnitsListSelection =
				 GetComboListSelection (m_coordinatesBar,
												 IDC_DisplayUnitsCombo,
												 m_coordinatesBar->m_displayUnitsCode);

		comboPtr = (wxComboBox*)m_coordinatesBar->FindWindow (IDC_DisplayUnitsCombo);
		if (comboPtr != NULL)
			comboPtr->SetSelection ((int)m_coordinatesBar->m_displayUnitsListSelection);

		SetUpAreaUnitsPopUpMenu (NULL,
										 windowInfoHandle, //gActiveImageWindowInfoH,
										 m_coordinatesBar);

		m_coordinatesBar->m_areaUnitsCode =
				 GetCoordinateViewAreaUnits(windowInfoHandle); // gActiveImageWindowInfoH);

		m_coordinatesBar->m_areaUnitsListSelection =
				 GetComboListSelection (m_coordinatesBar,
												IDC_AreaUnitsCombo,
												m_coordinatesBar->m_areaUnitsCode);

		comboPtr = (wxComboBox*)m_coordinatesBar->FindWindow (IDC_AreaUnitsCombo);
		if (comboPtr != NULL)
			comboPtr->SetSelection ((int)m_coordinatesBar->m_areaUnitsListSelection);
			
		}	// end "if (cflag && ...)"

	else 
		{ // !displayCoordinatesFlag
		m_coordinatesBar->Show(false);
		m_topWindow->Show(false);
		}
		
	// Update the image frame
	//Update();
	// Update the main frame
	//GetParent()->Update();
	//    m_topWindow->SetSize(m_coordinatesBar->GetSize());
	//    m_topWindow->Layout();
	//    this->Layout();
	//    this->SetAutoLayout(true);
	

	if (cflag)
		{
		::UpdateScaleInformation(windowInfoHandle);

		}	// end "if (cflag)"
	
	wxLayoutAlgorithm layout;
	layout.LayoutFrame (this, m_mainWindow);

}	// end "ShowCoordinateView"



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


//-----------------------------------------------------------------------------
//					 Copyright (1988-2016)
//            c Purdue Research Foundation
//					All rights reserved.
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//				coordinates in the coordinate dialog window if it is
//				showing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
//
// Called By:			FixCursor in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 06/01/2016
//

void CMImageFrame::UpdateCursorCoordinates (
        char* lineValueStringPtr,
        char* columnValueStringPtr) 
{
	if (this != NULL)
		{
		((wxStaticText*)m_coordinatesBar->FindWindow (IDC_CursorColumn))->Show(true);
		((wxStaticText*)m_coordinatesBar->FindWindow (IDC_CursorLine))->Show(true);
		
		((wxStaticText *)(m_coordinatesBar->FindWindow (IDC_CursorLine)))->SetLabel(wxString::FromUTF8(lineValueStringPtr));
		((wxStaticText *)(m_coordinatesBar->FindWindow (IDC_CursorColumn)))->SetLabel(wxString::FromUTF8(columnValueStringPtr));
		
		m_coordinatesBar->Layout();
		
		}	// end "if (this != NULL)"

}	// end "UpdateCursorCoordinates"


//-----------------------------------------------------------------------------
//					 Copyright (1988-2016)
//				c Purdue Research Foundation
//					All rights reserved.
//
//	Function name:		void UpdateCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to update the cursor
//				coordinates in the coordinate dialog window if it is
//				showing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
//
// Called By:			FixCursor in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1992
//	Revised By:			Larry L. Biehl			Date: 06/01/2016

void CMImageFrame::UpdateCursorCoordinates (void)

{
	((wxStaticText*)m_coordinatesBar->FindWindow(IDC_CursorColumn))->Show(false);
	((wxStaticText*)m_coordinatesBar->FindWindow(IDC_CursorLine))->Show(false);
	
}	// end "UpdateCursorCoordinates"


void CMImageFrame::UpdateScaleInformation(double scale, char* scaleStringPtr) 
{		
	if (this != NULL)
		{
		if (scale <= 0)
			{
			//m_coordinatesBar->FindWindow(IDC_ScalePrompt)->Show(false);
			((wxStaticText*)(m_coordinatesBar->FindWindow(IDC_ScalePrompt)))->SetLabel(wxT(" "));
			}

		else { // scale > 0
			//m_coordinatesBar->FindWindow(IDC_ScalePrompt)->Show(true);
			((wxStaticText*)(m_coordinatesBar->FindWindow(IDC_ScalePrompt)))->SetLabel(wxT("Scale"));
			//m_topWindow->Layout();
			}	
			  
		((wxStaticText*)(m_coordinatesBar->FindWindow(IDC_Scale)))->SetLabel(wxString::FromUTF8(scaleStringPtr));
		
		}	// end "if (this != NULL)"

}	// end "UpdateScaleInformation"



void CMImageFrame::UpdateSelectedAreaInformation (
        char* areaDescriptionStringPtr,
        char* areaValueStringPtr) 
		  
{
	if (this != NULL)
		{
		((wxStaticText *) (m_coordinatesBar->FindWindow(IDC_NumberPixelsPrompt)))->SetLabel(wxString::FromUTF8(areaDescriptionStringPtr));

		((wxStaticText *) (m_coordinatesBar->FindWindow(IDC_NumberPixels)))->SetLabel(wxString::FromUTF8(areaValueStringPtr));
		
		}	// end "if (this != NULL)"
		
}	// end "UpdateSelectedAreaInformation"



void CMImageFrame::UpdateSelectionCoordinates (void)

{
	DoubleRect						coordinateRectangle;
	LongRect							lineColumnRectangle;
	//wxStaticText*					stctrl;
	
	SInt64							numberPixels;
	
	Handle							windowInfoHandle;
	

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
		((wxStaticText*)m_coordinatesBar->FindWindow(IDC_SelectionLine))->Show(false);
		((wxStaticText*)m_coordinatesBar->FindWindow(IDC_SelectionColumn))->Show(false);
		
		((wxStaticText*)m_coordinatesBar->FindWindow(IDC_NumberPixelsPrompt))->Show(false);
		((wxStaticText*)m_coordinatesBar->FindWindow(IDC_NumberPixels))->Show(false);
		((wxWindow*)m_coordinatesBar->FindWindow(IDC_AreaUnitsCombo))->Show(false);

		}	// end "else !GetSelectionCoordinates (&selectionRectangle)"
		
    //m_coordinatesBar->Layout();

}	// end "UpdateSelectionCoordinates"



void CMImageFrame::UpdateSelectionCoordinates (
				char*								lineValueStringPtr,
				char*								columnValueStringPtr)

{
	((wxStaticText*)(m_coordinatesBar->FindWindow(IDC_SelectionLine)))->SetLabel(wxString::FromUTF8(lineValueStringPtr));
	((wxStaticText*)(m_coordinatesBar->FindWindow(IDC_SelectionColumn)))->SetLabel(wxString::FromUTF8(columnValueStringPtr));
	
}	// end "UpdateSelectionCoordinates"



void CMImageFrame::UpdateSelectionWindowList ()

{
   wxCommandEvent						event;
   Handle fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
   FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	   
   
   gSelectionGraphViewCPtr->SetCheckIOMemoryFlag (TRUE);
   //printf("update selection window\n");
   //save expand/collapse information to ShapeInfoPtr
	//gSelectionGraphViewCPtr->m_frame->UpdateExpandFlag ();
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
		//SetUpXAxisPopUpMenu (gActiveImageWindowInfoH);
		gSelectionGraphViewCPtr->m_frame->m_comboXlabel->Show ();   
		gSelectionGraphViewCPtr->m_frame->m_panel2->Layout ();
		
		}	// end "else fileInfoPtr != NULL && ..."
   
   gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();
   gSelectionGraphViewCPtr->m_frame->UpdateSplitterWindowLayout ();
	
}	// end "UpdateSelectionWindowList"
