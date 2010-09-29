/*
 *  Copyright (C) Boris Lechner 2009 2010 <boris.lechner.dev@gmail.com>
 *   
 *  This file is part of Galinette.
 *
 *  Galinette is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License.
 *
 *  Galinette is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "callbacks.h"
#include "galinette-gtk.h"

extern GladeXML *gxml;


void win_flash_delete_event_cb (GtkWidget *widget, gpointer data)
{
	g_main_loop_quit(mainloop);
}


void imnui_quit_activate_cb (GtkWidget *widget, gpointer data)
{
	g_main_loop_quit(mainloop);
}


void imnui_about_activate_cb (GtkWidget *widget, gpointer data)
{
	GtkWidget *aboutdialog = GTK_WIDGET( glade_xml_get_widget (gxml, "aboutdialog") );
	
	gtk_widget_show(aboutdialog);
}


void aboutdialog_close_cb (GtkWidget *widget, gpointer data)
{
	GtkWidget *aboutdialog = GTK_WIDGET( glade_xml_get_widget (gxml, "aboutdialog") );
	
	gtk_widget_hide(aboutdialog);
}


void fcb_loadfirmware_file_set_cb (GtkWidget *widget, gpointer data)
{
	loadFirmware(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
}


void but_flash_clicked_cb(GtkWidget *widget, gpointer data)
{
	startFlashing();
}

