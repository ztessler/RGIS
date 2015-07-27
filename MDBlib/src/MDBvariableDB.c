/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

MDBvariableDB.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <mdb.h>

static struct {
	char *CFname;
	char *GHAASname;
	char *LongName;
	char *Shadeset;
	char *Aggregation;
} _CMvariableLookupTable [] =
{ 	"air_temperature",                  "AirTemperature",              "Air Temperature",                 "blue-to-red", "avg",
	"min_air_temperature",              "AirTemperature-Min",          "Minimum Air Temperature",         "blue-to-red", "min",
	"max_air_temperature",              "AirTemperature-Max",          "Maximum Air Temperature",         "blue-to-red", "max",
	"diurnal_air_temperature_range",    "AirTemperature-Range",        "Diurnal Air Temperature Range",   "grey",        "max",
	"c_litterfall",                     "C-LitterFall",                "Carbon Litterfall",               "grey",        "sum",
	"cell_area",                        "CellArea",                    "Cell Area",                       "grey",        "avg",
	"cloud_cover",                      "CloudCover",                  "Cloud Cover",                     "grey",        "avg",
	"continents"                        "Continents",                  "Continents",                      "standard",    "avg",
	"cropland_fraction"                 "Cropland-Fraction",           "Crop Fraction",                   "grey",        "avg",
	"cropping_intensity"                "Crops-Intensity",             "Crops Intensity",                 "grey",        "avg",
	"daily_precipitation_fraction",     "Precipitation-DailyFraction", "Precipitation Daily Fraction",    "grey",        "avg",
	"din_areal_loading",                "DIN-AreaLoading",             "DIN Areal Loading",               "grey",        "sum",
	"din_point_loading",                "DIN-PointLoading",            "DIN Point Loading",               "grey",        "sum",
	"discharge"                         "Discharge",                   "Discharge",                       "blue",        "avg",
	"doc_areal_loading",                "DOC-ArealLoading",            "DOC Areal Loading",               "grey",        "sum",
	"doc_point_loading",                "DOC-PointLoading",            "DOC Point Loading",               "grey",        "sum",
	"don_areal_loading",                "DON-ArealLoading",            "DON Areal Loading",               "grey",        "sum",
	"don_point_loading",                "DON-PointLoading",            "DON Point Loading",               "grey",        "sum",
	"elevation",                        "Elevation",                   "Elevation",                       "elevation",   "avg",
	"min_elevation",                    "Elevation-Min",               "Minimum Elevation",               "elevation",   "min",
	"max_elevation",                    "Elevation-Max",               "Maximum Elevation",               "elevation",   "max",
	"field_capacity",                   "Soil-FieldCapacity",          "Soil Field Capacity",             "grey",        "avg",
	"growing_season1",                  "Crops-GrowingSeason1",        "Growing Season 1",                "grey",        "avg",         
	"growing_season2",                  "Crops-GrowingSeason2",        "Growing Season 2",                "grey",        "avg",
	"specific_humidity",                "Humidity-Specific",           "Specific Humidity",               "blue",        "avg",
	"irrigated_area_fraction",          "Irrigation-AreaFraction",     "Irrigation Area Fraction",        "grey",        "avg",
	"irrigation_efficiency",            "Irrigation-Efficiency",       "Irrigation Efficiency",           "grey",        "avg",
	"irrigation_gross_demand"           "IrrGrossDemand",              "Irrigation Gross Demand",         "blue",        "sum",
	"network",                          "Network",                     "Network",                         "standard",    "avg",
	"n_litterfall"                      "N-LitterFall",                "Nitrogen Litterfall",             "grey",        "sum",
	"nh4_areal_loading",                "NH4-ArealLoading",            "Ammonia Areal Loading",           "grey",        "sum",
	"nh4_point_loading",                "Nh4-PointLoading",            "Ammonia Point Loading",           "grey",        "sum",
	"no3_areal_loading",                "NO3-ArealLoading",            "Nitrate Areal Loading",           "grey",        "sum",
	"no3_point_loading",                "NO3-PointLoading",            "Nitrate Point Loading",           "grey",        "sum",
	"no2_areal_loading",                "NO2-ArealLoading",            "Nitrite Areal Loading",           "grey",        "sum",
	"no2_point_loading",                "NO2-PointLoading",            "Nitrite Point Loading",           "grey",        "sum",
	"ocean_basins",                     "OceanBasins",                 "Ocean Basins",                    "standard",    "avg",
	"other_crop_fraction",              "CropFraction-Other",          "Other Crop Fraction",             "grey",        "avg",
	"perennial_crop_fraction",          "CropFraction-Perennial",      "Perennial Crop Fraction",         "grey",        "avg",
	"population",                       "Population",                  "Population",                      "grey",        "avg",
	"population_density",               "Population-Density",          "Population Density",              "grey",        "avg",
	"population-rural",                 "Population-Rural",            "Rural Population",                "grey",        "avg",
	"population-urban",                 "Population-Urban",            "Urban Population",                "grey",        "avg",
	"precipitation"                     "Precipitation",               "Precipitation",                   "grey",        "sum",
	"short_wave_downwelling_radiation", "Radiation-ShortWave"          "Downwelling Shortwave Radiation", "grey",        "avg",
	"rain_fall)",                       "RainFall",                    "Rain Fall",                       "grey",        "sum",
	"rain_pet",                         "RainPET",                     "Potential Evapotranspiration",    "blue",        "avg",
	"reservoir_capacity",               "Reservoir-Capacity",          "Reservoir Capacity",              "grey",        "avg",
	"rice_crop_fraction",               "CropFraction-Rice",           "Rice Crop Fraction",              "grey",        "avg",
	"rice_percolation_rate",            "Crops-RicePercolationRate",   "Rice Percolation Rate",           "grey",        "avg",
	"rooting_depth",                    "Soil-RootingDepth",           "Soil Rooting Depth",              "grey",        "avg",
	"runoff",                           "Runoff",                      "Runoff",                          "blue",        "sum",
	"small_reservoir_coefficient",      "SmallReservoir-Coefficient",  "Small Reservoir Coefficient",     "grey",        "avg",
	"snow_fall",                        "SnowFall",                    "Snow Fall",                       "blue",        "sum",
	"soil_moisture",                    "SoilMoisture",                "Soil Moisture",                   "grey",        "avg",
	"soil_moisture_change",             "SoilMoistureChange",          "Soil Moisture Change",            "grey",        "avg",
	"vapor_pressure",                   "VaporPressure",               "Vapor Pressure",                  "grey",        "avg",
	"vegetables_crop_fraction",         "CropFraction-Vegetables",     "Vegetables Crop Fraction",        "grey",        "avg",
	"wilting_point",                    "Soil-WiltingPoint",           "Soil Wilting Point",              "grey",        "avg",
	"wind_speed",                       "WindSpeed",                   "Wind Speed",                      "grey",        "avg"};
