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
//	File:						xImageFrame.h
//	Implementation:		xImageFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMImageFrame class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/24/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "SDisplay_class.h"

#include "xCoordinateBar.h"
#include "xImageView.h"
#include "xLegendView.h"
#include "xTitleBar.h"

#include "wx/docview.h"
#include "wx/wx.h"
#include "wx/laywin.h"
#include "wx/string.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/icon.h"
#include "wx/menu.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/dialog.h"


class CMImageFrame : public wxDocChildFrame
{
		 DECLARE_DYNAMIC_CLASS (CMImageFrame)

	public:
		static Boolean						s_forcePaletteBackgroundFlag;
	
		CMImageFrame ();
	
		CMImageFrame (
				wxDocument* 						doc,
				wxView* 								view,
				wxDocParentFrame*					parent);
	
		~CMImageFrame ();
	
		void ActivateImageWindowItems (
				Boolean 								activateFlag,
				Boolean 								changeWindowFlag);
	
		void UpdateSelectionWindowList ();
	
		void ChangeClassGroupDisplay (
				SInt16 								newClassGroupCode);
	
		Boolean GetActiveWindowFlag (void);

		void GetCoordinateViewComboText (
				char* 								comboItemStringPtr,
				UInt16 								itemNumber);
	
		int GetTitleAndToolBarHeight (void);

		UInt16 GetDisplayPixelSize (void); // inline

		CMImageDoc* GetDocument (void); // inline

		CMLegendView* GetLegendViewCPtr (void); // inline

		void InitialUpdate (
				CSize 								imageViewSize);

		void SetActiveWindowFlag (
				Boolean 								setting);

		void SetImageViewCPtr (
				CMImageView* 						imageViewCPtr);

		void SetLegendWidth (
				SInt16 								newLegendWidth);

		void SetSplitterParameters (
				SInt16 								windowType);
	
		void ShowCoordinateView (
				SInt16								inputCode);
	
		void ShowTitleView (
				SInt16								inputCode);

		void UpdateActiveImageWindowInfo (void);

		void UpdateScaleInformation (
				double 								scale,
				char* 								scaleStringPtr);

		void UpdateSelectedAreaInformation (
				char* 								areaDescriptionStringPtr,
				char* 								areaValueStringPtr);

		void UpdateSelectionCoordinates (void);

		void UpdateSelectionCoordinates (
				char* 								lineValueStringPtr,
				char* 								columnValueStringPtr);

		void UpdateCursorCoordinates (void);

		void UpdateCursorCoordinates (
				char* 								lineValueStringPtr,
				char* 								columnValueStringPtr);
	
	
		enum
			{
			REFRESH=101,
			MDI_CHANGE_TITLE,
			MDI_CHANGE_POSITION,
			MDI_CHANGE_SIZE,
			ID_WINDOW_TOP,
			ID_WINDOW_RIGHT,
			ID_WINDOW_LEFT,
			ID_WINDOW_TITLE
			};

		LongPoint 							m_TempLongPoint;
		wxPoint 								m_scrollPos;
		wxSize 								m_frameSizeSaved;
	
		wxSashLayoutWindow				*m_mainWindow,	// Window for displaying image
												*m_topWindow,	// Window for displaying CMCoordinatebar
												*m_leftWindow,	// Window for displaying CMLegendView
												*m_titleWindow;	// Window for side by side title images
	
		wxCursor 							m_blinkOpenCursor,
												m_blinkShutCursor;
	
		CMCoordinateBar*					m_coordinatesBar;
		CMLegendView*						m_imageLegendViewCPtr;
		CMTitleBar*							m_titleBar;
	
		wxStaticText* 						m_zoomText;
	
		double 								m_zoom;
	
		bool   								m_frameMaximized;
	

	private:
		void DoKeyDownDownArrow (
				wxCommandEvent& 					event);
	
		void DoKeyDownLeftArrow (
				wxCommandEvent& 					event);
	
		void DoKeyDownRightArrow (
				wxCommandEvent& 					event);
	
		void DoKeyDownUpArrow (
				wxCommandEvent& 					event);
	
		void DoZoomIn (
				wxMouseEvent&						event);
	
		void DoZoomOut (
				wxMouseEvent&						event);
	
		void DoZoomToOne (
				wxMouseEvent&						event);
	
		void OnActivate (
				wxActivateEvent& 					event);
	
		void OnCancelDraw (
				wxCommandEvent& 					event);
	
		void OnChar (
				wxKeyEvent& 						event);
	
		void OnCharHook (
				wxKeyEvent& 						event);
	
		void OnClose (
				wxCommandEvent& 					event);

	
		void OnEditSelectAll (
				wxCommandEvent& 					event);

		void OnEditClearSelectionRectangle (
				wxCommandEvent& 					event);

	
		void OnFileSave (
				wxCommandEvent& 					event);

		void OnFileSaveAs (
				wxCommandEvent& 					event);

		void OnFocus (
				wxFocusEvent& 						event);
	
		void OnKeyDown (
				wxKeyEvent& 						event);
	
		void OnLeftUp (
				wxMouseEvent& 						event);
	
		void OnMaximizeWindow (
				wxMaximizeEvent& 					event);
	
		void OnRefresh (
				wxCommandEvent& 					event);

		void OnSashDrag (
				wxSashEvent& 						event);
	
		void OnSize (
				wxSizeEvent& 						event);
	
		void OnUpdateEditCopy (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditSelectAll (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateEditClearSelectionRectangle (
				wxUpdateUIEvent& 					pCmdUI);

	
		void OnUpdateFileCloseWindow (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateFileSave (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateFileSaveAs (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateFilePrint (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateFilePrintPreview (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateFilePrintSetup (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateOverlay (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateRefresh (
				wxUpdateUIEvent& 					pCmdUI);

		void OnUpdateWindowNewSelectionGraph (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateMagnification (
				wxUpdateUIEvent& 					event);
	
		void OnUpdateZoomIn (
				wxUpdateUIEvent& 					event);
	
		void OnUpdateZoomOut (
				wxUpdateUIEvent& 					event);
	
		void OnUpdateZoomIndicator (
				wxUpdateUIEvent& 					event);

		void OnViewCoordinatesBar (
				wxCommandEvent& 					event);
	
		void OnUpdateViewCoordinatesBar (
				wxUpdateUIEvent& 					event);
	
		void OnWindowNewSelectionGraph (
				wxCommandEvent& 					event);

		void OnWindowShowCoordinateView (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	

		void GetMinimumDisplaySizeForImage (
				Handle 								windowInfoHandle,
				LongPoint* 							minimumWindowSizePointPtr);
	
		#ifndef multispec_wxmac
			 enum{
				ID_FILE_SAVE,
				ID_FILE_SAVE_AS
			 };
		#endif

		CMImageView*						m_imageViewCPtr;
	
		#if defined multispec_wxmac
			wxToolBar 							*m_toolBar;
		#endif

		UInt16								m_displayPixelSize;

				// Flag indicated whether this image frame is active.

		Boolean								m_forcePaletteBackgroundUpdateFlag,
												m_imageFrameActiveFlag;
	
};	// end "class CMImageFrame"

inline UInt16 CMImageFrame::GetDisplayPixelSize ()
	   { return m_displayPixelSize; }

inline CMImageDoc* CMImageFrame::GetDocument ()
   { return m_imageViewCPtr->GetDocument (); }

inline CMLegendView* CMImageFrame::GetLegendViewCPtr ()
   { return m_imageLegendViewCPtr; }
