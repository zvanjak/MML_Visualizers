include(C:/Dev/Qt/Examples/Qt-6.8.2/opengl/2dpainting/build/Desktop_Qt_6_8_2_MSVC2022_64bit-Debug/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/2dpainting-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/Dev/Qt/Examples/Qt-6.8.2/opengl/2dpainting/build/Desktop_Qt_6_8_2_MSVC2022_64bit-Debug/2dpainting.exe
    GENERATE_QT_CONF
)
