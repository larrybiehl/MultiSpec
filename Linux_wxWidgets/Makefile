WXCXXFLAGS=$(shell . /etc/environ.sh;use -e -r wxwidgets-3.1.2-gtk2; wx-config --cxxflags;use -e -r gdal-1.11.2)
WXLDFLAGS=$(shell . /etc/environ.sh;use -e -r wxwidgets-3.1.2-gtk2; wx-config --libs;use -e -r gdal-1.11.2)
CXX=g++
GDALINC=$(shell . /etc/environ.sh;use -e -r gdal-1.11.2; echo $${GDAL_INCLUDE})
GDALLIB=$(shell . /etc/environ.sh;use -e -r gdal-1.11.2; echo $${GDAL_LIB})
OPNJINC=$(shell . /etc/environ.sh;use -e -r openjpeg-2.1.2; echo $${OPENJPEG_INC})
#OPNJLIB=$(shell . /etc/environ.sh;use -e -r openjpeg-2.1.2; echo $${OPNJ_LIB})
CXXFLAGS=-c -w $(WXCXXFLAGS) 
LDFLAGS=-L$(GDALLIB) -L /apps/share64/debian7/openjpeg/2.1.2/lib -lgdal -L/usr/lib -lhdf5 -lmfhdfalt -ldfalt -lopenjp2 $(WXLDFLAGS) /apps/share64/debian7/wxwidgets/wxwidget-3.1.2-gtk2/lib/libwxscintilla-3.1.a
INCLUDES=-ICommonMultiSpec -Ilinux_files -ISharedMultiSpec -IGDAL_Modified\
         -I$(GDALINC)/frmts/hdf5 -I$(GDALINC) -I$(GDALINC)/frmts/gtiff/libgeotiff\
         -I$(GDALINC)/ogr/ogrsf_frmts/avc\
		 -I/usr/include/hdf -I/$(GDALINC)/frmts/gtiff -I/$(OPNJINC)\
		  -I$(GDALINC)/frmts/pcidsk -I$(GDALINC)/frmts/pcidsk/sdk -I$(GDALINC)/frmts/pcidsk/sdk/core
#		 -I/usr/include/hdf -I/$(GDALINC)/frmts/gtiff -I/apps/share64/debian7/openjpeg/2.1.2/include

DEFS=-Dmultispec_lin -Dmultispec_wxlin
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=multispec

# Directory path
CDIR=CommonMultiSpec
LDIR=linux_files
SDIR=SharedMultiSpec
GDIR=GDAL_Modified
HDIR=HDF4_Modified

# File names
COMMON_FILES=$(CDIR)/CDisplay.cpp $(CDIR)/CFileStream.cpp $(CDIR)/CHistogram.cpp \
             $(CDIR)/CImageWindow.cpp $(CDIR)/CPalette.cpp \
             $(CDIR)/CStubs.cpp $(CDIR)/CWindowInfo.cpp

LINUX_FILES=$(LDIR)/LAbout.cpp $(LDIR)/LBiplotDialog.cpp $(LDIR)/LChangeChannelDescriptionDialog.cpp \
				 $(LDIR)/LChannelsDialog.cpp $(LDIR)/LClassesDialog.cpp $(LDIR)/LClassGroupDialog.cpp \
				 $(LDIR)/LClassifyCEMDialog.cpp $(LDIR)/LClassifyCorrelationDialog.cpp $(LDIR)/LClassifyDialog.cpp \
				 $(LDIR)/LClassifyEchoDialog.cpp $(LDIR)/LClassifyKNNDialog.cpp \
				 $(LDIR)/LClassPairWeightsDialog.cpp $(LDIR)/LClassWeightsDialog.cpp \
				 $(LDIR)/LClusterDialog.cpp $(LDIR)/LClusterIsodataDialog.cpp $(LDIR)/LClusterSinglePassDialog.cpp \
				 $(LDIR)/LCoordinateBar.cpp $(LDIR)/LDialog.cpp $(LDIR)/LDisplayMinMaxDialog.cpp \
				 $(LDIR)/LDisplayMultispectralDialog.cpp $(LDIR)/LDisplayThematicDialog.cpp $(LDIR)/LDrawObjects.cpp \
				 $(LDIR)/LEditClassFieldDialog.cpp \
				 $(LDIR)/LEditClassGroupDialog.cpp $(LDIR)/LEditSelectionDialog.cpp $(LDIR)/LEnhanceStatisticsDialog.cpp \
				 $(LDIR)/LEvaluateCovariancesDialog.cpp $(LDIR)/LEvaluateTransformationDialog.cpp \
				 $(LDIR)/LFalseColorDialog.cpp $(LDIR)/LFeatureExtractionDialog.cpp \
				 $(LDIR)/LFeatureSelectionDialog.cpp $(LDIR)/LFieldsToThematicDialog.cpp $(LDIR)/LFileFormatDialog.cpp \
				 $(LDIR)/LGaussianParameterDlg.cpp \
				 $(LDIR)/LGraphDoc.cpp $(LDIR)/LGraphFrame.cpp $(LDIR)/LGraphView.cpp \
				 $(LDIR)/LGraphWindow_img.cpp $(LDIR)/LHistogramDialog.cpp $(LDIR)/LImage_dialog.cpp \
				 $(LDIR)/LImageCanvas.cpp $(LDIR)/LImageDoc.cpp $(LDIR)/LImageFrame.cpp $(LDIR)/LImageView.cpp \
				 $(LDIR)/LLeaveOneOutMixingDialog.cpp \
				 $(LDIR)/LLegendList.cpp $(LDIR)/LLegendView.cpp $(LDIR)/LListDataDialog.cpp \
				 $(LDIR)/LListResultsDialog.cpp $(LDIR)/LListResultsOptionsDialog.cpp $(LDIR)/LMainFrame.cpp \
				 $(LDIR)/LMapCoordinateDialog.cpp $(LDIR)/LMosaicTwoImagesDialog.cpp $(LDIR)/LMultiSpec.cpp \
				 $(LDIR)/LNewClassFieldDialog.cpp $(LDIR)/LOneColumnDialog.cpp $(LDIR)/LOpenFileDialog.cpp \
				 $(LDIR)/LOverlayParametersDialog.cpp $(LDIR)/LPrincipalComponentsDialog.cpp \
				 $(LDIR)/LProjectionPursuitDialog.cpp $(LDIR)/LRecodeThematicImageDialog.cpp $(LDIR)/LReformatChangeDialog.cpp \
				 $(LDIR)/LReformatRectifyDialog.cpp $(LDIR)/LReformatTransformDialog.cpp $(LDIR)/LStatisticsDialog.cpp \
				 $(LDIR)/LStatisticsDoc.cpp $(LDIR)/LStatisticsFrame.cpp $(LDIR)/LStatisticsView.cpp \
				 $(LDIR)/LStatisticsHistogramDialog.cpp $(LDIR)/LStatisticsImageDialog.cpp $(LDIR)/LStatisticsListDialog.cpp \
				 $(LDIR)/LStatisticsOptionsDialog.cpp $(LDIR)/LStatusDialog.cpp \
				 $(LDIR)/LTextDoc.cpp $(LDIR)/LTextFrame.cpp $(LDIR)/LTextView.cpp $(LDIR)/LTitleBar.cpp \
				 $(LDIR)/LToolbar_img.cpp $(LDIR)/LTools.cpp $(LDIR)/LUtilities.cpp

SHARED_FILES=$(SDIR)/SArcView.cpp $(SDIR)/SBiPlotData.cpp $(SDIR)/SClassify.cpp \
				 $(SDIR)/SClassifyDialogs.cpp $(SDIR)/SClassifyEcho.cpp $(SDIR)/SClassifyEchoControl.cpp \
				 $(SDIR)/SClassifyEchoMemory.cpp $(SDIR)/SClassifyEchoPhase.cpp $(SDIR)/SClassifyEchoUtilities.cpp \
				 $(SDIR)/SCluster.cpp $(SDIR)/SClusterIsodata.cpp $(SDIR)/SClusterSinglePass.cpp \
				 $(SDIR)/SCoordinateView.cpp $(SDIR)/SDialogUtilities.cpp $(SDIR)/SDisplay.cpp \
				 $(SDIR)/SDisplayInfo.cpp $(SDIR)/SDisplayMultispectral.cpp $(SDIR)/SDisplayThematic.cpp \
				 $(SDIR)/SDisplay16_24Bits.cpp $(SDIR)/SDisplay4_8ByteData.cpp $(SDIR)/SEditStatistics.cpp \
				 $(SDIR)/SEllipsePlotV.cpp $(SDIR)/SFeatureExtraction.cpp $(SDIR)/SFeatureExtractionMath.cpp \
				 $(SDIR)/SFeatureSelection.cpp \
				 $(SDIR)/SFieldsToThematicFile.cpp $(SDIR)/SFileInfo.cpp $(SDIR)/SFileIO.cpp \
				 $(SDIR)/SGAIARoutines.cpp $(SDIR)/SGDALInterface.cpp $(SDIR)/SGeoTIFF.cpp \
				 $(SDIR)/SGraphic.cpp $(SDIR)/SGraphUtilities.cpp $(SDIR)/SGraphView.cpp \
				 $(SDIR)/SHDF4.cpp $(SDIR)/SHDF5.cpp $(SDIR)/SHistogram.cpp \
				 $(SDIR)/SHistogramInfo.cpp $(SDIR)/SImageOverlays.cpp $(SDIR)/SImagine.cpp \
				 $(SDIR)/SListData.cpp $(SDIR)/SListResults.cpp $(SDIR)/SLOOCovariance.cpp \
				 $(SDIR)/SMapCoordinates.cpp \
				 $(SDIR)/SMapProjectionAlgorithms.cpp $(SDIR)/SMask.cpp $(SDIR)/SMatlab.cpp \
				 $(SDIR)/SMatrixUtilities.cpp $(SDIR)/SMemoryUtilities.cpp $(SDIR)/SMenus.cpp \
             $(SDIR)/SMosaic.cpp $(SDIR)/SNDFFormat.cpp $(SDIR)/SOpenFileDialog.cpp \
             $(SDIR)/SOpenImage.cpp $(SDIR)/SOther.cpp $(SDIR)/SOutlineFields.cpp \
             $(SDIR)/SPalette.cpp $(SDIR)/SPrincipalComponents.cpp  $(SDIR)/SProcessorMemory.cpp \
             $(SDIR)/SProject.cpp $(SDIR)/SProjectComputeStatistics.cpp $(SDIR)/SProjectFileIO.cpp \
             $(SDIR)/SProjectHistogramStatistics.cpp $(SDIR)/SProjectListStatistics.cpp \
             $(SDIR)/SProjectMatrixUtilities.cpp \
             $(SDIR)/SProjectUtilities.cpp $(SDIR)/SProjectionPursuit.cpp $(SDIR)/SRecodeThematicImage.cpp \
             $(SDIR)/SRectifyImage.cpp $(SDIR)/SReformatChangeImageFileFormat.cpp $(SDIR)/SReformatUtilities.cpp \
             $(SDIR)/SRegistration.cpp $(SDIR)/SSaveWrite.cpp $(SDIR)/SSelectionGraph.cpp \
             $(SDIR)/SSelectionUtility.cpp $(SDIR)/SShapeToThematic.cpp \
             $(SDIR)/SStatistics.cpp $(SDIR)/SStatisticsEnhancement.cpp \
				 $(SDIR)/SStatisticsImage.cpp $(SDIR)/SStatisticsImageAlgorithms.cpp \
             $(SDIR)/SStrings.cpp $(SDIR)/SThematicWindow.cpp $(SDIR)/SThreads.cpp \
             $(SDIR)/SUtilities.cpp $(SDIR)/SWindow.cpp $(SDIR)/SWindowInfo.cpp \
			    $(SDIR)/SReadHDFHeader.cpp

MGDAL_FILES=$(GDIR)/hdf5dataset.cpp $(GDIR)/gh5_convenience.cpp $(GDIR)/hdf5imagedataset.cpp \
             $(GDIR)/geotiff.cpp $(GDIR)/gdalallregister.cpp \
             $(GDIR)/openjpegdataset.cpp \
             $(GDIR)/pcidskdataset2.cpp $(GDIR)/sysvirtualfile.cpp

MHDF4_FILES=$(HDIR)/hfile.cpp			 

SOURCES=$(COMMON_FILES) $(LINUX_FILES) $(SHARED_FILES) $(MGDAL_FILES) 

all: $(EXECUTABLE)

install: all
	cp $(EXECUTABLE) $(LDIR)/LStringTable.def ../bin/ 
    
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFS) $< -o $@

clean:
	rm -f $(CDIR)/*.o $(LDIR)/*.o $(SDIR)/*.o $(GDIR)/*.o $(HDIR)/*.o
	rm -f $(CDIR)/*.dep $(LDIR)/*.dep $(SDIR)/*.dep $(GDIR)/*.dep $(HDIR)/*.dep
	rm -f $(EXECUTABLE)

distclean: clean
	rm -f ../bin/$(EXECUTABLE) ../bin/LStringTable.def
 
%.dep: %.cpp
	$(CXX) -M $(DEFS) $(INCLUDES) $(CXXFLAGS) $< > $@

-include $(OBJECTS:.o=.dep)

