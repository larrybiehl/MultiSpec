// LFeatureSelectionDialog.h : header file
//   
// Revised by Larry Biehl on 06/21/2017
//
#if !defined __LFSELDLG_H__
#	define	__LFSELDLG_H__             

	#include "LDialog.h"
	//typedef bool BOOL;
	/////////////////////////////////////////////////////////////////////////////
	// CMFeatureSelectionDialog dialog

	class CMFeatureSelectionDialog : public CMDialog 
	{
		 // Construction
	public:
		CMFeatureSelectionDialog(wxWindow* pParent = NULL, wxWindowID id = IDD_FeatureSelection , const wxString& title = wxT("Set Feature Selection Specifications")); // standard constructor     

		~CMFeatureSelectionDialog(); // standard desctructor 

		Boolean DoDialog(
				SeparabilitySpecsPtr separabilitySpecsPtr,
				UInt16* allChanCombinationsPtr);

		// Dialog Data

		enum {
			IDD = IDD_FeatureSelection
			};
			
		int m_separabilityListSelection;
		int m_channelCombinationSelection;
		int m_savedContiguousChannelsPerGroup; // changed from long
		int m_localCombinationsToList;// changed from long
		int m_interClassWeightsSelection;
		int m_channelCombinationSelection_Saved;
		int m_interClassWeightsSelection_Saved;
		int m_classSelection_Saved;
		int m_channelSelection_Saved;
		bool m_textWindowFlag;
		bool m_diskFileFlag;
		bool m_searchFlag;

			// Implementation
	protected:
		bool TransferDataFromWindow();
		bool TransferDataToWindow();
		void HandleChannelCombinationsMenu (
					UInt16 channelCombinationsMenuItemNumber);

				// Generated message map functions
		void OnInitDialog(wxInitDialogEvent& event);
		void OnSelendokChannelCombo(wxCommandEvent& event);
		void OnSelendokDistanceMeasureCombo(wxCommandEvent& event);
		void OnSelendokNumberChannelsCombo(wxCommandEvent& event);
		void OnSelendokNumberChannelsComboDropDown(wxCommandEvent& event);
		void OnChangeChannelsPerGroup(wxCommandEvent& event);
		SInt16 CheckValues(int controlID, SInt16 minValue, SInt16 maxValue, SInt32* newValue);
		void OnStepProcedure(wxCommandEvent& event);
		void OnChangeNumberBestToList(wxCommandEvent& event);
		void OnSelendokClassCombo(wxCommandEvent& event);
		void OnFeatureTransformation(wxCommandEvent& event);
		void OnListOptions(wxCommandEvent& event);
		void OnSelendokClassPairWeightsCombo(wxCommandEvent& event);
		void CreateControls();  
		void OnHelpButton(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
					  
		SeparabilitySpecsPtr		m_separabilitySpecsPtr;
		SInt16						*m_localClassPairWeightsListPtr;
		UInt16						*m_allChanCombinationsPtr,
										*m_localChannelCombinationsPtr;

		SInt32						m_maxContiguousChannelsPerGroup,
										m_numberClassCombinations;

		SInt16						m_localDefaultClassPairWeight,
										m_separabilityDistance;

		UInt16						m_localNumberChannelGroupCombinations,
										m_savedNumberChannelGroupCombinations;

		wxBoxSizer* bSizer211;    
		wxStaticText* m_staticText213;
		wxStaticText* m_staticText214;
		wxStaticText* m_staticText215;
		wxStaticText* m_staticText216;
		wxStaticText* m_staticText217;
		wxStaticText* m_staticText218;
		wxStaticText* m_staticText219;
		wxStaticText* m_staticText221;
		wxStaticText* m_staticText222;
		wxStaticText* m_staticText223;
		wxStaticText* m_staticText224;
		wxStaticText* m_staticText225;
		wxStaticText* m_staticText226;
		wxStaticText* m_staticText228;
		wxStaticText* m_staticText229;
		wxComboBox* m_comboBox34;
		wxComboBox* m_comboBox35;
		wxComboBox* m_comboBox36;
		wxComboBox* m_comboBox38;
		wxComboBox* m_comboBox39;
		wxComboBox* m_comboBox40;
		wxCheckBox* m_checkBox47;
		wxCheckBox* m_checkBox48;
		wxCheckBox* m_checkBox49;
		wxCheckBox* m_checkBox50;
		wxTextCtrl* m_textCtrl120;
		wxTextCtrl* m_textCtrl121;
		wxButton* m_button46;
		wxButton* m_button47;
		wxButton* m_button48;

		wxString m_channelgroupString;
		wxString m_numBestString;

	};

	/////////////////////////////////////////////////////////////////////////////
	// CMFeatureSelectionListDialog dialog

	class CMFeatureSelectionListDialog : public CMDialog 
	{
		 // Construction
	public:
		 CMFeatureSelectionListDialog(wxWindow* pParent = NULL, wxWindowID id = IDD_ClassifyDialog, const wxString& title = wxT("Set Feature Selection List Options")); // standard constructor

		 Boolean DoDialog(
					SeparabilitySpecsPtr separabilitySpecsPtr,
					SInt32* combinationsToListPtr);

		 // Dialog Data

		 enum {
			  IDD = IDD_FeatureSelectionListOptions
		 };
		 int m_sortChannelCombinations;
		 bool m_thresholdedGroupTableFlag;
		 bool m_thresholdedClassPairTableFlag;
		 double m_localDistancesLessThanToGroup;
		 double m_localDistancesLessThanToList;
		 bool m_listClassPairDistancesFlag;
		 double m_localMinDistanceRangeToList;
		 double m_localMaxDistanceRangeToList;
		 int m_localCombinationsToList;  // changed from long
		 bool m_separabilityTableFlag;
		 
		 wxString m_mindistrangeString;
		 wxString m_maxdistrangeString;
		 wxString m_numberbestString;

			// Implementation
	protected:
		bool TransferDataFromWindow();
		bool TransferDataToWindow();
		
		void OnInitDialog(wxInitDialogEvent& event);
		void CreateControls();
		void OnHelpButton(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()

		SeparabilitySpecsPtr m_separabilitySpecsPtr;

		SInt32* m_combinationsToListPtr;
		wxBoxSizer* bSizer226;
		wxStaticText* m_staticText230;
		wxStaticText* m_staticText231;
		wxStaticText* m_staticText232;
		wxStaticText* m_staticText233;
		wxStaticText* m_staticText234;
		wxStaticText* m_staticText235;
		wxStaticText* m_staticText236;
		wxStaticText* m_staticText237;
		wxStaticText* m_staticText238;

		wxTextCtrl* m_textCtrl125;
		wxTextCtrl* m_textCtrl126;
		wxTextCtrl* m_textCtrl123;
		wxTextCtrl* m_textCtrl122;
		wxTextCtrl* m_textCtrl124;

		wxCheckBox* m_checkBox52;
		wxCheckBox* m_checkBox51;
		wxCheckBox* m_checkBox53;
		wxCheckBox* m_checkBox54;

		wxButton* m_button49;
		wxButton* m_button50;

		wxRadioButton* m_radioBtn24;
		wxRadioButton* m_radioBtn25;
		wxRadioButton* m_radioBtn26; 
		 
	};
#endif // !defined __LFSELDLG_H__
