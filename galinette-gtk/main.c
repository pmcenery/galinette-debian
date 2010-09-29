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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gtk/gtk.h>
#include <glade/glade.h>

#define GALINETTE_WEBSITE "https://sourceforge.net/projects/galinette/"


#include "callbacks.h"
#include "galinette-gtk.h"
#include "hal.h"
GladeXML *gxml;
GMainLoop *mainloop;


GtkWidget* create_window (void)
{
	GtkWidget *window;
	
	gxml = glade_xml_new (GLADEFILE, NULL, NULL);
	
	/* This is important */
	glade_xml_signal_autoconnect (gxml);
	window = glade_xml_get_widget (gxml, "win_flash");
	
	return window;
}


int main (int argc, char *argv[])
{
 	const gchar *authors[] = {"Boris Lechner <boris.lechner.dev@gmail.com>", '\0'};
	
	GtkWidget *window;
	GtkAboutDialog *aboutwin;
	mainloop = g_main_loop_new(NULL,FALSE);
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);

	window = create_window ();
	gtk_widget_show (window);
	halinitmain();

	aboutwin = GTK_ABOUT_DIALOG( glade_xml_get_widget (gxml, "aboutdialog") );
	g_signal_connect (aboutwin, "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), NULL);
	gtk_about_dialog_set_authors(aboutwin, authors);
	gtk_about_dialog_set_version(aboutwin, GALINETTE_VERSION);
	gtk_about_dialog_set_website(aboutwin, GALINETTE_WEBSITE);
	gtk_about_dialog_set_website_label(aboutwin, GALINETTE_WEBSITE);

	g_main_loop_run(mainloop);
	
	return 0;
}
