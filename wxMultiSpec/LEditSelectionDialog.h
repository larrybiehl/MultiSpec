//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LEditSelectionDialog.h
//	Implementation:		LEditSelectionDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEditCoordinatesDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------
//
#if !defined __LEDITSELREC_H__
#define	__LEDITSELREC_H__  
	
#include "LDialog.h"

typedef bool BOOL;

class CMEditCoordinatesDlg : public CMDialog 
	{
		 // Construction
	public:
				// standard constructor
		CMEditCoordinatesDlg (
				wxWindow* pParent = NULL,
				wxWindowID id=wxID_ANY,
				const wxString& title= wxT("Edit Selection Rectangle"));

				// Dialog Data

		enum {
			IDD = IDD_FieldCoordinates
			};
		double				m_newColumnStart;
		double				m_newColumnEnd;
		double				m_newLineEnd;
		double				m_newLineStart;
		double				m_doubleValueCheck;
		int					m_listSelectionUnits;
		unsigned long		m_uInt32ValueCheck;
		BOOL					m_applyToAllWindowsFlag;
		BOOL					m_useStartLineColumnFlag;

		Boolean DoDialog (
				WindowPtr windowPtr,
				WindowInfoPtr windowInfoPtr,
				LongRect* inputSelectionRectanglePtr,
				LongRect* selectionRectanglePtr,
				DoubleRect* coordinateRectanglePtr,
				SInt16 pointType,
				SInt16* unitsDisplayCodePtr,
				Boolean* changedFlagPtr,
				Boolean* applyToAllWindowsPtr,
				Boolean* useStartLineColumnFlagPtr,
				Boolean* previewWasUsedFlagPtr,
				SInt16 stringID);

		// Implementation
	protected:
		bool TransferDataFromWindow();
		bool TransferDataToWindow();
		void CreateControls();
		void CheckOKFlag(void);

		Boolean CheckLineColumnValues(
				UInt16* controlIDPtr);

		void OnInitDialog(wxInitDialogEvent& event);
		void OnChangeNewColumnEnd(wxCommandEvent& event);
		void OnChangeNewColumnStart(wxCommandEvent& event);
		void OnChangeNewLineEnd(wxCommandEvent& event);
		void OnChangeNewLineStart(wxCommandEvent& event);
		void OnPreview(wxCommandEvent& event);
		void OnSelendokCoordinateUnits(wxCommandEvent& event);
		void OnApplyToAllCheckbox(wxCommandEvent& event);
		DECLARE_EVENT_TABLE ()
	
	
		DoubleRect			m_inputCoordinateRectangle,
								m_minMaxCoordinateRectangle;

		LongRect				m_minMaxSelectionRectangle;

		double				m_realValue;

		WindowPtr			m_windowPtr;

		LongRect*			m_inputSelectionRectanglePtr;

		SInt16				m_selectionUnits,
								m_unitsDisplayCode;

		Boolean				m_coordinateUnitsChangingFlag,
								m_valueChangedFlag,
								m_initializedFlag,
								m_previewWasUsedFlag,
								m_stringID;

		SInt16				m_pointType;
		WindowInfoPtr		m_windowInfoPtr;
		LongRect*			m_selectionRectanglePtr;
		DoubleRect*			m_coordinateRectanglePtr;

		wxBoxSizer* bSizer200;
		wxStaticText* m_staticText313;
		wxStaticText* m_staticText314;
		wxStaticText* m_staticText315;
		wxStaticText* m_staticText316;
		wxStaticText* m_staticText317;
		wxStaticText* m_staticText318;
		wxStaticText* m_staticText319;
		wxStaticText* m_staticText3141;
		wxStaticText* m_staticText3151;
		wxStaticText* m_staticText3161;
		wxStaticText* m_staticText3171;
		wxStaticText* m_staticText3181;
		wxStaticText* m_staticText3191;

		wxTextCtrl* m_textCtrl182;
		wxTextCtrl* m_textCtrl183;
		wxTextCtrl* m_textCtrl1821;
		wxTextCtrl* m_textCtrl1831;

		wxCheckBox* m_checkBox15;
		wxCheckBox* m_checkBox16;

		wxButton* m_button29;
		wxButton* m_button30;
		wxButton* m_button31;

		wxChoice* m_choice1;
	
	};
#endif // !defined __LEDITSELREC_H__ 
