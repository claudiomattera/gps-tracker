project(gpxlib VERSION 0.0.1 LANGUAGES CXX)

find_package(EXPAT REQUIRED)

# Library

add_library(
    gpxlib
    SHARED
    Node.cpp
    Decimal.cpp
    DateTime.cpp
    String.cpp
    Unsigned.cpp
    DGPSStation
    Fix.cpp
    Degrees.cpp
    Longitude.cpp
    Latitude.cpp
    Bounds.cpp
    PTSeg.cpp
    PT.cpp
    EMail.cpp
    Link.cpp
    Person.cpp
    URI.cpp
    Copyright.cpp
    TRKSeg.cpp
    Extensions.cpp
    TRK.cpp
    WPT.cpp
    RTE.cpp
    Metadata.cpp
    GPX.cpp
    Parser.cpp
    Writer.cpp
    Report.cpp
    ReportCerr.cpp
    Algorithm.cpp
)

target_include_directories(
    gpxlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    gpx
)

set_target_properties(
    gpxlib
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    gpxlib
    LINK_PUBLIC
    expat
)

# Installation

install(
    TARGETS gpxlib EXPORT gpxlibConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT gpxlibConfig DESTINATION share/naivecoin/cmake)

# This makes the project importable from the build directory
export(TARGETS gpxlib FILE NaivecoinCryptoConfig.cmake)

