//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						xDialog.h
//	Implementation:		xDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/21/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMDialog class
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "wx/dialog.h"
#include "wx/combobox.h"

#define kChannelsMenu					1
#define kClassesMenu						2
#define kWeightsMenu						3
#define kClassPairWeightsMenu			4
#define kChannelsPossibleMenu			5
#define kChannelsAvailableMenu		6
#define kClassAreasMenu					7
#define kPaletteMenu						8

//typedef long unsigned int 	UINT;


class CMDialog : public wxDialog
{
   		// Construction
	public:
   	CMDialog ();
   	CMDialog (
				wxWindowID							id,
				wxWindow* 							pParent,
				const wxString& 					title); // standard constructor
	
   	~CMDialog (); // destructor stub
   
		SInt16 GetComboListSelection (
				UInt16 								dialogItemNumber,
				SInt32 								selectedListItemData);
	
		SInt16 GetComboListSelection2 (
				wxWindow* 							cWindowPtr,
				UInt16 								dialogItemNumber,
				SInt32 								selectedListItemData);
	
		void HideShowAreaItems (
				Boolean 								imageAreaFlag);
		
		void LoadDItemString (
				UInt16 								itemNumber,
				Str255* 								theStringPtr);
		
		void LoadDItemString (
				UInt16 								itemNumber,
				CharPtr 								theStringPtr);
	
		void LoadDItemValue (
				UInt16 								itemNumber,
				SInt32 								value);
	
		void MHideDialogItem (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);
	
		void MEnableDialogItem (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);
	
		void MDisableDialogItem (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);
	
		void MShowDialogItem (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);
	
		void SetChoiceItemText (
				UInt16								dialogItemNumber,
				SInt16								comboItem,
				CharPtr								stringPtr);
	
		void SetComboItemText (
				UInt16 								dialogItemNumber,
				SInt16 								comboItem,
				CharPtr 								stringPtr);
	
		void SetEntireImageButtons (
				DialogPtr 							dialogPtr,
				SInt32 								lineStart,
				SInt32 								lineEnd,
				SInt32 								columnStart,
				SInt32 								columnEnd);
	
		void SetMaxNumberLinesAndColumns (
				UInt32 								maxNumberLines,
				UInt32 								maxNumberColumns);
	
		void SetStaticTextOrDropDownList (
				SInt16             				dropDownListSelection,
				SInt16              				dropDownListID,
				SInt16              				staticTextID,
				Boolean             				deactivatedFlag);
	
		// Implementation
	protected:
		void CheckColumnEnd (
				wxCommandEvent&					event);

		void CheckColumnInterval (
				wxCommandEvent&					event);

		void CheckColumnStart (
				wxCommandEvent&					event);

		void CheckLineEnd (
				wxCommandEvent&					event);

		void CheckLineInterval (
				wxCommandEvent&					event);

		void CheckLineStart (
				wxCommandEvent&					event);

		void OnCharHook (
				wxKeyEvent& 						event);
	
		void OnChannelComboCloseUp (
				wxCommandEvent&					event);

		void OnChannelsSubset (
				wxCommandEvent&					event);

		void OnClassComboCloseUp (
				wxCommandEvent&					event);

		void OnClassPairWeightsComboCloseUp (
				wxCommandEvent&					event);

		void OnClassComboSelendok (
				wxCommandEvent&					event);

		void OnClassComboDropDown (
				wxCommandEvent&					event);

		void OnClassWeightComboCloseUp (
				wxCommandEvent&					event);

		void OnClassWeightComboDropDown (
				wxCommandEvent&					event);

		void OnChannelComboDropDown (
				wxCommandEvent&					event);

		void OnClassPairWeightsComboDropDown (
				wxCommandEvent&					event);

		void OnLButtonDown (
				wxMouseEvent& 						event);
	
		void ToEntireImage (
				wxCommandEvent&					event);

		void ToSelectedImage (
				wxCommandEvent&					event);

		DECLARE_EVENT_TABLE ();
	
		SInt32 CheckValue (
				int                  			controlID,
				SInt32               			minValue,
				SInt32               			maxValue);
	
		void CreateLineColumnControls (
				wxStaticBoxSizer*             sbSizer);
		
		void CreateStandardButtons (
				wxBoxSizer* 						bVMainSizer);
		
		void GetAllSubsetMenuControl (
				int									controlCode,
				wxWindow*							parentWindow,
				UInt16								controlItemNumber,
				int									horizontalSize,
				int									toolTipStringNumber);
	
		void GetEqualUnequalMenuControl (
				int									controlCode,
				wxWindow*							parentWindow,
				UInt16								controlItemNumber,
				int									horizontalSize,
				int									toolTipStringNumber);
	
		void GetPaletteMenuControl (
				int									controlCode,
				wxWindow*							parentWindow,
				UInt16								controlItemNumber,
				int									horizontalSize,
				int									toolTipStringNumber);
	
		void HandleChannelsMenu (
				UInt16 								channelsMenuItemNumber,
				Boolean 								tranformationFlag,
				SInt16 								totalNumberChannels,
				SInt16 								channelMenuType,
				Boolean 								okFlagSetting);
	
		void HandleClassesMenu (
				UInt32* 								numberOutputClassesPtr,
				SInt16* 								classListPtr,
				SInt16 								minimumNumberClasses,
				SInt16 								numberInputClasses,
				UInt16 								classesMenuItemNumber,
				int* 									classMenuSelectionPtr);
	
		void HandleClassPairWeightsMenu (
				SInt16** 							weightsListPtrPtr,
				UInt16 								classPairWeightsMenuItemNumber,
				int* 									classPairWeightsMenuSelectionPtr,
				SInt16* 								defaultClassPairWeightPtr);
	
		void HandleClassWeightsMenu (
				UInt16 								numberOutputClassesToUse,
				SInt16* 								classListPtr,
				float* 								weightsListPtr,
				Boolean 								useEnhancedStatFlag,
				UInt16 								classWeightsMenuItemNumber,
				int* 									classWeightsMenuSelectionPtr);
	
		void HideSomeAreaSelectionItems (void);
	
		void OnFeatureTransformation (void);
		
		void PositionDialogWindow (void);
		
		void SetDLogControl (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber,
				SInt16 								setting);
	
		void SetDLogControlHilite (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber,
				SInt16 								setting);
	
		void ShowHideDialogItem (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber,
				Boolean 								showFlag);
	
		void ShowSomeAreaSelectionItems ();
		
		void UpdateAllSubsetList (
				UInt16 								channelsMenuItemNumber);
		
		UInt16 UpdateOneColListSelections (
				UInt16 								numberInputVecItems,
				UInt16 								listStart,
				UInt16 								listEnd,
				UInt16 								listInterval);
	
		SInt16 VerifyLineColumnStartEndValues ();
		
		SInt16 VerifyLineColumnValues (
				SInt16               			startLineItem,
				SInt16               			startColumnItem,
				Boolean              			checkIntervalFlag);
	
		void TransferLinesColumnsFromWindow (void);
		
		void TransferLinesColumnsToWindow (void);
	
	
		DialogSelectArea 					m_dialogSelectArea;
	
				// Following used when verifying the line/column values
		wxString   							m_ColumnEndString,
												m_ColumnIntervalString,
												m_ColumnStartString,
												m_LineEndString,
												m_LineIntervalString,
												m_LineStartString,
	
					 							m_value,
												m_stringCheck;

		Ptr 									m_valuePtr;
	
		UInt16 								*m_availableFeaturePtr,
												*m_classListPtr,
												*m_localActiveFeaturesPtr,
												*m_localFeaturesPtr,
												*m_localTransformFeaturesPtr;
	
		unsigned char 						*m_localSymbolsPtr;
	
		wxBitmapButton						*m_bpButton4,
												*m_bpButton5;
		
		wxButton								*m_hiddenChannelSubset;
		
		wxPanel								*m_dialogPanel;
		
		wxTextCtrl							*m_colendctrl,
												*m_colintctrl,
												*m_colstartctrl,
												*m_lineendctrl,
												*m_lineintctrl,
												*m_linestartctrl;
		
		#if defined multispec_wxlin
			wxComboBox							*m_channelsCtrl,
													*m_channelsPossibleCtrl,
													*m_classesCtrl,
													*m_classAreasCtrl,
													*m_classPairWeightsCtrl,
													*m_paletteCtrl,
													*m_weightsCtrl;
		#endif
		#if defined multispec_wxmac
			wxChoice								*m_channelsCtrl,
													*m_channelsPossibleCtrl,
													*m_classesCtrl,
													*m_classAreasCtrl,
													*m_classPairWeightsCtrl,
													*m_paletteCtrl,
													*m_weightsCtrl;
		#endif

		double 								m_doubleValueCheck,
												m_double2DigitValueCheck,
												m_double4DigitValueCheck,
												m_double6DigitValueCheck,
												m_double8DigitValueCheck,
												m_double10DigitValueCheck;
	
		int 									m_channelSelection,
												m_classSelection,
												m_symbolSelection,
												m_classWeightsSelection,
												m_classPairWeightsSelection;
	
		SInt32 								m_ColumnEnd,
												m_ColumnInterval,
												m_ColumnStart,
												m_LineEnd,
												m_LineInterval,
												m_LineStart,

												m_maxNumberColumns,
												m_maxNumberLines;
	
		UInt32 								m_localNumberClasses;
		
		SInt16 								m_channelListType,
												m_outputStorageType;
		
		UInt16 								m_localActiveNumberFeatures,
												m_localNumberFeatures,
												m_localNumberTransformFeatures,
	
												m_numberEigenvectors;
		
		bool 									m_settingSelectedEntireButton;
		
		Boolean 								m_entireImageFlag,
												m_featureTransformAllowedFlag,
												m_featureTransformationFlag,
												m_initializedFlag;

};	// end "class CMDialog"
