cmake_minimum_required (VERSION 2.6)

set(GALINETTE_VERSION   "1.1"             CACHE STRING   "Software version" FORCE)
set(bindir              "bin"             CACHE FILEPATH "bin directory"    FORCE)
set(libdir              "lib"             CACHE FILEPATH "lib directory"    FORCE)
set(sharedir            "share/galinette" CACHE FILEPATH "share directory"  FORCE)

project          (galinette)
add_definitions  (-Wall)

add_subdirectory (libgalinette)
add_subdirectory (galinette-cli)
add_subdirectory (galinette-gtk)
