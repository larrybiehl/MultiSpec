// LStatisticsDialog.h : header file
//     
// Revised by Larry Biehl on 07/07/2017
// 
#if !defined __LSTATDLG_H__
	#define	__LSTATDLG_H__  

#include "LDialog.h"  
 
//typedef bool BOOL;
typedef wxString CString;

class CMStatisticsDialog : public CMDialog 
{
    // Construction
public:
	CMStatisticsDialog(wxWindow* pParent = NULL, wxWindowID id = IDD_StatisticsDialog, const wxString& title = wxT("Set Project Options")); // standard constructor 

	SInt16 DoDialog(
			SInt16* featurePtr,
			SInt16 totalNumberChannels,
			Handle* trainMaskFileInfoHandle,
			Handle* testMaskFileInfoHandle);

	// Dialog Data
	//{{AFX_DATA(CMStatisticsDialog)

	enum {
		IDD = IDD_StatisticsDialog
		};
	bool m_showFieldNames;
	bool m_showTestFields;
	bool m_showTrainingFields;
	int m_projectCommands;
	bool m_showClassNames;
	bool m_showTrainTestText;
	int m_outlineColorSelection;
	int m_testMaskCombo;
	int m_trainMaskCombo;

    // Implementation
protected:
	void SetOutlineAreaOptions(void);
	bool TransferDataFromWindow();
	bool TransferDataToWindow();

			// Generated message map functions

	void OnInitDialog(wxInitDialogEvent& event);
	void OnSelendokChannels(wxCommandEvent& event);
	void OnSelendokProjectChanges(wxCommandEvent& event);
	void OnDropdownProjectChanges(wxCommandEvent& event);
	void OnDropdownChannels(wxCommandEvent& event);
	void OnStatisticsOptions(wxCommandEvent& event);
	void OnShowClassNames(wxCommandEvent& event);
	void OnShowFieldNames(wxCommandEvent& event);
	void OnTestFields(wxCommandEvent& event);
	void OnTrainingFields(wxCommandEvent& event);
	void OnShowTrainTestLabel(wxCommandEvent& event);
	void OnDropdownTestMaskCOMBO(wxCommandEvent& event);
	void OnSelendokTestMaskCOMBO(wxCommandEvent& event);
	void OnDropdownTrainMaskCOMBO(wxCommandEvent& event);
	void OnSelendokTrainMaskCOMBO(wxCommandEvent& event);
	void CreateControls();
	DECLARE_EVENT_TABLE()

	double								m_minLogDeterminant,
											m_variance;

	Handle								m_testMaskFileInfoHandle,
											m_trainMaskFileInfoHandle;

	SInt16								*m_featurePtr,
											m_localStatCode,
											m_totalNumberChannels;
	 
	UInt16								m_maxNumberTestLayers,
											m_maxNumberTrainLayers;

	Boolean								m_initializedFlag,
											m_keepClassStatsFlag,
											m_useCommonCovarianceInLOOCFlag;
											
	wxBoxSizer*							bSizer_v1;

};
#endif // !defined __LSTATDLG_H__ 
