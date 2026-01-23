# ofxCv - OpenCV 4.x wrapper for openFrameworks (standalone)
#
# This addon includes OpenCV 4.x libraries and does NOT depend on ofxOpenCv.
# Run scripts/build_opencv_macos_arm64.sh to build OpenCV libraries.

meta:
	ADDON_NAME = ofxCv
	ADDON_DESCRIPTION = OpenCV 4.x wrapper for openFrameworks (standalone, no ofxOpenCv dependency)
	ADDON_AUTHOR = Kyle McDonald
	ADDON_TAGS = "computer vision" "opencv" "image processing" "opencv4" "dnn"
	ADDON_URL = https://github.com/kylemcdonald/ofxcv

common:
	# NO ofxOpenCv dependency - this addon is standalone
	# ADDON_DEPENDENCIES =

	# Include paths
	ADDON_INCLUDES = src
	ADDON_INCLUDES += libs/ofxCv/include
	ADDON_INCLUDES += libs/opencv/include/opencv4
	ADDON_INCLUDES += libs/CLD/include/CLD

	# Compiler flags
	ADDON_CPPFLAGS = -DOFXCV_OPENCV4

osx:
	# OpenCV static libraries for macOS
	ADDON_LIBS = libs/opencv/lib/osx-arm64/libopencv_core.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_imgproc.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_objdetect.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_video.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_calib3d.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_features2d.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_flann.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_dnn.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libopencv_imgcodecs.a

	# 3rdparty libraries (built with OpenCV)
	# Note: libzlib, liblibpng, liblibjpeg-turbo are provided by openFrameworks (FreeImage)
	# so we don't link them here to avoid duplicate symbol errors
	ADDON_LIBS += libs/opencv/lib/osx-arm64/liblibprotobuf.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libittnotify.a
	ADDON_LIBS += libs/opencv/lib/osx-arm64/libtegra_hal.a

	# System frameworks required by OpenCV
	ADDON_FRAMEWORKS = Accelerate
	ADDON_FRAMEWORKS += CoreGraphics
	ADDON_FRAMEWORKS += CoreVideo
	ADDON_FRAMEWORKS += OpenCL

linux64:
	# Use system OpenCV via pkg-config
	ADDON_PKG_CONFIG_LIBRARIES = opencv4

	# Or use static libraries:
	# ADDON_LIBS = libs/opencv/lib/linux64/libopencv_core.a
	# ADDON_LIBS += libs/opencv/lib/linux64/libopencv_imgproc.a
	# ... etc

vs:
	# Windows Visual Studio
	ADDON_INCLUDES += libs/opencv/include/opencv4

	# For static linking:
	# ADDON_LIBS = libs/opencv/lib/vs/x64/opencv_world4100.lib

	# For dynamic linking:
	# ADDON_DLLS_TO_COPY = libs/opencv/lib/vs/x64/opencv_world4100.dll
