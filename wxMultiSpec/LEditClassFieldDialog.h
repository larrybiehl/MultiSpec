// LEditClassFieldDialog.h : header file
//     
// Revised by Larry Biehl on 07/07/2017
// 
#if !defined __LEDITCLASSFIELDDLG_H__
	#define	__LEDITCLASSFIELDDLG_H__  

#include "LDialog.h"  
 
typedef wxString CString;

/////////////////////////////////////////////////////////////////////////////
// CMEditClassFieldDlg dialog

class CMEditClassFieldDlg : public CMDialog 
{
    // Construction
public:
	CMEditClassFieldDlg(wxWindow* pParent = NULL, wxWindowID id = IDD_ClassifyDialog, const wxString& title = wxT("Set Project Options")); // standard constructor

			// Dialog Data

	enum {
		IDD = IDD_ClassField
		};
	CString m_className;
	CString m_fieldName;
	char m_classNamebuf[1024];
	char m_fieldNamebuf[1024];

	int m_fieldType;

	Boolean DoDialog (
			SInt16 classFieldCode,
			SInt16 currentClass,
			SInt16 currentField,
			char* titleStringPtr);

    // Implementation
protected:	 
			// Generated message map functions
	void OnInitDialog(wxInitDialogEvent& event);
	DECLARE_EVENT_TABLE()
	 
	void CreateControls();
	void CheckLength(wxCommandEvent& event);
	bool TransferDataFromWindow();
	bool TransferDataToWindow();
	 
	char				*m_classNameCStringPtr,
						*m_fieldNameCStringPtr,
						*m_titleStringPtr;
						
	SInt16*			m_fieldTypePtr;

	SInt16			m_classFieldCode,
						m_currentField,
						m_classStorage;
						
	Boolean			m_initializedFlag;
    
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
#endif // !defined __LEDITCLASSFIELDDLG_H__ 
