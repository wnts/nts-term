#ifndef FONT_H_
#define FONT_H_
#include "common.h"

#include <stdbool.h>
#include <xcb/xcb.h>



x_status_t x_font_exists(xcb_connection_t *c, char *font_name, bool *exists);
x_status_t x_font_is_monospace(xcb_connection_t *c, char *font_name, bool *is_monospace);
x_status_t x_font_open(xcb_connection_t *c, char *font_name, xcb_font_t * fid);
x_status_t x_font_close(xcb_connection_t *c, xcb_font_t font);
x_status_t x_query_font_by_name(xcb_connection_t *c, char *font_name, xcb_query_font_reply_t **reply);
x_status_t x_query_font_by_fontable(xcb_connection_t *c, xcb_fontable_t font, xcb_query_font_reply_t **reply);


#endif /* FONT_H_ */
