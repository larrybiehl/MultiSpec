# MultiSpec

# Description: 
MultiSpec is a freeware image data analysis application developed for interactively analyzing Earth observational multispectral and hyperspectral image data from airborne and spaceborne systems, as well as a number of other types of multispectral image data, such as medical images on MacOS and Windows platforms. MultiSpec has also been integrated with the GABBs geospatial data solutions (termed MultiSpec Online) to provide users with a more seamless experience by using within a web browser. MultiSpec is programmed in C/C++; the wxWidgets library was used for the GUI interface to run in the linux environment on the HUBzero platform. The software uses several other libraries such as gdal, hdf4, hdf5, netcdf and OpenJPEG to handle the many different file formats for remote sensing image data. 
# Installation:
  General Information
    - shared directory includes those files which are used for Linux, MacOS, and Windows builds
    - wxMultiSpec directory includes those files which are used for the Linux and MacOS builds to manage the user interface
    - windows directory includes those files which are used for the Window build to manage the user interface
    
    - the SMultiSpec.h include file includes directives to 'turn off' access to other libraries including hdf4, gdal, and hdf5.
    - The code in this repository was using the following library versions:
      - HDF4.2r2
      - gdal-2.1.2
      - hdf5-1.8.8
      - netcdf-4.2
      - openjpeg-2.1.2
  
  MacOS: Xcode verson 9.2 was used to create the project file included in this repository (MacOS_wxwidgets). 

#License
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
