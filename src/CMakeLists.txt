file( GLOB source *.cpp *.h )
add_executable( JustTrim ${source} )
target_link_libraries( JustTrim PRIVATE
	fltk fltk_images
	Boost::headers Boost::filesystem
)
install( TARGETS JustTrim )