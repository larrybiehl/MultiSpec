//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassPairWeightsDialog.h
//	Implementation:		LClassPairWeightsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 03/13/2018
//	
//------------------------------------------------------------------------------------

#if !defined LCPWTDLG_H
	#define LCPWTDLG_H

	#include "LDialog.h"

	class CMClassPairWeightDlg : public CMDialog
		{
				// Construction
		public:
					 // standard constructor
			CMClassPairWeightDlg (
					wxWindow*						pParent = NULL,
					wxWindowID						id = IDD_ClassPairWeights,
					const wxString&				title = wxT("Specify Class Pair Weights"));

			SInt16 DoDialog (
					UInt16							numberOfClassesToUse,
					SInt16**							weightsListPtrPtr,
					SInt16							interClassWeightsSelection,
					SInt16*							defaultClassPairWeightPtr);

					// Dialog Data
		
			enum {
				IDD = IDD_ClassPairWeights
				};
		
			long								m_newClassPairWeight;
			long								m_localDefaultClassPairWeight;

		protected:
			bool								TransferDataFromWindow ();
			bool								TransferDataToWindow ();

				// Implementation
		
		protected:
		
					// Generated message map functions
		
			void OnInitDialog (wxInitDialogEvent& event);
			void OnHelpButton (wxCommandEvent& event);
			void OnSelchangeClassList (wxCommandEvent& event);
			void OnAddButton (wxCommandEvent& event);
			void OnChangeNewWeight (wxCommandEvent& event);
			void OnRemoveButton (wxCommandEvent& event);
			void OnSelchangeClassPairWeightList (wxCommandEvent& event);
			void OnChangeDefaultWeight (wxCommandEvent& event);
			void CreateControls ();
			DECLARE_EVENT_TABLE ()
					
			wxFont				m_weightsListFont;

			SInt16**				m_weightsListPtrPtr;

			SInt16*				m_defaultClassPairWeightPtr;

			SInt16				m_selectedClassCell,
									m_classPairWeightsSelection,
									m_selectedWeightGroupCell;

			UInt16				m_numberOfClassesToUse;

			Boolean				m_initializedFlag;

			wxBoxSizer*			bSizer240;
		
			wxButton				*m_button51,
									*m_button52,
									*m_button53,
									*m_button54,
									*m_button55;
		
			wxListBox			*m_listBox2,
									*m_listBox3;
		
			wxStaticText		*m_staticText239,
									*m_staticText240,
									*m_staticText241,
									*m_staticText242;
		
			wxTextCtrl			*m_textCtrl127,
									*m_textCtrl128;
						
		};
#endif	// !defined LCPWTDLG_H
