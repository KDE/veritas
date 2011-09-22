project(veritas)

set(KDE_MIN_VERSION "4.3.0")
find_package(KDE4 4.3.0 REQUIRED)
#find_package(BerkeleyDB)
#find_package(TDB)
include (KDE4Defaults)
include (MacroLibrary)

find_package(KDevPlatform 0.9.95 REQUIRED)

include_directories(${KDEVPLATFORM_INCLUDE_DIR} ${KDE4_INCLUDES})

add_subdirectory(tests)

add_definitions( ${KDE4_ENABLE_EXCEPTIONS} )
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=9003 )

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

########### next target ###############

set(veritas_SRCS
    test.cpp
    testresult.cpp
    internal/testexecutor.cpp
    testrunner.cpp
    itesttreebuilder.cpp
    itestframework.cpp
    testexecutableinfo.cpp
    ctestfileparser.cpp
    projectselection.cpp
    internal/utils.cpp
    internal/selectionstore.cpp
    testtoolviewfactory.cpp
    internal/toolviewdata.cpp
    internal/resultsmodel.cpp
    internal/resultsproxymodel.cpp
    internal/runnermodel.cpp
    internal/runnerproxymodel.cpp
    internal/runnerwindow.cpp
   
    internal/overlaymanager.cpp
    internal/overlaytoggle.cpp
    internal/resultswidget.cpp
    internal/itestframework_p.cpp
    internal/test_p.cpp
    tests/runnertesthelper.cpp )# Actual tests of concrete frameworks use these.
                               # eg QTestRunnerTest, CppUnitRunnerTest
                               # Before ui_* and other internals were installed
                               # to make this possible, which is bad.
    #${CMAKE_SOURCE_DIR}/tests/modeltest.cpp)

kde4_add_ui_files(veritas_SRCS
    internal/runnerwindow.ui)

qt4_add_resources(veritas_SRCS
    resources/qxrunner.qrc)

kde4_add_library(kdevplatformveritas SHARED ${veritas_SRCS})

set_target_properties(kdevplatformveritas PROPERTIES VERSION 4.0.0 SOVERSION 4)
target_link_libraries(kdevplatformveritas
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
    ${KDEVPLATFORM_SUBLIME_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS})
target_link_libraries(kdevplatformveritas LINK_INTERFACE_LIBRARIES 
        ${KDE4_KDECORE_LIBS} 
        ${KDE4_KTEXTEDITOR_LIBS} 
        ${KDE4_KDEUI_LIBS} 
        ${QT_QTTEST_LIBRARY})
qt4_automoc(${veritas_SRCS})
install(TARGETS kdevplatformveritas ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
    test.h
    veritasexport.h
    testresult.h
    testrunner.h
    itestframework.h
    itesttreebuilder.h
    testtoolviewfactory.h
    testexecutableinfo.h
    ctestfileparser.h
    projectselection.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/kdevplatform/veritas COMPONENT Devel )
install(FILES
    tests/runnertesthelper.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/kdevplatform/veritas/tests COMPONENT Devel )
