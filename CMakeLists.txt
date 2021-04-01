cmake_minimum_required( VERSION 3.19 )
project( JustTrim CXX )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

find_package( FLTK REQUIRED NO_MODULE )
find_package( Boost REQUIRED headers filesystem NO_MODULE )

add_subdirectory( src )