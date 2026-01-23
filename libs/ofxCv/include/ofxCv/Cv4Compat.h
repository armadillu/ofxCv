#pragma once

// OpenCV 4.x Compatibility Header
// Provides backward compatibility macros for migrating from OpenCV 2.x/3.x
// This file is included automatically by Utilities.h

#include <opencv2/core/version.hpp>

// Version detection
#if CV_VERSION_MAJOR >= 4
    #define OFXCV_USING_OPENCV4 1
#else
    #define OFXCV_USING_OPENCV4 0
#endif

#if OFXCV_USING_OPENCV4

// =============================================================================
// Color Conversion Codes
// =============================================================================

#ifndef CV_BGR2GRAY
    #define CV_BGR2GRAY cv::COLOR_BGR2GRAY
#endif
#ifndef CV_RGB2GRAY
    #define CV_RGB2GRAY cv::COLOR_RGB2GRAY
#endif
#ifndef CV_GRAY2BGR
    #define CV_GRAY2BGR cv::COLOR_GRAY2BGR
#endif
#ifndef CV_GRAY2RGB
    #define CV_GRAY2RGB cv::COLOR_GRAY2RGB
#endif
#ifndef CV_BGR2RGB
    #define CV_BGR2RGB cv::COLOR_BGR2RGB
#endif
#ifndef CV_RGB2BGR
    #define CV_RGB2BGR cv::COLOR_RGB2BGR
#endif
#ifndef CV_BGR2HSV
    #define CV_BGR2HSV cv::COLOR_BGR2HSV
#endif
#ifndef CV_HSV2BGR
    #define CV_HSV2BGR cv::COLOR_HSV2BGR
#endif
#ifndef CV_RGB2HSV
    #define CV_RGB2HSV cv::COLOR_RGB2HSV
#endif
#ifndef CV_HSV2RGB
    #define CV_HSV2RGB cv::COLOR_HSV2RGB
#endif
#ifndef CV_BGR2Lab
    #define CV_BGR2Lab cv::COLOR_BGR2Lab
#endif
#ifndef CV_Lab2BGR
    #define CV_Lab2BGR cv::COLOR_Lab2BGR
#endif
#ifndef CV_BGR2HLS
    #define CV_BGR2HLS cv::COLOR_BGR2HLS
#endif
#ifndef CV_HLS2BGR
    #define CV_HLS2BGR cv::COLOR_HLS2BGR
#endif
#ifndef CV_BGR2Luv
    #define CV_BGR2Luv cv::COLOR_BGR2Luv
#endif
#ifndef CV_Luv2BGR
    #define CV_Luv2BGR cv::COLOR_Luv2BGR
#endif
#ifndef CV_RGB2RGBA
    #define CV_RGB2RGBA cv::COLOR_RGB2RGBA
#endif
#ifndef CV_RGBA2RGB
    #define CV_RGBA2RGB cv::COLOR_RGBA2RGB
#endif
#ifndef CV_BGRA2GRAY
    #define CV_BGRA2GRAY cv::COLOR_BGRA2GRAY
#endif
#ifndef CV_RGBA2GRAY
    #define CV_RGBA2GRAY cv::COLOR_RGBA2GRAY
#endif
#ifndef CV_RGB2BGRA
    #define CV_RGB2BGRA cv::COLOR_RGB2BGRA
#endif
#ifndef CV_RGBA2BGR
    #define CV_RGBA2BGR cv::COLOR_RGBA2BGR
#endif
#ifndef CV_BGRA2RGBA
    #define CV_BGRA2RGBA cv::COLOR_BGRA2RGBA
#endif
#ifndef CV_GRAY2RGBA
    #define CV_GRAY2RGBA cv::COLOR_GRAY2RGBA
#endif

// BGR565/BGR555 conversions
#ifndef CV_BGR5652BGR
    #define CV_BGR5652BGR cv::COLOR_BGR5652BGR
#endif
#ifndef CV_BGR5652RGB
    #define CV_BGR5652RGB cv::COLOR_BGR5652RGB
#endif
#ifndef CV_BGR5652BGRA
    #define CV_BGR5652BGRA cv::COLOR_BGR5652BGRA
#endif
#ifndef CV_BGR5652RGBA
    #define CV_BGR5652RGBA cv::COLOR_BGR5652RGBA
#endif
#ifndef CV_BGR5652GRAY
    #define CV_BGR5652GRAY cv::COLOR_BGR5652GRAY
#endif
#ifndef CV_BGR5552BGR
    #define CV_BGR5552BGR cv::COLOR_BGR5552BGR
#endif
#ifndef CV_BGR5552RGB
    #define CV_BGR5552RGB cv::COLOR_BGR5552RGB
#endif
#ifndef CV_BGR5552BGRA
    #define CV_BGR5552BGRA cv::COLOR_BGR5552BGRA
#endif
#ifndef CV_BGR5552RGBA
    #define CV_BGR5552RGBA cv::COLOR_BGR5552RGBA
#endif
#ifndef CV_BGR5552GRAY
    #define CV_BGR5552GRAY cv::COLOR_BGR5552GRAY
#endif

// XYZ conversions
#ifndef CV_BGR2XYZ
    #define CV_BGR2XYZ cv::COLOR_BGR2XYZ
#endif
#ifndef CV_RGB2XYZ
    #define CV_RGB2XYZ cv::COLOR_RGB2XYZ
#endif
#ifndef CV_XYZ2BGR
    #define CV_XYZ2BGR cv::COLOR_XYZ2BGR
#endif
#ifndef CV_XYZ2RGB
    #define CV_XYZ2RGB cv::COLOR_XYZ2RGB
#endif

// YCrCb conversions
#ifndef CV_BGR2YCrCb
    #define CV_BGR2YCrCb cv::COLOR_BGR2YCrCb
#endif
#ifndef CV_RGB2YCrCb
    #define CV_RGB2YCrCb cv::COLOR_RGB2YCrCb
#endif
#ifndef CV_YCrCb2BGR
    #define CV_YCrCb2BGR cv::COLOR_YCrCb2BGR
#endif
#ifndef CV_YCrCb2RGB
    #define CV_YCrCb2RGB cv::COLOR_YCrCb2RGB
#endif

// Lab/Luv additional conversions
#ifndef CV_RGB2Lab
    #define CV_RGB2Lab cv::COLOR_RGB2Lab
#endif
#ifndef CV_Lab2RGB
    #define CV_Lab2RGB cv::COLOR_Lab2RGB
#endif
#ifndef CV_RGB2Luv
    #define CV_RGB2Luv cv::COLOR_RGB2Luv
#endif
#ifndef CV_Luv2RGB
    #define CV_Luv2RGB cv::COLOR_Luv2RGB
#endif

// HLS additional conversions
#ifndef CV_RGB2HLS
    #define CV_RGB2HLS cv::COLOR_RGB2HLS
#endif
#ifndef CV_HLS2RGB
    #define CV_HLS2RGB cv::COLOR_HLS2RGB
#endif

// Bayer conversions
#ifndef CV_BayerGB2BGR
    #define CV_BayerGB2BGR cv::COLOR_BayerGB2BGR
#endif
#ifndef CV_BayerBG2RGB
    #define CV_BayerBG2RGB cv::COLOR_BayerBG2RGB
#endif
#ifndef CV_BayerGB2RGB
    #define CV_BayerGB2RGB cv::COLOR_BayerGB2RGB
#endif
#ifndef CV_BayerRG2RGB
    #define CV_BayerRG2RGB cv::COLOR_BayerRG2RGB
#endif
#ifndef CV_BayerBG2RGB_VNG
    #define CV_BayerBG2RGB_VNG cv::COLOR_BayerBG2RGB_VNG
#endif
#ifndef CV_BayerGB2RGB_VNG
    #define CV_BayerGB2RGB_VNG cv::COLOR_BayerGB2RGB_VNG
#endif
#ifndef CV_BayerRG2RGB_VNG
    #define CV_BayerRG2RGB_VNG cv::COLOR_BayerRG2RGB_VNG
#endif
#ifndef CV_BayerGR2RGB_VNG
    #define CV_BayerGR2RGB_VNG cv::COLOR_BayerGR2RGB_VNG
#endif
#ifndef CV_BayerGR2RGB
    #define CV_BayerGR2RGB cv::COLOR_BayerGR2RGB
#endif
#ifndef CV_BayerBG2BGR
    #define CV_BayerBG2BGR cv::COLOR_BayerBG2BGR
#endif
#ifndef CV_BayerGR2BGR
    #define CV_BayerGR2BGR cv::COLOR_BayerGR2BGR
#endif
#ifndef CV_BayerRG2BGR
    #define CV_BayerRG2BGR cv::COLOR_BayerRG2BGR
#endif

// Full range HSV/HLS conversions
#ifndef CV_BGR2HSV_FULL
    #define CV_BGR2HSV_FULL cv::COLOR_BGR2HSV_FULL
#endif
#ifndef CV_RGB2HSV_FULL
    #define CV_RGB2HSV_FULL cv::COLOR_RGB2HSV_FULL
#endif
#ifndef CV_BGR2HLS_FULL
    #define CV_BGR2HLS_FULL cv::COLOR_BGR2HLS_FULL
#endif
#ifndef CV_RGB2HLS_FULL
    #define CV_RGB2HLS_FULL cv::COLOR_RGB2HLS_FULL
#endif
#ifndef CV_HSV2BGR_FULL
    #define CV_HSV2BGR_FULL cv::COLOR_HSV2BGR_FULL
#endif
#ifndef CV_HSV2RGB_FULL
    #define CV_HSV2RGB_FULL cv::COLOR_HSV2RGB_FULL
#endif
#ifndef CV_HLS2BGR_FULL
    #define CV_HLS2BGR_FULL cv::COLOR_HLS2BGR_FULL
#endif
#ifndef CV_HLS2RGB_FULL
    #define CV_HLS2RGB_FULL cv::COLOR_HLS2RGB_FULL
#endif

// Linear RGB Lab/Luv conversions
#ifndef CV_LBGR2Lab
    #define CV_LBGR2Lab cv::COLOR_LBGR2Lab
#endif
#ifndef CV_LRGB2Lab
    #define CV_LRGB2Lab cv::COLOR_LRGB2Lab
#endif
#ifndef CV_LBGR2Luv
    #define CV_LBGR2Luv cv::COLOR_LBGR2Luv
#endif
#ifndef CV_LRGB2Luv
    #define CV_LRGB2Luv cv::COLOR_LRGB2Luv
#endif
#ifndef CV_Lab2LBGR
    #define CV_Lab2LBGR cv::COLOR_Lab2LBGR
#endif
#ifndef CV_Lab2LRGB
    #define CV_Lab2LRGB cv::COLOR_Lab2LRGB
#endif
#ifndef CV_Luv2LBGR
    #define CV_Luv2LBGR cv::COLOR_Luv2LBGR
#endif
#ifndef CV_Luv2LRGB
    #define CV_Luv2LRGB cv::COLOR_Luv2LRGB
#endif

// YUV conversions
#ifndef CV_BGR2YUV
    #define CV_BGR2YUV cv::COLOR_BGR2YUV
#endif
#ifndef CV_RGB2YUV
    #define CV_RGB2YUV cv::COLOR_RGB2YUV
#endif
#ifndef CV_YUV2BGR
    #define CV_YUV2BGR cv::COLOR_YUV2BGR
#endif
#ifndef CV_YUV2RGB
    #define CV_YUV2RGB cv::COLOR_YUV2RGB
#endif

// =============================================================================
// Contour Retrieval Modes
// =============================================================================

#ifndef CV_RETR_EXTERNAL
    #define CV_RETR_EXTERNAL cv::RETR_EXTERNAL
#endif
#ifndef CV_RETR_LIST
    #define CV_RETR_LIST cv::RETR_LIST
#endif
#ifndef CV_RETR_CCOMP
    #define CV_RETR_CCOMP cv::RETR_CCOMP
#endif
#ifndef CV_RETR_TREE
    #define CV_RETR_TREE cv::RETR_TREE
#endif

// =============================================================================
// Contour Approximation Methods
// =============================================================================

#ifndef CV_CHAIN_APPROX_NONE
    #define CV_CHAIN_APPROX_NONE cv::CHAIN_APPROX_NONE
#endif
#ifndef CV_CHAIN_APPROX_SIMPLE
    #define CV_CHAIN_APPROX_SIMPLE cv::CHAIN_APPROX_SIMPLE
#endif
#ifndef CV_CHAIN_APPROX_TC89_L1
    #define CV_CHAIN_APPROX_TC89_L1 cv::CHAIN_APPROX_TC89_L1
#endif
#ifndef CV_CHAIN_APPROX_TC89_KCOS
    #define CV_CHAIN_APPROX_TC89_KCOS cv::CHAIN_APPROX_TC89_KCOS
#endif

// =============================================================================
// Threshold Types
// =============================================================================

#ifndef CV_THRESH_BINARY
    #define CV_THRESH_BINARY cv::THRESH_BINARY
#endif
#ifndef CV_THRESH_BINARY_INV
    #define CV_THRESH_BINARY_INV cv::THRESH_BINARY_INV
#endif
#ifndef CV_THRESH_TRUNC
    #define CV_THRESH_TRUNC cv::THRESH_TRUNC
#endif
#ifndef CV_THRESH_TOZERO
    #define CV_THRESH_TOZERO cv::THRESH_TOZERO
#endif
#ifndef CV_THRESH_TOZERO_INV
    #define CV_THRESH_TOZERO_INV cv::THRESH_TOZERO_INV
#endif
#ifndef CV_THRESH_OTSU
    #define CV_THRESH_OTSU cv::THRESH_OTSU
#endif

// =============================================================================
// Adaptive Threshold
// =============================================================================

#ifndef CV_ADAPTIVE_THRESH_MEAN_C
    #define CV_ADAPTIVE_THRESH_MEAN_C cv::ADAPTIVE_THRESH_MEAN_C
#endif
#ifndef CV_ADAPTIVE_THRESH_GAUSSIAN_C
    #define CV_ADAPTIVE_THRESH_GAUSSIAN_C cv::ADAPTIVE_THRESH_GAUSSIAN_C
#endif

// =============================================================================
// Morphology Operations
// =============================================================================

#ifndef CV_MOP_OPEN
    #define CV_MOP_OPEN cv::MORPH_OPEN
#endif
#ifndef CV_MOP_CLOSE
    #define CV_MOP_CLOSE cv::MORPH_CLOSE
#endif
#ifndef CV_MOP_GRADIENT
    #define CV_MOP_GRADIENT cv::MORPH_GRADIENT
#endif
#ifndef CV_MOP_TOPHAT
    #define CV_MOP_TOPHAT cv::MORPH_TOPHAT
#endif
#ifndef CV_MOP_BLACKHAT
    #define CV_MOP_BLACKHAT cv::MORPH_BLACKHAT
#endif

// =============================================================================
// Drawing Constants
// =============================================================================

#ifndef CV_AA
    #define CV_AA cv::LINE_AA
#endif
#ifndef CV_FILLED
    #define CV_FILLED cv::FILLED
#endif

// =============================================================================
// Font Constants
// =============================================================================

#ifndef CV_FONT_HERSHEY_SIMPLEX
    #define CV_FONT_HERSHEY_SIMPLEX cv::FONT_HERSHEY_SIMPLEX
#endif
#ifndef CV_FONT_HERSHEY_PLAIN
    #define CV_FONT_HERSHEY_PLAIN cv::FONT_HERSHEY_PLAIN
#endif
#ifndef CV_FONT_HERSHEY_DUPLEX
    #define CV_FONT_HERSHEY_DUPLEX cv::FONT_HERSHEY_DUPLEX
#endif
#ifndef CV_FONT_HERSHEY_COMPLEX
    #define CV_FONT_HERSHEY_COMPLEX cv::FONT_HERSHEY_COMPLEX
#endif

// =============================================================================
// Calibration Flags
// =============================================================================

#ifndef CV_CALIB_USE_INTRINSIC_GUESS
    #define CV_CALIB_USE_INTRINSIC_GUESS cv::CALIB_USE_INTRINSIC_GUESS
#endif
#ifndef CV_CALIB_FIX_PRINCIPAL_POINT
    #define CV_CALIB_FIX_PRINCIPAL_POINT cv::CALIB_FIX_PRINCIPAL_POINT
#endif
#ifndef CV_CALIB_FIX_ASPECT_RATIO
    #define CV_CALIB_FIX_ASPECT_RATIO cv::CALIB_FIX_ASPECT_RATIO
#endif
#ifndef CV_CALIB_ZERO_TANGENT_DIST
    #define CV_CALIB_ZERO_TANGENT_DIST cv::CALIB_ZERO_TANGENT_DIST
#endif
#ifndef CV_CALIB_FIX_K1
    #define CV_CALIB_FIX_K1 cv::CALIB_FIX_K1
#endif
#ifndef CV_CALIB_FIX_K2
    #define CV_CALIB_FIX_K2 cv::CALIB_FIX_K2
#endif
#ifndef CV_CALIB_FIX_K3
    #define CV_CALIB_FIX_K3 cv::CALIB_FIX_K3
#endif
#ifndef CV_CALIB_FIX_K4
    #define CV_CALIB_FIX_K4 cv::CALIB_FIX_K4
#endif
#ifndef CV_CALIB_FIX_K5
    #define CV_CALIB_FIX_K5 cv::CALIB_FIX_K5
#endif
#ifndef CV_CALIB_FIX_K6
    #define CV_CALIB_FIX_K6 cv::CALIB_FIX_K6
#endif
#ifndef CV_CALIB_CB_ADAPTIVE_THRESH
    #define CV_CALIB_CB_ADAPTIVE_THRESH cv::CALIB_CB_ADAPTIVE_THRESH
#endif
#ifndef CV_CALIB_CB_NORMALIZE_IMAGE
    #define CV_CALIB_CB_NORMALIZE_IMAGE cv::CALIB_CB_NORMALIZE_IMAGE
#endif
#ifndef CV_CALIB_CB_FILTER_QUADS
    #define CV_CALIB_CB_FILTER_QUADS cv::CALIB_CB_FILTER_QUADS
#endif
#ifndef CV_CALIB_CB_FAST_CHECK
    #define CV_CALIB_CB_FAST_CHECK cv::CALIB_CB_FAST_CHECK
#endif

// =============================================================================
// TermCriteria
// =============================================================================

#ifndef CV_TERMCRIT_ITER
    #define CV_TERMCRIT_ITER cv::TermCriteria::COUNT
#endif
#ifndef CV_TERMCRIT_EPS
    #define CV_TERMCRIT_EPS cv::TermCriteria::EPS
#endif

// =============================================================================
// Norm Types
// =============================================================================

#ifndef CV_L1
    #define CV_L1 cv::NORM_L1
#endif
#ifndef CV_L2
    #define CV_L2 cv::NORM_L2
#endif
#ifndef CV_C
    #define CV_C cv::NORM_INF
#endif

// =============================================================================
// Distance Types
// =============================================================================

#ifndef CV_DIST_L1
    #define CV_DIST_L1 cv::DIST_L1
#endif
#ifndef CV_DIST_L2
    #define CV_DIST_L2 cv::DIST_L2
#endif
#ifndef CV_DIST_C
    #define CV_DIST_C cv::DIST_C
#endif

// =============================================================================
// Interpolation Methods
// =============================================================================

#ifndef CV_INTER_NEAREST
    #define CV_INTER_NEAREST cv::INTER_NEAREST
#endif
#ifndef CV_INTER_LINEAR
    #define CV_INTER_LINEAR cv::INTER_LINEAR
#endif
#ifndef CV_INTER_AREA
    #define CV_INTER_AREA cv::INTER_AREA
#endif
#ifndef CV_INTER_CUBIC
    #define CV_INTER_CUBIC cv::INTER_CUBIC
#endif
#ifndef CV_INTER_LANCZOS4
    #define CV_INTER_LANCZOS4 cv::INTER_LANCZOS4
#endif

// =============================================================================
// Border Types
// =============================================================================

#ifndef CV_BORDER_CONSTANT
    #define CV_BORDER_CONSTANT cv::BORDER_CONSTANT
#endif
#ifndef CV_BORDER_REPLICATE
    #define CV_BORDER_REPLICATE cv::BORDER_REPLICATE
#endif
#ifndef CV_BORDER_REFLECT
    #define CV_BORDER_REFLECT cv::BORDER_REFLECT
#endif
#ifndef CV_BORDER_WRAP
    #define CV_BORDER_WRAP cv::BORDER_WRAP
#endif
#ifndef CV_BORDER_REFLECT_101
    #define CV_BORDER_REFLECT_101 cv::BORDER_REFLECT_101
#endif
#ifndef CV_BORDER_DEFAULT
    #define CV_BORDER_DEFAULT cv::BORDER_DEFAULT
#endif

#endif // OFXCV_USING_OPENCV4
