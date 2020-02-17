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
//	File:						SWindowInfo_class.h
//	Implementation:		SWindowInfo_class.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux, MacOS, and Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMWindowInfo class.
//
//------------------------------------------------------------------------------------

#pragma once

class	CMWindowInfo
	{
	public:
									CMWindowInfo ();						// constructor
									
									CMWindowInfo (							// constructor
										Handle								fileInfoHandle);
										
		virtual 					~CMWindowInfo ();		// destructor stubBoolean
		
		Handle 					GetWindowInfoHandle (void);
										
		void						ReleaseOffscreenSupportMemory (void);

		void						SetImageWindowCPtrInWindowInfo (
										CMImageWindow*						imageWindowCPtr);
		
		Boolean					m_initializedFlag; 
	
	protected:
		Handle					m_windowInfoHandle;
		
	};
