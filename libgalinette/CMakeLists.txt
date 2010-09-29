# Include & build
include_directories   (.)
add_library           (libgalinette SHARED device.c error.c firmware.c flash.c libgalinette.h libgalinette-private.h)
set_target_properties (libgalinette PROPERTIES PREFIX "")

# Install
install (TARGETS libgalinette LIBRARY DESTINATION ${libdir})
