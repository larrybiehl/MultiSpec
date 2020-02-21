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
//	File:						WDialog.h
//	Implementation:		WDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 03/18/2017
//
//------------------------------------------------------------------------------------

#pragma once
	
#include <afxext.h>


class CMDialog : public CDialog
{
	// Construction
	public:
		CMDialog (
				UINT 									IDD,
				CWnd* 								pParent);	// standard constructor
	
		virtual ~CMDialog ();		// destructor stub
	
		SInt16 GetComboListSelection (
				UInt16								dialogItemNumber,
				SInt32								selectedListItemData);
	
		SInt32 GetDialogItemValue (
				SInt16								itemNumber);
	
		void HideShowAreaItems (
				Boolean								imageAreaFlag);
	
		void LoadDItemString (
				UInt16								itemNumber,
				Str255* 								theStringPtr,
				Boolean								wideCharInputStringFlag=false);
	
		void LoadDItemString (
				UInt16								itemNumber,
				CharPtr 								theStringPtr,
				Boolean								wideCharInputStringFlag=false);
	
		void LoadDItemValue (
				UInt16								itemNumber,
				SInt32 								value);
	
		void MHideDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);
	
		void MShowDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);
	
		void SetComboItemText (
				UInt16								dialogItemNumber,
				SInt16								comboItem,
				UCharPtr								stringPtr,
				UInt16								stringCharCode=kASCIICharString);
	
		void SetEntireImageButtons (
				DialogPtr							dialogPtr,
				SInt32								lineStart,
				SInt32								lineEnd,
				SInt32								columnStart,
				SInt32								columnEnd);
	
		void SetMaxNumberLinesAndColumns (
				UInt32								maxNumberLines,
				UInt32								maxNumberColumns);

		void SetStaticTextOrDropDownList (
				SInt16								dropDownListSelection,
				SInt16								dropDownListID,
				SInt16								staticTextID,
				Boolean								deactivatedFlag);
	
	// Implementation
	protected:
		void CheckColumnEnd (void);
	
		void CheckColumnInterval (void);
	
		void CheckColumnStart (void);
	
		void CheckLineEnd (void);
	
		void CheckLineInterval (void);
	
		void CheckLineStart (void);
	
		Boolean CheckLocalValue (
				SInt16								itemNumber,
				UINT*									numberValuePtr);
	
		void HandleChannelsMenu (
				UInt16								channelsMenuItemNumber,
				Boolean								tranformationFlag,
				SInt16								totalNumberChannels,
				SInt16								channelMenuType,
				Boolean								okFlagSetting);
	
		void HandleClassesMenu (
				UInt32*								numberOutputClassesPtr,
				SInt16*								classListPtr,
				SInt16								minimumNumberClasses,
				SInt16								numberInputClasses,
				UInt16								classesMenuItemNumber,
				int*									classMenuSelectionPtr);

		void HandleClassPairWeightsMenu (
				SInt16**	 							weightsListPtrPtr,
				UInt16								classPairWeightsMenuItemNumber,
				int*									classPairWeightsMenuSelectionPtr,
				SInt16*								defaultClassPairWeightPtr);
	
		void HandleClassWeightsMenu (
				UInt16								numberOutputClassesToUse,
				SInt16*								classListPtr,
				float*								weightsListPtr,
				Boolean								useEnhancedStatFlag,
				UInt16								classWeightsMenuItemNumber,
				int*									classWeightsMenuSelectionPtr);
	
		void HideSomeAreaSelectionItems ();
	
		void OnSelendokClassCombo (void);
	
		void OnFeatureTransformation (void);
	
		void PositionDialogWindow (void);
	
		void SetDLogControl (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt16								setting);
	
		void SetDLogControlHilite (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt16								setting);
	
		void ShowHideDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				Boolean								showFlag);
	
		void ShowSomeAreaSelectionItems ();
	
		void UpdateAllSubsetList (
				UInt16								channelsMenuItemNumber);
	
		UInt16 UpdateOneColListSelections (
				UInt16								numberInputVecItems,
				UInt16								listStart,
				UInt16								listEnd,
				UInt16								listInterval);

		void VerifyLineColumnStartEndValues (
				CDataExchange* 					pDX);
	
		SInt16 VerifyLineColumnValues (
				CDataExchange* 					pDX,
				SInt16								startLineItem,
				SInt16								startColumnItem,
				SInt16								pixelSize,
				SInt16								displayType,
				SInt16								numberChannels);
	
		void ToEntireImage (void);
	
		void ToSelectedImage (void);
	
		CBitmapButton  					toEntireButton,
			  									toSelectedButton;
	
		CString	 							m_value;
	
		DialogSelectArea					m_dialogSelectArea;
	
		TBYTE*								m_valuePtr;
	
		CDataExchange						*m_dialogFromPtr,
												*m_dialogToPtr;
	
		unsigned char						*m_localSymbolsPtr;
	
		UInt16								*m_availableFeaturePtr,
												*m_classListPtr,
												*m_localActiveFeaturesPtr,
												*m_localFeaturesPtr,
												*m_localTransformFeaturesPtr;
	
		int									m_channelSelection,
												m_classSelection,
												m_symbolSelection;
	
		SInt32								m_ColumnEnd,
												m_ColumnInterval,
												m_ColumnStart,
												m_LineEnd,
												m_LineInterval,
												m_LineStart,
												m_maxNumberColumns,
												m_maxNumberLines;
	
		UInt32								m_localNumberClasses;
	
		SInt16								m_channelListType,
												m_outputStorageType;
	
		UInt16								m_localActiveNumberFeatures,
												m_localNumberFeatures,
												m_localNumberTransformFeatures,
												m_numberEigenvectors;
	
		BOOL									m_featureTransformationFlag;
	
		Boolean								m_entireImageFlag,
												m_featureTransformAllowedFlag,
												m_initializedFlag,
												m_settingSelectedEntireButton;
   
};	// end class CMDialog
