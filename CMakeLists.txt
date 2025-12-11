cmake_minimum_required(VERSION 3.16)


project(ProjectUnic VERSION 0.1 LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS UIfiles)


# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# === Определяем версию Qt ===
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
set(QT_VERSION_MAJOR ${QT_VERSION_MAJOR})

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Gui)

# === Подключаем компоненты Qt ===
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql)

# Для SQL — подключаем отдельно (не входит в Widgets/Core по умолчанию)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)

# === Список исходников ===
set(PROJECT_SOURCES
    main.cpp
    BusinessLogic/ProductManager/productmanager.cpp
    BusinessLogic/ProductManager/productmanager.h
    BusinessLogic/Authentication/authentication.cpp
    BusinessLogic/Authentication/authentication.h
    MainWindow/mainwindow.cpp
    MainWindow/mainwindow.h
    UIfiles/mainwindow.ui
    LoginPage/loginform.cpp
    LoginPage/loginform.h
    UIfiles/loginform.ui
    BasicPage/basicform.cpp
    BasicPage/basicform.h
    UIfiles/basicform.ui
)

# === Создаём таргет в зависимости от версии Qt ===
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ProjectUnic
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        BusinessLogic/ProductManager/productmanager.h BusinessLogic/ProductManager/productmanager.cpp
        AddPage/addform.h AddPage/addform.cpp UIfiles/addform.ui
        BusinessLogic/OutputWord/outputword.h BusinessLogic/OutputWord/outputword.cpp
        EditPage/editform.h EditPage/editform.cpp UIfiles/editform.ui
        Settings/settingsform.h Settings/settingsform.cpp UIfiles/settingsform.ui
        AddSupplyPage/addsupplyform.h AddSupplyPage/addsupplyform.cpp UIfiles/addsupplyform.ui
        AddShipmentPage/addshipmentform.h AddShipmentPage/addshipmentform.cpp UIfiles/addshipmentform.ui
    )

    # Линкуем нужные модули ПОСЛЕ создания таргета
    target_link_libraries(ProjectUnic PRIVATE
        Qt6::Widgets
        Qt6::Sql
    )

else()
    # Qt5 — используем add_executable (или add_library для Android)
    if(ANDROID)
        add_library(ProjectUnic SHARED ${PROJECT_SOURCES})
    else()
        add_executable(ProjectUnic ${PROJECT_SOURCES})
    endif()

    target_link_libraries(ProjectUnic PRIVATE
        Qt5::Widgets
        Qt5::Sql
    )
endif()

# === Установка свойств таргета ===
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ProjectUnic)
endif()

set_target_properties(ProjectUnic PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# === Установка (необязательно, но оставим) ===
include(GNUInstallDirs)
install(TARGETS ProjectUnic
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# === Финализация (обязательно при MANUAL_FINALIZATION) ===
if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(ProjectUnic)
endif()
