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
//	File:						xGraphView.h
//	Implementation:		xGraphView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMGraphView and
//								CMGraphViewButton classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/24/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "wx/wx.h"
//#include "wx/docmdi.h"
#include "wx/docview.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "xMainFrame.h"
#include "xGraphFrame.h"

#include "xMultiSpec.h"

#include "wx/evtloop.h"


class CMGraphViewButton : public wxButton
{
	public:
		CMGraphViewButton ();
	
		virtual ~CMGraphViewButton ();

		void SetButtonID (
				UInt16  								buttonID);
	
		void SetGraphViewCPtr (
				CMGraphView*  						graphViewCPtr);

		void OnLines2 (
				UInt32 								menuID);
	
		void OnSelectVector (
				UInt32								menuID);
	
		void OnBinWidth (
				UInt32								menuID);
	
	
		CMGraphView* 						m_graphViewCPtr;
		
		UInt16 								m_buttonID;

};	// end "class CMGraphViewButton"


//------------------------------------------------------------------------------------

class CMGraphView : public wxView 
{
	DECLARE_DYNAMIC_CLASS (CMGraphView)

	public:
		static Handle       				s_selectionIOInfoHandle;
	
		static UInt16       				s_graphicsWindowCount,
			       							s_numberOfGWindows;
	
		static wxBrush						s_grayBrush;
	
		static wxFont 						s_font;
	
		//static wxPen 					s_bluePen;
		static wxPen 						s_grayPen;
		//static wxPen 					s_greenPen;
		static wxPen						s_ltGrayPen;
		//static wxPen						s_redPen;
		static wxPen						s_whitePen;
		
		CMGraphView ();
	
		~CMGraphView ();
	
		void DoFilePrint ();
		  
		void DrawGraphGrowIcon (
				GraphPtr 							graphRecordPtr);
	
		Boolean FinishGraphRecordSetUp (
				SInt16*								channelListPtr,
				SInt32								channelListLength,
				SInt32								vectorLength,
				SInt32								numberYVectors,
				SInt32								numberSets,
				UInt16								xValueType,
				UInt16								yValueType);
	
		CMGraphDoc* GetDocument ();

		Handle GetGraphRecordHandle ();

		void GetGraphWindowClientRect (
				Rect*									clientRectPtr);
	
		UInt16 GetGraphWindowNumber ();
	
		Handle GetSelectionIOHandle ();
	
		ShapeInfoPtr GetShapeInfoFromHandle (
				Handle 								shapeInfoHandle,
				Handle 								activeImageWindowHandle);
	
		WindowPtr GetWindowPtr ();
	
		wxChoice* GetXAxisComboBoxPtr ();
	
		void HideFeatureList ();

		void IntializeGraphRecord (void);
	
		void MoveGraphControls ();
	
		virtual void OnActivateView (
				bool 									bActivate,
				wxView* 								pActivateView,
				wxView* 								pDeactiveView);
	
		void OnBinWidth (
				int 									menuID);
	
		bool OnClose (
				bool									deleteWindow);
	
		bool OnCreate (
				wxDocument*							doc,
				long 									WXUNUSED (flags));
	
		void OnDestroy (
				wxCloseEvent& 						event);
	
		virtual void OnDraw (
				wxDC* 								dc);
	
		virtual void OnInitialUpdate ();
	
		void OnOverlay (
				int 									menuID);
	
		void OnPaint (
				wxPaintEvent& 						event);
	
		void OnSelectVector (
				int 									menuID);
	
		void OnUpdateFileSaveAs (
				CCmdUI*  							pCmdUI);

		void ResetListControls ();
	
		void SetCheckIOMemoryFlag (
				Boolean								flagSetting);
	
		void SetWindowPtr (
				WindowPtr 							window);
	
		void ShowFeatureList ();
	
		void UpdateDataListCtrl ();

		Boolean UpdateGraphControls ();
	
		void UpdateShowOrHideFeatureList ();


		enum { IDD = IDD_GraphWindow };
	
		CMGraphFrame*						m_frame;
	
		WindowPtr							m_window;

		wxMenu								*m_binWidthMenu,
												*m_graphOverlayMenu,
												*m_selectVectorMenu;

		wxToolBar							*m_toolBar1;
	
		CMGraphViewButton       		m_binWidth,
			       							m_nextChannel,
			       							m_overlayControl,
			       							m_previousChannel,
			       							m_selectVectors;

		double								m_printerTextScaling;
	
		Handle								m_graphRecordHandle;
	
		int									m_xPixelsPerInch,
												m_yPixelsPerInch;
	
		UInt16								m_graphicsWindowNumber;
	
				// Flag indicating whether the current draw mode is for printing
				// or copying.

		Boolean								m_printCopyModeFlag;
	
	private:
		bool LoadChannelDescriptionIntoList (
				FileInfoPtr 						fileInfoPtr,
				LayerInfoPtr 						layerInfoPtr,
				SInt16 								channel);
	
		bool LoadChannelNumbersAndValues (
				GraphPtr 							graphRecordPtr);
	
		void OnPreviousChannel (
				wxCommandEvent& 					event);
	
		void OnNextChannel (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		wxString GetShapeFileName (
				ShapeInfoPtr 						shapeInfoPtr);
	
		void ReloadXAxis (int selection);
	
		void UpdateFeatureListCtrl (
				wxPoint 								SelectionPoint);
	
		void UpdateFieldNames (
				ShapeInfoPtr 						shapeInfoPtr,
				int 									shapefileNumber);
	
		void UpdateFieldValues (
				ShapeInfoPtr 						shapeInfoPtr,
				wxPoint 								SelectionPoint,
				int 									shapefileNumber);
	
};	// end "class CMGraphView"

	inline Handle CMGraphView::GetGraphRecordHandle ()
   	{ return (Handle)m_graphRecordHandle; }
      
	inline UInt16 CMGraphView::GetGraphWindowNumber ()
   	{ return (UInt16)m_graphicsWindowNumber; }
      
	inline Handle CMGraphView::GetSelectionIOHandle ()
   	{ return (Handle)s_selectionIOInfoHandle; }
      
	inline WindowPtr CMGraphView::GetWindowPtr ()
   	{ return (WindowPtr)m_window; }
      
	inline void CMGraphView::SetWindowPtr (WindowPtr window)
   	{ m_window = window; }
