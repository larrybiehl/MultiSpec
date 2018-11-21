//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LReformatChange.h :
//															CMChangeFormatDlg Definition
//	Implementation:		LReformatChange.cpp
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMChangeChannelDescriptionDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 09/28/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LRCHANGE_H__
	#define	__LRCHANGE_H__

#include "LDialog.h"
#include "SMultiSpec.h"
#include "wx/wx.h"
//#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CMChangeFormatDlg dialog

class CMChangeFormatDlg : public CMDialog

{
	// Construction
	DECLARE_DYNAMIC_CLASS (CMChangeFormatDlg)
	public:
		CMChangeFormatDlg (); // standard constructor
		CMChangeFormatDlg ( // standard constructor
						wxWindow* pParent,
						wxWindowID id = wxID_ANY,
						const wxString& title = wxT("Set Image File Format Change Specifications"));

		Boolean DoDialog (
					FileInfoPtr outFileInfoPtr,
					ReformatOptionsPtr reformatOptionsPtr);

		enum
			{
			IDD = IDD_FileFormatChange
			};
	
		int m_bandInterleaveSelection;
		int m_channelSelection;
		int m_dataValueListSelection;
		int m_headerListSelection;
		int m_outputFileSelection;
		
		bool m_invertBottomToTopFlag;
		bool m_invertLeftToRightFlag;
		bool m_swapBytesFlag;
		bool m_transformDataFlag;
		bool m_writeChanDescriptionFlag;
		
				// GUI control variables
	
		wxBoxSizer* bSizer124;

    // Implementation
protected:
	//Boolean Initialize(void);

	void CreateControls ();
	void OnInitDialog (wxInitDialogEvent& event);
	void OnOutputInWavelengthOrder (wxCommandEvent& event);
	void OnSelendokBandInterleave (wxCommandEvent& event);
	void OnSelendokOutChannels (wxCommandEvent& event);
	void OnSelendokHeader (wxCommandEvent& event);
	void OnWriteChanDescriptions (wxCommandEvent& event);
	void OnSelendokDataValueType (wxCommandEvent& event);
	void SetChannelDescriptionFlag (void);
	bool TransferDataFromWindow ();
	bool TransferDataToWindow ();
	void OnTransformData (wxCommandEvent& event);
	DECLARE_EVENT_TABLE()

	UInt8				m_inputBandInterleaveString[64],
						m_inputDataValueTypeString[64],
						m_tiffMenuNameString[16];

	static ReformatOptionsPtr s_reformatOptionsPtr;

	Boolean			m_channelDescriptionAllowedFlag,
						m_channelThematicDisplayFlag,
						m_dataValueTypeSelectionFlag,
						m_GAIAFormatAllowedFlag,
						m_initializedFlag,
						m_outputInWavelengthOrderFlag,
						m_outputInWavelengthOrderAllowedFlag,
						m_savedChannelDescriptionFlag,
						m_sessionUserSetDataValueTypeSelectionFlag;

    int m_headerOptionsSelection;

    SInt16 m_dataValueTypeSelection,
    m_eightBitsPerDataSelection,
    m_savedDataValueTypeSelection;

			// GUI control pointers
    wxBitmapButton* m_bpButton4;
    wxBitmapButton* m_bpButton5;
	
    wxButton* m_button40;
    wxButton* m_button41;
	
    wxComboBox* m_comboBox22;
    wxComboBox* m_comboBox23;
    wxComboBox* m_comboBox24;
    wxComboBox* m_comboBox25;
    wxComboBox* m_comboBox26;
	
    wxCheckBox* m_checkBox41;
    wxCheckBox* m_checkBox42;
    wxCheckBox* m_checkBox43;
    wxCheckBox* m_checkBox44;
    wxCheckBox* m_checkBox45;
    wxCheckBox* m_checkBox46;
	
    wxStaticText* m_staticText136;
    wxStaticText* m_staticText137;
    wxStaticText* m_staticText138;
    wxStaticText* m_staticText139;
    wxStaticText* m_staticText140;
    wxStaticText* m_staticText141;
    wxStaticText* m_staticText142;
    wxStaticText* m_staticText143;
    wxStaticText* m_staticText144;
    wxStaticText* m_staticText145;
    wxStaticText* m_staticText146;
    wxStaticText* m_staticText147;
    wxStaticText* m_staticText148;
    //wxStaticText* m_staticText63;
    //wxStaticText* m_staticText64;
    //wxStaticText* m_staticText65;
    //wxStaticText* m_staticText66;
    //wxStaticText* m_staticText60;
    //wxStaticText* m_staticText62;
    wxStaticText* m_staticText161;
    wxStaticText* m_staticText162;
    wxStaticText* m_staticText163;
    wxStaticText* m_staticText164;
    wxStaticText* m_staticText165;
    wxStaticText* m_staticText166;
	
    //wxTextCtrl* m_linestartctrl;
    //wxTextCtrl* m_lineendctrl;
    //wxTextCtrl* m_lineintctrl;
    //wxTextCtrl* m_colstartctrl;
    //wxTextCtrl* m_colendctrl;
    //wxTextCtrl* m_colintctrl;
};

#endif	// !defined __LRCHANGE_H__
