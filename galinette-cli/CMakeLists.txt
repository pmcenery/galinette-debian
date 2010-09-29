# Dependencies
find_package      (PkgConfig)
pkg_check_modules (FTDI  REQUIRED  libftdi>=0.13)

# Include & build
include_directories   (. ../libgalinette ${FTDI_INCLUDE_DIRS})
add_executable        (galinette-flash galinette-flash.c)
add_dependencies      (galinette-flash libgalinette)
target_link_libraries (galinette-flash libgalinette ${FTDI_LIBRARIES})

# Install
install (TARGETS galinette-flash DESTINATION ${bindir})
