#include "common.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <xcb/xcb.h>

x_status_t x_atom_get(xcb_connection_t *c, xcb_atom_t atom, char **atom_value)
{
    x_status_t                 status = X_OK;
    xcb_generic_error_t *      error  = NULL;
    xcb_get_atom_name_cookie_t cookie;
    xcb_get_atom_name_reply_t *reply = NULL;

    *atom_value = NULL;
    cookie      = xcb_get_atom_name(c, atom);
    if((reply = xcb_get_atom_name_reply(c, cookie, &error)) == NULL)
    {
        fprintf(stderr, "Error getting atom name of '%d': %d\n", atom_value, error->error_code);
        status = X_ERROR;
    }
    else
    {
        *atom_value = malloc(xcb_get_atom_name_name_length(reply) + 1);
        if(*atom_value == NULL)
        {
            status = X_INSUFFICIENT_MEMORY;
            free(reply);
        }
        else
        {
            memcpy(*atom_value, xcb_get_atom_name_name(reply), xcb_get_atom_name_name_length(reply));
            (*atom_value)[xcb_get_atom_name_name_length(reply)] = '\0';
        }
    }

    if(error != NULL)
        free(error);
    if(reply != NULL)
        free(reply);
    return status;
}
