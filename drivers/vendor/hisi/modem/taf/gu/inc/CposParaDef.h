
#ifndef __CPOSPARADEF_H___
#define __CPOSPARADEF_H___

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "PsTypeDef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define CPOS_POLYGON_COORDINATE_MAX_NUM                 (16)
#define CPOS_GNSS_MEAS_ARRAY_MAX_NUM                    (16)
#define CPOS_SAT_MEAS_PARAMS_GNSS_ARRAY_MAX_NUM         (8)
#define CPOS_GNSS_TOW_ASSIST_ARRAY_MAX_NUM              (16)
#define CPOS_SAT_MAX_NUM                                (16)                    /* 最大卫星个数 */
#define CPOS_GNSS_SAT_MAX_NUM                           (16)                    /* 最大GNSS卫星个数，协议最多64，当前实际最多35 */
//#define CPOS_GNSS_SAT_MAX_NUM                           (35)                    /* 最大GNSS卫星个数，协议最多64，当前实际最多35 */
#define CPOS_GNSS_DATA_BITS_MAX_LEN                     (128)
#define CPOS_GNSS_DATA_BITS_SGN_MAX_NUM                 (8)
#define CPOS_GNSS_ALMANAC_LIST_MAX_LEN                  (35)                    /* 协议最多64，当前实际最多35 */
#define CPOS_GRID_IONLIST_R12_MAX_NUM                   (8)
#define CPOS_OTDOA_NB_CELL_INFO_MAX_NUM                 (24)
#define CPOS_OTDOA_NB_FREQ_INFO_MAX_NUM                 (3)
#define CPOS_EVENT_PAR_MAX_NUM                          (8)
#define CPOS_MEAS_PARAMS_MAX_NUM                        (16)
#define CPOS_NB_MEAS_ELEM_MAX_NUM                       (24)
#define CPOS_MEAS_RESULT_ELEM_MAX_NUM                   (32)
#define CPOS_GNSS_MAX_NUM                               (6)
#define CPOS_MODEL_MAX_NUM                              (5)
#define CPOS_BAND_MAX_NUM                               (64)
#define CPOS_MSG_STATUS_MAX_LEN                         (256)
#define CPOS_MCC_LEN                                    (3)
#define CPOS_MNC_LEN                                    (3)
#define CPOS_INVALID_FF                                 (0xFF)
#define CPOS_INVALID_0F                                 (0x0F)
#define CPOS_REQUESTED_SV_LEN                           (8)
#define CPOS_DOUBLE_REQUESTED_SV_LEN                    (16)
#define CPOS_GNSSTODS_INTLEN                            (5)
#define CPOS_GNSSTODS_DECLEN                            (3)
#define CPOS_MAX_GNSSTODS_LEN                           (9)


/* CPOS子节点的个数的公共定义 */
#define CPOS_XML_NODE_COUNT_0       (0)                 /* 结点个数为0 */
#define CPOS_XML_NODE_COUNT_1       (1)                 /* 结点个数为1 */
#define CPOS_XML_NODE_COUNT_2       (2)                 /* 结点个数为2 */
#define CPOS_XML_NODE_COUNT_3       (3)                 /* 结点个数为3 */

/* CPOS子节点的索引的公共定义 */
#define CPOS_XML_NODE_INDEX_0       (0)                 /* 索引为0 */
#define CPOS_XML_NODE_INDEX_1       (1)                 /* 索引为1 */



/* CPOS根节点定义 */
#define CPOS_LABEL_POS                                  "pos"
#define CPOS_LABEL_POS_END                              "</pos"

/* <location>的节点名宏定义 */
#define CPOS_LABEL_LOCATION                             "location"
#define CPOS_LABEL_LOCATION_PARAMENTERS                 "location_parameters"
#define CPOS_LABEL_TIME_OF_FIX                          "time_of_fix"
#define CPOS_LABEL_SHAPE_DATA                           "shape_data"
#define CPOS_LABEL_TIME                                 "time"
#define CPOS_LABEL_DIRECTION                            "direction"
#define CPOS_LABEL_ELLIPSOID_POINT                      "ellipsoid_point"
#define CPOS_LABEL_ELLIPSOID_POINT_UNCERT_CIRCLE        "ellipsoid_point_uncert_circle"
#define CPOS_LABEL_ELLIPSOID_POINT_UNCERT_ELLIPSE       "ellipsoid_point_uncert_ellipse"
#define CPOS_LABEL_POLYGON                              "polygon"
#define CPOS_LABEL_ELLIPSOID_POINT_ALT                  "ellipsoid_point_alt"
#define CPOS_LABEL_ELLIPSOID_POINT_ALT_UNCERTELLIPSE    "ellipsoid_point_alt_uncertellipse"
#define CPOS_LABEL_ELLIPS_ARC                           "ellips_arc"

#define CPOS_LABEL_VELOCITY_DATA                        "velocity_data"
#define CPOS_LABEL_HOR_VELOCITY                         "hor_velocity"
#define CPOS_LABEL_VERT_VELOCITY                        "vert_velocity"
#define CPOS_LABEL_VERT_VELOCITY_DIRECTION              "vert_velocity_direction"
#define CPOS_LABEL_HOR_UNCERT                           "hor_uncert"
#define CPOS_LABEL_VERT_UNCERT                          "vert_uncert"

#define CPOS_LABEL_COORDINATE                           "coordinate"
#define CPOS_LABEL_LATITUDE                             "latitude"
#define CPOS_LABEL_LONGITUDE                            "longitude"
#define CPOS_LABEL_NORTH                                "north"
#define CPOS_LABEL_DEGREES                              "degrees"
#define CPOS_LABEL_UNCERT_CIRCLE                        "uncert_circle"
#define CPOS_LABEL_UNCERT_ELLIPSE                       "uncert_ellipse"

#define CPOS_LABEL_UNCERT_SEMI_MAJOR                    "uncert_semi_major"
#define CPOS_LABEL_UNCERT_SEMI_MINOR                    "uncert_semi_minor"
#define CPOS_LABEL_ORIENT_MAJOR                         "orient_major"
#define CPOS_LABEL_CONFIDENCE                           "confidence"
#define CPOS_LABEL_CONFIDENCE_R10                       "confidence_r10"

#define CPOS_LABEL_ALTITUDE                             "altitude"
#define CPOS_LABEL_HEIGHT_ABOVE_SURFACE                 "height_above_surface"
#define CPOS_LABEL_HEIGHT                               "height"

#define CPOS_LABEL_UNCERT_ALT                           "uncert_alt"

#define CPOS_LABEL_INNER_RAD                            "inner_rad"
#define CPOS_LABEL_UNCERT_RAD                           "uncert_rad"
#define CPOS_LABEL_OFFSET_ANGLE                         "offset_angle"
#define CPOS_LABEL_INCLUDED_ANGLE                       "included_angle"

#define CPOS_LABEL_WCDMA_TIME                           "WCDMA_time"
#define CPOS_LABEL_CELL_TIMING_CHIPS                    "cell_timing_chips"
#define CPOS_LABEL_FDD_CPICH                            "FDD_CPICH"
#define CPOS_LABEL_TDD_CELL_ID                          "TDD_cell_id"
#define CPOS_LABEL_SFN_UPPER                            "SFN"
#define CPOS_LABEL_LTE_TIME                             "LTE_time"
#define CPOS_LABEL_PHYS_CELL_ID                         "physCellId"
#define CPOS_LABEL_CELL_GLOBAL_ID_EUTRA                 "cellGlobalIdEUTRA"
#define CPOS_LABEL_EARFCN                               "earfcn"
#define CPOS_LABEL_VALUE                                "value"
#define CPOS_LABEL_UNITS                                "units"
#define CPOS_LABEL_TIME_UNC                             "TimeUnc"
#define CPOS_LABEL_TIME_ASSISTANCE                      "time_assistance"

/* <GNSS_provided_location_information> */
#define CPOS_LABEL_GNSS_LOCATION_INFO                   "GNSS_provided_location_information"

/* <assist_data> */
#define CPOS_LABEL_ASSIST_DATA                          "assist_data"
#define CPOS_LABEL_GPS_ASSIST                           "GPS_assist"
#define CPOS_LABEL_STATUS_HEALTH                        "status_health"
#define CPOS_LABEL_BTS_CLOCK_DRIFT                      "BTS_clock_drift"
#define CPOS_LABEL_REF_TIME                             "ref_time"
#define CPOS_LABEL_DGPS_CORRECTIONS                     "DGPS_corrections"
#define CPOS_LABEL_IODE_UPPER_CASE                      "IODE"
#define CPOS_LABEL_UDRE                                 "UDRE"
#define CPOS_LABEL_PRC                                  "PRC"
#define CPOS_LABEL_RRC                                  "RRC"
#define CPOS_LABEL_DELTA_PRC2                           "delta_PRC2"
#define CPOS_LABEL_DELTA_RRC2                           "delta_RRC2"
#define CPOS_LABEL_NAV_MODEL_ELEM                       "nav_model_elem"
#define CPOS_LABEL_SAT_STATUS                           "sat_status"
#define CPOS_LABEL_EPHEM_AND_CLOCK                      "ephem_and_clock"
#define CPOS_LABEL_L2_CODE                              "l2_code"
#define CPOS_LABEL_URA                                  "ura"
#define CPOS_LABEL_SV_HEALTH                            "sv_health"
#define CPOS_LABEL_IODC                                 "iodc"
#define CPOS_LABEL_L2P_FLAG                             "l2p_flag"
#define CPOS_LABEL_ESR1                                 "esr1"
#define CPOS_LABEL_ESR2                                 "esr2"
#define CPOS_LABEL_ESR3                                 "esr3"
#define CPOS_LABEL_ESR4                                 "esr4"
#define CPOS_LABEL_TGD                                  "tgd"
#define CPOS_LABEL_TOC                                  "toc"
#define CPOS_LABEL_AF2                                  "af2"
#define CPOS_LABEL_AF1                                  "af1"
#define CPOS_LABEL_AF0                                  "af0"
#define CPOS_LABEL_CRS                                  "crs"
#define CPOS_LABEL_DELTA_N                              "delta_n"
#define CPOS_LABEL_M0                                   "m0"
#define CPOS_LABEL_CUC                                  "cuc"
#define CPOS_LABEL_ECC                                  "ecc"
#define CPOS_LABEL_CUS                                  "cus"
#define CPOS_LABEL_POWER_HALF                           "power_half"
#define CPOS_LABEL_TOE                                  "toe"
#define CPOS_LABEL_FIT_FLAG                             "fit_flag"
#define CPOS_LABEL_AODA                                 "aoda"
#define CPOS_LABEL_CIC                                  "cic"
#define CPOS_LABEL_OMEGA0                               "omega0"
#define CPOS_LABEL_CIS                                  "cis"
#define CPOS_LABEL_I0                                   "i0"
#define CPOS_LABEL_CRC                                  "crc"
#define CPOS_LABEL_OMEGA                                "omega"
#define CPOS_LABEL_OMEGA_DOT                            "omega_dot"
#define CPOS_LABEL_IDOT                                 "idot"
#define CPOS_LABEL_DERIVED                              "derived"
#define CPOS_LABEL_ALFA0                                "alfa0"
#define CPOS_LABEL_ALFA1                                "alfa1"
#define CPOS_LABEL_ALFA2                                "alfa2"
#define CPOS_LABEL_ALFA3                                "alfa3"
#define CPOS_LABEL_BETA0                                "beta0"
#define CPOS_LABEL_BETA1                                "beta1"
#define CPOS_LABEL_BETA2                                "beta2"
#define CPOS_LABEL_BETA3                                "beta3"
#define CPOS_LABEL_NMS_CLOCK_NAV                        "nms_clock_nav"
#define CPOS_LABEL_NAV_TOC                              "nav_Toc"
#define CPOS_LABEL_NAV_AF2                              "nav_af2"
#define CPOS_LABEL_NAV_AF1                              "nav_af1"
#define CPOS_LABEL_NAV_AF0                              "nav_af0"
#define CPOS_LABEL_NAV_TGD                              "nav_Tgd"
#define CPOS_LABEL_NMS_ORBIT_NAV                        "nms_orbit_nav"
#define CPOS_LABEL_NAV_URA                              "nav_URA"
#define CPOS_LABEL_NAV_FITFLAG                          "nav_FitFlag"
#define CPOS_LABEL_NAV_TOE                              "nav_Toe"
#define CPOS_LABEL_NAV_OMEGA                            "nav_Omega"
#define CPOS_LABEL_NAV_DELTAN                           "nav_DeltaN"
#define CPOS_LABEL_NAV_M0                               "nav_M0"
#define CPOS_LABEL_NAV_OMEGAA_D                         "nav_OmegaA_d"
#define CPOS_LABEL_NAV_E                                "nav_E"
#define CPOS_LABEL_NAV_I_D                              "nav_I_d"
#define CPOS_LABEL_NAV_APOWERHALF                       "nav_APowerHalf"
#define CPOS_LABEL_NAV_I0                               "nav_I0"
#define CPOS_LABEL_NAV_OMEGAA0                          "nav_OmegaA0"
#define CPOS_LABEL_NAV_CRS                              "nav_Crs"
#define CPOS_LABEL_NAV_CIS                              "nav_Cis"
#define CPOS_LABEL_NAV_CUS                              "nav_Cus"
#define CPOS_LABEL_NAV_CRC                              "nav_Crc"
#define CPOS_LABEL_NAV_CIC                              "nav_Cic"
#define CPOS_LABEL_NAV_CUC                              "nav_Cuc"
#define CPOS_LABEL_NAV_CODEONL2                         "nav_CodeOnL2"
#define CPOS_LABEL_NAV_L2PFLAG                          "nav_L2Pflag"
#define CPOS_LABEL_NAV_SF1_1                            "nav_sf1_1"
#define CPOS_LABEL_NAV_SF1_2                            "nav_sf1_2"
#define CPOS_LABEL_NAV_SF1_3                            "nav_sf1_3"
#define CPOS_LABEL_NAV_SF1_4                            "nav_sf1_4"
#define CPOS_LABEL_NAV_AODA                             "nav_AODA"
#define CPOS_LABEL_NMS_CLOCK_CNAV                       "nms_clock_cnav"
#define CPOS_LABEL_CNAV_TOC                             "cnav_Toc"
#define CPOS_LABEL_CNAV_TOP                             "cnav_Top"
#define CPOS_LABEL_CNAV_URA0                            "cnav_URA0"
#define CPOS_LABEL_CNAV_URA1                            "cnav_URA1"
#define CPOS_LABEL_CNAV_URA2                            "cnav_URA2"
#define CPOS_LABEL_CNAV_AF2                             "cnav_Af2"
#define CPOS_LABEL_CNAV_AF1                             "cnav_Af1"
#define CPOS_LABEL_CNAV_AF0                             "cnav_Af0"
#define CPOS_LABEL_CNAV_TGD                             "cnav_Tgd"
#define CPOS_LABEL_CNAV_ISCL1CP                         "cnav_ISCl1cp"
#define CPOS_LABEL_CNAV_ISCL1CD                         "cnav_ISCl1cd"
#define CPOS_LABEL_CNAV_ISCL1CA                         "cnav_ISCl1ca"
#define CPOS_LABEL_CNAV_ISCL2C                          "cnav_ISCl2c"
#define CPOS_LABEL_CNAV_ISCL5I5                         "cnav_ISCl5i5"
#define CPOS_LABEL_CNAV_ISCL5Q5                         "cnav_ISCl5q5"
#define CPOS_LABEL_NMS_ORBIT_CNAV                       "nms_orbit_cnav"
#define CPOS_LABEL_CNAV_URAINDEX                        "cnav_URAindex"
#define CPOS_LABEL_CNAV_DELTAA                          "cnav_DeltaA"
#define CPOS_LABEL_CNAV_ADOT                            "cnav_Adot"
#define CPOS_LABEL_CNAV_DELTANODO                       "cnav_DeltaNo"
#define CPOS_LABEL_CNAV_DELTANODOT                      "cnav_DeltaNoDot"
#define CPOS_LABEL_CNAV_MO                              "cnav_Mo"
#define CPOS_LABEL_CNAV_E                               "cnav_E"
#define CPOS_LABEL_CNAV_OMEGA                           "cnav_Omega"
#define CPOS_LABEL_CNAV_OMEGA0                          "cnav_OMEGA0"
#define CPOS_LABEL_CNAV_DELTAOMEGADOT                   "cnav_DeltaOmegaDot"
#define CPOS_LABEL_CNAV_IO                              "cnav_Io"
#define CPOS_LABEL_CNAV_IODOT                           "cnav_IoDot"
#define CPOS_LABEL_CNAV_CIS                             "cnav_Cis"
#define CPOS_LABEL_CNAV_CIC                             "cnav_Cic"
#define CPOS_LABEL_CNAV_CRS                             "cnav_Crs"
#define CPOS_LABEL_CNAV_CRC                             "cnav_Crc"
#define CPOS_LABEL_CNAV_CUS                             "cnav_Cus"
#define CPOS_LABEL_CNAV_CUC                             "cnav_Cuc"
#define CPOS_LABEL_NMS_CLOCK_GLONASS                    "nms_clock_glonass"
#define CPOS_LABEL_GLO_TAU                              "glo_Tau"
#define CPOS_LABEL_GLO_GAMMA                            "glo_Gamma"
#define CPOS_LABEL_GLO_DELTATAU                         "glo_DeltaTau"
#define CPOS_LABEL_NMS_ORBIT_GLONASS                    "nms_orbit_glonass"
#define CPOS_LABEL_GLO_EN                               "glo_En"
#define CPOS_LABEL_GLO_P1                               "glo_P1"
#define CPOS_LABEL_GLO_P2                               "glo_P2"
#define CPOS_LABEL_GLO_M                                "glo_M"
#define CPOS_LABEL_GLO_X                                "glo_X"
#define CPOS_LABEL_GLO_X_D                              "glo_X_d"
#define CPOS_LABEL_GLO_X_DD                             "glo_X_dd"
#define CPOS_LABEL_GLO_Y                                "glo_Y"
#define CPOS_LABEL_GLO_Y_D                              "glo_Y_d"
#define CPOS_LABEL_GLO_Y_DD                             "glo_Y_dd"
#define CPOS_LABEL_GLO_Z                                "glo_Z"
#define CPOS_LABEL_GLO_Z_D                              "glo_Z_d"
#define CPOS_LABEL_GLO_Z_DD                             "glo_Z_dd"
#define CPOS_LABEL_NMS_CLOCK_SBAS                       "nms_clock_sbas"
#define CPOS_LABEL_SBAS_TO                              "sbas_To"
#define CPOS_LABEL_SBAS_AGFO                            "sbas_Agfo"
#define CPOS_LABEL_SBAS_AGF1                            "sbas_Agf1"
#define CPOS_LABEL_NMS_ORBIT_SBAS                       "nms_orbit_sbas"
#define CPOS_LABEL_SBAS_ACCURACY                        "sbas_accuracy"
#define CPOS_LABEL_SBAS_XG                              "sbas_Xg"
#define CPOS_LABEL_SBAS_YG                              "sbas_Yg"
#define CPOS_LABEL_SBAS_ZG                              "sbas_Zg"
#define CPOS_LABEL_SBAS_XG_D                            "sbas_Xg_d"
#define CPOS_LABEL_SBAS_YG_D                            "sbas_Yg_d"
#define CPOS_LABEL_SBAS_ZG_D                            "sbas_Zg_d"
#define CPOS_LABEL_SBAS_XG_DD                           "sbas_Xg_dd"
#define CPOS_LABEL_SBAS_YG_DD                           "sbas_Yg_dd"
#define CPOS_LABEL_SBAS_ZG_DD                           "sbas_Zg_dd"
#define CPOS_LABEL_NMS_CLOCK_BDS                        "nms_clock_bds"
#define CPOS_LABEL_BDSTOC_R12                           "bdsToc-r12"
#define CPOS_LABEL_BDSA0_R12                            "bdsA0-r12"
#define CPOS_LABEL_BDSA1_R12                            "bdsA1-r12"
#define CPOS_LABEL_BDSA2_R12                            "bdsA2-r12"
#define CPOS_LABEL_BDSTGD1_R12                          "bdsTgd1-r12"
#define CPOS_LABEL_NMS_ORBIT_BDS                        "nms_orbit_bds"
#define CPOS_LABEL_BDSURAI_R12                          "bdsURAI-r12"
#define CPOS_LABEL_BDSTOE_R12                           "bdsToe-r12"
#define CPOS_LABEL_BDSAPOWERHALF_R12                    "bdsAPowerHalf-r12"
#define CPOS_LABEL_BDSE_R12                             "bdsE-r12"
#define CPOS_LABEL_BDSW_R12                             "bdsW-r12"
#define CPOS_LABEL_BDSDELTAN_R12                        "bdsDeltaN-r12"
#define CPOS_LABEL_BDSM0_R12                            "bdsM0-r12"
#define CPOS_LABEL_BDSOMEGA0_R12                        "bdsOmega0-r12"
#define CPOS_LABEL_BDSOMEGADOT_R12                      "bdsOmegaDot-r12"
#define CPOS_LABEL_BDSI0_R12                            "bdsI0-r12"
#define CPOS_LABEL_BDSIDOT_R12                          "bdsIDot-r12"
#define CPOS_LABEL_BDSCUC_R12                           "bdsCuc-r12"
#define CPOS_LABEL_BDSCUS_R12                           "bdsCus-r12"
#define CPOS_LABEL_BDSCRC_R12                           "bdsCrc-r12"
#define CPOS_LABEL_BDSCRS_R12                           "bdsCrs-r12"
#define CPOS_LABEL_BDSCIC_R12                           "bdsCic-r12"
#define CPOS_LABEL_BDSCIS_R12                           "bdsCis-r12"
#define CPOS_LABEL_GNSS_SATELITE                        "GNSS_satelite"
#define CPOS_LABEL_NMS_HEALTH                           "nms_health"
#define CPOS_LABEL_NMS_IOD                              "nms_iod"
#define CPOS_LABEL_GNSS_ALMANACLIST                     "gnss-AlmanacList"
#define CPOS_LABEL_KEPLERIANBDS_ALMANAC_R12             "keplerianBDS-Almanac-r12"
#define CPOS_LABEL_BDSALMTOA_R12                        "bdsAlmToa-r12"
#define CPOS_LABEL_BDSALMSQRTA_R12                      "bdsAlmSqrtA-r12"
#define CPOS_LABEL_BDSALME_R12                          "bdsAlmE-r12"
#define CPOS_LABEL_BDSALMW_R12                          "bdsAlmW-r12"
#define CPOS_LABEL_BDSALMM0_R12                         "bdsAlmM0-r12"
#define CPOS_LABEL_BDSALMOMEGA0_R12                     "bdsAlmOmega0-r12"
#define CPOS_LABEL_BDSALMOMEGADOT_R12                   "bdsAlmOmegaDot-r12"
#define CPOS_LABEL_BDSALMDELTAI_R12                     "bdsAlmDeltaI-r12"
#define CPOS_LABEL_BDSALMA0_R12                         "bdsAlmA0-r12"
#define CPOS_LABEL_BDSALMA1_R12                         "bdsAlmA1-r12"
#define CPOS_LABEL_BDSSVHEALTH_R12                      "bdsSvHealth-r12"
#define CPOS_LABEL_GNSS_ALMANAC                         "GNSS_almanac"
#define CPOS_LABEL_WEEK_NUMBER                          "week_number"
#define CPOS_LABEL_TOA                                  "toa"
#define CPOS_LABEL_IODA                                 "ioda"
#define CPOS_LABEL_COMPLETEALMANACPROVIDED              "completeAlmanacProvided"
#define CPOS_LABEL_GRIDIONLIST_R12                      "gridIonList-r12"
#define CPOS_LABEL_IGP_ID_R12                           "igp-ID-r12"
#define CPOS_LABEL_DT_R12                               "dt-r12"
#define CPOS_LABEL_GIVEI_R12                            "givei-r12"
#define CPOS_LABEL_UTCMODEL5_R12                        "utcModel5-r12"
#define CPOS_LABEL_UTCA0_R12                            "utcA0-r12"
#define CPOS_LABEL_UTCA1_R12                            "utcA1-r12"
#define CPOS_LABEL_UTCDELTATLS_R12                      "utcDeltaTls-r12"
#define CPOS_LABEL_UTCWNLSF_R12                         "utcWNlsf-r12"
#define CPOS_LABEL_UTCDN_R12                            "utcDN-r12"
#define CPOS_LABEL_UTCDELTATLSF_R12                     "utcDeltaTlsf-r12"
#define CPOS_LABEL_GNSS_ID_GLONASS_SAT_ELEMENT          "GNSS_id_glonass_sat_element"
#define CPOS_LABEL_GNSS_SIGNAL_ID                       "GNSS_signal_id"
#define CPOS_LABEL_CHANNEL_NUMBER                       "channel_number"
#define CPOS_LABEL_GNSS_ID_GLONASS                      "GNSS_id_glonass"
#define CPOS_LABEL_GNSS_ASSIST                          "GNSS_assist"
#define CPOS_LABEL_GNSS_REF_TIME                        "GNSS_ref_time"
#define CPOS_LABEL_GNSS_REF_LOCATION                    "GNSS_ref_location"
#define CPOS_LABEL_GNSS_REF_MEASURMENT_ASSIST           "GNSS_ref_measurment_assist"
#define CPOS_LABEL_GNSS_IONOSPHERIC_MODEL               "GNSS_ionospheric_model"
#define CPOS_LABEL_GNSS_EARTH_ORIENT_PARAM              "GNSS_earth_orient_param"
#define CPOS_LABEL_GNSS_ADDITIONAL_ION_MODEL            "GNSS_additional_ion_model"
#define CPOS_LABEL_GNSS_TIME_MODEL                      "GNSS_time_model"
#define CPOS_LABEL_GNSS_NAV_MODEL                       "GNSS_nav_model"
#define CPOS_LABEL_GNSS_INTEGRITY                       "GNSS_integrity"
#define CPOS_LABEL_ACQU_ASSIST                          "acqu_assist"
#define CPOS_LABEL_GNSS_UTC_MODEL                       "GNSS_UTC_model"
#define CPOS_LABEL_BDS_BRIDMODEL_R12                    "BDS-GridModel-r12"
#define CPOS_LABEL_BDS_REFTIME_R12                      "bds-RefTime-r12"
#define CPOS_LABEL_GNSS_AUXILIARY_INFO                  "GNSS_auxiliary_info"
#define CPOS_LABEL_PLMN_IDENTITY                        "plmn_identity"
#define CPOS_LABEL_CELL_IDENTITY                        "cell_identity"
#define CPOS_LABEL_PRS_MUTING_INFO_R9                   "prs_muting_info_r9"
#define CPOS_LABEL_MUTING_INFO                          "muting_info"
#define CPOS_LABEL_PRS_INFO                             "prs_info"
#define CPOS_LABEL_PRS_BANDWIDTH                        "prs_bandwidth"
#define CPOS_LABEL_PRS_CONF_INDEX                       "prs_conf_index"
#define CPOS_LABEL_NUM_DL_FRAMES                        "num_dl_frames"
#define CPOS_LABEL_ANTENNA_PORT_CONF                    "antenna_port_conf"
#define CPOS_LABEL_CP_LENGTH                            "cp_length"
#define CPOS_LABEL_SLOT_NUMBER_OFFSET                   "slot_number_offset"
#define CPOS_LABEL_PRS_SUBFRAME_OFFSET                  "prs_subframe_offset"
#define CPOS_LABEL_EXPECTED_RSTD                        "expected_RSTD"
#define CPOS_LABEL_EXPECTED_RSTD_UNCERTAINTY            "expected_RSTD_uncertainty"
#define CPOS_LABEL_OTDOA_NEIGHBOUR_CELL_INFO_LIST       "OTDOA_neighbour_cell_info_list"
#define CPOS_LABEL_OTDOA_NEIGHBOUR_FREQ_INFO            "OTDOA_neighbour_freq_info"
#define CPOS_LABEL_OTDOA_NEIGHBOUR_CELL_INFO_ELEMENT    "OTDOA_neighbour_cell_info_element"
#define CPOS_LABEL_OTDOA_REF_CELL_INFO                  "OTDOA_ref_cell_info"
#define CPOS_LABEL_OTDOA_ASSIST_DATA                    "OTDOA_assist_data"
#define CPOS_LABEL_MSR_ASSIST_DATA                      "msr_assist_data"
#define CPOS_LABEL_SYSTEM_INFO_ASSIST_DATA              "system_info_assist_data"
#define CPOS_LABEL_MORE_ASSIST_DATA                     "more_assist_data"
#define CPOS_LABEL_EXT_CONTAINER                        "ext_container"
#define CPOS_LABEL_REL98_ASSIST_DATA_EXT                "rel98_assist_data_ext"
#define CPOS_LABEL_REL5_ASSIST_DATA_EXT                 "rel5_assist_data_ext"
#define CPOS_LABEL_REL7_ASSIST_DATA_EXT                 "rel7_assist_data_ext"


/* <!ELEMENT UTC_model (a1,a0,tot,wnt,dtls,wnlsf,dn,dtlsf)>*/
#define CPOS_LABEL_A1                                   "a1"
#define CPOS_LABEL_A0                                   "a0"
#define CPOS_LABEL_TOT                                  "tot"
#define CPOS_LABEL_WNT                                  "wnt"
#define CPOS_LABEL_DTLS                                 "dtls"
#define CPOS_LABEL_WNLSF                                "wnlsf"
#define CPOS_LABEL_DN                                   "dn"
#define CPOS_LABEL_DTLSF                                "dtlsf"

#define CPOS_LABEL_IONOSPHERIC_MODEL                    "ionospheric_model"
#define CPOS_LABEL_UTC_MODEL                            "UTC_model"
#define CPOS_LABEL_UTC_MODEL_LOWER                      "utc_model"
#define CPOS_LABEL_ALMANAC                              "almanac"
#define CPOS_LABEL_WNA                                  "wna"
#define CPOS_LABEL_ALM_ELEM                             "alm_elem"
#define CPOS_LABEL_DATA_ID                              "data_id"
#define CPOS_LABEL_ALM_ECC                              "alm_ecc"
#define CPOS_LABEL_ALM_TOA                              "alm_toa"
#define CPOS_LABEL_ALM_KSII                             "alm_ksii"
#define CPOS_LABEL_ALM_OMEGA_DOT                        "alm_omega_dot"
#define CPOS_LABEL_ALM_SV_HEALTH                        "alm_sv_health"
#define CPOS_LABEL_ALM_POWER_HALF                       "alm_power_half"
#define CPOS_LABEL_ALM_OMEGA0                           "alm_omega0"
#define CPOS_LABEL_ALM_OMEGA                            "alm_omega"
#define CPOS_LABEL_ALM_M0                               "alm_m0"
#define CPOS_LABEL_ALM_AF0                              "alm_af0"
#define CPOS_LABEL_ALM_AF1                              "alm_af1"
#define CPOS_LABEL_SAT_INFO                             "sat_info"
#define CPOS_LABEL_DOPL0                                "dopl0"
#define CPOS_LABEL_DOPL_EXTRA                           "dopl_extra"
#define CPOS_LABEL_DOPPLER_UNCERTAINTY_EXT_R10          "doppler_uncertainty_ext_r10"
#define CPOS_LABEL_DOPL                                 "dopl"
#define CPOS_LABEL_DOPL1                                "dopl1"
#define CPOS_LABEL_DOPL1_UNCERT                         "dopl1_uncert"
#define CPOS_LABEL_CODE_PH                              "code_ph"
#define CPOS_LABEL_CODE_PH_INT                          "code_ph_int"
#define CPOS_LABEL_GPS_BITNO                            "GPS_bitno"
#define CPOS_LABEL_SRCH_W                               "srch_w"
#define CPOS_LABEL_AZ_EL                                "az_el"
#define CPOS_LABEL_AZ                                   "az"
#define CPOS_LABEL_ELEV                                 "elev"
#define CPOS_LABEL_GPS_RT_INTEGRITY                     "GPS_rt_integrity"
#define CPOS_LABEL_BAD_SATELLITE_SET                    "bad_satellite_set"
#define CPOS_LABEL_GPS_TIME                             "GPS_time"
#define CPOS_LABEL_GPS_TOW_MSEC                         "GPS_TOW_msec"
#define CPOS_LABEL_GPS_WEEK                             "GPS_week"
#define CPOS_LABEL_GSM_TIME                             "GSM_time"
#define CPOS_LABEL_FRAME_NUMBER                         "frame_number"
#define CPOS_LABEL_FRAME_NUMBER_MSB                     "frame_number_msb"
#define CPOS_LABEL_TIME_SLOT_NUMBER                     "time_slot_number"
#define CPOS_LABEL_BIT_NUMBER                           "bit_number"
#define CPOS_LABEL_BCCH_CARRIER                         "BCCH_carrier"
#define CPOS_LABEL_BSIC                                 "BSIC"
#define CPOS_LABEL_GPS_TOW_ASSIST                       "GPS_TOW_assist"
#define CPOS_LABEL_TLM_WORD                             "tlm_word"
#define CPOS_LABEL_ANTI_SP                              "anti_sp"
#define CPOS_LABEL_ALERT                                "alert"
#define CPOS_LABEL_TLM_RES                              "tlm_res"

/* <GPS_meas> */
#define CPOS_LABEL_GPS_MEAS                             "GPS_meas"
#define CPOS_LABEL_REF_TIME_ONLY                        "ref_time_only"
#define CPOS_LABEL_TOW_MSEC                             "tow_msec"
#define CPOS_LABEL_MEAS_PARAMS                          "meas_params"
#define CPOS_LABEL_NAV_ADDL_DATA                        "nav_addl_data"
#define CPOS_LABEL_SAT_ID                               "sat_id"
#define CPOS_LABEL_CODE_PHASE_AMBIGUITY                 "code_phase_ambiguity"
#define CPOS_LABEL_CARR2_NOISE                          "carr2_noise"
#define CPOS_LABEL_DOPPLER_MPS                          "doppler_mps"
#define CPOS_LABEL_CODE_PHASE_MS                        "code_phase_ms"
#define CPOS_LABEL_INTEGER_CODE_PHASE                   "integer_code_phase"
#define CPOS_LABEL_WHOLE_CHIPS                          "whole_chips"
#define CPOS_LABEL_FRACT_CHIPS                          "fract_chips"
#define CPOS_LABEL_MULTI_PATH                           "multi_path"
#define CPOS_LABEL_PSR_RMS_ERR                          "psr_rms_err"
#define CPOS_LABEL_SAT_MEAS_PARAMS_GNSS                 "sat_meas_params_gnss"
#define CPOS_LABEL_TOD_MS                               "TOD_ms"
#define CPOS_LABEL_GNSS_MEAS                            "GNSS_meas"
#define CPOS_LABEL_GNSS_MEAS_REF_TIME                   "GNSS_meas_ref_time"
#define CPOS_LABEL_GNSS_TOD_MSEC                        "gnss_TOD_msec"
#define CPOS_LABEL_GNSS_DAY                             "GNSS_day"
#define CPOS_LABEL_GNSS_TOD                             "gnss-TOD"
#define CPOS_LABEL_GNSS_TODFRAC                         "gnss-TODfrac"
#define CPOS_LABEL_GNSS_TOD_FRAC                        "gnss_TOD_frac"
#define CPOS_LABEL_GNSS_TOD_UNC                         "gnss_TOD_unc"
#define CPOS_LABEL_AGNSS_LIST                           "agnss_list"
#define CPOS_LABEL_LOCATION_PARAMETERS                  "location_parameters"
#define CPOS_LABEL_GNSS_TOD_S                           "GNSS_TOD_s"
#define CPOS_LABEL_NOTIFICATION_LEAP_SEC                "notification_leap_sec"
#define CPOS_LABEL_ADR                                  "adr"

#define CPOS_LABEL_GNSS_TIME                            "GNSS_time"

#define CPOS_LABEL_GNSS_MEAS_LOWER                      "gnss_meas"

/* <pos_err>的节点名宏定义 */
#define CPOS_LABEL_POS_ERR                              "pos_err"
#define CPOS_LABEL_ERR_REASON                           "err_reason"
#define CPOS_LABEL_GPS_ASSIST_REQ                       "GPS_assist_req"
#define CPOS_LABEL_GNSS_ASSIST_REQ                      "GNSS_assist_req"
#define CPOS_LABEL_GPS_TOE                              "GPS_toe"
#define CPOS_LABEL_TTOE_LIMIT                           "ttoe_limit"
#define CPOS_LABEL_ADDL_REQ_SAT                         "addl_req_sat"
#define CPOS_LABEL_IODE                                 "iode"
#define CPOS_LABEL_ECID_MEAS_ERROR_CAUSE                "ECID_meas_error_cause"

/* <pos_meas>的节点名宏定义 */
#define CPOS_LABEL_POS_MEAS                             "pos_meas"
#define CPOS_LABEL_MEAS_ABORT                           "meas_abort"
#define CPOS_LABEL_RRLP_MEAS                            "RRLP_meas"
#define CPOS_LABEL_RRLP_POS_INSTRUCT                    "RRLP_pos_instruct"
#define CPOS_LABEL_RRLP_METHOD_TYPE                     "RRLP_method_type"
#define CPOS_LABEL_MS_ASSISTED                          "ms_assisted"
#define CPOS_LABEL_METHOD_ACCURACY                      "method_accuracy"
#define CPOS_LABEL_MS_ASSISTED_NO_ACCURACY              "ms_assisted_no_accuracy"
#define CPOS_LABEL_MS_BASED                             "ms_based"
#define CPOS_LABEL_MS_BASED_PREF                        "ms_based_pref"
#define CPOS_LABEL_MS_ASSISTED_PREF                     "ms_assisted_pref"
#define CPOS_LABEL_RRLP_METHOD                          "RRLP_method"
#define CPOS_LABEL_RESP_TIME_SECONDS                    "resp_time_seconds"
#define CPOS_LABEL_MULT_SETS                            "mult_sets"
#define CPOS_LABEL_RRC_MEAS                             "RRC_meas"
#define CPOS_LABEL_REP_QUANT                            "rep_quant"
#define CPOS_LABEL_RRC_METHOD_TYPE                      "RRC_method_type"
#define CPOS_LABEL_RRC_METHOD                           "RRC_method"
#define CPOS_LABEL_HOR_ACC                              "hor_acc"
#define CPOS_LABEL_VERT_ACC                             "vert_acc"
#define CPOS_LABEL_REP_CRIT                             "rep_crit"
#define CPOS_LABEL_NO_REP                               "no_rep"
#define CPOS_LABEL_EVENT_REP_CRIT                       "event_rep_crit"
#define CPOS_LABEL_EVENT_PAR                            "event_par"
#define CPOS_LABEL_REP_AMOUNT                           "rep_amount"
#define CPOS_LABEL_MEAS_INTERVAL                        "meas_interval"
#define CPOS_LABEL_REP_INTERVAL                         "rep_interval"
#define CPOS_LABEL_EVENT_SPECIFIC_INFO                  "event_specific_info"
#define CPOS_LABEL_TR_POS_CHG                           "tr_pos_chg"
#define CPOS_LABEL_TR_SFN_SFN_CHG                       "tr_SFN_SFN_chg"
#define CPOS_LABEL_TR_SFN_GPS_TOW                       "tr_SFN_GPS_TOW"

/* <pos_meas_req>的节点名宏定义 */
#define CPOS_LABEL_POS_MEAS_REQ                         "pos_meas_req"

#define CPOS_LABEL_PERIOD_REP_CRIT                      "period_rep_crit"

#define CPOS_LABEL_GNSS_ION_FLAGS                       "gnss_ion_flags"

#define CPOS_LABEL_GNSS_ALLOWED_METHODS                 "GNSS_allowed_methods"
#define CPOS_LABEL_GPS_METHOD                           "GPS_method"
#define CPOS_LABEL_GPS_METHOD_LOWER                     "gps_method"
#define CPOS_LABEL_SBAS_METHOD                          "sbas_method"
#define CPOS_LABEL_QZSS_METHOD                          "qzss_method"
#define CPOS_LABEL_GLONASS_METHOD                       "glonass_method"
#define CPOS_LABEL_BDS_METHOD                           "BDS_method"
#define CPOS_LABEL_REQ_INFO                             "req_info"
#define CPOS_LABEL_HACC                                 "hacc"
#define CPOS_LABEL_VACC                                 "vacc"
#define CPOS_LABEL_QOS                                  "qos"
#define CPOS_LABEL_RESPONSE_TIME                        "response_time"
#define CPOS_LABEL_REPORTING_INTERVAL                   "reporting_interval"
#define CPOS_LABEL_AMOUNT                               "amount"
#define CPOS_LABEL_GNSS_ION_AI0                         "gnss_ion_ai0"
#define CPOS_LABEL_GNSS_ION_AI1                         "gnss_ion_ai1"
#define CPOS_LABEL_GNSS_ION_AI2                         "gnss_ion_ai2"

#define CPOS_LABEL_GNSS_EOP_TEOP                        "gnss_eop_teop"
#define CPOS_LABEL_GNSS_EOP_PMX                         "gnss_eop_pmX"
#define CPOS_LABEL_GNSS_EOP_PMX_D                       "gnss_eop_pmX_d"
#define CPOS_LABEL_GNSS_EOP_PMY                         "gnss_eop_pmY"
#define CPOS_LABEL_GNSS_EOP_PMY_D                       "gnss_eop_pmY_d"
#define CPOS_LABEL_GNSS_EOP_DELTAUT1                    "gnss_eop_deltaUT1"
#define CPOS_LABEL_GNSS_EOP_DELTAUT1_D                  "gnss_eop_deltaUT1_d"
#define CPOS_LABEL_GNSS_ADDITIONAL_ION_MODEM            "GNSS_additional_ion_model"
#define CPOS_LABEL_TME_REF_TIME                         "tme_ref_time"
#define CPOS_LABEL_TME_A0                               "tme_A0"
#define CPOS_LABEL_TME_A1                               "tme_A1"
#define CPOS_LABEL_TME_A2                               "tme_A2"
#define CPOS_LABEL_TME_WEEK                             "tme_week"


#define CPOS_LABEL_BAD_SIGNAL                           "bad_signal"
#define CPOS_LABEL_GNSS_SIGNAL                          "GNSS_signal"
#define CPOS_LABEL_GNSS_DATABITS                        "GNSS_databits"
#define CPOS_LABEL_GNSS_DATABITS_SGNLIST                "gnss-DataBitsSgnList"
#define CPOS_LABEL_GNSS_DATABITS_SATLIST                "gnss-DataBitsSatList"
#define CPOS_LABEL_GNSS_DATABITASSISTANCE               "GNSS_databitassistance"

#define CPOS_LABEL_ABORT                                "abort"
#define CPOS_LABEL_REQUEST_LOCATION_INFO                "request_location_info"
#define CPOS_LABEL_OTDOA_REQ_LOC_INFO                   "OTDOA_req_loc_info"
#define CPOS_LABEL_ECID_REQ_LOC_INFO                    "ECID_req_loc_info"
#define CPOS_LABEL_PERIODIC_REPORTING                   "periodic_reporting"
#define CPOS_LABEL_TRIGGERED_REPORTING                  "triggered_reporting"
#define CPOS_LABEL_REP_DURATION                         "rep_duration"

/* <OTDOA_ECID_req> */
#define CPOS_LABEL_OTDOA_ECID_REQ                       "OTDOA_ECID_req"

/* <OTDOA_meas> */
#define CPOS_LABEL_CELL_GLOBAL_ID_REF                   "cell_global_id_ref"
#define CPOS_LABEL_MCC                                  "mcc"
#define CPOS_LABEL_MNC                                  "mnc"
#define CPOS_LABEL_CELL_ID                              "cell_id"
#define CPOS_LABEL_OTDOA_MEAS_QUALITY                   "OTDOA_meas_quality"
#define CPOS_LABEL_ERR_RESOLUTION                       "err_resolution"
#define CPOS_LABEL_ERR_VALUE                            "err_value"
#define CPOS_LABEL_ERR_NUM_SAMPLES                      "err_num_samples"
#define CPOS_LABEL_NEIGHBOUR_MEAS_LIST                  "neighbour_meas_list"
#define CPOS_LABEL_NEIGHBOUR_MEAS_ELEMENT               "neighbour_meas_element"
#define CPOS_LABEL_PHYS_CELL_ID_NEIGHBOUR               "phys_cell_id_neighbour"
#define CPOS_LABEL_CELL_GLOBAL_ID_NEIGHBOUR             "cell_global_id_neighbour"
#define CPOS_LABEL_RSTD                                 "rstd"
#define CPOS_LABEL_RSTD_QUALITY                         "rstd_quality"
#define CPOS_LABEL_OTDOA_MEAS                           "OTDOA_meas"
#define CPOS_LABEL_SYSTEM_FRAME_NUMBER                  "system_frame_number"
#define CPOS_LABEL_PHYS_CELL_ID_REF                     "phys_cell_id_ref"
#define CPOS_LABEL_EARFCN_REF                           "earfcn_ref"
#define CPOS_LABEL_REF_QUALITY                          "ref_quality"

/* <ECID_meas> */
#define CPOS_LABEL_MEASURED_RESULT_ELEMENT              "measured_result_element"
#define CPOS_LABEL_CELL_GLOBAL_ID                       "cell_global_id"
#define CPOS_LABEL_EARFCN_NEIGHBOUR                     "earfcn_neighbour"
#define CPOS_LABEL_SFN_LOWER                            "sfn"
#define CPOS_LABEL_EARFCN_EUTRA                         "earfcn_eutra"
#define CPOS_LABEL_RSRP_RESULT                          "rsrp_result"
#define CPOS_LABEL_RSRQ_RESULT                          "rsrq_result"
#define CPOS_LABEL_UE_RXTX_TIME_DIFF                    "ue_rxtx_time_diff"
#define CPOS_LABEL_PRIMARY_CELL_MEASURED_RESULTS        "primary_cell_measured_results"
#define CPOS_LABEL_MEASURED_RESULT_LIST                 "measured_result_list"
#define CPOS_LABEL_ECID_MEAS                            "ECID_meas"

/* <capability_req> */
#define CPOS_LABEL_CAPABILITY_REQ                       "capability_req"

/* <capabilities> */
#define CPOS_LABEL_CAPABILITIES                         "capabilities"
#define CPOS_LABEL_MSA_SUP                              "MSA_sup"
#define CPOS_LABEL_MSB_SUP                              "MSB_sup"
#define CPOS_LABEL_STANDALONE_SUP                       "Standalone_sup"
#define CPOS_LABEL_GNSS_METHOD                          "GNSS_Method"
#define CPOS_LABEL_ALMANAC_SUP                          "almanac_sup"
#define CPOS_LABEL_UTC_MODEL_SUP                        "UTC_model_sup"
#define CPOS_LABEL_IONOSPHERIC_SUP                      "ionospheric_sup"
#define CPOS_LABEL_NAV_MODEL_SUP                        "nav_model_sup"
#define CPOS_LABEL_DGPS_SUP                             "DGPS_sup"
#define CPOS_LABEL_REF_LOC_SUP                          "ref_loc_sup"
#define CPOS_LABEL_REF_TIME_SUP                         "ref_time_sup"
#define CPOS_LABEL_ACQUISTION_SUP                       "acquisition_sup"
#define CPOS_LABEL_RT_INTEGR_SUP                        "rt_integr_sup"
#define CPOS_LABEL_ORBIT_SUP                            "orbit_sup"
#define CPOS_LABEL_CLOCK_SUP                            "clock_sup"
#define CPOS_LABEL_GNSS_AID_FOR_ONE_GNSS                "GNSS_aid_for_one_gnss"
#define CPOS_LABEL_ACQUISITION_SUP                      "acquisition_sup"
#define CPOS_LABEL_GNSS_AUXILIARY_INFO_SUP              "GNSS_auxiliary_info_sup"
#define CPOS_LABEL_GNSS_DATABITASSISTANCE_SUP           "GNSS_databitassistance_sup"
#define CPOS_LABEL_GNSS_ALMANAC_SUP                     "GNSS_almanac_sup"
#define CPOS_LABEL_ALMANAC_MODEL                        "almanac_model"
#define CPOS_LABEL_GNSS_UTC_MODEL_SUP                   "GNSS_UTC_model_sup"
#define CPOS_LABEL_BDS_GRID_MODEL_SUP                   "BDS_grid_model_sup"
#define CPOS_LABEL_EARTH_ORIENT_SUP                     "earth_orient_sup"
#define CPOS_LABEL_SUPPORTED_BAND_LIST_EUTRA            "supported_band_list_EUTRA"
#define CPOS_LABEL_BAND_EUTRA                           "band_EUTRA"
#define CPOS_LABEL_INTERFREQRSTDMEA_R10                 "interFreqRSTDmeasurement-r10"
#define CPOS_LABEL_OTDOA_CAPABILITY                     "OTDOA_capability"
#define CPOS_LABEL_OTDOA_MODE                           "otdoa_mode"
#define CPOS_LABEL_ECID_CAPABILITY                      "ECID_capability"
#define CPOS_LABEL_ECID_MEASSUPPORTED                   "ECID_measSupported"
#define CPOS_LABEL_GPS_AID                              "GPS_aid"
#define CPOS_LABEL_GNSS_AID                             "GNSS_aid"

/* <GNSS_assist_req> */
#define CPOS_LABEL_GNSS_REF_TIME_REQ                    "GNSS_ref_time_req"
#define CPOS_LABEL_GNSS_REF_LOC_REQ                     "GNSS_ref_loc_req"
#define CPOS_LABEL_GNSS_ION_MODEL_REQ                   "GNSS_ion_model_req"
#define CPOS_LABEL_GNSS_EARTH_ORIENT_REQ                "GNSS_earth_orient_req"
#define CPOS_LABEL_STORED_SAT_LIST_ELMENT               "stored_sat_list_element"
#define CPOS_LABEL_REQUESTED_NAV_LIST_INFO              "requested_nav_list_info"
#define CPOS_LABEL_STORED_NAV_LIST                      "stored_nav_list"
#define CPOS_LABEL_SNL_WEEK_OR_DAY                      "snl_week_or_day"
#define CPOS_LABEL_SNL_TOE                              "snl_toe"
#define CPOS_LABEL_SNL_TOE_LIMIT                        "snl_toe_limit"
#define CPOS_LABEL_STORED_SAT_LIST_ELEMENT              "stored_sat_list_element"
#define CPOS_LABEL_REQUESTED_NAV_LIST                   "requested_nav_list"
#define CPOS_LABEL_GNSS_NAVIGATION_MODEL_REQ            "GNSS_navigation_model_req"
#define CPOS_LABEL_DATABIT_REQSAT_LIST                  "databit_reqsat_list"
#define CPOS_LABEL_DATABIT_INTERVAL                     "databit_interval"
#define CPOS_LABEL_GNSS_TOD_REQ                         "gnss_TOD_req"
#define CPOS_LABEL_GNSS_TOD_FRAQ                        "gnss_TOD_fraq"
#define CPOS_LABEL_GNSS_DATABITASSISTANCE_REQ           "GNSS_databitassistance_req"
#define CPOS_LABEL_GNSS_ASSIST_REQ_PER_GNSS             "GNSS_assist_req_per_gnss"
#define CPOS_LABEL_GNSS_TIME_MODEL_REQ                  "GNSS_time_model_req"
#define CPOS_LABEL_GNSS_INTEGRITY_REQ                   "GNSS_integrity_req"
#define CPOS_LABEL_GNSS_ACQUISITION_REQ                 "GNSS_acquisition_req"
#define CPOS_LABEL_GNSS_AUXILIARY_INFO_REQ              "GNSS_auxiliary_info_req"
#define CPOS_LABEL_GNSS_ALMANAC_REQ                     "GNSS_Almanac_req"
#define CPOS_LABEL_GNSS_UTC_MODEL_REQ                   "GNSS_UTC_model_req"
#define CPOS_LABEL_BDS_GRID_MODEL_REQ_R12               "BDS_grid_model_req_r12"

/* <OTDOA_assist_req> */
#define CPOS_LABEL_OTDOA_ASSIST_REQ                     "OTDOA_assist_req"
#define CPOS_LABEL_PHYS_CELL_ID_LOWER                   "phys_cell_id"


/* <msg> */
#define CPOS_LABEL_MSG                                  "msg"

/* <reset_assist_data> */
#define CPOS_LABEL_RESET_ASSIST_DATA                    "reset_assist_data"

/* <storbe> */
#define CPOS_LABEL_STORBE                               "storbe"


/* 属性名宏定义 */
#define CPOS_ATTRIB_LITERAL                             "literal"
#define CPOS_ATTRIB_ALM_REQ                             "alm_req"
#define CPOS_ATTRIB_UTC_MODEL_REQ                       "UTC_model_req"
#define CPOS_ATTRIB_ION_REQ                             "ion_req"
#define CPOS_ATTRIB_NAV_MODEL_REQ                       "nav_model_req"
#define CPOS_ATTRIB_DGPS_CORR_REQ                       "DGPS_corr_req"
#define CPOS_ATTRIB_REF_LOC_REQ                         "ref_loc_req"
#define CPOS_ATTRIB_REF_TIME_REQ                        "ref_time_req"
#define CPOS_ATTRIB_AQU_ASSIST_REQ                      "aqu_assist_req"
#define CPOS_ATTRIB_RT_INTEGR_REQ                       "rt_integr_req"

#define CPOS_ATTRIB_ION_MODEL_DATA_ID                   "ion_model_data_id"

#define CPOS_ATTRIB_GPS_TIMING_OF_CELL_WANTED           "gps_timing_of_cell_wanted"
#define CPOS_ATTRIB_ADDL_ASSIST_DATA_REQ                "addl_assist_data_req"

#define CPOS_ATTRIB_REPORT_FIRST_FIX                    "report_first_fix"

#define CPOS_ATTRIB_REP_AMOUNT                          "rep_amount"
#define CPOS_ATTRIB_REP_INTERVAL_LONG                   "rep_interval_long"

#define CPOS_ATTRIB_MEASURED_SIGNAL                     "measured_signal"
#define CPOS_ATTRIB_GNSS_TIME_ID                        "gnss_time_id"
#define CPOS_ATTRIB_TIME_ID                             "time_id"
#define CPOS_ATTRIB_GNSS_ID                             "gnss_id"
#define CPOS_ATTRIB_SBAS_ID                             "sbas_id"

#define CPOS_ATTRIB_DOPL0                               "dopl0"

#define CPOS_ATTRIB_VALUE                               "Value"
#define CPOS_ATTRIB_UNITS                               "units"

#define CPOS_ATTRIB_LOC_INFO_TYPE                       "loc_info_type"
#define CPOS_ATTRIB_VELOCITY_REQ                        "velocity_req"
#define CPOS_ATTRIB_TIME_REQ                            "time_req"
#define CPOS_ATTRIB_MULTIPLE_SETS                       "multiple_sets"
#define CPOS_ATTRIB_ADDITIONAL_INFO                     "additional_info"

#define CPOS_ATTRIB_ACCURACY_K                          "accuracy_k"
#define CPOS_ATTRIB_CONFIDENCE                          "confidence"
#define CPOS_ATTRIB_SEC                                 "sec"
#define CPOS_ATTRIB_RI_EXP                              "ri_exp"
#define CPOS_ATTRIB_AMOUNT                              "amount"
#define CPOS_ATTRIB_AID_REQ_ALLOWED                     "aid_req_allowed"

#define CPOS_ATTRIB_STORM_FLAG1                         "storm_flag1"
#define CPOS_ATTRIB_STORM_FLAG2                         "storm_flag2"
#define CPOS_ATTRIB_STORM_FLAG3                         "storm_flag3"
#define CPOS_ATTRIB_STORM_FLAG4                         "storm_flag4"
#define CPOS_ATTRIB_STORM_FLAG5                         "storm_flag5"

#define CPOS_ATTRIB_GNSS_SIGNAL                         "GNSS_signal"
#define CPOS_ATTRIB_SIGNAL_ID                           "signal_id"

#define CPOS_ATTRIB_ABORT_OTDOA_MEAS_REQ                "abort_otdoa_meas_req"
#define CPOS_ATTRIB_ABORT_ECID_MEAS_REQ                 "abort_ecid_meas_req"
#define CPOS_ATTRIB_RS_RP_REQ                           "rs_rp_req"
#define CPOS_ATTRIB_RS_RQ_REQ                           "rs_rq_req"
#define CPOS_ATTRIB_UE_RX_TX_REQ                        "ue_rx_tx_req"
#define CPOS_ATTRIB_CELL_CHANGE                         "cell_change"
#define CPOS_ATTRIB_EMERGENCY_CALL_INDICATOR            "emergency_call_indicator"
#define CPOS_ATTRIB_ASSISTANCE_AVAILABLE                "assistance_available"

#define CPOS_ATTRIB_FTA_SUP                             "fta_sup"
#define CPOS_ATTRIB_VELOCITY_SUP                        "velocity_sup"
#define CPOS_ATTRIB_SUPPORTED_SIGNAL                    "supported_signal"
#define CPOS_ATTRIB_MODEL                               "model"

#define CPOS_ATTRIB_PROTOCOL                            "protocol"
#define CPOS_ATTRIB_TRANSACTION_ID                      "transaction_id"

#define CPOS_ATTRIB_STORED_ID                           "stored_id"
#define CPOS_ATTRIB_STORED_IOD                          "stored_iod"
#define CPOS_ATTRIB_STORED_CLOCK_MODEL                  "stored_clock_model"
#define CPOS_ATTRIB_STORED_ORBIT_MODEL                  "stored_orbit_model"
#define CPOS_ATTRIB_REQUESTED_SV                        "requested_sv"
#define CPOS_ATTRIB_REQUESTED_CLOCK_MODEL               "requested_clock_model"
#define CPOS_ATTRIB_REQUESTED_ORBIT_MODEL               "requested_orbit_model"
#define CPOS_ATTRIB_REQUESTED_ADD_NAV_PARAM             "requested_add_nav_param"
#define CPOS_ATTRIB_SIGNAL                              "signal"
#define CPOS_ATTRIB_CONFIDENCE_SUPPORT                  "confidence_support"
#define CPOS_ATTRIB_DOPPLER_UNCERTAINTY_EXT_SUPPORT     "doppler_uncertainty_ext_support"
#define CPOS_ATTRIB_DELTA_T_REQ                         "delta_T_req"
#define CPOS_ATTRIB_NON_BROADCAST_IND_FLAG              "non_broadcast_ind_flag"

#define CPOS_ATTRIB_PORTS                               "ports"

#define CPOS_ATTRIB_TIME_REQ_PREF                       "time_req_pref"
#define CPOS_ATTRIB_TOW_REQ                             "tow_req"
#define CPOS_ATTRIB_LEAP_SEC_REQ                        "leap_sec_req"

/* 属性值宏定义 */
#define CPOS_ATTRIB_VALUE_NOT_MEASURED                  "not_measured"
#define CPOS_ATTRIB_VALUE_NOT_LOW                       "low"
#define CPOS_ATTRIB_VALUE_NOT_MEDIA                     "medium"
#define CPOS_ATTRIB_VALUE_NOT_HIGH                      "high"


#define CPOS_ATTRIB_VALUE_UNDEFINED_ERROR                                   "undefined_error"
#define CPOS_ATTRIB_VALUE_NOT_ENOUGH_GPS_SATELLITES                         "not_enough_gps_satellites"
#define CPOS_ATTRIB_VALUE_GPS_ASSIST_DATA_MISSING                           "gps_assist_data_missing"
#define CPOS_ATTRIB_VALUE_GNSS_ASSIST_DATA_MISSING                          "gnss_assist_data_missing"
#define CPOS_ATTRIB_VALUE_NOT_ENOUGH_GNSS_SATELLITES                        "not_enough_gnss_satellites"
#define CPOS_ATTRIB_VALUE_OTDOA_UNDEFINED                                   "OTDOA_undefined"
#define CPOS_ATTRIB_VALUE_OTDOA_ASSIST_DATA_MISSING                         "OTDOA_assistance_data_missing"
#define CPOS_ATTRIB_VALUE_OTDOA_UNABLE_TO_MEAS_REF_CELL                     "OTDOA_unable_to_measure_referencecell"
#define CPOS_ATTRIB_VALUE_OTDOA_UNABLE_TO_MEAS_ANY_NB_CELL                  "OTDOA_unable_to_measure_any_neighbourcell"
#define CPOS_ATTRIB_VALUE_OTDOA_ATT_BUT_UNABLE_TO_MEAS_SOME_NB_CELL         "OTDOA_attempted_but_unable_to_measure_some_neighbourcells"
#define CPOS_ATTRIB_VALUE_ECID_UNDEFINED                                    "ECID_undefined"
#define CPOS_ATTRIB_VALUE_ECID_REQUESTED_MEAS_NOT_AVAILABLE                 "ECID_requested_measurement_not_available"
#define CPOS_ATTRIB_VALUE_ECID_NOT_ALL_REQUESTED_MEAS_POSSIBLE              "ECID_not_all_requested_measurements_possible"


#define CPOS_ATTRIB_VALUE_TRUE                          "true"
#define CPOS_ATTRIB_VALUE_FALSE                         "false"

#define CPOS_ATTRIB_VALUE_YES                           "yes"
#define CPOS_ATTRIB_VALUE_NO                            "no"

#define CPOS_ATTRIB_VALUE_GPS                           "gps"
#define CPOS_ATTRIB_VALUE_MULTIPLE                      "multiple"
#define CPOS_ATTRIB_VALUE_ONE                           "one"

#define CPOS_ATTRIB_VALUE_UE_ASSISTED                   "ue_assisted"
#define CPOS_ATTRIB_VALUE_UE_BASED                      "ue_based"
#define CPOS_ATTRIB_VALUE_UE_BASED_PREF                 "ue_based_pref"
#define CPOS_ATTRIB_VALUE_UE_ASSISTED_PREF              "ue_assisted_pref"
#define CPOS_ATTRIB_VALUE_OTDOA                         "otdoa"
#define CPOS_ATTRIB_VALUE_OTDOAORGPS                    "otdoaOrGPS"
#define CPOS_ATTRIB_VALUE_CELLID                        "cellID"

#define CPOS_ATTRIB_VALUE_NS_NN_U                       "NS_NN-U"
#define CPOS_ATTRIB_VALUE_ES_NN_U                       "ES_NN-U"
#define CPOS_ATTRIB_VALUE_NS_NN                         "NS_NN"
#define CPOS_ATTRIB_VALUE_ES_SN                         "ES_SN"
#define CPOS_ATTRIB_VALUE_REVD                          "REVD"

#define CPOS_ATTRIB_VALUE_RA1                           "ra1"
#define CPOS_ATTRIB_VALUE_RA2                           "ra2"
#define CPOS_ATTRIB_VALUE_RA4                           "ra4"
#define CPOS_ATTRIB_VALUE_RA8                           "ra8"
#define CPOS_ATTRIB_VALUE_RA16                          "ra16"
#define CPOS_ATTRIB_VALUE_RA32                          "ra32"
#define CPOS_ATTRIB_VALUE_RA64                          "ra64"
#define CPOS_ATTRIB_VALUE_RA_INFINITY                   "ra-Infinity"

#define CPOS_ATTRIB_VALUE_E5                            "e5"
#define CPOS_ATTRIB_VALUE_E15                           "e15"
#define CPOS_ATTRIB_VALUE_E60                           "e60"
#define CPOS_ATTRIB_VALUE_E300                          "e300"
#define CPOS_ATTRIB_VALUE_E900                          "e900"
#define CPOS_ATTRIB_VALUE_E1800                         "e1800"
#define CPOS_ATTRIB_VALUE_E3600                         "e3600"
#define CPOS_ATTRIB_VALUE_E7200                         "e7200"

#define CPOS_ATTRIB_VALUE_RI1                           "ri1"
#define CPOS_ATTRIB_VALUE_RI2                           "ri2"
#define CPOS_ATTRIB_VALUE_RI4                           "ri4"
#define CPOS_ATTRIB_VALUE_RI8                           "ri8"
#define CPOS_ATTRIB_VALUE_RI16                          "ri16"
#define CPOS_ATTRIB_VALUE_RI32                          "ri32"
#define CPOS_ATTRIB_VALUE_RI64                          "ri64"

#define CPOS_ATTRIB_VALUE_PC10                          "pc10"
#define CPOS_ATTRIB_VALUE_PC20                          "pc20"
#define CPOS_ATTRIB_VALUE_PC30                          "pc30"
#define CPOS_ATTRIB_VALUE_PC40                          "pc40"
#define CPOS_ATTRIB_VALUE_PC50                          "pc50"
#define CPOS_ATTRIB_VALUE_PC100                         "pc100"
#define CPOS_ATTRIB_VALUE_PC200                         "pc200"
#define CPOS_ATTRIB_VALUE_PC300                         "pc300"
#define CPOS_ATTRIB_VALUE_PC500                         "pc500"
#define CPOS_ATTRIB_VALUE_PC1000                        "pc1000"
#define CPOS_ATTRIB_VALUE_PC2000                        "pc2000"
#define CPOS_ATTRIB_VALUE_PC5000                        "pc5000"
#define CPOS_ATTRIB_VALUE_PC10000                       "pc10000"
#define CPOS_ATTRIB_VALUE_PC20000                       "pc20000"
#define CPOS_ATTRIB_VALUE_PC50000                       "pc50000"
#define CPOS_ATTRIB_VALUE_PC100000                      "pc100000"

#define CPOS_ATTRIB_VALUE_C0_25                         "c0-25"
#define CPOS_ATTRIB_VALUE_C0_5                          "c0-5"
#define CPOS_ATTRIB_VALUE_C1                            "c1"
#define CPOS_ATTRIB_VALUE_C2                            "c2"
#define CPOS_ATTRIB_VALUE_C3                            "c3"
#define CPOS_ATTRIB_VALUE_C4                            "c4"
#define CPOS_ATTRIB_VALUE_C5                            "c5"
#define CPOS_ATTRIB_VALUE_C10                           "c10"
#define CPOS_ATTRIB_VALUE_C20                           "c20"
#define CPOS_ATTRIB_VALUE_C50                           "c50"
#define CPOS_ATTRIB_VALUE_C100                          "c100"
#define CPOS_ATTRIB_VALUE_C200                          "c200"
#define CPOS_ATTRIB_VALUE_C500                          "c500"
#define CPOS_ATTRIB_VALUE_C1000                         "c1000"
#define CPOS_ATTRIB_VALUE_C2000                         "c2000"
#define CPOS_ATTRIB_VALUE_C5000                         "c5000"

#define CPOS_ATTRIB_VALUE_D60                           "d60"
#define CPOS_ATTRIB_VALUE_D80                           "d80"
#define CPOS_ATTRIB_VALUE_D100                          "d100"
#define CPOS_ATTRIB_VALUE_D120                          "d120"
#define CPOS_ATTRIB_VALUE_NO_INFO                       "no_information"

#define CPOS_ATTRIB_VALUE_MS1                           "ms1"
#define CPOS_ATTRIB_VALUE_MS2                           "ms2"
#define CPOS_ATTRIB_VALUE_MS3                           "ms3"
#define CPOS_ATTRIB_VALUE_MS5                           "ms5"
#define CPOS_ATTRIB_VALUE_MS10                          "ms10"
#define CPOS_ATTRIB_VALUE_MS20                          "ms20"
#define CPOS_ATTRIB_VALUE_MS50                          "ms50"
#define CPOS_ATTRIB_VALUE_MS100                         "ms100"

#define CPOS_ATTRIB_VALUE_RIL0                          "ril0"
#define CPOS_ATTRIB_VALUE_RIL0_25                       "ril0-25"
#define CPOS_ATTRIB_VALUE_RIL0_5                        "ril0-5"
#define CPOS_ATTRIB_VALUE_RIL1                          "ril1"
#define CPOS_ATTRIB_VALUE_RIL2                          "ril2"
#define CPOS_ATTRIB_VALUE_RIL3                          "ril3"
#define CPOS_ATTRIB_VALUE_RIL4                          "ril4"
#define CPOS_ATTRIB_VALUE_RIL6                          "ril6"
#define CPOS_ATTRIB_VALUE_RIL8                          "ril8"
#define CPOS_ATTRIB_VALUE_RIL12                         "ril12"
#define CPOS_ATTRIB_VALUE_RIL16                         "ril16"
#define CPOS_ATTRIB_VALUE_RIL20                         "ril20"
#define CPOS_ATTRIB_VALUE_RIL24                         "ril24"
#define CPOS_ATTRIB_VALUE_RIL28                         "ril28"
#define CPOS_ATTRIB_VALUE_RIL32                         "ril32"
#define CPOS_ATTRIB_VALUE_RIL64                         "ril64"

#define CPOS_ATTRIB_VALUE_HZ12_5                        "hz12-5"
#define CPOS_ATTRIB_VALUE_HZ25                          "hz25"
#define CPOS_ATTRIB_VALUE_HZ50                          "hz50"
#define CPOS_ATTRIB_VALUE_HZ100                         "hz100"
#define CPOS_ATTRIB_VALUE_HZ200                         "hz200"

#define CPOS_ATTRIB_VALUE_GPS_L1                        "GPS_L1"
#define CPOS_ATTRIB_VALUE_GPS_L1C                       "GPS_L1C"
#define CPOS_ATTRIB_VALUE_GPS_L2C                       "GPS_L2C"
#define CPOS_ATTRIB_VALUE_GPS_L5                        "GPS_L5"
#define CPOS_ATTRIB_VALUE_SBAS_L1                       "SBAS_L1"
#define CPOS_ATTRIB_VALUE_GLO_G1                        "GLO_G1"
#define CPOS_ATTRIB_VALUE_GLO_G2                        "GLO_G2"
#define CPOS_ATTRIB_VALUE_GLO_G3                        "GLO_G3"
#define CPOS_ATTRIB_VALUE_BDS_B1I                       "BDS_B1I"

#define CPOS_ATTRIB_VALUE_S                             "s"
#define CPOS_ATTRIB_VALUE_MS                            "ms"
#define CPOS_ATTRIB_VALUE_US                            "us"
#define CPOS_ATTRIB_VALUE_RRLP_K                        "rrlp_k"
#define CPOS_ATTRIB_VALUE_RRC_K                         "rrc_k"
#define CPOS_ATTRIB_VALUE_LPP_K                         "lpp_k"

#define CPOS_ATTRIB_VALUE_TIME_ID_GPS                   "GPS"
#define CPOS_ATTRIB_VALUE_TIME_ID_GLONASS               "Glonass"
#define CPOS_ATTRIB_VALUE_TIME_ID_QZSS                  "QZSS"
#define CPOS_ATTRIB_VALUE_TIME_ID_BDS                   "BDS"

#define CPOS_ATTRIB_VALUE_ID_GPS                        "GPS"
#define CPOS_ATTRIB_VALUE_ID_SBAS                       "SBAS"
#define CPOS_ATTRIB_VALUE_ID_QZSS                       "QZSS"
#define CPOS_ATTRIB_VALUE_ID_GALILEO                    "Galileo"
#define CPOS_ATTRIB_VALUE_ID_GLONASS                    "Glonass"
#define CPOS_ATTRIB_VALUE_ID_BDS                        "BDS"
#define CPOS_ATTRIB_VALUE_ID_MGPS                       "MGPS"

#define CPOS_ATTRIB_VALUE_ID_WASS                       "WASS"
#define CPOS_ATTRIB_VALUE_ID_EGNOS                      "EGNOS"
#define CPOS_ATTRIB_VALUE_ID_MSAS                       "MSAS"
#define CPOS_ATTRIB_VALUE_ID_GAGAN                      "GAGAN"

#define CPOS_ATTRIB_VALUE_RRC_LOWER                     "rrc"
#define CPOS_ATTRIB_VALUE_HZ                            "Hz"
#define CPOS_ATTRIB_VALUE_MPS                           "mps"

#define CPOS_ATTRIB_VALUE_CODE_PH_UNITS_CHIPS           "chips"
#define CPOS_ATTRIB_VALUE_CODE_PH_UNITS_MS              "ms"

#define CPOS_ATTRIB_VALUE_RRLP_LOWER                    "rrlp"
#define CPOS_ATTRIB_VALUE_LPP_LOWER                     "lpp"
#define CPOS_ATTRIB_VALUE_DEGREE_LOWER                  "degree"

#define CPOS_ATTRIB_VALUE_LOC_REQ                       "loc_req"
#define CPOS_ATTRIB_VALUE_MEAS_REQ                      "meas_req"
#define CPOS_ATTRIB_VALUE_LOC_PREF                      "loc_pref"
#define CPOS_ATTRIB_VALUE_MSB_REQ                       "MSB_req"
#define CPOS_ATTRIB_VALUE_MSA_REQ                       "MSA_req"
#define CPOS_ATTRIB_VALUE_MSB_PREF                      "MSB_pref"
#define CPOS_ATTRIB_VALUE_MSA_PREF                      "MSA_pref"

#define CPOS_ATTRIB_VALUE_ION_MODEL_DATA_ID_OTHER       "00"
#define CPOS_ATTRIB_VALUE_ION_MODEL_DATA_ID_BDS         "01"
#define CPOS_ATTRIB_VALUE_ION_MODEL_DATA_ID_QZSS        "11"

#define CPOS_ATTRIB_VALUE_0                             "0"
#define CPOS_ATTRIB_VALUE_1                             "1"
#define CPOS_ATTRIB_VALUE_2                             "2"
#define CPOS_ATTRIB_VALUE_3                             "3"
#define CPOS_ATTRIB_VALUE_4                             "4"
#define CPOS_ATTRIB_VALUE_5                             "5"
#define CPOS_ATTRIB_VALUE_6                             "6"

#define CPOS_ATTRIB_VALUE_RRLP                          "RRLP"
#define CPOS_ATTRIB_VALUE_RRC                           "RRC"
#define CPOS_ATTRIB_VALUE_LPP                           "LPP"

#define CPOS_ATTRIB_VALUE_PO2_R9                        "po2-r9"
#define CPOS_ATTRIB_VALUE_PO4_R9                        "po4-r9"
#define CPOS_ATTRIB_VALUE_PO8_R9                        "po8-r9"
#define CPOS_ATTRIB_VALUE_PO16_R9                       "po16-r9"

#define CPOS_ATTRIB_VALUE_N6                            "n6"
#define CPOS_ATTRIB_VALUE_N15                           "n15"
#define CPOS_ATTRIB_VALUE_N25                           "n25"
#define CPOS_ATTRIB_VALUE_N50                           "n50"
#define CPOS_ATTRIB_VALUE_N75                           "n75"
#define CPOS_ATTRIB_VALUE_N100                          "n100"

#define CPOS_ATTRIB_VALUE_SF_1                          "sf-1"
#define CPOS_ATTRIB_VALUE_SF_2                          "sf-2"
#define CPOS_ATTRIB_VALUE_SF_4                          "sf-4"
#define CPOS_ATTRIB_VALUE_SF_6                          "sf-6"

#define CPOS_ATTRIB_VALUE_PORTS1_OR_2                   "ports1-or-2"
#define CPOS_ATTRIB_VALUE_PORTS4                        "ports4"

#define CPOS_ATTRIB_VALUE_NORMAL                        "normal"
#define CPOS_ATTRIB_VALUE_EXTENDED                      "extended"



/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum CPOS_BOOL_ENUM
{
    CPOS_FALSE                          = 0,
    CPOS_TRUE                           = 1,
    CPOS_BOOL_BUTT
};
typedef VOS_UINT8 CPOS_BOOL_ENUM_UINT8;


enum CPOS_WHETHER_ENUM
{
    CPOS_NO                             = 0,
    CPOS_YES                            = 1,
    CPOS_WHETHER_BUTT
};
typedef VOS_UINT8 CPOS_WHETHER_ENUM_UINT8;


enum CPOS_POS_PROTOCOL_ENUM
{
    CPOS_POS_PROTOCOL_RRLP              = 0,
    CPOS_POS_PROTOCOL_RRC,
    CPOS_POS_PROTOCOL_LPP,

    CPOS_POS_PROTOCOL_BUTT
};
typedef VOS_UINT8 CPOS_POS_PROTOCOL_ENUM_UINT8;


enum CPOS_POS_ELEMENT_TYPE_ENUM
{
    CPOS_POS_ELEMENT_LOCATION               = 0,                                /* _H2ASN_MsgChoice CPOS_LOCATION_STRU */
    CPOS_POS_ELEMENT_GNSS_LOCATION_INFO,                                        /* _H2ASN_MsgChoice CPOS_GNSS_LOCATION_INFO_STRU */
    CPOS_POS_ELEMENT_ASSIST_DATA,                                               /* _H2ASN_MsgChoice CPOS_ASSIST_DATA_STRU */
    CPOS_POS_ELEMENT_POS_MEAS,                                                  /* _H2ASN_MsgChoice CPOS_POS_MEAS_STRU */
    CPOS_POS_ELEMENT_POS_MEAS_REQ,                                              /* _H2ASN_MsgChoice CPOS_POS_MEAS_REQ_STRU */
    CPOS_POS_ELEMENT_GPS_MEAS,                                                  /* _H2ASN_MsgChoice CPOS_GPS_MEAS_STRU */
    CPOS_POS_ELEMENT_OTDOA_ECID_REQ,                                            /* _H2ASN_MsgChoice CPOS_OTDOA_ECID_REQ_STRU */
    CPOS_POS_ELEMENT_OTDOA_MEAS,                                                /* _H2ASN_MsgChoice CPOS_OTDOA_MEAS_STRU */
    CPOS_POS_ELEMENT_ECID_MEAS,                                                 /* _H2ASN_MsgChoice CPOS_ECID_MEAS_STRU */
    CPOS_POS_ELEMENT_GNSS_MEAS_ARRAY,                                           /* _H2ASN_MsgChoice CPOS_GNSS_MEAS_ARRAY_STRU */
    CPOS_POS_ELEMENT_GPS_ASSIST_REQ,                                            /* _H2ASN_MsgChoice CPOS_GPS_ASSIST_REQ_STRU */
    CPOS_POS_ELEMENT_GNSS_ASSIST_REQ,                                           /* _H2ASN_MsgChoice CPOS_GNSS_ASSIST_REQ_STRU */
    CPOS_POS_ELEMENT_OTDOA_ASSIST_REQ,                                          /* _H2ASN_MsgChoice CPOS_OTDOA_ASSIST_REQ_STRU */
    CPOS_POS_ELEMENT_CAPABILITY_REQ,                                            /* _H2ASN_MsgChoice VOS_UINT32 */
    CPOS_POS_ELEMENT_CAPABILITIES,                                              /* _H2ASN_MsgChoice CPOS_CAPABILITIES_STRU */
    CPOS_POS_ELEMENT_MSG,                                                       /* _H2ASN_MsgChoice CPOS_MSG_STRU */
    CPOS_POS_ELEMENT_POS_ERR,                                                   /* _H2ASN_MsgChoice CPOS_POS_ERR_STRU */
    CPOS_POS_ELEMENT_RESET_ASSIST_DATA,                                         /* _H2ASN_MsgChoice VOS_UINT32 */
    CPOS_POS_ELEMENT_STROBE,                                                    /* _H2ASN_MsgChoice VOS_UINT32 */

    CPOS_POS_ELEMENT_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_POS_ELEMENT_TYPE_ENUM_UINT8;


enum CPOS_GNSS_LOCATION_INFO_ELE_TYPE_ENUM
{
    CPOS_GNSS_LOCATION_INFO_ELE_GNSS_MEAS_ARRAY             = 0,
    CPOS_GNSS_LOCATION_INFO_ELE_MULTI_LOCATION,

    CPOS_GNSS_LOCATION_INFO_ELE_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_GNSS_LOCATION_INFO_ELE_TYPE_ENUM_UINT8;


enum CPOS_LATITUDE_TYPE_ENUM
{
    CPOS_LATITUDE_TYPE_NORTH            = 0,
    CPOS_LATITUDE_TYPE_SOUTH,

    CPOS_LATITUDE_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_LATITUDE_TYPE_ENUM_UINT8;


enum CPOS_ALTITUDE_TYPE_ENUM
{
    CPOS_ALTITUDE_TYPE_ABOVE            = 0,
    CPOS_ALTITUDE_TYPE_BELOW,

    CPOS_ALTITUDE_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_ALTITUDE_TYPE_ENUM_UINT8;


enum CPOS_VERT_VELOCITY_DIR_ENUM
{
    CPOS_VERT_VELOCITY_DIR_UPWARD       = 0,
    CPOS_VERT_VELOCITY_DIR_DOWNWARD,

    CPOS_VERT_VELOCITY_DIR_BUTT
};
typedef VOS_UINT8 CPOS_VERT_VELOCITY_DIR_ENUM_UINT8;


enum CPOS_GNSS_ID_ENUM
{
    CPOS_GNSS_ID_GPS                    = 0,
    CPOS_GNSS_ID_SBAS,
    CPOS_GNSS_ID_QZSS,
    CPOS_GNSS_ID_GALILEO,
    CPOS_GNSS_ID_GLONASS,
    CPOS_GNSS_ID_BDS,
    CPOS_GNSS_ID_MGPS,

    CPOS_GNSS_ID_BUTT
};
typedef VOS_UINT8 CPOS_GNSS_ID_ENUM_UINT8;


enum CPOS_MULTI_PATH_LITERAL_ENUM
{
    CPOS_MULTI_PATH_LITERAL_NOT_MEASURED        = 0,
    CPOS_MULTI_PATH_LITERAL_LOW,
    CPOS_MULTI_PATH_LITERAL_MEDIUM,
    CPOS_MULTI_PATH_LITERAL_HIGH,

    CPOS_MULTI_PATH_LITERAL_BUTT
};
typedef VOS_UINT8 CPOS_MULTI_PATH_LITERAL_ENUM_UINT8;


enum CPOS_GNSS_TIME_ID_ENUM
{
    CPOS_GNSS_TIME_ID_GPS               = 0,
    CPOS_GNSS_TIME_ID_GLONASS,
    CPOS_GNSS_TIME_ID_QZSS,
    CPOS_GNSS_TIME_ID_BDS,

    CPOS_GNSS_TIME_ID_BUTT
};
typedef VOS_UINT8 CPOS_GNSS_TIME_ID_ENUM_UINT8;


enum CPOS_TIME_ID_ENUM
{
    CPOS_TIME_ID_SBAS                   = 0,
    CPOS_TIME_ID_MGPS,
    CPOS_TIME_ID_QZSS,
    CPOS_TIME_ID_GLONASS,
    CPOS_TIME_ID_GPS,
    CPOS_TIME_ID_BDS,

    CPOS_TIME_ID_BUTT
};
typedef VOS_UINT8 CPOS_TIME_ID_ENUM_UINT8;


enum CPOS_SHAPE_DATA_ELE_TYPE_ENUM
{
    CPOS_SHAPE_DATA_ELE_ELLIP_POINT                     = 0,
    CPOS_SHAPE_DATA_ELE_ELLIP_POINT_UNCERT_CIRCLE,
    CPOS_SHAPE_DATA_ELE_ELLIP_POINT_UNCERT_ELLIP,
    CPOS_SHAPE_DATA_ELE_POLYGON,
    CPOS_SHAPE_DATA_ELE_ELLIP_POINT_ALT,
    CPOS_SHAPE_DATA_ELE_ELLIP_POINT_ALT_UNCERT_ELLIP,
    CPOS_SHAPE_DATA_ELE_ELLIP_ARC,

    CPOS_SHAPE_DATA_ELE_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_SHAPE_DATA_ELE_TYPE_ENUM_UINT8;


enum CPOS_GUL_TIME_ELE_TYPE_ENUM
{
    CPOS_GUL_TIME_ELE_GSM_TIME          = 0,
    CPOS_GUL_TIME_ELE_WCDMA_TIME,
    CPOS_GUL_TIME_ELE_LTE_TIME,

    CPOS_GUL_TIME_ELE_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_GUL_TIME_ELE_TYPE_ENUM_UINT8;


enum CPOS_WCDMA_TIME_ELE_TYPE_ENUM
{
    CPOS_WCDMA_TIME_ELE_FDD_CPICH       = 0,
    CPOS_WCDMA_TIME_ELE_TDD_CELL_ID,
    CPOS_WCDMA_TIME_ELE_SFN,

    CPOS_WCDMA_TIME_ELE_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_WCDMA_TIME_ELE_TYPE_ENUM_UINT8;


enum CPOS_TIME_UNC_UNITS_ENUM
{
    CPOS_TIME_UNC_UNITS_S               = 0,
    CPOS_TIME_UNC_UNITS_MS,
    CPOS_TIME_UNC_UNITS_US,
    CPOS_TIME_UNC_UNITS_RRLP_K,
    CPOS_TIME_UNC_UNITS_RRC_K,
    CPOS_TIME_UNC_UNITS_LPP_K,

    CPOS_TIME_UNC_UNITS_BUTT
};
typedef VOS_UINT8 CPOS_TIME_UNC_UNITS_ENUM_UINT8;


enum CPOS_STATUS_HEALTH_ENUM
{
    CPOS_STATUS_HEALTH_UDRE10           = 0,
    CPOS_STATUS_HEALTH_UDRE075,
    CPOS_STATUS_HEALTH_UDRE05,
    CPOS_STATUS_HEALTH_UDRE03,
    CPOS_STATUS_HEALTH_UDRE02,
    CPOS_STATUS_HEALTH_UDRE01,
    CPOS_STATUS_HEALTH_NO_DATA,
    CPOS_STATUS_HEALTH_INVALID_DATA,

    CPOS_STATUS_HEALTH_BUTT
};
typedef VOS_UINT8 CPOS_STATUS_HEALTH_ENUM_UINT8;


enum CPOS_REF_TIME_ELE_TYPE_ENUM
{
    CPOS_REF_TIME_ELE_GPS_TIME          = 0,
    CPOS_REF_TIME_ELE_GNSS_TIME,

    CPOS_REF_TIME_ELE_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_REF_TIME_ELE_TYPE_ENUM_UINT8;


enum CPOS_UDRE_ENUM
{
    CPOS_UDRE_LESS_THAN_1               = 0,
    CPOS_UDRE_BETWEEN_1_AND_4,
    CPOS_UDRE_BETWEEN_4_AND_8,
    CPOS_UDRE_OVER_8,

    CPOS_UDRE_BUTT
};
typedef VOS_UINT8 CPOS_UDRE_ENUM_UINT8;


enum CPOS_POS_SAT_STA_ENUM
{
    CPOS_POS_SAT_STA_NS_NN_U            = 0,                                    /* New satellite, new Navigation Model */
    CPOS_POS_SAT_STA_ES_NN_U,                                                   /* Existing satellite, new Navigation Model */
    CPOS_POS_SAT_STA_NS_NN,                                                     /* New satellite, new Navigation Model */
    CPOS_POS_SAT_STA_ES_SN,                                                     /* Existing satellite, same Navigation Model */
    CPOS_POS_SAT_STA_REVD,                                                      /* Reserved */

    CPOS_POS_SAT_STA_BUTT
};
typedef VOS_UINT8 CPOS_POS_SAT_STA_ENUM_UINT8;


enum CPOS_DOPL0_UNITS_ENUM
{
    CPOS_DOPL0_UNITS_RRLP               = 0,
    CPOS_DOPL0_UNITS_RRC,
    CPOS_DOPL0_UNITS_LPP,
    CPOS_DOPL0_UNITS_HZ,
    CPOS_DOPL0_UNITS_MPS,

    CPOS_DOPL0_UNITS_BUTT
};
typedef VOS_UINT8 CPOS_DOPL0_UNITS_ENUM_UINT8;


enum CPOS_DOPL1_UNITS_ENUM
{
    CPOS_DOPL1_UNITS_RRLP               = 0,
    CPOS_DOPL1_UNITS_RRC,
    CPOS_DOPL1_UNITS_LPP,

    CPOS_DOPL1_UNITS_BUTT
};
typedef VOS_UINT8 CPOS_DOPL1_UNITS_ENUM_UINT8;


enum CPOS_DOPL1_UNCERT_ENUM
{
    CPOS_DOPL1_UNCERT_HZ12_5            = 0,
    CPOS_DOPL1_UNCERT_HZ25,
    CPOS_DOPL1_UNCERT_HZ50,
    CPOS_DOPL1_UNCERT_HZ100,
    CPOS_DOPL1_UNCERT_HZ200,

    CPOS_DOPL1_UNCERT_BUTT
};
typedef VOS_UINT8 CPOS_DOPL1_UNCERT_ENUM_UINT8;


enum CPOS_CODE_PH_UNITS_ENUM
{
    CPOS_CODE_PH_UNITS_CHIPS            = 0,
    CPOS_CODE_PH_UNITS_MS,

    CPOS_CODE_PH_UNITS_BUTT
};
typedef VOS_UINT8 CPOS_CODE_PH_UNITS_ENUM_UINT8;


enum CPOS_AZ_EL_UNITS_ENUM
{
    CPOS_AZ_EL_UNITS_RRLP               = 0,
    CPOS_AZ_EL_UNITS_LPP,
    CPOS_AZ_EL_UNITS_DEGREE,

    CPOS_AZ_EL_UNITS_BUTT
};
typedef VOS_UINT8 CPOS_AZ_EL_UNITS_ENUM_UINT8;


enum CPOS_DOPL_UNCERT_EXT_R10_ENUM
{
    CPOS_DOPL_UNCERT_EXT_R10_D60        = 0,
    CPOS_DOPL_UNCERT_EXT_R10_D80,
    CPOS_DOPL_UNCERT_EXT_R10_D100,
    CPOS_DOPL_UNCERT_EXT_R10_D120,
    CPOS_DOPL_UNCERT_EXT_R10_NO_INFO,

    CPOS_DOPL_UNCERT_EXT_R10_BUTT
};
typedef VOS_UINT8 CPOS_DOPL_UNCERT_EXT_R10_ENUM_UINT8;


enum CPOS_SBAS_ID_ENUM
{
    CPOS_SBAS_ID_WASS                   = 0,
    CPOS_SBAS_ID_EGNOS,
    CPOS_SBAS_ID_MSAS,
    CPOS_SBAS_ID_GAGAN,

    CPOS_SBAS_ID_BUTT
};
typedef VOS_UINT8 CPOS_SBAS_ID_ENUM_UINT8;


enum CPOS_ION_MODEL_DATA_ID_ENUM
{
    CPOS_ION_MODEL_DATA_ID_OTHER        = 0,
    CPOS_ION_MODEL_DATA_ID_BDS,
    CPOS_ION_MODEL_DATA_ID_QZSS,

    CPOS_ION_MODEL_DATA_ID_BUTT
};
typedef VOS_UINT8 CPOS_ION_MODEL_DATA_ID_ENUM_UINT8;


enum CPOS_GNSS_SATELITE_ELEM_ENUM
{
    CPOS_GNSS_SATELITE_ELEM_NAV         = 0,
    CPOS_GNSS_SATELITE_ELEM_CNAV,
    CPOS_GNSS_SATELITE_ELEM_GLNASS,
    CPOS_GNSS_SATELITE_ELEM_SBAS,
    CPOS_GNSS_SATELITE_ELEM_BDS,

    CPOS_GNSS_SATELITE_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_GNSS_SATELITE_ELEM_ENUM_UINT8;


enum CPOS_SIGNAL_ID_ENUM
{
    CPOS_SIGNAL_ID_GPS_L1               = 0,
    CPOS_SIGNAL_ID_GPS_L1C,
    CPOS_SIGNAL_ID_GPS_L2C,
    CPOS_SIGNAL_ID_GPS_L5,
    CPOS_SIGNAL_ID_SBAS_L1,
    CPOS_SIGNAL_ID_GLO_G1,
    CPOS_SIGNAL_ID_GLO_G2,
    CPOS_SIGNAL_ID_GLO_G3,
    CPOS_SIGNAL_ID_BDS_B1I,

    CPOS_SIGNAL_ID_BUTT
};
typedef VOS_UINT8 CPOS_SIGNAL_ID_ENUM_UINT8;


enum CPOS_ANTENNA_PORT_ENUM
{
    CPOS_ANTENNA_PORT_1_OR_2            = 0,
    CPOS_ANTENNA_PORT_4,

    CPOS_ANTENNA_PORT_BUTT
};
typedef VOS_UINT8 CPOS_ANTENNA_PORT_ENUM_UINT8;


enum CPOS_CP_LENGTH_ENUM
{
    CPOS_CP_LENGTH_NORMAL               = 0,
    CPOS_CP_LENGTH_EXTENDED,

    CPOS_CP_LENGTH_BUTT
};
typedef VOS_UINT8 CPOS_CP_LENGTH_ENUM_UINT8;


enum CPOS_PRS_MUTING_INFO_R9_ENUM
{
    CPOS_PRS_MUTING_INFO_R9_PO2         = 0,
    CPOS_PRS_MUTING_INFO_R9_PO4,
    CPOS_PRS_MUTING_INFO_R9_PO8,
    CPOS_PRS_MUTING_INFO_R9_PO16,

    CPOS_PRS_MUTING_INFO_R9_BUTT
};
typedef VOS_UINT8 CPOS_PRS_MUTING_INFO_R9_ENUM_UINT8;


enum CPOS_PRS_BANDWIDTH_ENUM
{
    CPOS_PRS_BANDWIDTH_N6               = 0,
    CPOS_PRS_BANDWIDTH_N15,
    CPOS_PRS_BANDWIDTH_N25,
    CPOS_PRS_BANDWIDTH_N50,
    CPOS_PRS_BANDWIDTH_N75,
    CPOS_PRS_BANDWIDTH_N100,

    CPOS_PRS_BANDWIDTH_BUTT
};
typedef VOS_UINT8 CPOS_PRS_BANDWIDTH_ENUM_UINT8;


enum CPOS_NUM_DL_FRAMES_ENUM
{
    CPOS_NUM_DL_FRAMES_SF1              = 0,
    CPOS_NUM_DL_FRAMES_SF2,
    CPOS_NUM_DL_FRAMES_SF4,
    CPOS_NUM_DL_FRAMES_SF6,

    CPOS_NUM_DL_FRAMES_BUTT
};
typedef VOS_UINT8 CPOS_NUM_DL_FRAMES_ENUM_UINT8;


enum CPOS_POS_MEAS_ELEM_ENUM
{
    CPOS_POS_MEAS_ABORT                 = 0,
    CPOS_POS_MEAS_RRLP,
    CPOS_POS_MEAS_RRC,

    CPOS_POS_MEAS_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_POS_MEAS_ELEM_ENUM_UINT8;


enum CPOS_RRLP_METHOD_TYPE_ELEM_ENUM
{
    CPOS_RRLP_METHOD_TYPE_ELEM_MS_ASSISTED          = 0,
    CPOS_RRLP_METHOD_TYPE_ELEM_MS_ASSISTED_NO_ACC,
    CPOS_RRLP_METHOD_TYPE_ELEM_MS_BASED,
    CPOS_RRLP_METHOD_TYPE_ELEM_MS_BASED_PREF,
    CPOS_RRLP_METHOD_TYPE_ELEM_MS_ASSISTED_PREF,

    CPOS_RRLP_METHOD_TYPE_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_RRLP_METHOD_TYPE_ELEM_ENUM_UINT8;


enum CPOS_RRLP_METHOD_ENUM
{
    CPOS_RRLP_METHOD_GPS                = 0,

    CPOS_RRLP_METHOD_BUTT
};
typedef VOS_UINT8 CPOS_RRLP_METHOD_ENUM_UINT8;


enum CPOS_MULT_SETS_ENUM
{
    CPOS_MULT_SETS_MULTIPLE             = 0,
    CPOS_MULT_SETS_ONE,

    CPOS_MULT_SETS_BUTT
};
typedef VOS_UINT8 CPOS_MULT_SETS_ENUM_UINT8;


enum CPOS_RRC_METHOD_TYPE_ENUM
{
    CPOS_RRC_METHOD_TYPE_UE_ASSISTED            = 0,
    CPOS_RRC_METHOD_TYPE_UE_BASED,
    CPOS_RRC_METHOD_TYPE_UE_BASED_PREF,
    CPOS_RRC_METHOD_TYPE_UE_ASSISTED_PREF,

    CPOS_RRC_METHOD_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_RRC_METHOD_TYPE_ENUM_UINT8;


enum CPOS_RRC_METHOD_ENUM
{
    CPOS_RRC_METHOD_OTDOA               = 0,
    CPOS_RRC_METHOD_GPS,
    CPOS_RRC_METHOD_OTDOA_OR_GPS,
    CPOS_RRC_METHOD_CELL_ID,

    CPOS_RRC_METHOD_BUTT
};
typedef VOS_UINT8 CPOS_RRC_METHOD_ENUM_UINT8;


enum CPOS_REP_CRIT_ELEM_ENUM
{
    CPOS_REP_CRIT_ELEM_NO_REP               = 0,
    CPOS_REP_CRIT_ELEM_EVENT_REP_CRIT,
    CPOS_REP_CRIT_ELEM_PERIOD_REP_CRIT,

    CPOS_REP_CRIT_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_REP_CRIT_ELEM_ENUM_UINT8;


enum CPOS_REP_AMOUNT_ENUM
{
    CPOS_REP_AMOUNT_RA1                 = 0,
    CPOS_REP_AMOUNT_RA2,
    CPOS_REP_AMOUNT_RA4,
    CPOS_REP_AMOUNT_RA8,
    CPOS_REP_AMOUNT_RA16,
    CPOS_REP_AMOUNT_RA32,
    CPOS_REP_AMOUNT_RA64,
    CPOS_REP_AMOUNT_RA_INFINITY,

    CPOS_REP_AMOUNT_BUTT
};
typedef VOS_UINT8 CPOS_REP_AMOUNT_ENUM_UINT8;


enum CPOS_MEAS_INTERVAL_ENUM
{
    CPOS_MEAS_INTERVAL_E5                 = 0,
    CPOS_MEAS_INTERVAL_E15,
    CPOS_MEAS_INTERVAL_E60,
    CPOS_MEAS_INTERVAL_E300,
    CPOS_MEAS_INTERVAL_E900,
    CPOS_MEAS_INTERVAL_E1800,
    CPOS_MEAS_INTERVAL_E3600,
    CPOS_MEAS_INTERVAL_E7200,

    CPOS_MEAS_INTERVAL_BUTT
};
typedef VOS_UINT8 CPOS_MEAS_INTERVAL_ENUM_UINT8;


enum CPOS_EVENT_SPEC_INFO_ELEM_ENUM
{
    CPOS_EVENT_SPEC_INFO_ELEM_TR_POS_CHG        = 0,
    CPOS_EVENT_SPEC_INFO_ELEM_TR_SFN_SFN_CHG,
    CPOS_EVENT_SPEC_INFO_ELEM_TR_SFN_GPS_TOW,

    CPOS_EVENT_SPEC_INFO_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_EVENT_SPEC_INFO_ELEM_ENUM_UINT8;


enum CPOS_TR_POS_CHG_ENUM
{
    CPOS_TR_POS_CHG_PC10                 = 0,
    CPOS_TR_POS_CHG_PC20,
    CPOS_TR_POS_CHG_PC30,
    CPOS_TR_POS_CHG_PC40,
    CPOS_TR_POS_CHG_PC50,
    CPOS_TR_POS_CHG_PC100,
    CPOS_TR_POS_CHG_PC200,
    CPOS_TR_POS_CHG_PC300,
    CPOS_TR_POS_CHG_PC500,
    CPOS_TR_POS_CHG_PC1000,
    CPOS_TR_POS_CHG_PC2000,
    CPOS_TR_POS_CHG_PC5000,
    CPOS_TR_POS_CHG_PC10000,
    CPOS_TR_POS_CHG_PC20000,
    CPOS_TR_POS_CHG_PC50000,
    CPOS_TR_POS_CHG_PC100000,

    CPOS_TR_POS_CHG_BUTT
};
typedef VOS_UINT8 CPOS_TR_POS_CHG_ENUM_UINT8;


enum CPOS_TR_SFN_SFN_CHG_ENUM
{
    CPOS_TR_SFN_SFN_CHG_C0_25           = 0,
    CPOS_TR_SFN_SFN_CHG_C0_5,
    CPOS_TR_SFN_SFN_CHG_C1,
    CPOS_TR_SFN_SFN_CHG_C2,
    CPOS_TR_SFN_SFN_CHG_C3,
    CPOS_TR_SFN_SFN_CHG_C4,
    CPOS_TR_SFN_SFN_CHG_C5,
    CPOS_TR_SFN_SFN_CHG_C10,
    CPOS_TR_SFN_SFN_CHG_C20,
    CPOS_TR_SFN_SFN_CHG_C50,
    CPOS_TR_SFN_SFN_CHG_C100,
    CPOS_TR_SFN_SFN_CHG_C200,
    CPOS_TR_SFN_SFN_CHG_C500,
    CPOS_TR_SFN_SFN_CHG_C1000,
    CPOS_TR_SFN_SFN_CHG_C2000,
    CPOS_TR_SFN_SFN_CHG_C5000,

    CPOS_TR_SFN_SFN_CHG_BUTT
};
typedef VOS_UINT8 CPOS_TR_SFN_SFN_CHG_ENUM_UINT8;


enum CPOS_TR_SFN_GPS_TOW_ENUM
{
    CPOS_TR_SFN_GPS_TOW_MS1             = 0,
    CPOS_TR_SFN_GPS_TOW_MS2,
    CPOS_TR_SFN_GPS_TOW_MS3,
    CPOS_TR_SFN_GPS_TOW_MS5,
    CPOS_TR_SFN_GPS_TOW_MS10,
    CPOS_TR_SFN_GPS_TOW_MS20,
    CPOS_TR_SFN_GPS_TOW_MS50,
    CPOS_TR_SFN_GPS_TOW_MS100,

    CPOS_TR_SFN_GPS_TOW_BUTT
};
typedef VOS_UINT8 CPOS_TR_SFN_GPS_TOW_ENUM_UINT8;


enum CPOS_REP_INTERVAL_LONG_ENUM
{
    CPOS_REP_INTERVAL_LONG_RIL0         = 0,
    CPOS_REP_INTERVAL_LONG_RIL0_25,
    CPOS_REP_INTERVAL_LONG_RIL0_5,
    CPOS_REP_INTERVAL_LONG_RIL1,
    CPOS_REP_INTERVAL_LONG_RIL2,
    CPOS_REP_INTERVAL_LONG_RIL3,
    CPOS_REP_INTERVAL_LONG_RIL4,
    CPOS_REP_INTERVAL_LONG_RIL6,
    CPOS_REP_INTERVAL_LONG_RIL8,
    CPOS_REP_INTERVAL_LONG_RIL12,
    CPOS_REP_INTERVAL_LONG_RIL16,
    CPOS_REP_INTERVAL_LONG_RIL20,
    CPOS_REP_INTERVAL_LONG_RIL24,
    CPOS_REP_INTERVAL_LONG_RIL28,
    CPOS_REP_INTERVAL_LONG_RIL32,
    CPOS_REP_INTERVAL_LONG_RIL64,

    CPOS_REP_INTERVAL_LONG_BUTT
};
typedef VOS_UINT8 CPOS_REP_INTERVAL_LONG_ENUM_UINT8;


enum CPOS_POS_MEAS_REQ_ELEM_ENUM
{
    CPOS_POS_MEAS_REQ_ELEM_ABORT            = 0,
    CPOS_POS_MEAS_REQ_ELEM_REQ_INFO_QOS,

    CPOS_POS_MEAS_REQ_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_POS_MEAS_REQ_ELEM_ENUM_UINT8;


enum CPOS_LOC_INFO_TYPE_ENUM
{
    CPOS_LOC_INFO_TYPE_LOC_REQ          = 0,
    CPOS_LOC_INFO_TYPE_MEAS_REQ,
    CPOS_LOC_INFO_TYPE_LOC_PREF,
    CPOS_LOC_INFO_TYPE_MSB_REQ,
    CPOS_LOC_INFO_TYPE_MSA_REQ,
    CPOS_LOC_INFO_TYPE_MSB_PREF,
    CPOS_LOC_INFO_TYPE_MSA_PREF,

    CPOS_LOC_INFO_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_LOC_INFO_TYPE_ENUM_UINT8;


enum CPOS_RI_EXP_ENUM
{
    CPOS_RI_EXP_RIL1                    = 0,
    CPOS_RI_EXP_RIL2,
    CPOS_RI_EXP_RIL3,
    CPOS_RI_EXP_RIL4,
    CPOS_RI_EXP_RIL6,
    CPOS_RI_EXP_RIL8,
    CPOS_RI_EXP_RIL12,
    CPOS_RI_EXP_RIL16,
    CPOS_RI_EXP_RIL20,
    CPOS_RI_EXP_RIL24,
    CPOS_RI_EXP_RIL28,
    CPOS_RI_EXP_RIL32,
    CPOS_RI_EXP_RIL64,

    CPOS_RI_EXP_BUTT
};
typedef VOS_UINT8 CPOS_RI_EXP_ENUM_UINT8;


enum CPOS_AMOUNT_ENUM
{
    CPOS_AMOUNT_RA1                     = 0,
    CPOS_AMOUNT_RA2,
    CPOS_AMOUNT_RA4,
    CPOS_AMOUNT_RA8,
    CPOS_AMOUNT_RA16,
    CPOS_AMOUNT_RA32,
    CPOS_AMOUNT_RA64,
    CPOS_AMOUNT_RA_INFINITY,

    CPOS_AMOUNT_BUTT
};
typedef VOS_UINT8 CPOS_AMOUNT_ENUM_UINT8;


enum CPOS_MULTI_PATH_ENUM
{
    CPOS_MULTI_PATH_NOT_MEASURED        = 0,
    CPOS_MULTI_PATH_LOW,
    CPOS_MULTI_PATH_MEDIUM,
    CPOS_MULTI_PATH_HIGH,

    CPOS_MULTI_PATH_BUTT
};
typedef VOS_UINT8 CPOS_MULTI_PATH_ENUM_UINT8;


enum CPOS_OTDOA_ECID_REQ_ELEM_ENUM
{
    CPOS_OTDOA_ECID_REQ_ELEM_ABORT              = 0,
    CPOS_OTDOA_ECID_REQ_ELEM_REQ_LOCATION_INFO,

    CPOS_OTDOA_ECID_REQ_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_OTDOA_ECID_REQ_ELEM_ENUM_UINT8;


enum CPOS_REQ_LOC_INFO_TYPE_ELEM_ENUM
{
    CPOS_REQ_LOC_INFO_TYPE_ELEM_RSP_TIME        = 0,
    CPOS_REQ_LOC_INFO_TYPE_ELEM_PERIODIC_RPT,
    CPOS_REQ_LOC_INFO_TYPE_ELEM_TRIG_RPT,

    CPOS_REQ_LOC_INFO_TYPE_ELEM_BUTT
};
typedef VOS_UINT8 CPOS_REQ_LOC_INFO_TYPE_ELEM_ENUM_UINT8;


enum CPOS_REP_INTERVAL_ENUM
{
    CPOS_REP_INTERVAL_RIL1              = 0,
    CPOS_REP_INTERVAL_RIL2,
    CPOS_REP_INTERVAL_RIL4,
    CPOS_REP_INTERVAL_RIL8,
    CPOS_REP_INTERVAL_RIL16,
    CPOS_REP_INTERVAL_RIL32,
    CPOS_REP_INTERVAL_RIL64,

    CPOS_REP_INTERVAL_BUTT
};
typedef VOS_UINT8 CPOS_REP_INTERVAL_ENUM_UINT8;


enum CPOS_CLOCK_MODEL_ENUM
{
    CPOS_CLOCK_MODEL_2                  = 2,                                    /* GPS NAV */
    CPOS_CLOCK_MODEL_3,                                                         /* GPS CNAV */
    CPOS_CLOCK_MODEL_4,                                                         /* GLONASS */
    CPOS_CLOCK_MODEL_5,                                                         /* SBAS */
    CPOS_CLOCK_MODEL_6,                                                         /* BDS */

    CPOS_CLOCK_MODEL_BUTT
};
typedef VOS_UINT8 CPOS_CLOCK_MODEL_ENUM_UINT8;


enum CPOS_ORBIT_MODEL_ENUM
{
    CPOS_ORBIT_MODEL_2                  = 2,                                    /* GPS NAV */
    CPOS_ORBIT_MODEL_3,                                                         /* GPS CNAV */
    CPOS_ORBIT_MODEL_4,                                                         /* GLONASS */
    CPOS_ORBIT_MODEL_5,                                                         /* SBAS */
    CPOS_ORBIT_MODEL_6,                                                         /* BDS */

    CPOS_ORBIT_MODEL_BUTT
};
typedef VOS_UINT8 CPOS_ORBIT_MODEL_ENUM_UINT8;


enum CPOS_ERR_REASON_ENUM
{
    CPOS_ERR_REASON_UNDEFINED_ERR                               = 0,            /* undefined_error */
    CPOS_ERR_REASON_NOT_ENOUGH_GPS_SATELLITES,                                  /* not_enough_gps_satellites */
    CPOS_ERR_REASON_GPS_ASSIST_DATA_MISSING,                                    /* gps_assist_data_missing */
    CPOS_ERR_REASON_GNSS_ASSIST_DATA_MISSING,                                   /* gnss_assist_data_missing */
    CPOS_ERR_REASON_NOT_ENOUGH_GNSS_SATELLITES,                                 /* not_enough_gnss_satellites */
    CPOS_ERR_REASON_OTDOA_UNDEFINED,                                            /* OTDOA_undefined */
    CPOS_ERR_REASON_OTDOA_ASSIST_DATA_MISSING,                                  /* OTDOA_assistance_data_missing */
    CPOS_ERR_REASON_OTDOA_UNABLE_TO_MEAS_REF_CELL,                              /* OTDOA_unable_to_measure_referencecell */
    CPOS_ERR_REASON_OTDOA_UNABLE_TO_MEAS_ANY_NB_CELL,                           /* OTDOA_unable_to_measure_any_neighbourcell */
    CPOS_ERR_REASON_OTDOA_ATT_BUT_UNABLE_TO_MEAS_SOME_NB_CELL,                  /* OTDOA_attempted_but_unable_to_measure_some_neighbourcells */
    CPOS_ERR_REASON_ECID_UNDEFINED,                                             /* ECID_undefined */
    CPOS_ERR_REASON_ECID_REQUESTED_MEAS_NOT_AVAILABLE,                          /* ECID_requested_measurement_not_available */
    CPOS_ERR_REASON_ECID_NOT_ALL_REQUESTED_MEAS_POSSIBLE,                       /* ECID_not_all_requested_measurements_possible */

    CPOS_ERR_REASON_BUTT
};
typedef VOS_UINT32 CPOS_ERR_REASON_ENUM_UINT32;


enum CPOS_ASSIST_DATA_TYPE_ENUM
{
    CPOS_ASSIST_DATA_GPS               = 0,
    CPOS_ASSIST_DATA_GNSS,
    CPOS_ASSIST_DATA_OTDOA,

    CPOS_ASSIST_DATA_TYPE_BUTT
};
typedef VOS_UINT8 CPOS_ASSIST_DATA_TYPE_ENUM_UINT8;


enum CPOS_TIME_REQ_PREF_ENUM
{
    CPOS_TIME_REQ_PREF_SBAS               = 0,
    CPOS_TIME_REQ_PREF_MGPS,
    CPOS_TIME_REQ_PREF_QZSS,
    CPOS_TIME_REQ_PREF_GLONASS,
    CPOS_TIME_REQ_PREF_GPS,

    CPOS_TIME_REQ_PREF_BUTT
};
typedef VOS_UINT8 CPOS_TIME_REQ_PREF_ENUM_UINT8;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/

/*****************************************************************************
  6 消息定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


typedef struct
{
    /*
    <!ELEMENT latitude (north,degrees)>
        <!ELEMENT north (#PCDATA)>
        <!ELEMENT degrees (#PCDATA)>  <!-- 0..8388607 -->
    */
    CPOS_LATITUDE_TYPE_ENUM_UINT8       enNorth;                                /* ELEMENT north */
    VOS_UINT8                           aucReserved[3];
    VOS_UINT32                          ulDegrees;                              /* ELEMENT degrees */
} CPOS_LATITUDE_STRU;


typedef struct
{
    /*
    <!ELEMENT coordinate (latitude,longitude)>
        <!ELEMENT longitude (#PCDATA)> <!-- -8388608..8388607 -->
    */
    CPOS_LATITUDE_STRU                  stLatitude;                             /* ELEMENT latitude */
    VOS_INT32                           lLongitude;                             /* ELEMENT longitude */
} CPOS_COORDINATE_STRU;


typedef struct
{
    /*
    <!ELEMENT polygon (coordinate+)>
    */
    VOS_UINT32                          ulCnt;
    CPOS_COORDINATE_STRU                astCoordinate[CPOS_POLYGON_COORDINATE_MAX_NUM]; /* ELEMENT coordinate */
} CPOS_POLYGON_STRU;


typedef struct
{
    /*
    <!ELEMENT ellipsoid_point (coordinate)>
    */
    CPOS_COORDINATE_STRU                stCoordinate;                           /* ELEMENT coordinate */
} CPOS_ELLIP_POINT_STRU;


typedef struct
{
    /*
    <!ELEMENT ellipsoid_point_uncert_circle (coordinate,uncert_circle)>
        <!ELEMENT uncert_circle (#PCDATA)> <!-- 0..127 -->
    */
    CPOS_COORDINATE_STRU                stCoordinate;                           /* ELEMENT coordinate */
    VOS_UINT8                           ucUncertCircle;                         /* ELEMENT uncert_circle */
    VOS_UINT8                           aucReserved[3];
} CPOS_ELLIP_POINT_UNCERT_CIRCLE_STRU;


typedef struct
{
    /*
    <!ELEMENT uncert_ellipse (uncert_semi_major,uncert_semi_minor,orient_major,confidence)>
        <!ELEMENT uncert_semi_major (#PCDATA)>  <!-- 0..127 -->
        <!ELEMENT uncert_semi_minor (#PCDATA)>  <!-- 0..127 -->
        <!ELEMENT orient_major (#PCDATA)>  <!-- 0..89 -->
        <!ELEMENT confidence (#PCDATA)>  <!-- 0..100 -->
    */
    VOS_UINT8                           ucUncertSemiMajor;                      /* ELEMENT uncert_semi_major */
    VOS_UINT8                           ucUncertSemiMinor;                      /* ELEMENT uncert_semi_minor */
    VOS_UINT8                           ucOrientMajor;                          /* ELEMENT orient_major */
    VOS_UINT8                           ucConfidence;                           /* ELEMENT confidence */
} CPOS_UNCERT_ELLIPSE_STRU;


typedef struct
{
    /*
    <!ELEMENT ellipsoid_point_uncert_ellipse (coordinate,uncert_ellipse)>
    */
    CPOS_COORDINATE_STRU                stCoordinate;                           /* ELEMENT coordinate */
    CPOS_UNCERT_ELLIPSE_STRU            stUncertEllipse;                        /* ELEMENT uncert_ellipse */
} CPOS_ELLIP_POINT_UNCERT_ELLIP_STRU;


typedef struct
{
    /*
    <!ELEMENT altitude (height_above_surface,height)>
        <!ELEMENT height_above_surface (#PCDATA)>
        <!ELEMENT height (#PCDATA)> <!-- 0..32767 -->
    */
    CPOS_ALTITUDE_TYPE_ENUM_UINT8       enHeightAboveSurface;                   /* ELEMENT height_above_surface, 表示海平面以上还是以下 */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT16                          usHeight;                               /* ELEMENT height, 绝对值 */
} CPOS_ALTITUDE_STRU;


typedef struct
{
    /*
    <!ELEMENT ellipsoid_point_alt (coordinate,altitude)>
    */
    CPOS_COORDINATE_STRU                stCoordinate;                           /* ELEMENT coordinate, 坐标 */
    CPOS_ALTITUDE_STRU                  stAltitude;                             /* ELEMENT altitude, 海拔 */
} CPOS_ELLIP_POINT_ALT_STRU;


typedef struct
{
    /*
    <!ELEMENT ellipsoid_point_alt_uncertellipse (coordinate,altitude,
        uncert_semi_major,uncert_semi_minor,orient_major,confidence,uncert_alt)>
       <!ELEMENT uncert_semi_major (#PCDATA)>  <!-- 0..127 -->
       <!ELEMENT uncert_semi_minor (#PCDATA)>  <!-- 0..127 -->
       <!ELEMENT orient_major (#PCDATA)>  <!-- 0..89 -->
       <!ELEMENT confidence (#PCDATA)>  <!-- 0..100 -->
       <!ELEMENT uncert_alt (#PCDATA)>  <!-- 0..127 -->
    */
    CPOS_COORDINATE_STRU                stCoordinate;                           /* ELEMENT coordinate, 坐标 */
    CPOS_ALTITUDE_STRU                  stAltitude;                             /* ELEMENT altitude, 海拔 */
    VOS_UINT8                           ucUncertSemiMajor;                      /* ELEMENT uncert_semi_major */
    VOS_UINT8                           ucUncertSemiMinor;                      /* ELEMENT uncert_semi_minor */
    VOS_UINT8                           ucOrientMajor;                          /* ELEMENT orient_major */
    VOS_UINT8                           ucConfidence;                           /* ELEMENT confidence */
    VOS_UINT8                           ucUncertAlt;                            /* ELEMENT uncert_alt */
    VOS_UINT8                           aucReserved[3];
} CPOS_ELLIP_POINT_ALT_UNCERT_ELLIP_STRU;


typedef struct
{
    /*
    <!ELEMENT ellips_arc (coordinate,inner_rad,uncert_rad,offset_angle,included_angle,confidence)>
        <!ELEMENT inner_rad (#PCDATA)>
        <!ELEMENT uncert_rad (#PCDATA)>
        <!ELEMENT offset_angle (#PCDATA)>
        <!ELEMENT included_angle (#PCDATA)>
        <!ELEMENT confidence (#PCDATA)>  <!-- 0..100 -->
    */
    CPOS_COORDINATE_STRU                stCoordinate;                           /* ELEMENT coordinate, 坐标 */
    VOS_UINT16                          usInnerRad;                             /* ELEMENT inner_rad, 2 octets */
    VOS_UINT8                           ucUncertRad;                            /* ELEMENT uncert_rad, 1 octets */
    VOS_UINT8                           ucOffsetAngle;                          /* ELEMENT offset_angle, 1 octets */
    VOS_UINT8                           ucIncludedAngle;                        /* ELEMENT included_angle, 1 octets */
    VOS_UINT8                           ucConfidence;                           /* ELEMENT confidence */
    VOS_UINT8                           aucReserved[2];
} CPOS_ELLIP_ARC_STRU;


typedef struct
{
    /*
    <!ELEMENT shape_data (ellipsoid_point|ellipsoid_point_uncert_circle|
        ellipsoid_point_uncert_ellipse|polygon|ellipsoid_point_alt|
        ellipsoid_point_alt_uncertellipse|ellips_arc)>
    */
    VOS_UINT8                                   aucReserved[3];

    CPOS_SHAPE_DATA_ELE_TYPE_ENUM_UINT8         enChoice;
    union
    {
        CPOS_ELLIP_POINT_STRU                   stEllipPoint;                   /* ELEMENT ellipsoid_point */
        CPOS_ELLIP_POINT_UNCERT_CIRCLE_STRU     stEllipPointUncertCircle;       /* ELEMENT ellipsoid_point_uncert_circle */
        CPOS_ELLIP_POINT_UNCERT_ELLIP_STRU      stEllipPointUncertEllip;        /* ELEMENT ellipsoid_point_uncert_ellipse */
        CPOS_POLYGON_STRU                       stPolygon;                      /* ELEMENT polygon */
        CPOS_ELLIP_POINT_ALT_STRU               stEllipPointAlt;                /* ELEMENT ellipsoid_point_alt */
        CPOS_ELLIP_POINT_ALT_UNCERT_ELLIP_STRU  stEllipPointAltiEllip;          /* ELEMENT ellipsoid_point_alt_uncertellipse */
        CPOS_ELLIP_ARC_STRU                     stEllipArc;                     /* ELEMENT ellips_arc */
    } u;
} CPOS_SHAPE_DATA_STRU;


typedef struct
{
    /*
    <!ELEMENT velocity_data (hor_velocity?,vert_velocity?,vert_velocity_direction?,hor_uncert?,vert_uncert?)>
        <!ELEMENT hor_velocity (#PCDATA)>  <!-- 0..2047 -->
        <!ELEMENT vert_velocity (#PCDATA)> <!-- 0..255 -->
        <!ELEMENT vert_velocity_direction (#PCDATA)>
        <!ELEMENT hor_uncert (#PCDATA)> <!-- 0..255 -->
        <!ELEMENT vert_uncert (#PCDATA)> <!-- 0..255 -->
    */
    VOS_UINT32                          bitOpHorVelocity            : 1;
    VOS_UINT32                          bitOpVertVelocity           : 1;
    VOS_UINT32                          bitOpVertVelocityDirection  : 1;
    VOS_UINT32                          bitOpHorUncer               : 1;
    VOS_UINT32                          bitOpVertUncert             : 1;
    VOS_UINT32                          bitOpSpare                  : 27;

    VOS_UINT16                          usHorVelocity;                          /* ELEMENT hor_velocity */
    VOS_UINT8                           ucVertVelocity;                         /* ELEMENT vert_velocity */
    CPOS_VERT_VELOCITY_DIR_ENUM_UINT8   enVertVelocityDirection;                /* ELEMENT vert_velocity_direction */
    VOS_UINT8                           ucHorUncert;                            /* ELEMENT hor_uncert */
    VOS_UINT8                           ucVertUncert;                           /* ELEMENT vert_uncert */
    VOS_UINT8                           aucReserved[2];
} CPOS_VELOCITY_DATA_STRU;


typedef struct
{
    /*
    <!ELEMENT location_parameters (time?,direction?,shape_data,velocity_data?)>
        <!ELEMENT time (#PCDATA)> <!-- 0..14399999 -->
        <!ELEMENT direction (#PCDATA)> <!-- 0..359 -->
    */
    VOS_UINT32                          bitOpTime           : 1;
    VOS_UINT32                          bitOpDirection      : 1;
    VOS_UINT32                          bitOpVelocityData   : 1;
    VOS_UINT32                          bitOpSpare          : 29;

    VOS_UINT32                          ulTime;                                 /* ELEMENT time, 和GPS TOW含义相同 */
    VOS_UINT16                          usDirection;                            /* ELEMENT direction, 映射到水平方向的速率 */
    VOS_UINT8                           aucReserved[2];
    CPOS_SHAPE_DATA_STRU                stShapeData;                            /* ELEMENT shape_data */
    CPOS_VELOCITY_DATA_STRU             stVelocityData;                         /* ELEMENT velocity_data */
} CPOS_LOCATION_PARAMS_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_time (GPS_TOW_msec,GPS_week)>
        <!ELEMENT GPS_TOW_msec (#PCDATA)>
        <!ELEMENT GPS_week (#PCDATA)>
    */
    VOS_UINT32                          ulGpsTowMsec;                           /* ELEMENT GPS_TOW_msec */
    VOS_UINT16                          usGpsWeek;                              /* ELEMENT GPS_week */
    VOS_UINT8                           aucReserved[2];
}CPOS_GPS_TIME_STRU;


typedef struct
{
    /*
    <!ELEMENT GSM_time (frame_number,frame_number_msb,time_slot_number,bit_number,BCCH_carrier?,BSIC?)>
       <!ELEMENT frame_number (#PCDATA)>  <!-- 0..2097151 -->
       <!ELEMENT frame_number_msb (#PCDATA)>
       <!ELEMENT time_slot_number (#PCDATA)> <!-- 0..7 -->
       <!ELEMENT bit_number (#PCDATA)>  <!-- 0..156 -->
       <!ELEMENT BCCH_carrier (#PCDATA)>  <!-- 0..1023 -->
       <!ELEMENT BSIC (#PCDATA)>  <!-- 0..63 -->
    */
    VOS_UINT32                          bitOpBcchCarrier    : 1;
    VOS_UINT32                          bitOpBsic           : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT32                          ulFrameNum;                             /* ELEMENT frame_number */
    VOS_UINT32                          ulFrameNumMsb;                          /* ELEMENT frame_number_msb */
    VOS_UINT8                           ucTimeSlotNum;                          /* ELEMENT time_slot_number, INTEGER() */
    VOS_UINT8                           ucBitNum;                               /* ELEMENT bit_number, INTEGER(0..156) */
    VOS_UINT16                          usBcchCarrier;                          /* ELEMENT BCCH_carrier, INTEGER(0..1023) */
    VOS_UINT8                           ucBsic;                                 /* ELEMENT BSIC, INTEGER(0..63) */
    VOS_UINT8                           aucReserved[3];
} CPOS_GSM_TIME_STRU;


typedef struct
{
    /*
    <!ELEMENT WCDMA_time (cell_timing_chips,(FDD_CPICH|TDD_cell_id|SFN))>
       <!ELEMENT cell_timing_chips (#PCDATA)>
       <!ELEMENT FDD_CPICH (#PCDATA)>
       <!ELEMENT TDD_cell_id (#PCDATA)>
       <!ELEMENT SFN (#PCDATA)>
    */
    VOS_UINT32                          ulCellTimingChips;                      /* ELEMENT cell_timing_chips */
    VOS_UINT8                           aucReserved[3];
    CPOS_WCDMA_TIME_ELE_TYPE_ENUM_UINT8 enChoice;
    union
    {
        VOS_UINT32                      ulFddCpich;                             /* ELEMENT FDD_CPICH */
        VOS_UINT32                      ulTddCellId;                            /* ELEMENT TDD_cell_id */
        VOS_UINT32                      ulSFN;                                  /* ELEMENT SFN */
    } u;
}CPOS_WCDMA_TIME_STRU;


typedef struct
{
    /*
    <!ELEMENT LTE_time (physCellId,cellGlobalIdEUTRA?,earfcn)>
        <!ELEMENT physCellId (#PCDATA)>
        <!ELEMENT cellGlobalIdEUTRA (#PCDATA)>
        <!ELEMENT earfcn (#PCDATA)>
    */
    VOS_UINT32                          bitOpCellGlobalIdEutra  : 1;
    VOS_UINT32                          bitOpSpare              : 31;

    VOS_UINT32                          ulCellGlobalIdEutra;                    /* ELEMENT cellGlobalIdEUTRA */
    VOS_UINT16                          usPhysCellId;                           /* ELEMENT physCellId */
    VOS_UINT16                          usEarfcn;                               /* ELEMENT earfcn */
}CPOS_LTE_TIME_STRU;


typedef struct
{
    VOS_UINT8                           aucReserved[3];

    CPOS_GUL_TIME_ELE_TYPE_ENUM_UINT8   enChoice;
    union
    {
        CPOS_GSM_TIME_STRU              stGsmTime;                              /* ELEMENT GSM_time */
        CPOS_WCDMA_TIME_STRU            stWcdmaTime;                            /* ELEMENT WCDMA_time */
        CPOS_LTE_TIME_STRU              stLteTime;                              /* ELEMENT LTE_time */
    } u;
}CPOS_GUL_TIME_STRU;


typedef struct
{
    /*
    <!ELEMENT TimeUnc EMPTY>
        <!ATTLIST TimeUnc Value CDATA "127"
            units (s|ms|us|rrlp_k|rrc_k|lpp_k) "rrc_k">
    */
    VOS_UINT32                          ulValue;                                /* ATTRIBUTE Value */
    CPOS_TIME_UNC_UNITS_ENUM_UINT8      enUnits;                                /* ATTRIBUTE units */
    VOS_UINT8                           aucReserved[3];
}CPOS_TIME_UNC_STRU;


typedef struct
{
    /*
    <!ELEMENT time_assistance (GPS_time,(GSM_time|WCDMA_time|LTE_time)?,TimeUnc?)>
    */
    VOS_UINT32                          bitOpGULTime    : 1;
    VOS_UINT32                          bitOpTimeUnc    : 1;
    VOS_UINT32                          bitOpSpare      : 30;

    CPOS_GPS_TIME_STRU                  stGpsTime;                              /* ELEMENT GPS_time */
    CPOS_GUL_TIME_STRU                  stGULTime;                              /* ELEMENT (GSM_time|WCDMA_time|LTE_time) */
    CPOS_TIME_UNC_STRU                  stTimeUnc;                              /* ELEMENT TimeUnc */
} CPOS_TIME_ASSISTANCE_STRU;


typedef struct
{
    /*
    <!ELEMENT location (location_parameters,time_of_fix?,time_assistance?)>
        <!ELEMENT time_of_fix (#PCDATA)>
    */
    VOS_UINT32                          bitOpTimeOfFix         : 1;
    VOS_UINT32                          bitOpTimeAssistance    : 1;
    VOS_UINT32                          bitOpSpare             : 30;

    CPOS_LOCATION_PARAMS_STRU           stLocationParams;                       /* ELEMENT location_parameters, 位置信息 */
    VOS_UINT32                          ulTimeOfFix;                            /* ELEMENT time_of_fix, 当前接入层不使用 */
    CPOS_TIME_ASSISTANCE_STRU           stTimeAssistance;                       /* ELEMENT time_assistance */
} CPOS_LOCATION_STRU;


typedef struct
{
    /*
    <!ELEMENT sat_meas_params_gnss (sat_id,code_phase_ambiguity?,carr2_noise,
        doppler_mps?,code_phase_ms,integer_code_phase?,multi_path,psr_rms_err,
        adr?,whole_chips?,fract_chips?)>
        <!ATTLIST sat_meas_params_gnss measured_signal (GPS_L1|GPS_L1C|GPS_L2C|
            GPS_L5|SBAS_L1|GLO_G1|GLO_G2|GLO_G3|BDS_B11) "GPS_L1">
        <!ELEMENT sat_id (#PCDATA)> <!-- 0..63 -->
        <!ELEMENT code_phase_ambiguity (#PCDATA)>  <!-- 0..127 -->
        <!ELEMENT carr2_noise (#PCDATA)>
        <!ELEMENT doppler_mps (#PCDATA)>  <!-- m/s Range -32768..32767 -->
        <!ELEMENT code_phase_ms (#PCDATA)>  <!-- ms  Range 0..2097151 -->
        <!ELEMENT integer_code_phase (#PCDATA)>  <!-- 0..127 -->
        <!ELEMENT multi_path EMPTY>
            <!ATTLIST multi_path literal (not_measured|low|medium|high) #REQUIRED>
        <!ELEMENT psr_rms_err (#PCDATA)>  <!-- 0..63 -->
        <!ELEMENT adr (#PCDATA)>  <!-- 0..33554431 -->
        <!ELEMENT whole_chips (#PCDATA)>  <!-- 0..1022 -->
        <!ELEMENT fract_chips (#PCDATA)>  <!-- 0..1024 -->
    */
    VOS_UINT32                          bitOpCodePhaseAmbiguity : 1;
    VOS_UINT32                          bitOpDopplerMps         : 1;
    VOS_UINT32                          bitOpIntegerCodePhase   : 1;
    VOS_UINT32                          bitOpAdr                : 1;
    VOS_UINT32                          bitOpWholeChips         : 1;
    VOS_UINT32                          bitOpFractChips         : 1;
    VOS_UINT32                          bitSpare                : 26;

    CPOS_SIGNAL_ID_ENUM_UINT8           enMeasuredSignal;                       /* ATTRIBUTE measured_signal */
    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id */
    VOS_UINT8                           ucCodePhaseAmbiguity;                   /* ELEMENT code_phase_ambiguity */
    VOS_UINT8                           ucCarr2Noise;                           /* ELEMENT carr2_noise */
    VOS_INT16                           sDopplerMps;                            /* ELEMENT doppler_mps */
    VOS_UINT8                           aucReserved0[2];
    VOS_UINT32                          ulCodePhaseMs;                          /* ELEMENT code_phase_ms */
    VOS_UINT8                           ucIntegerCodePhase;                     /* ELEMENT integer_code_phase */
    CPOS_MULTI_PATH_LITERAL_ENUM_UINT8  enMultiPath;                            /* ELEMENT multi_path */
    VOS_UINT8                           ucPsrRmsErr;                            /* ELEMENT psr_rms_err */
    VOS_UINT8                           aucReserved1[1];
    VOS_UINT32                          ulAdr;                                  /* ELEMENT adr */
    VOS_UINT16                          usWholeChips;                           /* ELEMENT whole_chips */
    VOS_UINT16                          usFracChips;                            /* ELEMENT fract_chips */
}CPOS_SAT_MEAS_PARAMS_GNSS_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_SAT_MEAS_PARAMS_GNSS_STRU      astSatMeasParamsGnss[CPOS_GNSS_SAT_MAX_NUM];
}CPOS_SAT_MEAS_PARAMS_GNSS_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT TOD_ms (#PCDATA)>  <!-- time of day in ms in decimal to 5 decimal places. Format #######.##### -->
       <!ATTLIST TOD_ms gnss_time_id (GPS|Glonass|QZSS|BDS) "GPS">
    */
    CPOS_GNSS_TIME_ID_ENUM_UINT8        enGnssTimeId;                           /* ATTRIBUTE gnss_time_id */
    VOS_UINT8                           aucReserved[3];
    VOS_UINT32                          ulValueInt;                             /* 小数点前的整数 */
    VOS_UINT32                          ulValueDec;                             /* 小数点后的小数 */
}CPOS_TOD_MS_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_meas (sat_meas_params_gnss*,TOD_ms)>
        <!ATTLIST GNSS_meas gnss_id (GPS|SBAS|QZSS|Galileo|Glonass|BDS) "GPS">
    */
    CPOS_GNSS_ID_ENUM_UINT8                     enGnssId;                       /* ATTRIBUTE gnss_id */
    VOS_UINT8                                   aucReserved[3];
    CPOS_SAT_MEAS_PARAMS_GNSS_ARRAY_STRU        stSatMeasParamsGnssList;        /* ELEMENT sat_meas_params_gnss* */
    CPOS_TOD_MS_STRU                            stTODms;                        /* ELEMENT TOD_ms */
}CPOS_GNSS_MEAS_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_MEAS_STRU                 astGnssMeas[CPOS_GNSS_MEAS_ARRAY_MAX_NUM];
}CPOS_GNSS_MEAS_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_meas_ref_time (gnss_TOD_msec,gnss_TOD_frac?,gnss_TOD_unc?)>
        <!ATTLIST GNSS_meas_ref_time time_id (SBAS|MGPS|QZSS|Glonass|GPS|BDS) #IMPLIED>
        <!ELEMENT gnss_TOD_msec (#PCDATA)>  <!-- 0..3599999 -->
        <!ELEMENT gnss_TOD_frac (#PCDATA)>  <!-- 0..39999 -->
        <!ELEMENT gnss_TOD_unc (#PCDATA)>  <!-- 0..127 -->
    */
    VOS_UINT32                          bitOpTimeId         : 1;
    VOS_UINT32                          bitOpGnssTodFrac    : 1;
    VOS_UINT32                          bitOpGnssTodUnc     : 1;
    VOS_UINT32                          bitSpare            : 29;

    CPOS_TIME_ID_ENUM_UINT8             enTimeId;                               /* ATTRIBUTE time_id* */
    VOS_UINT8                           ucGnssTodUnc;                           /* ELEMENT gnss_TOD_unc* */
    VOS_UINT16                          usGnssTodFrac;                          /* ELEMENT gnss_TOD_frac* */
    VOS_UINT32                          ulGnssTodMsec;                          /* ELEMENT gnss_TOD_msec* */
}CPOS_GNSS_MEAS_REF_TIME_STRU;


typedef struct
{
    /*
    <!ELEMENT agnss_list (#PCDATA)>  <!-- Integer corresponds to bit string 1-8 where
        bit at position if set, means particular GNSS id is supported bit0-GPS,
        bit1-sbas,bit2-qzss,bit3-galileo,bit4-glonass,bit5-bds -->
    */
    VOS_UINT8                           ucAGnssList;
    VOS_UINT8                           aucReserved[3];
    CPOS_GNSS_MEAS_REF_TIME_STRU        stGnssMeasRefTime;
    CPOS_LOCATION_PARAMS_STRU           stLocationParams;
}CPOS_GNSS_LOCATION_INFO_MULTI_LOCATION_STRU;


typedef struct
{
    /*
    <!ELEMNT GNSS_provided_location_information (GNSS_meas*|(agnss_list,
        GNSS_meas_ref_time,location_parameters))>
    */
    VOS_UINT8                                       aucReserved[3];
    CPOS_GNSS_LOCATION_INFO_ELE_TYPE_ENUM_UINT8     enChoice;

    union
    {
        CPOS_GNSS_MEAS_ARRAY_STRU                   stGnssMeasArray;            /* ELEMENT GNSS_meas* */
        CPOS_GNSS_LOCATION_INFO_MULTI_LOCATION_STRU stMultiLocation;            /* ELEMENT (agnss_list,GNSS_meas_ref_time,location_parameters) */
    } u;

}CPOS_GNSS_LOCATION_INFO_STRU;


typedef struct
{
    /*
    R12协议中没有描述，R13协议节点被删除
    */
    VOS_UINT32                          ulGnssTodMsec;                          /* ELEMENT gnss_TOD_msec */
    VOS_UINT32                          ulGnssDay;                              /* ELEMENT GNSS_day */
}CPOS_GNSS_TIME_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_TOW_assist (sat_id,tlm_word,anti_sp,alert,tlm_res)>
       <!ELEMENT sat_id (#PCDATA)> <!-- 0..63 -->
       <!ELEMENT tlm_word (#PCDATA)>
       <!ELEMENT anti_sp (#PCDATA)>
       <!ELEMENT alert (#PCDATA)>
       <!ELEMENT tlm_res (#PCDATA)>
    */
    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id */
    VOS_UINT8                           aucReserved1[1];
    VOS_UINT16                          usTlmWord;                              /* ELEMENT tlm_word */
    CPOS_BOOL_ENUM_UINT8                enAntiSp;                               /* ELEMENT anti_sp */
    CPOS_BOOL_ENUM_UINT8                enAlert;                                /* ELEMENT alert */
    VOS_UINT8                           ucTlmRes;                               /* ELEMENT tlm_res */
    VOS_UINT8                           aucReserved2[1];
}CPOS_GPS_TOW_ASSIST_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GPS_TOW_ASSIST_STRU            astGpsTowAssist[CPOS_GNSS_TOW_ASSIST_ARRAY_MAX_NUM];
}CPOS_GPS_TOW_ASSIST_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT ref_time ((GPS_time|GNSS_time),(GSM_time|WCDMA_time|LTE_time)?,
        TimeUnc?,GPS_TOW_assist*)>
    */
    VOS_UINT32                          bitOpGULTime            : 1;
    VOS_UINT32                          bitOpTimeUnc            : 1;
    VOS_UINT32                          bitSpare                : 30;

    VOS_UINT8                           aucReserved[3];
    CPOS_REF_TIME_ELE_TYPE_ENUM_UINT8   enChoice;

    union
    {
        CPOS_GPS_TIME_STRU              stGpsTime;                              /* ELEMENT GPS_time */
        CPOS_GNSS_TIME_STRU             stGnssTime;                             /* ELEMENT GNSS_time */
    } u;

    CPOS_GUL_TIME_STRU                  stGULTime;                              /* ELEMENT (GSM_time|WCDMA_time|LTE_time) */
    CPOS_TIME_UNC_STRU                  stTimeUnc;                              /* ELEMENT TimeUnc */
    CPOS_GPS_TOW_ASSIST_ARRAY_STRU      stGpsTowAssistArray;                    /* ELEMENT GPS_TOW_assist* */
}CPOS_REF_TIME_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_REF_TIME_STRU                  astRefTime[CPOS_SAT_MAX_NUM];
}CPOS_REF_TIME_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT DGPS_corrections (sat_id,iode,UDRE,PRC,RRC,delta_PRC2?,delta_RRC2?)>
       <!ELEMENT sat_id (#PCDATA)> <!-- 0..63 -->
       <!ELEMENT iode (#PCDATA)>  <!-- 0..255 -->
       <!ELEMENT UDRE (#PCDATA)>
       <!ELEMENT PRC (#PCDATA)>  <!-- -127..127 -->
       <!ELEMENT RRC (#PCDATA)>  <!-- -2047..2047 -->
       <!ELEMENT delta_PRC2 (#PCDATA)> <!-- -127..127 -->
       <!ELEMENT delta_RRC2 (#PCDATA)> <!-- -7..7 -->
    */
    VOS_UINT32                          bitOpDeltaPrc2  :1;
    VOS_UINT32                          bitOpDeltaRrc2  :1;
    VOS_UINT32                          bitSpare        :30;

    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT8                           ucIode;                                 /* ELEMENT IODE, INTEGER() */
    CPOS_UDRE_ENUM_UINT8                enUdre;                                 /* ELEMENT UDRE, ENUMERATED */
    VOS_INT8                            cRrc;                                   /* ELEMENT RRC, INTEGER(-127..127) */
    VOS_INT16                           sPrc;                                   /* ELEMENT PRC, INTEGER(-2047..2047) */
    VOS_INT8                            cDeltaPrc2;                             /* ELEMENT delta_PRC2, INTEGER(-127..127) */
    VOS_INT8                            cDeltaRrc2;                             /* ELEMENT delta_RRC2, INTEGER(-7..7) */
}CPOS_DGPS_CORRECTIONS_STRU;


typedef struct
{
    /*
    <!ELEMENT ephem_and_clock (l2_code,ura,sv_health,iodc,l2p_flag,
        esr1,esr2,esr3,esr4,tgd,toc,af2,af1,af0,crs,delta_n,m0,cuc,
        ecc,cus,power_half,toe,fit_flag,aoda,cic,omega0,cis,i0,crc,
        omega,omega_dot,idot,derived?)>
        <!ELEMENT l2_code (#PCDATA)>
        <!ELEMENT ura (#PCDATA)>
        <!ELEMENT sv_health (#PCDATA)>
        <!ELEMENT iodc (#PCDATA)>
        <!ELEMENT l2p_flag (#PCDATA)>
        <!ELEMENT esr1 (#PCDATA)>
        <!ELEMENT esr2 (#PCDATA)>
        <!ELEMENT esr3 (#PCDATA)>
        <!ELEMENT esr4 (#PCDATA)>
        <!ELEMENT tgd (#PCDATA)>
        <!ELEMENT toc (#PCDATA)>
        <!ELEMENT af2 (#PCDATA)>
        <!ELEMENT af1 (#PCDATA)>
        <!ELEMENT af0 (#PCDATA)>
        <!ELEMENT crs (#PCDATA)>
        <!ELEMENT delta_n (#PCDATA)>
        <!ELEMENT m0 (#PCDATA)>
        <!ELEMENT cuc (#PCDATA)>
        <!ELEMENT ecc (#PCDATA)>
        <!ELEMENT cus (#PCDATA)>
        <!ELEMENT power_half (#PCDATA)>
        <!ELEMENT toe (#PCDATA)>
        <!ELEMENT fit_flag (#PCDATA)>
        <!ELEMENT aoda (#PCDATA)>
        <!ELEMENT cic (#PCDATA)>
        <!ELEMENT omega0 (#PCDATA)>
        <!ELEMENT cis (#PCDATA)>
        <!ELEMENT i0 (#PCDATA)>
        <!ELEMENT crc (#PCDATA)>
        <!ELEMENT omega (#PCDATA)>
        <!ELEMENT omega_dot (#PCDATA)>
        <!ELEMENT idot (#PCDATA)>
        <!ELEMENT derived EMPTY>  <!-- data derived locally, e.g from EE -->
    */
    VOS_UINT8                           bitOpDerived    : 1;
    VOS_UINT8                           bitSpare        : 7;

    VOS_UINT8                           ucL2Code;                               /* ELEMENT l2_code, INTEGER (0..3),BIT STRING(SIZE(2)) */
    VOS_UINT8                           ucUra;                                  /* ELEMENT ura, INTEGER (0..15),BIT STRING(SIZE(4)) */
    VOS_UINT8                           ucSvHealth;                             /* ELEMENT sv_health, INTEGER (0..63),BIT STRING(SIZE(6)) */
    VOS_UINT16                          usIodc;                                 /* ELEMENT iodc, INTEGER (0..1023),BIT STRING(SIZE(10)) */
    VOS_UINT8                           ucL2PFlag;                              /* ELEMENT l2p_flag, INTEGER (0..1),BIT STRING(SIZE(1)) */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT32                          ulEsr1;                                 /* ELEMENT esr1 INTEGER (0..8388607), -- 23-bit field */
    VOS_UINT32                          ulEsr2;                                 /* ELEMENT esr2 INTEGER (0..16777215), -- 24-bit field */
    VOS_UINT32                          ulEsr3;                                 /* ELEMENT esr3 INTEGER (0..16777215), -- 24-bit field */
    VOS_UINT16                          ulEsr4;                                 /* ELEMENT esr4 INTEGER (0..65535), -- 16-bit field*/
    VOS_INT16                           sTgd;                                   /* ELEMENT tgd, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_UINT16                          usToc;                                  /* ELEMENT toc, INTEGER (0..37799),BIT STRING(SIZE(16)) */
    VOS_INT16                           sAf2;                                   /* ELEMENT af2, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT32                           lAf1;                                   /* ELEMENT af1, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_INT32                           lAf0;                                   /* ELEMENT af0, INTEGER (-2097152..2097151),BIT STRING(SIZE(22)) */
    VOS_INT32                           lCrs;                                   /* ELEMENT crs, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_INT32                           lDeltaN;                                /* ELEMENT delta_n, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_INT32                           lM0;                                    /* ELEMENT m0, INTEGER (-2147483648..2147483647),BIT STRING(SIZE(32)) */
    VOS_INT32                           lCuc;                                   /* ELEMENT cuc, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_UINT32                          ulEcc;                                  /* ELEMENT ecc, INTEGER (0..4294967295),BIT STRING(SIZE(32)) */
    VOS_INT32                           lCus;                                   /* ELEMENT cus, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_UINT32                          ulPowerHalf;                            /* ELEMENT power_half, BIT STRING(SIZE(32)) */
    VOS_UINT16                          usToe;                                  /* ELEMENT toe, INTEGER (0..37799),BIT STRING(SIZE(16)) */
    VOS_UINT8                           ucFitFlag;                              /* ELEMENT fit_flag, INTEGER (0..1),BIT STRING(SIZE(1)) */
    VOS_UINT8                           ucAoda;                                 /* ELEMENT aoda, INTEGER (0..31),BIT STRING(SIZE(5)) */
    VOS_INT32                           lCic;                                   /* ELEMENT cic, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_INT32                           lOmega0;                                /* ELEMENT omega0, INTEGER (-2147483648..2147483647),BIT STRING(SIZE(32)) */
    VOS_INT32                           lCis;                                   /* ELEMENT cis, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_INT32                           lI0;                                    /* ELEMENT i0, INTEGER (-2147483648..2147483647),BIT STRING(SIZE(32)) */
    VOS_INT32                           lCrc;                                   /* ELEMENT crc, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_INT32                           lOmega;                                 /* ELEMENT omega, INTEGER (-2147483648..2147483647),BIT STRING(SIZE(32)) */
    VOS_INT32                           lOmegaDot;                              /* ELEMENT omega_dot, INTEGER (-8388608..8388607),BIT STRING(SIZE(24)) */
    VOS_INT32                           lIdot;                                  /* ELEMENT idot, INTEGER (-8192..8191), BIT STRING(SIZE(14)) */
}CPOS_EPHEM_AND_CLOCK_STRU;


typedef struct
{
    /*
    <!ELEMENT nav_model_elem (sat_id,sat_status,ephem_and_clock?)>
        <!ELEMENT sat_status EMPTY>
            <!ATTLIST sat_status literal (NS_NN-U|ES_NN-U|NS_NN|ES_SN|REVD) #REQUIRED>
    */
    VOS_UINT32                          bitEphemClock:1;
    VOS_UINT32                          bitOpSpare:31;

    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    CPOS_POS_SAT_STA_ENUM_UINT8         enSatStaus;                             /* ATTLIST sat_status, ENUMERATED */
    VOS_UINT8                           aucReserved[2];

    CPOS_EPHEM_AND_CLOCK_STRU           stEphemAndClock;
}CPOS_NAV_MODEL_ELEM_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_NAV_MODEL_ELEM_STRU            astNavModelElem[CPOS_SAT_MAX_NUM];
}CPOS_NAV_MODEL_ELEM_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT ionospheric_model (alfa0,alfa1,alfa2,alfa3,beta0,beta1,beta2,beta3)>
        <!ELEMENT alfa0 (#PCDATA)>
        <!ELEMENT alfa1 (#PCDATA)>
        <!ELEMENT alfa2 (#PCDATA)>
        <!ELEMENT alfa3 (#PCDATA)>
        <!ELEMENT beta0 (#PCDATA)>
        <!ELEMENT beta1 (#PCDATA)>
        <!ELEMENT beta2 (#PCDATA)>
        <!ELEMENT beta3 (#PCDATA)>
    */
    VOS_INT16                           sAlpha0;                                /* ELEMENT alfa0, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sAlpha1;                                /* ELEMENT alfa1, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sAlpha2;                                /* ELEMENT alfa2, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sAlpha3;                                /* ELEMENT alfa3, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sBeta0;                                 /* ELEMENT beta0, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sBeta1;                                 /* ELEMENT beta1, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sBeta2;                                 /* ELEMENT beta2, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sBeta3;                                 /* ELEMENT beta3, INTEGER (-128..127),BIT STRING(SIZE(8)) */
}CPOS_IONOSPHERIC_MODEL_STRU;


typedef struct
{
    /*
    <!ELEMENT UTC_model (a1,a0,tot,wnt,dtls,wnlsf,dn,dtlsf)>
        <!ELEMENT a1 (#PCDATA)>
        <!ELEMENT a0 (#PCDATA)>
        <!ELEMENT tot (#PCDATA)>
        <!ELEMENT wnt (#PCDATA)>
        <!ELEMENT dtls (#PCDATA)>
        <!ELEMENT wnlsf (#PCDATA)>
        <!ELEMENT dn (#PCDATA)>
        <!ELEMENT dtlsf (#PCDATA)>
    */
    VOS_INT32                           lA1;                                    /* ELEMENT a1, INTEGER (-8388608..8388607),BIT STRING(SIZE(24)) */
    VOS_INT32                           lA0;                                    /* ELEMENT a0, INTEGER (-2147483648..2147483647),BIT STRING(SIZE(32)) */
    VOS_UINT8                           ucTot;                                  /* ELEMENT tot, INTEGER (0..255),BIT STRING(SIZE(8)) */
    VOS_UINT8                           ucWnt;                                  /* ELEMENT wnt, INTEGER (0..255),BIT STRING(SIZE(8)) */
    VOS_UINT8                           ucWnLsf;                                /* ELEMENT wnlsf, INTEGER (0..255),BIT STRING(SIZE(8)) */
    VOS_UINT8                           aucReserved1[1];                        /* 保留位 */
    VOS_INT16                           sDtls;                                  /* ELEMENT dtls, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           sDn;                                    /* ELEMENT dn, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_INT16                           cDtlsf;                                 /* ELEMENT dtlsf, INTEGER (-128..127),BIT STRING(SIZE(8)) */
    VOS_UINT8                           aucReserved2[2];                        /* 保留位 */
}CPOS_UTC_MODEL_STRU;


typedef struct
{
    /*
    <!ELEMENT alm_elem (data_id?,sat_id,alm_ecc,alm_toa,alm_ksii,alm_omega_dot,
        alm_sv_health,alm_power_half,alm_omega0,alm_omega,alm_m0,alm_af0,alm_af1)>
        <!ELEMENT data_id (#PCDATA)>
        <!ELEMENT alm_ecc (#PCDATA)>
        <!ELEMENT alm_toa (#PCDATA)>
        <!ELEMENT alm_ksii (#PCDATA)>
        <!ELEMENT alm_omega_dot (#PCDATA)>
        <!ELEMENT alm_sv_health (#PCDATA)>
        <!ELEMENT alm_power_half (#PCDATA)>
        <!ELEMENT alm_omega0 (#PCDATA)>
        <!ELEMENT alm_omega (#PCDATA)>
        <!ELEMENT alm_m0 (#PCDATA)>
        <!ELEMENT alm_af0 (#PCDATA)>
        <!ELEMENT alm_af1 (#PCDATA)>
    */
    VOS_UINT32                          bitOpDataId  :1;
    VOS_UINT32                          bitOpSpare   :31;

    VOS_UINT8                           ucDataId;                               /* ELEMENT data_id, INTEGER(0..3) */
    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT16                          usAlmEcc;                               /* ELEMENT alm_ecc, BIT STRING(SIZE(16)) */
    VOS_UINT8                           ucAlmToa;                               /* ELEMENT alm_toa, BIT STRING(SIZE(8)) */
    VOS_UINT8                           aucReserved1[3];
    VOS_INT32                           lAlmKsii;                               /* ELEMENT alm_ksii, INTEGER (-32768..32767),usDeltaI, BIT STRING(SIZE(16)) */
    VOS_INT32                           lAlmOmegaDot;                           /* ELEMENT alm_omega_dot, INTEGER (-32768..32767),BIT STRING(SIZE(16)) */
    VOS_UINT8                           ucAlmSvHealth;                          /* ELEMENT alm_sv_health, INTEGER (0..255),ucSatHealth, BIT STRING(SIZE(8)) */
    VOS_UINT8                           aucReserved2[3];
    VOS_UINT32                          ulAlmPowerHalf;                         /* ELEMENT alm_power_half, INTEGER (0..16777215),ulASqrt, BIT STRING(SIZE(24)) */
    VOS_INT32                           lAlmOmega0;                             /* ELEMENT alm_omega0, INTEGER (-8388608..8388607),BIT STRING(SIZE(24)) */
    VOS_INT32                           lAlmOmega;                              /* ELEMENT alm_omega, INTEGER (-8388608..8388607),BIT STRING(SIZE(24)) */
    VOS_INT32                           lAlmM0;                                 /* ELEMENT alm_m0, INTEGER (-8388608..8388607),BIT STRING(SIZE(24)) */
    VOS_INT16                           sAlmAf0;                                /* ELEMENT valm_af0, INTEGER (-1024..1023),BIT STRING(SIZE(11)) */
    VOS_INT16                           sAlmAf1;                                /* ELEMENT alm_af1, INTEGER (-1024..1023),BIT STRING(SIZE(11)) */
}CPOS_ALM_ELEM_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_ALM_ELEM_STRU                  astAlmElem[CPOS_SAT_MAX_NUM];
}CPOS_ALM_ELEM_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT almanac (wna,alm_elem*)>
        <!ELEMENT wna (#PCDATA)>
    */
    VOS_UINT8                           ucWna;                                  /* ELEMENT wna, INTEGER (0..255),BIT STRING(SIZE(8)) */
    VOS_UINT8                           aucReserved1[3];                        /* 保留位 */
    CPOS_ALM_ELEM_ARRAY_STRU            stAlmElemArray;
}CPOS_ALMANAC_STRU;


typedef struct
{
    /*
    <!ELEMENT dopl0 (#PCDATA)>
        <!ATTLIST dopl0 units (rrlp|rrc|lpp|Hz|mps) "rrlp">
        <!-- rrlp gps is 2.5 Hz -->
        <!-- rrc  gps is 2.5 Hz -->
        <!-- lpp  gnss is 0.5 m/s -->
    */
    VOS_INT16                           sValue;                                 /* INTEGER (-2048..2047) */
    CPOS_DOPL0_UNITS_ENUM_UINT8         enUnits;                                /* ATTRIBUTE units */
    VOS_UINT8                           aucReserved1[1];                        /* 保留位 */
}CPOS_DOPL0_STRU;


typedef struct
{
    /*
    <!ELEMENT dopl1 (#PCDATA)>
        <!ATTLIST dopl1 units (rrlp|rrc|lpp) "rrlp">
        <!-- rrlp is 1/42 Hz/s -->
        <!-- rrc  is 0.023 Hz/s -->
        <!-- lpp  is 1/210 m/s/s -->
    */
    VOS_INT16                           sValue;                                 /* INTEGER (-2048..2047) */
    CPOS_DOPL1_UNITS_ENUM_UINT8         enUnits;                                /* ATTRIBUTE units */
    VOS_UINT8                           aucReserved1[1];                        /* 保留位 */
}CPOS_DOPL1_STRU;


typedef struct
{
    /*
    <!ELEMENT dopl_extra (dopl1,dopl1_uncert)>
        <!ELEMENT dopl1_uncert EMPTY>
            <!ATTLIST dopl1_uncert literal (hz12-5|hz25|hz50|hz100|hz200) #REQUIRED>
    */
    CPOS_DOPL1_STRU                     stDopl1;                                /* ELEMENT dopl1 */
    CPOS_DOPL1_UNCERT_ENUM_UINT8        enDopl1Uncert;                          /* ATTRIBUTE dopl1_uncert */
    VOS_UINT8                           aucReserved1[3];                        /* 保留位 */
}CPOS_DOPL_EXTRA_STRU;


typedef struct
{
    /*
    <!ELEMENT code_ph (#PCDATA)>
        <!ATTLIST code_ph units (chips|ms) "chips">
        <!-- chips used for GPS in RRLP and RRC, ms used for LPP -->
    */
    VOS_UINT16                          usValue;                                /* INTEGER(0..1022) */
    CPOS_CODE_PH_UNITS_ENUM_UINT8       enUnits;                                /* ATTRIBUTE units */
    VOS_UINT8                           aucReserved1[1];                        /* 保留位 */
}CPOS_CODE_PH_STRU;


typedef struct
{
    /*
    <!ELEMENT az_el (az,elev)>
        <!ATTLIST az_el units (rrlp|lpp|degree) "rrlp">
        <!-- rrlp and rrc have units of 11.25 degree -->
        <!-- lpp has units of 0.703125 degrees -->
        <!ELEMENT az (#PCDATA)>
        <!ELEMENT elev (#PCDATA)>
    */
    CPOS_AZ_EL_UNITS_ENUM_UINT8         enUnits;                                /* ATTRIBUTE units */
    VOS_UINT8                           ucElev;                                 /* ELEMENT elev, INTEGER(0..7) */
    VOS_UINT16                          usAz;                                   /* ELEMENT az, INTEGER(0..551) */
}CPOS_AZ_EL_STRU;


typedef struct
{
    /*
    <!ELEMENT sat_info (sat_id,dopl0,dopl_extra?,code_ph,code_ph_int,GPS_bitno,
        srch_w,az_el?,doppler_uncertainty_ext_r10?)>
        <!ELEMENT code_ph_int (#PCDATA)>
        <!ELEMENT GPS_bitno (#PCDATA)>
        <!ELEMENT srch_w (#PCDATA)>
        <!ELEMENT doppler_uncertainty_ext_r10 EMPTY>
            <!ATTLIST doppler_uncertainty_ext_r10 literal (d60|d80|d100|d120|no_information) #IMPLIED>
    */
    VOS_UINT32                          bitOpDoplExtra          : 1;
    VOS_UINT32                          bitOpAzEl               : 1;
    VOS_UINT32                          bitOpDoplUncertExtR10   : 1;
    VOS_UINT32                          bitOpSpare              : 29;

    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT8                           aucReserved1[3];
    CPOS_DOPL0_STRU                     stDopl0;                                /* ELEMENT dopl0 */
    CPOS_DOPL_EXTRA_STRU                stDoplExtra;                            /* ELEMENT dopl_extra */
    CPOS_CODE_PH_STRU                   stCodePh;                               /* ELEMENT code_ph */
    VOS_UINT8                           ucCodePhInt;                            /* ELEMENT code_ph_int, INTEGER(0..19) */
    VOS_UINT8                           ucGpsBitno;                             /* ELEMENT GPS_bitno, INTEGER(0..3) */
    VOS_UINT8                           ucSrchW;                                /* ELEMENT srch_w, INTEGER(0..15) */
    VOS_UINT8                           aucReserved2[1];
    CPOS_AZ_EL_STRU                     stAzEl;                                 /* ELEMENT az_el */
    CPOS_DOPL_UNCERT_EXT_R10_ENUM_UINT8 enDoplUncertExtR10;                     /* ELEMENT doppler_uncertainty_ext_r10 */
    VOS_UINT8                           aucReserved3[3];
}CPOS_SAT_INFO_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_SAT_INFO_STRU                  astSatInfo[CPOS_GNSS_SAT_MAX_NUM];
}CPOS_SAT_INFO_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT acqu_assist (tow_msec,sat_info*,confidence_r10?)>
        <!ELEMENT tow_msec (#PCDATA)>
        <!ELEMENT confidence_r10 (#PCDATA)>  <!-- 0..100 -->
    */
    VOS_UINT32                          bitOpConfidenceR10  :1;
    VOS_UINT32                          bitOpSpare          :31;

    VOS_UINT32                          ulTowMsec;                              /* ELEMENT tow_msec, ulGpsRefTime, INTEGER(0..604799999) */
    CPOS_SAT_INFO_ARRAY_STRU            stSatInfoArray;                         /* ELEMENT sat_info, INTEGER(0..16) */
    VOS_UINT8                           ucConfidenceR10;                        /* ELEMENT confidence_r10, INTEGER(0..100) */
    VOS_UINT8                           aucReserved1[3];                        /* 保留位 */
}CPOS_ACQU_ASSIST_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_ACQU_ASSIST_STRU               astAcquAssist[CPOS_SAT_MAX_NUM];
}CPOS_ACQU_ASSIST_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_rt_integrity (bad_satellite_set)>
        <!ELEMENT bad_satellite_set (sat_id+)>  <!-- 1..16 -->
    */
    VOS_UINT32                          ulCnt;
    VOS_UINT8                           aucSatId[CPOS_SAT_MAX_NUM];
}CPOS_BAD_SATELLITE_SET_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_assist (status_health?,BTS_clock_drift?,ref_time*,
        location_parameters?,DGPS_corrections?,nav_model_elem*,ionospheric_model?,
        UTC_model?,almanac?,acqu_assist*,GPS_rt_integrity?)>
        <!ELEMENT status_health (#PCDATA)>
        <!ELEMENT BTS_clock_drift (#PCDATA)>
    */
    VOS_UINT32                          bitOpStatusHealth      : 1;
    VOS_UINT32                          bitOpBtsClockDrift     : 1;
    VOS_UINT32                          bitOpLocationParams    : 1;
    VOS_UINT32                          bitOpDgpsCorrections   : 1;
    VOS_UINT32                          bitOpIonosphericModel  : 1;
    VOS_UINT32                          bitOpUtcModel          : 1;
    VOS_UINT32                          bitOpAlmanac           : 1;
    VOS_UINT32                          bitOpGpsRtIntegrity    : 1;
    VOS_UINT32                          bitSpare               : 23;

    CPOS_STATUS_HEALTH_ENUM_UINT8       enStatusHealth;                         /* ELEMENT status_health */
    VOS_UINT8                           aucReserved1[1];
    VOS_UINT16                          usBtsClockDrift;                        /* ELEMENT BTS_clock_drift, BTS时钟偏移 */
    CPOS_REF_TIME_ARRAY_STRU            stRefTimeArray;                         /* ELEMENT ref_time* */
    CPOS_LOCATION_PARAMS_STRU           stLocationParams;                       /* ELEMENT location_parameters */
    CPOS_DGPS_CORRECTIONS_STRU          stDgpsCorrections;                      /* ELEMENT DGPS_corrections */
    CPOS_NAV_MODEL_ELEM_ARRAY_STRU      stNavModelElemArray;                    /* ELEMENT nav_model_elem* */
    CPOS_IONOSPHERIC_MODEL_STRU         stIonosphericModel;                     /* ELEMENT ionospheric_model */
    CPOS_UTC_MODEL_STRU                 stUtcModel;                             /* ELEMENT UTC_model */
    CPOS_ALMANAC_STRU                   stAlmanac;                              /* ELEMENT almanac */
    CPOS_ACQU_ASSIST_ARRAY_STRU         stAcquAssistArray;                      /* ELEMENT acqu_assist* */
    CPOS_BAD_SATELLITE_SET_STRU         stGpsRtIntegrity;                       /* ELEMENT GPS_rt_integrity* */
}CPOS_GPS_ASSIST_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_ref_time (GNSS_day,GNSS_TOD_s,notification_leap_sec)>
        <!ATTLIST GNSS_ref_time gnss_time_id (GPS|Glonass|QZSS|BDS) #REQUIRED>
        <!ELEMENT GNSS_day (#PCDATA)>
        <!ELEMENT GNSS_TOD_s (#PCDATA)>  <!-- seconds -->
        <!-- 0..86399 for RRLP; 0.000 to 86399.999 for LPP with ms -->
        <!-- TOD Uncertainty presented in TimeUnc -->
        <!ELEMENT notification_leap_sec (#PCDATA) >  <!-- hex LPP only -->
    */
    VOS_UINT8                           bitOpGnssTodSDecimal    : 1;
    VOS_UINT8                           bitSpare                : 7;

    CPOS_GNSS_TIME_ID_ENUM_UINT8        enGnssTimeId;                           /* ATTRIBUTE gnss_time_id */
    VOS_UINT8                           aucReserved1[2];
    VOS_UINT32                          ulGnssDay;                              /* ELEMENT GNSS_day */
    VOS_UINT32                          ulGnssTodS;                             /* ELEMENT GNSS_TOD_s */
    VOS_UINT16                          usGnssTodSDecimal;                      /* ELEMENT GNSS_TOD_s Decimal (LPP) */
    VOS_UINT8                           ucGnssNotifLeapSec;                     /* ELEMENT notification_leap_sec */
    VOS_UINT8                           aucReserved2[1];
}CPOS_GNSS_REF_TIME_STRU;


typedef struct
{
    /*
    <!ELEMENT gnss_ion_flags EMPTY>
        <!ATTLIST gnss_ion_flags
            storm_flag1 (0|1) "0"
            storm_flag2 (0|1) "0"
            storm_flag3 (0|1) "0"
            storm_flag4 (0|1) "0"
            storm_flag5 (0|1) "0">
    */
    VOS_UINT32                          bitStormFlag1       : 1;                /* ATTRIBUTE storm_flag1 */
    VOS_UINT32                          bitStormFlag2       : 1;                /* ATTRIBUTE storm_flag2 */
    VOS_UINT32                          bitStormFlag3       : 1;                /* ATTRIBUTE storm_flag3 */
    VOS_UINT32                          bitStormFlag4       : 1;                /* ATTRIBUTE storm_flag4 */
    VOS_UINT32                          bitStormFlag5       : 1;                /* ATTRIBUTE storm_flag5 */
    VOS_UINT32                          bitSpare            : 27;
}CPOS_GNSS_ION_FLAGS_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_ionospheric_model (gnss_ion_ai0,gnss_ion_ai1,gnss_ion_ai2,gnss_ion_flags?)>
        <!ELEMENT gnss_ion_ai0 (#PCDATA)>  <!-- range 0..4095 -->
        <!ELEMENT gnss_ion_ai1 (#PCDATA)>  <!-- range 0..4095 -->
        <!ELEMENT gnss_ion_ai2 (#PCDATA)>  <!-- range 0..4095 -->
    */
    VOS_UINT32                          bitOpGnssIonFlags   : 1;
    VOS_UINT32                          bitSpare            : 31;

    VOS_UINT16                          usGnssIonAi0;                           /* ELEMENT gnss_ion_ai0, INTEGER (0..4095) */
    VOS_UINT16                          usGnssIonAi1;                           /* ELEMENT gnss_ion_ai1, INTEGER (0..4095) */
    VOS_UINT16                          usGnssIonAi2;                           /* ELEMENT gnss_ion_ai2, INTEGER (0..4095) */
    VOS_UINT8                           aucReserved1[2];

    CPOS_GNSS_ION_FLAGS_STRU            stGnssIonFlags;                         /* ELEMENT gnss_ion_flags */
}CPOS_GNSS_IONOSPHERIC_MODEL_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_earth_orient_param (gnss_eop_teop,gnss_eop_pmX,gnss_eop_pmX_d,
        gnss_eop_pmY,gnss_eop_pmY_d,gnss_eop_deltaUT1,gnss_eop_deltaUT1_d)>
        <!ELEMENT gnss_eop_teop (#PCDATA)>  <!-- range 0..65535 -->
        <!ELEMENT gnss_eop_pmX (#PCDATA)>  <!-- range -1048576..1048575 -->
        <!ELEMENT gnss_eop_pmX_d (#PCDATA)>  <!-- range -16384..16383 -->
        <!ELEMENT gnss_eop_pmY (#PCDATA)>  <!-- range -1048576..1048575 -->
        <!ELEMENT gnss_eop_pmY_d (#PCDATA)>  <!-- range -16384..16383 -->
        <!ELEMENT gnss_eop_deltaUT1 (#PCDATA)> <!-- range -1073741824..1073741823 -->
        <!ELEMENT gnss_eop_deltaUT1_d (#PCDATA)>  <!-- range -262144..262143 -->
    */
    VOS_UINT16                          usGnssEopTeop;                          /* ELEMENT gnss_eop_teop, INTEGER (0..65535) */
    VOS_UINT8                           aucReserved1[2];
    VOS_INT32                           lGnssEopPmx;                            /* ELEMENT gnss_eop_pmX, INTEGER (-1048576..1048575) */
    VOS_INT32                           lGnssEopPmy;                            /* ELEMENT gnss_eop_pmY, INTEGER (-1048576..1048575) */
    VOS_INT16                           sGnssEopPmxD;                           /* ELEMENT gnss_eop_pmX_d, INTEGER (-16384..16383) */
    VOS_INT16                           sGnssEopPmyD;                           /* ELEMENT gnss_eop_pmY_d, INTEGER (-16384..16383) */
    VOS_INT32                           lGnssEopDeltaut1;                       /* ELEMENT gnss_eop_deltaUT1, INTEGER (-1073741824..1073741823) */
    VOS_INT32                           lGnssEopDeltaut1D;                      /* ELEMENT gnss_eop_deltaUT1_d, INTEGER (-262144..262143) */
}CPOS_GNSS_EARTH_ORIENT_PARAM_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_additional_ion_model (ionospheric_model)>
        <!ATTLIST GNSS_additional_ion_model
            ion_model_data_id (00|11|01) "00">  <!-- from rrlp 11=QZSS 00=other 01=BDS -->
    */
    CPOS_ION_MODEL_DATA_ID_ENUM_UINT8   enIonModelDataId;                       /* ATTRIBUTE ion_model_data_id */
    VOS_UINT8                           aucReserved1[3];
    CPOS_IONOSPHERIC_MODEL_STRU         stIonosphericModel;                     /* ELEMENT ionospheric_model */
}CPOS_GNSS_ADD_ION_MODEL_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_time_model (tme_ref_time,tme_A0,tme_A1?,tme_A2?,tme_week?)>
        <!ATTLIST GNSS_time_model gnss_time_id (GPS|Glonass|QZSS|BDS) #REQUIRED>
        <!ELEMENT tme_ref_time (#PCDATA)>  <!-- sec scale 2**4 -->
        <!ELEMENT tme_A0 (#PCDATA)>  <!-- sec scale 2**-35 -->
        <!ELEMENT tme_A1 (#PCDATA)>  <!-- sec/sec scale 2**-51 -->
        <!ELEMENT tme_A2 (#PCDATA)>  <!-- sec/sec sec scale 2**-68 -->
        <!ELEMENT tme_week (#PCDATA)>  <!-- 0..8191 -->
    */
    VOS_UINT32                          bitOpTmeA1          : 1;
    VOS_UINT32                          bitOpTmeA2          : 1;
    VOS_UINT32                          bitOpTmeWeek        : 1;
    VOS_UINT32                          bitSpare            : 29;

    CPOS_GNSS_TIME_ID_ENUM_UINT8        enGnssTimeId;                           /* ATTRIBUTE gnss_time_id */
    VOS_UINT8                           aucReserved1[3];
    VOS_UINT32                          ulTmeRefTime;                           /* ELEMENT tme_ref_time */
    VOS_INT32                           lTmeA0;                                 /* ELEMENT tme_A0 */
    VOS_INT16                           sTmeA1;                                 /* ELEMENT tme_A1 */
    VOS_INT8                            cTmeA2;                                 /* ELEMENT tme_A2 */
    VOS_UINT8                           aucReserved2[1];
    VOS_UINT16                          usTmeWeek;                              /* ELEMENT tme_week */
    VOS_UINT8                           aucReserved3[2];
}CPOS_GNSS_TIME_MODEL_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_clock_nav (nav_Toc,nav_af2,nav_af1,nav_af0,nav_Tgd)>
        <!ELEMENT nav_Toc (#PCDATA)>  <!--  0..37799 -->
        <!ELEMENT nav_af2 (#PCDATA)>  <!-- -128..127 -->
        <!ELEMENT nav_af1 (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_af0 (#PCDATA)>  <!-- -2097152..2097151 -->
        <!ELEMENT nav_Tgd (#PCDATA)>  <!-- -128..127  -->
    */

    VOS_UINT16                          usNavToc;                               /* ELEMENT nav_Toc */
    VOS_INT8                            cNavAf2;                                /* ELEMENT nav_af2 */
    VOS_INT8                            cNavTgd;                                /* ELEMENT nav_Tgd */
    VOS_INT16                           sNavAf1;                                /* ELEMENT nav_af1 */
    VOS_UINT8                           aucReserved1[2];
    VOS_INT32                           lNavAf0;                                /* ELEMENT nav_af0 */
}CPOS_NMS_CLOCK_NAV_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_orbit_nav (nav_URA,nav_FitFlag,nav_Toe,nav_Omega,
        nav_DeltaN,nav_M0,nav_OmegaA_d,nav_E,nav_I_d,nav_APowerHalf,
        nav_I0,nav_OmegaA0,nav_Crs,nav_Cis,nav_Cus,nav_Crc,nav_Cic,
        nav_Cuc,(nav_CodeOnL2,nav_L2Pflag,nav_sf1_1,nav_sf1_2,
        nav_sf1_3,nav_sf1_4,nav_AODA)?)>
        <!ELEMENT nav_URA (#PCDATA)>  <!--  0..15 -->
        <!ELEMENT nav_FitFlag (#PCDATA)>  <!--  0..1 -->
        <!ELEMENT nav_Toe (#PCDATA)>  <!--  0..37799 -->
        <!ELEMENT nav_Omega (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT nav_DeltaN (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_M0 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT nav_OmegaA_d (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT nav_E (#PCDATA)>  <!--  0..4294967295 -->
        <!ELEMENT nav_I_d (#PCDATA)>  <!-- -8192..8191 -->
        <!ELEMENT nav_APowerHalf (#PCDATA)>  <!--  0..4294967295 -->
        <!ELEMENT nav_I0 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT nav_OmegaA0 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT nav_Crs (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_Cis (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_Cus (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_Crc (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_Cic (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_Cuc (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT nav_CodeOnL2 (#PCDATA)>  <!--  0..3 -->
        <!ELEMENT nav_L2Pflag (#PCDATA)>  <!--  0..1 -->
        <!ELEMENT nav_sf1_1 (#PCDATA)>  <!--  0..8388607 -->
        <!ELEMENT nav_sf1_2 (#PCDATA)>  <!--  0..16777215 -->
        <!ELEMENT nav_sf1_3 (#PCDATA)>  <!--  0..16777215 -->
        <!ELEMENT nav_sf1_4 (#PCDATA)>  <!--  0..65535 -->
        <!ELEMENT nav_AODA (#PCDATA)>  <!--  0..31 -->
    */
    VOS_UINT32                          bitOpSubElemSet     : 1;
    VOS_UINT32                          bitSpare            : 31;

    VOS_UINT8                           ucNavUra;                               /* ELEMENT nav_URA */
    VOS_UINT8                           ucNavFitFlag;                           /* ELEMENT nav_FitFlag */
    VOS_UINT16                          usNavToe;                               /* ELEMENT nav_Toe */
    VOS_INT32                           lNavOmega;                              /* ELEMENT nav_Omega */
    VOS_INT16                           sNavDeltaN;                             /* ELEMENT nav_DeltaN */
    VOS_UINT8                           aucReserved1[2];
    VOS_INT32                           lNavM0;                                 /* ELEMENT nav_M0 */
    VOS_INT32                           lNavOmegaAD;                            /* ELEMENT nav_OmegaA_d */
    VOS_UINT32                          ulNavE;                                 /* ELEMENT nav_E */
    VOS_INT16                           sNavID;                                 /* ELEMENT nav_I_d */
    VOS_UINT8                           aucReserved2[2];
    VOS_UINT32                          ulNavAPowerHalf;                        /* ELEMENT nav_APowerHalf */
    VOS_INT32                           lNavI0;                                 /* ELEMENT nav_I0 */
    VOS_INT32                           lNavOmegaA0;                            /* ELEMENT nav_OmegaA0 */
    VOS_INT16                           sNavCrs;                                /* ELEMENT nav_Crs */
    VOS_INT16                           sNavCis;                                /* ELEMENT nav_Cis */
    VOS_INT16                           sNavCus;                                /* ELEMENT nav_Cus */
    VOS_INT16                           sNavCrc;                                /* ELEMENT nav_Crc */
    VOS_INT16                           sNavCic;                                /* ELEMENT nav_Cic */
    VOS_INT16                           sNavCuc;                                /* ELEMENT nav_Cuc */

    VOS_UINT8                           ucNavCodeOnL2;                          /* ELEMENT nav_CodeOnL2 */
    VOS_UINT8                           ucNavL2Pflag;                           /* ELEMENT nav_L2Pflag */
    VOS_UINT8                           ucNavAoda;                              /* ELEMENT nav_AODA */
    VOS_UINT8                           aucReserved3[1];
    VOS_UINT32                          ulNavSf11;                              /* ELEMENT nav_sf1_1 */
    VOS_UINT32                          ulNavSf12;                              /* ELEMENT nav_sf1_2 */
    VOS_UINT32                          ulNavSf13;                              /* ELEMENT nav_sf1_3 */
    VOS_UINT16                          usNavSf14;                              /* ELEMENT nav_sf1_4 */
    VOS_UINT8                           aucReserved4[2];
}CPOS_NMS_ORBIT_NAV_STRU;


typedef struct
{
    CPOS_NMS_CLOCK_NAV_STRU             stNmsClockNav;
    CPOS_NMS_ORBIT_NAV_STRU             stNmsOrbitNav;
}CPOS_NMS_CLK_ORBIT_NAV_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_clock_cnav (cnav_Toc,cnav_Top,cnav_URA0,cnav_URA1,cnav_URA2,
        cnav_Af2,cnav_Af1,cnav_Af0,cnav_Tgd,cnav_ISCl1cp?,cnav_ISCl1cd?,
        cnav_ISCl1ca?,cnav_ISCl2c?,cnav_ISCl5i5?,cnav_ISCl5q5?)>
        <!ELEMENT cnav_Toc (#PCDATA)>  <!-- 0..2015 -->
        <!ELEMENT cnav_Top (#PCDATA)>  <!-- 0..2015 -->
        <!ELEMENT cnav_URA0 (#PCDATA)>  <!-- -16..15 -->
        <!ELEMENT cnav_URA1 (#PCDATA)>  <!-- 0..7 -->
        <!ELEMENT cnav_URA2 (#PCDATA)>  <!-- 0..7 -->
        <!ELEMENT cnav_Af2 (#PCDATA)>  <!-- -512..511 -->
        <!ELEMENT cnav_Af1 (#PCDATA)>  <!-- -524288..524287 -->
        <!ELEMENT cnav_Af0 (#PCDATA)>  <!-- -33554432..33554431 -->
        <!ELEMENT cnav_Tgd (#PCDATA)>  <!-- -4096..4095 -->
        <!ELEMENT cnav_ISCl1cp (#PCDATA)>  <!-- -4096..4095 -->
        <!ELEMENT cnav_ISCl1cd (#PCDATA)>  <!-- -4096..4095 -->
        <!ELEMENT cnav_ISCl1ca (#PCDATA)>  <!-- -4096..4095 -->
        <!ELEMENT cnav_ISCl2c (#PCDATA)>  <!-- -4096..4095 -->
        <!ELEMENT cnav_ISCl5i5 (#PCDATA)>  <!-- -4096..4095 -->
        <!ELEMENT cnav_ISCl5q5 (#PCDATA)>  <!-- -4096..4095 -->
    */
    VOS_UINT32                          bitOpCnavIscl1cp     : 1;
    VOS_UINT32                          bitOpCnavIscl1cd     : 1;
    VOS_UINT32                          bitOpCnavIscl1ca     : 1;
    VOS_UINT32                          bitOpCnavIscl2c      : 1;
    VOS_UINT32                          bitOpCnavIscl5i5     : 1;
    VOS_UINT32                          bitOpCnavIscl5q5     : 1;
    VOS_UINT32                          bitOpSpare           : 26;

    VOS_UINT16                          usCnavToc;                              /* ELEMENT cnav_Toc */
    VOS_UINT16                          usCnavTop;                              /* ELEMENT cnav_Top */
    VOS_INT8                            cCnavUra0;                              /* ELEMENT cnav_URA0 */
    VOS_UINT8                           ucCnavUra1;                             /* ELEMENT cnav_URA1 */
    VOS_UINT8                           ucCnavUra2;                             /* ELEMENT cnav_URA2 */
    VOS_UINT8                           ucReserved;
    VOS_INT16                           sCnavTgd;                               /* ELEMENT cnav_Tgd */
    VOS_INT16                           sCnavAf2;                               /* ELEMENT cnav_Af2 */
    VOS_INT32                           lCnavAf1;                               /* ELEMENT cnav_Af1 */
    VOS_INT32                           lCnavAf0;                               /* ELEMENT cnav_Af0 */

    VOS_INT16                           sCnavIscl1cp;                           /* ELEMENT cnav_ISCl1cp */
    VOS_INT16                           sCnavIscl1cd;                           /* ELEMENT cnav_ISCl1cd */
    VOS_INT16                           sCnavIscl1ca;                           /* ELEMENT cnav_ISCl1ca */
    VOS_INT16                           sCnavIscl2c;                            /* ELEMENT cnav_ISCl2c */
    VOS_INT16                           sCnavIscl5i5;                           /* ELEMENT cnav_ISCl5i5 */
    VOS_INT16                           sCnavIscl5q5;                           /* ELEMENT cnav_ISCl5q5 */
}CPOS_NMS_CLOCK_CNAV_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_orbit_cnav (cnav_Top,cnav_URAindex,cnav_DeltaA,cnav_Adot,
        cnav_DeltaNo,cnav_DeltaNoDot,cnav_Mo,cnav_E,cnav_Omega,cnav_OMEGA0,
        cnav_DeltaOmegaDot,cnav_Io,cnav_IoDot,cnav_Cis,cnav_Cic,cnav_Crs,
        cnav_Crc,cnav_Cus,cnav_Cuc)>
        <!ELEMENT cnav_URAindex (#PCDATA)>  <!-- -16..15 -->
        <!ELEMENT cnav_DeltaA (#PCDATA)>  <!-- -33554432..33554431 -->
        <!ELEMENT cnav_Adot (#PCDATA)>  <!-- -16777216..16777215 -->
        <!ELEMENT cnav_DeltaNo (#PCDATA)>  <!-- -65536..65535 -->
        <!ELEMENT cnav_DeltaNoDot (#PCDATA)>  <!-- -4194304..4194303 -->
        <!ELEMENT cnav_Mo (#PCDATA)>  <!-- -4294967296..4294967295 -->
        <!ELEMENT cnav_E (#PCDATA)>  <!-- 0..8589934591 -->
        <!ELEMENT cnav_Omega (#PCDATA)>  <!-- -4294967296..4294967295 -->
        <!ELEMENT cnav_OMEGA0 (#PCDATA)>  <!-- -4294967296..4294967295 -->
        <!ELEMENT cnav_DeltaOmegaDot (#PCDATA)>  <!-- -65536..65535 -->
        <!ELEMENT cnav_Io (#PCDATA)>  <!-- -4294967296..4294967295 -->
        <!ELEMENT cnav_IoDot (#PCDATA)>  <!-- -16384..16383 -->
        <!ELEMENT cnav_Cis (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT cnav_Cic (#PCDATA)>  <!-- -32768..32767 -->
        <!ELEMENT cnav_Crs (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT cnav_Crc (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT cnav_Cus (#PCDATA)>  <!-- -1048576..1048575 -->
        <!ELEMENT cnav_Cuc (#PCDATA)>  <!-- -1048576..1048575 -->
    */
    VOS_UINT16                          usCnavTop;                              /* ELEMENT cnav_Top */
    VOS_UINT8                           aucReserved1[1];
    VOS_INT8                            cCnavUraIndex;                          /* ELEMENT cnav_URAindex */
    VOS_INT32                           lCnavDeltaA;                            /* ELEMENT cnav_DeltaA */
    VOS_INT32                           lCnavAdot;                              /* ELEMENT cnav_Adot */
    VOS_INT32                           lCnavDeltaNo;                           /* ELEMENT cnav_DeltaNo */
    VOS_INT32                           lCnavDeltaNoDot;                        /* ELEMENT cnav_DeltaNoDot */

    VOS_UINT8                           aucCnavMo[8];                           /* ELEMENT cnav_Mo */
    VOS_UINT8                           aucCnavE[8];                            /* ELEMENT cnav_E */
    VOS_UINT8                           aucCnavOmega[8];                        /* ELEMENT cnav_Omega */
    VOS_UINT8                           aucCnavOmega0[8];                       /* ELEMENT cnav_OMEGA0 */
    VOS_UINT8                           aucCnavIo[8];                           /* ELEMENT cnav_Io */

    VOS_INT32                           lCnavDeltaOmegaDot;                     /* ELEMENT cnav_DeltaOmegaDot */
    VOS_INT16                           sCnavIoDot;                             /* ELEMENT cnav_IoDot */
    VOS_UINT8                           aucReserved2[2];

    VOS_INT16                           sCnavCis;                               /* ELEMENT cnav_Cis */
    VOS_INT16                           sCnavCic;                               /* ELEMENT cnav_Cic */
    VOS_INT32                           lCnavCrs;                               /* ELEMENT cnav_Crs */
    VOS_INT32                           lCnavCrc;                               /* ELEMENT cnav_Crc */
    VOS_INT32                           lCnavCus;                               /* ELEMENT cnav_Cus */
    VOS_INT32                           lCnavCuc;                               /* ELEMENT cnav_Cuc */

}CPOS_NMS_ORBIT_CNAV_STRU;


typedef struct
{
    CPOS_NMS_CLOCK_CNAV_STRU            stNmsClockCnav;
    CPOS_NMS_ORBIT_CNAV_STRU            stNmsOrbitCnav;
}CPOS_NMS_CLK_ORBIT_CNAV_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_clock_glonass (glo_Tau,glo_Gamma,glo_DeltaTau)>
        <!ELEMENT glo_Tau (#PCDATA)>  <!-- -2097152..2097151 -->
        <!ELEMENT glo_Gamma (#PCDATA)>  <!-- -1024..1023  -->
        <!ELEMENT glo_DeltaTau (#PCDATA)>  <!-- -16..15 -->
    */
    VOS_INT32                           lGloTau;                                /* ELEMENT glo_Tau */
    VOS_INT16                           sGloGamma;                              /* ELEMENT glo_Gamma */
    VOS_INT8                            cGloDeltaTau;                           /* ELEMENT glo_DeltaTau */
    VOS_UINT8                           aucReserved1[1];
}CPOS_NMS_CLOCK_GLNSS_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_orbit_glonass (glo_En,glo_P1,glo_P2,glo_M,glo_X,glo_X_d,
        glo_X_dd,glo_Y,glo_Y_d,glo_Y_dd,glo_Z,glo_Z_d,glo_Z_dd)>
        <!ELEMENT glo_En (#PCDATA)>  <!--  0..31 -->
        <!ELEMENT glo_P1 (#PCDATA)>  <!-- hex -->
        <!ELEMENT glo_P2 (#PCDATA)>  <!--  0..1 -->
        <!ELEMENT glo_M (#PCDATA)>  <!--  0..3 -->
        <!ELEMENT glo_X (#PCDATA)>  <!-- -67108864..67108863 -->
        <!ELEMENT glo_X_d (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT glo_X_dd (#PCDATA)>  <!-- -16..15 -->
        <!ELEMENT glo_Y (#PCDATA)>  <!-- -67108864..67108863 -->
        <!ELEMENT glo_Y_d (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT glo_Y_dd (#PCDATA)>  <!-- -16..15 -->
        <!ELEMENT glo_Z (#PCDATA)>  <!-- -67108864..67108863 -->
        <!ELEMENT glo_Z_d (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT glo_Z_dd (#PCDATA)>  <!-- -16..15 -->
    */
    VOS_UINT8                           ucGloEn;                                /* ELEMENT glo_En */
    VOS_UINT8                           ucGloP1;                                /* ELEMENT glo_P1 */
    VOS_UINT8                           ucGloP2;                                /* ELEMENT glo_P2 */
    VOS_UINT8                           ucGloM;                                 /* ELEMENT glo_M */

    VOS_INT32                           lGloX;                                  /* ELEMENT glo_X */
    VOS_INT32                           lGloXD;                                 /* ELEMENT glo_X_d */
    VOS_INT32                           lGloY;                                  /* ELEMENT glo_Y */
    VOS_INT32                           lGloYD;                                 /* ELEMENT glo_Y_d */
    VOS_INT32                           lGloZ;                                  /* ELEMENT glo_Z */
    VOS_INT32                           lGloZD;                                 /* ELEMENT glo_Z_d */

    VOS_INT8                            cGloXDd;                                /* ELEMENT glo_X_dd */
    VOS_INT8                            cGloYDd;                                /* ELEMENT glo_Y_dd */
    VOS_INT8                            cGloZDd;                                /* ELEMENT glo_Z_dd */
    VOS_UINT8                           aucReserved1[1];
}CPOS_NMS_ORBIT_GLNSS_STRU;


typedef struct
{
    CPOS_NMS_CLOCK_GLNSS_STRU           stNmsClockGlnss;
    CPOS_NMS_ORBIT_GLNSS_STRU           stNmsOrbitGlnss;
}CPOS_NMS_CLK_ORBIT_GLNSS_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_clock_sbas (sbas_To,sbas_Agfo,sbas_Agf1)>  <!-- model 4 -->
        <!ELEMENT sbas_To (#PCDATA)>
        <!ELEMENT sbas_Agfo (#PCDATA)>
        <!ELEMENT sbas_Agf1 (#PCDATA)>
    */
    VOS_UINT16                          usSbasTo;                               /* ELEMENT sbas_To */
    VOS_INT16                           sSbasAgfo;                              /* ELEMENT sbas_Agfo */
    VOS_INT16                           sSbasAgf1;                              /* ELEMENT sbas_Agf1 */
    VOS_UINT8                           aucReserved1[2];
}CPOS_NMS_CLOCK_SBAS_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_orbit_sbas (sbas_To,sbas_accuracy,sbas_Xg,sbas_Yg,sbas_Zg,
        sbas_Xg_d,sbas_Yg_d,sbas_Zg_d,sbas_Xg_dd,sbas_Yg_dd,sbas_Zg_dd)>  <!-- model 4-->
        <!ELEMENT sbas_accuracy (#PCDATA)>  <!-- hex -->
        <!ELEMENT sbas_Xg (#PCDATA)>
        <!ELEMENT sbas_Yg (#PCDATA)>
        <!ELEMENT sbas_Zg (#PCDATA)>
        <!ELEMENT sbas_Xg_d (#PCDATA)>
        <!ELEMENT sbas_Yg_d (#PCDATA)>
        <!ELEMENT sbas_Zg_d (#PCDATA)>
        <!ELEMENT sbas_Xg_dd (#PCDATA)>
        <!ELEMENT sbas_Yg_dd (#PCDATA)>
        <!ELEMENT sbas_Zg_dd (#PCDATA)>
    */
    VOS_UINT16                          usSbasTo;                               /* ELEMENT sbas_To */
    VOS_UINT16                          usSbasAccuracy;                         /* ELEMENT sbas_accuracy */
    VOS_INT32                           lSbasXg;                                /* ELEMENT sbas_Xg */
    VOS_INT32                           lSbasYg;                                /* ELEMENT sbas_Yg */
    VOS_INT32                           lSbasZg;                                /* ELEMENT sbas_Zg */
    VOS_INT32                           lSbasXgDot;                             /* ELEMENT sbas_Xg_d */
    VOS_INT32                           lSbasYgDot;                             /* ELEMENT sbas_Yg_d */
    VOS_INT32                           lSbasZgDot;                             /* ELEMENT sbas_Zg_d */
    VOS_INT16                           sSbasXgDotDot;                          /* ELEMENT sbas_Xg_dd */
    VOS_INT16                           sSbasYgDotDot;                          /* ELEMENT sbas_Yg_dd */
    VOS_INT16                           sSbasZgDotDot;                          /* ELEMENT sbas_Zg_dd */
    VOS_UINT8                           aucReserved1[2];
}CPOS_NMS_ORBIT_SBAS_STRU;


typedef struct
{
    CPOS_NMS_CLOCK_SBAS_STRU            stNmsClockSbas;
    CPOS_NMS_ORBIT_SBAS_STRU            stNmsOrbitSbas;
}CPOS_NMS_CLK_ORBIT_SBAS_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_clock_bds (bdsToc-r12,bdsA0-r12,bdsA1-r12,bdsA2-r12,bdsTgd1-r12)>
        <!ELEMENT bdsToc-r12 (#PCDATA)>  <!-- 0..131071 -->
        <!ELEMENT bdsA0-r12 (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT bdsA1-r12 (#PCDATA)>  <!-- -2097152..2097151 -->
        <!ELEMENT bdsA2-r12 (#PCDATA)>  <!-- -1024..1023 -->
        <!ELEMENT bdsTgd1-r12 (#PCDATA)>  <!-- -512..511-->
    */
    VOS_UINT32                          ulBdsTocR12;                            /* ELEMENT bdsToc-r12 */
    VOS_INT32                           lBdsA0R12;                              /* ELEMENT bdsA0-r12 */
    VOS_INT32                           lBdsA1R12;                              /* ELEMENT bdsA1-r12 */
    VOS_INT16                           sBdsA2R12;                              /* ELEMENT bdsA2-r12 */
    VOS_INT16                           sBdsTgd1R12;                            /* ELEMENT bdsTgd1-r12-r12 */
}CPOS_NMS_CLOCK_BDS_STRU;


typedef struct
{
    /*
    <!ELEMENT nms_orbit_bds(bdsURAI-r12,bdsToe-r12,bdsAPowerHalf-r12,
        bdsE-r12,bdsW-r12,bdsDeltaN-r12,bdsM0-r12,bdsOmega0-r12,
        bdsOmegaDot-r12,bdsI0-r12,bdsIDot-r12,bdsCuc-r12,bdsCus-r12,
        bdsCrc-r12,bdsCrs-r12,bdsCic-r12,bdsCis-r12)>
        <!ELEMENT bdsURAI-r12 (#PCDATA)>  <!-- 0..15 -->
        <!ELEMENT bdsToe-r12 (#PCDATA)>  <!-- 0..131071 -->
        <!ELEMENT bdsAPowerHalf-r12 (#PCDATA)>  <!-- 0..4294967295 -->
        <!ELEMENT bdsE-r12 (#PCDATA)>  <!-- 0..4294967295 -->
        <!ELEMENT bdsW-r12 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT bdsDeltaN-r12 (#PCDATA)>  <!-- (-32768..32767 -->
        <!ELEMENT bdsM0-r12 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT bdsOmega0-r12 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT bdsOmegaDot-r12 (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT bdsI0-r12 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT bdsIDot-r12 (#PCDATA)>  <!-- -8192..8191 -->
        <!ELEMENT bdsCuc-r12 (#PCDATA)>  <!-- -131072..131071 -->
        <!ELEMENT bdsCus-r12 (#PCDATA)>  <!-- -131072..131071 -->
        <!ELEMENT bdsCrc-r12 (#PCDATA)>  <!-- -131072..131071 -->
        <!ELEMENT bdsCrs-r12 (#PCDATA)>  <!-- -131072..131071 -->
        <!ELEMENT bdsCic-r12 (#PCDATA)>  <!-- -131072..131071 -->
        <!ELEMENT bdsCis-r12 (#PCDATA)>  <!-- -131072..131071 -->
    */
    VOS_UINT8                           ucBdsUraiR12;                           /* ELEMENT bdsURAI-r12 */
    VOS_UINT8                           aucReserved1[3];

    VOS_UINT32                          ulBdsToeR12;                            /* ELEMENT bdsToe-r12 */
    VOS_UINT32                          ulBdsAPowerHalfR12;                     /* ELEMENT bdsAPowerHalf-r12 */
    VOS_UINT32                          ulBdsER12;                              /* ELEMENT bdsE-r12 */
    VOS_INT32                           lBdsWR12;                               /* ELEMENT bdsW-r12 */

    VOS_INT16                           sBdsDeltaNR12;                          /* ELEMENT bdsDeltaN-r12 */
    VOS_INT16                           sBdsIDotR12;                            /* ELEMENT bdsIDot-r12 */

    VOS_INT32                           lBdsM0R12;                              /* ELEMENT bdsM0-r12 */
    VOS_INT32                           lBdsOmega0R12;                          /* ELEMENT bdsOmega0-r12 */
    VOS_INT32                           lBdsOmegaDotR12;                        /* ELEMENT bdsOmegaDot-r12 */
    VOS_INT32                           lBdsI0R12;                              /* ELEMENT bdsI0-r12 */

    VOS_INT32                           lBdsCucR12;                             /* ELEMENT bdsCuc-r12 */
    VOS_INT32                           lBdsCusR12;                             /* ELEMENT bdsCus-r12 */
    VOS_INT32                           lBdsCrcR12;                             /* ELEMENT bdsCrc-r12 */
    VOS_INT32                           lBdsCrsR12;                             /* ELEMENT bdsCrs-r12 */
    VOS_INT32                           lBdsCicR12;                             /* ELEMENT bdsCic-r12 */
    VOS_INT32                           lBdsCisR12;                             /* ELEMENT bdsCis-r12 */
}CPOS_NMS_ORBIT_BDS_STRU;


typedef struct
{
    CPOS_NMS_CLOCK_BDS_STRU             stNmsClockBds;
    CPOS_NMS_ORBIT_BDS_STRU             StNmsOrbitBds;
}CPOS_NMS_CLK_ORBIT_BDS_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_satelite (sat_id,nms_health,nms_iod,
        ((nms_clock_nav,nms_orbit_nav)|(nms_clock_cnav,nms_orbit_cnav)|
         (nms_clock_glonass,nms_orbit_glonass)|(nms_clock_sbas,nms_orbit_sbas)|
         (nms_clock_bds,nms_orbit_bds)))>
        <!ELEMENT nms_health (#PCDATA)>  <!-- in hexadecimal -->
        <!ELEMENT nms_iod (#PCDATA)>  <!-- includes iod_msb -->
    */
    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT8                           aucReserved1[1];
    VOS_UINT16                          usNmsHealth;                            /* ELEMENT nms_health */
    VOS_UINT16                          usNmsIod;                               /* ELEMENT nms_iod */
    VOS_UINT8                           aucReserved2[1];

    CPOS_GNSS_SATELITE_ELEM_ENUM_UINT8  enChoice;
    union
    {
        CPOS_NMS_CLK_ORBIT_NAV_STRU     stNmsClkOrbitNav;
        CPOS_NMS_CLK_ORBIT_CNAV_STRU    stNmsClkOrbitCnav;
        CPOS_NMS_CLK_ORBIT_GLNSS_STRU   stNmsClkOrbitGlnss;
        CPOS_NMS_CLK_ORBIT_SBAS_STRU    stNmsClkOrbitSbas;
        CPOS_NMS_CLK_ORBIT_BDS_STRU     stNmsClkOrbitBds;
    } u;
}CPOS_GNSS_SATELITE_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_SATELITE_STRU             astGnssSat[CPOS_GNSS_SAT_MAX_NUM];
}CPOS_GNSS_SATELITE_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_nav_model (GNSS_satelite+)>
        <!ATTLIST GNSS_nav_model non_broadcast_ind_flag (0|1) "0">
    */
    VOS_UINT32                          ulNonBroadcastIndFlag;                  /* ATTRIBUTE non_broadcast_ind_flag */
    CPOS_GNSS_SATELITE_ARRAY_STRU       stGnssSateliteArray;
}CPOS_GNSS_NAV_MODEL_STRU;


typedef struct
{
    /*
    <!ELEMENT bad_signal (sat_id,GNSS_signal?)>
        <!ELEMENT GNSS_signal (#PCDATA)>
    */
    VOS_UINT8                           bitOpGnssSignal     : 1;
    VOS_UINT8                           bitOpSpare          : 7;

    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT8                           ucGnssSignal;                           /* ELEMENT GNSS_signal */
    VOS_UINT8                           ucReserved1;
}CPOS_BAD_SIGNAL_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_BAD_SIGNAL_STRU                astBadSignal[CPOS_GNSS_SAT_MAX_NUM];
}CPOS_BAD_SIGNAL_ARRAY_STRU;


typedef struct
{
    /*
        <!ELEMENT GNSS_databits (#PCDATA)>  <!-- 1..1024 -->
    */
    VOS_UINT32                          ulCnt;
    VOS_UINT8                           aucGnssDataBits[CPOS_GNSS_DATA_BITS_MAX_LEN];
}CPOS_GNSS_DATABITS_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT gnss-DataBitsSgnList (GNSS_databits+)>
        <!ELEMENT GNSS_databits (#PCDATA)>  <!-- 1..1024 -->
    */
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_DATABITS_ARRAY_STRU       astGnssDataBitsArray[CPOS_GNSS_DATA_BITS_SGN_MAX_NUM];
}CPOS_GNSS_DATABITS_SGNLIST_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT gnss-DataBitsSatList(sat_id,gnss-DataBitsSgnList+)>
        <!ATTLIST gnss-DataBitsSatList signal_id (GPS_L1|GPS_L1C|
            GPS_L2C|GPS_L5|SBAS_L1|GLO_G1|GLO_G2|GLO_G3|BDS_B1I) "GPS_L1">
    */
    CPOS_SIGNAL_ID_ENUM_UINT8               enSignalId;                         /* ATTRIBUTE signal_id */
    VOS_UINT8                               ucSatId;                            /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT8                               aucReserved1[2];

    CPOS_GNSS_DATABITS_SGNLIST_ARRAY_STRU   stGnssDataBitsSgnListArray;
}CPOS_GNSS_DATABITS_SAT_LIST_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_DATABITS_SAT_LIST_STRU    astGnssDataBitsSatList[CPOS_GNSS_SAT_MAX_NUM];
}CPOS_GNSS_DATABITS_SAT_LIST_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_databitassistance (gnss-TOD,gnss-TODfrac?,gnss-DataBitsSatList+)>
        <!ELEMENT gnss-TOD (#PCDATA)>  <!-- 0..3599 -->
        <!ELEMENT gnss-TODfrac (#PCDATA)>  <!-- 0..99 -->
    */
    VOS_UINT32                              bitOpGnssTodfrac    : 1;
    VOS_UINT32                              bitOpSpare          : 31;

    VOS_UINT16                              usGnssTod;                          /* ELEMENT gnss-TOD */
    VOS_UINT8                               ucGnssTodfrac;                      /* ELEMENT gnss-TODfrac */
    VOS_UINT8                               aucReserved1[1];

    CPOS_GNSS_DATABITS_SAT_LIST_ARRAY_STRU  stGnssDataBitsSatListArray;         /* ELEMENT gnss-DataBitsSatList+ */
}CPOS_GNSS_DATABIT_ASSIST_STRU;


typedef struct
{
    /*
    <!ELEMENT gnss-AlmanacList(keplerianBDS-Almanac-r12)>
        <!ELEMENT keplerianBDS-Almanac-r12(sat_id,bdsAlmToa-r12?,bdsAlmSqrtA-r12,
            bdsAlmE-r12,bdsAlmW-r12,bdsAlmM0-r12,bdsAlmOmega0-r12,bdsAlmOmegaDot-r12,
            bdsAlmDeltaI-r12,bdsAlmA0-r12,bdsAlmA1-r12,bdsSvHealth-r12?)>
            <!ELEMENT bdsAlmToa-r12 (#PCDATA)>  <!-- 0..256 -->
            <!ELEMENT bdsAlmSqrtA-r12 (#PCDATA)>  <!-- 0..16777215 -->
            <!ELEMENT bdsAlmE-r12 (#PCDATA)>  <!-- 0..131071 -->
            <!ELEMENT bdsAlmW-r12 (#PCDATA)>  <!-- -8388608..8388607 -->
            <!ELEMENT bdsAlmM0-r12 (#PCDATA)>  <!-- -8388608..8388607 -->
            <!ELEMENT bdsAlmOmega0-r12(#PCDATA)>  <!-- -8388608..8388607 -->
            <!ELEMENT bdsAlmOmegaDot-r12 (#PCDATA)>  <!-- -65536..65535 -->
            <!ELEMENT bdsAlmDeltaI-r12 (#PCDATA)>  <!-- -32768..32767 -->
            <!ELEMENT bdsAlmA0-r12 (#PCDATA)>  <!-- -1024..1023 -->
            <!ELEMENT bdsAlmA1-r12 (#PCDATA)>  <!-- -1024..1023 -->
            <!ELEMENT bdsSvHealth-r12(#PCDATA)>  <!-- 0..3599 -->
    */
    VOS_UINT32                          bitOpBdsAlmToaR12   : 1;
    VOS_UINT32                          bitOpBdsSvHealthR12 : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT8                           aucReserved1[1];
    VOS_UINT16                          usBdsAlmToaR12;                         /* ELEMENT bdsAlmToa-r12 */
    VOS_UINT32                          ulBdsAlmSqrtAR12;                       /* ELEMENT bdsAlmSqrtA-r12 */
    VOS_UINT32                          ulBdsAlmER12;                           /* ELEMENT bdsAlmE-r12 */
    VOS_INT32                           lBdsAlmWR12;                            /* ELEMENT bdsAlmW-r12 */
    VOS_INT32                           lBdsAlmM0R12;                           /* ELEMENT bdsAlmM0-r12 */
    VOS_INT32                           lBdsAlmOmega0R12;                       /* ELEMENT bdsAlmOmega0-r12 */
    VOS_INT32                           lBdsAlmOmegaDotR12;                     /* ELEMENT bdsAlmOmegaDot-r12 */
    VOS_INT16                           sBdsAlmDeltaIR12;                       /* ELEMENT bdsAlmDeltaI-r12 */
    VOS_INT16                           sBdsAlmA0R12;                           /* ELEMENT bdsAlmA0-r12 */
    VOS_INT16                           sBdsAlmA1R12;                           /* ELEMENT bdsAlmA1-r12 */
    VOS_UINT16                          usBdsSvHealthR12;                       /* ELEMENT bdsSvHealth-r12 */

}CPOS_KEPLERIAN_BDS_ALMANAC_R12_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_KEPLERIAN_BDS_ALMANAC_R12_STRU astKeplerianBdsAlmanacR12[CPOS_GNSS_ALMANAC_LIST_MAX_LEN];
}CPOS_GNSS_ALMANAC_LIST_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_almanac(week_number?,toa,ioda?,completeAlmanacProvided,gnss-AlmanacList+)>
        <!ELEMENT week_number (#PCDATA)>  <!-- 0..255 -->
        <!ELEMENT toa (#PCDATA)>  <!-- 0..255 -->
        <!ELEMENT ioda(#PCDATA)>  <!-- 0..3 -->
        <!ELEMENT completeAlmanacProvided (#PCDATA)>
    */
    VOS_UINT32                          bitOpWeekNumber     : 1;
    VOS_UINT32                          bitOpIoda           : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           ucWeekNumber;                           /* ELEMENT week_number */
    VOS_UINT8                           ucToa;                                  /* ELEMENT toa */
    VOS_UINT8                           ucIoda;                                 /* ELEMENT ioda */
    VOS_UINT8                           ucCmpleteAlmanacProvided;               /* ELEMENT completeAlmanacProvided */

    CPOS_GNSS_ALMANAC_LIST_ARRAY_STRU   stGnssAlmanacListArray;                 /* ELEMENT gnss-AlmanacList+ */
}CPOS_GNSS_ALMANAC_STRU;


typedef struct
{
    /*
    <!ELEMENT utcModel5-r12 (utcA0-r12,utcA1-r12,utcDeltaTls-r12,
        utcWNlsf-r12,utcDN-r12,utcDeltaTlsf-r12)>
        <!ELEMENT utcA0-r12 (#PCDATA)>  <!-- -2147483648..2147483647 -->
        <!ELEMENT utcA1-r12 (#PCDATA)>  <!-- -8388608..8388607 -->
        <!ELEMENT utcDeltaTls-r12 (#PCDATA)>  <!-- -128..127 -->
        <!ELEMENT utcWNlsf-r12 (#PCDATA)>  <!-- 0..255 -->
        <!ELEMENT utcDN-r12 (#PCDATA)>  <!-- 0..255 -->
        <!ELEMENT utcDeltaTlsf-r12 (#PCDATA)>  <!-- -128..127 -->
    */
    VOS_INT32                           lUtcA0R12;                              /* ELEMENT utcA0-r12 */
    VOS_INT32                           lUtcA1R12;                              /* ELEMENT utcA1-r12 */
    VOS_INT8                            cUtcDeltaTlsR12;                        /* ELEMENT utcDeltaTls-r12 */
    VOS_UINT8                           ucUtcWnlsfR12;                          /* ELEMENT utcWNlsf-r12 */
    VOS_UINT8                           ucUtcDnR12;                             /* ELEMENT utcDN-r12 */
    VOS_INT8                            cUtcDeltaTlsfR12;                       /* ELEMENT utcDeltaTlsf-r12 */
}CPOS_UTCMODEL5_R12_STRU;


typedef struct
{
    /*
    <!ELEMENT gridIonList-r12 (igp-ID-r12,dt-r12,givei-r12)>
        <!ELEMENT igp-ID-r12 (#PCDATA)>  <!-- 1..320 -->
        <!ELEMENT dt-r12 (#PCDATA)>  <!-- 0..511 -->
        <!ELEMENT givei-r12 (#PCDATA)>  <!-- 0..15 -->
    */
    VOS_UINT16                          usIgpIdR12;                             /* ELEMENT igp-ID-r12 */
    VOS_UINT16                          usDtR12;                                /* ELEMENT dt-r12 */
    VOS_UINT8                           ucGiveiR12;                             /* ELEMENT givei-r12 */
    VOS_UINT8                           aucReserved1[3];
}CPOS_GRID_IONLIST_R12_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GRID_IONLIST_R12_STRU          astGridIonListR12[CPOS_GRID_IONLIST_R12_MAX_NUM];
}CPOS_GRID_IONLIST_R12_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT BDS-GridModel-r12 (bds-RefTime-r12,gridIonList-r12+)>
        <!ELEMENT bds-RefTime-r12 (#PCDATA)>  <!-- 0..3599 -->
    */
    VOS_UINT16                          usBdsRefTimeR12;                        /* ELEMENT bds-RefTime-r12 */
    VOS_UINT8                           aucReserved1[2];

    CPOS_GRID_IONLIST_R12_ARRAY_STRU    stGridIonListR12Array;
}CPOS_BDS_GRIDMODEL_R12_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_id_glonass_sat_element
        (sat_id,GNSS_signal_id,channel_number?)>  <!-- sat_id 0..63 -->
        <!ELEMENT GNSS_signal_id (#PCDATA)>
        <!-- Integer corresponds to bit string 1-8 where bit at position
        if set, means particular signal is addressed; a zero value at the
        particular bit position means the signal is not addressed -->
        <!ELEMENT channel_number (#PCDATA)>  <!-- -7..13 -->
    */
    VOS_UINT32                          bitOpChannelNumber  : 1;
    VOS_UINT32                          bitSpare            : 31;

    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id, INTEGER(0..63) */
    VOS_UINT8                           ucGnssSignalId;                         /* ELEMENT GNSS_signal_id */
    VOS_INT8                            cChannelNumber;                         /* ELEMENT channel_number */
    VOS_UINT8                           aucReserved1[1];
}CPOS_GNSS_ID_GLONASS_SAT_ELEM_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_ID_GLONASS_SAT_ELEM_STRU  astGnssIdGlonassSatElem[CPOS_GNSS_SAT_MAX_NUM];
}CPOS_GNSS_ID_GLONASS_SAT_ELEM_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_id_glonass (GNSS_id_glonass_sat_element+)>
    */
    CPOS_GNSS_ID_GLONASS_SAT_ELEM_ARRAY_STRU    stGnssIdGlonassSatElemArray;
}CPOS_GNSS_ID_GLONASS_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_assist (GNSS_ref_time?,GNSS_ref_location?,
        GNSS_ref_measurment_assist?,GNSS_ionospheric_model?,
        GNSS_earth_orient_param?,GNSS_additional_ion_model?,
        GNSS_time_model,GNSS_nav_model?,GNSS_integrity?,acqu_assist*,
        GNSS_databitassistance?,GNSS_almanac?,GNSS_UTC_model?,
        BDS-GridModel-r12?,GNSS_auxiliary_info?)>
        <!ATTLIST GNSS_assist
            gnss_id (SBAS|MGPS|QZSS|Glonass|GPS|BDS) #IMPLIED
            sbas_id (WASS|EGNOS|MSAS|GAGAN) #IMPLIED>
        <!ELEMENT GNSS_ref_location (location_parameters)>
        <!ELEMENT GNSS_ref_measurment_assist (acqu_assist)>
        <!ELEMENT GNSS_integrity (bad_signal*)>
        <!ELEMENT GNSS_UTC_model (utcModel5-r12)>
        <!ELEMENT GNSS_auxiliary_info (GNSS_id_glonass)>
    */
    VOS_UINT32                          bitOpGnssId                 : 1;
    VOS_UINT32                          bitOpSbasId                 : 1;
    VOS_UINT32                          bitOpGnssRefTime            : 1;
    VOS_UINT32                          bitOpGnssRefLocation        : 1;
    VOS_UINT32                          bitOpGnssRefMeaAssist       : 1;
    VOS_UINT32                          bitOpGnssIonosphericModel   : 1;
    VOS_UINT32                          bitOpGnssEarthOrientParam   : 1;
    VOS_UINT32                          bitOpGnssAddIonModel        : 1;
    VOS_UINT32                          bitOpGnssNavModel           : 1;
    VOS_UINT32                          bitOpGnssIntegrity          : 1;
    VOS_UINT32                          bitOpGnssDatabitAssist      : 1;
    VOS_UINT32                          bitOpGnssAlmanac            : 1;
    VOS_UINT32                          bitOpGnssUtcModel           : 1;
    VOS_UINT32                          bitOpBdsGridModelR12        : 1;
    VOS_UINT32                          bitOpGnssAuxiliaryInfo      : 1;
    VOS_UINT32                          bitOpGnssTimeModel          : 1;
    VOS_UINT32                          bitSpare                    : 16;

    CPOS_GNSS_ID_ENUM_UINT8             enGnssId;                               /* ATTRIBUTE gnss_id */
    CPOS_SBAS_ID_ENUM_UINT8             enSbasId;                               /* ATTRIBUTE sbas_id */
    VOS_UINT8                           aucReserved1[2];
    CPOS_GNSS_REF_TIME_STRU             stGnssRefTime;                          /* ELEMENT GNSS_ref_time */
    CPOS_LOCATION_PARAMS_STRU           stGnssRefLocation;                      /* ELEMENT GNSS_ref_location */
    CPOS_ACQU_ASSIST_STRU               stGnssRefMeaAssist;                     /* ELEMENT GNSS_ref_measurment_assist */
    CPOS_GNSS_IONOSPHERIC_MODEL_STRU    stGnssIonosphericModel;                 /* ELEMENT GNSS_ionospheric_model */
    CPOS_GNSS_EARTH_ORIENT_PARAM_STRU   stGnssEarthOrientParam;                 /* ELEMENT GNSS_earth_orient_param */
    CPOS_GNSS_ADD_ION_MODEL_STRU        stGnssAddIonModel;                      /* ELEMENT GNSS_additional_ion_model */
    CPOS_GNSS_TIME_MODEL_STRU           stGnssTimeModel;                        /* ELEMENT GNSS_time_model */
    CPOS_GNSS_NAV_MODEL_STRU            stGnssNavModel;                         /* ELEMENT GNSS_nav_model */
    CPOS_BAD_SIGNAL_ARRAY_STRU          stGnssIntegrity;                        /* ELEMENT GNSS_integrity */
    CPOS_ACQU_ASSIST_ARRAY_STRU         stAcquAssistArray;                      /* ELEMENT acqu_assist* */
    CPOS_GNSS_DATABIT_ASSIST_STRU       stGnssDataBitAssist;                    /* ELEMENT GNSS_databitassistance */
    CPOS_GNSS_ALMANAC_STRU              stGnssAlmanac;                          /* ELEMENT GNSS_almanac */
    CPOS_UTCMODEL5_R12_STRU             stGnssUtcModel;                         /* ELEMENT GNSS_UTC_model */
    CPOS_BDS_GRIDMODEL_R12_STRU         stBdsGridModelR12;                      /* ELEMENT BDS-GridModel-r12 */
    CPOS_GNSS_ID_GLONASS_STRU           stGnssAuxiliayInfo;                     /* ELEMENT GNSS_auxiliary_info */

}CPOS_GNSS_ASSIST_STRU;


typedef struct
{
    /*
    <!ELEMENT plmn_identity (mcc,mnc)>
        <!ELEMENT mcc (#PCDATA)> "460" -> 0x04 0x06 0x00
        <!ELEMENT mnc (#PCDATA)> "01"  -> 0x00 0x01 0xFF
    */
    VOS_UINT8                           aucMcc[CPOS_MCC_LEN];                   /* ELEMENT mcc */
    VOS_UINT8                           aucMnc[CPOS_MNC_LEN];                   /* ELEMENT mnc */
    VOS_UINT8                           aucReserved[2];
}CPOS_PLMN_IDENTITY_STRU;


typedef struct
{
    /*
    <!ELEMENT cell_global_id (plmn_identity,cell_identity)>
        <!ELEMENT cell_identity #PCDATA>
        <!-- 32-bit extended UTRAN cell ID; in case the cell ID is shorter,
            the most significant bits are set to 0. -->
    */
    CPOS_PLMN_IDENTITY_STRU             stPlmnId;                               /* ELEMENT plmn_identity */
    VOS_UINT32                          ulCellIdentity;                         /* ELEMENT cell_identity */
}CPOS_CELL_GLOBAL_ID_STRU;


typedef struct
{
    /*
    <!ELEMENT prs_muting_info_r9 (muting_info)>
        <!ATTLIST prs_muting_info_r9 literal(po2-r9|po4-r9|po8-r9|po16-r9) #REQUIRED>
        <!ELEMENT muting_info (#PCDATA)>
    */
    CPOS_PRS_MUTING_INFO_R9_ENUM_UINT8  enLiteral;                              /* ATTRIBE literal */
    VOS_UINT8                           aucReserved1[1];
    VOS_UINT16                          usMutingInfo;                           /* ELEMENT muting_info */
}CPOS_PRS_MUTING_INFO_R9_STRU;


typedef struct
{
    /*
    <!ELEMENT prs_info (prs_bandwidth,prs_conf_index,num_dl_frames,prs_muting_info_r9?)>
        <!ELEMENT prs_bandwidth EMPTY>
            <!ATTLIST prs_bandwidth literal(n6|n15|n25|n50|n75|n100)>
        <!ELEMENT prs_conf_index (#PCDATA)>  <!-- 0..4095 -->
        <!ELEMENT num_dl_frames EMPTY>
            <!ATTLIST num_dl_frames literal(sf-1|sf-2|sf-4|sf-6)#REQUIRED>
    */
    VOS_UINT32                          bitOpPrsMutingInfoR9        : 1;
    VOS_UINT32                          bitSpare                    : 31;

    CPOS_PRS_BANDWIDTH_ENUM_UINT8       enPrsBandwidth;                         /* ELEMENT prs_bandwidth */
    CPOS_NUM_DL_FRAMES_ENUM_UINT8       enNumDlFrames;                          /* ELEMENT num_dl_frames */
    VOS_UINT16                          usPrsConfIndex;                         /* ELEMENT prs_conf_index */
    CPOS_PRS_MUTING_INFO_R9_STRU        stPrsMutingInfoR9;                      /* ELEMENT prs_muting_info_r9 */

}CPOS_PRS_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_ref_cell_info (phys_cell_id,cell_global_id,earfcn_ref?,
        antenna_port_conf?,cp_length,prs_info?)>
        <!ELEMENT phys_cell_id (#PCDATA)>  <!-- 0..503 -->
        <!ELEMENT earfcn_ref (#PCDATA)>  <!-- 0..65535 -->
        <!ELEMENT antenna_port_conf EMPTY>
            <!ATTLIST antenna_port_conf ports(ports1-or-2|ports4) "ports1-or-2">
        <!ELEMENT cp_length EMPTY>
            <!ATTLIST cp_length literal(normal|extended) #REQUIRED>
    */
    VOS_UINT32                          bitOpEarfcnRef          : 1;
    VOS_UINT32                          bitOpAntennaPortConf    : 1;
    VOS_UINT32                          bitOpPrsInfo            : 1;
    VOS_UINT32                          bitOpSpare              : 29;

    VOS_UINT16                          usPhysCellId;                           /* ELEMENT phys_cell_id */
    VOS_UINT16                          usEarfcnRef;                            /* ELEMENT earfcn_ref */
    CPOS_CELL_GLOBAL_ID_STRU            stCellGlobalId;                         /* ELEMENT cell_global_id */
    CPOS_ANTENNA_PORT_ENUM_UINT8        enAntennaPortConf;                      /* ELEMENT antenna_port_conf */
    CPOS_CP_LENGTH_ENUM_UINT8           enCpLength;                             /* ELEMENT cp_length */
    VOS_UINT8                           aucReserved1[2];
    CPOS_PRS_INFO_STRU                  stPrsInfo;                              /* ELEMENT prs_info */
}CPOS_OTDOA_REF_CELL_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_neighbour_cell_info_element (phys_cell_id,cell_global_id?,
        earfcn?,cp_length?,prs_info?,antenna_port_conf?,slot_number_offset?,
        prs_subframe_offset?,expected_RSTD,expected_RSTD_uncertainty)>
        <!ELEMENT phys_cell_id (#PCDATA)>  <!-- 0..503 -->
        <!ELEMENT cell_global_id (#PCDATA)>
        <!ELEMENT earfcn (#PCDATA)>  <!-- 0..65535 -->
        <!ELEMENT slot_number_offset (#PCDATA)>  <!-- 0..19 -->
        <!ELEMENT prs_subframe_offset (#PCDATA)>  <!-- 0..1279 -->
        <!ELEMENT expected_RSTD (#PCDATA)>  <!-- 0..16383 -->
        <!ELEMENT expected_RSTD_uncertainty (#PCDATA)>  <!-- 0..1023 -->
    */
    VOS_UINT32                          bitOpCellGlobalId       : 1;
    VOS_UINT32                          bitOpEarfcn             : 1;
    VOS_UINT32                          bitOpCpLength           : 1;
    VOS_UINT32                          bitOpPrsInfo            : 1;
    VOS_UINT32                          bitOpAntennaPortConf    : 1;
    VOS_UINT32                          bitOpSlotNumOffset      : 1;
    VOS_UINT32                          bitOpPrsSubframeOffset  : 1;
    VOS_UINT32                          bitOpSpare              : 25;

    VOS_UINT16                          usPhysCellId;                           /* ELEMENT phys_cell_id */
    VOS_UINT16                          usEarfcn;                               /* ELEMENT earfcn */
    CPOS_CELL_GLOBAL_ID_STRU            stCellGlobalId;                         /* ELEMENT cell_global_id */
    CPOS_CP_LENGTH_ENUM_UINT8           enCpLength;                             /* ELEMENT cp_length */
    CPOS_ANTENNA_PORT_ENUM_UINT8        enAntennaPortConf;                      /* ELEMENT antenna_port_conf */
    VOS_UINT8                           ucSlotNubOffset;                        /* ELEMENT slot_number_offset */
    VOS_UINT8                           aucReserved1[1];
    VOS_UINT16                          usPrsSubframeOffset;                    /* ELEMENT prs_subframe_offset */
    VOS_UINT8                           aucReserved2[2];
    CPOS_PRS_INFO_STRU                  stPrsInfo;                              /* ELEMENT prs_info */
    VOS_UINT16                          usExpectedRstd;                         /* ELEMENT expected_RSTD */
    VOS_UINT16                          usExpectedRstdUncertainty;              /* ELEMENT expected_RSTD_uncertainty */

}CPOS_OTDOA_NB_CELL_INFO_ELEM_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_neighbour_freq_info (OTDOA_neighbour_cell_info_element+)>
    */
    VOS_UINT32                          ulCnt;
    CPOS_OTDOA_NB_CELL_INFO_ELEM_STRU   astOtdoaNBCellInfoElem[CPOS_OTDOA_NB_CELL_INFO_MAX_NUM];
}CPOS_OTDOA_NB_FREQ_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_neighbour_cell_info_list (OTDOA_neighbour_freq_info+)>
        <!ELEMENT OTDOA_neighbour_freq_info (OTDOA_neighbour_cell_info_element+)>
    */
    VOS_UINT32                          ulCnt;
    CPOS_OTDOA_NB_FREQ_INFO_STRU        astOtdoaNBFreqInfo[CPOS_OTDOA_NB_FREQ_INFO_MAX_NUM];
}CPOS_OTDOA_NB_CELL_INFO_LIST_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_assist_data (OTDOA_ref_cell_info,OTDOA_neighbour_cell_info_list)>
    */
    CPOS_OTDOA_REF_CELL_INFO_STRU       stOtdoaRefCellInfo;                     /* ELEMENT OTDOA_ref_cell_info */
    CPOS_OTDOA_NB_CELL_INFO_LIST_STRU   stOtdoaNBCellInfoList;                  /* ELEMENT OTDOA_neighbour_cell_info_list */
}CPOS_OTDOA_ASSIST_DATA_STRU;


typedef struct
{
    /*
    <!ELEMENT assist_data (GPS_assist?,GNSS_assist?,msr_assist_data?,
        system_info_assist_data?,more_assist_data?,ext_container?,
        rel98_assist_data_ext?,rel5_assist_data_ext?,rel7_assist_data_ext?,
        OTDOA_assist_data?)>
        <!ELEMENT msr_assist_data EMPTY>
        <!ELEMENT system_info_assist_data EMPTY>
        <!ELEMENT more_assist_data EMPTY>
        <!ELEMENT ext_container EMPTY>
        <!ELEMENT rel98_assist_data_ext EMPTY>
        <!ELEMENT rel5_assist_data_ext EMPTY>
        <!ELEMENT rel7_assist_data_ext EMPTY>
    */
    VOS_UINT32                          bitOpMsrAssistData      : 1;
    VOS_UINT32                          bitOpSysInfoAssistData  : 1;
    VOS_UINT32                          bitOpMoreAssistData     : 1;
    VOS_UINT32                          bitOpExtContainer       : 1;
    VOS_UINT32                          bitOpRel98AssistDataExt : 1;
    VOS_UINT32                          bitOpRel5AssistDataExt  : 1;
    VOS_UINT32                          bitOpRel7AssistDataExt  : 1;
    VOS_UINT32                          bitOpSpare              : 25;

    /* 目前不存在同时上报GPS_ASSIST、GNSS_ASSIST、OTDOA_ASSIST_DATA，为了减少结构体大小，修改为联合体 */
    /* 当enChoice为CPOS_ASSIST_DATA_TYPE_BUTT时,三个节点均不存在 */
    VOS_UINT8                           aucReserved[3];
    CPOS_ASSIST_DATA_TYPE_ENUM_UINT8    enChoice;

    union
    {
        CPOS_GPS_ASSIST_STRU            stGpsAssist;                            /* ELEMENT GPS_assist */
        CPOS_GNSS_ASSIST_STRU           stGnssAssist;                           /* ELEMENT GNSS_assist */
        CPOS_OTDOA_ASSIST_DATA_STRU     stOtdoaAssistData;                      /* ELEMENT OTDOA_assist_data */
    }u;
}CPOS_ASSIST_DATA_STRU;


typedef struct
{
    /*
    <!ELEMENT RRLP_method_type (ms_assisted|ms_assisted_no_accuracy|ms_based|
        ms_based_pref|ms_assisted_pref)>
        <!ELEMENT ms_assisted (method_accuracy)>
            <!ELEMENT method_accuracy (#PCDATA)>
        <!ELEMENT ms_assisted_no_accuracy EMPTY>
        <!ELEMENT ms_based (method_accuracy)>
        <!ELEMENT ms_based_pref (method_accuracy)>
        <!ELEMENT ms_assisted_pref (method_accuracy)>
    */
    CPOS_RRLP_METHOD_TYPE_ELEM_ENUM_UINT8   enChoice;
    VOS_UINT8                               aucReserved[1];
    VOS_UINT16                              usMethodAccuracy;
}CPOS_RRLP_METHOD_TYPE_STRU;


typedef struct
{
    /*
    <!ELEMENT RRLP_pos_instruct (RRLP_method_type,RRLP_method,resp_time_seconds,mult_sets)>
        <!ELEMENT RRLP_method EMPTY>
            <!ATTLIST RRLP_method literal (gps) #REQUIRED>
        <!ELEMENT resp_time_seconds (#PCDATA)>
        <!ELEMENT mult_sets EMPTY>
            <!ATTLIST mult_sets literal (multiple|one) #REQUIRED>
    */
    CPOS_RRLP_METHOD_TYPE_STRU          stRrlpMethodType;                       /* ELEMENT RRLP_method_type */
    CPOS_RRLP_METHOD_ENUM_UINT8         enRrlpMethod;                           /* ELEMENT RRLP_method */
    CPOS_MULT_SETS_ENUM_UINT8           enMultSets;                             /* ELEMENT mult_sets */
    VOS_UINT16                          usRespTimeSeconds;                      /* ELEMENT resp_time_seconds */
}CPOS_RRLP_POS_INSTRUCT_STRU;


typedef struct
{
    /*
    <!ELEMENT rep_quant (RRC_method_type,RRC_method,hor_acc?,vert_acc?)>
        <!ATTLIST rep_quant
            gps_timing_of_cell_wanted (true|false) "false"
            addl_assist_data_req      (true|false) #REQUIRED>
        <!ELEMENT RRC_method_type EMPTY>
            <!ATTLIST RRC_method_type literal (ue_assisted|ue_based|
                ue_based_pref|ue_assisted_pref) #REQUIRED>
        <!ELEMENT RRC_method EMPTY>
            <!ATTLIST RRC_method literal (otdoa|gps|otdoaOrGPS|cellID) #REQUIRED>
        <!ELEMENT hor_acc (#PCDATA)>
        <!ELEMENT vert_acc (#PCDATA)>
    */
    VOS_UINT32                          bitOpHorAcc         : 1;
    VOS_UINT32                          bitOpVertAcc        : 1;
    VOS_UINT32                          bitSpare            : 30;

    CPOS_BOOL_ENUM_UINT8                enGpsTocWanted;                         /* ELEMENT gps_timing_of_cell_wanted */
    CPOS_BOOL_ENUM_UINT8                enAddlAssistDataReq;                    /* ELEMENT addl_assist_data_req */
    CPOS_RRC_METHOD_TYPE_ENUM_UINT8     enRrcMethodType;                        /* ELEMENT RRC_method_type */
    CPOS_RRC_METHOD_ENUM_UINT8          enRrcMethod;                            /* ELEMENT RRC_method */
    VOS_UINT8                           ucHorAcc;                               /* ELEMENT hor_acc */
    VOS_UINT8                           ucVertAcc;                              /* ELEMENT vert_acc */
    VOS_UINT8                           aucReserved[2];
}CPOS_REP_QUANT_STRU;


typedef struct
{
    /*
    <!ELEMENT event_specific_info (tr_pos_chg|tr_SFN_SFN_chg|tr_SFN_GPS_TOW)>
        <!ELEMENT tr_pos_chg EMPTY>
            <!ATTLIST tr_pos_chg literal (pc10|pc20|pc30|pc40|pc50|
                pc100|pc200|pc300|pc500|pc1000|pc2000|pc5000|
                pc10000|pc20000|pc50000|pc100000) #REQUIRED>
        <!ELEMENT tr_SFN_SFN_chg EMPTY>
            <!ATTLIST tr_SFN_SFN_chg literal (c0-25|c0-5|c1|c2|c3|c4|c5|c10|
                c20|c50|c100|c200|c500|c1000|c2000|c5000) #REQUIRED>
        <!ELEMENT tr_SFN_GPS_TOW EMPTY>
            <!ATTLIST tr_SFN_GPS_TOW literal (ms1|ms2|ms3|ms5|
                ms10|ms20|ms50|ms100)  #REQUIRED>
    */
    VOS_UINT8                               aucReserved[2];
    CPOS_EVENT_SPEC_INFO_ELEM_ENUM_UINT8    enChoice;
    union
    {
        CPOS_TR_POS_CHG_ENUM_UINT8          enTrPosChg;                         /* ELEMENT tr_pos_chg */
        CPOS_TR_SFN_SFN_CHG_ENUM_UINT8      enTrSfnSfnChg;                      /* ELEMENT tr_SFN_SFN_chg */
        CPOS_TR_SFN_GPS_TOW_ENUM_UINT8      enTrSfnGpsTow;                      /* ELEMENT tr_SFN_GPS_TOW */
    } u;
}CPOS_EVENT_SPEC_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT event_par (rep_amount,meas_interval,event_specific_info?)>
        <!ATTLIST event_par report_first_fix (true|false) #REQUIRED>
        <!ELEMENT rep_amount EMPTY>
            <!ATTLIST rep_amount literal (ra1|ra2|ra4|ra8|ra16|ra32|ra64|ra-Infinity) #REQUIRED>
        <!ELEMENT meas_interval EMPTY>
            <!ATTLIST meas_interval literal (e5|e15|e60|e300|e900|e1800|e3600|e7200) #REQUIRED>
    */
    VOS_UINT32                          bitOpEventSpecInfo  : 1;
    VOS_UINT32                          bitSpare            : 31;

    CPOS_BOOL_ENUM_UINT8                enReportFirstFix;                       /* ATTRIBE report_first_fix */
    CPOS_REP_AMOUNT_ENUM_UINT8          enRepAmount;                            /* ELEMENT rep_amount */
    CPOS_MEAS_INTERVAL_ENUM_UINT8       enMeasInterval;                         /* ELEMENT meas_interval */
    VOS_UINT8                           aucReserved[1];
    CPOS_EVENT_SPEC_INFO_STRU           stEventSpecInfo;                        /* ELEMENT event_specific_info */
}CPOS_EVENT_PAR_STRU;


typedef struct
{
    /*
    <!ELEMENT event_rep_crit (event_par*)>
    */
    VOS_UINT32                          ulCnt;
    CPOS_EVENT_PAR_STRU                 astEventPar[CPOS_EVENT_PAR_MAX_NUM];
}CPOS_EVENT_REP_CRIT_STRU;


typedef struct
{
    /*
    <!ELEMENT period_rep_crit EMPTY>
        <!ATTLIST period_rep_crit rep_amount (ra1|ra2|ra4|ra8|ra16|ra32|
            ra64|ra-Infinity) "ra-Infinity">
        <!ATTLIST period_rep_crit rep_interval_long (ril0|ril0-25|
            ril0-5|ril1|ril2|ril3|ril4|ril6|ril8|ril12|ril16|
            ril20|ril24|ril28|ril32|ril64) #REQUIRED>
    */
    CPOS_REP_AMOUNT_ENUM_UINT8          enRepAmount;                            /* ATTRIBE rep_amount */
    CPOS_REP_INTERVAL_LONG_ENUM_UINT8   enRepIntervalLong;                      /* ELEMENT rep_interval_long */
    VOS_UINT8                           aucReserved[2];
}CPOS_PERIOD_REP_CRIT_STRU;


typedef struct
{
    /*
    <!ELEMENT rep_crit (no_rep|event_rep_crit|period_rep_crit)>
       <!ELEMENT no_rep EMPTY>
    */
    VOS_UINT8                           aucReserved[3];
    CPOS_REP_CRIT_ELEM_ENUM_UINT8       enChoice;
    union
    {
        CPOS_EVENT_REP_CRIT_STRU        stEventRepCrit;                         /* ELEMENT event_rep_crit */
        CPOS_PERIOD_REP_CRIT_STRU       stPeriodRepCrit;                        /* ELEMENT period_rep_crit */
    } u;
}CPOS_REP_CRIT_STRU;


typedef struct
{
    /*
    <!ELEMENT RRC_meas (rep_quant,rep_crit)>
    */
    CPOS_REP_QUANT_STRU                 stRepQuant;                             /* ELEMENT rep_quant */
    CPOS_REP_CRIT_STRU                  stRepCrit;                              /* ELEMENT rep_crit */
}CPOS_RRC_MEAS_STRU;


typedef struct
{
    /*
    <!ELEMENT pos_meas (meas_abort|RRLP_meas|RRC_meas)>
        <!ELEMENT meas_abort EMPTY>
        <!ELEMENT RRLP_meas (RRLP_pos_instruct)>
    */
    VOS_UINT8                           aucReserved[3];

    CPOS_POS_MEAS_ELEM_ENUM_UINT8       enChoice;
    union
    {
        CPOS_RRLP_POS_INSTRUCT_STRU     stRrlpMeas;                             /* ELEMENT RRLP_meas */
        CPOS_RRC_MEAS_STRU              stRrcMeas;                              /* ELEMENT RRC_meas */
    } u;

}CPOS_POS_MEAS_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_allowed_methods (gps_method?,sbas_method?,qzss_method?,
        glonass_method?,BDS_method?)>
        <!ELEMENT gps_method EMPTY>
        <!ELEMENT sbas_method EMPTY>
        <!ELEMENT qzss_method EMPTY>
        <!ELEMENT glonass_method EMPTY>
        <!ELEMENT BDS_method EMPTY>
    */
    VOS_UINT32                          bitOpGpsMethod      : 1;
    VOS_UINT32                          bitOpSbasMethod     : 1;
    VOS_UINT32                          bitOpQzssMethod     : 1;
    VOS_UINT32                          bitOpGlonassMethod  : 1;
    VOS_UINT32                          bitOpBdsMethod      : 1;
    VOS_UINT32                          bitSpare            : 27;
}CPOS_GNSS_ALLOWED_METHODS_STRU;


typedef struct
{
    /*
    <!ELEMENT req_info (GNSS_allowed_methods?)>
        <!ATTLIST req_info
            loc_info_type (loc_req|meas_req|loc_pref|MSB_req|MSA_req|MSB_pref|MSA_pref) "loc_req"
            velocity_req (true|false) "false"
            time_req (true|false) "false"
            multiple_sets (yes|no) "no"
            additional_info (yes|no) "no">  <!-- lpp only -->  <!-- MSB synonomous with loc_req -->
    */
    VOS_UINT32                          bitOpGnssAllowedMethods : 1;
    VOS_UINT32                          bitSpare                : 31;

    CPOS_LOC_INFO_TYPE_ENUM_UINT8       enLocInfoType;                          /* ATTRIBE loc_info_type */
    CPOS_BOOL_ENUM_UINT8                enVelocityReq;                          /* ATTRIBE velocity_req */
    CPOS_BOOL_ENUM_UINT8                enTimeReq;                              /* ATTRIBE time_req */
    CPOS_WHETHER_ENUM_UINT8             enMultipleSet;                          /* ATTRIBE multiple_sets */
    CPOS_WHETHER_ENUM_UINT8             enAdditionalInfo;                       /* ATTRIBE additional_info */
    VOS_UINT8                           aucReserved[3];
    CPOS_GNSS_ALLOWED_METHODS_STRU      stGnssAllowedMethods;                   /* ELEMENT GNSS_allowed_methods */
}CPOS_REQ_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT hacc EMPTY>
        <!ATTLIST hacc accuracy_k CDATA #IMPLIED confidence CDATA #IMPLIED>
    */

    VOS_UINT32                          bitOpAccuracyK  : 1;
    VOS_UINT32                          bitOpConfidence : 1;
    VOS_UINT32                          bitSpare        : 30;

    VOS_UINT16                          usAccuracyK;                            /* ATTRIBE accuracy_k */
    VOS_UINT16                          usConfidence;                           /* ATTRIBE confidence */
}CPOS_HACC_STRU;


typedef CPOS_HACC_STRU                  CPOS_VACC_STRU;


typedef struct
{
    /*
    <!ELEMENT qos (response_time?,(reporting_interval,amount)?,hacc?,vacc?)>
        <!-- The presence of reporting_interval implies a periodic procedure -->
        <!-- The absence of reporting_interval implies a one-shot procedure -->
        <!ELEMENT response_time EMPTY>
            <!ATTLIST response_time sec CDATA #IMPLIED>
        <!ELEMENT reporting_interval EMPTY>
            <!ATTLIST reporting_interval ri_exp (ril1|ril2|ril3|ril4|ril6|ril8|
                ril12|ril16|ril20|ril24|ril28|ril32|ril64) "ril20" >
        <!ELEMENT amount EMPTY>
            <!ATTLIST amount amount (ra1|ra2|ra4|ra8|ra16|ra32|ra64|ra-Infinity) "ra-Infinity">
        <!ELEMENT vacc EMPTY>
            <!ATTLIST vacc accuracy_k CDATA #IMPLIED confidence CDATA #IMPLIED>
    */
    VOS_UINT32                          bitOpResponseTime       : 1;
    VOS_UINT32                          bitOpRptIntervalAmount  : 1;
    VOS_UINT32                          bitOpHacc               : 1;
    VOS_UINT32                          bitOpVacc               : 1;
    VOS_UINT32                          bitSpare                : 28;

    VOS_UINT16                          usResponseTimeSec;                      /* ELEMENT response_time */
    CPOS_RI_EXP_ENUM_UINT8              enReportingInterval;                    /* ELEMENT reporting_interval */
    CPOS_AMOUNT_ENUM_UINT8              enAmount;                               /* ELEMENT amount */

    CPOS_HACC_STRU                      stHacc;                                 /* ELEMENT hacc */
    CPOS_VACC_STRU                      stVacc;                                 /* ELEMENT vacc */
}CPOS_QOS_STRU;


typedef struct
{
    /*
    <!-- Alternative to pos_meas, supports RRLP, RRC and LPP -->
    <!ELEMENT pos_meas_req (abort|(req_info,qos))>
        <!ATTLIST pos_meas_req aid_req_allowed (yes|no) #IMPLIED>
        <!-- Present for RRC and LPP procedures -->
        <!ELEMENT abort EMPTY)>
    */
    VOS_UINT8                           bitOpAidReqAllowed  : 1;
    VOS_UINT8                           bitSpare            : 7;

    CPOS_WHETHER_ENUM_UINT8             enAidReqAllowed;                        /* ATTRIBE aid_req_allowed */
    VOS_UINT8                           ucReserved;

    CPOS_POS_MEAS_REQ_ELEM_ENUM_UINT8   enChoice;

    CPOS_REQ_INFO_STRU                  stReqInfo;                              /* ELEMENT req_info */
    CPOS_QOS_STRU                       stQos;                                  /* ELEMENT qos */

}CPOS_POS_MEAS_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT ref_time_only (tow_msec)>
    */
    VOS_UINT32                          ulTowMsec;                              /* ELEMENT tow_msec, ulGpsRefTime, INTEGER(0..604799999) */
}CPOS_REF_TIME_ONLY_STRU;



typedef struct
{
    /*
    <!ELEMENT meas_params (sat_id,carr2_noise,dopl,whole_chips,fract_chips,multi_path,psr_rms_err)>
        <!ELEMENT carr2_noise (#PCDATA)>
        <!ELEMENT dopl (#PCDATA)>
        <!ELEMENT whole_chips (#PCDATA)>
        <!ELEMENT fract_chips (#PCDATA)>
        <!ELEMENT multi_path EMPTY>
            <!ATTLIST multi_path literal (not_measured|low|medium|high) #REQUIRED>
        <!ELEMENT psr_rms_err (#PCDATA)>
    */
    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id,INTEGER(0..63) */
    VOS_UINT8                           ucCarr2Noise;                           /* ELEMENT carr2_noise, INTEGER(0..63) */
    CPOS_MULTI_PATH_ENUM_UINT8          enMultiPath;                            /* ELEMENT multi_path */
    VOS_UINT8                           ucPsrRmsErr;                            /* ELEMENT psr_rms_err, INTEGER(0..63) */
    VOS_INT32                           lDoppler;                               /* ELEMENT dopl, RRLP:INTEGER(-32768..32767),RRC:INTEGER(-32768..32768) */
    VOS_UINT16                          usWholeChips;                           /* ELEMENT whole_chips, INTEGER(0..1022) */
    VOS_UINT16                          usFracChips;                            /* ELEMENT fract_chips, INTEGER(0..1024) */
} CPOS_MEAS_PARAMS_STRU;



typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_MEAS_PARAMS_STRU               astMeasParams[CPOS_MEAS_PARAMS_MAX_NUM];
}CPOS_MEAS_PARAMS_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_meas (ref_time_only,meas_params*)>
    */
    CPOS_REF_TIME_ONLY_STRU             stRefTimeOnly;                          /* ELEMENT ref_time_only */
    CPOS_MEAS_PARAMS_ARRAY_STRU         stMeasParams;                           /* ELEMENT meas_params* */
}CPOS_GPS_MEAS_STRU;


typedef struct
{
    /*
    <!ELEMENT abort EMPTY>
        <!ATTLIST abort
            abort_otdoa_meas_req (true|false) "false"
            abort_ecid_meas_req (true|false) "false">
        <!-- Session ID will be provided in Transaction ID under root element <pos> -->
    */
    CPOS_BOOL_ENUM_UINT8                enAbortOtdoaMeasReq;                    /* ELEMENT abort_otdoa_meas_req */
    CPOS_BOOL_ENUM_UINT8                enAbortEcidMeasReq;                     /* ELEMENT abort_ecid_meas_req* */
    VOS_UINT8                           aucReserved[2];
}CPOS_OTDOA_ECID_REQ_ABORT_STRU;


typedef struct
{
    /*
    <!ELEMENT ECID_req_loc_info EMPTY>
        <!ATTLIST ECID_req_loc_info
            rs_rp_req (true|false) "false"
            rs_rq_req (true|false) "false"
            ue_rx_tx_req (true|false) "false">
    */
    CPOS_BOOL_ENUM_UINT8                enRsRpReq;                              /* ELEMENT rs_rp_req */
    CPOS_BOOL_ENUM_UINT8                enRsRqReq;                              /* ELEMENT rs_rq_req* */
    CPOS_BOOL_ENUM_UINT8                enUeRxTxReq;                            /* ELEMENT ue_rx_tx_req* */
    VOS_UINT8                           aucReserved[1];
}CPOS_ECID_REQ_LOC_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT periodic_reporting (rep_amount,rep_interval)>
        <!ELEMENT rep_amount EMPTY>
            <!ATTLIST rep_amount literal (ra1|ra2|ra4|ra8|ra16|ra32|ra64|ra-Infinity) #REQUIRED>
        <!ELEMENT rep_interval EMPTY>
            <!ATTLIST rep_interval literal (ri1|ri2|ri4|ri8|ri16|ri32|ri64) #REQUIRED>
    */
    CPOS_REP_AMOUNT_ENUM_UINT8          enRepAmount;                            /* ELEMENT rep_amount */
    CPOS_REP_INTERVAL_ENUM_UINT8        enRepInterval;                          /* ELEMENT rep_interval* */
    VOS_UINT8                           aucReserved[2];
}CPOS_PERIODIC_REPORTING_STRU;


typedef struct
{
    /*
    <!ELEMENT triggered_reporting (rep_duration)>
        <!ATTLIST triggered_reporting cell_change (true|false) #REQUIRED>
        <!ELEMENT rep_duration #PCDATA>  <!-- 0..255 -->
    */
    CPOS_BOOL_ENUM_UINT8                enCellChange;                           /* ATTRIBE cell_change */
    VOS_UINT8                           ucRepDuration;                          /* ELEMENT rep_duration */
    VOS_UINT8                           aucReserved[2];
}CPOS_TRIGGERED_REPORTING_STRU;


typedef struct
{
    /*
    <!ELEMENT response_time #PCDATA>  <!-- 1..128 -->
    */
    VOS_UINT8                               aucReserved[3];

    CPOS_REQ_LOC_INFO_TYPE_ELEM_ENUM_UINT8  enChoice;
    union
    {
        VOS_UINT8                           ucResponseTime;                     /* ELEMENT response_time */
        CPOS_PERIODIC_REPORTING_STRU        stPeriodicReporting;                /* ELEMENT periodic_reporting */
        CPOS_TRIGGERED_REPORTING_STRU       stTriggeredReporting;               /* ELEMENT triggered_reporting */
    } u;
}CPOS_REQ_LOC_INFO_TYPE_STRU;


typedef struct
{
    /*
    <!ELEMENT request_location_info (OTDOA_req_loc_info?,ECID_req_loc_info?,
        (response_time|periodic_reporting|triggered_reporting)?)>
        <!-- triggered_reporting for ECID_req_loc_info only -->
        <!ATTLIST request_location_info emergency_call_indicator (true|false) "false">
        <!ELEMENT OTDOA_req_loc_info EMPTY>
            <!ATTLIST OTDOA_req_loc_info assistance_available (true|false) #REQUIRED>
    */
    VOS_UINT32                          bitOpOtdoaReqLocInfo    : 1;
    VOS_UINT32                          bitOpEcidReqLocInfo     : 1;
    VOS_UINT32                          bitOpReqLocInfoType     : 1;
    VOS_UINT32                          bitSpare                : 29;

    CPOS_BOOL_ENUM_UINT8                enEmergencyCallInd;                     /* ATTRIBE emergency_call_indicator */
    CPOS_BOOL_ENUM_UINT8                enOtdoaReqLocInfoAsistAvailable;        /* ELEMENT OTDOA_req_loc_info */
    VOS_UINT8                           aucReserved[2];

    CPOS_ECID_REQ_LOC_INFO_STRU         stEcidReqLocInfo;                       /* ELEMENT ECID_req_loc_info */
    CPOS_REQ_LOC_INFO_TYPE_STRU         stReqLocInfoType;                       /* ELEMENT (response_time|periodic_reporting|triggered_reporting) */

}CPOS_REQUEST_LOCATION_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_ECID_req (abort|request_location_info)>
    */
    VOS_UINT8                           aucReserved[3];

    CPOS_OTDOA_ECID_REQ_ELEM_ENUM_UINT8 enChoice;
    union
    {
        CPOS_OTDOA_ECID_REQ_ABORT_STRU  stAbort;                                /* ELEMENT abort */
        CPOS_REQUEST_LOCATION_INFO_STRU stReqLocationInfo;                      /* ELEMENT request_location_info */
    } u;
}CPOS_OTDOA_ECID_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT cell_global_id_ref (mcc,mnc,cell_id)>
        <!ELEMENT mcc (#PCDATA)>
        <!ELEMENT mnc (#PCDATA)>
        <!ELEMENT cell_id (#PCDATA)>
    */
    VOS_UINT8                           aucMcc[CPOS_MCC_LEN];                   /* ELEMENT mcc */
    VOS_UINT8                           aucMnc[CPOS_MNC_LEN];                   /* ELEMENT mnc */
    VOS_UINT8                           aucReserved[2];
    VOS_UINT32                          ulCellId;                               /* ELEMENT cell_id */
}CPOS_CELL_GLOBAL_ID_REF_STRU;


typedef struct
{
    /*
    <!ELEMENT cell_global_id_neighbour (mcc,mnc,cell_id)>
        <!ELEMENT mcc (#PCDATA)>
        <!ELEMENT mnc (#PCDATA)>
        <!ELEMENT cell_identity (#PCDATA)>
    */
    VOS_UINT8                           aucMcc[CPOS_MCC_LEN];                   /* ELEMENT mcc */
    VOS_UINT8                           aucMnc[CPOS_MNC_LEN];                   /* ELEMENT mnc */
    VOS_UINT8                           aucReserved[2];
    VOS_UINT32                          ulCellIdentity;                               /* ELEMENT cell_id */
}CPOS_CELL_GLOBAL_ID_NEIGHBOUR_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_meas_quality (err_resolution,err_value,err_num_samples?)>
         <!ELEMENT err_resolution (#PCDATA)>
         <!ELEMENT err_value (#PCDATA)>
         <!ELEMENT err_num_samples (#PCDATA)>
    */
    VOS_UINT32                          bitOpErrNumSample   : 1;
    VOS_UINT32                          bitSpare            : 31;

    VOS_UINT8                           ucErrResolution;                        /* ELEMENT err_resolution */
    VOS_UINT8                           ucErrValue;                             /* ELEMENT err_value */
    VOS_UINT8                           ucErrNumSamples;                        /* ELEMENT err_num_samples */
    VOS_UINT8                           ucReserved;
}CPOS_OTDOA_MEAS_QUALITY_STRU;


typedef struct
{
    /*
    <!ELEMENT neighbour_meas_element (phys_cell_id_neighbour,cell_global_id_neighbour?,earfcn_neighbour?,rstd,rstd_quality)>
        <!ELEMENT phys_cell_id_neighbour (#PCDATA)>
        <!ELEMENT cell_global_id_neighbour (mcc,mnc,cell_identity)>
            <!ELEMENT mcc (#PCDATA)>
            <!ELEMENT mnc (#PCDATA)>
            <!ELEMENT cell_identity (#PCDATA)>
        <!ELEMENT earfcn_neighbour (#PCDATA)>  <!-- 0..65535 -->
        <!ELEMENT rstd (#PCDATA)>  <!-- 0..12711 -->
        <!ELEMENT rstd_quality (OTDOA_meas_quality)>
    */
    VOS_UINT32                          bitOpCellGlobalIdNB : 1;
    VOS_UINT32                          bitOpEarfcnNB       : 1;
    VOS_UINT32                          bitSpare            : 30;

    VOS_UINT32                          ulPhysCellIdNB;                         /* ELEMENT phys_cell_id_neighbour */
    CPOS_CELL_GLOBAL_ID_NEIGHBOUR_STRU  stCellGlobalIdNB;                       /* ELEMENT cell_global_id_neighbour */
    VOS_UINT16                          usEarfcnNB;                             /* ELEMENT earfcn_neighbour */
    VOS_UINT16                          usRstd;                                 /* ELEMENT rstd */
    CPOS_OTDOA_MEAS_QUALITY_STRU        stRstdQuality;                          /* ELEMENT rstd_quality */
}CPOS_NB_MEAS_ELEM_STRU;


typedef struct
{
    /*
    <!ELEMENT neighbour_meas_list (neighbour_meas_element+)>
    */
    VOS_UINT32                          ulCnt;
    CPOS_NB_MEAS_ELEM_STRU              astNBMeasElem[CPOS_NB_MEAS_ELEM_MAX_NUM];
}CPOS_NB_MEAS_LIST_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_meas (system_frame_number,phys_cell_id_ref,
        cell_global_id_ref?,earfcn_ref?,ref_quality?,neighbour_meas_list)>
        <!ELEMENT system_frame_number (#PCDATA)>
        <!ELEMENT phys_cell_id_ref (#PCDATA)>  <!-- 0..503 -->
        <!ELEMENT earfcn_ref (#PCDATA)>  <!-- 0..65535 -->
        <!ELEMENT ref_quality (OTDOA_meas_quality)>
    */
    VOS_UINT32                          bitOpCellGlobalIdRef    : 1;
    VOS_UINT32                          bitOpEarfcnRef          : 1;
    VOS_UINT32                          bitOpRefQuality         : 1;
    VOS_UINT32                          bitSpare                : 29;

    VOS_UINT16                          usSystemFrameNumber;                    /* ELEMENT system_frame_number */
    VOS_UINT16                          usPhysCellIdRef;                        /* ELEMENT phys_cell_id_ref */
    CPOS_CELL_GLOBAL_ID_REF_STRU        stCellGlobalIdRef;                      /* ELEMENT cell_global_id_ref */
    VOS_UINT16                          usEarfcnRef;                            /* ELEMENT earfcn_ref */
    VOS_UINT8                           aucReserved[2];
    CPOS_OTDOA_MEAS_QUALITY_STRU        stRefQuality;                           /* ELEMENT ref_quality */
    CPOS_NB_MEAS_LIST_STRU              stNBMeasList;                           /* ELEMENT neighbour_meas_list */
}CPOS_OTDOA_MEAS_STRU;


typedef struct
{
    /*
    <!ELEMENT measured_result_element (phys_cell_id,cell_global_id?,
        earfcn_eutra,sfn?,rsrp_result?,rsrq_result?,ue_rxtx_time_diff?)>
        <!ELEMENT phys_cell_id (#PCDATA)>  <!-- 0..503 -->
        <!ELEMENT cell_global_id (mcc,mnc,cell_id)>
            <!ELEMENT mcc (#PCDATA)>
            <!ELEMENT mnc (#PCDATA)>
            <!ELEMENT cell_id (#PCDATA)>
        <!ELEMENT earfcn_eutra (#PCDATA)>  <!-- 0..262143 -->
        <!ELEMENT sfn (#PCDATA)>
        <!ELEMENT rsrp_result (#PCDATA)>  <!-- 0..97 -->
        <!ELEMENT rsrq_result (#PCDATA)>  <!-- 0..34 -->
        <!ELEMENT ue_rxtx_time_diff (#PCDATA)>  <!-- 0..4095 -->
    */
    VOS_UINT32                          bitOpCellGlobalId   : 1;
    VOS_UINT32                          bitOpSfn            : 1;
    VOS_UINT32                          bitOpRsrpResult     : 1;
    VOS_UINT32                          bitOpRsrqResult     : 1;
    VOS_UINT32                          bitOpUeRxtxTimeDiff : 1;
    VOS_UINT32                          bitSpare            : 27;

    VOS_UINT16                          usPhysCellId;                           /* ELEMENT phys_cell_id */
    VOS_UINT8                           aucReserved[2];
    CPOS_CELL_GLOBAL_ID_REF_STRU        stCellGlobalId;                         /* ELEMENT cell_global_id */
    VOS_UINT32                          ulEarfcnEutra;                          /* ELEMENT earfcn_neighbour */
    VOS_UINT32                          ulSfn;                                  /* ELEMENT sfn */
    VOS_UINT8                           ucRsrpResult;                           /* ELEMENT rsrp_result */
    VOS_UINT8                           ucRsrqResult;                           /* ELEMENT rsrq_result */
    VOS_UINT16                          usUeRxtxTimeDiff;                       /* ELEMENT ue_rxtx_time_diff */
}CPOS_MEAS_RESULT_ELEM_STRU;


typedef struct
{
    /*
    <!ELEMENT measured_result_list (measured_result_element+)>  <!-- 1..32 -->
    */
    VOS_UINT32                          ulCnt;
    CPOS_MEAS_RESULT_ELEM_STRU          astMeasResultElem[CPOS_MEAS_RESULT_ELEM_MAX_NUM];
}CPOS_MEAS_RESULT_LIST_STRU;


typedef struct
{
    /*
    <!ELEMENT ECID_meas (primary_cell_measured_results?,measured_result_list)>
        <!ELEMENT primary_cell_measured_results (measured_result_element)>
    */
    VOS_UINT32                          bitOpPrimCellMeasResults    : 1;
    VOS_UINT32                          bitSpare                    : 31;

    CPOS_MEAS_RESULT_ELEM_STRU          stPrimCellMeasResults;                  /* ELEMENT primary_cell_measured_results */
    CPOS_MEAS_RESULT_LIST_STRU          stMeasResultList;                       /* ELEMENT measured_result_list */
}CPOS_ECID_MEAS_STRU;


typedef struct
{
    /*
    <!ELEMENT addl_req_sat (sat_id,iode)>
    */
    VOS_UINT8                           ucSatId;                                /* ELEMENT sat_id,INTEGER(0..63) */
    VOS_UINT8                           ucIode;                                 /* ELEMENT IODE */
    VOS_UINT8                           aucReserved[2];
}CPOS_ADDL_REQ_SAT_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_ADDL_REQ_SAT_STRU              astAddlReqSat[CPOS_SAT_MAX_NUM];
}CPOS_ADDL_REQ_SAT_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT nav_addl_data (GPS_week,GPS_toe,ttoe_limit,addl_req_sat*)>
        <!ELEMENT GPS_toe (#PCDATA)>
        <!ELEMENT ttoe_limit (#PCDATA)>
    */
    VOS_UINT16                          usGpsWeek;                              /* ELEMENT GPS_week */
    VOS_UINT16                          usGpsToe;                               /* ELEMENT GPS_toe */
    VOS_UINT32                          ulTtoeLimit;                            /* ELEMENT ttoe_limit */
    CPOS_ADDL_REQ_SAT_ARRAY_STRU        stAddlReqSatArray;                      /* ELEMENT addl_req_sat* */
}CPOS_NAV_ADDL_DATA_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_assist_req (nav_addl_data?)>
        <!ATTLIST GPS_assist_req
            alm_req        (true|false) "false"
            UTC_model_req  (true|false) "false"
            ion_req        (true|false) "false"
            nav_model_req  (true|false) "false"
            DGPS_corr_req  (true|false) "false"
            ref_loc_req    (true|false) "false"
            ref_time_req   (true|false) "false"
            aqu_assist_req (true|false) "false"
            rt_integr_req  (true|false) "false">
    */
    VOS_UINT32                          bitOpNavAddlData    : 1;
    VOS_UINT32                          bitSpare            : 31;

    CPOS_BOOL_ENUM_UINT8                enAlmReq;                               /* ATTRIBE alm_req */
    CPOS_BOOL_ENUM_UINT8                enUtcModelReq;                          /* ATTRIBE UTC_model_req */
    CPOS_BOOL_ENUM_UINT8                enIonReq;                               /* ATTRIBE ion_req */
    CPOS_BOOL_ENUM_UINT8                enNavModelReq;                          /* ATTRIBE nav_model_req */
    CPOS_BOOL_ENUM_UINT8                enDgpsCorrReq;                          /* ATTRIBE DGPS_corr_req */
    CPOS_BOOL_ENUM_UINT8                enRefLocReq;                            /* ATTRIBE ref_loc_req */
    CPOS_BOOL_ENUM_UINT8                enRefTimeReq;                           /* ATTRIBE ref_time_req */
    CPOS_BOOL_ENUM_UINT8                enAquAssistReq;                         /* ATTRIBE aqu_assist_req */
    CPOS_BOOL_ENUM_UINT8                enRtIntegrReq;                          /* ATTRIBE rt_integr_req */
    VOS_UINT8                           aucReserved[3];

    CPOS_NAV_ADDL_DATA_STRU             stNavAddlData;                          /* ELEMENT nav_addl_data */
}CPOS_GPS_ASSIST_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT stored_sat_list_element EMPTY>
        <!ATTLIST stored_sat_list_element
            stored_id CDATA #REQUIRED
            stored_iod CDATA #REQUIRED
            stored_clock_model (2|3|4|5|6) #IMPLIED
            stored_orbit_model (2|3|4|5|6) #IMPLIED>
    */
    VOS_UINT32                          bitOpStoredClockModel   : 1;
    VOS_UINT32                          bitOpStoredOrbitModel   : 1;
    VOS_UINT32                          bitSpare                : 30;

    VOS_UINT16                          usStoredId;                             /* ATTRIBE stored_id */
    VOS_UINT16                          usStoredIod;                            /* ATTRIBE stored_iod */
    CPOS_CLOCK_MODEL_ENUM_UINT8         enStoredClockModel;                     /* ATTRIBE stored_clock_model */
    CPOS_ORBIT_MODEL_ENUM_UINT8         enStoredOrbitModel;                     /* ATTRIBE stored_orbit_model */
    VOS_UINT8                           aucReserved[2];
}CPOS_STORED_SAT_LIST_ELEM_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_STORED_SAT_LIST_ELEM_STRU      astStoredSatListElem[CPOS_SAT_MAX_NUM];
}CPOS_STORED_SAT_LIST_STRU;


typedef struct
{
    /*
    <!ELEMENT requested_nav_list_info EMPTY>
        <!ATTLIST requested_nav_list_info
            requested_sv CDATA "00000000ffffffff"
            requested_clock_model (2|3|4|5|6) #IMPLIED
            requested_orbit_model (2|3|4|5|6) #IMPLIED
            requested_add_nav_param (false|true) #IMPLIED>
        <!-- requested_sv is a bit string in hexidecimal, needed for LPP -->
    */
    VOS_UINT32                          bitOpRequestedClockModel    : 1;
    VOS_UINT32                          bitOpRequestedOrbitModel    : 1;
    VOS_UINT32                          bitOpRequestedAddNavParam   : 1;
    VOS_UINT32                          bitSpare                    : 29;

    VOS_UINT8                           aucRequestedSv[CPOS_REQUESTED_SV_LEN];  /* ATTRIBE requested_sv */
    CPOS_CLOCK_MODEL_ENUM_UINT8         enRequestedClockModel;                  /* ATTRIBE requested_clock_model */
    CPOS_ORBIT_MODEL_ENUM_UINT8         enRequestedOrbitModel;                  /* ATTRIBE requested_orbit_model */
    CPOS_BOOL_ENUM_UINT8                enRequestedAddNavParam;                 /* ATTRIBE requested_add_nav_param */
    VOS_UINT8                           aucReserved[1];
}CPOS_REQUESTED_NAV_LIST_INFO_STRU;


typedef struct
{
    /*
    <!ELEMENT requested_nav_list (requested_nav_list_info*)>
    */
    VOS_UINT32                          ulCnt;
    CPOS_REQUESTED_NAV_LIST_INFO_STRU   astRequestedNavListInfo[CPOS_SAT_MAX_NUM];
}CPOS_REQUESTED_NAV_LIST_STRU;


typedef struct
{
    /*
    <!ELEMENT stored_nav_list (snl_week_or_day,snl_toe,snl_toe_limit,
        stored_sat_list_element*,requested_nav_list?)>
        <!ELEMENT snl_week_or_day (#PCDATA)>
        <!ELEMENT snl_toe (#PCDATA)>
        <!ELEMENT snl_toe_limit (#PCDATA)>
    */
    VOS_UINT32                          bitOpRequestedNavList    : 1;
    VOS_UINT32                          bitSpare                 : 31;

    VOS_UINT16                          usSnlWeekOrDay;                         /* ELEMENT snl_week_or_day */
    VOS_UINT8                           ucSnlToe;                               /* ELEMENT snl_toe */
    VOS_UINT8                           ucSnlToeLimit;                          /* ELEMENT snl_toe_limit */
    CPOS_STORED_SAT_LIST_STRU           stStoredSatList;                        /* ELEMENT stored_sat_list_element* */
    CPOS_REQUESTED_NAV_LIST_STRU        stRequestedNavList;                     /* ELEMENT requested_nav_list */

}CPOS_STORED_NAV_LIST_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_navigation_model_req (stored_nav_list?,requested_nav_list?) >
    */
    VOS_UINT32                          bitOpStoredNavList      : 1;
    VOS_UINT32                          bitOpRequestedNavList   : 1;
    VOS_UINT32                          bitSpare                : 30;

    CPOS_STORED_NAV_LIST_STRU           stStoredNavList;                        /* ELEMENT stored_nav_list */
    CPOS_REQUESTED_NAV_LIST_STRU        stRequestedNavList;                     /* ELEMENT requested_nav_list */
}CPOS_GNSS_NAV_MODEL_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT databit_reqsat_list (sat_id)>  <!-- sat_id  0..63 -->
    */
    VOS_UINT32                          ulCnt;
    VOS_UINT8                           aucSatId[CPOS_GNSS_SAT_MAX_NUM];        /* ELEMENT sat_id */
    /* VOS_UINT8                           ucReserved; */
}CPOS_DATABIT_REQSAT_LIST_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_databitassistance_req (gnss_TOD_req,gnss_TOD_fraq?,databit_interval,databit_reqsat_list+)>
        <!ATTLIST GNSS_databitassistance_req
            signal (GPS_L1|GPS_L1C|GPS_L2C|GPS_L5|SBAS_L1|GLO_G1|GLO_G2|GLO_G3|BDS_B1l) "GPS_L1"
            confidence_support (yes|no) #IMPLIED
            doppler_uncertainty_ext_support(yes|no) #IMPLIED>
        <!ELEMENT gnss_TOD_req (#PCDATA)>  <!-- 0..3599 -->
        <!ELEMENT gnss_TOD_fraq (#PCDATA)>  <!-- 0..599 -->
        <!ELEMENT databit_interval (#PCDATA)>  <!-- 0..15 -->
    */
    VOS_UINT32                          bitOpConfidenceSupport      : 1;
    VOS_UINT32                          bitOpDplUncertExtSupport    : 1;
    VOS_UINT32                          bitOpGnssTodFraq            : 1;
    VOS_UINT32                          bitSpare                    : 29;

    CPOS_SIGNAL_ID_ENUM_UINT8           enSignal;                               /* ATTLIST signal */
    CPOS_WHETHER_ENUM_UINT8             enConfidenceSupport;                    /* ATTLIST confidence_support */
    CPOS_WHETHER_ENUM_UINT8             enDplUncertExtSupport;                  /* ATTLIST doppler_uncertainty_ext_support */
    VOS_UINT8                           ucDataBitInterval;                      /* ELEMENT databit_interval */
    VOS_UINT16                          usGnssTodReq;                           /* ELEMENT gnss_TOD_req */
    VOS_UINT16                          usGnssTodFraq;                          /* ELEMENT gnss_TOD_fraq */
    CPOS_DATABIT_REQSAT_LIST_STRU       stDataBitReqSatList;                    /* ELEMENT databit_reqsat_list+ */
}CPOS_GNSS_DATABIT_ASSIST_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_assist_req_per_gnss (GNSS_time_model_req?,
        GNSS_navigation_model_req?,GNSS_integrity_req?,GNSS_acquisition_req?,
        GNSS_auxiliary_info_req?,GNSS_databitassistance_req?,GNSS_Almanac_req?,
        GNSS_UTC_model_req?,BDS_grid_model_req_r12?)>
        <!ATTLIST GNSS_assist_req_per_gnss
            gnss_id (SBAS|MGPS|QZSS|Glonass|GPS) #REQUIRED
            sbas_id (WASS|EGNOS|MSAS|GAGAN) #IMPLIED>
        <!ELEMENT GNSS_time_model_req EMPTY>
            <!-- For LPP gnss-TO-IDsReq is derived from gnss_id -->
            <!ATTLIST GNSS_time_model_req delta_T_req (false|true) #IMPLIED>
        <!ELEMENT GNSS_integrity_req EMPTY>
        <!ELEMENT GNSS_acquisition_req EMPTY>
            <!ATTLIST GNSS_acquisition_req signal (GPS_L1|GPS_L1C|GPS_L2C|GPS_L5|SBAS_L1|GLO_G1|GLO_G2|GLO_G3|BDS_B1I) "GPS_L1">
        <!ELEMENT GNSS_auxiliary_info_req EMPTY>
        <!ELEMENT GNSS_Almanac_req EMPTY>
            <!ATTLIST GNSS_Almanac_req model (7) #IMPLIED>
        <!ELEMENT GNSS_UTC_model_req EMPTY>
            <!ATTLIST GNSS_UTC_model_req model (5) #IMPLIED>
        <!ELEMENT BDS_grid_model_req_r12 EMPTY>
    */
    VOS_UINT32                          bitOpSbasId                 : 1;
    VOS_UINT32                          bitOpGnssTimeModelReq       : 1;
    VOS_UINT32                          bitOpGnssTimeModelReqDeltaT : 1;
    VOS_UINT32                          bitOpGnssNavModelReq        : 1;
    VOS_UINT32                          bitOpGnssIntergrityReq      : 1;
    VOS_UINT32                          bitOpGnssAcquReq            : 1;
    VOS_UINT32                          bitOpGnssAuxInfoReq         : 1;
    VOS_UINT32                          bitOpGnssDatabitAssitsReq   : 1;
    VOS_UINT32                          bitOpGnssAlmanacReq         : 1;
    VOS_UINT32                          bitOpGnssUtcModelReq        : 1;
    VOS_UINT32                          bitOpBdsGridModelReqR12     : 1;
    VOS_UINT32                          bitSpare                    : 21;

    CPOS_GNSS_ID_ENUM_UINT8             enGnssId;                               /* ELEMENT gnss_id */
    CPOS_SBAS_ID_ENUM_UINT8             enSbasId;                               /* ELEMENT sbas_id */
    CPOS_BOOL_ENUM_UINT8                enGnssTimeModelReqDeltaTReq;            /* ELEMENT GNSS_time_model_req */
    CPOS_SIGNAL_ID_ENUM_UINT8           enGnssAcquReqSignal;                    /* ELEMENT GNSS_acquisition_req */
    CPOS_GNSS_NAV_MODEL_REQ_STRU        stGnssNavModelReq;                      /* ELEMENT GNSS_navigation_model_req */
    CPOS_GNSS_DATABIT_ASSIST_REQ_STRU   stGnssDataBitAssistReq;                 /* ELEMENT GNSS_databitassistance_req */
    VOS_UINT8                           ucGnssAlmanacReqModel;                  /* ELEMENT GNSS_Almanac_req */
    VOS_UINT8                           ucGnssUtcModelReqModel;                 /* ELEMENT GNSS_UTC_model_req */
    VOS_UINT8                           aucReserved[2];
}CPOS_GNSS_ASSIST_REQ_PER_GNSS_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_ASSIST_REQ_PER_GNSS_STRU  astGnssAssistReqPerGnss[CPOS_GNSS_MAX_NUM];
}CPOS_GNSS_ASSIST_REQ_PER_GNSS_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_ref_time_req EMPTY>
       <!ATTLIST GNSS_ref_time_req
       time_req_pref (SBAS|MGPS|QZSS|Glonass|GPS) #IMPLIED
       tow_req (false|true) "false"
       leap_sec_req (false|true) #IMPLIED>
    */
    VOS_UINT8                           bitOpTimeReqPref    : 1;
    VOS_UINT8                           bitOpTowReq         : 1;
    VOS_UINT8                           bitOpLeapSecReq     : 1;
    VOS_UINT8                           bitSpare            : 5;

    CPOS_TIME_REQ_PREF_ENUM_UINT8       enTimeReqPref;                          /* ATTRIBUTE time_req_pref */
    CPOS_BOOL_ENUM_UINT8                enTowReq;                               /* ATTRIBUTE tow_req */
    CPOS_BOOL_ENUM_UINT8                enLeapSecReq;                           /* ATTRIBUTE leap_sec_req */
}CPOS_GNSS_REF_TIME_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_assist_req (GNSS_ref_time_req?,GNSS_ref_loc_req?,GNSS_ion_model_req?,GNSS_earth_orient_req?,GNSS_assist_req_per_gnss*)>
    */
    VOS_UINT32                                  bitOpGnssRefTimeReq         : 1;
    VOS_UINT32                                  bitOpGnssRefLocReq          : 1;
    VOS_UINT32                                  bitOpGnssIonModelReq        : 1;
    VOS_UINT32                                  bitOpGnssEarthOrientReq     : 1;
    VOS_UINT32                                  bitSpare                    : 28;

    CPOS_GNSS_REF_TIME_REQ_STRU                 stGnssRefTimeReq;
    CPOS_GNSS_ASSIST_REQ_PER_GNSS_ARRAY_STRU    stGnssAssistReqPerGnssArray;    /* ELEMENT GNSS_assist_req_per_gnss* */
}CPOS_GNSS_ASSIST_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_assist_req (phys_cell_id)>
        <phys_cell_id (#PCDATA)> <!-- 0..503 -->
    */
    VOS_UINT16                          usPhysCellId;                           /* ELEMENT phys_cell_id */
    VOS_UINT8                           aucReserved[2];
}CPOS_OTDOA_ASSIST_REQ_STRU;


typedef struct
{
    /*
    <!ELEMENT MSB_sup EMPTY>
        <!ATTLIST MSB_sup
            fta_sup CDATA "000"
            velocity_sup (true|false) "false">
    */
    VOS_UINT16                          usFtaSup;                               /* ELEMENT fta_sup, Binary */
    CPOS_BOOL_ENUM_UINT8                enVelocitySup;                          /* ELEMENT velocity_sup */
    VOS_UINT8                           aucReserved[1];
}CPOS_MSB_SUP_STRU;


typedef CPOS_MSB_SUP_STRU               CPOS_STANDALONE_SUP_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_Method (MSA_sup?,MSB_sup?,Standalone_sup?)>
        <!ELEMENT MSA_sup EMPTY>
            <!ATTLIST MSA_sup fta_sup CDATA "000">
        <!-- as LPP fta-MeasSupport -->
        <!ELEMENT Standalone_sup EMPTY>
            <!ATTLIST Standalone_sup
                fta_sup CDATA "000"
                velocity_sup (true|false) "false">
        <!-- as LPP fta-MeasSupport -->
    */
    VOS_UINT32                          bitOpMsaSup         : 1;
    VOS_UINT32                          bitOpMsbSup         : 1;
    VOS_UINT32                          bitOpStandaloneSup  : 1;
    VOS_UINT32                          bitSpare            : 29;

    VOS_UINT16                          usMsaSupFtaSup;                         /* ELEMENT MSA_sup */
    VOS_UINT8                           aucReserved[2];
    CPOS_MSB_SUP_STRU                   stMsbSup;                               /* ELEMENT MSB_sup */
    CPOS_STANDALONE_SUP_STRU            stStandaloneSup;                        /* ELEMENT Standalone_sup */
}CPOS_GPS_METHOD_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_Method (MSA_sup?,MSB_sup?,Standalone_sup?)>
        <!ATTLIST GNSS_Method supported_signal (GPS_L1|GPS_L1C|GPS_L2C|GPS_L5|
            SBAS_L1|GLO_G1|GLO_G2|GLO_G3|BDS_B1I) "GPS_L1">
    */
    VOS_UINT32                          bitOpMsaSup         : 1;
    VOS_UINT32                          bitOpMsbSup         : 1;
    VOS_UINT32                          bitOpStandaloneSup  : 1;
    VOS_UINT32                          bitSpare            : 29;

    CPOS_SIGNAL_ID_ENUM_UINT8           enSupportedSignal;                      /* ATTRIBE supported_signal */
    VOS_UINT8                           aucReserved[1];
    VOS_UINT16                          usMsaSup;                               /* ELEMENT MSA_sup */
    CPOS_MSB_SUP_STRU                   stMsbSup;                               /* ELEMENT MSB_sup */
    CPOS_STANDALONE_SUP_STRU            stStandaloneSup;                        /* ELEMENT Standalone_sup */
}CPOS_GNSS_METHOD_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_METHOD_STRU               astGnssMethod[CPOS_GNSS_MAX_NUM];
}CPOS_GNSS_METHOD_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GPS_aid (almanac_sup?,UTC_model_sup?,ionospheric_sup?,
        nav_model_sup?,DGPS_sup?,ref_loc_sup?,ref_time_sup?,
        acquisition_sup?,rt_integr_sup?)>
        <!ELEMENT almanac_sup EMPTY>
        <!ELEMENT UTC_model_sup EMPTY>
        <!ELEMENT ionospheric_sup EMPTY>
        <!ELEMENT nav_model_sup EMPTY>
        <!ELEMENT DGPS_sup EMPTY>
        <!ELEMENT ref_loc_sup EMPTY>
        <!ELEMENT ref_time_sup EMPTY>
        <!ELEMENT acquisition_sup EMPTY>
        <!ELEMENT rt_integr_sup EMPTY>
    */
    VOS_UINT32                          bitOpAlmanacSup         : 1;
    VOS_UINT32                          bitOpUtcModelSup        : 1;
    VOS_UINT32                          bitOpIonosphericSup     : 1;
    VOS_UINT32                          bitOpNavModelSup        : 1;
    VOS_UINT32                          bitOpDgpsSup            : 1;
    VOS_UINT32                          bitOpRefLocSup          : 1;
    VOS_UINT32                          bitOpRefTimeSup         : 1;
    VOS_UINT32                          bitOpAcquisitionSup     : 1;
    VOS_UINT32                          bitOpRtIntegrSup        : 1;
    VOS_UINT32                          bitSpare                : 23;
}CPOS_GPS_AID_STRU;


typedef struct
{
    /*
    <!ELEMENT orbit_sup EMPTY>
        <!ATTLIST orbit_sup model (2|3|4|5|6) #REQUIRED>
        <!-- 2=GPS NAV, 3=GPS CNAV, 4=Glonass, 5=SBAS, 6=BDS -->
    */
    VOS_UINT8                           ucCnt;
    CPOS_ORBIT_MODEL_ENUM_UINT8         aenModel[CPOS_MODEL_MAX_NUM];
    VOS_UINT8                           aucReserved[2];
}CPOS_ORBIT_SUP_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT clock_sup EMPTY>
        <!ATTLIST clock_sup model (2|3|4|5|6) #REQUIRED>
    */
    VOS_UINT8                           ucCnt;
    CPOS_CLOCK_MODEL_ENUM_UINT8         aenModel[CPOS_MODEL_MAX_NUM];
    VOS_UINT8                           aucReserved[2];
}CPOS_CLOCK_SUP_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_aid_for_one_gnss (orbit_sup*,clock_sup*,acquisition_sup?,
        rt_integr_sup?,GNSS_auxiliary_info_sup?,GNSS_databitassistance_sup?,
        GNSS_almanac_sup?,GNSS_UTC_model_sup?,BDS_grid_model_sup?)>
        <!ATTLIST GNSS_aid_for_one_gnss
            gnss_id (SBAS|MGPS|QZSS|Glonass|GPS|BDS) #IMPLIED
            sbas_id (WASS|EGNOS|MSAS|GAGAN) #IMPLIED>
        <!ELEMENT GNSS_auxiliary_info_sup EMPTY>
        <!ELEMENT GNSS_databitassistance_sup EMPTY>
        <!ELEMENT GNSS_almanac_sup (almanac_model)>
            <!ELEMENT almanac_model (#PCDATA)>
            <!--
                Integer corresponds to bit string 1-8 where bit at
                position if set, means particular almanac model is
                supported -Cbit6 set means BDS almanac model is supported.
            -->
        <!ELEMENT GNSS_UTC_model_sup (utc_model)>
            <!ELEMENT utc_model (#PCDATA)>
            <!--
                Integer corresponds to bit string 1-8 where bit at
                position if set, means particular UTC model is
                supported -Cbit4 set means BDS UTC model is supported
            -->
        <!ELEMENT BDS_grid_model_sup EMPTY>
    */
    VOS_UINT32                          bitOpGnssId                 : 1;
    VOS_UINT32                          bitOpSbasId                 : 1;
    VOS_UINT32                          bitOpAcquisitionSup         : 1;
    VOS_UINT32                          bitOpRtIntegrSup            : 1;
    VOS_UINT32                          bitOpGnssAuxInfoSup         : 1;
    VOS_UINT32                          bitOpGnssDatabitAssitsSup   : 1;
    VOS_UINT32                          bitOpGnssAlmanacReq         : 1;
    VOS_UINT32                          bitOpGnssUtcModelReq        : 1;
    VOS_UINT32                          bitOpBdsGridModelSup        : 1;
    VOS_UINT32                          bitSpare                    : 23;

    CPOS_GNSS_ID_ENUM_UINT8             enGnssId;                               /* ATTRIBE gnss_id */
    CPOS_SBAS_ID_ENUM_UINT8             enSbasId;                               /* ATTRIBE sbas_id */
    VOS_UINT8                           ucGnssAlmanacSupModel;                  /* ELEMENT GNSS_almanac_sup */
    VOS_UINT8                           ucGnssUtcModelSupModel;                 /* ELEMENT GNSS_UTC_model_sup */

    CPOS_ORBIT_SUP_ARRAY_STRU           stOrbitSupArray;                        /* ELEMENT orbit_sup* */
    CPOS_CLOCK_SUP_ARRAY_STRU           stClockSupArray;                        /* ELEMENT clock_sup* */
}CPOS_GNSS_AID_FOR_ONE_GNSS_STRU;


typedef struct
{
    VOS_UINT32                          ulCnt;
    CPOS_GNSS_AID_FOR_ONE_GNSS_STRU     astGnssAidForOneGnss[CPOS_GNSS_MAX_NUM];
}CPOS_GNSS_AID_FOR_ONE_GNSS_ARRAY_STRU;


typedef struct
{
    /*
    <!ELEMENT GNSS_aid (ionospheric_sup?,ref_loc_sup?,ref_time_sup?,
        earth_orient_sup?,GNSS_aid_for_one_gnss*)>
        <!ELEMENT earth_orient_sup EMPTY>
    */
    VOS_UINT32                              bitOpIonosphericSup     : 1;
    VOS_UINT32                              bitOpRefLocSup          : 1;
    VOS_UINT32                              bitOpRefTimeSup         : 1;
    VOS_UINT32                              bitOpEarthOrientSup     : 1;
    VOS_UINT32                              bitSpare                : 28;

    CPOS_GNSS_AID_FOR_ONE_GNSS_ARRAY_STRU   stGnssAidForOneGnssArray;           /* ELEMENT GNSS_aid_for_one_gnss */
}CPOS_GNSS_AID_STRU;


typedef struct
{
    /*
    <!ELEMENT supported_band_list_EUTRA (band_EUTRA+)>
    */
    VOS_UINT32                          ulCnt;
    VOS_UINT16                          ausBandEutra[CPOS_BAND_MAX_NUM];
}CPOS_SUPPORTED_BAND_LIST_EUTRA_STRU;


typedef struct
{
    /*
    <!ELEMENT OTDOA_capability (otdoa_mode,supported_band_list_EUTRA,interFreqRSTDmeasurement-r10?)>
        <!ELEMENT otdoa_mode #PCDATA>
        <!--
            Integer corresponds to bit string 1-8 where bit at position if set,
            means particular OTDOA mode is supported bit0 -Cue-assisted supported
        -->
        <!ELEMENT interFreqRSTDmeasurement-r10 EMPTY>
    */
    VOS_UINT8                           bitOpInterFreqRstdMeaR10    : 1;
    VOS_UINT8                           bitSpare                    : 7;

    VOS_UINT8                           ucOtdoaMode;                            /* ELEMENT otdoa_mode */
    VOS_UINT8                           aucReserved[2];
    CPOS_SUPPORTED_BAND_LIST_EUTRA_STRU stSupportedBandListEutra;               /* ELEMENT supported_band_list_EUTRA */
}CPOS_OTDOA_CAPABILITY_STRU;


typedef struct
{
    /*
    <!ELEMENT ECID_capability (ECID_measSupported)>
        <!ELEMENT ECID_measSupported (#PCDATA)>
        <!--
            Integer corresponds to bit string 1-8 where bit at position if set,
            means particular ECID mode is supported bit0-Crsrp Supported,
            bit1-rsrq Supported,bit2-ueRxTx Supported
        -->
    */
    VOS_UINT8                           ucEcidMeasSupported;                    /* ELEMENT ECID_measSupported */
    VOS_UINT8                           aucReserved[3];
}CPOS_ECID_CAPABILITY_STRU;


typedef struct
{
    /*
    <!ELEMENT capabilities (GPS_method?,GNSS_method*,GPS_aid?,GNSS_aid?,
        OTDOA_capability?,ECID_capability?)>
    */
    VOS_UINT32                          bitOpGpsMethod          : 1;
    VOS_UINT32                          bitOpGpsAid             : 1;
    VOS_UINT32                          bitOpGnssAid            : 1;
    VOS_UINT32                          bitOpOtdoaCapability    : 1;
    VOS_UINT32                          bitOpEcidCapability     : 1;
    VOS_UINT32                          bitSpare                : 27;

    CPOS_GPS_METHOD_STRU                stGpsMethod;                            /* ELEMENT GPS_method */
    CPOS_GNSS_METHOD_ARRAY_STRU         stGnssMethodArray;                      /* ELEMENT GNSS_method* */
    CPOS_GPS_AID_STRU                   stGpsAid;                               /* ELEMENT GPS_aid */
    CPOS_GNSS_AID_STRU                  stGnssAid;                              /* ELEMENT GNSS_aid */
    CPOS_OTDOA_CAPABILITY_STRU          stOtdoaCapability;                      /* ELEMENT OTDOA_capability */
    CPOS_ECID_CAPABILITY_STRU           stEcidCapability;                       /* ELEMENT ECID_capability */
}CPOS_CAPABILITIES_STRU;


typedef struct
{
    /*
    <!ELEMENT msg EMPTY>
       <!ATTLIST msg status (assist_data_delivered) #REQUIRED>
    */
    VOS_CHAR                            aucStatus[CPOS_MSG_STATUS_MAX_LEN];     /* ATTRIBE status */
}CPOS_MSG_STRU;


typedef struct
{
    /*
    <!ELEMENT pos_err (err_reason,GPS_assist_req?,ECID_meas_error_cause?,GNSS_assist_req?)>
        <!ELEMENT err_reason EMPTY>
            <!ATTLIST err_reason literal (undefined_error|
                not_enough_gps_satellites|gps_assist_data_missing|
                gnss_assist_data_missing|not_enough_gnss_satellites|
                OTDOA_undefined|OTDOA_assistance_data_missing|
                OTDOA_unable_to_measure_referencecell|
                OTDOA_unable_to_measure_any_neighbourcell|
                OTDOA_attempted_but_unable_to_measure_some_neighbourcells|
                ECID_undefined|ECID_requested_measurement_not_available|
                ECID_not_all_requested_measurements_possible) #REQUIRED>
        <!ELEMENT ECID_meas_error_cause (#PCDATA)>
        <!--
            Integer corresponds to bit string 1-8 where bit at position
            represents ECID error cause. Only applicable, when value of
            err_reason = 'ECID_requested_measurements_not_available'
            bit0-CrsrpMeasurementNotPossible, bit1- rsrqMeasurementNotPossible,
            bit2-ueRxTxMeasurementNotPossible
        -->
    */
    VOS_UINT32                          bitOpGpsAssistReq       : 1;
    VOS_UINT32                          bitOpEcidMeasErrorCause : 1;
    VOS_UINT32                          bitOpGnssAssistReq      : 1;
    VOS_UINT32                          bitSpare                : 29;

    CPOS_ERR_REASON_ENUM_UINT32         enErrReason;                            /* ELEMENT err_reason */
    CPOS_GPS_ASSIST_REQ_STRU            stGpsAssistReq;                         /* ELEMENT GPS_assist_req */
    VOS_UINT8                           ucEcidMeasErrorCause;                   /* ELEMENT ECID_meas_error_cause */
    VOS_UINT8                           aucReserved[3];
    CPOS_GNSS_ASSIST_REQ_STRU           stGnssAssistReq;                        /* ELEMENT GNSS_assist_req */
}CPOS_POS_ERR_STRU;


typedef struct
{
    /*
    <!ELEMENT pos (location|GNSS_provided_location_information|assist_data|
        pos_meas|pos_meas_req|GPS_meas|OTDOA_ECID_req|OTDOA_meas|ECID_meas|
        GNSS_meas*|GPS_assist_req|GNSS_assist_req|OTDOA_assist_req|capability_req|
        capabilities|msg|pos_err|reset_assist_data|Strobe)>
        <!ATTLIST pos protocol (RRLP|RRC|LPP) #IMPLIED transaction_id CDATA #IMPLIED>
        <!ELEMENT capability_req EMPTY>
        <!ELEMENT reset_assist_data EMPTY>
        <!-- Reset UE positioning stored AGNSS/OTDOA assistance data -->
        <!ELEMENT Strobe (#PCDATA)>
        <!--  Triggers the generation of a strobe -->
    */
    VOS_UINT32                          bitOpProtocol       : 1;
    VOS_UINT32                          bitOpTransactionId  : 1;
    VOS_UINT32                          bitSpare            : 30;

    VOS_UINT32                          ulTransactionId;                        /* ATTRIBUTE transaction_id */
    CPOS_POS_PROTOCOL_ENUM_UINT8        enProtocol;                             /* ATTRIBUTE protocol */
    VOS_UINT8                           aucReserved[2];

    CPOS_POS_ELEMENT_TYPE_ENUM_UINT8    enChoice;                               /*_H2ASN_MsgChoice_Export CPOS_POS_ELEMENT_TYPE_ENUM_UINT8*/
    VOS_UINT8                           aucContent[4];

    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          CPOS_POS_ELEMENT_TYPE_ENUM_UINT8
    ****************************************************************************/
#if 0
    union
    {
        CPOS_LOCATION_STRU              stLocation;                             /* ELEMENT location */
        CPOS_GNSS_LOCATION_INFO_STRU    stGnssLocationInfo;                     /* ELEMENT GNSS_provided_location_information */
        CPOS_ASSIST_DATA_STRU           stAssistData;                           /* ELEMENT assist_data */
        CPOS_POS_MEAS_STRU              stPosMeas;                              /* ELEMENT pos_meas */
        CPOS_POS_MEAS_REQ_STRU          stPosMeasReq;                           /* ELEMENT pos_meas_req */
        CPOS_GPS_MEAS_STRU              stGpsMeas;                              /* ELEMENT GPS_meas */
        CPOS_OTDOA_ECID_REQ_STRU        stOtdoaEcidReq;                         /* ELEMENT OTDOA_ECID_req */
        CPOS_OTDOA_MEAS_STRU            stOtdoaMeas;                            /* ELEMENT OTDOA_meas */
        CPOS_ECID_MEAS_STRU             stEcidMeas;                             /* ELEMENT ECID_meas */
        CPOS_GNSS_MEAS_ARRAY_STRU       stGnssMeasArray;                        /* ELEMENT GNSS_meas* */
        CPOS_GPS_ASSIST_REQ_STRU        stGpsAssistReq;                         /* ELEMENT GPS_assist_req */
        CPOS_GNSS_ASSIST_REQ_STRU       stGnssAssistReq;                        /* ELEMENT GNSS_assist_req */
        CPOS_OTDOA_ASSIST_REQ_STRU      stOtdoaAssistReq;                       /* ELEMENT OTDOA_assist_req */
        CPOS_CAPABILITIES_STRU          stCapability;                           /* ELEMENT capabilities */
        CPOS_MSG_STRU                   stMsg;                                  /* ELEMENT msg */
        CPOS_POS_ERR_STRU               stPosErr;                               /* ELEMENT pos_err */
        VOS_UINT32                      ulStrobe;                               /* ELEMENT Strobe */
    } u;
#endif
}CPOS_POS_PARA_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/




#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __CPOSPARADEF_H___ */

