# ofxCv - OpenCV 4.x wrapper for openFrameworks
#
# Reuse the OpenCV headers and libraries bundled with openFrameworks' ofxOpenCv.

meta:
	ADDON_NAME = ofxCv
	ADDON_DESCRIPTION = OpenCV 4.x wrapper for openFrameworks using bundled ofxOpenCv libraries
	ADDON_AUTHOR = Kyle McDonald
	ADDON_TAGS = "computer vision" "opencv" "image processing" "opencv4" "dnn"
	ADDON_URL = https://github.com/kylemcdonald/ofxcv

common:
	ADDON_DEPENDENCIES = ofxOpenCv

	# Include paths
	ADDON_INCLUDES = src
	ADDON_INCLUDES += libs/ofxCv/include
	ADDON_INCLUDES += libs/CLD/include/CLD

	# Compiler flags
	ADDON_CPPFLAGS = -DOFXCV_OPENCV4

osx:
	# OpenCV headers and libraries are supplied by the ofxOpenCv dependency.

linux64:
	# ofxOpenCv supplies the system OpenCV dependency via pkg-config.
	# ... etc

vs:
	# Windows Visual Studio
