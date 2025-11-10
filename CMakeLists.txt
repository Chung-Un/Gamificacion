cmake_minimum_required(VERSION 3.16)

project(Estru2Parcial1Qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)


set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Estru2Parcial1Qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        terrace.qrc
        Player.h
        Player.cpp
        scenebase.h scenebase.cpp
        terracescene.h terracescene.cpp
        game.h game.cpp
        ruletascene.h ruletascene.cpp
        juegoarte.h juegoarte.cpp
        juegopolitica.h juegopolitica.cpp
        juegohistoria.h juegohistoria.cpp
        puertajuegohistoria.h puertajuegohistoria.cpp
        runningman.h runningman.cpp
        juegociencia.h juegociencia.cpp
        scenebatalla.h scenebatalla.cpp
        npcbatalla.h npcbatalla.cpp
        startscene.h startscene.cpp
        menupartidadialog.h menupartidadialog.cpp
        caminouni.h

        caminouni.cpp
        aulakantscene.h aulakantscene.cpp
        subescenalibro.h subescenalibro.cpp
        subescenapizarra.h subescenapizarra.cpp
        labdescartes.h labdescartes.cpp




        vertice.h
        grafos.h
        mapa.h mapa.cpp
        grafos.cpp
        videoscene.h videoscene.cpp
        playerstats.h playerstats.cpp
        statsfile.h statsfile.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Estru2Parcial1Qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Estru2Parcial1Qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Estru2Parcial1Qt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Estru2Parcial1Qt
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Estru2Parcial1Qt)
endif()
set_target_properties(Estru2Parcial1Qt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Estru2Parcial1Qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Estru2Parcial1Qt)
endif()
