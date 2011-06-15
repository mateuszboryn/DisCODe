# Include the directory itself as a path to include directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE (OpenCL REQUIRED)
include_directories( ${OPENCL_INCLUDE_DIR} )
include_directories( ${OPENCL_OCLUTILS_INCLUDE_DIR} )
include_directories( ${OPENCL_SHRUTILS_INCLUDE_DIR} )

# Find OpenCV library files
FIND_PACKAGE( OpenCV REQUIRED )

# Create a variable containing all .cpp files:
FILE(GLOB files *.cpp)

# Create an executable file from sources:
#add_library(oclUtil STATIC IMPORTED)

#set_property(TARGET oclUtil PROPERTY IMPORTED_LOCATION )

#add_library(shrUtil STATIC IMPORTED)
#set_property(TARGET shrUtil PROPERTY IMPORTED_LOCATION )

LINK_DIRECTORIES(/home/szymszy/NVIDIA_GPU_Computing_SDK/OpenCL/common/lib)
LINK_DIRECTORIES(/home/szymszy/NVIDIA_GPU_Computing_SDK/shared/lib)
LINK_DIRECTORIES(/home/szymszy/NVIDIA_GPU_Computing_SDK/shared/lib/linux)




ADD_LIBRARY(MSZ_OpenCL_Segmentation SHARED ${files})

TARGET_LINK_LIBRARIES(MSZ_OpenCL_Segmentation  ${OpenCV_LIBS} ${DisCODe_LIBRARIES} ${OPENCL_LIBRARIES} TypesBlobs -loclUtil_i386 -lshrutil_i386 )


INSTALL_PROCESSOR(MSZ_OpenCL_Segmentation)

FILE(GLOB clfiles *.cl)
INSTALL(
	FILES ${clfiles}
	DESTINATION bin
)
