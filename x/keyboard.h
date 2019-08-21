#ifndef KEYBOARD_H_
#define KEYBOARD_H_
#include <stdbool.h>
#include <stdint.h>

#include <xcb/xcb.h>

typedef struct
{
    xcb_keysym_t * keymap;
    uint8_t        keysyms_per_keycode;
    xcb_keycode_t *modmap;
    uint8_t        keycodes_per_modifier;
    uint16_t       modifier_numlock;
    uint16_t       modifier_capslock;
    uint16_t       modifier_shiftlock;
} x_keyboard_settings_t;

extern const char *modifier_names[];

void          x_keyboard_init(xcb_connection_t *c, const xcb_setup_t *setup, x_keyboard_settings_t *keyboard_settings);
void          x_keyboard_modifiers_get(xcb_connection_t *c, const xcb_setup_t *setup, xcb_keycode_t **modmap, uint8_t *keycodes_per_modifier);
void          x_keyboard_map_get(xcb_connection_t *c, const xcb_setup_t *setup, xcb_keysym_t **keymap, uint8_t *keysyms_per_keycode);
void          x_keyboard_map_print(xcb_keycode_t keycode_min, uint8_t keycode_cnt, xcb_keysym_t *keymap, uint8_t keysyms_per_keycode);
void          x_keyboard_modifiers_print(xcb_keycode_t *modifier_keycodes, uint8_t keycodes_per_modifier);
xcb_keycode_t x_keyboard_keysym_to_keycode(xcb_keysym_t ks, const xcb_setup_t *setup, const x_keyboard_settings_t *keyboard_settings);
uint16_t      x_keyboard_keysym_to_modifier(xcb_keysym_t keysym, const xcb_setup_t *setup, const x_keyboard_settings_t *keyboard_settings);
bool          x_keyboard_keysym_is_keypad(xcb_keysym_t keycode);
xcb_keysym_t  x_keyboard_keypress_to_keysym(xcb_key_press_event_t *kp, const xcb_setup_t *setup, const x_keyboard_settings_t *keyboard_settings);
unsigned int  x_keyboard_keysym_to_character(xcb_keysym_t keysym, char *dst);

#endif /* KEYBOARD_H_ */
