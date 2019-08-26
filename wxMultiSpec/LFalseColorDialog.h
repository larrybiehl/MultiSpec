//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LFalseColorDialog.h
//	Implementation:		LFalseColorDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFalseColorDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 08/23/2019
//
//------------------------------------------------------------------------------------
//
#if !defined __WFCOLDLG_H__
	#define	__WFCOLDLG_H__       
	     
#include "LDialog.h"


class CMFalseColorDlg : public CMDialog
	{
	// Construction
	public:
				// standard constructor
		CMFalseColorDlg (
				wxWindow* 			pParent = NULL,
				wxWindowID 			id = IDS_ProcessorBiPlot,
				const wxString& 	title = wxT("Assign Channels for False Color Palette"));
	
		Boolean					DoDialog (void);

	// Dialog Data
		enum { IDD = IDD_FalseColor };
		long	m_redChannel;
		long	m_greenChannel;
		long	m_blueChannel;
		//}}AFX_DATA

	// Implementation
	protected:
		bool TransferDataFromWindow();
		bool TransferDataToWindow();
		// Generated message map functions
			  void OnInitDialog(wxInitDialogEvent& event);
	
			  DECLARE_EVENT_TABLE()
			  void CreateControls();

			  LayerInfoPtr m_layerInfoPtr;
			  FileInfoPtr m_fileInfoPtr;

			  Boolean m_initializedFlag;

			  SInt16 m_blueChannelIndex;
			  SInt16 m_greenChannelIndex;
			  SInt16 m_redChannelIndex;

			  SInt32 m_maxChannelFeatureNum;
	
			  wxBoxSizer* bSizer322;
			  wxStaticText* m_staticText293;
			  wxStaticText* m_staticText294;
			  wxStaticText* m_staticText295;
			  wxStaticText* m_staticText296;
			  wxListBox* m_listBox5;
			  wxTextCtrl* m_textCtrl132;
			  wxTextCtrl* m_textCtrl133;
			  wxTextCtrl* m_textCtrl134;
			  wxButton* m_button78;
			  wxButton* m_button79;
	};
#endif // !defined __WFCOLDLG_H__
