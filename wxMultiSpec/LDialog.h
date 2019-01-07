//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDialog.h
//	Implementation:		LDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2018
//
//	Language:				C++
//
//	System:					Linux Operating Systems
//
//	Brief description:	This file is the definition for the CMDialog class
//
//------------------------------------------------------------------------------------

#if !defined __MDIALOG_H__
#define __MDIALOG_H__
#include "SMultiSpec.h"
#include "wx/dialog.h"
#include "wx/combobox.h"

/////////////////////////////////////////////////////////////////////////////
// CMDialog dialog

class CMDialog : public wxDialog
	{
   // Construction
	public:
   	CMDialog();
   	CMDialog (wxWindowID id, wxWindow* pParent, const wxString& title); // standard constructor
   
   	~CMDialog(); // destructor stub
   
		SInt16 GetComboListSelection (
						UInt16 						dialogItemNumber,
						SInt32 						selectedListItemData);
						
   SInt16 GetComboListSelection2(
                                 wxWindow* cWindowPtr,
                                 UInt16 dialogItemNumber,
                                 SInt32 selectedListItemData);
   
   void HideShowAreaItems(
                          Boolean imageAreaFlag);
   
   void LoadDItemString(
                        UInt16 itemNumber,
                        Str255* theStringPtr);
   
   void LoadDItemString(
                        UInt16 itemNumber,
                        CharPtr theStringPtr);
   
   void LoadDItemValue(
                       UInt16 itemNumber,
                       SInt32 value);
   
   void MHideDialogItem(
                        DialogPtr dialogPtr,
                        SInt16 itemNumber);
   
   void MEnableDialogItem(
                          DialogPtr dialogPtr,
                          SInt16 itemNumber);
   
   void MDisableDialogItem(
                           DialogPtr dialogPtr,
                           SInt16 itemNumber);
   
   void MShowDialogItem(
                        DialogPtr dialogPtr,
                        SInt16 itemNumber);
   
   void SetComboItemText(
                         UInt16 dialogItemNumber,
                         SInt16 comboItem,
                         CharPtr stringPtr);
   
   void SetEntireImageButtons(
                              DialogPtr dialogPtr,
                              SInt32 lineStart,
                              SInt32 lineEnd,
                              SInt32 columnStart,
                              SInt32 columnEnd);
   
   void SetMaxNumberLinesAndColumns(
                                    UInt32 maxNumberLines,
                                    UInt32 maxNumberColumns);
   
   void SetStaticTextOrDropDownList (
                                     SInt16               dropDownListSelection,
                                     SInt16               dropDownListID,
                                     SInt16               staticTextID,
                                     Boolean               deactivatedFlag);
   
   // Implementation
protected:
   
   void OnCharHook (wxKeyEvent& event);
   void ResetComboBox (int controlID);
   //void ResetClassComboBox(int controlID);
   DECLARE_EVENT_TABLE();
   
   void CheckColumnEnd(wxCommandEvent& event);
   
   void CheckColumnInterval(wxCommandEvent& event);
   
   void CheckColumnStart(wxCommandEvent& event);
   
   SInt32 CheckValue(
                     int                  controlID,
                     SInt32               minValue,
                     SInt32               maxValue);
   
   void CheckLineEnd(wxCommandEvent& event);
   
   void CheckLineInterval(wxCommandEvent& event);
   
   void CheckLineStart(wxCommandEvent& event);
   
   void CreateLineColumnControls (
					wxStaticBoxSizer*               		sbSizer);
	
	void CreateStandardButtons (
 					wxBoxSizer* 								bVMainSizer);
   
   void HandleChannelsMenu(
                           UInt16 channelsMenuItemNumber,
                           Boolean tranformationFlag,
                           SInt16 totalNumberChannels,
                           SInt16 channelMenuType,
                           Boolean okFlagSetting);
   
   void HandleClassesMenu(
                          UInt32* numberOutputClassesPtr,
                          SInt16* classListPtr,
                          SInt16 minimumNumberClasses,
                          SInt16 numberInputClasses,
                          UInt16 classesMenuItemNumber,
                          int* classMenuSelectionPtr);
   
   void HandleClassPairWeightsMenu(
                                   SInt16** weightsListPtrPtr,
                                   UInt16 classPairWeightsMenuItemNumber,
                                   int* classPairWeightsMenuSelectionPtr,
                                   SInt16* defaultClassPairWeightPtr);
   
   void HandleClassWeightsMenu(
                               UInt16 numberOutputClassesToUse,
                               SInt16* classListPtr,
                               float* weightsListPtr,
                               Boolean useEnhancedStatFlag,
                               UInt16 classWeightsMenuItemNumber,
                               int* classWeightsMenuSelectionPtr);
   
   void HideSomeAreaSelectionItems();
   
   //   void               LoadLineColumnItems (
   //                        DialogSelectAreaPtr   dialogSelectAreaPtr,
   //                        DialogPtr            dialogPtr);
   
   void OnSelendokClassCombo(wxCommandEvent& event);
   void OnSelendokClassComboDropDown(wxCommandEvent& event);
   void OnSelendokAreasComboDropDown(wxCommandEvent& event);
   void OnSelendokClassWeightsComboDropDown(wxCommandEvent& event);
   void OnSelendokChannelComboDropDown(wxCommandEvent& event);
   void OnSelendokClassPairWeightsComboDropDown(wxCommandEvent& event);
   
   void OnFeatureTransformation(void);
   
   void PositionDialogWindow(void);
   
   void SetDLogControl(
                       DialogPtr dialogPtr,
                       SInt16 itemNumber,
                       SInt16 setting);
   
   void SetDLogControlHilite(
                             DialogPtr dialogPtr,
                             SInt16 itemNumber,
                             SInt16 setting);
   
   void ShowHideDialogItem(
                           DialogPtr dialogPtr,
                           SInt16 itemNumber,
                           Boolean showFlag);
   
   void ShowSomeAreaSelectionItems();
   
   void UpdateAllSubsetList(
                            UInt16 channelsMenuItemNumber);
   
   UInt16 UpdateOneColListSelections(
                                     UInt16 numberInputVecItems,
                                     UInt16 listStart,
                                     UInt16 listEnd,
                                     UInt16 listInterval);
   
   SInt16 VerifyLineColumnStartEndValues();
   
   SInt16 VerifyLineColumnValues(
                                 SInt16               startLineItem,
                                 SInt16               startColumnItem,
                                 Boolean               checkIntervalFlag);
   
   void ToEntireImage(wxCommandEvent& event);
   
   void ToSelectedImage(wxCommandEvent& event);
	
	

	wxBitmapButton				*m_bpButton4,
									*m_bpButton5;
	
	wxPanel*			m_dialogPanel;
	
   wxString m_value;
   
   		// Following used when verifying the line/column values
	
   wxString   m_LineStartString;
   wxString   m_LineEndString;
   wxString   m_LineIntervalString;
   wxString   m_ColumnStartString;
   wxString   m_ColumnEndString;
   wxString   m_ColumnIntervalString;
   wxString   m_stringCheck;
   
   Ptr m_valuePtr;
   
   DialogSelectArea m_dialogSelectArea;
   
   Boolean m_featureTransformationFlag;
   
   Boolean m_entireImageFlag,
   m_featureTransformAllowedFlag,
   m_initializedFlag;
   
   /*CDataExchange      *m_dialogFromPtr,
    *m_dialogToPtr;*/
   
   unsigned char *m_localSymbolsPtr;
   
   UInt16 *m_availableFeaturePtr,
   *m_classListPtr,
   *m_localActiveFeaturesPtr,
   *m_localFeaturesPtr,
   *m_localTransformFeaturesPtr;
   
   //   WindowInfoPtr      m_imageWindowInfoPtr;
   
   double m_doubleValueCheck;
   
   int m_channelSelection;
   int m_classSelection;
   int m_symbolSelection;
   
   int m_classSelection_Saved;
   int m_channelSelection_Saved;
   int m_weightSelection_Saved;
   int m_areaSelection_Saved;
   int m_interClassWeightsSelection_Saved;
   
   SInt32 m_ColumnEnd;
   SInt32 m_ColumnInterval;
   SInt32 m_ColumnStart;
   SInt32 m_LineEnd;
   SInt32 m_LineInterval;
   SInt32 m_LineStart;
   
   SInt32 m_maxNumberColumns;
   SInt32 m_maxNumberLines;
   
   UInt32 m_localNumberClasses;
   
   SInt16 m_channelListType,
   m_outputStorageType;
   
   UInt16 m_localActiveNumberFeatures,
   m_localNumberFeatures,
   m_localNumberTransformFeatures;
   
   UInt16 m_numberEigenvectors;
   
   bool m_settingSelectedEntireButton;
	
	wxTextCtrl							*m_colendctrl,
											*m_colintctrl,
											*m_colstartctrl,
											*m_lineendctrl,
											*m_lineintctrl,
											*m_linestartctrl;
   
};

#endif // !defined __MDIALOG_H__

