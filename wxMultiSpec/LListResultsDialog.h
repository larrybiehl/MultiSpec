// LListResultsDialog.h : header file
//      
// Revised by Larry Biehl on 06/21/2017
//

#if !defined __WLSTRDLG_H__
#	define	__WLSTRDLG_H__             

#	include "LDialog.h"
	//typedef bool BOOL;
	/////////////////////////////////////////////////////////////////////////////
	// CMListResultsDialog dialog

	class CMListResultsDialog : public CMDialog 
	{
				// Construction
	public:
		CMListResultsDialog(wxWindow* pParent = NULL, wxWindowID id = IDD_ClassifyDialog, const wxString& title = wxT("Set List Results Specifications")); // standard constructor      

		~CMListResultsDialog(); // standard desctructor  

		Boolean DoDialog(void);

		// Dialog Data

		enum {
			IDD = IDD_ListResults
			};
		bool m_trainingFlag;
		bool m_testFlag;
		bool m_imageFlag;
		bool m_fieldSummaryFlag;
		bool m_classSummaryFlag;
		bool m_groupSummaryFlag;
		double m_conversionFactor;
		bool m_thresholdResultsFlag;
		double m_thresholdPercent;
		bool m_textWindowFlag;
		bool m_diskFileFlag;
		int m_tableType;
		int m_tableUnits;

		 // Implementation
	protected:
		bool TransferDataFromWindow();
		bool TransferDataToWindow();
		void CheckListAreaItems();
		void CheckSummaryItems();
		void HideShowUnitsItems(
				Boolean areaUnitsFlag);
		void SetListAreaItems();
		void SetSummarizeByItems();

				// Generated message map functions
		void OnInitDialog(wxInitDialogEvent& event);
		void OnImage(wxCommandEvent& event);
		void OnSelendokClassCombo(wxCommandEvent& event);
		void OnTraining(wxCommandEvent& event);
		void OnTest(wxCommandEvent& event);
		void OnFieldSummary(wxCommandEvent& event);
		void OnClassSummary(wxCommandEvent& event);
		void OnGroupSummary(wxCommandEvent& event);
		void OnThresholdResults(wxCommandEvent& event);
		void OnSampleUnits(wxCommandEvent& event);
		void OnPercentUnits(wxCommandEvent& event);
		void OnAreaUnits(wxCommandEvent& event);
		void CreateControls();
		DECLARE_EVENT_TABLE()

		double m_saveAreaConversionFactor,
		m_saveThresholdPercent;

		Boolean m_initializedFlag,
		m_projectWindowFlag;

		SInt16 m_localAreaCode,
		m_localSummaryCode,
		m_nonClusterFieldTypeCode;


		wxBoxSizer* bSizer188;
		wxCheckBox* m_checkBox36;
		wxCheckBox* m_checkBox37;
		wxCheckBox* m_checkBox38;
		wxCheckBox* m_checkBox39;
		wxCheckBox* m_checkBox40;
		wxCheckBox* m_checkBox41;
		wxCheckBox* m_checkBox42;
		wxCheckBox* m_checkBox43;
		wxCheckBox* m_checkBox44;

		wxComboBox* m_comboBox33;    
		wxButton* m_button44;
		wxButton* m_button45;
		wxBitmapButton* m_bpButton4;
		wxBitmapButton* m_bpButton5;
		wxTextCtrl* m_linestartctrl;
		wxTextCtrl* m_lineendctrl;
		wxTextCtrl* m_lineintctrl;
		wxTextCtrl* m_colstartctrl;
		wxTextCtrl* m_colendctrl;
		wxTextCtrl* m_colintctrl;
		wxTextCtrl* m_textCtrl106;
		wxTextCtrl* m_textCtrl107;
		wxTextCtrl* m_textCtrl206;

		wxStaticText* m_staticText177;
		wxStaticText* m_staticText60;
		wxStaticText* m_staticText62;
		wxStaticText* m_staticText203;
		wxStaticText* m_staticText204;
		wxStaticText* m_staticText205;
		wxStaticText* m_staticText206;

		wxRadioButton* m_radioBtn19;
		wxRadioButton* m_radioBtn20;
		wxRadioButton* m_radioBtn21;
		wxRadioButton* m_radioBtn22;
		wxRadioButton* m_radioBtn23;
		 
	};
#endif // !defined __WLSTRDLG_H__
