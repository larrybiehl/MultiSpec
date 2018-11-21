// LEvaluateCovariancesDialog.h : header file
//  
// Revised by Larry Biehl on 06/21/2017
//    
#if !defined __LCOVEDLG_H__
	#define	__LCOVEDLG_H__             
	     
	#include "LDialog.h"

	//typedef bool BOOL;

	/////////////////////////////////////////////////////////////////////////////
	// CMEvalCovarianceDialog dialog

	class CMEvalCovarianceDialog : public CMDialog
	{
	public:
		CMEvalCovarianceDialog(wxWindow* pParent = NULL, wxWindowID id = wxID_ANY, 
		const wxString& title = wxT("Check Covariances"));   // standard constructor

		~CMEvalCovarianceDialog();		// standard desctructor
		Boolean DoDialog(
		EvaluateCovarianceSpecsPtr  evaluateCovarianceSpecsPtr);

		bool TransferDataToWindow();
		bool TransferDataFromWindow();

		// Dialog Data    
		enum { IDD = IDD_EvalCovariances };
		bool	m_listOriginalMatrixFlag;
		bool	m_listInvertedMatrixFlag;
		bool	m_listInvertedInvertedMatrixFlag;
		bool	m_listOriginalXInvertedMatrixFlag;
		bool	m_featureTransformationFlag; 
		 
			// Implementation
	protected:
		void CreateControls();
		virtual void  OnInitDialog(wxInitDialogEvent& event);

		void OnTransformation(wxCommandEvent& event);
		void OnSelendokChannelCombo(wxCommandEvent& event);
		void OnSelendokClassCombo(wxCommandEvent& event);

		EvaluateCovarianceSpecsPtr  m_evaluateCovarianceSpecsPtr;
		Boolean		  	m_featureTransformAllowedFlag,
										  m_initializedFlag;
		UInt16			m_numberEigenvectors;

		DECLARE_EVENT_TABLE();

		/*Dialog items*/
		wxBoxSizer* bSizer107;
		wxBoxSizer* bSizer108;
		wxBoxSizer* bSizer175;
		wxCheckBox* m_checkBox14;
		wxCheckBox* m_checkBox15;
		wxCheckBox* m_checkBox16;
		wxCheckBox* m_checkBox17;
		wxCheckBox* m_checkBox18;

		wxStaticText* m_staticText190;
		wxComboBox*   m_comboBox26;
		wxComboBox*   m_comboBox27;
		wxStaticText* m_staticText191;
		wxButton* m_button20;
		wxButton* m_button21;
		 
	};
#endif // !defined __LCOVEDLG_H__