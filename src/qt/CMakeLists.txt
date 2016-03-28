FIND_PACKAGE(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Phonon4Qt5 REQUIRED)

SET( QT_USE_PHONON TRUE )

set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET(mainwindow_SOURCES main.cpp mainwindow.cpp)
SET(mainwindow_HEADERS mainwindow.h)
SET(mainwindow_FORMS mainwindow.ui)

QT5_WRAP_CPP(mainwindow_HEADERS_MOC ${mainwindow_HEADERS})
QT5_WRAP_UI(mainwindow_FORMS_HEADERS ${mainwindow_FORMS})
QT5_ADD_RESOURCES(mainwindow_RESOURCES_RCC ${mainwindow_RESOURCES})

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${PHONON4QT5_INCLUDE_DIR}
)

ADD_EXECUTABLE(
  satip-qt
  ${mainwindow_SOURCES} 
  ${mainwindow_HEADERS_MOC} 
  ${mainwindow_FORMS_HEADERS}
  ${mainwindow_RESOURCES_RCC}
)

target_link_libraries(
  satip-qt
  satip
  ${QT_QTCORE_LIBRARY}
  Phonon::phonon4qt5
  Qt5::Widgets
)