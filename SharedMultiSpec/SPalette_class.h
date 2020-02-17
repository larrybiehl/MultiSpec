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
//	File:						SPalette_class.h
//	Implementation:		SPalette_class.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux, MacOS and Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMPalette class
//
//------------------------------------------------------------------------------------

#pragma once

#if defined multispec_wx
	#include "SMultiSpec.h" 
#endif	// defined multispec_wx

#if defined multispec_wx
	class CMPalette : public wxPalette 
#endif	// defined multispec_wx
#if defined multispec_win
	class	CMPalette : public CPalette
#endif	// defined multispec_win
		{
		public:
										CMPalette();	// constructor
			#if defined multispec_wx
											CMPalette (	// constructor
												SInt16								numberEntries);
			#endif	// defined multispec_wx
			
			virtual 					~CMPalette();		// destructor stub

			#if defined multispec_wx
				bool						Create (
												SInt16								numberEntries,
												UInt8*								redpalette,
												UInt8*								greenpalette,
												UInt8*								bluepalette);
			#endif	// defined multispec_wx
			
			#if defined multispec_win
				Boolean					LoadRGBQUADStructure (
												RGBQUAD								*RGBQuadPtr,
												SInt16								wUsage);
			#endif	// defined multispec_win
												
			SInt16					GetNumberPaletteEntries (void);
		
			Boolean					GetPaletteLoadedFlag () {return (m_loadedFlag);}
			
			void						SetNumberPaletteEntries (
											SInt16								numberEntries);
													
			void 						SetPalette (
											SInt16								paletteId);
		
			void						SetPaletteLoadedFlag (Boolean loadedFlag) {m_loadedFlag = loadedFlag;}
		
			#if defined multispec_wx
				RGB8BitColor			mPaletteObject[256];
			#endif
			#if defined multispec_win
				PaletteHandle			mPaletteObject;
			#endif
	
		protected:
			void						MSetPalette (void);
											
			SInt16					mNumberEntries;
			
			static UInt16			s_palette1001[117];
			
			static UInt16			s_palette1002[396];
			
			static UInt16			s_palette1003[759];
			
			static UInt16			s_palette1004[315];
			
			static UInt16			s_palette1005[372];
			
			static UInt16			s_palette1006[768];
			
			static UInt16			s_palette1007[768];
			
			static UInt16			s_palette1008[768];
		
			Boolean					m_loadedFlag;
			
		};  
