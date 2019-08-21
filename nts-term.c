#include "keyboard.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>
#include <xcb/xcb.h>

#define WIDTH  300
#define HEIGHT 300
#define FONT   ""

static xcb_connection_t *    c      = NULL;
static const xcb_setup_t *   setup  = NULL;
static xcb_screen_t *        screen = NULL;
static x_keyboard_settings_t keyboard_settings;

void x_init(void);
void main_loop(void);

int main(int argc, char *argv[])
{
    uint32_t   mask          = 0;
    uint32_t   value_list[3] = { 0 };
    xcb_font_t font;

    x_init();

    main_loop();

    return 0;
}

void main_loop(void)
{
    /* main X client event loop */
    xcb_generic_event_t *e = NULL;
    uint32_t             i = 0;
    while((e = xcb_wait_for_event(c)))
    {
        switch(e->response_type & ~0x80)
        {
            case XCB_EXPOSE:
                printf("Expose %d!\n", ++i);
                break;
            case XCB_KEY_PRESS:
            {
                char output[256];
                bool mods = false;

                xcb_key_press_event_t *kp = (xcb_key_press_event_t *)e;
                sprintf(output, "keypress: keycode %u", kp->detail);
                for(int i = 0; i < 8; i++)
                {
                    if((kp->state >> i) & 1)
                    {
                        if(!mods)
                        {
                            mods = true;
                            sprintf(output + strlen(output), " mods [");
                        }
                        sprintf(output + strlen(output), " %s", modifier_names[i]);
                    }
                }
                sprintf(output + strlen(output), " ] keysym %u\n", x_keyboard_keypress_to_keysym(kp, setup, &keyboard_settings));
                printf(output);
                char         dst[5];
                unsigned int used = x_keyboard_keysym_to_character(x_keyboard_keypress_to_keysym(kp, setup, &keyboard_settings), dst);
                dst[used]         = 0;
                printf("\t '%s'\n", dst);
                break;
            }
            default:
                break;
        }
        free(e);
    }
    pause();
}

void x_init(void)
{
    /* Connect to X server */
    c = xcb_connect(NULL, NULL);

    /* Get first screen */
    screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    /* Get X server setup */
    setup = xcb_get_setup(c);

    x_keyboard_init(c, setup, &keyboard_settings);

    /* Create window */
    uint32_t win               = xcb_generate_id(c);
    uint32_t window_value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t window_value_list[2];
    window_value_list[0] = screen->black_pixel;
    window_value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

    xcb_create_window(c, XCB_COPY_FROM_PARENT, win, screen->root, 0, 0, WIDTH, HEIGHT, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
                      window_value_mask, window_value_list);

    /* Map window on screen */
    xcb_map_window(c, win);

    /* Flush request */
    xcb_flush(c);
}
