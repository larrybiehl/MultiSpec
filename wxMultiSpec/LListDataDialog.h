// LListDataDialog.h : header file
//     
// Revised by Larry Biehl on 09/28/2018
//
#if !defined __LLSTDDLG_H__
#	define	__LLSTDDLG_H__

	#include "SMultiSpec.h"
	#include "LDialog.h"
	#include "wx/wx.h"

	/////////////////////////////////////////////////////////////////////////////
	// CMListDataDialog dialog

	class CMListDataDialog : public CMDialog 
	{
		 // Construction
	public:
		CMListDataDialog();
		CMListDataDialog(wxWindow* pParent, wxWindowID id=wxID_ANY, const wxString& title= wxT("Set List Data Specifications")); // standard constructor      

		~CMListDataDialog(); // standard desctructor 

		SInt16 DoDialog(void);

				// Dialog Data

		enum {
			IDD = IDD_ListData
			};
		bool m_areaFlag;
		bool m_classFlag;
		bool m_diskFileFlag;
		bool m_graphDataFlag;
		bool m_includeClassFieldFlag;
		bool m_includeLineColumnFlag;
		bool m_includeLatLongFlag;
		bool m_latLongPossibleFlag;
		bool m_savedGraphDataFlag;
		bool m_textWindowFlag;
		bool m_trainingFlag;
		bool m_testFlag;
		long m_numberDecimalPlaces;
		int m_listDataFormatCode;
		int m_columnsLinesFormat;   // not used now
		//wxString m_LineStartString;
		//wxString m_LineEndString;
		//wxString m_LineIntervalString;
		//wxString m_ColumnStartString;
		//wxString m_ColumnEndString;
		//wxString m_ColumnIntervalString;
		wxString m_numdecString;

				// Implementation
	protected:
		wxBoxSizer* bSizer155;
		wxComboBox* m_comboBox27;
		wxComboBox*  m_comboBox28;
		wxComboBox* m_comboBox29;
		wxComboBox*  m_comboBox30;

		wxCheckBox* m_checkBox50;
		wxCheckBox* m_checkBox51;
		wxCheckBox* m_checkBox52;
		wxCheckBox* m_checkBox54;
		wxCheckBox* m_checkBox55;
		wxCheckBox* m_checkBox56;
		wxCheckBox* m_checkBox57;
		wxCheckBox* m_checkBox58;
		wxCheckBox* m_checkBox59;
		wxCheckBox* m_checkBox60;

		wxBitmapButton* m_bpButton4;
		wxBitmapButton* m_bpButton5;

		wxStaticText* m_staticText205;
		wxStaticText* m_staticText206;
		wxStaticText* m_staticText207;
		wxStaticText* m_staticText204;
		//wxStaticText* m_staticText65;
		//wxStaticText* m_staticText66;
		//wxStaticText* m_staticText63;
		//wxStaticText* m_staticText64;
		//wxStaticText* m_staticText60;
		//wxStaticText* m_staticText62;
		wxStaticText* m_staticText216;

		//wxTextCtrl* m_linestartctrl;
		//wxTextCtrl* m_lineendctrl;
		//wxTextCtrl* m_lineintctrl;
		//wxTextCtrl* m_colstartctrl;
		//wxTextCtrl* m_colendctrl;
		//wxTextCtrl* m_colintctrl;
		wxTextCtrl* m_textCtrl115;
		wxTextCtrl* m_numberdec;

		wxButton* m_button46;
		wxButton* m_button47;

		wxStaticBox* m_staticBoxArea;
		void CreateControls();
		bool TransferDataFromWindow();
		bool TransferDataToWindow();

		void CheckClassItems(
				Boolean listClassDataFlag);

		void CheckOKButton(void);

		Boolean m_initializedFlag;

				// Generated message map functions
	
		void OnInitDialog (wxInitDialogEvent& event);
		void OnSelendokChannelCombo (wxCommandEvent& event);
		void OnSelendokClassCombo (wxCommandEvent& event);
		void OnArea (wxCommandEvent& event);
		void OnClasses (wxCommandEvent& event);
		void OnGraphData (wxCommandEvent& event);
		void OnSelAreaUpdate (wxUpdateUIEvent& event);
		void OnChangeDecimalPlaces (wxCommandEvent& event);
		DECLARE_EVENT_TABLE()

		Boolean m_allChannelsAtOnceFlag;
		 
	public:
		 void OnCbnSelendokListchannelsformatcombo(wxCommandEvent& event);
	};

#endif // !defined __LLSTDDLG_H__
