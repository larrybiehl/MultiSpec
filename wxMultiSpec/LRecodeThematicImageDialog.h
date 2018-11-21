// LRecodeThematicImageDialog.h : header file
// CMRecodeThematicDialog dialog
//
// Revised by Larry Biehl on 06/21/2017
//
#if !defined __LRECTDLG_H__
#	define __LRECTDLG_H__             

	#include "LDialog.h"

	//typedef bool BOOL;

	class CMRecodeThematicDialog : public CMDialog
		{
				// Construction
		public:
			CMRecodeThematicDialog(wxWindow* pParent = NULL, wxWindowID id = wxID_ANY, 
			const wxString& title = wxT("Set Thematic Recode Parameters"));

			~CMRecodeThematicDialog(); // standard desctructor  

			Boolean DoDialog(
					RecodeThematicImagePtr  recodeThematicImagePtr);

			void CreateControls();
			bool TransferDataToWindow();
			bool TransferDataFromWindow();

			void OnSelendokThresholdFileCombo(wxCommandEvent& event);

					// Dialog Data
			enum { IDD = IDD_ThematicRecode };
			SInt32	m_thresholdValue;
			SInt32	m_recodedValue;
			int		m_compareParameterCode;
			int		m_thresholdImageSelection;

			DECLARE_EVENT_TABLE();
				
				// Implementation
		protected:    
			virtual void OnInitDialog(wxInitDialogEvent& event);

			RecodeThematicImagePtr  m_recodeThematicImagePtr;
			Handle                  m_thresholdFileInfoHandle;
			Boolean                 m_initializedFlag;

			wxStaticText*           m_staticText339;
			wxStaticText*           m_staticText340;
			wxStaticText*           m_staticText343;
			wxStaticText*           m_staticText344;
			wxStaticText*           m_staticText345;
			wxStaticText*           m_staticText341;
			wxStaticText*           m_staticText342;

			wxTextCtrl*             m_textCtrl173;
			wxTextCtrl*             m_textCtrl175;

			wxComboBox*             m_comboBox71;
			wxComboBox*             m_comboBox70;

			wxButton*               m_button88;
			wxButton*               m_button89;
			wxStdDialogButtonSizer*	m_sdbSizer6;
			wxButton*					m_sdbSizer6OK;
			wxButton*					m_sdbSizer6Cancel;
			 
		};
#endif // !defined __LRECTDLG_H__