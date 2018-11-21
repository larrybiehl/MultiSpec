// LEnhanceStatisticsDialog.h
//
// Revised by Larry Biehl on 11/07/2018
//

#if !defined __LESTADLG_H__
	#define	__LESTADLG_H__

	#include "SMultiSpec.h"

	#include "LDialog.h"

	#include "wx/wx.h"
	#include "wx/bmpcbox.h"

	//typedef bool BOOL;

	/////////////////////////////////////////////////////////////////////////////
	// CMEnhanceStatisticsDialog dialog

	class CMEnhanceStatisticsDialog : public CMDialog
	{
			// Construction
	public:
		CMEnhanceStatisticsDialog(wxWindow* parent, wxWindowID id = wxID_ANY, 
								const wxString& title = wxT("Set Statistics Enhancement Specifications"), 
								const wxPoint& pos = wxDefaultPosition, 
								const wxSize& size = /*wxDefaultSize*/wxSize( 200,200 ), 
								long style = wxDEFAULT_DIALOG_STYLE);   // standard constructor

		~CMEnhanceStatisticsDialog(void);

		  

		SInt16  DoDialog( StatEnhanceSpecsPtr   statEnhanceSpecsPtr);

		void CreateControls();
		  
		enum { IDD = IDD_EnhanceStatistics };
		double	m_logLikeStopPercent;
		SInt32	m_iterationStopLength;
		SInt32	m_iterationMax;
		double	m_labelWeight;
		bool	m_useEnhancedStatisticsFlag;
		bool	m_weightLabeledFlag;
		int         m_weightsSelection;
		double	m_softChiChiThreshold;  
		double	m_hardChiChiThreshold;
		double	m_softPercentThreshold;
		double	m_hardPercentThreshold;
		int         m_softThresholdCode;
		int         m_hardThresholdCode;
		int         saveSoftThresholdSelection;

			// Implementation
	protected:
	 
		void             OnInitDialog(wxInitDialogEvent& event); 
		void             OnWeightLabeledSamples(wxCommandEvent& event);
		void             OnUseEnhancedStats(wxCommandEvent& event);
		void             CheckStatColumnEnd(wxCommandEvent& event);
		void             CheckStatColumnStart(wxCommandEvent& event);
		void             CheckStatLineEnd(wxCommandEvent& event);
		void             CheckStatLineStart(wxCommandEvent& event);
		void             ToStatEntireImage(wxCommandEvent& event);
		void             ToStatSelectedImage(wxCommandEvent& event);
		void             CheckStatLineInterval(wxCommandEvent& event);
		void             CheckStatColumnInterval(wxCommandEvent& event);
		void             OnSelendokStatClassCombo(wxCommandEvent& event);
		void             OnSelchangeHardThresholdCombo(wxCommandEvent& event);
		void             OnSelchangeSoftThresholdCombo(wxCommandEvent& event);
		void             OnChangeHardPercentThreshold(wxCommandEvent& event);
		void             OnChangeSoftPercentThreshold(wxCommandEvent& event);
		void             OnChangeHardChiChiThreshold(wxCommandEvent& event);
		void             OnChangeSoftChiChiThreshold(wxCommandEvent& event);
		void             OnSelendokClassWeightsCombo(wxCommandEvent& event);
	 
		bool            TransferDataFromWindow();
		bool            TransferDataToWindow();

		DECLARE_EVENT_TABLE();
							
		float*                      m_classWeightsPtr;
		StatEnhanceSpecsPtr         m_statEnhanceSpecsPtr;
		SInt16                      m_classWeightSet;							
		Boolean                     m_initializedFlag,
											m_updatingThresholdItemsFlag;

		wxStaticText*       m_staticText249;
		wxStaticText*       m_staticText250;
		wxStaticText*       m_staticText307;
		wxStaticText*       m_staticText308;
		wxStaticText*       m_staticText317;
		wxStaticText*       m_textCtrl170;
		wxStaticText*       m_textCtrl171;
		//wxButton*           m_bpButton4;
		//wxButton*           m_bpButton5;
		wxTextCtrl*         m_textCtrl191;
		wxTextCtrl*         m_textCtrl192;
		wxTextCtrl*         m_textCtrl193;
		wxTextCtrl*         m_textCtrl194;
		wxTextCtrl*         m_textCtrl195;
		wxTextCtrl*         m_textCtrl196;
		wxStaticText*       m_staticText318;
		wxStaticText*       m_staticText319;
		wxStaticText*       m_staticText263;
		wxStaticText*       m_staticText280;
		wxStaticText*       m_staticText281;
		wxTextCtrl*         m_textCtrl181;
		wxTextCtrl*         m_textCtrl182;

		wxCheckBox*         m_checkBox58;
		wxStaticText*       m_staticText326;
		wxComboBox*         m_comboBox35;
		wxStaticText*       m_staticText327;
		wxComboBox*         m_comboBox36;
		wxComboBox*         m_comboBox23;
		wxComboBox*         m_comboBox24;
		wxCheckBox*         m_checkBox59;
		wxTextCtrl*         m_textCtrl204;
		wxStaticText*       m_staticText3071;
		wxStaticText*       m_staticText3081;
		wxStaticText*       m_staticText30711;
		wxComboBox*         m_bcomboBox1;
		wxTextCtrl*         m_textCtrl184;
		wxStaticText*       m_staticText30712;
		wxTextCtrl*         m_textCtrl185;
		wxStaticText*       m_staticText307121;
		wxComboBox*         m_comboBox32;
		wxTextCtrl*         m_textCtrl1851;
		wxStaticText*       m_staticText3071211;
		wxTextCtrl*         m_textCtrl18511;
		wxStaticText*       m_staticText278;
		wxStaticText*       m_staticText279;
		wxTextCtrl*         m_textCtrl180;
		wxStaticText*       m_staticText188;

		wxBoxSizer*						bSizer119;
		wxFlexGridSizer*				fgSizer15;

		wxStdDialogButtonSizer*		m_sdbSizer6;
		wxButton*						m_sdbSizer6OK;
		wxButton*						m_sdbSizer6Cancel;
		wxString					m_softChiThresString;
		wxString m_softpercentThresString;
		wxString m_hardchiThresString;
		wxString m_hardpercentThresString;
		wxString m_weightString;
		wxString m_maxNumberString;
		wxString m_iterlengthString;        
		wxString m_loglikeString;
	};
#endif
