#include "font.h"
#include "common.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <xcb/xcb.h>

x_status_t x_font_exists(xcb_connection_t *c, char *font_name, bool *exists)
{
	char pattern[] = "*";
	char name[128];
	xcb_generic_error_t *error;
	xcb_list_fonts_reply_t *reply;
	x_status_t status = X_OK;

	xcb_list_fonts_cookie_t cookie_list_fonts = xcb_list_fonts(c, UINT16_MAX, strlen(pattern), pattern);
	if((reply = xcb_list_fonts_reply(c, cookie_list_fonts, &error)) != NULL)
	{
		for(xcb_str_iterator_t iter = xcb_list_fonts_names_iterator(reply); iter.rem; xcb_str_next(&iter))
		{
			memcpy(name, xcb_str_name(iter.data), iter.data->name_len);
			name[iter.data->name_len] = '\0';
			if(!strcmp(font_name, name))
			{
				*exists = true;
				break;
			}

		}
		free(reply);
	}
	else
	{
		fprintf(stderr, "Can't list fonts: %d\n", error->error_code);
		free(error);
	}

	return status;
}

x_status_t x_font_is_monospace(xcb_connection_t *c, char *font_name, bool *is_monospace)
{
	x_status_t status = X_OK;
	xcb_query_font_reply_t *reply = NULL;

	status = x_query_font_by_name(c, font_name, &reply);
	if(status == X_OK)
	{
		if(reply->min_bounds.character_width == reply->max_bounds.character_width)
		{
			*is_monospace = true;
		}
		//@todo Add more thorough checks here, see ComputeFontType() function in xlsfonts.c (x11-utils)
	}

	if(reply != NULL)
		free(reply);
	return status;
}

x_status_t x_query_font_by_name(xcb_connection_t *c, char *font_name, xcb_query_font_reply_t **reply)
{
	x_status_t status = X_OK;
	xcb_font_t font;

	status = x_font_open(c, font_name, &font);
	if(status == X_OK)
	{
		status = x_query_font_by_fontable(c, font, reply);
		x_font_close(c, font);
	}

	return status;
}

x_status_t x_query_font_by_fontable(xcb_connection_t *c, xcb_fontable_t font, xcb_query_font_reply_t **reply)
{
	xcb_generic_error_t *error = NULL;
	x_status_t status = X_OK;

	xcb_query_font_cookie_t cookie = xcb_query_font(c, font);
	if((*reply = xcb_query_font_reply(c, cookie, &error)) == NULL)
	{
		status = X_ERROR;
		fprintf(stderr, "Can't query font: %d\n", error->error_code);
		free(error);
	}

	return status;
}

x_status_t x_font_close(xcb_connection_t *c, xcb_font_t font)
{
	x_status_t status = X_OK;
	xcb_generic_error_t *error = NULL;
	xcb_void_cookie_t cookie;

	cookie = xcb_close_font_checked(c, font);
	if((error = xcb_request_check(c, cookie)) != NULL)
	{
		fprintf(stderr, "Can't close font: %d\n", error->error_code);
		free(error);
		status = X_ERROR;
	}

	return status;
}


x_status_t x_font_open(xcb_connection_t *c, char *font_name, xcb_font_t * fid)
{
	xcb_generic_error_t *error = NULL;
	xcb_void_cookie_t cookie;
	x_status_t status = X_OK;
	xcb_font_t font = xcb_generate_id(c);

	cookie = xcb_open_font_checked(c, font, strlen(font_name), font_name);
	if((error = xcb_request_check(c, cookie)) != NULL)
	{
		fprintf(stderr, "Can't open font '%s': %d\n", font_name, error->error_code);
		free(error);
	}

	*fid = font;
	return status;
}
