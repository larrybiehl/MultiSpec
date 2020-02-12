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
//	File:						xImageView.h
//	Implementation:		xImageView.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMImageView class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/11/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "SHistogram_class.h"

#include "xCoordinateBar.h"
#include "xImageCanvas.h"
#include "xMainFrame.h"

#include "wx/wx.h"
#include "wx/docview.h"
#include "wx/dc.h"


class CMImageCanvas;


class CMImageView : public wxView
{
	DECLARE_DYNAMIC_CLASS (CMImageView)
	
	public:
				// This structure contains the rectangle to be updated in the current
				// window.
		static RECT 						s_updateRect;
	
	
		CMImageView ();
	
		virtual 			~CMImageView ();

		Boolean 			CheckIfOffscreenImageExists (void);
	
		Boolean			CheckIfVectorOverlaysExist (void);

		void 				ClearView (bool);

		void 				ClientToDoc (
								wxPoint& 			point);

		void 				CreateScaledBitmap ();

		void 				DisposeImageWindowSupportMemory (void);

		void 				DrawLegend ();

		Boolean			GetActiveWindowFlag (void);

		SInt16			GetClassGroupCode (void);

		Boolean 			GetControlKeyFlag (void);

		UInt16 			GetDisplayPixelSize (void);

		Handle 			GetDisplaySpecsHandle (void);

		CMImageDoc* 	GetDocument (void);// Its already declared in wxview

		SInt16			GetImageType (void);

		CMImageWindow* GetImageWindowCPtr (void);

		CMImageFrame* 	GetImageFrameCPtr (void);

		CMLegendList* 	GetImageLegendListCPtr (void);

		CMLegendView* 	GetImageLegendViewCPtr (void);

		SInt16 			GetLegendFullHeight (void);

		SInt16 			GetLegendWidth (void);

		SInt16			GetNumberGroups (void);

		wxBitmap 		GetScaledBitmap ();

		Boolean 			GetShiftKeyFlag (void);

		SInt16			GetTitleHeight (void);

		const wxSize& 	GetViewOffset (void);

		SInt16 			GetWindowType (void);
	
		Boolean			ImageWindowIsAvailable (void);

		void 				InvalidateRect (
								Rect* 				rect,
								bool 					erase);

		void 				InvalidateRect (
								wxRect* 				rectp,
								bool 					erase);

		virtual void 	OnActivateView (
								bool 					bActivate,
								wxView* 				pActivateView,
								wxView* 				pDeactiveView);

		virtual bool 	OnClose (
								bool					deleteWindow = true);

		bool 				OnCreate (
								wxDocument* 		doc,
								long 					flags);

		virtual void 	OnDraw (
								wxDC* 				dc);

		virtual void 	OnUpdate (
								wxView*				sender,
								wxObject*			hint = (wxObject*)NULL);

		void				UpdateCursorCoordinates (void);

		void        	UpdateCursorCoordinates (
								LongPoint* 			mousePtPtr);

		void        	UpdateCursorCoordinatesNoDisplayOrigin (
								LongPoint* 			mousePtPtr,
								DisplaySpecsPtr	displaySpecsPr);

		void				UpdateSelectionCoordinates (void);

		void				ScrollChanged (void);

		void				SetActiveWindowFlag (
								Boolean				setting);

		void 				SetControlKeyFlag (
								Boolean 				flag);

		void 				SetImageWindowCPtr (
								CMImageWindow*		imageWindowCPtr);

		void 				SetLegendBitMapInfoHeaderHandle (
								Handle 				bitMapInfoHeaderHandle);

		void 				SetShiftKeyFlag (
								Boolean 				flag);

		void 				SetViewOffset (
								const wxSize& 		value);

		void				UpdateOffscreenMapOrigin (void);

		void 				UpdateScrolls (
								double 				newMagnification);

		void 				ZoomIn (void);

		void 				ZoomSize (void);

		void 				ZoomOut (void);
	
	

				// Bitmap object to display
		wxBitmap 							m_ScaledBitmap;

		wxSize 								m_ViewOffset;

		CMImageCanvas* 					m_Canvas;
	
		CMImageFrame* 						m_frame;
	
		CMainFrame* 						m_mainFrame;

				// Pointer to the display class for multispectral image.
		CMDisplay* 							m_displayMultiCPtr;

				// Pointer to the histogram class for the image.
		CMHistogram* 						m_histogramCPtr;

		double 								m_printerTextScaling;
	
				// Zooming factor
		double 								m_Scale;

				// Save the channel number for side by side display
		SInt16 								m_startchannel_sbs,
	
				 								m_xPixelsPerInch,
				 								m_yPixelsPerInch;

				// Flag indicating whether the current draw mode is for printing
				// or copying.
		Boolean 								m_printCopyModeFlag;

				// Flag indicating whether the class members have been
				// initialized successfully.
		Boolean 								m_initializedFlag;
	
	
	protected:
		void 				OnFocus (
								wxFocusEvent& 		event);
	
		void 				OnKeyDown (
								wxKeyEvent& 		event);

		void 				OnZoomInTool (
								wxCommandEvent& 	event);
		DECLARE_EVENT_TABLE ()
	
		SInt32			NormalizeScrollOffset (
								double				magnification,
								SInt32				scrollIncrement,
								SInt32				scrollOffset);

		void				UpdateScrollRange (
		 						double				magnification);

		void				InitialUpdate (void);
	
		 		// Flag indicated whether this view is active
		Boolean 								m_activeFlag;

		 		// Flag indicating whether the control key is down. The contexts
				// indicates what it will be used for.
		Boolean 								m_ctlKeyDownFlag;

		 		// Flag indicating whether the shift key is down. The context indicates
		 		// what it will be used for.
		Boolean 								m_shiftKeyDownFlag;

		 		// Pointer to the image window class for the image view.
		CMImageWindow* 					m_imageWindowCPtr;

		 		// Saved cursor line and column value
		SInt32 								m_cursorColumnValue,
				 								m_cursorLineValue;

};	// end "class CMImageView"


inline CMImageDoc* CMImageView::GetDocument ()
	{
	return (CMImageDoc*) wxView::GetDocument ();
	}


inline CMImageWindow* CMImageView::GetImageWindowCPtr ()
	{
	return (CMImageWindow*)m_imageWindowCPtr;

	}
