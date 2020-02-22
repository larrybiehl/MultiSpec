# MultiSpec

# Description: 
MultiSpec is a freeware image data analysis application developed for interactively analyzing Earth observational multispectral and hyperspectral image data from airborne and spaceborne systems, as well as a number of other types of multispectral image data, such as medical images on MacOS and Windows platforms. MultiSpec has also been integrated with the GABBs geospatial data solutions (termed MultiSpec Online) to provide users with a more seamless experience by using within a web browser. MultiSpec is programmed in C/C++; the wxWidgets library was used for the GUI interface to run in the linux environment on the HUBzero platform. The software uses several other libraries such as gdal, hdf4, hdf5, netcdf and OpenJPEG to handle the many different file formats for remote sensing image data. 
# Installation:
  General Information
  
    - SharedMultiSpec directory includes those files which are used for Linux, MacOS, and Windows builds
    - wxMultiSpec directory includes those files which are used for the Linux and MacOS builds to manage the user interface
    - Windows directory includes those files which are used for the Windows build to manage the user interface
    - the SMultiSpec.h include file includes directives to 'turn off' access to other libraries including hdf4, gdal, and hdf5.
  
## Linux: 
This version has always been created from the command line with a makefile. The source is also available from MultiSpec Online (https://mygeohub.org/resources/multispec). Note that a stand alone Linux version has not been created to be able to test how the process will work.
  
    - The code for the Linux version used for MultiSpec Online uses the following library versions. These libraries are not provided:
      - wxWidgets-3.1.2-gtk2 library is used for the graphical user interface
      - HDF4 ??
      - gdal-1.11.2
      - hdf5 ??
      - netcdf ??
      - openjpeg-2.1.2
  
  The makefile that is provided includes accesses several libraries that are available on mygeohub including wxWidgets. The directories needed for the Linux version are SharedMultiSpec, wxMultiSpec and Linux_build (Linux_wxwidgets). One will need to include the wxWidgets library to make the basic version of MultiSpec compile for Linux.
  
## MacOS: 
Xcode verson 9.2 was used to create the project file included in this repository (MacOS_build) in MacOS 10.13.6.
  
    - The code for the full MacOS version uses the following library versions. Only the wxWidgets library is provided:
      - wxWidgets-3.1.2 library is used for the graphical user interface
      - HDF4.2r2
      - gdal-2.1.2
      - hdf5-1.8.8
      - netcdf-4.2
      - openjpeg-2.1.2
      - libecwj2-3.3
  
  The MultiSpec_basic.xcodeproj file is set up for just the core version of MultiSpec without the libraries (except that for  wxWidgets). The directories in the git archive needed for the MacOS version are ShareMultiSpec, wxMultiSpec, MacOS_build, and MacOS_wxWidgets. The MacOS_wxWidgets directory contains a zip file with the wxWidgets code needed to create the wxWidgets library for just MacOS. Unzip the wxWidgets archive and move the resulting wxWidgets-3.1.2_basic_MacOS folder to the same level as the MacOS_build folder. There will be four directories (folders) at the same level in your development environment: MacOS_build, SharedMultiSpec, wxMultiSpec, and WxWidgets-3.1.2_basic_MacOS. Unpack the MultiSpec_basic.xcodeproj.zip file inside of the MacOS_build.
  
  The MultiSpec.xcodeproj.zip archive contains the xcode project for the full version of MultiSpec with the libraries. One can sort of see how the libraries are organized but you will need to provide them.  
  
## WindowsOS: 
Visual Studio 2015 was used to create the project file included in this repository (Windows_build) in Windows7 OS.
  
    - The code for the full Windows version uses the following library versions. These libraries are not provided:
      - HDF4.2r2
      - gdal-2.1.2
      - hdf5-1.8.8
      - netcdf-4.2
      - openjpeg-2.3.1
      - libecwj2-3.3
       
  The multispec_basic.vcxproj file is set up for just the core version of MultiSpec without the libraries. You may need to set the flag to x64 to create the 64-bit version if your version of Visual Studio does not default to x64. The directories needed for the Windows version are SharedMultiSpec, Windows, and Windows_build.
  
# License

                                     MultiSpec
                   Copyright 1988-2020 Purdue Research Foundation

 Licensed under the Apache License, Version 2.0 (the "License"); you may not use
 this software except in compliance with the License. You may obtain a copy of the
 License at:  https://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software distributed
 under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
 language governing permissions and limitations under the License.

 MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
 Purdue University in West Lafayette, IN and licensed by Larry Biehl.
