// LDisplayMinMaxDialog.h : header file
// 
// Revised by Larry Biehl on 06/20/2017
//
#if !defined __LDMINDLG_H__
#define	__LDMINDLG_H__ 

#include "SMultiSpec.h"
#include "wx/wx.h"
#include "LDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMDisplayMinMaxDialog dialog

class CMDisplayMinMaxDialog : public CMDialog {
    // Construction
public:
	CMDisplayMinMaxDialog();
	CMDisplayMinMaxDialog(wxWindow* pParent = NULL, wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Enhancement Specifications")); // standard constructor 
	~CMDisplayMinMaxDialog(); // standard desctructor
	Boolean DoDialog(
			SInt16* channelsPtr,
			SInt16 rgbColors,
			SInt16 displayType,
			SInt16 numberLevels,
			SInt16* percentTailsClippedPtr,
			SInt16* minMaxSelectionPtr,
			double* minMaxValuesPtr);

	// Dialog Data

	enum {
	  IDD = IDD_DisplayEnhancement
		};
	double m_newBlueChannelMax;
	double m_newBlueChannelMin;
	double m_newGreenChannelMax;
	double m_newGreenChannelMin;
	double m_newRedChannelMax;
	double m_newRedChannelMin;
	int m_percentClipped;
	int m_localMinMaxCode;

	//wxString m_redminString;
	//wxString m_redmaxString;
	//wxString m_greenminString;
	//wxString m_greenmaxString;
	//wxString m_blueminString;
	//wxString m_bluemaxString;
	wxString m_percentClippedString;
	
protected:
	wxBoxSizer* bSizer1;
	wxStaticText* m_staticText34;
	wxStaticText* m_staticText35;
	wxStaticText* m_staticText5;
	wxStaticText* m_staticText7;
	wxStaticText* m_staticText9;
	wxStaticText* m_staticText36;
	wxStaticText* m_staticText37;
	wxStaticText* m_staticText38;
	wxStaticText* m_staticText12;
	wxStaticText* m_staticText23;
	wxStaticText* m_staticText241;
	wxStaticText* m_staticText251;
	wxStaticText* m_staticText261;
	wxStaticText* m_staticText271;
	wxStaticText* m_staticText281;
	wxStaticText* m_staticText24;
	wxStaticText* m_staticText25;
	wxStaticText* m_staticText26;
	wxStaticText* m_staticText27;
	wxStaticText* m_staticText21;
	wxStaticText* m_staticText28;
	wxStaticText* m_staticText29;
	wxStaticText* m_staticText30;

	wxTextCtrl* m_textCtrl1;
	wxTextCtrl* m_textCtrl2;
	wxTextCtrl* m_textCtrl3;
	wxTextCtrl* m_textCtrl4;
	wxTextCtrl* m_textCtrl5;
	wxTextCtrl* m_textCtrl6;
	wxTextCtrl* m_textCtrl7;
	//    wxTextCtrl* m_textCtrl12;
	//    wxTextCtrl* m_textCtrl13;
	//    wxTextCtrl* m_textCtrl14;
	//    wxTextCtrl* m_textCtrl15;
	//    wxTextCtrl* m_textCtrl16;
	//    wxTextCtrl* m_textCtrl17;

	wxRadioButton* m_radioBtn1;
	wxRadioButton* m_radioBtn2;
	wxRadioButton* m_radioBtn3;
	wxRadioButton* m_radioBtn4;

	wxButton* m_button1;
	wxButton* m_button2;
	
	void CheckNewMinMaxValue (
				int						id,
				SInt16					channelIndex);
	void CreateControls();
	SInt16 GetNumberDecimalDigits (SInt16 channelIndex);
	bool TransferDataFromWindow();
	bool TransferDataToWindow();
	void UpdateMinMaxOptions();
	int VerifyDataRangeStartEndValues (
				double				minValue,
				double				maxValue,
				int					minID);

			// Generated message map functions
	//virtual BOOL OnInitDialog();
	void OnInitDialog (wxInitDialogEvent& event);
	void OnChannelMinMax (wxCommandEvent& event);
	void OnChangePercentClipped (wxCommandEvent& event);
	void OnChangeMinMaxBlue (wxCommandEvent& event);
	void OnChangeMinMaxGreen (wxCommandEvent& event);
	void OnChangeMinMaxRed (wxCommandEvent& event);
	//void OnButtonPress (wxKeyEvent& event);

	DECLARE_EVENT_TABLE()

	HistogramSpecsPtr m_histogramSpecsPtr;
	HistogramSummaryPtr m_histogramSummaryPtr;

	double* m_minMaxValuesPtr;

	SInt16 *m_channelsPtr;

	double m_maxValue, m_minValue;

	SInt16 m_displayType,
	m_minMaxOptionCode,
	m_numberLevels,
	m_rgbColors;

	Boolean m_histogramAvailableFlag,
	m_updatingMinMaxFlag;

public:
	void OnStnClickedRedchannelmin(wxCommandEvent& event);
};

#endif // !defined __LDISPDLG_H__
