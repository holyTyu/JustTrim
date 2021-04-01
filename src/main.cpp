#include <string>
#include <iostream>

#include <sdkddkver.h>
#include <boost/process.hpp>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_JPEG_Image.H>

#include "fltk_helper.h"

using namespace std;
namespace bp = boost::process;
namespace bf = boost::filesystem;

Fl_Image* fit_image( const Fl_Widget& w, Fl_Image& i )
{
	double iw = i.w();
	double ih = i.h();
	double c = min( (0. + w.w()) / iw, (0. + w.h()) / ih );
	return i.copy( iw * c, ih * c );
}

int main()
{
	constexpr int w = 1200, h = 800, d = 20;
	constexpr int uw = w - 2 * d, uh = h - 2 * d;
	auto main_window = new Fl_Window{ w, h, "Just Trim" };
	main_window->color( FL_WHITE );

	auto main_pack = new Fl_Pack{ d, d, uw, uh };
	main_pack->spacing( d );

	auto row_choose_video = new Fl_Pack{ 0, 0, 0, 50 };
	row_choose_video->type( Fl_Pack::HORIZONTAL );
	auto button_choose_video = new Fl_Button{ 0, 0, 200, 0, "Choose video file" };
	button_choose_video->labelsize( 24 );
	auto video_file = new Fl_Box{ 0, 0, 0, 0 };
	video_file->labelsize( 20 );
	row_choose_video->resizable( video_file );
	row_choose_video->end();

	auto group_pick_time = new Fl_Group{ 0, 0, uw, uw / 2 - d / 2 + 50 };

	auto pack_start_time = new Fl_Pack( 0, 0, uw / 2 - d / 2, 0 );

	auto start_time_preview = new Fl_Box{ 0, 0, uw / 2 - d / 2, uw / 2 - d / 2 };

	auto start_time_controls = new Fl_Pack{ 0, 0, 0, 50 };
	start_time_controls->type( Fl_Pack::HORIZONTAL );
	auto start_time_label = new Fl_Box{ 0, 0, 100, 0, "Start" };
	start_time_label->labelsize( 24 );
	auto input_start_time = new Fl_Input{ 0, 0, 300, 0 };
	input_start_time->textsize( 24 );
	start_time_controls->end();
	pack_start_time->end();

	auto pick_time_resize = new Fl_Box{ uw / 2 - d / 2, 0, 0, 0 };

	auto pack_end_time = new Fl_Pack( uw / 2 - d / 2, 0, uw / 2 - d / 2, 0 );

	auto end_time_preview = new Fl_Box{ 0, 0, uw / 2 - d / 2, uw / 2 - d / 2 };

	auto end_time_controls = new Fl_Pack{ 0, 0, 0, 50 };
	end_time_controls->type( Fl_Pack::HORIZONTAL );
	auto end_time_label = new Fl_Box{ 0, 0, 100, 0, "End" };
	end_time_label->labelsize( 24 );
	auto input_end_time = new Fl_Input{ 0, 0, 300, 0 };
	input_end_time->textsize( 24 );
	end_time_controls->end();
	pack_end_time->end();

	group_pick_time->resizable( pick_time_resize );
	group_pick_time->end();

	auto row4 = new Fl_Pack( 0, 0, 0, 50 );
	row4->type( Fl_Pack::HORIZONTAL );
	auto button_preview = new Fl_Button{ 0, 0, 250, 0, "Preview" };
	button_preview->labelsize( 24 );
	auto button_trim = new Fl_Button{ 0, 0, 250, 0, "Trim" };
	button_trim->labelsize( 24 );
	row4->end();

	main_pack->end();

	auto choose_video = new Fl_Native_File_Chooser{};
	choose_video->type( Fl_Native_File_Chooser::BROWSE_FILE );
	choose_video->title( "Choosing video file" );

	cb_func cb_choose_video{
		[&choose_video, &video_file]( Fl_Widget* w ) {
			switch( w->when() ) {
				case FL_WHEN_RELEASE:
				{
					choose_video->show();
					video_file->label( choose_video->filename() );
				}
			}
		}
	};
	bind_callback( button_choose_video, cb_choose_video );

	cb_func cb_preview{
		[&choose_video, &input_start_time, &input_end_time, &start_time_preview, &end_time_preview]( Fl_Widget* w ) {
			switch( w->when() ) {
				case FL_WHEN_RELEASE:
				{
					auto input_path = bf::path( choose_video->filename() );

					bp::system(
						bp::search_path( "ffmpeg" ), "-y",
						"-ss", input_start_time->value(),
						"-i", input_path,
						"-frames:v", "1",
						"start.jpg",
						bp::std_out > stdout, bp::std_err > stdout
					);
					Fl_JPEG_Image new_start_image{ "start.jpg" };
					auto old_start_image = reinterpret_cast<Fl_JPEG_Image*>(start_time_preview->image());
					start_time_preview->image( fit_image( *start_time_preview, new_start_image ) );
					if( old_start_image != nullptr ) delete old_start_image;
					start_time_preview->redraw();

					bp::system(
						bp::search_path( "ffmpeg" ), "-y",
						"-ss", input_end_time->value(),
						"-i", input_path,
						"-frames:v", "1",
						"end.jpg",
						bp::std_out > stdout, bp::std_err > stdout
					);
					Fl_JPEG_Image new_end_image{ "end.jpg" };
					auto old_end_image = reinterpret_cast<Fl_JPEG_Image*>(end_time_preview->image());
					end_time_preview->image( fit_image( *end_time_preview, new_end_image ) );
					if( old_end_image != nullptr ) delete old_end_image;
					end_time_preview->redraw();
				}
			}
		}
	};
	bind_callback( button_preview, cb_preview );

	cb_func cb_trim{
		[&choose_video, &input_start_time, &input_end_time]( Fl_Widget* w ) {
			switch( w->when() ) {
				case FL_WHEN_RELEASE:
				{
					auto input_path = bf::path( choose_video->filename() );
					bp::system(
						bp::search_path( "ffmpeg" ),
						"-i", input_path,
						"-ss", input_start_time->value(),
						"-to", input_end_time->value(),
						input_path.parent_path() / "output.mp4",
						bp::std_out > stdout, bp::std_err > stdout
					);
				}
			}
		}
	};
	bind_callback( button_trim, cb_trim );

	main_window->end();
	main_window->show();
	return Fl::run();
}