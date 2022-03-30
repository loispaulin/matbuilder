if(TARGET galois::galois)
    return()
endif()

message(STATUS "Third-party (external): creating target 'Galois++'")

include(FetchContent)
FetchContent_Declare(
    galois
    GIT_REPOSITORY https://github.com/wkjarosz/galois.git
    GIT_TAG 4cd153fd7b9fcec55de85e13fbae4af1d51aedfd
)
FetchContent_MakeAvailable(galois)


add_library(galois INTERFACE)
add_library(galois::galois ALIAS galois)


include(GNUInstallDirs)
target_include_directories(galois SYSTEM INTERFACE
    $<BUILD_INTERFACE:${galois_SOURCE_DIR}/src/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
