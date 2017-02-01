#!/bin/bash

if [[ "${RGIS_FUNCTIONS}" == "sourced" ]]
then
	return 0
else
	export RGIS_FUNCTIONS="sourced"
fi

if [[ "${1}" == "gzipped" ]]
then
    __RGIS_GZEXT=".gz"
else
    __RGIS_GZEXT=""
fi

function RGISlookupSubject ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${variable}" in
		(air_temperature)                            #  0
			echo "AirTemperature"
		;;
		(min_air_temperature)                         #  1
			echo "AirTemperature-Min"
		;;
		(max_air_temperature)                         #  2
			echo "AirTemperature-Max"
		;;
		(diurnal_air_temperature_range)               #  3
			echo "AirTemperature-Range"
		;;
		(c_litterfall)                                #  4
			echo "C-LitterFall"
		;;
		(cell_area)                                   #  5
			echo "CellArea"
		;;
		(cloud_cover)                                 #  6
			echo "CloudCover"
		;;
		(continents)                                  #  7
			echo "Continents"
		;;
		(countries)                                   #  8
			echo "Countries"
		;;
		(states)                                      #  9
			echo "States"
		;;
		(counties)                                    # 10
			echo "Counties"
		;;
		(cropland_fraction)                           # 11
			echo "Cropland-Fraction"
		;;
		(cropping_intensity)                          # 12
			echo "Crops-Intensity"
		;;
		(daily_precipitation_fraction)                # 13
			echo "Precipitation-DailyFraction"
		;;
		(din_areal_loading)                           # 14
			echo "DIN-AreaLoading"
		;;
		(din_point_loading)                           # 15
			echo "DIN-PointLoading"
		;;
		(discharge)                                   # 16
			echo "Discharge"
		;;
		(doc_areal_loading)                           # 17
			echo "DOC-ArealLoading"
		;;
		(doc_point_loading)                           # 18
			echo "DOC-PointLoading"
		;;
		(don_areal_loading)                           # 19
			echo "DON-ArealLoading"
		;;
		(don_point_loading)                           # 20
			echo "DON-PointLoading"
		;;
		(elevation)                                   # 21
			echo "Elevation"
		;;
		(min_elevation)                               # 22
			echo "Elevation-Min"
		;;
		(max_elevation)                               # 23
			echo "Elevation-Max"
		;;
		(field_capacity)                              # 24
			echo "Soil-FieldCapacity"
		;;
		(growing_season1)                             # 25
			echo "Crops-GrowingSeason1"
		;;
		(growing_season2)                             # 26
			echo "Crops-GrowingSeason2"
		;;
		(heatindex)                                   # 27
			echo "Heatindex"
		;;
		(max_heatindex)                               # 28
			echo "Heatindex-Max"
		;;
		(min_heatindex)                               # 29
			echo "Heatindex-Min"
		;;
		(dewpoint_temperature)                        # 30
			echo "Humidity-Dewpoint"
		;;
		(relative_humidity)                           # 31
			echo "Humidity-Relative"
		;;
		(specific_humidity)                           # 32
			echo "Humidity-Specific"
		;;
		(vapor_pressure)                              # 33
			echo "Humidity-VaporPressure"
		;;
		(irrigated_area_fraction)                     # 34
			echo "Irrigation-AreaFraction"
		;;
		(irrigation_efficiency)                       # 35
			echo "Irrigation-Efficiency"
		;;
		(irrigation_gross_demand)                     # 36
			echo "IrrGrossDemand"
		;;
		(landcover_barren_percentage)                 # 37
			echo "Landcover-PctBarren"
		;;
		(landcover_deciduous_broadleaf_percentage)    # 38
			echo "Landcover-PctBroadleafDeciduous"
		;;
		(landcover_evergreen_broadleaf_percentage)    # 39
			echo "Landcover-PctBroadleafEvergreen"
		;;
		(landcover_deciduous_conifer_percentage)      # 40
			echo "Landcover-PctConiferDeciduous"
		;;
		(landcover_evergreen_conifer_percentage)      # 41
			echo "Landcover-PctConiferEvergreen"
		;;
		(landcover_cropland_percentage)               # 42
			echo "Landcover-PctCropland"
		;;
		(landcover_crop-natural_mosaic_percentage)    # 43
			echo "Landcover-PctCropNaturalMosaic"
		;;
		(landcover_grassland_percentage)              # 44
			echo "Landcover-PctGrassland"
		;;
		(landcover_mixed_forest_percentage)           # 45
			echo "Landcover-PctMixedForest"
		;;
		(landcover_mixed_savannah_percentage)         # 46
			echo "Landcover-PctSavannah"
		;;
		(landcover_mixed_woody_savannah_percentage)   # 47
			echo "Landcover-PctSavannahWoody"
		;;
		(landcover_mixed_closed_shrubland_percentage) # 48
			echo "Landcover-PctShrublandClosed"
		;;
		(landcover_mixed_open_shrubland_percentage)   # 49
			echo "Landcover-PctShrublandOpen"
		;;
		(landcover_mixed_snow_ice_percentage)         # 50
			echo "Landcover-PctSnowIce"
		;;
		(landcover_urban_built-up_percentage)         # 51
			echo "Landcover-PctUrbanBuiltUp"
		;;
		(landcover_vegetation_percentage)             # 52
			echo "Landcover-PctVegetation"
		;;
		(landcover_water_percentage)                  # 53
			echo "Landcover-PctWater"
		;;
		(landcover_wetland_percentage)                # 54
			echo "Landcover-PctWetland"
		;;
		(network)                                     # 55
			echo "Network"
		;;
		(n_litterfall)                                # 56
			echo "N-LitterFall"
		;;
		(nh4_areal_loading)                           # 57
			echo "NH4-ArealLoading"
		;;
		(nh4_point_loading)                           # 58
			echo "Nh4-PointLoading"
		;;
		(no3_areal_loading)                           # 59
			echo "NO3-ArealLoading"
		;;
		(no3_point_loading)                           # 60
			echo "NO3-PointLoading"
		;;
		(ocean_basins)                                # 61
			echo "OceanBasins"
		;;
		(other_crop_fraction)                         # 62
			echo "CropFraction-Other"
		;;
		(perennial_crop_fraction)                     # 63
			echo "CropFraction-Perennial"
		;;
		(population)                                  # 64
			echo "Population"
		;;
		(population_density)	                      # 65
			echo "Population-Density"
		;;
		(population-rural)                            # 66
			echo "Population-Rural"
		;;
		(population-urban)                            # 67
			echo "Population-Urban"
		;;
		(precipitation)                               # 68
			echo "Precipitation"
		;;
		(shortwave_downwelling_radiation)             # 69
			echo "Radiation-ShortWave"
		;;
		(longwave_downwelling_radiation)              # 70
			echo "Radiation-LongWave"
		;;
		(rain_fall)                                   # 71
			echo "RainFall"
		;;
		(rain_pet)                                    # 72
			echo "RainPET"
		;;
		(reservoir_capacity)                          # 73
			echo "Reservoir-Capacity"
		;;
		(rice_crop_fraction)                          # 74
			echo "CropFraction-Rice"
		;;
		(rice_percolation_rate)                       # 75
			echo "Crops-RicePercolationRate"
		;;
		(rooting_depth)                               # 76
			echo "Soil-RootingDepth"
		;;
		(runoff)                                      # 77
			echo "Runoff"
		;;
		(small_reservoir_coefficient)                 # 78
			echo "SmallReservoir-Coefficient"
		;;
		(snow_fall)                                   # 79
			echo "SnowFall"
		;;
		(soil_moisture)                               # 80
			echo "SoilMoisture"
		;;
		(soil_moisture_change)                        # 81
			echo "SoilMoistureChange"
		;;
		(surface_air_pressure)                        # 82
			echo "AirPressure"
		;;
		(vegetables_crop_fraction)                    # 83
			echo "CropFraction-Vegetables"
		;;
		(wilting_point)                               # 84
			echo "Soil-WiltingPoint"
		;;
		(wind_speed)                                  # 85
			echo "WindSpeed"
		;;
		(*)
			echo "${variable}"
		;;
	esac
}

function _RGISlookupFullName ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${variable}" in
		(air_temperature)                             #  0
			echo "Air Temperature"
		;;
		(max_air_temperature)                         #  1
			echo "Maximum Air Temperature"
		;;
		(min_air_temperature)                         #  2
			echo "Minimum Air Temperature"
		;;
		(diurnal_air_temperature_range)               #  3
			echo "Diurnal AirTemperature Range"
		;;
		(c_litterfall)                                #  4
			echo "C LitterFall"
		;;
		(cell_area)                                   #  5
			echo "Cell Area"
		;;
		(cloud_cover)                                 #  6
			echo "Cloud Cover"
		;;
		(continents)                                  #  7
			echo "Continents"
		;;
		(countries)                                   #  8
			echo "Countries"
		;;
		(states)                                      #  9
			echo "States"
		;;
		(counties)                                    # 10
			echo "Counties"
		;;
		(cropland_fraction)                           # 11
			echo "Cropland Fraction"
		;;
		(cropping_intensity)                          # 12
			echo "Cropping Intensity"
		;;
		(din_areal_loading)                           # 13
			echo "DIN Areal Loading"
		;;
		(din_point_loading)                           # 14
			echo "DIN Point Loading"
		;;
		(discharge)                                   # 15
			echo "Discharge"
		;;
		(don_areal_loading)                           # 16
			echo "DON Areal Loading"
		;;
		(doc_point_loading)                           # 17
			echo "DOC Point Loading"
		;;
		(doc_areal_loading)                           # 18
			echo "DOC Areal Loading"
		;;
		(don_point_loading)                           # 19
			echo "DON Point Loading"
		;;
		(daily_precipitation_fraction)                # 20
			echo "Daily Precipitation Fraction"
		;;
		(elevation)                                   # 21
			echo "Elevation"
		;;
		(max_elevation)                               # 22
			echo "Maximum Elevation"
		;;
		(min_elevation)                               # 23
			echo "Minimum Elevation"
		;;
		(field_capacity)                              # 24
			echo "Field Capacity"
		;;
		(growing_season1)                             # 25
			echo "Growing Season 1"
		;;
		(growing_season2)                             # 26
			echo "Growing Season 2"
		;;
		(heatindex)                                   # 27
			echo "Heatindex"
		;;
		(max_heatindex)                               # 28
			echo "Maximum Heatindex"
		;;
		(min_heatindex)                               # 29
			echo "Minimum Heatindex"
		;;
		(dewpoint_temperature)                        # 30
			echo "Dewpoint Temperature"
		;;
		(relative_humidity)                           # 31
			echo "Relative Humidity"
		;;
		(specific_humidity)                           # 32
			echo "Specific Humidity"
		;;
		(vapor_pressure)                              # 33
			echo "Vapor Pressure"
		;;
		(irrigated_area_fraction)                     # 34
			echo "Irrigated Area Fraction"
		;;
		(irrigation_efficiency)                       # 35
			echo "Irrigation Efficiency"
		;;
		(irrigation_gross_demand)                     # 36
			echo "Irrigation Gross Demand"
		;;
		(landcover_barren_percentage)                 # 37
			echo "Barren Percentage"
		;;
		(landcover_deciduous_broadleaf_percentage)    # 38
			echo "Deciduous Broadleaf Percentage"
		;;
		(landcover_evergreen_broadleaf_percentage)    # 39
			echo "Evergreen Broadleaf Percentage"
		;;
		(landcover_deciduous_conifer_percentage)      # 40
			echo "Deciduous Conifer Percentage"
		;;
		(landcover_evergreen_conifer_percentage)      # 41
			echo "Evergreen Conifer Percentage"
		;;
		(landcover_cropland_percentage)               # 42
			echo "Cropland Percentage"
		;;
		(landcover_crop-natural_mosaic_percentage)    # 43
			echo "Cropland-Natural Mosaic Percentage"
		;;
		(landcover_grassland_percentage)              # 44
			echo "Grassland Percentage"
		;;
		(landcover_mixed_forest_percentage)           # 45
			echo "Mixed Forest Percentage"
		;;
		(landcover_mixed_savannah_percentage)         # 46
			echo "Savannah Percentage"
		;;
		(landcover_mixed_woody_savannah_percentage)   # 47
			echo "Woody Savannah Percentage"
		;;
		(landcover_mixed_closed_shrubland_percentage) # 48
			echo "Closed Shrubland Percentage"
		;;
		(landcover_mixed_open_shrubland_percentage)   # 49
			echo "Open Shrubland Percentage"
		;;
		(landcover_mixed_snow_ice_percentage)         # 50
			echo "Snow-Ice Percentage"
		;;
		(landcover_urban_built-up_percentage)         # 51
			echo "Urban Built-Up Percentage"
		;;
		(landcover_vegetation_percentage)             # 52
			echo "Vegetation Fraction"
		;;
		(landcover_water_percentage)                  # 53
			echo "Water Percentage"
		;;
		(landcover_wetland_percentage)                # 54
			echo "Wetland Percentage"
		;;
		(network)                                     # 55
			echo "STNetwork"
		;;
		(n_litterfall)                                # 56
			echo "N LitterFall"
		;;
		(nh4_areal_loading)                           # 57
			echo "NH4 Areal Loading"
		;;
		(nh4_point_loading)                           # 58
			echo "NH4 Point Loading"
		;;
		(no3_areal_loading)                           # 59
			echo "NO3 Areal Loading"
		;;
		(no3_point_loading)                           # 60
			echo "NO3 Point Loading"
		;;
		(ocean_basins)                                # 61
			echo "Ocean Basins"
		;;
		(other_crop_fraction)                         # 62
			echo "Other Crop Fraction"
		;;
		(perennial_crop_fraction)                     # 63
			echo "Perennial Crop Fraction"
		;;
		(population)                                  # 64
			echo "Population"
		;;
		(population_density)                          # 65
			echo "Population Density"
		;;
		(population-rural)                            # 66
			echo "Rural Population"
		;;
		(population-urban)                            # 67
			echo "Urban Population"
		;;
		(precipitation)                               # 68
			echo "Precipitation"
		;;
		(shortwave_downwelling_radiation)             # 69
			echo "Short Wave Radiation"
		;;
		(longwave_downwelling_radiation)              # 70
			echo "Long Wave Radiation"
		;;
		(rain_fall)                                   # 71
			echo "Rain Fall"
		;;
		(rain_pet)                                    # 72
			echo "Rain Potential ET"
		;;
		(reservoir_capacity)                          # 73
			echo "Reservoir Capacity"
		;;
		(rice_crop_fraction)                          # 74
			echo "Rice Crop Fraction"
		;;
		(rice_percolation_rate)                       # 75
			echo "Rice Percolation Rate"
		;;
		(rooting_depth)                               # 76
			echo "Rooting Depth"
		;;
		(runoff)                                      # 77
			echo "Runoff"
		;;
		(small_reservoir_coefficient)                 # 78
			echo "Small-reservoir Coefficient"
		;;
		(snow_fall)                                   # 79
			echo "Snow Fall"
		;;
		(soil_moisture)                               # 80
			echo "Soil Moisture"
		;;
		(soil_moisture_change)                        # 81
			echo "Soil Moisture Change"
		;;
		(surface_air_pressure)                        # 82
			echo "Air Pressure"
		;;
		(vegetables_crop_fraction)                    # 83
			echo "Vegetables Crop Fraction"
		;;
		(wilting_point)                               # 84
			echo "Wilting Point"
		;;
		(wind_speed)                                  # 85
			echo "Wind Speed"
		;;
		(*)
			echo "${variable}"
		;;
	esac
}

function RGISlookupShadeset ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${variable}" in
		(air_temperature|max_air_temperature|min_air_temperature) # 0 1 2
			echo "blue-to-red"
		;;
		(diurnal_air_temperature_range)               #  3
			echo "grey"
		;;
		(c_litterfall)                                #  4
			echo "grey"
		;;
		(cell_area)                                   #  5
			echo "grey"
		;;
		(cloud_cover)                                 #  6
			echo "grey"
		;;
		(continents)                                  #  7
			echo "grey"
		;;
		(countries)                                   #  8
			echo "grey"
		;;
		(states)                                      #  9
			echo "grey"
		;;
		(counties)                                    # 10
			echo "grey"
		;;
		(cropland_fraction)                           # 11
			echo "grey"
		;;
		(cropping_intensity)                          # 12
			echo "grey"
		;;
		(daily_precipitation_fraction)                # 13
			echo "grey"
		;;
		(din_areal_loading)                           # 14
			echo "grey"
		;;
		(din_point_loading)                           # 15
			echo "grey"
		;;
		(discharge)                                   # 16
			echo "blue"
		;;
		(don_areal_loading)                           # 17
			echo "grey"
		;;
		(don_point_loading)                           # 18
			echo "grey"
		;;
		(doc_areal_loading)                           # 19
			echo "grey"
		;;
		(doc_point_loading)                           # 20
			echo "grey"
		;;
		(elevation|max_elevation|min_elevation)       # 21 22 23
			echo "elevation"
		;;
		(field_capacity)                              # 24
			echo "grey"
		;;
		(growing_season1)                             # 25
			echo "grey"
		;;
		(growing_season2)                             # 26
			echo "grey"
		;;
		(heatindex)                                   # 27
			echo "blue-to-red"
		;;
		(max_heatindex)                               # 28
			echo "blue-to-red"
		;;
		(min_heatindex)                               # 29
			echo "blue-to-red"
		;;
		(dewpoint_temperature)                        # 30
			echo "blue"
		;;
		(relative_humidity)                           # 31
			echo "blue"
		;;
		(specific_humidity)                           # 32
			echo "blue"
		;;
		(vapor_pressure)                              # 33
			echo "blue"
		;;
		(irrigated_area_fraction)                     # 34
			echo "grey"
		;;
		(irrigation_efficiency)                       # 35
			echo "grey"
		;;
		(irrigation_gross_demand)                     # 36
			echo "grey"
		;;
		(landcover_barren_percentage)                 # 37
			echo "grey"
		;;
		(landcover_deciduous_broadleaf_percentage)    # 38
			echo "grey"
		;;
		(landcover_evergreen_broadleaf_percentage)    # 39
			echo "grey"
		;;
		(landcover_deciduous_conifer_percentage)      # 40
			echo "grey"
		;;
		(landcover_evergreen_conifer_percentage)      # 41
			echo "grey"
		;;
		(landcover_cropland_percentage)               # 42
			echo "grey"
		;;
		(landcover_crop-natural_mosaic_percentage)    # 43
			echo "grey"
		;;
		(landcover_grassland_percentage)              # 44
			echo "grey"
		;;
		(landcover_mixed_forest_percentage)           # 45
			echo "grey"
		;;
		(landcover_mixed_savannah_percentage)         # 46
			echo "grey"
		;;
		(landcover_mixed_woody_savannah_percentage)   # 47
			echo "grey"
		;;
		(landcover_mixed_closed_shrubland_percentage) # 48
			echo "grey"
		;;
		(landcover_mixed_open_shrubland_percentage)   # 49
			echo "grey"
		;;
		(landcover_mixed_snow_ice_percentage)         # 50
			echo "grey"
		;;
		(landcover_urban_built-up_percentage)         # 51
			echo "grey"
		;;
		(landcover_vegetation_percentage)             # 52
			echo "grey"
		;;
		(landcover_water_percentage)                  # 53
			echo "grey"
		;;
		(landcover_wetland_percentage)                # 54
			echo "grey"
		;;
		(network)                                     # 55
			echo "grey"
		;;
		(n_litterfall)                                # 56
			echo "grey"
		;;
		(nh4_areal_loading)                           # 57
			echo "grey"
		;;
		(nh4_point_loading)                           # 58
			echo "grey"
		;;
		(no3_areal_loading)                           # 59
			echo "grey"
		;;
		(no3_point_loading)                           # 60
			echo "grey"
		;;
		(ocean_basins)                                # 61
			echo "grey"
		;;
		(other_crop_fraction)                         # 62
			echo "grey"
		;;
		(perennial_crop_fraction)                     # 63
			echo "grey"
		;;
		(population)                                  # 64
			echo "grey"
		;;
		(population_density)                          # 65
			echo "grey"
		;;
		(population-rural)                            # 66
			echo "grey"
		;;
		(population-urban)                            # 67
			echo "grey"
		;;
		(precipitation)                               # 68
			echo "blue"
		;;
		(shortwave_downwelling_radiation)             # 69
			echo "grey"
		;;
		(longwave_downwelling_radiation)              # 70
			echo "grey"
		;;
		(rain_fall)                                   # 71
			echo "blue"
		;;
		(rain_pet)                                    # 72
			echo "grey"
		;;
		(reservoir_capacity)                          # 73
			echo "grey"
		;;
		(rice_crop_fraction)                          # 74
			echo "grey"
		;;
		(rice_percolation_rate)                       # 75
			echo "grey"
		;;
		(rooting_depth)                               # 76
			echo "grey"
		;;
		(runoff)                                      # 77
			echo "blue"
		;;
		(small_reservoir_coefficient)                 # 78
			echo "grey"
		;;
		(snow_fall)                                   # 79
			echo "grey"
		;;
		(soil_moisture)                               # 80
			echo "grey"
		;;
		(soil_moisture_change)                        # 81
			echo "blue-to-red"
		;;
		(surface_air_pressure)                        # 82
			echo "grey"
		;;
		(vegetables_crop_fraction)                    # 83
			echo "grey"
		;;
		(wilting_point)                               # 84
			echo "grey"
		;;
		(wind_speed)                                  # 85
			echo "grey"
		;;
		(*)
			echo "grey"
		;;
	esac
}

function RGISlookupAggrMethod ()
{
	local variable=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${variable}" in
		(air_temperature|max_air_temperature|min_air_temperature)  # 0 1 2
			echo "avg"
		;;
		(diurnal_air_temperature_range)               #  3
			echo "avg"
		;;
		(c_litterfall)                                #  4
			echo "sum"
		;;
		(cell_area)                                   #  5
			echo "avg"
		;;
		(cloud_cover)                                 #  6
			echo "avg"
		;;
		(continents)                                  #  7
			echo "N/A"
		;;
		(countries)                                   #  8
			echo "N/A"
		;;
		(states)                                      #  9
			echo "N/A"
		;;
		(counties)                                    # 10
			echo "N/A"
		;;
		(cropland_fraction)                           # 11
			echo "avg"
		;;
		(cropping_intensity)                          # 12
			echo "avg"
		;;
		(daily_precipitation_fraction)                # 13
			echo "avg"
		;;
		(din_areal_loading)                           # 14
			echo "sum"
		;;
		(din_point_loading)                           # 15
			echo "sum"
		;;
		(discharge)                                   # 16
			echo "avg"
		;;
		(don_areal_loading)                           # 17
			echo "sum"
		;;
		(don_point_loading)                           # 18
			echo "sum"
		;;
		(doc_areal_loading)                           # 19
			echo "sum"
		;;
		(doc_point_loading)                           # 20
			echo "sum"
		;;
		(elevation|max_elevation|min_elevation)       # 21 22 23
			echo "avg"
		;;
		(field_capacity)                              # 24
			echo "avg"
		;;
		(growing_season1)                             # 25
			echo "avg"
		;;
		(growing_season2)                             # 26
			echo "avg"
		;;
		(heatindex)                                   # 27
			echo "avg"
		;;
		(max_heatindex)                               # 28
			echo "avg"
		;;
		(min_heatindex)                               # 29
			echo "avg"
		;;
		(dewpoint_temperature)                        # 30
			echo "avg"
		;;
		(relative_humidity)                           # 31
			echo "avg"
		;;
		(specific_humidity)                           # 32
			echo "avg"
		;;
		(vapor_pressure)                              # 33
			echo "avg"
		;;
		(irrigated_area_fraction)                     # 34
			echo "avg"
		;;
		(irrigation_efficiency)                       # 35
			echo "avg"
		;;
		(irrigation_gross_demand)                     # 36
			echo "sum"
		;;
		(landcover_barren_percentage)                 # 37
			echo "avg"
		;;
		(landcover_deciduous_broadleaf_percentage)    # 38
			echo "avg"
		;;
		(landcover_evergreen_broadleaf_percentage)    # 39
			echo "avg"
		;;
		(landcover_deciduous_conifer_percentage)      # 40
			echo "avg"
		;;
		(landcover_evergreen_conifer_percentage)      # 41
			echo "avg"
		;;
		(landcover_cropland_percentage)               # 42
			echo "avg"
		;;
		(landcover_crop-natural_mosaic_percentage)    # 43
			echo "avg"
		;;
		(landcover_grassland_percentage)              # 44
			echo "avg"
		;;
		(landcover_mixed_forest_percentage)           # 45
			echo "avg"
		;;
		(landcover_mixed_savannah_percentage)         # 46
			echo "avg"
		;;
		(landcover_mixed_woody_savannah_percentage)   # 47
			echo "avg"
		;;
		(landcover_mixed_closed_shrubland_percentage) # 48
			echo "avg"
		;;
		(landcover_mixed_open_shrubland_percentage)   # 49
			echo "avg"
		;;
		(landcover_mixed_snow_ice_percentage)         # 50
			echo "avg"
		;;
		(landcover_urban_built-up_percentage)         # 51
			echo "avg"
		;;
		(landcover_vegetation_percentage)             # 52
			echo "avg"
		;;
		(landcover_water_percentage)                  # 53
			echo "avg"
		;;
		(landcover_wetland_percentage)                # 54
			echo "avg"
		;;
		(network)                                     # 55
			echo "avg"
		;;
		(n_litterfall)                                # 56
			echo "sum"
		;;
		(nh4_areal_loading)                           # 57
			echo "sum"
		;;
		(nh4_point_loading)                           # 58
			echo "sum"
		;;
		(no3_areal_loading)                           # 59
			echo "sum"
		;;
		(no3_point_loading)                           # 60
			echo "sum"
		;;
		(ocean_basins)                                # 61
			echo "N/A"
		;;
		(other_crop_fraction)                         # 62
			echo "avg"
		;;
		(perennial_crop_fraction)                     # 63
			echo "avg"
		;;
		(population)                                  # 64
			echo "sum"
		;;
		(population_density)                          # 65
			echo "avg"
		;;
		(population-rural)                            # 66
			echo "avg"
		;;
		(population-urban)                            # 67
			echo "avg"
		;;
		(precipitation)                               # 68
			echo "sum"
		;;
		(shortwave_downwelling_radiation)             # 69
			echo "avg"
		;;
		(longwave_downwelling_radiation)              # 70
			echo "avg"
		;;
		(rain_fall)                                   # 71
			echo "sum"
		;;
		(rain_pet)                                    # 72
			echo "sum"
		;;
		(reservoir_capacity)                          # 73
			echo "avg"
		;;
		(rice_crop_fraction)                          # 74
			echo "avg"
		;;
		(rice_percolation_rate)                       # 75
			echo "avg"
		;;
		(rooting_depth)                               # 76
			echo "avg"
		;;
		(runoff)                                      # 77
			echo "sum"
		;;
		(small_reservoir_coefficient)                 # 78
			echo "avg"
		;;
		(snow_fall)                                   # 79
			echo "avg"
		;;
		(soil_moisture)                               # 80
			echo "avg"
		;;
		(soil_moisture_change)                        # 81
			echo "avg"
		;;
		(surface_air_pressure)                        # 82
			echo "avg"
		;;
		(vegetables_crop_fraction)                    # 83
			echo "avg"
		;;
		(wilting_point)                               # 84
			echo "avg"
		;;
		(wind_speed)                                  # 85
			echo "avg"
		;;
		(*)
			echo "avg"
			echo "Defaulting aggregation method: ${variable}" > /dev/stderr
		;;
	esac
}

function RGISlookupTimeStep ()
{
	local timeStep=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${timeStep}" in
		(daily)
			echo "day"
		;;
		(monthly)
			echo "month"
		;;
		(annual)
			echo "year"
		;;
	esac
}

function RGIStimeNumberOfDays ()
{
	local    year="${1}"
	local   month=$((${2} - 1))
	local century="${year%??}"

	local monthLength[0]=31
	local monthLength[1]=28
	local monthLength[2]=31
	local monthLength[3]=30
	local monthLength[4]=31
	local monthLength[5]=30
	local monthLength[6]=31
	local monthLength[7]=31
	local monthLength[8]=30
	local monthLength[9]=31
	local monthLength[10]=30
	local monthLength[11]=31

	if (( month == 1))
	then
		if [[ "${year}" == "${century}00" ]]
		then
			local leapYear=$(( (${century} & 0xffffe) == ${century} ? 1 : 0))
		else
			local leapYear=$(( (${year}    & 0xffffe) == ${year}    ? 1 : 0))
		fi
	else
		local leapYear=0
	fi
	echo $((${monthLength[${month}]} + ${leapYear}))
}

function _RGISvariableDir ()
{
	local      archive="${1}"
	local       domain="${2}"
	local     variable="${3}"

	local       varDir=$(RGISlookupSubject "${variable}")

	if [ -e "${archive}/${domain%+}/${varDir}" ]
	then
		echo "${domain%+}/${varDir}"
		return 0
	else
		if [ "${domain%+}" == "${domain}" ] # Exact domain
		then
			echo "Missing ${archive}/${domain}/${varDir}" > /dev/stderr
			echo ""
			return 1
		else # Dynamic domain
			local parent="${archive}/${domain%+}/parent"
			if [ -e "${parent}" ]
			then
				_RGISvariableDir "${archive}" "$(cat ${archive}/${domain%+}/parent)+" "${variable}" || return 1
			else
				echo "Missing ${parent}" > /dev/stderr
				echo ""
				return 1
			fi
		fi
	fi
	return 0
}

function _RGISresolutionDir ()
{
	local      archive="${1}"
	local       domain="${2}"
	local     variable="${3}"
	local      product="${4}"
	local   resolution="${5}"

	local varDir=$(_RGISvariableDir "${archive}" "${domain}" "${variable}")
	if [ "${varDir}" == "" ]
	then
		return
	else
		if [ -e "${archive}/${varDir}/${product}/${resolution%+}" ]
		then
			echo "${varDir}/${product}/${resolution%+}"
		else
			if [ "${resolution%+}" == "${resolution}" ] # Exact resolution
			then
				echo "Missing ${archive}/${varDir}/${product}/${resolution}" > /dev/stderr
				echo ""
				return 1
			else # Dynamic resolution
				if [ "${resolution%km+}" == "${resolution}" ] # Geographic
				then
					local geogNum=0
					local geogRes[${geogNum}]="15sec";  (( ++geogNum ))
					local geogRes[${geogNum}]="30sec";  (( ++geogNum ))
					local geogRes[${geogNum}]="01min";  (( ++geogNum ))
					local geogRes[${geogNum}]="03min";  (( ++geogNum ))
					local geogRes[${geogNum}]="05min";  (( ++geogNum ))
					local geogRes[${geogNum}]="06min";  (( ++geogNum ))
					local geogRes[${geogNum}]="10min";  (( ++geogNum ))
					local geogRes[${geogNum}]="15min";  (( ++geogNum ))
					local geogRes[${geogNum}]="30min";  (( ++geogNum ))
					local geogRes[${geogNum}]="60min";  (( ++geogNum ))
					local geogRes[${geogNum}]="90min";  (( ++geogNum ))
					local geogRes[${geogNum}]="150min"; (( ++geogNum ))

					for (( num = 0; num < ${geogNum}; ++num ))
					do
						if [ "${geogRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < geogNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${geogRes[((${num} + 1))]}+" || return 1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return 1
					fi
				else # Cartesian
					local cartNum=0
					local cartRes[${cartNum}]="1km";    (( ++cartNum ))
					local cartRes[${cartNum}]="2km";    (( ++cartNum ))
					local cartRes[${cartNum}]="5km";    (( ++cartNum ))
					local cartRes[${cartNum}]="10km";   (( ++cartNum ))
					local cartRes[${cartNum}]="25km";   (( ++cartNum ))
					local cartRes[${cartNum}]="50km";   (( ++cartNum ))

					for (( num = 0; num < ${cartNum}; ++num ))
					do
						if [ "${cartRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < cartNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${cartRes[((${num} + 1))]}+" || return 1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return 1
					fi
				fi
			fi
		fi
	fi
	return 0
}
function _RGIStStepDir ()
{
	local tStepType="${1}"
	local     tStep="${2}"

	case "${tStepType}" in
		(TS)
			case "${tStep}" in
				(hourly)
					echo "Hourly"
				;;
				(3hourly)
					echo "3Hourly"
				;;
				(6hourly)
					echo "6Hourly"
				;;
				(daily)
					echo "Daily"
				;;
				(monthly)
					echo "Monthly"
				;;
				(annual)
					echo "Annual"
				;;
				(*)
					echo "${tStep}"
				;;
			esac
		;;
		(LT|LTmin|LTmax|LTslope|LTrange|LTstdDev|Stats|static)
			echo "Static"
		;;
	esac
	return 0
}

function RGISdirectoryPath ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local  tStepType="${6}"
	local      tStep=$(echo "${7}" | tr "[A-Z]" "[a-z]")

	local        dir=$(_RGIStStepDir ${tStepType} ${tStep})
	local     varDir=$(RGISlookupSubject "${variable}")

	echo "${archive}/${domain}/${varDir}/${product}/${resolution}/${dir}"
}

function RGISdirectory ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local  tStepType="${6}"
	local      tStep=$(echo "${7}" | tr "[A-Z]" "[a-z]")

	local dir=$(_RGIStStepDir ${tStepType} ${tStep})

	local resDir=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}")
	if [ "${resDir}" == "" ]
	then
		return 1
	else
		echo "${archive}/${resDir}/${dir}"
	fi
	return 0
}

function RGISfileExtension ()
{
	local     variable="${1}"
	case "${variable}" in
		(network)
			local extension="gdbn"
		;;
		(continents)
			local extension="gdbd"
		;;
		(countries)
			local extension="gdbd"
		;;
		(states)
			local extension="gdbd"
		;;
		(counties)
			local extension="gdbd"
		;;
		(ocean_basins)
			local extension="gdbd"
		;;
		(*)
			local extension="gdbc"
		;;
    esac
    echo "${extension}"
}

function RGISfilePath ()
{
	local      archive="${1}"
	local       domain="${2}"
	local     variable="${3}"
	local      product="${4}"
	local   resolution="${5}"
	local    tStepType="${6}"
	local        tStep=$(echo "${7}" | tr "[A-Z]" "[a-z]")
	local    timeRange="${8}"

	case "${tStep}" in
		(hourly)
			local tStepStr="h"
		;;
		(3hourly)
			local tStepStr="3h"
		;;
		(6hourly)
			local tStepStr="6h"
		;;
		(daily)
			local tStepStr="d"
		;;
		(monthly)
			local tStepStr="m"
		;;
		(annual)
			local tStepStr="a"
		;;
		("")
			local tStepStr=""
		;;
		(*)
			echo "Unknown time step ${tStep}" > /dev/stderr
			return 1
		;;
	esac

    local extension=$(RGISfileExtension "${variable}")

	local rgisDirectory=$(RGISdirectoryPath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(RGISdirectoryPath            "" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(echo ${fileName} | sed "s:/::" | sed "s:/:_:g" )
	local      fileName=${fileName%_*}
	if [ "${rgisDirectory}" == "" ]
	then
		echo ""
		return 1
	fi
	if [[ "${tStepType}" == "static" ]]
	then
		local tStepType="Static"
		local tStepStr=""
	fi

	echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}${__RGIS_GZEXT}"
}

function RGISfile ()
{
	local      archive="${1}"
	local       domain="${2}"
	local     variable="${3}"
	local      product="${4}"
	local   resolution="${5}"
	local    tStepType="${6}"
	local        tStep=$(echo "${7}" | tr "[A-Z]" "[a-z]")
	local    timeRange="${8}"

	case "${tStep}" in
		(hourly)
			local tStepStr="h"
		;;
		(3hourly)
			local tStepStr="3h"
		;;
		(6hourly)
			local tStepStr="6h"
		;;
		(daily)
			local tStepStr="d"
		;;
		(monthly)
			local tStepStr="m"
		;;
		(annual)
			local tStepStr="a"
		;;
		("")
			local tStepStr=""
		;;
		(*)
			echo "Unknown time step ${tStep}" > /dev/stderr
			return 1
		;;
	esac

    local extension=$(RGISfileExtension "${variable}")

	local rgisDirectory=$(RGISdirectory      "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" | sed "s:/:_:g" )
	if [ "${rgisDirectory}" == "" ]
	then
		echo ""
		return 1
	fi
	if [[ "${tStepType}" == "static" ]]
	then
		local tStepType="Static"
		local tStepStr=""
	fi

#	local  variableName=$(RGISlookupSubject "${variable}")
	echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}${__RGIS_GZEXT}"
}

function RGIStitle ()
{
	local     domain="${1}"
	local   variable="${2}"
	local    product="${3}"
	local resolution="${4}"
	local  tStepType="${5}"
	local      tStep=$(echo "${6}" | tr "[A-Z]" "[a-z]")
	local  timeRange="${7}"
	local    version="${8}"


	local variableFullName=$(_RGISlookupFullName "${variable}")

	if [[ "${tStepType}" == "static" ]]
	then
		local tStepString=""
	else
		case "${tStep}" in
			(hourly)
				local tStepStr="Hourly"
			;;
			(3hourly)
				local tStepStr="3Hourly"
			;;
			(6hourly)
				local tStepStr="6Hourly"
			;;
			(daily)
				local tStepStr="Daily"
			;;
			(monthly)
				local tStepStr="Monthly"
			;;
			(annual)
				local tStepStr="Annual"
			;;
			(*)
				echo "Unknown time step ${tStep}"  > /dev/stderr
				return 1
			;;
		esac
		if [[ "${timeRange}" == "" ]]
		then
			local tStepString=", ${tStepStr}${tStepType}"
		else
			local tStepString=", ${tStepStr}${tStepType}, ${timeRange}"
		fi
	fi
	echo "${domain}, ${product} ${variableFullName} (${resolution}${tStepString}) V${version}"
	return 0
}

function RGISAppend ()
{
   local    archive="${1}"
   local     domain="${2}"
   local   variable="${3}"
   local    product="${4}"
   local resolution="${5}"
   local    version="${6}"
	local  startyear="${7}"
	local    endyear="${8}"
   local      tStep="${9}"

 	local      files=""
	local  separator=" "

	for (( year = ${startyear}; year <= ${endyear}; ++year ))
	do
		local  filename=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${year}")
		local     files="${files}${separator}${filename}"
		local separator=" "
	done

	local filename=$(RGISfile  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${startyear}-${endyear}")
	local    title=$(RGIStitle              "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${startyear}-${endyear}" "${version}")
	local  subject=$(RGISlookupSubject  "${variable}")
   local shadeset=$(RGISlookupShadeset "${variable}")

	grdAppendLayers -t "${title}" -d "${domain}" -u "${subject}" -v "${version}" -o "${filename}" ${files}
}

function RGISAggregateTS ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
	local  startyear="${7}"
	local    endyear="${8}"
	local  fromtStep="${9}"
	local    totStep="${10}"

   local     files=""
   local separator=""

   for ((year = ${startyear}; year <= ${endyear} ; ++year))
   do
      local fromFile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${fromtStep}" "${year}")
      local   toFile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${totStep}"   "${year}")
      local    title=$(RGIStitle "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${totStep}" "${year}" "${version}")
      local  subject=$(RGISlookupSubject    "${variable}")
      local shadeset=$(RGISlookupShadeset   "${variable}")
		local   method=$(RGISlookupAggrMethod "${variable}")

      grdTSAggr -a "${method}" -e "$(RGISlookupTimeStep ${totStep})" -t "${title}" -d "${domain}" -u "${subject}" -s "${shadeset}" "${fromFile}" "${toFile}" || return 1
   done
}

function RGISClimatology ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
 	local  startyear="${7}"
	local    endyear="${8}"

	local     ltDir=$(RGISdirectory "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT" "monthly")
	local    tsFile=$(RGISfile      "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "monthly" "${startyear}-${endyear}")
	local    ltFile=$(RGISfile      "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT" "monthly" "${startyear}-${endyear}")
	local     title=$(RGIStitle                  "${domain}" "${variable}" "${product}" "${resolution}" "LT" "monthly" "${startyear}-${endyear}")
	local   subject=$(RGISlookupSubject  "${variable}")
	local  shadeset=$(RGISlookupShadeset "${variable}")

	[ -e "${ltDir}" ] || mkdir -p "${ltDir}" || return 1
	grdCycleMean	-t "${title}" -d "${domain}" -u "${subject}" -v "${version}" -s "${shadeset}" -n 12 "${tsFile}" - |\
	grdDateLayers   -e "month" - "${ltFile}" || return 1
	return 0
}

function RGISCellStats ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
 	local  startyear="${7}"
	local    endyear="${8}"

	local  statsDir=$(RGISdirectory  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual")
	local    tsFile=$(RGISfile       "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual" "${startyear}-${endyear}")
	local statsFile=$(RGISfile       "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}")
	local     title=$(RGIStitle                   "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}" "${version}")
	local   subject=$(RGISlookupSubject "${variable}")

	[ -e "${statsDir}" ] || mkdir -p "${statsDir}" || return 1
	grdCellStats    -t "${title}" -u "${variable}" -d "${domain}" -v "${version}" "${tsFile}" "${statsFile}" || return 1

	local  annualLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "avg" -l "avg" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${annualLTfile}" || return 1

	local     maxLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Maximum" -l "Maximum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${maxLTfile}" || return 1

	local     minLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Minimum" -l "Minimum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${minLTfile}" || return 1

	local   rangeLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Range"   -l "Range"   -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${rangeLTfile}" || return 1

	local   slopeLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "SigSlopeB1" -l "SigSlopeB1" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${slopeLTfile}" || return 1

	local  stdDevLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "StdDev" -l "StdDev" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${stdDevLTfile}" || return 1

#	rm "${statsFile}"
	return 0
}

function RGISStatistics ()
{
	local    archive="${1}"
	local     domain="${2}"
	local   variable="${3}"
	local    product="${4}"
	local resolution="${5}"
	local    version="${6}"
	local  startyear="${7}"
	local    endyear="${8}"

	RGISAppend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "monthly" || return 1
	RGISAppend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "annual"  || return 1

	RGISClimatology "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return 1
	RGISCellStats   "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return 1
	local  annualTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual"  "${startyear}-${endyear}")
	local monthlyTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "monthly" "${startyear}-${endyear}")
	[ -e  "${annualTSfile}" ] && rm  "${annualTSfile}"
	[ -e "${monthlyTSfile}" ] && rm "${monthlyTSfile}"
	return 0
}
