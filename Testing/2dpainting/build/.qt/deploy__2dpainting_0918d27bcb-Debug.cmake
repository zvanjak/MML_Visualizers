include(C:/Projects/MML_Visualizers/2dpainting/build/.qt/QtDeploySupport-Debug.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/2dpainting-plugins-Debug.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/Projects/MML_Visualizers/2dpainting/build/Debug/2dpainting.exe
    GENERATE_QT_CONF
)
