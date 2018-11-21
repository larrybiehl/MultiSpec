// LFeatureSelectionDialog.cpp : implementation file
//   
// Revised by Larry Biehl on 11/15/2018
//
#include "wx/cshelp.h"
#include "wx/aboutdlg.h"

#include "SMultiSpec.h"

#include "LChannelsDialog.h"
#include "LClassPairWeightsDialog.h"
#include "LFeatureSelectionDialog.h"
#include "LMultiSpec.h"

extern void InitializeDialogFeatureParameters(
   Boolean featureTransformationFlag,
   UInt16 localActiveNumberFeatures,
   UInt16 numberFeatures,
   UInt16 numberTransformFeatures,
   UInt16* localFeaturesPtr,
   UInt16* localTransformFeaturesPtr,
   UInt16* localNumberFeaturesPtr,
   UInt16* localNumberTransformFeaturesPtr,
   UInt16** localActiveFeaturesPtrPtr);

extern void SeparabilityDialogCheckFeatureTransformation(
   DialogPtr dialogPtr,
   Boolean* featureTransformationFlagPtr,
   UInt16* localActiveNumberFeaturesPtr,
   UInt16** localActiveFeaturePtrPtr,
   Boolean featureTransformAllowedFlag,
   SInt16 featureTransformItem,
   SInt16 channelFeatureItem,
   SInt16* channelSelectionPtr,
   SInt16 localNumberFeatures,
   UInt16* localFeaturesPtr,
   SInt16 localNumberTransformedFeatures,
   UInt16* localTransformFeaturesPtr);

extern Boolean SeparabilityDialogGetFeatureTransformAllowedFlag(
   SInt16 separabilityDistance,
   UInt16 numberEigenvectors);

extern void SeparabilityDialogInitialize(
   DialogPtr dialogPtr,
   SeparabilitySpecsPtr separabilitySpecsPtr,
   UInt16* localFeaturesPtr,
   UInt16* transformFeaturesPtr,
   UInt16* localClassPtr,
   unsigned char* localSymbolPtr,
   UInt16* localChannelCombinationsPtr,
   SInt16** localClassPairWeightsListPtr,
   SInt16* separabilityDistancePtr,
   UInt16* numberEigenvectorsPtr,
   Boolean* featureTransformAllowedFlagPtr,
   Boolean* featureTransformationFlagPtr,
   SInt16* channelSelectionPtr,
   UInt16* localNumberFeaturesPtr,
   SInt16* channelCombinationSelectionPtr,
   UInt16* numberChannelGroupCombinationsPtr,
   UInt16* savedNumberChannelGroupCombinationsPtr,
   SInt32* savedContiguousChannelsPerGroupPtr,
   Boolean* savedSearchFlagPtr,
   SInt32* maxContiguousChannelsPerGroupPtr,
   SInt32* localCombinationsToListPtr,
   SInt16* classSelectionPtr,
   UInt32* localNumberClassesPtr,
   SInt32* numberClassCombinationsPtr,
   SInt16* interClassWeightsSelectionPtr,
   SInt16* defaultClassPairWeightPtr,
   SInt16* symbolSelectionPtr,
   Boolean* textWindowFlagPtr,
   Boolean* diskFileFlagPtr);

extern void SeparabilityDialogOK(
   SeparabilitySpecsPtr separabilitySpecsPtr,
   SInt16 separabilityDistance,
   Boolean featureTransformationFlag,
   SInt16 channelSelection,
   UInt16 localNumberFeatures,
   UInt16* localFeaturesPtr,
   SInt16 channelCombinationSelection,
   UInt16 localNumberChannelGroupCombinations,
   UInt16* localChannelCombinationsPtr,
   SInt32 contiguousChannelsPerGroup,
   Boolean searchFlag,
   SInt32 numberCombinationsToList,
   SInt16 classSelection,
   UInt32 localNumberClasses,
   UInt16* localClassPtr,
   SInt16 interClassWeightsSelection,
   SInt16* localClassPairWeightsListPtr,
   SInt16 localDefaultClassPairWeight,
   SInt16 symbolSelection,
   unsigned char* localSymbolsPtr,
   Boolean outputTextWindowFlag,
   Boolean diskFileFlag);

extern void SeparabilityDialogUpdateChannelCombinationItems(
   DialogPtr dialogPtr,
   SeparabilitySpecsPtr separabilitySpecsPtr,
   Boolean updateChannelCombinationVectorFlag,
   UInt16 numberFeatures,
   UInt16* allChanCombinationsPtr,
   SInt32 contiguousChannelsPerGroup,
   Boolean searchFlag,
   SInt16 channelCombinationSelection,
   UInt16* channelCombinationsPtr,
   UInt16* numberChannelGroupCombinationsPtr,
   UInt16* savedNumberChannelGroupCombinationsPtr,
   SInt32* combinationsToListPtr,
   SInt32* maxContiguousChannelsPerGroupPtr);

extern void SeparabilityDialogUpdateChannelFeatureGroupText(
   DialogPtr dialogPtr,
   SInt16 contiguousChannelsPerGroup,
   Boolean featureTransformationFlag);

extern void SeparabilityDialogUpdateNumberClassCombintations(
   DialogPtr dialogPtr,
   SInt16 classSelection,
   UInt32* numberClassesPtr,
   UInt16* classPtr);

extern void SeparabilityListDialog(
   SInt32* localCombinationsToListPtr);

extern void SeparabilityListDialogInitialize(
   DialogPtr dialogPtr,
   SeparabilitySpecsPtr separabilitySpecsPtr,
   SInt32 combinationsToList,
   Boolean* separabilityTableFlagPtr,
   SInt32* localCombinationsToListPtr,
   double* localMinDistanceRangeToListPtr,
   double* localMaxDistanceRangeToListPtr,
   SInt16* sortChannelCombinationsPtr,
   Boolean* thresholdedClassPairTableFlagPtr,
   double* localDistancesLessThanToListPtr,
   Boolean* thresholdedGroupTableFlagPtr,
   double* localDistancesLessThanToGroupPtr,
   Boolean* listClassPairDistancesFlagPtr);

extern void SeparabilityListDialogOK(
   SeparabilitySpecsPtr separabilitySpecsPtr,
   SInt32* combinationsToListPtr,
   Boolean separabilityTableFlag,
   SInt32 localCombinationsToList,
   double localMinDistanceRangeToList,
   double localMaxDistanceRangeToList,
   Boolean sortByAverageDistanceFlag,
   Boolean sortByMinimumDistanceFlag,
   Boolean sortByOrderComputedFlag,
   Boolean thresholdedClassPairTableFlag,
   double localDistancesLessThanToList,
   Boolean thresholdedGroupTableFlag,
   double localDistancesLessThanToGroup,
   Boolean listClassPairDistancesFlag);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionDialog dialog

CMFeatureSelectionDialog::CMFeatureSelectionDialog(wxWindow* parent, wxWindowID id, const wxString& title)
: CMDialog(CMFeatureSelectionDialog::IDD, parent, title) {
   //{{AFX_DATA_INIT(CMFeatureSelectionDialog) 
   m_separabilityDistance = -1;
   m_savedContiguousChannelsPerGroup = 1;
   m_localCombinationsToList = 1;
   m_textWindowFlag = FALSE;
   m_diskFileFlag = FALSE;
   m_channelCombinationSelection = -1;
   m_interClassWeightsSelection = -1;
   m_searchFlag = FALSE;
   //}}AFX_DATA_INIT 

   m_separabilitySpecsPtr = NULL;
   m_localChannelCombinationsPtr = NULL;
   m_maxContiguousChannelsPerGroup = 1;
   m_localClassPairWeightsListPtr = NULL;
   m_localDefaultClassPairWeight = 0;

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
      m_initializedFlag = GetDialogLocalVectors(
      &m_localFeaturesPtr,
      &m_localTransformFeaturesPtr,
      &m_classListPtr,
      NULL,
      NULL,
      &m_localSymbolsPtr,
      &m_localChannelCombinationsPtr,
      &m_localClassPairWeightsListPtr);
   CreateControls();
   //SetSizerAndFit(bSizer211);

}

CMFeatureSelectionDialog::~CMFeatureSelectionDialog(void) {
   ReleaseDialogLocalVectors(m_localFeaturesPtr,
      m_localTransformFeaturesPtr,
      m_classListPtr,
      NULL,
      NULL,
      m_localSymbolsPtr,
      m_localChannelCombinationsPtr,
      m_localClassPairWeightsListPtr);

} // end "~CMFeatureSelectionDialog"

//void CMFeatureSelectionDialog::DoDataExchange(CDataExchange* pDX) {
//   CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CMFeatureSelectionDialog) 
//   DDX_CBIndex(pDX, IDC_DistanceMeasureCombo, m_separabilityListSelection);
//   DDX_CBIndex(pDX, IDC_NumberChannelsCombo, m_channelCombinationSelection);
//   DDX_Text(pDX, IDC_ChannelsPerGroup, m_savedContiguousChannelsPerGroup);
//   DDV_MinMaxLong(pDX, m_savedContiguousChannelsPerGroup, 1, m_maxContiguousChannelsPerGroup);
//   DDX_Text(pDX, IDC_NumberBestToList, m_localCombinationsToList);
//   DDV_MinMaxLong(pDX, m_localCombinationsToList, 1, SInt16_MAX);
//   DDX_CBIndex(pDX, IDC_ClassPairWeightsCombo, m_interClassWeightsSelection);
//   DDX_Check(pDX, IDC_TextWindow, m_textWindowFlag);
//   DDX_Check(pDX, IDC_DiskFile, m_diskFileFlag);
//   DDX_Check(pDX, IDC_StepProcedure, m_searchFlag);
//   DDX_Check(pDX, IDC_FeatureTransformation, m_featureTransformationFlag);
//   DDX_CBIndex(pDX, IDC_ChannelCombo, m_channelSelection);
//   DDX_CBIndex(pDX, IDC_ClassCombo, m_classSelection);
//   DDX_CBIndex(pDX, IDC_SymbolCombo, m_symbolSelection);
//}}AFX_DATA_MAP
//}

bool CMFeatureSelectionDialog::TransferDataToWindow() {
   wxComboBox* distancemeas = (wxComboBox*) FindWindow(IDC_DistanceMeasureCombo);
   wxComboBox* numchannel = (wxComboBox*) FindWindow(IDC_NumberChannelsCombo);
   wxComboBox* classpairweight = (wxComboBox*) FindWindow(IDC_ClassPairWeightsCombo);
   wxComboBox* channel = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   wxComboBox* classcombo = (wxComboBox*) FindWindow(IDC_ClassCombo);
   wxComboBox* symbolcombo = (wxComboBox*) FindWindow(IDC_SymbolCombo);

   wxCheckBox* textwindow = (wxCheckBox*) FindWindow(IDC_TextWindow);
   wxCheckBox* disk = (wxCheckBox*) FindWindow(IDC_DiskFile);
   wxCheckBox* step = (wxCheckBox*) FindWindow(IDC_StepProcedure);
   wxCheckBox* feature = (wxCheckBox*) FindWindow(IDC_FeatureTransformation);

   wxTextCtrl* channelpergroup = (wxTextCtrl*) FindWindow(IDC_ChannelsPerGroup);
   wxTextCtrl* numberbest = (wxTextCtrl*) FindWindow(IDC_NumberBestToList);

   distancemeas->SetSelection(m_separabilityListSelection);
   numchannel->SetSelection(m_channelCombinationSelection);
   classpairweight->SetSelection(m_interClassWeightsSelection);
   channel->SetSelection(m_channelSelection);
   classcombo->SetSelection(m_classSelection);
   symbolcombo->SetSelection(m_symbolSelection);

   textwindow->SetValue(m_textWindowFlag);
   disk->SetValue(m_diskFileFlag);
   step->SetValue(m_searchFlag);
   feature->SetValue(m_featureTransformationFlag);

   channelpergroup->ChangeValue(wxString::Format(wxT("%i"), m_savedContiguousChannelsPerGroup));
   numberbest->ChangeValue(wxString::Format(wxT("%i"), m_localCombinationsToList));

   return true;
}

bool CMFeatureSelectionDialog::TransferDataFromWindow() {
   SInt16 returnCode = 0;
   
   wxComboBox* distancemeas = (wxComboBox*) FindWindow(IDC_DistanceMeasureCombo);
   wxComboBox* numchannel = (wxComboBox*) FindWindow(IDC_NumberChannelsCombo);
   wxComboBox* classpairweight = (wxComboBox*) FindWindow(IDC_ClassPairWeightsCombo);
   wxComboBox* channel = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   wxComboBox* classcombo = (wxComboBox*) FindWindow(IDC_ClassCombo);
   wxComboBox* symbolcombo = (wxComboBox*) FindWindow(IDC_SymbolCombo);

   wxCheckBox* textwindow = (wxCheckBox*) FindWindow(IDC_TextWindow);
   wxCheckBox* disk = (wxCheckBox*) FindWindow(IDC_DiskFile);
   wxCheckBox* step = (wxCheckBox*) FindWindow(IDC_StepProcedure);
   wxCheckBox* feature = (wxCheckBox*) FindWindow(IDC_FeatureTransformation);

   wxTextCtrl* channelpergroup = (wxTextCtrl*) FindWindow(IDC_ChannelsPerGroup);
   wxTextCtrl* numberbest = (wxTextCtrl*) FindWindow(IDC_NumberBestToList);

   m_separabilityListSelection = distancemeas->GetSelection();
   m_channelCombinationSelection = numchannel->GetSelection();
   if (m_channelCombinationSelection < 0)
      m_channelCombinationSelection = m_channelCombinationSelection_Saved;

   m_interClassWeightsSelection = classpairweight->GetSelection();
   if (m_interClassWeightsSelection < 0)
      m_interClassWeightsSelection = m_interClassWeightsSelection_Saved;
   
   m_channelSelection = channel->GetSelection();
   if (m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
   m_classSelection = classcombo->GetSelection();
   if (m_classSelection < 0)
      m_classSelection = m_classSelection_Saved;
   
   m_symbolSelection = symbolcombo->GetSelection();

   m_textWindowFlag = textwindow->GetValue();
   m_diskFileFlag = disk->GetValue();
   m_searchFlag = step->GetValue();
   m_featureTransformationFlag = feature->GetValue();

   wxString channelpergroupString = channelpergroup->GetValue();
   //   channelpergroupString.ToLong(&m_savedContiguousChannelsPerGroup);
   m_savedContiguousChannelsPerGroup = wxAtoi(channelpergroupString);
   wxString numberbestString = numberbest->GetValue();
   //   numberbestString.ToLong(&m_localCombinationsToList);
   m_localCombinationsToList = wxAtoi(numberbestString);

   returnCode = CheckDialogRealValue (this,
													IDC_ChannelsPerGroup,
													1,
													m_maxContiguousChannelsPerGroup,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   if(returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_NumberBestToList,
													1,
													SInt16_MAX,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);      
   return (returnCode == 0);
}

//BEGIN_MESSAGE_MAP(CMFeatureSelectionDialog, CMDialog)
////{{AFX_MSG_MAP(CMFeatureSelectionDialog)
//ON_CBN_SELENDOK(IDC_ChannelCombo, OnSelendokChannelCombo)
//ON_CBN_SELENDOK(IDC_DistanceMeasureCombo, OnSelendokDistanceMeasureCombo)
//ON_CBN_SELENDOK(IDC_NumberChannelsCombo, OnSelendokNumberChannelsCombo)
//ON_EN_CHANGE(IDC_ChannelsPerGroup, OnChangeChannelsPerGroup)
//ON_BN_CLICKED(IDC_StepProcedure, OnStepProcedure)
//ON_EN_CHANGE(IDC_NumberBestToList, OnChangeNumberBestToList)
//ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)
//ON_BN_CLICKED(IDC_FeatureTransformation, OnFeatureTransformation)
//ON_BN_CLICKED(IDC_ListOptions, OnListOptions)
//ON_CBN_SELENDOK(IDC_ClassPairWeightsCombo, OnSelendokClassPairWeightsCombo)
////}}AFX_MSG_MAP
//END_MESSAGE_MAP()

BEGIN_EVENT_TABLE(CMFeatureSelectionDialog, CMDialog)
EVT_INIT_DIALOG(CMFeatureSelectionDialog::OnInitDialog)
EVT_COMBOBOX(IDC_ChannelCombo, CMFeatureSelectionDialog::OnSelendokChannelCombo)
EVT_COMBOBOX_DROPDOWN(IDC_ChannelCombo, CMFeatureSelectionDialog::OnSelendokChannelComboDropDown)
EVT_COMBOBOX(IDC_DistanceMeasureCombo, CMFeatureSelectionDialog::OnSelendokDistanceMeasureCombo)
EVT_COMBOBOX(IDC_NumberChannelsCombo, CMFeatureSelectionDialog::OnSelendokNumberChannelsCombo)
EVT_COMBOBOX_DROPDOWN(IDC_NumberChannelsCombo, CMFeatureSelectionDialog::OnSelendokNumberChannelsComboDropDown)
EVT_TEXT(IDC_ChannelsPerGroup, CMFeatureSelectionDialog::OnChangeChannelsPerGroup)
EVT_CHECKBOX(IDC_StepProcedure, CMFeatureSelectionDialog::OnStepProcedure)
EVT_TEXT(IDC_NumberBestToList, CMFeatureSelectionDialog::OnChangeNumberBestToList)
EVT_COMBOBOX(IDC_ClassCombo, CMFeatureSelectionDialog::OnSelendokClassCombo)
EVT_COMBOBOX_DROPDOWN(IDC_ClassCombo, CMFeatureSelectionDialog::OnSelendokClassComboDropDown)
EVT_CHECKBOX(IDC_FeatureTransformation, CMFeatureSelectionDialog::OnFeatureTransformation)
EVT_BUTTON(IDC_ListOptions, CMFeatureSelectionDialog::OnListOptions)
EVT_COMBOBOX(IDC_ClassPairWeightsCombo, CMFeatureSelectionDialog::OnSelendokClassPairWeightsCombo)
EVT_COMBOBOX_DROPDOWN(IDC_ClassPairWeightsCombo, CMFeatureSelectionDialog::OnSelendokClassPairWeightsComboDropDown)
EVT_BUTTON(IDS_HelpButton , CMFeatureSelectionDialog::OnHelpButton)
END_EVENT_TABLE()
//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the classification
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/28/98
//	Revised By:			Larry L. Biehl			Date: 05/04/98	

Boolean
CMFeatureSelectionDialog::DoDialog(
   SeparabilitySpecsPtr separabilitySpecsPtr,
   UInt16* allChanCombinationsPtr) {
   SInt16 returnCode;

   Boolean continueFlag = FALSE;
   SInt16 index;

   // Make sure initialization has been completed.

   if (!m_initializedFlag)
      return (FALSE);

   m_separabilitySpecsPtr = separabilitySpecsPtr;

   m_allChanCombinationsPtr = allChanCombinationsPtr;

   returnCode = ShowModal();

   while (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0 && returnCode == wxID_OK) {
         index = IDS_FeatureSelection11;
         if (m_featureTransformationFlag)
            index = IDS_FeatureSelection12;

         if (m_savedContiguousChannelsPerGroup > 1)
            index = IDS_FeatureSelection13;

         SetDLogControlHilite(NULL, wxID_OK, 255);
         DisplayAlert(kErrorAlertID, 1, kFeatureSelectionStrID, index, 0, NULL);
         SetDLogControlHilite(NULL, wxID_OK, 0);
//         continueFlag = false;
         returnCode = ShowModal();
   }// end "if m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0)
   
   if (returnCode == wxID_OK) {
      continueFlag = TRUE;
      
      SeparabilityDialogOK(separabilitySpecsPtr,
         m_separabilityDistance, // added because comparison index start from 1 
         m_featureTransformationFlag,
         m_channelSelection,
         m_localActiveNumberFeatures,
         m_localActiveFeaturesPtr,
         m_channelCombinationSelection,
         m_localNumberChannelGroupCombinations,
         m_localChannelCombinationsPtr,
         m_savedContiguousChannelsPerGroup,
         m_searchFlag,
         m_localCombinationsToList,
         m_classSelection,
         m_localNumberClasses,
         m_classListPtr,
         m_interClassWeightsSelection + 1,
         m_localClassPairWeightsListPtr,
         m_localDefaultClassPairWeight,
         m_symbolSelection,
         m_localSymbolsPtr,
         m_textWindowFlag,
         m_diskFileFlag);      
   } // end "if (returnCode == IDOK)"

   return (continueFlag);

} // end "DoDialog" 

void
CMFeatureSelectionDialog::HandleChannelCombinationsMenu(
   UInt16 channelCombinationsMenuItemNumber) {
   Boolean returnFlag;


   //DDX_CBIndex(m_dialogFromPtr, channelCombinationsMenuItemNumber, m_channelCombinationSelection);
   wxComboBox* channelcombination = (wxComboBox*) FindWindow(channelCombinationsMenuItemNumber);
   m_channelCombinationSelection = channelcombination->GetSelection();

   if (m_channelCombinationSelection == kSubsetMenuItem) {
      // Subset of channels to be used.				
      SetDLogControlHilite(NULL, wxID_OK, 255);
      CMChannelCombinationsDlg* channelCombinationsDialogPtr = NULL;

      try {
         channelCombinationsDialogPtr = new CMChannelCombinationsDlg();

         returnFlag = channelCombinationsDialogPtr->DoDialog(
            m_channelCombinationSelection,
            &m_localNumberChannelGroupCombinations,
            m_localChannelCombinationsPtr,
            m_separabilitySpecsPtr->numberAllChanCombinations,
            m_allChanCombinationsPtr,
            m_savedContiguousChannelsPerGroup,
            m_featureTransformationFlag);

         if (channelCombinationsDialogPtr != NULL)
            delete channelCombinationsDialogPtr;
      } catch (int e) {
         MemoryMessage(0, kObjectMessage);
      }
      //END_CATCH_ALL

      if (m_localNumberChannelGroupCombinations ==
         m_separabilitySpecsPtr->numberAllChanCombinations)
         m_channelCombinationSelection = kAllMenuItem;

      if (m_channelCombinationSelection == kAllMenuItem) {
         //         DDX_CBIndex(m_dialogToPtr, channelCombinationsMenuItemNumber, m_channelCombinationSelection);
         channelcombination->SetSelection(m_channelCombinationSelection);
      }

      SetDLogControlHilite(NULL, wxID_OK, 0);

   } // end "if (m_channelCombinationsSelection == kSubsetMenuItem)"          

} // end "HandleChannelCombinationsMenu"  


/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionDialog message handlers

void CMFeatureSelectionDialog::OnInitDialog(wxInitDialogEvent& event) {
   SInt16 channelCombinationSelection,
      channelSelection,
      classSelection,
      interClassWeightsSelection,
      symbolSelection;


   //CDialog::OnInitDialog();

   // Initialize dialog variables.

   SeparabilityDialogInitialize(this,
      m_separabilitySpecsPtr,
      m_localFeaturesPtr,
      m_localTransformFeaturesPtr,
      m_classListPtr,
      m_localSymbolsPtr,
      m_localChannelCombinationsPtr,
      &m_localClassPairWeightsListPtr,
      &m_separabilityDistance,
      &m_numberEigenvectors,
      (Boolean*) & m_featureTransformAllowedFlag,
      (Boolean*) & m_featureTransformationFlag,
      &channelSelection,
      &m_localActiveNumberFeatures,
      &channelCombinationSelection,
      &m_localNumberChannelGroupCombinations,
      &m_savedNumberChannelGroupCombinations,
      &m_savedContiguousChannelsPerGroup,
      (Boolean*) & m_searchFlag,
      &m_maxContiguousChannelsPerGroup,
      &m_localCombinationsToList,
      &classSelection,
      &m_localNumberClasses,
      &m_numberClassCombinations,
      &interClassWeightsSelection,
      &m_localDefaultClassPairWeight,
      &symbolSelection,
      (Boolean*) & m_textWindowFlag,
      (Boolean*) & m_diskFileFlag);

   // Set feature/transform feature parameters  

   InitializeDialogFeatureParameters(m_featureTransformationFlag,
      m_localActiveNumberFeatures,
      gProjectInfoPtr->numberStatisticsChannels,
      gTransformationMatrix.numberFeatures,
      m_localFeaturesPtr,
      m_localTransformFeaturesPtr,
      &m_localNumberFeatures,
      &m_localNumberTransformFeatures,
      &m_localActiveFeaturesPtr);

   // Get the feature selection algorithm list selection that matches the input
   // feature selection algorithm.

   m_separabilityListSelection = GetComboListSelection(IDC_DistanceMeasureCombo,
      m_separabilityDistance);
   //   wxComboBox* distmeasCombo = (wxComboBox*)FindWindow(IDC_DistanceMeasureCombo);
   //   m_separabilityListSelection = distmeasCombo->GetSelection();

   if (m_separabilityListSelection == -1)
      m_separabilityListSelection = 0;

   //	Set the channels/features list item				

   m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
   m_channelSelection = channelSelection;

   // Number channel combinations.

   m_channelCombinationSelection = channelCombinationSelection;

   // Current maximum number of channel combinations 

   LoadDItemRealValue(this,
      IDC_MaxSearchCombinations,
      (double) m_separabilitySpecsPtr->maxNumberFeatureCombinations,
      0);

   // Classes to use.         

   m_classSelection = classSelection;
   ::LoadDItemValue(this, IDC_NumberClassCombinations, m_numberClassCombinations);

   // Interclass weights to use.             
   // Adjust for 0 base index.
   // Unequal interclass weights are not available yet.																	

   m_interClassWeightsSelection = interClassWeightsSelection - 1;
   HideDialogItem(this, IDC_WeightsEqual);
   //	((CComboBox*)GetDlgItem(IDC_ClassPairWeightsCombo))->DeleteString(1); 

   // Symbols to use.             
   // Adjust for 0 base index.
   // User defined symbols are not available yet.																	

   m_symbolSelection = symbolSelection - 1;
   wxComboBox* symbolcombo = (wxComboBox*) FindWindow(IDC_SymbolCombo);
   if(symbolcombo->HasClientData())
      symbolcombo->Delete(1);
   
   HideDialogItem(this, IDC_SymbolCombo);
   //   ((CComboBox*) GetDlgItem(IDC_SymbolCombo))->DeleteString(1);


   if (TransferDataToWindow())
      PositionDialogWindow();

   // Set default text selection to first edit text item	

   //	GetDlgItem(IDC_ChannelsPerGroup)->SetFocus();
   //	SendDlgItemMessage( IDC_ChannelsPerGroup, EM_SETSEL, 0, MAKELPARAM(0, -1) );
   SelectDialogItemText(this, IDC_ChannelsPerGroup, 0, SInt16_MAX);

   //return FALSE; // return TRUE  unless you set the focus to a control
   this->Layout();
   this->Fit();
} // end "OnInitDialog" 

void
CMFeatureSelectionDialog::OnSelendokDistanceMeasureCombo(wxCommandEvent& event) {
   wxComboBox* comboBoxPtr;

   SInt16 channelSelection,
      separabilityDistance,
      savedSeparabilityListSelection;

   Boolean returnFlag = TRUE;


   savedSeparabilityListSelection = m_separabilityListSelection;

   comboBoxPtr = (wxComboBox*) FindWindow(IDC_DistanceMeasureCombo);

   //DDX_CBIndex(m_dialogFromPtr,IDC_DistanceMeasureCombo, m_separabilityListSelection);
   m_separabilityListSelection = comboBoxPtr->GetSelection();
   // Get the actual classification procedure code.


   //separabilityDistance = (SInt16) comboBoxPtr->GetItemData(m_separabilityListSelection);

   SInt64 separabilityDistance64 = (SInt64) ((int*) comboBoxPtr->GetClientData(m_separabilityListSelection));
   separabilityDistance = (SInt16) separabilityDistance64;


   if (separabilityDistance != 0) {
      m_separabilityDistance = separabilityDistance;

      m_featureTransformAllowedFlag =
         SeparabilityDialogGetFeatureTransformAllowedFlag(
         m_separabilityDistance,
         m_numberEigenvectors);

      SeparabilityDialogCheckFeatureTransformation(this,
         (Boolean*) & m_featureTransformationFlag,
         &m_localActiveNumberFeatures,
         &m_localActiveFeaturesPtr,
         m_featureTransformAllowedFlag,
         IDC_FeatureTransformation,
         IDC_ChannelPrompt,
         &channelSelection,
         m_localNumberFeatures,
         m_localFeaturesPtr,
         m_localNumberTransformFeatures,
         m_localTransformFeaturesPtr);

      m_channelSelection = channelSelection;

      //DDX_CBIndex(m_dialogToPtr, IDC_ChannelCombo, m_channelSelection);      
      wxComboBox* channelcombo = (wxComboBox*) FindWindow(IDC_ChannelCombo);
      channelcombo->SetSelection(m_channelSelection);

      if (m_featureTransformationFlag && !m_featureTransformAllowedFlag) {
         SeparabilityDialogUpdateChannelCombinationItems(
            this,
            m_separabilitySpecsPtr,
            TRUE,
            m_localActiveNumberFeatures,
            m_allChanCombinationsPtr,
            m_savedContiguousChannelsPerGroup,
            m_searchFlag,
            m_channelCombinationSelection,
            m_localChannelCombinationsPtr,
            &m_localNumberChannelGroupCombinations,
            &m_savedNumberChannelGroupCombinations,
            &m_localCombinationsToList,
            &m_maxContiguousChannelsPerGroup);

         SeparabilityDialogUpdateChannelFeatureGroupText(
            this,
            (SInt16) m_savedContiguousChannelsPerGroup,
            m_featureTransformationFlag);

      } // end "if (m_featureTransformFlag && !m_featureTransformAllowedFlag)"

   }// end "if (separabilityDistance != 0)"

   else // separabilityDistance == 0
   {
      m_separabilityListSelection = savedSeparabilityListSelection;

   } // end "else separabilityDistance == 0" 

   //   DDX_CBIndex(m_dialogToPtr,IDC_DistanceMeasureCombo, m_separabilityListSelection);
   wxComboBox* distmeas = (wxComboBox*) FindWindow(IDC_DistanceMeasureCombo);
   distmeas->SetSelection(m_separabilityListSelection);

   //   m_separabilityDistance =(SInt16) comboBoxPtr->GetItemData(m_separabilityListSelection);

   separabilityDistance64 = (SInt64) ((int*) comboBoxPtr->GetClientData(m_separabilityListSelection));
   m_separabilityDistance = (SInt16) separabilityDistance;


} // end "OnSelendokDistanceMeasureCombo" 

void
CMFeatureSelectionDialog::OnFeatureTransformation(wxCommandEvent& event) {
   SInt16 channelSelection;


   //DDX_Check(m_dialogFromPtr,IDC_FeatureTransformation,m_featureTransformationFlag);

   wxCheckBox* feature = (wxCheckBox*) FindWindow(IDC_FeatureTransformation);
   m_featureTransformationFlag = feature->GetValue();

   SeparabilityDialogCheckFeatureTransformation(this,
      (Boolean*) & m_featureTransformationFlag,
      &m_localActiveNumberFeatures,
      &m_localActiveFeaturesPtr,
      m_featureTransformAllowedFlag,
      IDC_FeatureTransformation,
      IDC_ChannelPrompt,
      &channelSelection,
      m_localNumberFeatures,
      m_localFeaturesPtr,
      m_localNumberTransformFeatures,
      m_localTransformFeaturesPtr);

   m_channelSelection = channelSelection;

   //   DDX_CBIndex(m_dialogToPtr, IDC_ChannelCombo, m_channelSelection);
   wxComboBox* channel = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   channel->SetSelection(m_channelSelection);


   SeparabilityDialogUpdateChannelCombinationItems(
      this,
      m_separabilitySpecsPtr,
      TRUE,
      m_localActiveNumberFeatures,
      m_allChanCombinationsPtr,
      m_savedContiguousChannelsPerGroup,
      m_searchFlag,
      m_channelCombinationSelection,
      m_localChannelCombinationsPtr,
      &m_localNumberChannelGroupCombinations,
      &m_savedNumberChannelGroupCombinations,
      &m_localCombinationsToList,
      &m_maxContiguousChannelsPerGroup);

   SeparabilityDialogUpdateChannelFeatureGroupText(
      this,
      (SInt16) m_savedContiguousChannelsPerGroup,
      m_featureTransformationFlag);

} // end "OnFeatureTransformation"

void
CMFeatureSelectionDialog::OnSelendokChannelCombo(wxCommandEvent& event) {
   HandleChannelsMenu(IDC_ChannelCombo,
      m_featureTransformationFlag,
      (SInt16) gProjectInfoPtr->numberStatisticsChannels,
      2,
      TRUE);

   SeparabilityDialogUpdateChannelCombinationItems(
      this,
      m_separabilitySpecsPtr,
      TRUE,
      m_localActiveNumberFeatures,
      m_allChanCombinationsPtr,
      m_savedContiguousChannelsPerGroup,
      m_searchFlag,
      m_channelCombinationSelection,
      m_localChannelCombinationsPtr,
      &m_localNumberChannelGroupCombinations,
      &m_savedNumberChannelGroupCombinations,
      &m_localCombinationsToList,
      &m_maxContiguousChannelsPerGroup);

} // end "OnSelendokChannelCombo"

//void
//CMFeatureSelectionDialog::OnSelendokChannelComboDropDown(wxCommandEvent& event) {
//   wxComboBox* channelcombo = (wxComboBox *) FindWindow(IDC_ChannelCombo);
//   m_channelSelection_Saved = channelcombo->GetSelection();
//   channelcombo->SetSelection(-1);
//}

void
CMFeatureSelectionDialog::OnSelendokNumberChannelsCombo(wxCommandEvent& event) {
   HandleChannelCombinationsMenu(IDC_NumberChannelsCombo);

   SeparabilityDialogUpdateChannelCombinationItems(
      this,
      m_separabilitySpecsPtr,
      FALSE,
      m_localActiveNumberFeatures,
      m_allChanCombinationsPtr,
      m_savedContiguousChannelsPerGroup,
      m_searchFlag,
      m_channelCombinationSelection,
      m_localChannelCombinationsPtr,
      &m_localNumberChannelGroupCombinations,
      &m_savedNumberChannelGroupCombinations,
      &m_localCombinationsToList,
      &m_maxContiguousChannelsPerGroup);

} // end "OnSelendokNumberChannelsCombo"

void
CMFeatureSelectionDialog::OnSelendokNumberChannelsComboDropDown(wxCommandEvent& event) {
   wxComboBox* numberchannelcombo = (wxComboBox *) FindWindow(IDC_NumberChannelsCombo);
   m_channelCombinationSelection_Saved = numberchannelcombo->GetSelection();
   numberchannelcombo->SetSelection(-1);
}

void
CMFeatureSelectionDialog::OnChangeChannelsPerGroup(wxCommandEvent& event) {
   SInt32 newContiguousChannelsPerGroup;
   SInt16 returncode = 0;
   
   returncode = CheckValues(IDC_ChannelsPerGroup, 1, m_maxContiguousChannelsPerGroup, &newContiguousChannelsPerGroup);
   //DDX_Text(m_dialogFromPtr,IDC_ChannelsPerGroup,newContiguousChannelsPerGroup);
//   wxTextCtrl* channelpergroup = (wxTextCtrl*) FindWindow(IDC_ChannelsPerGroup);
//   wxString channelpergroupString = channelpergroup->GetValue();   
//   newContiguousChannelsPerGroup = wxAtoi(channelpergroupString);
//   
//      if (newContiguousChannelsPerGroup < 1 ||
//      newContiguousChannelsPerGroup > m_maxContiguousChannelsPerGroup) {
//      //DDX_Text(m_dialogToPtr,IDC_ChannelsPerGroup, m_savedContiguousChannelsPerGroup);
//      channelpergroup->ChangeValue(wxString::Format(wxT("%i"), m_maxContiguousChannelsPerGroup));

//      
//   } else // newContiguousChannelsPerGroup in proper range
//   {
   if(returncode == 0){
      if (m_savedContiguousChannelsPerGroup != newContiguousChannelsPerGroup) {
         m_savedContiguousChannelsPerGroup = newContiguousChannelsPerGroup;

         SeparabilityDialogUpdateChannelCombinationItems(
            this,
            m_separabilitySpecsPtr,
            TRUE,
            m_localActiveNumberFeatures,
            m_allChanCombinationsPtr,
            m_savedContiguousChannelsPerGroup,
            m_searchFlag,
            m_channelCombinationSelection,
            m_localChannelCombinationsPtr,
            &m_localNumberChannelGroupCombinations,
            &m_savedNumberChannelGroupCombinations,
            &m_localCombinationsToList,
            &m_maxContiguousChannelsPerGroup);

         SeparabilityDialogUpdateChannelFeatureGroupText(
            this,
            (SInt16) m_savedContiguousChannelsPerGroup,
            m_featureTransformationFlag);

      } // end "if (m_savedContiguousChannelsPerGroup != ..."
   }
//   } // end "else newContiguousChannelsPerGroup in proper range"                                                          
}
// end "OnChangeChannelsPerGroup"

SInt16 CMFeatureSelectionDialog::CheckValues(int controlID, SInt16 minValue, SInt16 maxValue, SInt32* newValue){
   SInt16 errorFlag = 0;
   wxTextCtrl* textControlPtr = wxDynamicCast(FindWindow(controlID), wxTextCtrl);
        
   if (textControlPtr != NULL) {
      wxString value = textControlPtr->GetValue();

      if (value.Length() == 0) {
         errorFlag = 1;
      }else{
         SInt32 outputValue;
         outputValue = wxAtoi(value);
         if(outputValue > maxValue || outputValue < minValue){
            errorFlag = 1;
            if(outputValue < minValue){  
               outputValue = minValue;
        
            } else{ 
               outputValue = maxValue;
            }                 
            textControlPtr->ChangeValue(wxString::Format(wxT("%i"), outputValue));     
          } // end "valueDouble > maxValue || valueDouble < minValue"
         *newValue = outputValue;
      } // end "if (m_value.Length() > 0)"

   } // end "if (textControlPtr != NULL)"

   return errorFlag;
}

void
CMFeatureSelectionDialog::OnStepProcedure(wxCommandEvent& event) {
   //   DDX_Check(m_dialogFromPtr, IDC_StepProcedure, m_searchFlag);
   wxCheckBox* search = (wxCheckBox*) FindWindow(IDC_StepProcedure);
   m_searchFlag = search->GetValue();


   SeparabilityDialogUpdateChannelCombinationItems(
      this,
      m_separabilitySpecsPtr,
      TRUE,
      m_localActiveNumberFeatures,
      m_allChanCombinationsPtr,
      m_savedContiguousChannelsPerGroup,
      m_searchFlag,
      m_channelCombinationSelection,
      m_localChannelCombinationsPtr,
      &m_localNumberChannelGroupCombinations,
      &m_savedNumberChannelGroupCombinations,
      &m_localCombinationsToList,
      &m_maxContiguousChannelsPerGroup);

} // end "OnStepProcedure"

void
CMFeatureSelectionDialog::OnChangeNumberBestToList(wxCommandEvent& event) {
   SInt32 newCombinationsToList;

   //   DDX_Text(m_dialogFromPtr,IDC_NumberBestToList, newCombinationsToList);
   wxTextCtrl* numberbest = (wxTextCtrl*) FindWindow(IDC_NumberBestToList);
   wxString numberbestString = numberbest->GetValue();
   newCombinationsToList = wxAtoi(numberbestString);
   if (newCombinationsToList < 1 || newCombinationsToList > SInt16_MAX) {
      //      DDX_Text(m_dialogToPtr,IDC_NumberBestToList, m_localCombinationsToList);
      numberbest->ChangeValue(wxString::Format(wxT("%i"), m_localCombinationsToList));

      //      DDV_MinMaxLong(m_dialogFromPtr, newCombinationsToList,1, SInt16_MAX);

   }// end "if (newCombinationsToList < 1 || ..."

   else // newCombinationsToList in proper range 
      m_localCombinationsToList = newCombinationsToList;

} // end "OnChangeNumberBestToList"

void
CMFeatureSelectionDialog::OnSelendokClassCombo(wxCommandEvent& event) {
   HandleClassesMenu(&m_localNumberClasses,
      (SInt16*) m_classListPtr,
      2,
      (SInt16) gProjectInfoPtr->numberStatisticsClasses,
      IDC_ClassCombo,
      &m_classSelection);

   SeparabilityDialogUpdateNumberClassCombintations(
      this,
      m_classSelection,
      &m_localNumberClasses,
      m_classListPtr);

} // end "OnSelendokClassCombo"

//void
//CMFeatureSelectionDialog::OnSelendokClassComboDropDown(wxCommandEvent& event) {
//   wxComboBox* classcombo = (wxComboBox *) FindWindow(IDC_ClassCombo);
//   m_classSelection_Saved = classcombo->GetSelection();
//   classcombo->SetSelection(-1);
//}
//void CMFeatureSelectionDialog::OnOK() {
//   SInt16 index;
//
//   if (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0) {
//      index = IDS_FeatureSelection11;
//      if (m_featureTransformationFlag)
//         index = IDS_FeatureSelection12;
//
//      if (m_savedContiguousChannelsPerGroup > 1)
//         index = IDS_FeatureSelection13;
//
//      SetDLogControlHilite(NULL, wxID_OK, 255);
//      DisplayAlert(kErrorAlertID, 1, kFeatureSelectionStrID, index, 0, NULL);
//      SetDLogControlHilite(NULL, wxID_OK, 0);
//   }// end "if (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0)" 
//
//   else // everything is 'okay'
//      CMDialog::OnOK();
//
//} // end "OnOK" 

void
CMFeatureSelectionDialog::OnListOptions(wxCommandEvent& event) {

   SetDLogControlHilite(NULL, wxID_OK, 255);
   SeparabilityListDialog(&m_localCombinationsToList);
   SetDLogControlHilite(NULL, wxID_OK, 0);

   //   DDX_Text(m_dialogToPtr,IDC_NumberBestToList, m_localCombinationsToList);
   wxTextCtrl* numberbest = (wxTextCtrl*) FindWindow(IDC_NumberBestToList);
   numberbest->ChangeValue(wxString::Format(wxT("%i"), m_localCombinationsToList));

} // end "OnListOptions"

void CMFeatureSelectionDialog::OnSelendokClassPairWeightsCombo(wxCommandEvent& event) {
   HandleClassPairWeightsMenu(
      &m_localClassPairWeightsListPtr,
      IDC_ClassPairWeightsCombo,
      &m_interClassWeightsSelection,
      &m_localDefaultClassPairWeight);

} // end "OnSelendokClassPairWeightsCombo"

//void
//CMFeatureSelectionDialog::OnSelendokClassPairWeightsComboDropDown(wxCommandEvent& event) {
//   wxComboBox* classweightcombo = (wxComboBox *) FindWindow(IDC_ClassPairWeightsCombo);
//   m_interClassWeightsSelection_Saved = classweightcombo->GetSelection();
//   classweightcombo->SetSelection(-1);
//}
void CMFeatureSelectionDialog::OnHelpButton(wxCommandEvent& event)
{
//   wxMessageBox(wxT("This menu item ... "), wxT("Help"), wxOK| wxSTAY_ON_TOP, this);
   DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_DialogMsg0, 0, NULL);
//   wxAboutDialogInfo info;
//   info.SetDescription(_("Hello "));;
//   
//   wxAboutBox(info);
   
   
}

void CMFeatureSelectionDialog::CreateControls()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);

   wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
   //wxBoxSizer* bSizer211;
   bSizer211 = new wxBoxSizer(wxHORIZONTAL);

   wxBoxSizer* bSizer212;
   bSizer212 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer214;
   bSizer214 = new wxBoxSizer(wxVERTICAL);

   m_staticText213 = new wxStaticText(this, wxID_ANY, wxT("Distance measure:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText213->Wrap(-1);
   bSizer214->Add(m_staticText213, 0, wxALL, 5);

   m_comboBox34 = new wxComboBox(this, IDC_DistanceMeasureCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox34->Append(wxT("Bhattacharyya"));
   m_comboBox34->Append(wxT("Error function Bhattacharyya"));
   m_comboBox34->Append(wxT("'Mean' Bhattacharyya"));
   m_comboBox34->Append(wxT("'Covariance' Bhattacharyya"));
   m_comboBox34->Append(wxT("'Noncovariance' Bhattacharyya"));
   m_comboBox34->Append(wxT("Transformed divergence"));
   m_comboBox34->Append(wxT("Divergence"));
   SetUpToolTip(m_comboBox34, IDS_ToolTip149);
   bSizer214->Add(m_comboBox34, 0, wxLEFT, 25);

   m_checkBox50 = new wxCheckBox(this, IDC_FeatureTransformation, wxT("Use feature transformation"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox50, IDS_ToolTip150);
   bSizer214->Add(m_checkBox50, 0, wxLEFT | wxTOP, 15);


   bSizer212->Add(bSizer214, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer215;
   bSizer215 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText214 = new wxStaticText(this, IDC_ChannelPrompt, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText214->Wrap(-1);
   SetUpToolTip(m_staticText214, IDS_ToolTip52);
   bSizer215->Add(m_staticText214, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox35 = new wxComboBox(this, IDC_ChannelCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox35->Append(wxT("All"));
   m_comboBox35->Append(wxT("Subset..."));
   SetUpToolTip(m_comboBox35, IDS_ToolTip52);
   bSizer215->Add(m_comboBox35, 0, wxALIGN_CENTER | wxALL, 5);


   bSizer212->Add(bSizer215, 0, wxEXPAND, 5);

   wxStaticBoxSizer* sbSizer19;
   sbSizer19 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Channel Combinations")), wxVERTICAL);

   wxBoxSizer* bSizer216;
   bSizer216 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText215 = new wxStaticText(sbSizer19->GetStaticBox(), IDC_NumberChannelsPrompt, wxT("Number of channels:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText215->Wrap(-1);
   SetUpToolTip(m_staticText215, IDS_ToolTip151);
   bSizer216->Add(m_staticText215, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox36 = new wxComboBox(sbSizer19->GetStaticBox(), IDC_NumberChannelsCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox36->Append(wxT("All possible"));
   m_comboBox36->Append(wxT("Subset..."));
   SetUpToolTip(m_comboBox36, IDS_ToolTip151);
   bSizer216->Add(m_comboBox36, 0, wxALIGN_CENTER | wxALL, 5);


   sbSizer19->Add(bSizer216, 0, wxEXPAND | wxLEFT, 15);

   wxBoxSizer* bSizer217;
   bSizer217 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText216 = new wxStaticText(sbSizer19->GetStaticBox(), IDC_ChannelsPerGroupPrompt, wxT("Contiguous channels per group:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText216->Wrap(-1);
    SetUpToolTip(m_staticText216, IDS_ToolTip152);
   bSizer217->Add(m_staticText216, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl120 = new wxTextCtrl(sbSizer19->GetStaticBox(), IDC_ChannelsPerGroup, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0);
   m_textCtrl120->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_channelgroupString));
   SetUpToolTip(m_textCtrl120, IDS_ToolTip152);
   bSizer217->Add(m_textCtrl120, 0, wxALL, 5);


   sbSizer19->Add(bSizer217, 1, wxEXPAND | wxLEFT, 15);

   m_checkBox47 = new wxCheckBox(sbSizer19->GetStaticBox(), IDC_StepProcedure, wxT("Use step procedure"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox47, IDS_ToolTip153);
   sbSizer19->Add(m_checkBox47, 0, wxLEFT, 20);

   wxBoxSizer* bSizer218;
   bSizer218 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText217 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Max search combinations:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText217->Wrap(-1);
   SetUpToolTip(m_staticText217, IDS_ToolTip154);
   bSizer218->Add(m_staticText217, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText218 = new wxStaticText(sbSizer19->GetStaticBox(), IDC_MaxSearchCombinations, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText218->Wrap(-1);
   SetUpToolTip(m_staticText218, IDS_ToolTip154);
   bSizer218->Add(m_staticText218, 0, wxALIGN_CENTER | wxALL, 5);


   sbSizer19->Add(bSizer218, 0, wxEXPAND | wxLEFT, 15);

   wxBoxSizer* bSizer219;
   bSizer219 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText219 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Max number combinations to list:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText219->Wrap(-1);
   SetUpToolTip(m_staticText219, IDS_ToolTip155);
   bSizer219->Add(m_staticText219, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl121 = new wxTextCtrl(sbSizer19->GetStaticBox(), IDC_NumberBestToList, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0);
   SetUpToolTip(m_textCtrl121, IDS_ToolTip155);
   m_textCtrl121->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_numBestString));
   bSizer219->Add(m_textCtrl121, 0, wxALL, 5);


   sbSizer19->Add(bSizer219, 0, wxEXPAND | wxLEFT, 15);


   bSizer212->Add(sbSizer19, 0, wxEXPAND, 5);


   bSizer211->Add(bSizer212, 0, wxALL | wxEXPAND, 12);

   wxBoxSizer* bSizer213;
   bSizer213 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer220;
   bSizer220 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText221 = new wxStaticText(this, IDC_ClassPrompt, wxT("Classes:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText221->Wrap(-1);
   SetUpToolTip(m_staticText221, IDS_ToolTip103);
   bSizer220->Add(m_staticText221, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox38 = new wxComboBox(this, IDC_ClassCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox38->Append(wxT("All"));
   m_comboBox38->Append(wxT("Subset..."));
   SetUpToolTip(m_comboBox38, IDS_ToolTip103);
   bSizer220->Add(m_comboBox38, 0, wxALIGN_CENTER | wxALL, 5);


   bSizer213->Add(bSizer220, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer221;
   bSizer221 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText222 = new wxStaticText(this, IDC_NumberClassCombinations, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText222->Wrap(-1);
   //bSizer221->Add(m_staticText222, 0, wxALIGN_RIGHT | wxALL, 5);
   bSizer221->Add(m_staticText222, 0, wxALIGN_CENTER | wxALL, 5);
   m_staticText223 = new wxStaticText(this, wxID_ANY, wxT("combinations"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText223->Wrap(-1);
   bSizer221->Add(m_staticText223, 0, wxALL, 5);


   bSizer213->Add(bSizer221, 0, wxEXPAND | wxLEFT, 25);

   wxBoxSizer* bSizer222;
   bSizer222 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText224 = new wxStaticText(this, IDC_ClassPairWeightsPrompt, wxT("Weights:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText224->Wrap(-1);
   SetUpToolTip(m_staticText224, IDS_ToolTip156);
   bSizer222->Add(m_staticText224, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox39 = new wxComboBox(this, IDC_ClassPairWeightsCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox39->Append(wxT("Equal"));
   m_comboBox39->Append(wxT("Unequal..."));
   SetUpToolTip(m_comboBox39, IDS_ToolTip156);
   bSizer222->Add(m_comboBox39, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText229 = new wxStaticText(this, IDC_WeightsEqual, wxT("Equal"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText229->Wrap(-1);
   bSizer222->Add(m_staticText229, 0, wxALIGN_CENTER | wxALL, 5);


   bSizer213->Add(bSizer222, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer223;
   bSizer223 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText225 = new wxStaticText(this, IDC_SymbolPrompt, wxT("Symbols:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText225->Wrap(-1);
   bSizer223->Add(m_staticText225, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox40 = new wxComboBox(this, IDC_SymbolCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox40->Append(wxT("Default set"));
   m_comboBox40->Append(wxT("User defined..."));
   SetUpToolTip(m_comboBox40, IDS_ToolTip157);
   bSizer223->Add(m_comboBox40, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText228 = new wxStaticText(this, IDC_SymbolsDefaultSet, wxT("Default set"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText228->Wrap(-1);
   bSizer223->Add(m_staticText228, 0, wxALIGN_CENTER | wxALL, 5);


   bSizer213->Add(bSizer223, 0, wxEXPAND, 5);

   m_button46 = new wxButton(this, IDC_ListOptions, wxT("List Options..."), wxDefaultPosition, wxSize(180, -1), 0);
   SetUpToolTip(m_button46, IDS_ToolTip158);
   bSizer213->Add(m_button46, 0, wxALL, 5);

   wxBoxSizer* bSizer224;
   bSizer224 = new wxBoxSizer(wxVERTICAL);

   m_staticText226 = new wxStaticText(this, wxID_ANY, wxT("Write results to:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText226->Wrap(-1);
   bSizer224->Add(m_staticText226, 0, wxALL, 5);

   m_checkBox48 = new wxCheckBox(this, IDC_TextWindow, wxT("project text window"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox48, IDS_ToolTip159);
   bSizer224->Add(m_checkBox48, 0, wxLEFT, 15);

   m_checkBox49 = new wxCheckBox(this, IDC_DiskFile, wxT("disk file"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox49, IDS_ToolTip160);
   bSizer224->Add(m_checkBox49, 0, wxLEFT, 15);


   bSizer213->Add(bSizer224, 0, wxEXPAND, 5);
	/*
   wxBoxSizer* bSizer225;
   bSizer225 = new wxBoxSizer(wxHORIZONTAL);
   
   wxContextHelpButton* helpButton = new wxContextHelpButton(this, IDS_HelpButton , wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(helpButton, IDS_DialogMsg1);
      
   bSizer225->Add(helpButton, 0, wxALIGN_CENTER|wxALL, 5);
   
   m_button47 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer225->Add(m_button47, 0, wxALL, 5);

   m_button48 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer225->Add(m_button48, 0, wxALL, 5);

   bSizer213->Add(bSizer225, 0, wxALIGN_RIGHT, 5);
	*/
   bSizer211->Add (bSizer213, 0, wxEXPAND | wxLEFT|wxTOP|wxRIGHT, 12);
	
   bVSizerMain->Add (bSizer211, 0);
	
	CreateStandardButtons (bVSizerMain);

   this->SetSizer(bVSizerMain);
   this->Layout();

   this->Centre(wxBOTH);
}



/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionListDialog dialog

CMFeatureSelectionListDialog::CMFeatureSelectionListDialog(wxWindow* parent, wxWindowID id, const wxString& title)
: CMDialog(CMFeatureSelectionListDialog::IDD, parent, title) {
   //{{AFX_DATA_INIT(CMFeatureSelectionListDialog)
   m_sortChannelCombinations = -1;
   //new
   m_sortChannelCombinations = 0;
   //end new
   m_thresholdedGroupTableFlag = FALSE;
   m_thresholdedClassPairTableFlag = FALSE;
   m_localDistancesLessThanToGroup = 0;
   m_localDistancesLessThanToList = 0;
   m_listClassPairDistancesFlag = FALSE;
   m_localMinDistanceRangeToList = 0;
   m_localMaxDistanceRangeToList = 0;
   m_localCombinationsToList = 1;
   m_separabilityTableFlag = FALSE;
   //}}AFX_DATA_INIT

   m_initializedFlag = CMDialog::m_initializedFlag;

   CreateControls();
   //SetSizerAndFit(bSizer226);
}



bool CMFeatureSelectionListDialog::TransferDataToWindow ()

{
   wxRadioButton* avgclassdist = (wxRadioButton*) FindWindow(IDC_AverageClassDistance);
   wxRadioButton* minclassdist = (wxRadioButton*) FindWindow(IDC_MinimumClassDistance);
   wxRadioButton* ordercompute = (wxRadioButton*) FindWindow(IDC_OrderComputed);

   wxCheckBox* thresholdgroup = (wxCheckBox*) FindWindow(IDC_ClassGroupThresholdTable);
   wxCheckBox* thresholdclass = (wxCheckBox*) FindWindow(IDC_ClassPairThresholdTable);
   wxCheckBox* seperatetabe = (wxCheckBox*) FindWindow(IDC_SeparabilityTable);
   wxCheckBox* listclasspair = (wxCheckBox*) FindWindow(IDC_ListClassPairDistances);

   wxTextCtrl* classgroupthres = (wxTextCtrl*) FindWindow(IDC_ClassGroupThreshold);
   wxTextCtrl* classpairthres = (wxTextCtrl*) FindWindow(IDC_ClassPairThreshold);
   wxTextCtrl* mindistrange = (wxTextCtrl*) FindWindow(IDC_MinDistanceRange);
   wxTextCtrl* maxdistrange = (wxTextCtrl*) FindWindow(IDC_MaxDistanceRange);
   wxTextCtrl* numberbest = (wxTextCtrl*) FindWindow(IDC_NumberBestCombinations);

   thresholdgroup->SetValue(m_thresholdedGroupTableFlag);
   thresholdclass->SetValue(m_thresholdedClassPairTableFlag);
   seperatetabe->SetValue(m_separabilityTableFlag);
   listclasspair->SetValue(m_listClassPairDistancesFlag);

   classgroupthres->ChangeValue(wxString::Format(wxT("%.1f"), m_localDistancesLessThanToGroup));
   classpairthres->ChangeValue(wxString::Format(wxT("%.1f"), m_localDistancesLessThanToList));
   mindistrange->ChangeValue(wxString::Format(wxT("%.1f"), m_localMinDistanceRangeToList));
   maxdistrange->ChangeValue(wxString::Format(wxT("%.1f"), m_localMaxDistanceRangeToList));
   numberbest->ChangeValue(wxString::Format(wxT("%i"), m_localCombinationsToList));

   if (m_sortChannelCombinations == 0) {
      avgclassdist->SetValue(true);
      minclassdist->SetValue(false);
      ordercompute->SetValue(false);
   } else if (m_sortChannelCombinations == 1) {
      avgclassdist->SetValue(false);
      minclassdist->SetValue(true);
      ordercompute->SetValue(false);
   } else {
      avgclassdist->SetValue(false);
      minclassdist->SetValue(false);
      ordercompute->SetValue(true);
   }

   return true;
}

bool CMFeatureSelectionListDialog::TransferDataFromWindow() {
   SInt16 returnCode = 0;
   
   wxRadioButton* avgclassdist = (wxRadioButton*) FindWindow(IDC_AverageClassDistance);
   wxRadioButton* minclassdist = (wxRadioButton*) FindWindow(IDC_MinimumClassDistance);
   wxRadioButton* ordercompute = (wxRadioButton*) FindWindow(IDC_OrderComputed);

   wxCheckBox* thresholdgroup = (wxCheckBox*) FindWindow(IDC_ClassGroupThresholdTable);
   wxCheckBox* thresholdclass = (wxCheckBox*) FindWindow(IDC_ClassPairThresholdTable);
   wxCheckBox* seperatetabe = (wxCheckBox*) FindWindow(IDC_SeparabilityTable);
   wxCheckBox* listclasspair = (wxCheckBox*) FindWindow(IDC_ListClassPairDistances);

   wxTextCtrl* classgroupthres = (wxTextCtrl*) FindWindow(IDC_ClassGroupThreshold);
   wxTextCtrl* classpairthres = (wxTextCtrl*) FindWindow(IDC_ClassPairThreshold);
   wxTextCtrl* mindistrange = (wxTextCtrl*) FindWindow(IDC_MinDistanceRange);
   wxTextCtrl* maxdistrange = (wxTextCtrl*) FindWindow(IDC_MaxDistanceRange);
   wxTextCtrl* numberbest = (wxTextCtrl*) FindWindow(IDC_NumberBestCombinations);

   m_thresholdedGroupTableFlag = thresholdgroup->GetValue();
   m_thresholdedClassPairTableFlag = thresholdclass->GetValue();
   m_separabilityTableFlag = seperatetabe->GetValue();
   m_listClassPairDistancesFlag = listclasspair->GetValue();

   wxString classgroupthresString = classgroupthres->GetValue();
   classgroupthresString.ToDouble(&m_localDistancesLessThanToGroup);
   wxString classpairthresString = classpairthres->GetValue();
   classpairthresString.ToDouble(&m_localDistancesLessThanToList);
   m_mindistrangeString = mindistrange->GetValue();
   m_mindistrangeString.ToDouble(&m_localMinDistanceRangeToList);
   m_maxdistrangeString = maxdistrange->GetValue();
   m_maxdistrangeString.ToDouble(&m_localMaxDistanceRangeToList);
   m_numberbestString = numberbest->GetValue();
   m_localCombinationsToList = wxAtoi(m_numberbestString);

   if (avgclassdist->GetValue() == true) {
      m_sortChannelCombinations = 0;
   } else if (minclassdist->GetValue() == true) {
      m_sortChannelCombinations = 1;
   } else {
      m_sortChannelCombinations = 2;
   }

   returnCode = CheckDialogRealValue (this,
													IDC_ClassGroupThreshold,
													0,       // minValue
													32767,   // maxValue
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
    
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
													IDC_ClassPairThreshold,
													0,       // minValue
													32767,   // maxValue
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);  
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
													IDC_MinDistanceRange,
													0,       // minValue
													32767,   // maxValue
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);  
   
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
													IDC_MaxDistanceRange,
													0,       // minValue
													32767,   // maxValue
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);  
   
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
													IDC_NumberBestCombinations,
													1,       // minValue
													32767,   // maxValue
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);  
      
   return (returnCode == 0);

}

//BEGIN_MESSAGE_MAP(CMFeatureSelectionListDialog, CMDialog)
////{{AFX_MSG_MAP(CMFeatureSelectionListDialog)
////}}AFX_MSG_MAP
//END_MESSAGE_MAP()

BEGIN_EVENT_TABLE(CMFeatureSelectionListDialog, CMDialog)
EVT_INIT_DIALOG(CMFeatureSelectionListDialog::OnInitDialog)
EVT_BUTTON(IDS_HelpButton , CMFeatureSelectionListDialog::OnHelpButton)
END_EVENT_TABLE()
//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the feature
//							selection list dialog box to the user and copy the
//							revised back to the feature selection specification structure 
//							if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			 
//
//	Coded By:			Larry L. Biehl			Date: 05/06/98
//	Revised By:			Larry L. Biehl			Date: 05/06/98	

Boolean
CMFeatureSelectionListDialog::DoDialog(
   SeparabilitySpecsPtr separabilitySpecsPtr,
   SInt32* combinationsToListPtr) {
   SInt16 returnCode;

   Boolean continueFlag = FALSE;


   // Make sure intialization has been completed.

   if (!m_initializedFlag)
      return (FALSE);

   m_separabilitySpecsPtr = separabilitySpecsPtr;
   m_combinationsToListPtr = combinationsToListPtr;

   returnCode = ShowModal();

   if (returnCode == wxID_OK) {
      continueFlag = TRUE;

      // Adjust sort channel combinations variable to be 1 based.  

      m_sortChannelCombinations++;

      SeparabilityListDialogOK(m_separabilitySpecsPtr,
         m_combinationsToListPtr,
         m_separabilityTableFlag,
         m_localCombinationsToList,
         (float) m_localMinDistanceRangeToList,
         (float) m_localMaxDistanceRangeToList,
         (m_sortChannelCombinations == 1),
         (m_sortChannelCombinations == 2),
         (m_sortChannelCombinations == 0),
         m_thresholdedClassPairTableFlag,
         m_localDistancesLessThanToList,
         m_thresholdedGroupTableFlag,
         m_localDistancesLessThanToGroup,
         m_listClassPairDistancesFlag);

   } // end "if (returnCode == IDOK)"

   return (continueFlag);

} // end "DoDialog" 


/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionListDialog message handlers

void CMFeatureSelectionListDialog::OnInitDialog(wxInitDialogEvent& event) {
   SInt16 sortChannelCombinations;


   //CDialog::OnInitDialog();

   // Initialize dialog variables.

   SeparabilityListDialogInitialize(this,
      m_separabilitySpecsPtr,
      *m_combinationsToListPtr,
      (Boolean*) & m_separabilityTableFlag,
      &m_localCombinationsToList,
      &m_localMinDistanceRangeToList,
      &m_localMaxDistanceRangeToList,
      &sortChannelCombinations,
      (Boolean*) & m_thresholdedClassPairTableFlag,
      &m_localDistancesLessThanToList,
      (Boolean*) & m_thresholdedGroupTableFlag,
      &m_localDistancesLessThanToGroup,
      (Boolean*) & m_listClassPairDistancesFlag);

   // Adjust sort channel combinations variable to be 0 based.

   m_sortChannelCombinations = sortChannelCombinations - 1;

   if (TransferDataToWindow())
      PositionDialogWindow();

   // Set default text selection to first edit text item	

   //	GetDlgItem(IDC_NumberBestCombinations)->SetFocus();
   //	SendDlgItemMessage( IDC_NumberBestCombinations, EM_SETSEL, 0, MAKELPARAM(0, -1) );
   SelectDialogItemText(this, IDC_NumberBestCombinations, 0, SInt16_MAX);
   this->Layout();
   this->Fit();
   //   return FALSE; // return TRUE  unless you set the focus to a control

} // end "OnInitDialog"

void CMFeatureSelectionListDialog::OnHelpButton(wxCommandEvent& event)
{
//   wxMessageBox(wxT("Information... "), wxT("Help"), wxOK| wxSTAY_ON_TOP, this);
//   DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_DialogMsg0, 0, NULL);
}

void CMFeatureSelectionListDialog::CreateControls() {
   this->SetSizeHints(wxDefaultSize, wxDefaultSize);
   //wxBoxSizer* bSizer226;
   bSizer226 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer227;
   bSizer227 = new wxBoxSizer(wxVERTICAL);

   m_checkBox53 = new wxCheckBox(this, IDC_SeparabilityTable, wxT("Separability table with:"), wxDefaultPosition, wxDefaultSize, 0);
//   SetUpToolTip(m_checkBox53, IDS_ToolTip161);
   bSizer227->Add(m_checkBox53, 0, wxALL, 5);

   wxBoxSizer* bSizer231;
   bSizer231 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText230 = new wxStaticText(this, wxID_ANY, wxT("Best"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText230->Wrap(-1);
   bSizer231->Add(m_staticText230, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl124 = new wxTextCtrl(this, IDC_NumberBestCombinations, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0);
   m_textCtrl124->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_numberbestString));
   SetUpToolTip(m_textCtrl124, IDS_ToolTip161);
   bSizer231->Add(m_textCtrl124, 0, wxALL, 5);

   m_staticText231 = new wxStaticText(this, IDC_PossibleCombinations, wxT("out of 9999999999 possible combinations"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText231->Wrap(-1);
   bSizer231->Add(m_staticText231, 0, wxALIGN_CENTER | wxALL, 5);


   bSizer227->Add(bSizer231, 0, wxEXPAND | wxLEFT, 30);

   wxBoxSizer* bSizer232;
   bSizer232 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText232 = new wxStaticText(this, wxID_ANY, wxT("Distance range:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText232->Wrap(-1);
   bSizer232->Add(m_staticText232, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl125 = new wxTextCtrl(this, IDC_MinDistanceRange, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_textCtrl125->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_mindistrangeString));
   SetUpToolTip(m_textCtrl125, IDS_ToolTip169);
   bSizer232->Add(m_textCtrl125, 0, wxALL, 5);

   m_staticText233 = new wxStaticText(this, wxID_ANY, wxT("to"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText233->Wrap(-1);
   bSizer232->Add(m_staticText233, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl126 = new wxTextCtrl(this, IDC_MaxDistanceRange, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_textCtrl126->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_maxdistrangeString));
   SetUpToolTip(m_textCtrl126, IDS_ToolTip170);
   bSizer232->Add(m_textCtrl126, 0, wxALL, 5);


   bSizer227->Add(bSizer232, 0, wxEXPAND | wxLEFT, 30);

   wxBoxSizer* bSizer233;
   bSizer233 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText234 = new wxStaticText(this, wxID_ANY, wxT("Sort channel combinations by:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText234->Wrap(-1);
   bSizer233->Add(m_staticText234, 0, wxALL, 5);

   wxBoxSizer* bSizer239;
   bSizer239 = new wxBoxSizer(wxVERTICAL);

   m_radioBtn24 = new wxRadioButton(this, IDC_AverageClassDistance, wxT("Average class distance"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn24, IDS_ToolTip171);
   bSizer239->Add(m_radioBtn24, 0, wxLEFT | wxTOP, 5);

   m_radioBtn25 = new wxRadioButton(this, IDC_MinimumClassDistance, wxT("Minimum class distance"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn25, IDS_ToolTip172);
   bSizer239->Add(m_radioBtn25, 0, wxLEFT | wxTOP, 5);

   m_radioBtn26 = new wxRadioButton(this, IDC_OrderComputed, wxT("Order computed"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn26, IDS_ToolTip173);
   bSizer239->Add(m_radioBtn26, 0, wxLEFT | wxTOP, 5);


   bSizer233->Add(bSizer239, 0, wxEXPAND, 5);


   bSizer227->Add(bSizer233, 0, wxEXPAND | wxLEFT, 30);

   wxBoxSizer* bSizer234;
   bSizer234 = new wxBoxSizer(wxHORIZONTAL);

   wxBoxSizer* bSizer235;
   bSizer235 = new wxBoxSizer(wxVERTICAL);

   m_checkBox54 = new wxCheckBox(this, IDC_ListClassPairDistances, wxT("List class pair distances"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox54, IDS_ToolTip174);
   bSizer235->Add(m_checkBox54, 0, wxALL, 5);


   bSizer234->Add(bSizer235, 1, wxEXPAND, 5);

   wxBoxSizer* bSizer236;
   bSizer236 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer237;
   bSizer237 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText235 = new wxStaticText(this, IDC_IncludeChannelComboPrompt, wxT("Include channel combinations"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText235->Wrap(-1);
   //bSizer237->Add(m_staticText235, 0, wxALIGN_LEFT | wxALL, 5);
   bSizer237->Add(m_staticText235, 0, wxALIGN_CENTER | wxALL, 5);
   m_staticText237 = new wxStaticText(this, IDC_IncludeChannelCombo, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_staticText237->Wrap(-1);
   //bSizer237->Add(m_staticText237, 0, wxALIGN_LEFT | wxALL, 5);
   bSizer237->Add(m_staticText237, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer236->Add(bSizer237, 1, wxEXPAND, 5);

   wxBoxSizer* bSizer238;
   bSizer238 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText236 = new wxStaticText(this, IDC_ExcludeChannelComboPrompt, wxT("Exclude channel combinations"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText236->Wrap(-1);
   //bSizer238->Add(m_staticText236, 0, wxALIGN_LEFT | wxALL, 5);
   bSizer238->Add(m_staticText236, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText238 = new wxStaticText(this, IDC_ExcludeChannelCombo, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_staticText238->Wrap(-1);
   //bSizer238->Add(m_staticText238, 0, wxALIGN_CENTER | wxALIGN_LEFT | wxALL, 5);
   bSizer238->Add(m_staticText238, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer236->Add(bSizer238, 1, wxEXPAND, 5);
   bSizer234->Add(bSizer236, 1, wxEXPAND, 5);
   bSizer227->Add(bSizer234, 0, wxEXPAND | wxLEFT, 30);
   //bSizer226->Add(bSizer227, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 12);
   bSizer226->Add(bSizer227, 0, wxEXPAND | wxLEFT | wxTOP, 12);

   wxBoxSizer* bSizer228;
   bSizer228 = new wxBoxSizer(wxHORIZONTAL);

   m_checkBox51 = new wxCheckBox(this, IDC_ClassPairThresholdTable, wxT("Table of class pairs with distance < :"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer228->Add(m_checkBox51, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl122 = new wxTextCtrl(this, IDC_ClassPairThreshold, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   bSizer228->Add(m_textCtrl122, 0, wxALL, 5);


   //bSizer226->Add(bSizer228, 0, wxALIGN_CENTER | wxEXPAND | wxLEFT | wxRIGHT, 12);
   bSizer226->Add(bSizer228, 0,  wxLEFT | wxEXPAND, 12);
   wxBoxSizer* bSizer229;
   bSizer229 = new wxBoxSizer(wxHORIZONTAL);

   m_checkBox52 = new wxCheckBox(this, IDC_ClassGroupThresholdTable, wxT("Class grouping table with threshold of:"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer229->Add(m_checkBox52, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl123 = new wxTextCtrl(this, IDC_ClassGroupThreshold, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   bSizer229->Add(m_textCtrl123, 0, wxALIGN_CENTER | wxALL, 5);


   //bSizer226->Add(bSizer229, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);
   bSizer226->Add(bSizer229, 0, wxEXPAND | wxLEFT, 12);
   wxBoxSizer* bSizer230;
   bSizer230 = new wxBoxSizer(wxHORIZONTAL);

//   wxContextHelpButton* helpButton = new wxContextHelpButton(this, IDS_HelpButton , wxDefaultPosition, wxDefaultSize, 0);

//   bSizer230->Add(helpButton, 0, wxALIGN_CENTER|wxALL, 5);
      
   m_button49 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer230->Add(m_button49, 0, wxALL, 5);

   m_button50 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer230->Add(m_button50, 0, wxALL, 5);


   //bSizer226->Add(bSizer230, 0, wxALIGN_RIGHT | wxBOTTOM | wxLEFT | wxRIGHT, 12);
   bSizer226->Add(bSizer230, 0, wxALIGN_CENTER | wxBOTTOM | wxLEFT, 12);

   this->SetSizer(bSizer226);
   this->Layout();

   this->Centre(wxBOTH);
}
