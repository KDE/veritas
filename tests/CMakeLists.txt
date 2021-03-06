set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(..)

# QtTest needs this in order to include the gui stuff.
# The alternative (which requires Qt>=4.6) is #include <QTestGui>
add_definitions(-DQT_GUI_LIB)

macro(kdev_add_test test_EXEC)
    set(test_SRCS ${ARGN})
    qt4_automoc( ${test_SRCS} )
    kde4_add_unit_test(veritas-${test_EXEC} ${test_SRCS})
    target_link_libraries(veritas-${test_EXEC}
        ${QT_QTTEST_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${KDE4_KDECORE_LIBS}
        ${KDEVPLATFORM_TESTS_LIBRARIES}
        kdevplatformveritas)
endmacro(kdev_add_test)

kdev_add_test(unit-test testtest.cpp)
kdev_add_test(unit-resultsmodel modeltest.cpp resultsmodeltest.cpp)
kdev_add_test(unit-runnermodel modeltest.cpp runnermodeltest.cpp)
kdev_add_test(unit-resultsproxymodel resultsproxymodeltest.cpp)
kdev_add_test(unit-testexecutor testexecutortest.cpp)
kdev_add_test(sys-runnerwindow runnerwindowtest.cpp)
kdev_add_test(unit-selectionstoretest selectionstoretest.cpp)
kdev_add_test(unit-treetraverse treetraversetest.cpp)
kdev_add_test(unit-resultswidget resultswidgettest.cpp)
kdev_add_test(unit-ctestfileparser ctestfileparsertest.cpp)
kdev_add_test(unit-testresult testresulttest.cpp)
kdev_add_test(unit-projectselection projectselectiontest.cpp)

macro(veri_add_mem_test test_EXEC prefix)
    add_test(veritas-mem-${test_EXEC}
        ${CMAKE_CURRENT_SOURCE_DIR}/runMemcheck.py ${CMAKE_CURRENT_BINARY_DIR}/veritas-${prefix}-${test_EXEC} ${CMAKE_BINARY_DIR})
endmacro(veri_add_mem_test)

veri_add_mem_test(test unit)
veri_add_mem_test(resultsmodel unit)
veri_add_mem_test(runnermodel unit)
veri_add_mem_test(resultsproxymodel unit)
veri_add_mem_test(testexecutor unit)
veri_add_mem_test(selectionstoretest unit)
veri_add_mem_test(treetraverse unit)
veri_add_mem_test(resultswidget unit)
veri_add_mem_test(runnerwindow sys)
veri_add_mem_test(ctestfileparser unit)
veri_add_mem_test(testresult unit)
# Disabled as it is too slow and uses too much memory (as it uses a KDevelop::Core)
# veri_add_mem_test(projectselection unit)

