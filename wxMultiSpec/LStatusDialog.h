//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatusDialog.h
//	Implementation:		LStatusDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CShortStatusDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/02/2019
//
//------------------------------------------------------------------------------------
//
#include "SMultiSpec.h"

#include "LDialog.h"
#include "LImageView.h"
#include "wx/wx.h"

typedef long unsigned int UINT;

class CShortStatusDlg : public CMDialog
	{
	DECLARE_DYNAMIC_CLASS (CShortStatusDlg)
		// Construction
	public:
		CShortStatusDlg ();
	
		 		// standard constructor
		CShortStatusDlg (
		 		UInt16 			identifier,
		 		wxWindow* 		pParent = NULL);

		Boolean				m_initializedFlag,
								m_canceledCommandInitiatedFlag;

				// Dialog Data

		enum { IDD = IDD_GraphicsStatus };

		wxBoxSizer* bSizer35;
		wxBoxSizer* bSizer93;
		wxBoxSizer* bSizer195;
	
			// Implementation
	protected:
		UInt16 m_identifier;
		wxStaticText* m_staticText39;
		wxStaticText* m_staticText40;
		wxStaticText* m_staticText41;
		wxStaticText* m_staticText42;
		wxStaticText* m_staticText43;
		wxStaticText* m_staticText44;
		wxStaticText* m_staticText45;
		wxStaticText* m_staticText46;
		wxStaticText* m_staticText47;
		wxStaticText* m_staticText48;
		wxStaticText* m_staticText49;
		wxStaticText* m_staticText50;
		wxStaticText* m_staticText51;
		wxStaticText* m_staticText52;
		wxStaticText* m_staticText53;
		wxStaticText* m_staticText54;
		wxStaticText* m_staticText55;
		wxStaticText* m_staticText56;
		wxStaticText* m_staticText57;
		wxStaticText* m_staticText59;
		wxStaticText* m_staticText58;

		wxStaticText* m_staticText105;
		wxStaticText* m_staticText106;
		wxStaticText* m_staticText308;
		wxStaticText* m_staticText309;
		wxStaticText* m_staticText310;
		wxStaticText* m_staticText311;
		wxStaticText* m_staticText312;
		wxButton* m_button11;

		wxGauge* m_gauge3;
		wxButton* m_button28;
		wxButton* m_button6;

		void CreateControl ();
		void CreateControl_Short ();
		void CreateControl_Graphics ();
		void OnInitDialog (wxInitDialogEvent& event);

		DECLARE_EVENT_TABLE ()

			// Implementation
	private:
		void OnCharHook (wxKeyEvent& event);
		void OnCancel (wxCommandEvent& event);
		void OnClose (wxCloseEvent& event);
		void OnKeyDown (wxKeyEvent& event);

	};
