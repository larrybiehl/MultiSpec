// LOpenFileDialog.h
//
// Revised by Larry Biehl on 08/02/2017
//
#ifndef __FILEDLG__
#	define __FILEDLG__

#include "SMultiSpec.h"

#include "wx/filedlg.h"
#include "wx/string.h"
#include "wx/sizer.h"
#include "wx/combobox.h"
#include <wx/combo.h>
#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/arrstr.h>
#include <wx/stattext.h>
#include <wx/gbsizer.h>
#include "wx/wx.h"

class MyExtraPanel : public wxPanel
{
	DECLARE_CLASS(MyExtraPanel)
public:
	int     m_imageType;
	int     m_linkOption;
	wxWindow* parent;
    
	enum {
		ID_UploadButton
		};

	MyExtraPanel (wxWindow *parent);
	void OnCheckBox (wxCommandEvent& event) 
		{
		m_linkToActiveImageFlag = event.IsChecked();
		
		if (m_linkToActiveImageFlag)
			SetImageLinkToTrue();
		else		// !m_linkToActiveImageFlag
			SetImageLinkToFalse();
			
		}

	int getlinktoactiveimagevalue()
		{
		return (int)m_linkToActiveImageFlag;
		}

	int getimagetype()
		{
		return m_imageType;
		}
    
	void OnSelendokImageType (wxCommandEvent& event);
	void OnSelendokLinkOption (wxCommandEvent& event);
	void OnUploadFile (wxCommandEvent& event);

	bool CreateControls (void);
	void OnInitDialog (void);
	void OnFileNameChange (void);
	void SetImageLinkToFalse (void);
	void SetImageLinkToTrue (void);
	~MyExtraPanel();
    
private:
	bool    m_linkToActiveImageFlag;
	bool    m_showLinkPopupMenuFlag;
	bool    m_initialLinkSelectedFilesFlag;
	int     m_linkOptionSelectionDataCode;
	int     m_numberImageFiles;

	int     m_userSetImageType;
	int     m_getSelectStatus;
	int     m_selectedFileCount;

	wxComboBox*			m_list;
	wxComboBox*			m_link;
	wxCheckBox*			m_cb;
	wxButton*			uploadbutton;

	wxStaticText*		m_staticText2;
	wxStaticText*		m_staticText3;
	wxStaticText*		m_staticText4;
	wxStaticText*		m_staticText5;
	wxArrayString		strings;

};

class CMOpenFileDialog
{
public:
	//CMOpenFileDialog(wxWindow* pParent = NULL);	// standard constructor 
	CMOpenFileDialog (wxWindow* pParent);
	bool DoDialog (int stringIndex, long style);
	void OnInitDialog (wxInitDialogEvent& event);
	//~CMOpenFileDialog();		// standard desctructor
	wxString					GetPath() const{return m_path;}
	wxArrayString			GetPaths();
	wxString					m_prompt;
	int						m_imageType;
	wxString					m_path;
	wxArrayString			m_paths;
	//Boolean					DoModal();
	//virtual int &getFileStatusValue() {return gGetFileS;}
	//void setFilestatusValue(int getFile) {CMOpenFileDialog::gGetFileS = &getFile; printf("SetTest:%d\n", *gGetFileS); }
    
protected:   
   void SetImageLinkToTrue (void);
   void SetImageLinkToFalse (void);
   wxString					m_fileName;
   wxWindow*				m_parent;
   wxFileDialog*			m_wxFileDialog;
   
private:
	Boolean m_showLinkPopupMenuFlag;   
};
#endif

