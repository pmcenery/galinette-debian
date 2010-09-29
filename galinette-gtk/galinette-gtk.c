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

#include "../libgalinette/libgalinette.h"
#include "callbacks.h"
#include "galinette-gtk.h"

guint firmwareLoaded              = FALSE;
long firmwareSize                 = 0;
const unsigned char *firmware_buf = NULL;


guint loadFirmware(char *path)
{
	firmwareSize = galinette_loadFirmware(path);
	GtkStatusbar *status = GTK_STATUSBAR( glade_xml_get_widget (gxml, "sb_main") );
	
	if ( firmwareSize > 0 && galinette_checkFirmwareIntegrity() == 0 ) {
		firmwareLoaded = TRUE;
		firmware_buf = galinette_getFirmwareData(&firmwareSize);
		update_flash_button_state();
		gtk_statusbar_pop(status, 1);
		gtk_statusbar_push(status, 1, "Firmware successfully loaded");
		return TRUE;
	}
	else {
		galinette_freeFirmwareMem();
		firmwareLoaded = FALSE;
		update_flash_button_state();
		gtk_statusbar_pop(status, 1);
		gtk_statusbar_push(status, 1, galinette_getErrMsg());
	}

	return FALSE;
}


void update_flash_button_state()
{
	GtkWidget *flash = glade_xml_get_widget (gxml, "but_flash");
	gtk_widget_set_sensitive(flash, deviceConnected && firmwareLoaded);
}



void startFlashing()
{
	galinette_detect();
	galinette_connect();
	galinette_setProgressCallback(progressbar);
	galinette_flash(firmware_buf, firmwareSize);
}


void progressbar(unsigned long min, unsigned long max, unsigned long val, const char *comment)
{
	GtkProgressBar *pg = GTK_PROGRESS_BAR( glade_xml_get_widget (gxml, "pb_flash") );
	GtkStatusbar *status = GTK_STATUSBAR( glade_xml_get_widget (gxml, "sb_main") );
	
	gdouble percent = (((float)val+0.5-(float)min)/(float)max-(float)min);

	gtk_statusbar_pop(status, 1);
	gtk_statusbar_push(status, 1, comment);
	gtk_progress_bar_set_fraction(pg, percent);
	while (gtk_events_pending ())
		gtk_main_iteration ();	
}