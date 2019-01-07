// LNewClassFieldDialog.h : header file
//       
#if !defined __LNEWCLS_H__
#	define __LNEWCLS_H__  

	#include "LDialog.h"

	typedef wxString CString;
	//typedef bool BOOL;
	/////////////////////////////////////////////////////////////////////////////
	// WEditClassFieldDlg dialog

	class CMNewClassFieldDlg : public CMDialog 
	{
			// Construction
	public:
		CMNewClassFieldDlg(wxWindow* pParent = NULL, wxWindowID id = IDD_ClassField, const wxString& title = wxT("Define Class and/or Field Description")); // standard constructor

			// Dialog Data
	enum {
		IDD = IDD_ClassField
		};
		
	int m_classList;
	CString m_className;
	CString m_fieldName;
	int m_fieldType;

	Boolean DoDialog(
			Boolean newClassOnlyFlag,
			char* classNamePtr,
			char* fieldNamePtr,
			SInt16* fieldTypePtr);

			// Implementation
	protected:
		bool TransferDataToWindow();
		bool TransferDataFromWindow();

		// Generated message map functions
		void OnInitDialog(wxInitDialogEvent& event);
		void OnSelendokClassList(wxCommandEvent& event);
		void OnFieldType(wxCommandEvent& event);
		void CheckLength(wxCommandEvent& event);
		void OnCharHook (wxKeyEvent& event);
		void CreateControls();
		DECLARE_EVENT_TABLE()

		char* m_classNameCStringPtr;
		char* m_fieldNameCStringPtr;
		char m_classNamebuf[1024];
		char m_fieldNamebuf[1024];

		Boolean m_initializedFlag;
		Boolean m_newClassOnlyFlag;
		char* m_classNamePtr;
		char* m_fieldNamePtr;
		SInt16* m_fieldTypePtr;

		SInt64 m_numberSelectionPixels;

		wxBoxSizer* bSizer285;
		wxStaticText* m_staticText265;
		wxStaticText* m_staticText266;
		wxStaticText* m_staticText267;
		wxStaticText* m_staticText268;
		wxStaticText* m_staticText269;
		wxStaticText* m_staticText270;
		wxComboBox* m_comboBox56;
		wxTextCtrl* m_textCtrl102;
		wxTextCtrl* m_textCtrl103;
		wxRadioButton* m_radioBtn27;
		wxRadioButton* m_radioBtn28;
		wxButton* m_button84;
		wxButton* m_button85;
					
	};
#endif	// !defined __LNEWCLS_H__
