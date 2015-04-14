cmake_minimum_required(VERSION 2.8)

project(App)

add_definitions( -D_CRT_SECURE_NO_WARNINGS )

include_directories(
   ${PROJECT_SOURCE_DIR}/src
   ${PROJECT_SOURCE_DIR}/external/Assimp/include
   ${PROJECT_SOURCE_DIR}/external/ALVAR/include
   ${PROJECT_SOURCE_DIR}/external/glew/include
   ${PROJECT_SOURCE_DIR}/external/glm
   ${PROJECT_SOURCE_DIR}/external/SDL/include
   ${PROJECT_SOURCE_DIR}/external/SDL2_ttf/include
   ${PROJECT_SOURCE_DIR}/external/opencv/include
   ${PROJECT_SOURCE_DIR}/external/opencv/include/opencv
)

link_directories(
   ${PROJECT_SOURCE_DIR}/external/Assimp/lib
   ${PROJECT_SOURCE_DIR}/external/ALVAR/lib/vc12
   ${PROJECT_SOURCE_DIR}/external/glew/lib
   ${PROJECT_SOURCE_DIR}/external/SDL/lib
   ${PROJECT_SOURCE_DIR}/external/SDL2_ttf/lib
   ${PROJECT_SOURCE_DIR}/external/opencv/lib/vc12
)

file( GLOB_RECURSE SRC_LIST src/*.hpp src/*.cpp)

add_executable( ${PROJECT_NAME} ${SRC_LIST} )

target_link_libraries( ${PROJECT_NAME}
   alvar200d
   opencv_calib3d249d
   opencv_contrib249d
   opencv_core249d
   opencv_features2d249d
   opencv_flann249d
   opencv_gpu249d
   opencv_highgui249d
   opencv_imgproc249d
   opencv_legacy249d
   opencv_ml249d
   opencv_nonfree249d
   opencv_objdetect249d
   opencv_ocl249d
   opencv_photo249d
   opencv_stitching249d
   opencv_superres249d
   opencv_ts249d
   opencv_video249d
   opencv_videostab249d
   assimp
   glew32
   SDL_ttf
   SDL
   SDLmain
   glu32
   opengl32
)
