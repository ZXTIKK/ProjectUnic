cmake_minimum_required(VERSION 3.16)
project(ProjectUnic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/UIfiles)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql PrintSupport)

set(PROJECT_SOURCES
    main.cpp

    # ... (весь ваш список исходников)
    BusinessLogic/QCustomPlot/qcustomplot.cpp
    BusinessLogic/ProductManager/productmanager.cpp
    BusinessLogic/ProductManager/productmanager.h
    BusinessLogic/Authentication/authentication.cpp
    BusinessLogic/Authentication/authentication.h
    BusinessLogic/OutputWord/outputword.cpp
    BusinessLogic/OutputWord/outputword.h
    MainWindow/mainwindow.cpp
    MainWindow/mainwindow.h
    UIfiles/mainwindow.ui
    LoginPage/loginform.cpp
    LoginPage/loginform.h
    UIfiles/loginform.ui
    BasicPage/basicform.cpp
    BasicPage/basicform.h
    UIfiles/basicform.ui
    AddPage/addform.cpp
    AddPage/addform.h
    UIfiles/addform.ui
    EditPage/editform.cpp
    EditPage/editform.h
    UIfiles/editform.ui
    Settings/settingsform.cpp
    Settings/settingsform.h
    UIfiles/settingsform.ui
    AddSupplyPage/addsupplyform.cpp
    AddSupplyPage/addsupplyform.h
    UIfiles/addsupplyform.ui
    AddShipmentPage/addshipmentform.cpp
    AddShipmentPage/addshipmentform.h
    UIfiles/addshipmentform.ui
    About/aboutform.cpp
    About/aboutform.h
    UIfiles/aboutform.ui
    BusinessLogic/OutputExcel/outputexcel.cpp
)

# ==========================================================
# !!! КРИТИЧНОЕ ИСПРАВЛЕНИЕ: ДОБАВЛЕНИЕ ИСПОЛНЯЕМОГО ФАЙЛА
# ==========================================================
add_executable(ProjectUnic ${PROJECT_SOURCES})
# Теперь ProjectUnic определен!

target_sources(ProjectUnic PRIVATE
    BusinessLogic/QCustomPlot/qcustomplot.h
)

set_source_files_properties(BusinessLogic/QCustomPlot/qcustomplot.cpp
    PROPERTIES SKIP_AUTOMOC ON
)

target_include_directories(ProjectUnic PRIVATE
    BusinessLogic/QCustomPlot
)

target_link_libraries(ProjectUnic PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
)

if(MINGW)
    target_link_options(ProjectUnic PRIVATE -Wa,-mbig-obj)
    target_compile_options(ProjectUnic PRIVATE -Wa,-mbig-obj)
    target_compile_options(ProjectUnic PRIVATE -Wno-deprecated-declarations)
endif()

set_target_properties(ProjectUnic PROPERTIES
    MACOSX_BUNDLE ON
    WIN32_EXECUTABLE ON
)

include(GNUInstallDirs)
install(TARGETS ProjectUnic
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(ProjectUnic)
