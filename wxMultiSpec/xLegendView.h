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
//	File:						xLegendView.h
//	Implementation:		xLegendView.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMLegendView class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xLegendList.h"
#include "wx/choice.h"
#include "wx/window.h"


class CMLegendView : public wxPanel
{
	DECLARE_DYNAMIC_CLASS (CMLegendView)
	
	public:
		CMLegendView ();
		
		CMLegendView ( 	// protected constructor used by dynamic creation
				wxWindow*							parent,
				wxWindowID 							id,
				wxDocument* 						doc,
				CMImageView* 						view);
	
		virtual void OnInitialUpdate (
				wxInitDialogEvent& 				event);	// first time after construct

		void AdjustLegendListLength (void);
	
		CMImageDoc* GetDocument (void);
	
		CMImageFrame* GetImageFrameCPtr (void);

		CMImageView* GetImageView (void);
	
		CMLegendList* GetLegendListCPtr (void);
	
		void SetImageView (
				CMImageView*						imageViewCPtr);
	
		void SetLegendWidthSetFlag (void);
	
		void UpdateClassGroupComboList (
				SInt16								newClassGroupCode);
	
		void UpdateThematicLegendControls (void);
	
		void UpdateLegendComboControls ();
	

		enum { IDD = IDD_LegendWindow };
		
		CMLegendList*						m_legendListBox;
	

			// Implementation
	protected:
		void OnPaletteCtrlCloseUp (
				wxCommandEvent& 					event);
	
		void OnPaletteCtrlDropDown (
				wxCommandEvent& 					event);

		void OnPaletteCtrlSelendok (
				wxCommandEvent& 					event);

		void OnSelendokClassGroup (
				wxCommandEvent& 					event);

		void OnSelendokPalette (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		virtual ~CMLegendView ();
	
		void OnDropdownClassGroupCombo ();

		void SetupView ();
	
		void UpdateLegendComboBox ();

				// Flag indicating whether we are already in the SetLegendWidth
				// routine flag. This is to stop recursion in the routine from
				// call to OnSize..
				// Currently not used.
		static bool							s_inSetLegendWidthFlag;

		CMImageView*						m_imageViewCPtr;
		CMImageDoc* 						m_pDocument;
	
		wxChoice*							m_legendTitleCtrl;
	
		wxComboBox*							m_legendPaletteCtrl;
	
		int									m_classGroupCode,
												m_paletteSelection;
	
		SInt16								m_numberDisplayedListItems;
	
		bool									m_initializedFlag,
												m_legendWidthSetFlag,
												m_localPaletteUpToDateFlag,
												m_paletteChangedFlag;

};	// end "class CMLegendView"

inline CMImageDoc* CMLegendView::GetDocument ()
	   { return (CMImageDoc*)m_pDocument; }

inline CMImageView* CMLegendView::GetImageView ()
	   { return (CMImageView*)m_imageViewCPtr; }
