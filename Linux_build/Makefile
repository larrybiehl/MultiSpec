WXCXXFLAGS=$(shell . /etc/environ.sh;use -e -r wxwidgets-3.1.2-gtk2; wx-config --cxxflags;use -e -r gdal-1.11.2)
WXLDFLAGS=$(shell . /etc/environ.sh;use -e -r wxwidgets-3.1.2-gtk2; wx-config --libs;use -e -r gdal-1.11.2)
CXX=g++
GDALINC=$(shell . /etc/environ.sh;use -e -r gdal-1.11.2; echo $${GDAL_INCLUDE})
GDALLIB=$(shell . /etc/environ.sh;use -e -r gdal-1.11.2; echo $${GDAL_LIB})
OPNJINC=$(shell . /etc/environ.sh;use -e -r openjpeg-2.1.2; echo $${OPENJPEG_INC})
#OPNJLIB=$(shell . /etc/environ.sh;use -e -r openjpeg-2.1.2; echo $${OPNJ_LIB})
CXXFLAGS=-c -w $(WXCXXFLAGS) 
LDFLAGS=-L$(GDALLIB) -L /apps/share64/debian7/openjpeg/2.1.2/lib -lgdal -L/usr/lib -lhdf5 -lmfhdfalt -ldfalt -lopenjp2 $(WXLDFLAGS) /apps/share64/debian7/wxwidgets/wxwidget-3.1.2-gtk2/lib/libwxscintilla-3.1.a
INCLUDES=-Ilinux_files -ISharedMultiSpec -IGDAL_Modified\
         -I$(GDALINC)/frmts/hdf5 -I$(GDALINC) -I$(GDALINC)/frmts/gtiff/libgeotiff\
         -I$(GDALINC)/ogr/ogrsf_frmts/avc\
		 -I/usr/include/hdf -I/$(GDALINC)/frmts/gtiff -I/$(OPNJINC)\
		  -I$(GDALINC)/frmts/pcidsk -I$(GDALINC)/frmts/pcidsk/sdk -I$(GDALINC)/frmts/pcidsk/sdk/core
#		 -I/usr/include/hdf -I/$(GDALINC)/frmts/gtiff -I/apps/share64/debian7/openjpeg/2.1.2/include

DEFS=-Dmultispec_wx -Dmultispec_wxlin
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=multispec

# Directory path
LDIR=linux_files
SDIR=SharedMultiSpec
GDIR=GDAL_Modified
HDIR=HDF4_Modified

# File names
LINUX_FILES=$(LDIR)/xAbout.cpp $(LDIR)/xBiplotDialog.cpp $(LDIR)/xChangeChannelDescriptionDialog.cpp \
				 $(LDIR)/xChannelsDialog.cpp $(LDIR)/xClassesDialog.cpp $(LDIR)/xClassGroupDialog.cpp \
				 $(LDIR)/xClassifyCEMDialog.cpp $(LDIR)/xClassifyCorrelationDialog.cpp $(LDIR)/xClassifyDialog.cpp \
				 $(LDIR)/xClassifyEchoDialog.cpp $(LDIR)/xClassifyKNNDialog.cpp $(LDIR)/xClassifySVMDialog.cpp \
				 $(LDIR)/xClassPairWeightsDialog.cpp $(LDIR)/xClassWeightsDialog.cpp \
				 $(LDIR)/xClusterDialog.cpp $(LDIR)/xClusterIsodataDialog.cpp $(LDIR)/xClusterSinglePassDialog.cpp \
				 $(LDIR)/xCoordinateBar.cpp $(LDIR)/xCursor_images.cpp $(LDIR)/xDialog.cpp \
				 $(LDIR)/xDialog_images.cpp $(LDIR)/xDisplayMinMaxDialog.cpp \
				 $(LDIR)/xDisplayMultispectralDialog.cpp $(LDIR)/xDisplayThematicDialog.cpp $(LDIR)/xDrawObjects.cpp \
				 $(LDIR)/xEditClassFieldDialog.cpp \
				 $(LDIR)/xEditClassGroupDialog.cpp $(LDIR)/xEditSelectionDialog.cpp $(LDIR)/xEnhanceStatisticsDialog.cpp \
				 $(LDIR)/xEvaluateCovariancesDialog.cpp $(LDIR)/xEvaluateTransformationDialog.cpp \
				 $(LDIR)/xFalseColorDialog.cpp $(LDIR)/xFeatureExtractionDialog.cpp \
				 $(LDIR)/xFeatureSelectionDialog.cpp $(LDIR)/xFieldsToThematicDialog.cpp $(LDIR)/xFileFormatDialog.cpp \
				 $(LDIR)/xGaussianParameterDialog.cpp \
				 $(LDIR)/xGraphDoc.cpp $(LDIR)/xGraphFrame.cpp $(LDIR)/xGraphView.cpp \
				 $(LDIR)/xGraphWindow_images.cpp $(LDIR)/xHistogramDialog.cpp \
				 $(LDIR)/xImageCanvas.cpp $(LDIR)/xImageDoc.cpp $(LDIR)/xImageFrame.cpp $(LDIR)/xImageView.cpp \
				 $(LDIR)/xLeaveOneOutMixingDialog.cpp \
				 $(LDIR)/xLegendList.cpp $(LDIR)/xLegendView.cpp $(LDIR)/xListDataDialog.cpp \
				 $(LDIR)/xListResultsDialog.cpp $(LDIR)/xListResultsOptionsDialog.cpp $(LDIR)/xMainFrame.cpp \
				 $(LDIR)/xMapCoordinateDialog.cpp $(LDIR)/xMosaicTwoImagesDialog.cpp $(LDIR)/xMultiSpec.cpp \
				 $(LDIR)/xNewClassFieldDialog.cpp $(LDIR)/xOneColumnDialog.cpp $(LDIR)/xOpenFileDialog.cpp \
				 $(LDIR)/xOverlayParametersDialog.cpp $(LDIR)/xPrincipalComponentsDialog.cpp \
				 $(LDIR)/xProjectionPursuitDialog.cpp $(LDIR)/xRecodeThematicImageDialog.cpp $(LDIR)/xReformatChangeDialog.cpp \
				 $(LDIR)/xReformatRectifyDialog.cpp $(LDIR)/xReformatTransformDialog.cpp $(LDIR)/xStatisticsDialog.cpp \
				 $(LDIR)/xStatisticsDoc.cpp $(LDIR)/xStatisticsFrame.cpp $(LDIR)/xStatisticsView.cpp \
				 $(LDIR)/xStatisticsHistogramDialog.cpp $(LDIR)/xStatisticsImageDialog.cpp $(LDIR)/xStatisticsListDialog.cpp \
				 $(LDIR)/xStatisticsOptionsDialog.cpp $(LDIR)/xStatusDialog.cpp \
				 $(LDIR)/xTextDoc.cpp $(LDIR)/xTextFrame.cpp $(LDIR)/xTextView.cpp $(LDIR)/xTitleBar.cpp \
				 $(LDIR)/xToolBar_images.cpp $(LDIR)/xTools.cpp $(LDIR)/xUtilities.cpp

SHARED_FILES=$(SDIR)/SArcView.cpp $(SDIR)/SBiPlotData.cpp $(SDIR)/SClassify.cpp \
				 $(SDIR)/SClassifyDialogs.cpp $(SDIR)/SClassifyEcho.cpp $(SDIR)/SClassifyEchoControl.cpp \
				 $(SDIR)/SClassifyEchoMemory.cpp $(SDIR)/SClassifyEchoPhase.cpp $(SDIR)/SClassifyEchoUtilities.cpp \
				 $(SDIR)/SCluster.cpp $(SDIR)/SClusterIsodata.cpp $(SDIR)/SClusterSinglePass.cpp \
				 $(SDIR)/SCoordinateView.cpp $(SDIR)/SDialogUtilities.cpp $(SDIR)/SDisplay.cpp \
				 $(SDIR)/SDisplayInfo.cpp $(SDIR)/SDisplay_class.cpp $(SDIR)/SDisplayMultispectral.cpp \
				 $(SDIR)/SDisplayThematic.cpp $(SDIR)/SDisplay16_24Bits.cpp $(SDIR)/SDisplay4_8ByteData.cpp \
				 $(SDIR)/SEditStatistics.cpp $(SDIR)/SEllipsePlotV.cpp $(SDIR)/SFeatureExtraction.cpp \
				 $(SDIR)/SFeatureExtractionMath.cpp  $(SDIR)/SFeatureSelection.cpp $(SDIR)/SFieldsToThematicFile.cpp \
				 $(SDIR)/SFileInfo.cpp $(SDIR)/SFileIO.cpp $(SDIR)/SFileStream_class.cpp \
				 $(SDIR)/SGAIARoutines.cpp $(SDIR)/SGDALInterface.cpp $(SDIR)/SGeoTIFF.cpp \
				 $(SDIR)/SGraphic.cpp $(SDIR)/SGraphUtilities.cpp $(SDIR)/SGraphView.cpp \
				 $(SDIR)/SHDF4.cpp $(SDIR)/SHDF5.cpp $(SDIR)/SHistogram.cpp \
				 $(SDIR)/SHistogramInfo.cpp $(SDIR)/SHistogram_class.cpp $(SDIR)/SImageOverlays.cpp \
				 $(SDIR)/SImageWindow_class.cpp $(SDIR)/SImagine.cpp $(SDIR)/SListData.cpp \
				 $(SDIR)/SListResults.cpp $(SDIR)/SLOOCovariance.cpp $(SDIR)/SMapCoordinates.cpp \
				 $(SDIR)/SMapProjectionAlgorithms.cpp $(SDIR)/SMask.cpp $(SDIR)/SMatlab.cpp \
				 $(SDIR)/SMatrixUtilities.cpp $(SDIR)/SMemoryUtilities.cpp $(SDIR)/SMenus.cpp \
             $(SDIR)/SMosaic.cpp $(SDIR)/SNDFFormat.cpp $(SDIR)/SOpenFileDialog.cpp \
             $(SDIR)/SOpenImage.cpp $(SDIR)/SOther.cpp $(SDIR)/SOutlineFields.cpp \
             $(SDIR)/SPalette.cpp $(SDIR)/SPalette_class.cpp $(SDIR)/SPrincipalComponents.cpp \
             $(SDIR)/SProcessorMemory.cpp $(SDIR)/SProject.cpp $(SDIR)/SProjectComputeStatistics.cpp \
             $(SDIR)/SProjectFileIO.cpp $(SDIR)/SProjectHistogramStatistics.cpp $(SDIR)/SProjectListStatistics.cpp \
             $(SDIR)/SProjectMatrixUtilities.cpp $(SDIR)/SProjectUtilities.cpp $(SDIR)/SProjectionPursuit.cpp \
             $(SDIR)/SReadHDFHeader.cpp $(SDIR)/SRecodeThematicImage.cpp $(SDIR)/SRectifyImage.cpp \
             $(SDIR)/SReformatChangeImageFileFormat.cpp $(SDIR)/SReformatUtilities.cpp $(SDIR)/SRegistration.cpp \
             $(SDIR)/SSaveWrite.cpp $(SDIR)/SSelectionGraph.cpp $(SDIR)/SSelectionUtility.cpp \
             $(SDIR)/SShapeToThematic.cpp $(SDIR)/SStatistics.cpp $(SDIR)/SStatisticsEnhancement.cpp \
				 $(SDIR)/SStatisticsImage.cpp $(SDIR)/SStatisticsImageAlgorithms.cpp $(SDIR)/SStrings.cpp \
				 $(SDIR)/SStubs.cpp $(SDIR)/Ssvm.cpp $(SDIR)/SThematicWindow.cpp \
				 $(SDIR)/SThreads.cpp $(SDIR)/SUtilities.cpp $(SDIR)/SWindow.cpp \
				 $(SDIR)/SWindowInfo.cpp $(SDIR)/SWindowInfo_class.cpp 

MGDAL_FILES=$(GDIR)/hdf5dataset.cpp $(GDIR)/gh5_convenience.cpp $(GDIR)/hdf5imagedataset.cpp \
             $(GDIR)/geotiff.cpp $(GDIR)/gdalallregister.cpp \
             $(GDIR)/openjpegdataset.cpp \
             $(GDIR)/pcidskdataset2.cpp $(GDIR)/sysvirtualfile.cpp

MHDF4_FILES=$(HDIR)/hfile.cpp			 

SOURCES=$(LINUX_FILES) $(SHARED_FILES) $(MGDAL_FILES) 

all: $(EXECUTABLE)

install: all
	cp $(EXECUTABLE) $(LDIR)/xStringTable.def ../bin/ 
    
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFS) $< -o $@

clean:
	rm -f $(LDIR)/*.o $(SDIR)/*.o $(GDIR)/*.o $(HDIR)/*.o
	rm -f $(LDIR)/*.dep $(SDIR)/*.dep $(GDIR)/*.dep $(HDIR)/*.dep
	rm -f $(EXECUTABLE)

distclean: clean
	rm -f ../bin/$(EXECUTABLE) ../bin/xStringTable.def
 
%.dep: %.cpp
	$(CXX) -M $(DEFS) $(INCLUDES) $(CXXFLAGS) $< > $@

-include $(OBJECTS:.o=.dep)

