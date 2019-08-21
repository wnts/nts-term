#ifndef X_COMMON_H_
#define X_COMMON_H_

#include <xcb/xcb.h>

typedef enum
{
    X_OK,
    X_ERROR,
    X_INSUFFICIENT_MEMORY
} x_status_t;

x_status_t x_atom_get(xcb_connection_t *c, xcb_atom_t atom, char **atom_value);

#endif /* X_COMMON_H_ */
