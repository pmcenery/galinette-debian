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

#include <glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus.h>
#include <stdio.h>
#include <string.h>

#include "hal.h"

#include "../libgalinette/libgalinette.h"
#include "callbacks.h"
#include "galinette-gtk.h"

char *myudi = "/org/freedesktop/Hal/devices/usb_device_403_6001_";
const short myudilen = 49;

guint deviceConnected = 0;


gboolean deviceDetect(LibHalContext *ctx, const char *udi)
{
	char  **ret, **work;
	int devnum;
	
	ret = libhal_get_all_devices(ctx, &devnum, NULL);

	for (work=ret ; *work ; work++) {
		if ( isB1Device(*work) )
			deviceConnected++;
	}

	updateStatus();

	libhal_free_string_array(ret);
	
	return TRUE;
}


void updateStatus()
{
	GtkImage *status = GTK_IMAGE( glade_xml_get_widget (gxml, "img_devicestatus") );
	
	if ( deviceConnected > 0 ) {
		gtk_image_set_from_stock (status, "gtk-connect", GTK_ICON_SIZE_LARGE_TOOLBAR);
		gtk_widget_set_tooltip_text(GTK_WIDGET(status), "B1 connected");
		update_flash_button_state();
	}
	else {
		gtk_image_set_from_stock (status, "gtk-disconnect", GTK_ICON_SIZE_LARGE_TOOLBAR);
		gtk_widget_set_tooltip_text(GTK_WIDGET(status), "V1 disconnected");
		update_flash_button_state();
	}
}


gboolean isB1Device( const char *udi )
{
	gboolean ret = FALSE;
	char *work = (char*) udi;
	
	if ( strncmp(work, myudi, myudilen) == 0 ) {
		for(work+=myudilen ; ret == FALSE && *work != '_' ; work++) {
			if (*work == '\0')
				ret = TRUE;
		}
	}

	return ret;
}




static void handle_device_removed(LibHalContext *ctx, const char *udi)
{
	if ( isB1Device(udi) ) {
		deviceConnected--;
	}
	
	updateStatus();
}

static void handle_device_added(LibHalContext *ctx, const char *udi)
{
    if ( isB1Device(udi) ) {
		deviceConnected++;
	}
	
	updateStatus();
}

int halinitmain()
{
    DBusConnection *connection;
    DBusError error;
	
    LibHalContext *ctx;
	
    dbus_error_init(&error);
    connection = dbus_bus_get(DBUS_BUS_SYSTEM,&error);
    
    if ( dbus_error_is_set(&error) ) {
        printf("Unable to connect to DBus: %s\n",error.message);
        dbus_error_free(&error);
        return 1;
    }
	
    ctx = libhal_ctx_new();
	
    if ( !libhal_ctx_set_dbus_connection(ctx, connection) ) {
		printf("Error %s\n",error.message);
		dbus_error_free(&error);
		return 1;
    }
	
    if ( !libhal_ctx_init(ctx, &error) ) {
		printf("Hal context initializing failure %s\n",error.message);
		return 1;
    }


    dbus_connection_setup_with_g_main(connection,NULL);

	deviceDetect(ctx, myudi);
	
    libhal_ctx_set_device_added(ctx, handle_device_added);
    libhal_ctx_set_device_removed(ctx, handle_device_removed);

	return TRUE;
}
