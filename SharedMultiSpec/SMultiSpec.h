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
// File:						SMultiSpec.h
//
//	Authors:					Larry L. Biehl
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	Main header file for MultiSpec which contains calls to other 
//								include files for constants, resource constants, definitions, 
//								and function prototypes.
//
//	Written By:				Larry L. Biehl			Date: 03/29/1988
//	Revised By:				Abdur Maud				Date: 06/24/2013
//	Revised By:				Larry L. Biehl			Date: 10/19/2018
//
//------------------------------------------------------------------------------------

#pragma once

#if defined multispec_mac || defined multispec_mac_swift
	#include "wchar.h"
#endif

#if defined multispec_win
	#include "stdafx.h"
	//#ifndef WINVER
	//	#define  WINVER  0x0501
	//#endif	// !WINVER

	//	#ifndef _WIN32_WINNT
	//#define _WIN32_WINNT	0x0501 // _WIN32_WINNT_NT4
	//	#endif	// !_WIN32_WINNT

	//	#ifndef _WIN32_WINDOWS
	//#define _WIN32_WINDOWS	0x0501 // _WIN32_WINNT_NT4
	//	#endif	// !_WIN32_WINDOWS

	//	#ifndef _WIN32_IE
	//#define _WIN32_IE		0x0501 // _WIN32_WINNT_NT4
	//	#endif	// !_WIN32_IE

	//#ifndef DOUBLE
	//	typedef double DOUBLE;
	//#endif // !DOUBLE_ 

	#define include_hdf_capability 1 
	#define include_gdal_capability 1
	#define include_hdf5_capability 1

	//	#include "stdafx.h"
	#include <math.h>
	#include "float.h" 
	#include "limits.h" 
	#include "windowsx.h" 
	#include "resource.h"       // main symbols
	void AFXAPI DDX_Text2(CDataExchange* pDX, int nIDC, float& value);
	void AFXAPI DDX_Text2(CDataExchange* pDX, int nIDC, double& value);
#endif	// defined multispec_win 

#if defined multispec_wx
         // GDAL capability
	#ifdef NetBeansProject
		#define include_hdf_capability 1
		#define include_gdal_capability 1
		#define include_hdf5_capability 1
	#else	// ifndef NetBeansProject
		#ifdef multispec_wxmac
			#define include_hdf_capability 1
			#define include_gdal_capability 1
			#define include_hdf5_capability 1
		#else
			#define include_hdf_capability 1
			#define include_gdal_capability 1
			#define include_hdf5_capability 1
		#endif
	#endif
#endif	// end "defined multispec_wx"

#define use_mlte_for_text_window  0

#include "SConstants.h"
#include "SDefines.h" 
#include "SGraphic.h"
#include "SExternalGlobals.h"
#include "SResourceConstants.h"
#include "SPrototypes.h"  

#if defined multispec_wx
	#include "xResource.h"
	#include <time.h>
	#include <cmath>
	#include <cfloat>
	#include <float.h>
	#include <math.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <limits.h>
	#include <iostream>
	#include <string>
	#include "wx/gdicmn.h"
#endif	// defined multispec_wx     
