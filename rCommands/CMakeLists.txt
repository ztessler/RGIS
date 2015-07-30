set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ../CMlib/include ../DBlib/include ../RGlib/include ../MFlib/include)

foreach(loop_var dm2rgis ds2rgis dsAggregate dsDuration dsSampling getHeader grdAppendLayers grdBoxAggr grdCalculate grdCellStats grdCreateNetwork
grdCycleMean grdDateLayers grdDifference grdExtractLayers grdImport grdMerge grdMinMax grdNoNeg grdOperation grdReclassDisc grdRenameLayers grdRunningMean
grdSeasonAggr grdSeasonMean grdTimeSeries grdTSAggr grdZoneHist grdZoneStats netAccumulate netBasinDistrib netBasinHist netBasinProf netBasinStats netBuild
netcdf2ds netcdf2rgis netCells2Grid netCellSearch netCellSlopes netConfluence netCreatePnts netDefragment netErosion netImportASCII netInvAccum netTransfer
netTrim netUnaccumulate netUpstreamAvg pntGridSampling pntGridValue pntStationDist pntSTNChar pntSubbasCenter pntSubbasHist pntSubbasins pntSubbasStats
pntSymbolField rgis2asciigrid rgis2dm rgis2domain rgis2ds rgis2netcdf rgis2table rgisMetadb setHeader table2rgis tblAddField tblCalculate tblConv2Point
tblConvCoord tblDecimalDate tblDeleteField tblDeleteRec tblIdleRec tblJoinTables tblMergeCoord tblMergeDate tblMJoinTables tblRedefField tblSelectRec
tblSortRec tblSplitDate tblStatistics tblTopoCalc )
	add_executable(${loop_var} src/CMD${loop_var}.C)
	if(${CMAKE_HOST_APPLE})
		target_link_libraries(${loop_var} LINK_PUBLIC RG22 DB22 MF10 CM10 -lnetcdf -ludunits2 -lm)
	else(${CMAKE_HOST_APPLE})
		target_link_libraries(${loop_var} LINK_PUBLIC RG22 DB22 MF10 CM10 -lnetcdf -ludunits2 -lm -pthread)
	endif(${CMAKE_HOST_APPLE})
	install (TARGETS ${loop_var} RUNTIME DESTINATION ghaas/bin)
endforeach(loop_var)
