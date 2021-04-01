#pragma once
#include <functional>

using cb_func = std::function<void( Fl_Widget* )>;
class Fl_Widget;

inline void fltk_callback( Fl_Widget* w, void* data )
{
	reinterpret_cast<cb_func*>(data)->operator()(w);
}

inline void bind_callback( Fl_Widget* w, cb_func& cb )
{
	w->callback( fltk_callback, reinterpret_cast<void*>(&cb) );
}