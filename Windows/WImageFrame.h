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
//	File:						WImageFrame.h
//	Implementation:		WImageFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMImagDoc class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#ifndef __AFXEXT_H__
	#include <afxext.h>
#endif         
                     
#include "WImageView.h"
#include "WCoordinateBar.h"
#include "WLegendView.h"
#include "WImageSplitterWnd.h"

class CMImageFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE (CMImageFrame)
	protected:
		CMImageFrame ();		// protected constructor used by dynamic creation
	
	public:
		void ActivateImageWindowItems (
				Boolean 								activateFlag,
				Boolean								changeWindowFlag);
	
		void ChangeClassGroupDisplay (
				SInt16								newClassGroupCode,
				Boolean								updateClassGroupListFlag);

		LRESULT DoRealize (
				Boolean								backgroundFlag,
				CWnd*									hWnd);
	
		Boolean GetActiveWindowFlag (void);
	
		void GetCoordinateViewComboText (
				char*									comboItemStringPtr,
				UInt16								itemNumber);
	
		UInt16 GetDisplayPixelSize (void);		// inline
	
		CMImageDoc* GetDocument (void);				// inline
	
		CMLegendView* GetLegendViewCPtr (void);		// inline
	
		void InitialUpdate (
				CSize     							imageViewSize);
	
		void OnViewCoordinatesBar (
				Boolean								displayCoordinatesFlag);
	
		void SetImageViewCPtr (
				CMImageView*						imageViewCPtr);
	
		void SetLegendWidth (
				SInt16								newLegendWidth);
	
		void SetSplitterParameters (
				SInt16								windowType);
	
		void UpdateActiveImageWindowInfo (void);

		void UpdateScaleInformation (
				double								scale,
				char*									scaleStringPtr);
	
		void UpdateSelectedAreaInformation (
				char*									areaDescriptionStringPtr,
				char*									areaValueStringPtr);
	
		void UpdateSelectionCoordinates (void);
	
		void UpdateSelectionCoordinates (
				char*									lineValueStringPtr,
				char*									columnValueStringPtr);
	
		void UpdateCursorCoordinates (void);
	
		void UpdateCursorCoordinates (
				char*									lineValueStringPtr,
				char*									columnValueStringPtr);
	
	protected:
		CMImageSplitterWnd				m_wndSplitter;
	
		CMCoordinateBar					m_coordinatesBar;
	
		CMLegendView*						m_imageLegendViewCPtr;
	
		CMImageView*						m_imageViewCPtr;
	
		UInt16								m_displayPixelSize;
	
				 // Flag indicated whether this image frame is active.
		Boolean								m_imageFrameActiveFlag;
	
		Boolean								m_forcePaletteBackgroundUpdateFlag;
	
		static Boolean						s_forcePaletteBackgroundFlag;
	
	//public:
	protected:
		virtual ~CMImageFrame ();
	
		virtual BOOL OnCreateClient (
				LPCREATESTRUCT 					lpcs,
				CCreateContext* 					pContext);

		// Generated message map functions
		//{{AFX_MSG (CMImageFrame)
		afx_msg void OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnClearSelectionRectangle ();
	
		afx_msg void OnClose ();
	
		afx_msg int OnCreate (
				LPCREATESTRUCT 					lpCreateStruct);
	
		afx_msg void OnEditCopy ();
	
		afx_msg void OnEditSelectionRectangle ();
	
		afx_msg void OnEditSelectAll ();
	
		afx_msg void OnFilePrint ();
	
		afx_msg void OnFilePrintPreview ();
	
		afx_msg void OnGetMinMaxInfo (
				MINMAXINFO FAR* 					lpMMI);
	
		afx_msg void OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnMagnification (void);
	
		afx_msg void OnOverlay ();
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
	
		afx_msg void OnSetFocus (
				CWnd* 								pOldWnd);
	
		afx_msg void OnSysCommand (
				UINT 									nID,
				LPARAM 								lParam);
	
		afx_msg void OnUpdateClearSelectionRectangle (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditCopy (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditSelectAll (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditSelectionRectangle (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileImageClose (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrint (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrintPreview (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateMagnification (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOverlay (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateZoomIn (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateZoomIndicator (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateZoomOut (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnZoomIn (void);
	
		afx_msg void OnZoomOut (void);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
};	// end class CMImageFrame
	   
inline UInt16 CMImageFrame::GetDisplayPixelSize ()
	   { return m_displayPixelSize; }                       
	
inline CMImageDoc* CMImageFrame::GetDocument ()
	   { return m_imageViewCPtr->GetDocument (); }                         
	
inline CMLegendView* CMImageFrame::GetLegendViewCPtr ()
	   { return m_imageLegendViewCPtr; }   

