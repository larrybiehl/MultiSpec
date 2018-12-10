//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SRConsnt.h
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	Header file which contains the resource constants used by 
//								MultiSpec.
//
//	Written By:				Larry L. Biehl			Date: 03/29/1988
//	Revised By:				Abdur Maud				Date: 06/24/2013
//	Revised By:				Larry L. Biehl			Date: 11/30/2018
//	
//------------------------------------------------------------------------------------

#ifndef SRCONSNT
#define SRCONSNT

#if defined multispec_mac || defined multispec_mac_swift
	#pragma once

			// resource IDs of windows 														
		
	#define	kOutputWindowID 					131
	#define	kImageWindowID 					132
	#define	kImageCWindowID					133
	#define	kGraphWindowID						134
	#define	kGraphSelectionWindowID 		135
	
			// resource IDs of alerts															
			
	#define	kErrorAlertID						141
	#define	kErrorAlert2ID						142
	#define	kMemoryAlertID						143
	#define	kOpenImageWAlertID				144
	#define	kContinueCancelAlertID			145
	#define	kSaveAlertID						146
	#define	kMemoryWarningAlertID			147
	#define 	kOKCancelAlertID					148
	#define	kYesCancelAlertID					149
	#define	kCoprocessorAlertID				1150
	#define	kIndirectCoprocessorAlertID	1151
	#define	kOKNoSoundAlertID					1152
	#define	kToLargeToCopyAlertID			1153
	#define	kTextWindowLowMemoryAlertID	1154
	#define	kTextWindowResultsAlertID		1156
	#define	kUpdateCancelAlertID				1160
	#define	kFinishDontCancelAlertID		1161
	#define	kRedoDontCancelAlertID			1162
	#define	kOperatingSystemAlertID			1163
	#define	kChangeDontAlertID				1164
	#define	kSaveDontSaveAlertID				1165
	#define	kYesNoAlertID						1166
	#define	kValueRangeAlertID				1167
	#define	kCoreGraphicsAlertID				1168
	#define	kContinueStopAlertID				1169
	
			// resource IDs of menus 															

	#define 	kAppleMenuID								131
	#define 	kFileMenuID									132
	#define 	kEditMenuID									133
	#define 	kWindowMenuID								134
	#define 	kProcessorMenuID							135
	#define 	kPaletteMenuID								136
	#define 	kOptionMenuID								137
	#define 	kColorMenuID								138
	#define	kProjectMenuID								139
	#define	kUtilitiesMenuID							140
	#define 	kReformatMenuID							141
	#define	kClearOverlaysMenuID						142
	
	#define 	kPopUpClassMenuID							151
	#define 	kPopUpAllSubsetMenuID					152
	#define 	kPopUpProjectChangesMenuID				153
	#define 	kPopUpWeightsMenuID						154
	#define	kPopUpPaletteMenuID						155
	#define	kPopUpDiskFileMenuID						156
	#define	kPopUpDisplayTypeMenuID					157
	#define	kPopUpBitsOfColorMenuID					158
	#define	kPopUpFEAlgorithmMenuMenuID			159
	#define	kPopUpSymbolsMenuID						160
	#define	kPopUpAllAvailableSubsetMenuID		161
	#define	kPopUpAllPossibleSubsetMenuID			162
	#define	kPopUpDataValueTypeMenuID				163
	#define	kPopUpBandInterleaveMenuID				164
	#define	kPopUpClusterDiskFileMenuID			165
	#define	kPopUpClassGroupMenuID					166
	#define	kPopUpHeaderOptionsMenuID				167
	#define	kPopUpSaveStatsMenuID					168
	#define	kPopUpOutputFileMenuID					169
	#define	kPopUpChannelDescriptionMenuID		170
	#define	kPopUpSeparabilityDistanceMenuID		171
	#define	kPopUpClassifyProcedureMenuID			172
	#define	kPopUpHistogramMethodMenuID			173
	#define 	kPopUpPreprocessMenuID					174
	#define 	kPopUpClassGroupMenuID2					175
	#define	kPopUpEchoAlgorithmMenuID				176
	#define	kPopUpCovarianceMenuID					177
	#define	kPopUpStatisticsTypeMenuID				178
	#define	kPopUpListDataFormatMenuID				180
	#define	kPopUpSelectMaskImage1MenuID			181
	#define	kPopUpSelectThresholdImageMenuID		181
	#define	kPopUpSelectMaskImage2MenuID			182
	#define	kPopUpCompareParameterMenuID			183 
	#define	kPopUpSoftThresholdMenuID				184 
	#define	kPopUpHardThresholdMenuID				185 
	#define	kPopUpReferenceSystemMenuID			186 
	#define	kPopUpMapUnitsMenuID						187 
	#define	kPopUpEllipsoidMenuID					188 
	#define	kPopUpCoordinateUnitsMenuID			189
	#define	kPopUpAreaUnitsMenuID					190
	#define	kPopUpOverlaysMenuID						191
	#define	kPopUpDatumMenuID							192
	#define	kPopUpStretchMenuID						193
	#define	kPopUpMinMaxMenuID						194
	#define	kPopUpTreatZeroAsMenuID					195
	#define	kPopUpImageOverlayMenuID				196
	#define	kDefaultAreaUnitsMenuID					197
	#define	kPopUpBitsPerDataValueMenuID			198
	#define	kPopUpCollapseClassMenuID				199
	#define	kPopUpSelectionDisplayUnitsMenuID	200 
	#define	kPopUpProjectionMenuID					201
	#define	kPopUpResampleMenuID						202
	#define	kPopUpFunctionMenuID						203
	#define	kPopUpLinkOptionsMenuID					204
	#define	kPopUpDiskFileFormatMenuID				205
	#define	kPopUpMosaicDirectionMenuID			206
	#define	kPopUpGraphAxisLabelMenuID				207
	
		// resource IDs of modal dialogs 													
		
	#define	kFormatDescriptionID 					151
	#define 	kStatEnhanceSpecificationID			152
	#define 	kDisplaySpecificationID					153
	#define 	kHistogramSpecificationID   			154
	#define 	kMemoryID									155
	#define 	kDragGrayRgnID								156
	#define 	kShortStatusInfoID						157
	#define 	kAboutMultiSpecID							158
	#define 	kOverlayParameterID						159
	#define 	kErdasHeaderSpecID						160
	#define 	kChangeImageFormatDialogID				161
	#define 	kStatisticsSpecificationID				162
	#define 	kNewClassFieldDialogID					163
	#define	kClassifySpecificationID				164
	#define	kUpdateStatsInfoID						165
	#define	kChannelSpecificationID					166
	#define	kChannelDescriptionID					167
	#define	kEditCoordinatesID						168
	#define	kPageLayoutSpecificationID				169
	#define	kPrintDlogSpecificationID				265
	#define	kSeparabilitySpecificationID			170
	#define	kClusterSpecificationID					171
	#define	kOnePassClusterSpecificationID		172
	#define	kSepListSpecificationID					173
	#define	kThematicDisplaySpecificationID		174
	#define	kClassifyWeightsDialogID				175
	#define	kStatHistogramSpecificationID			176
	#define	kListDataSpecificationID				177
	#define	kClassPairWeightsDialogID				178
	#define	kSymbolsDialogID							179
	#define	kISODATADialogID							180
	#define	kEvaluateCovarianceSpecificationID 	181
	#define	kGroupsDialogID							182
	#define	kListResultsDialogID						183
	#define	kfieldsToImageID							184
	#define	kPrincipalComponentsID					185
	#define	kGroupNameDialogID						186
	#define	kEchoParametersDialogID					187
	#define	kStatisticsImageDialogID				188
	#define	kAppendFileDialogID						189
	#define	kGraphicStatusDialogID					190
	#define	kChangeImageTransformID					191
	#define	kCoodinateDialogID						192
	#define	kRectifyImageDialogID					193
	#define	kChanDescriptionFileDialogID			194
	#define	kFeatureExtractionDialogID				195
	#define	kMosaicDialogID							196
	#define	kToThematicDialogID						197
	#define	kStatisticsOptionsDialogID				198
	#define	kMinMaxDialogID							199
	#define	kBiPlotDataSpecificationID				200
	#define	kEvaluateTransformSpecificationID	201
	#define	kDecisionTreeDialogID					202
	#define	kProjectionPursuitDialogID 			203
	#define	kMoveableAboutMultiSpecID 				204
	#define 	kFalseColorPaletteDialogID 			205
	#define	kListResultsOptionsDialogID			206
	#define	kCEMParameterDialogID					207
	#define	kSAMParameterDialogID					208
	#define	kStatListSpecificationID				209
	#define	kSetLOOCParameterSpecificationID		210
	#define	kRecodeThematicSpecificationID		211
	
	#define 	kAppDisplaySpecificationID				10153
	
			// resource IDs of patterns 														
		
	#define	kPatternID						161
	#define	kLinePatternID					162
	
			// resource ID of cursors															
			
	#define	kSpinCursor1ID					128
	#define	kHandCursorID					151
	#define	kCrossCursorID					152
	#define	kLegendWidthDownCursorID	153
	#define	kBlinkCursor1ID				154
	#define	kBlinkCursor2ID				155
	#define	kHandClosedCursorID			156
	#define	kLegendWidthCursorID			157
	#define	kCrossCursor2ID				158
	
			// resource ID of strings															
			
	#define	kFileIOStrID					151
	#define 	kHistogramStrID				152
	#define	kReformatStrID					153
	#define	kProjectStrID					154
	#define	kSymbolsStrID					155
	#define	kClassifyStrID					156
	#define	kDialogStrID					157
	#define	kAlertStrID						158
	#define	kListResultsStrID				159
	#define	kProcessorStrID				160
	#define	kProjectVersionsStrID		161
	#define	kGAIAStrID						162
	#define	kFeatureExtractStrID			163
	#define	kFeatureSelectionStrID		164
	#define	kEnhanceStatStrID				165
	#define	kClusterStrID					166
	#define	kUtilitiesStrID				167
	#define	kSharedStrID					169
	#define	kPrincipalCompStrID			170
	#define	kListDataStrID					171
	#define	kStatisticsImageStrID		172
	#define	kFileTypeStrID					175
	#define	kBandInterleaveStrID			176
	#define	kCompressionStrID				177
	#define	kInstrumentNameStrID			178
	#define	kBiPlotStrID					179
	#define	kDisplayStrID					180

			// string indexes
			//		Alert string indexes
			
	
			// Alert strings
			
	#define	IDS_FileSizeLimit				1
	#define	IDS_Alert4						4
	#define	IDS_Alert5						5
	#define	IDS_Alert6						6
	#define	IDS_DisplayColumnLimit		8		// kSDisplayColumnLimit
	#define	IDS_Alert10						10
	#define	IDS_Alert11						11
	#define	IDS_Alert12						12
	#define	kSQD32DisplayColumnLimit	14		// kSQD32DisplayColumnLimit
	#define	IDS_Alert15						15
	#define	IDS_Alert16						16
	#define	IDS_Alert17						17
	#define	IDS_Alert18						18
	#define	IDS_DisplayLineLimit			19		// kSDisplayLineLimit
	#define	IDS_Alert20						20
	#define	IDS_Alert21						21
	#define	IDS_Alert22						22
	#define	IDS_Alert23						23
	#define	IDS_Alert24						24
	#define	IDS_Alert25						25
	#define	IDS_Alert26						26
	#define	IDS_Alert27						27
	#define	IDS_Alert28						28
	#define	IDS_Alert29						29
	#define	IDS_Alert30						30
	#define	IDS_Alert31						31
	#define	IDS_Alert32						32
	#define	IDS_Alert33						33
	#define	IDS_Alert34						34
	#define	IDS_Alert35						35
	#define	IDS_Alert36						36
	#define	IDS_Alert37						37
	#define	IDS_Alert38						38
	#define	IDS_Alert39						39
	#define	IDS_Alert40						40
	#define	IDS_Alert41						41
	#define	IDS_Alert42						42
	#define	IDS_Alert43						43
	#define	IDS_Alert44						44
	#define	IDS_Alert45						45
	#define	IDS_Alert46						46
	#define	IDS_Alert47						47
	#define	IDS_Alert48						48
	#define	IDS_Alert49						49
	#define	IDS_Alert50						50
	#define	IDS_Alert51						51
	#define	IDS_Alert52						52	
	#define	IDS_Alert53						53	
	#define	IDS_Alert54						54	
	#define	IDS_Alert55						55
	#define	IDS_Alert56						56	
	#define	IDS_Alert57						57
	#define	IDS_Alert58						58
	#define	IDS_Alert59						59
	#define	IDS_Alert60						60
	#define	IDS_Alert61						61
	#define	IDS_Alert62						62
	#define	IDS_Alert63						63
	#define	IDS_Alert64						64
	#define	IDS_Alert65						65
	#define	IDS_Alert66						66
	#define	IDS_Alert67						67
	#define	IDS_Alert68						68
	#define	IDS_Alert69						69
	#define	IDS_Alert70						70
	#define	IDS_Alert71						71
	#define	IDS_Alert72						72
	#define	IDS_Alert73						73
	#define	IDS_Alert74						74
	#define	IDS_Alert75						75
	#define	IDS_Alert76						76
	#define	IDS_Alert77						77
	#define	IDS_Alert78						78
	#define	IDS_Alert79						79
	#define	IDS_Alert80						80
	#define	IDS_Alert81						81
	#define	IDS_Alert82						82
	#define	IDS_Alert83						83
	#define	IDS_Alert84						84
	#define	IDS_Alert85						85
	#define	IDS_Alert86						86
	#define	IDS_Alert87						87
	#define	IDS_Alert88						88
	#define	IDS_Alert89						89
	#define	IDS_Alert90						90
	#define	IDS_Alert91						91
	#define	IDS_Alert92						92
	#define	IDS_Alert93						93
	#define	IDS_Alert94						94
	#define	IDS_Alert95						95
	#define	IDS_Alert96						96
	#define	IDS_Alert97						97
	#define	IDS_Alert98						98	
	
	#define	IDS_Alert103					103
	#define	IDS_Alert104					104
	#define	IDS_Alert105					105
	#define	IDS_Alert106					106
	#define	IDS_Alert107					107
	#define	IDS_Alert108					108
	#define	IDS_Alert109					109	
	#define	IDS_Alert110					110	
	#define	IDS_Alert111					111
	#define	IDS_Alert112					112
	#define	IDS_Alert113					113
	
	#define	IDS_Alert115					115
	#define	IDS_Alert116					116
	#define	IDS_Alert117					117
	#define	IDS_Alert118					118
	
	#define	IDS_Alert120					120
	#define	IDS_Alert121					121
  	#define	IDS_Alert122					122
	#define	IDS_Alert123					123
	#define	IDS_Alert124					124
  	#define	IDS_Alert125					125
	#define	IDS_Alert126					126
	#define	IDS_Alert127					127
  	#define	IDS_Alert128					128
	#define	IDS_Alert129					129

	#define	IDS_Alert136					136
	#define	IDS_Alert137					137
	
	
			// 	Classify Processor strings													
			
	#define	IDS_Classify1					1		// Writing ERDAS .TRL disk file.
	#define	IDS_Classify2					2
	#define	IDS_Classify3					3
	#define	IDS_Classify4					4
	#define	IDS_Classify5					5
	#define	IDS_Classify6					6
	#define	IDS_Classify7					7
	#define	IDS_Classify8					8
	#define	IDS_Classify9					9
	#define	IDS_Classify10					10
	#define	IDS_Classify11					11
	#define	IDS_Classify12					12
	#define	IDS_Classify13					13
	#define	IDS_Classify14					14
	#define	IDS_Classify15					15
	#define	IDS_Classify16					16
	#define	IDS_Classify17					17
	#define	IDS_Classify18					18
	#define	IDS_Classify19					19
	#define	IDS_Classify20					20
	#define	IDS_Classify21					21
	#define	IDS_Classify22					22
	#define	IDS_Classify23					23
	#define	IDS_Classify24					24
	#define	IDS_Classify25					25
	#define	IDS_Classify26					26
	#define	IDS_Classify27					27
	#define	IDS_Classify28					28
	#define	IDS_Classify29					29
	#define	IDS_Classify30					30
	#define	IDS_Classify31					31
	#define	IDS_Classify32					32
	#define	IDS_Classify33					33
	#define	IDS_Classify34					34
	#define	IDS_Classify35					35
	#define	IDS_Classify36					36
	#define	IDS_Classify37					37
	#define	IDS_Classify38					38
	#define	IDS_Classify39					39
	#define	IDS_Classify40					40
	#define	IDS_Classify41					41
	#define	IDS_Classify42					42
	#define	IDS_Classify43					43
	#define	IDS_Classify44					44
	#define	IDS_Classify45					45
	#define	IDS_Classify46					46
	#define	IDS_Classify47					47
	#define	IDS_Classify48					48
	#define	IDS_Classify49					49
	#define	IDS_Classify50					50
	#define	IDS_Classify51					51
	#define	IDS_Classify52					52
	#define	IDS_Classify53					53
	#define	IDS_Classify54					54
	#define	IDS_Classify55					55
	#define	IDS_Classify56					56
	#define	IDS_Classify57					57
	#define	IDS_Classify58					58
	#define	IDS_Classify59					59
	#define	IDS_Classify60					60
	#define	IDS_Classify61					61
	#define	IDS_Classify62					62
	#define	IDS_Classify63					63
	#define	IDS_Classify64					64
	#define	IDS_Classify65					65
	#define	IDS_Classify66					66

			// 	Cluster string indexes
			
	#define	IDS_Cluster1					1
	#define	IDS_Cluster2					2
	#define	IDS_Cluster3					3
	#define	IDS_Cluster4					4
	#define	IDS_Cluster5					5
	#define	IDS_Cluster6					6
	#define	IDS_Cluster7					7
	#define	IDS_Cluster8					8
	#define	IDS_Cluster9					9
	#define	IDS_Cluster10					10
	#define	IDS_Cluster11					11
	#define	IDS_Cluster12					12
	#define	IDS_Cluster13					13
	#define	IDS_Cluster14					14
	#define	IDS_Cluster15					15
	#define	IDS_Cluster16					16
	#define	IDS_Cluster17					17
	#define	IDS_Cluster18					18
	#define	IDS_Cluster19					19
	#define	IDS_Cluster20					20
	#define	IDS_Cluster21					21
	#define	IDS_Cluster22					22
	#define	IDS_Cluster23					23
	#define	IDS_Cluster24					24
	#define	IDS_Cluster25					25
	#define	IDS_Cluster26					26
	#define	IDS_Cluster27					27
	#define	IDS_Cluster28					28
	#define	IDS_Cluster29					29
	#define	IDS_Cluster30					30
	#define	IDS_Cluster31					31
	#define	IDS_Cluster32					32
	#define	IDS_Cluster33					33
	#define	IDS_Cluster34					34
	#define	IDS_Cluster35					35
	#define	IDS_Cluster36					36
	#define	IDS_Cluster37					37
	#define	IDS_Cluster38					38
	#define	IDS_Cluster39					39
	#define	IDS_Cluster40					40
	#define	IDS_Cluster41					41
	#define	IDS_Cluster42					42
	#define	IDS_Cluster43					43
	#define	IDS_Cluster44					44
	#define	IDS_Cluster45					45
	#define	IDS_Cluster46					46
	#define	IDS_Cluster47					47
	#define	IDS_Cluster48					48
	#define	IDS_Cluster49					49
	#define	IDS_Cluster50					50
	#define	IDS_Cluster51					51

			// 	Compression string indexes	          				
			
	#define	IDS_Compression01				1		 
	#define	IDS_Compression02				2 		 
	#define	IDS_Compression03				3 		 
	#define	IDS_Compression04				4 		 
	#define	IDS_Compression05				5 		 
	#define	IDS_Compression06				6 		 
	#define	IDS_Compression07				7 		 
	#define	IDS_Compression08				8 		 
	#define	IDS_Compression09				9 		 
	#define	IDS_Compression10				10 		 
	#define	IDS_Compression11				11 		 
	#define	IDS_Compression12				12 		 
	#define	IDS_Compression13				13 

			// 	Dialog string indexes
			
	#define	IDS_Dialog11					11
	#define	IDS_Dialog16					16
	#define	IDS_Dialog17					17	// Channels:
	#define	IDS_Dialog18					18	// Blank string
	#define	IDS_Dialog19					19
	#define	IDS_Dialog20					20
	#define	IDS_Dialog21					21
	#define	IDS_Dialog22					22
	#define	IDS_Dialog23					23
	#define	IDS_Dialog24					24 // Edit rectangluar field
	#define	IDS_Dialog25					25 // Edit polygonal field
	#define	IDS_Dialog26					26 // Edit selection rectangle
	#define	IDS_Dialog27					27 // 
	#define	IDS_Dialog28					28 // Classes:
	#define	IDS_Dialog29					29 // Groups:

			// 	Feature Extraction Processor strings													
			
	#define	IDS_FeatureExtract1			1
	#define	IDS_FeatureExtract2			2
	#define	IDS_FeatureExtract3			3
	#define	IDS_FeatureExtract4			4
	#define	IDS_FeatureExtract5			5
	#define	IDS_FeatureExtract6			6
	#define	IDS_FeatureExtract7			7
	#define	IDS_FeatureExtract8			8
	#define	IDS_FeatureExtract9			9
	#define	IDS_FeatureExtract10			10
	#define	IDS_FeatureExtract11			11
	#define	IDS_FeatureExtract12			12
	#define	IDS_FeatureExtract13			13
	#define	IDS_FeatureExtract14			14
	#define	IDS_FeatureExtract15			15
	#define	IDS_FeatureExtract16			16
	#define	IDS_FeatureExtract17			17
	#define	IDS_FeatureExtract18			18
	#define	IDS_FeatureExtract19			19
	#define	IDS_FeatureExtract20			20
	#define	IDS_FeatureExtract21			21
	#define	IDS_FeatureExtract22			22
	#define	IDS_FeatureExtract23			23
	#define	IDS_FeatureExtract24			24
	#define	IDS_FeatureExtract25			25
	#define	IDS_FeatureExtract26			26
	#define	IDS_FeatureExtract27			27
	#define	IDS_FeatureExtract28			28
	#define	IDS_FeatureExtract29			29
	#define	IDS_FeatureExtract30			30
	#define	IDS_FeatureExtract31			31
	#define	IDS_FeatureExtract32			32
	#define	IDS_FeatureExtract33			33
	#define	IDS_FeatureExtract34			34
	#define	IDS_FeatureExtract35			35
	#define	IDS_FeatureExtract36			36
	#define	IDS_FeatureExtract37			37
	#define	IDS_FeatureExtract38			38
	#define	IDS_FeatureExtract39			39
	#define	IDS_FeatureExtract40			40
	#define	IDS_FeatureExtract41			41
	#define	IDS_FeatureExtract42			42
	#define	IDS_FeatureExtract43			43
	#define	IDS_FeatureExtract44			44
	#define	IDS_FeatureExtract45			45
	#define	IDS_FeatureExtract46			46
	#define	IDS_FeatureExtract47			47
	#define	IDS_FeatureExtract48			48
	#define	IDS_FeatureExtract49			49
	#define	IDS_FeatureExtract50			50
	#define	IDS_FeatureExtract51			51
	#define	IDS_FeatureExtract52			52
	#define	IDS_FeatureExtract53			53
	#define	IDS_FeatureExtract54			54
	#define	IDS_FeatureExtract55			55
	#define	IDS_FeatureExtract56			56
	#define	IDS_FeatureExtract57			57
	#define	IDS_FeatureExtract58			58
	#define	IDS_FeatureExtract59			59
	#define	IDS_FeatureExtract60			60
	#define	IDS_FeatureExtract61			61
	#define	IDS_FeatureExtract62			62
	#define	IDS_FeatureExtract63			63
	#define	IDS_FeatureExtract64			64
	#define	IDS_FeatureExtract65			65
	#define	IDS_FeatureExtract66			66
	#define	IDS_FeatureExtract67			67
	#define	IDS_FeatureExtract68			68
	#define	IDS_FeatureExtract69			69
	#define	IDS_FeatureExtract70			70
	#define	IDS_FeatureExtract71			71
	#define	IDS_FeatureExtract72			72
	#define	IDS_FeatureExtract73			73
	
			// 	Feature Selection Processor strings
	
	#define	IDS_FeatureSelection1		1
	#define	IDS_FeatureSelection2		2
	#define	IDS_FeatureSelection3		3
	#define	IDS_FeatureSelection4		4
	#define	IDS_FeatureSelection5		5
	#define	IDS_FeatureSelection6		6
	#define	IDS_FeatureSelection7		7
	#define	IDS_FeatureSelection8		8
	#define	IDS_FeatureSelection9		9
	#define	IDS_FeatureSelection10		10
	#define	IDS_FeatureSelection11		11
	#define	IDS_FeatureSelection12		12
	#define	IDS_FeatureSelection13		13
	#define	IDS_FeatureSelection14		14
	#define	IDS_FeatureSelection15		15
	#define	IDS_FeatureSelection16		16
	#define	IDS_FeatureSelection17		17
	#define	IDS_FeatureSelection18		18
	#define	IDS_FeatureSelection19		19
	#define	IDS_FeatureSelection20		20
	#define	IDS_FeatureSelection21		21
	#define	IDS_FeatureSelection22		22
	#define	IDS_FeatureSelection23		23
	#define	IDS_FeatureSelection24		24
	#define	IDS_FeatureSelection25		25
	#define	IDS_FeatureSelection26		26
	#define	IDS_FeatureSelection27		27
	#define	IDS_FeatureSelection28		28
	#define	IDS_FeatureSelection29		29
	#define	IDS_FeatureSelection30		30
	#define	IDS_FeatureSelection31		31
	#define	IDS_FeatureSelection32		32
	#define	IDS_FeatureSelection33		33
	#define	IDS_FeatureSelection34		34

			// 	FileIO string indexes	          				
			
	#define	IDS_FileIO01				1		 
	#define	IDS_FileIO02				2 		// Not currently used.
	#define	IDS_ImageFile				3
	#define	IDS_FileIO4					4
	#define	IDS_FileIO5					5
	#define	IDS_SaveTextOutputAs		6		// Save text output window as: kFileIOStr6
	#define	IDS_SelectImage			7		// Select Image File
	#define	IDS_SelectProject			8		// Select Project File kFileIOStr8
	#define	IDS_WhereIsProjectImage	9		// Where Is Project Image File? 
	#define	IDS_SaveImageStats		10		// Save Image Statistics File As: kFileIOStr10
	#define	IDS_SaveNewImageFileAs	11		// Save New Image File As:
	#define	IDS_SaveProjectFile		12		// Save Project File As: kFileIOStr12
	#define	IDS_SaveClassificationAs	13		// Save classification results as: kFileIOStr13
	#define	IDS_SelectImageStatistics	14		// Select Image Statistics File kFileIOStr14
	#define	kFileIOStr15				15		// Bad file name.
	#define	kFileIOStr16				16		// File directory is full.
	#define	kFileIOStr17				17		// Volume (disk) is locked.
	#define	kFileIOStr18				18		// File already exists.
	#define	kFileIOStr19				19		// Tried to read past end-of-file.
	#define	kFileIOStr20				20		// File is locked.
	#define	kFileIOStr21				21		// File is not open.
	#define	kFileIOStr22				22		// Too many files are open.
	#define	kFileIOStr23				23		// Volume is locked.
	#define	kFileIOStr24				24		// File is already open for writing.
	#define	kFileIOStr25				25		// File IO error accessing '
	#define	kFileIOStr26				26
	#define	IDS_SelectThematicImage		27		// Select 'Thematic' Image File kThematicPromptStr
	#define  IDS_MacLARSYSType 		28
	#define  IDS_NumberClasses 		29
	#define  IDS_ClassifySelectedArea 	30
	#define  IDS_ClassesUsed	 		31
	#define	kFileIOStr32				32
	#define	kFileIOStr33				33
	#define	IDS_SaveProbabilityAs	34		// Save Probability Map As: kFileIOStr34
	#define	IDS_SelectNewBaseImage	35		// kFileIONewBaseImage
	#define	IDS_ImageStartLine		36
	#define	IDS_SaveReportMapAs		37
	#define	IDS_SaveSepReportAs		38
	#define	IDS_FileIO39				39
	#define	IDS_FileIO40				40
	#define	IDS_FileIO41				41
	#define  IDS_MultiSpecType 		42
	#define	IDS_SelectFileToLink		43
	#define  IDS_VICARLabelSize		44
	#define	IDS_FileIO45				45
	#define	IDS_FileIO46				46
	#define	IDS_FileIO47				47
	#define	IDS_FileIO48				48
	#define	IDS_FileIO49				49
	#define	IDS_FileIO50				50
	#define	IDS_FileIO51				51
	#define	IDS_FileIO52				52
	#define	IDS_FileIO53				53
	#define  IDS_ChannelDescriptions	54
	#define  IDS_GroupDescriptions	55
	#define	IDS_ChannelValues			56
	#define	IDS_BlankString16			57
	#define	IDS_FileIO58				58
	#define	IDS_FileIO59				59
	#define	IDS_FileIO60				60
	#define	IDS_FileIO61				61
	#define	IDS_FileIO62				62
	#define	IDS_FileIO63				63
	#define	IDS_FileIO64				64
	#define	IDS_FileIO65				65
	#define  IDS_Erdas73Type 			66
	#define  IDS_Erdas74Type 			67
	#define	IDS_FileIO68				68
	#define	IDS_FileIO69				69
	#define	IDS_FileIO70				70
	#define	IDS_FileIO71				71
	#define	IDS_FileIO72				72
	#define	IDS_FileIO73				73
	#define	IDS_FileIO74				74
	#define	IDS_FileIO75				75 
	#define	IDS_FileIO76				76 
	#define	IDS_FileIO77				77 
	#define	IDS_PDSIdentifier			78
	#define	IDS_FileIO79				79
	#define	IDS_FileIO80				80
	#define	IDS_FileIO81				81
	#define	IDS_FileIO82				82
	#define	IDS_FileIO83				83
	#define	IDS_FileIO84				84
	#define	IDS_FileIO85				85
	#define	IDS_FileIO86				86
	#define	IDS_FileIO87				87
	#define	IDS_FileIO88				88
	#define	IDS_FileIO89				89
	#define	IDS_FileIO90				90
	#define	IDS_FileIO91				91
	#define	IDS_FileIO92				92
	#define	IDS_FileIO93				93
	#define	IDS_FileIO94				94
	#define	IDS_FileIO95				95
	#define	IDS_FileIO96				96
	#define	IDS_FileIO97				97
	#define	IDS_FileIO98				98
	#define	IDS_FileIO99				99
	#define	IDS_FileIO100				100
	#define	IDS_FileIO101				101
	#define	IDS_FileIO102				102
	#define	IDS_FileIO103				103
	#define	IDS_FileIO104				104
	#define	IDS_FileIO105				105
	#define	IDS_FileIO106				106
	#define	IDS_FileIO107				107
	#define	IDS_FileIO108				108
	#define	IDS_FileIO109				109
	#define	IDS_FileIO110				110
	#define	IDS_FileIO111				111
	#define	IDS_FileIO112				112
	#define	IDS_FileIO113				113
	#define	IDS_FileIO114				114
	#define	IDS_FileIO115				115
	#define	IDS_FileIO116				116
	#define	IDS_FileIO117				117
	#define	IDS_FileIO118				118
	#define	IDS_FileIO119				119
	#define	IDS_FileIO120				120
	#define	IDS_FileIO121				121
	#define	IDS_FileIO122				122
	#define	IDS_FileIO123				123
	#define	IDS_FileIO124				124
	#define	IDS_FileIO125				125
	#define	IDS_FileIO126				126
	#define	IDS_FileIO127				127
	#define	IDS_FileIO128				128
	#define	IDS_FileIO129				129
	#define	IDS_FileIO130				130
	#define	IDS_FileIO131				131
	#define	IDS_FileIO132				132
	#define	IDS_FileIO133				133
	#define	IDS_FileIO134				134
	#define	IDS_FileIO135				135
	#define	IDS_FileIO136				136
	#define	IDS_FileIO137				137
	#define	IDS_FileIO138				138
	#define	IDS_FileIO139				139	
	#define	IDS_FileIO140				140
	#define	IDS_FileIO141				141
	#define	IDS_FileIO142				142
	#define	IDS_FileIO143				143
	#define	IDS_FileIO144				144
	#define	IDS_FileIO145				145
	#define	IDS_FileIO146				146
	#define	IDS_FileIO147				147
	#define	IDS_FileIO148				148
	#define	IDS_FileIO149				149
	#define	IDS_FileIO150				150
	#define	IDS_FileIO151				151
	#define	IDS_FileIO152				152
	#define	IDS_FileIO153				153
	#define	IDS_FileIO154				154
	#define	IDS_FileIO155				155
	#define	IDS_FileIO156				156
	#define	IDS_FileIO157				157
	#define	IDS_FileIO175				175
	
			// File type string indexes
	
	#define	IDS_FileType01				1
	
			// Histogram processor string indexes
	
	#define  IDS_HistogramStatus1 				1	// Computing histogram for kHistogramStr1
	#define  IDS_HistogramStatus2 				2	// Printing histogram kHistogramStr2
	#define  IDS_HistogramStatus3					3	// Writing image statistics kHistogramStr3
	#define  IDS_HistogramStatus4					4	// Reading image statistics kHistogramStr4
	#define  IDS_HistogramTitle1 					5
	#define  IDS_HistogramTitle2 					6
	#define  IDS_HistogramTitle3 					7
	#define  IDS_HistogramTitle4 					8
	#define  IDS_HistogramTitle5					9
	#define  IDS_HistogramTitle6					10
	#define  IDS_HistogramTitle7					11
	#define  IDS_HistogramTitle8					12
	#define  IDS_HistogramTitle9					13
	#define  IDS_HistogramTitle10					14
	#define  IDS_HistogramTitle11					15
	#define  IDS_HistogramTitle12					16
	#define  IDS_HistogramStatus5					17
	#define  IDS_HistogramStatus6					18 
	
			// Instrument name string indexes
	
	#define	IDS_InstrumentName01				1
	
			// 	List Data Processor strings	
			
	#define  IDS_ListData1						1
	#define  IDS_ListData2						2
	#define  IDS_ListData3						3
	#define  IDS_ListData4						4
	#define  IDS_ListData5						5
	#define  IDS_ListData6						6
	#define  IDS_ListData7						7
	#define  IDS_ListData8						8
	#define 	IDS_ListData10 					10
	#define 	IDS_ListData15 					15
	
			// 	List Results Processor strings													
			
	#define	IDS_ListResult1					1
	#define	IDS_ListResult2					2
	#define	IDS_ListResult3					3
	#define	IDS_ListResult4					4
	#define	IDS_ListResult5					5
	#define	IDS_ListResult6					6
	#define	IDS_ListResult7					7
	#define	IDS_ListResult8					8
	#define	IDS_ListResult9					9
	#define	IDS_ListResult10					10
	#define	IDS_ListResult11					11
	#define	IDS_ListResult12					12
	#define	IDS_ListResult13					13
	#define	IDS_ListResult14					14
	#define	IDS_ListResult15					15
	#define	IDS_ListResult16					16
	#define	IDS_ListResult17					17
	#define	IDS_ListResult18					18
	#define	IDS_ListResult19					19
	#define	IDS_ListResult20					20
	#define	IDS_ListResult21					21
	#define	IDS_ListResult22					22
	#define	IDS_ListResult23					23
	#define	IDS_ListResult24					24
	#define	IDS_ListResult25					25
	#define	IDS_ListResult26					26
	#define	IDS_ListResult27					27
	#define	IDS_ListResult28					28
	#define	IDS_ListResult29					29
	#define	IDS_ListResult30					30
	#define	IDS_ListResult31					31
	#define	IDS_ListResult32					32
	#define	IDS_ListResult33					33
	#define	IDS_ListResult34					34
	#define	IDS_ListResult35					35
	#define	IDS_ListResult36					36
	#define	IDS_ListResult37					37
	#define	IDS_ListResult38					38
	#define	IDS_ListResult39					39
	#define	IDS_ListResult40					40
	#define	IDS_ListResult41					41
	#define	IDS_ListResult42					42
	#define	IDS_ListResult43					43
	#define	IDS_ListResult44					44
	#define	IDS_ListResult45					45
	#define	IDS_ListResult46					46
	#define	IDS_ListResult47					47
	#define	IDS_ListResult48					48
	
	#define  IDS_PrincipalComponent1				1
	#define  IDS_PrincipalComponent2				2
	#define  IDS_PrincipalComponent3				3
	#define  IDS_PrincipalComponent4				4
	#define  IDS_PrincipalComponent5				5
	#define  IDS_PrincipalComponent6				6
	#define  IDS_PrincipalComponent7				7
	#define  IDS_PrincipalComponent8				8
	#define  IDS_PrincipalComponent9				9
	#define  IDS_PrincipalComponent10			10
	#define  IDS_PrincipalComponent11			11
	#define  IDS_PrincipalComponent12			12
	#define  IDS_PrincipalComponent13			13
	
	#define  IDS_ProcessorDisplay					1
	#define  IDS_ProcessorHistogram				2
	#define  IDS_ProcessorReformat				3
	#define  IDS_ProcessorCluster					4
	#define  IDS_ProcessorListStats				5
	#define  IDS_ProcessorHistStats				6
	#define  IDS_ProcessorFeature					7
	#define  IDS_ProcessorClassify				8
	#define  IDS_ProcessorResults					9
	#define  IDS_ProcessorCheckCov				10
	#define  IDS_ProcessorListData				11
	#define  IDS_ProcessorPrinc					12
	#define  IDS_ProcessorCreateStat				13
	#define  IDS_ProcessorExtract					14
	#define  IDS_ProcessorCheckTran				15
	#define  IDS_ProcessorEnhance					16
	#define  IDS_ProcessorBiPlot					17
	#define	IDS_Processor18						18
	#define	IDS_Processor19						19

	#define	IDC_ShortStatusText					2
	#define	IDC_ShortStatusValue					3
	
			// Reformat string indexes
	
	#define	IDS_PercentComplete				1		// Percent complete kReformatStr1
	#define	IDS_Reform7							7
	#define	IDS_Reform8							8
	#define	IDS_Reform9							9
	#define	IDS_Reform10						10
	#define	IDS_Reform11						11
	#define	IDS_Reform12						12
	#define	IDS_Reform13						13
	#define	IDS_Reform14						14
	#define	IDS_Reform15						15
	#define	IDS_Reform16						16
	#define	IDS_Reform17						17
	#define	IDS_Reform18						18
	#define	IDS_Reform19						19
	#define	IDS_Reform20						20
	#define	IDS_Reform21						21
	#define	IDS_Reform22						22
	#define	IDS_Reform23						23
	#define	IDS_Reform24						24
	#define	IDS_Reform25						25
	#define	IDS_Reform26						26
	#define	IDS_Reform27						27
	#define	IDS_Reform28						28
	#define	IDS_Reform30						30
	#define	IDS_Reform31						31
	#define	IDS_Reform32						32
	#define	IDS_Reform33						33
	
			// Project string indexes
			
	#define	IDS_Project1				1
	#define	IDS_Project2				2
	#define	IDS_Project3				3
	#define	IDS_Project4				4
	#define	IDS_Project5				5
	#define	IDS_Project6				6
	#define	IDS_Project7				7
	#define	IDS_Project8				8
	#define	IDS_Project9				9
	#define	IDS_Project10				10
	#define	IDS_Project11				11
	#define	IDS_Project12				12
	#define	IDS_Project13				13
	#define	IDS_Project14				14
	#define	IDS_Project15				15
	#define	IDS_Project16				16
	#define	IDS_Project17				17
	#define	IDS_Project18				18
	#define	IDS_Project19				19
	#define	IDS_Project20				20
	#define	IDS_Project21				21
	#define	IDS_Project22				22
	#define	IDS_Project23				23
	#define	IDS_Project24				24
	#define	IDS_Project25				25
	#define	IDS_Project26				26
	#define	IDS_Project27				27
	#define	IDS_Project28				28
	#define	IDS_Project29				29
	#define	IDS_Project30				30
	#define	IDS_Project31				31
	#define	IDS_Project32				32
	#define	IDS_Project33				33
	#define	IDS_Project34				34
	#define	IDS_Project35				35
	#define	IDS_Project36				36
	#define	IDS_Project37				37
	#define	IDS_Project38				38
	#define	IDS_Project39				39
	#define	IDS_Project40				40	
	#define	IDS_Project41				41	
	#define	IDS_Project42				42	
	#define	IDS_Project43				43	
	#define	IDS_Project44				44	
	#define	IDS_Project45				45
	#define	IDS_Project46				46
	#define	IDS_Project47				47
	#define	IDS_Project48				48
	#define	IDS_Project49				49
	#define	IDS_Project50				50
	#define	IDS_Project51				51
	#define	IDS_Project52				52
	#define	IDS_Project53				53
	#define	IDS_Project54				54
	#define	IDS_Project55				55
	#define	IDS_Project56				56 
	#define	IDS_Project57				57 
	#define	IDS_Project58				58 
	#define	IDS_Project59				59 
	#define	IDS_Project60				60 
	#define	IDS_Project61				61
	#define	IDS_Project62				62
	#define	IDS_Project63				63
	#define	IDS_Project64				64
	#define	IDS_Project65				65
	#define	IDS_Project66				66
	#define	IDS_Project67				67
	#define	IDS_Project68				68
	#define	IDS_Project69				69
	#define	IDS_Project70				70
	#define	IDS_Project71				71
	#define	IDS_Project72				72
	#define	IDS_Project73				73
	#define	IDS_Project74				74
	#define	IDS_Project75				75
	#define	IDS_Project76				76
	#define	IDS_Project77				77
	#define	IDS_Project78				78
  	#define	IDS_Project79				79
  	#define	IDS_Project80				80
  	#define	IDS_Project81				81
  	#define	IDS_Project82				82
  	#define	IDS_Project83				83
  	#define	IDS_Project84				84
  	#define	IDS_Project85				85
  	#define	IDS_Project86				86
  	#define	IDS_Project87				87
  	#define	IDS_Project88				88
  	#define	IDS_Project89				89
	
			// Symbols string indexes
	
	#define	IDS_Symbol1					1		// string with symbols for classify processor
	
			// Dialog string indexes
	
	#define	kDialogStr1					1		// Set Thematic File Specifications
	
			// Project version string indexes
	
	#define	IDS_ProjectVersion1		1
	#define	IDS_ProjectVersion2		2
	
			// Statistics Enhancement Processor strings													
			
	#define	IDS_StatEnhance1			1
	#define	IDS_StatEnhance2			2
	#define	IDS_StatEnhance3			3
	#define	IDS_StatEnhance4			4
	#define	IDS_StatEnhance5			5
	#define	IDS_StatEnhance6			6
	#define	IDS_StatEnhance7			7
	#define	IDS_StatEnhance8			8
	#define	IDS_StatEnhance9			9
	#define	IDS_StatEnhance10			10
	#define	IDS_StatEnhance11			11
	#define	IDS_StatEnhance12			12
	#define	IDS_StatEnhance13			13
	#define	IDS_StatEnhance14			14
	#define	IDS_StatEnhance15			15
	#define	IDS_StatEnhance16			16
	#define	IDS_StatEnhance17			17
	#define	IDS_StatEnhance18			18
	#define	IDS_StatEnhance19			19
	#define	IDS_StatEnhance20			20
	#define	IDS_StatEnhance21			21
	#define	IDS_StatEnhance22			22
	#define	IDS_StatEnhance23			23
	#define	IDS_StatEnhance24			24
	#define	IDS_StatEnhance25			25
	#define	IDS_StatEnhance26			26
	#define	IDS_StatEnhance27			27
	#define	IDS_StatEnhance28			28
	#define	IDS_StatEnhance29			29
	#define	IDS_StatEnhance30			30
	#define	IDS_StatEnhance31			31
	#define	IDS_StatEnhance32			32
	
			// Statistics Image string indexes
			
	#define	IDS_StatisticsImage1		1
	#define	IDS_StatisticsImage2		2
	#define	IDS_StatisticsImage3		3
	#define	IDS_StatisticsImage4		4
	#define	IDS_StatisticsImage5		5
	#define	IDS_StatisticsImage6		6
	#define	IDS_StatisticsImage7		7
	#define	IDS_StatisticsImage8		8
	#define	IDS_StatisticsImage9		9
	#define	IDS_StatisticsImage10	10
	#define	IDS_StatisticsImage11	11
	#define	IDS_StatisticsImage12	12
	#define	IDS_StatisticsImage13	13
	#define	IDS_StatisticsImage14	14
	#define	IDS_StatisticsImage15	15
	#define	IDS_StatisticsImage16	16
	#define	IDS_StatisticsImage17	17
	#define	IDS_StatisticsImage18	18
	#define	IDS_StatisticsImage19	19
	
			// Utility version string indexes
	                                 
	
	#define  IDS_Utility1				1
	#define  IDS_Utility2				2
	#define  IDS_Utility3				3
	#define  IDS_Utility4				4
	#define  IDS_Utility5				5
	#define	IDS_Utility6				6
	#define	IDS_Utility7				7
	#define	IDS_Utility8				8
	#define	IDS_Utility9				9
	#define	IDS_Utility10				10
	#define	IDS_Utility11				11
	#define	IDS_Utility12				12
	#define	IDS_Utility13				13
	
			// Shared string indexes
			
	#define	IDS_Shared1					1
	#define	IDS_Shared2					2
	#define	IDS_Shared3					3
	#define	IDS_Shared4					4
	#define	IDS_Shared5					5
	#define	IDS_Shared6					6
	#define	IDS_Shared7					7
	#define	IDS_Shared8					8
	
			// Status Dialog Item Constants
			
	#define	IDCANCEL						1
	#define	IDC_Status2					2	
	#define	IDC_Status3					3	
	#define	IDC_Status4					4	
	#define	IDC_Status5					5	
	#define	IDC_Status6					6	
	#define	IDC_Status7					7	
	#define	IDC_Status8					8	
	#define	IDC_Status9					9
	#define	IDC_Status10				10
	#define	IDC_Status11				11
	#define	IDC_Status12				12
	#define	IDC_Status13				13
	#define	IDC_Status14				14
	#define	IDC_Status15				15
	#define	IDC_Status16				16
	#define	IDC_Status17				17
	#define	IDC_Status18				18
	#define	IDC_Status19				19
	#define	IDC_Status20				20
	#define	IDC_Status21				21
	#define	IDC_Status22				22
#endif	// defined multispec_mac

#if defined multispec_lin || defined multispec_win
	
			// resource IDs of modal dialogs 													
		                                             
	#define 	kShortStatusInfoID					IDD_ShortStatus
	#define	kGraphicStatusDialogID				IDD_GraphicsStatus	// IDD_GraphicsStatus
	#define	kUpdateStatsInfoID					IDD_UpdateStatus
	
			// resource IDs of alerts	
																	
	#if defined multispec_lin
		#define	kErrorAlertID							wxOK
		#define	kErrorAlert2ID							wxOK
		#define	kMemoryAlertID							wxOK
		#define	kOpenImageWAlertID					wxYES_NO|wxCANCEL
		#define	kContinueCancelAlertID				wxOK
		#define	kSaveAlertID							wxYES_NO|wxCANCEL
		#define	kMemoryWarningAlertID				wxOK
		#define 	kOKCancelAlertID						wxOK|wxCANCEL
		#define	kYesCancelAlertID						wxOK|wxCANCEL
		#define	kTextWindowLowMemoryAlertID		wxOK
		#define	kTextWindowResultsAlertID			wxOK
		#define	kUpdateCancelAlertID					wxOK|wxCANCEL
		#define	kFinishDontCancelAlertID			wxYES_NO|wxCANCEL
		#define	kRedoDontCancelAlertID				wxYES_NO
		#define	kSaveDontSaveAlertID					wxYES_NO|wxCANCEL
		#define	kYesNoAlertID							wxOK|wxCANCEL
		#define	kContinueStopAlertID					wxYES_NO|wxCANCEL
	#endif	// defined multispec_lin
	
	#if defined multispec_win
		#define	kErrorAlertID							MB_OK
		#define	kErrorAlert2ID							MB_OK
		#define	kMemoryAlertID							MB_OK
		#define	kOpenImageWAlertID					MB_YESNOCANCEL
		#define	kContinueCancelAlertID				MB_OKCANCEL
		#define	kSaveAlertID							MB_YESNOCANCEL
		#define	kMemoryWarningAlertID				MB_OK
		#define 	kOKCancelAlertID						MB_OKCANCEL
		#define	kYesCancelAlertID						MB_OKCANCEL
		#define	kTextWindowLowMemoryAlertID		MB_OK
		#define	kTextWindowResultsAlertID			MB_OK 
		#define	kUpdateCancelAlertID					MB_OKCANCEL
		#define	kFinishDontCancelAlertID			MB_YESNOCANCEL
		#define	kRedoDontCancelAlertID				MB_YESNOCANCEL
		#define	kSaveDontSaveAlertID					MB_YESNOCANCEL
		#define	kYesNoAlertID							MB_OKCANCEL
		#define	kContinueStopAlertID					MB_YESNOCANCEL
	#endif	// defined multispec_win
	
			// resource ID of cursors																	
			
	#define	kHandCursorID							151
	#define	kCrossCursorID							1
	#define	kLegendWidthCursorID					153
	#define	kBlinkCursor1ID						154
	#define	kBlinkCursor2ID						155
	#define	kWaitCursorID							156
	
			// resource ID of strings											
			
	#define	kFileIOStrID							0
	#define 	kHistogramStrID						0
	#define	kReformatStrID							0
	#define	kProjectStrID							0
	#define	kSymbolsStrID							0
	#define	kClassifyStrID							0
	#define	kDialogStrID							0
	#define	kAlertStrID								0
	#define	kListResultsStrID						0
	#define	kProcessorStrID						0
	#define	kProjectVersionsStrID				0
	#define	kGAIAStrID								0
	#define	kFeatureExtractStrID					0
	#define	kFeatureSelectionStrID				0
	#define	kEnhanceStatStrID						0
	#define	kClusterStrID							0
	#define	kUtilitiesStrID						0 
	#define	kSharedStrID							0
	#define	kPrincipalCompStrID					0
	#define	kListDataStrID							0
	#define	kStatisticsImageStrID				0
	#define	kFileTypeStrID							0
	#define	kBandInterleaveStrID					0
	#define	kCompressionStrID						0
	#define	kInstrumentNameStrID					0
	#define	kBiPlotStrID                     0
	#define	kDisplayStrID							0
	
	//#define  IDS_MacLARSYSType						5
	//#define  IDS_MultiSpecType						6
	//#define  IDS_Erdas73Type 						7
	//#define  IDS_Erdas74Type 						8
	//#define  IDS_VICARLabelSizeType				8
	//#define  IDS_ProjectType							8
	//#define  IDS_LastVersionIndex					8
	
	#define  IDS_HistogramStatus1 				401
	#define  IDS_HistogramStatus2 				402
	#define  IDS_HistogramStatus3					403
	#define  IDS_HistogramStatus4					404
	#define  IDS_HistogramTitle1 					405
	#define  IDS_HistogramTitle2 					406
	#define  IDS_HistogramTitle3 					407
	#define  IDS_HistogramTitle4 					408
	#define  IDS_HistogramTitle5					409
	#define  IDS_HistogramTitle6					410
	#define  IDS_HistogramTitle7					411
	#define  IDS_HistogramTitle8					412
	#define  IDS_HistogramTitle9					413
	#define  IDS_HistogramTitle10					414
	#define  IDS_HistogramTitle11					415
	#define  IDS_HistogramTitle12					416
	#define  IDS_HistogramStatus5					417
	#define  IDS_HistogramStatus6					418 
	
	#define  IDS_ProcessorDisplay					501
	#define  IDS_ProcessorHistogram				502
	#define  IDS_ProcessorReformat				503
	#define  IDS_ProcessorCluster					504
	#define  IDS_ProcessorListStats				505
	#define  IDS_ProcessorHistStats				506
	#define  IDS_ProcessorFeature					507
	#define  IDS_ProcessorClassify				508
	#define  IDS_ProcessorResults					509
	#define  IDS_ProcessorCheckCov				510
	#define  IDS_ProcessorListData				511
	#define  IDS_ProcessorPrinc					512
	#define  IDS_ProcessorCreateStat				513
	#define  IDS_ProcessorExtract					514
	#define  IDS_ProcessorCheckTran				515
	#define  IDS_ProcessorEnhance					516
	#define  IDS_ProcessorBiPlot					517
	
			// Alert strings
			
	#define	kSQD32DisplayColumnLimit			308  
	
#endif  // defined multispec_lin || defined multispec_win 
#endif // end of SRCONST
