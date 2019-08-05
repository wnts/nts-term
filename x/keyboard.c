#include "keyboard.h"
#include "unicode.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include <xcb/xcb.h>

const char *modifier_names[] = { "shift", "lock", "control", "mod1", "mod2", "mod3", "mod4", "mod5"};


void x_keyboard_init(xcb_connection_t * c, const xcb_setup_t * setup, x_keyboard_settings_t * keyboard_settings)
{
	/* Get keyboard keymap */
	x_keyboard_map_get(c, setup, &keyboard_settings->keymap, &keyboard_settings->keysyms_per_keycode);
	//x_keyboard_map_print(setup->min_keycode, setup->max_keycode - setup->min_keycode + 1, keyboard_settings->keymap, keyboard_settings->keysyms_per_keycode);

	/* Get keyboard modifier map */
	x_keyboard_modifiers_get(c, setup, &keyboard_settings->modmap, &keyboard_settings->keycodes_per_modifier);
	//x_keyboard_modifiers_print(keyboard_settings->modmap, keyboard_settings->keycodes_per_modifier);
	/* Get modifier associated with numlock, caps lock */
	keyboard_settings->modifier_numlock = x_keyboard_keysym_to_modifier(XK_Num_Lock, setup, keyboard_settings);
	keyboard_settings->modifier_capslock = x_keyboard_keysym_to_modifier(XK_Caps_Lock, setup, keyboard_settings);
	keyboard_settings->modifier_shiftlock = x_keyboard_keysym_to_modifier(XK_Shift_Lock, setup, keyboard_settings);
	//printf("keyboard modifier for numlock: %s\n", modifier_names[keyboard_settings->modifier_numlock]);
	//printf("keyboard modifier for caps lock: %s\n", modifier_names[keyboard_settings->modifier_shiftlock]);
	//printf("keyboard modifier for shift lock: %s\n", modifier_names[keyboard_settings->modifier_capslock]);
}

void x_keyboard_map_get(xcb_connection_t * c, const xcb_setup_t *setup, xcb_keysym_t **keymap, uint8_t * keysyms_per_keycode)
{
	uint32_t keycode_cnt = setup->max_keycode - setup->min_keycode + 1;

	xcb_get_keyboard_mapping_reply_t * keyboard_mapping = xcb_get_keyboard_mapping_reply(c, xcb_get_keyboard_mapping(c, setup->min_keycode, keycode_cnt), NULL);
	xcb_keysym_t * keysyms = xcb_get_keyboard_mapping_keysyms(keyboard_mapping);
	*keysyms_per_keycode = keyboard_mapping->keysyms_per_keycode;
	uint32_t buffer_size = sizeof(xcb_keysym_t) * keycode_cnt * keyboard_mapping->keysyms_per_keycode;
	*keymap = malloc(buffer_size);
	memcpy(*keymap, keysyms, buffer_size);

	free(keyboard_mapping);
}



void x_keyboard_modifiers_get(xcb_connection_t *c, const xcb_setup_t *setup, xcb_keycode_t **modmap, uint8_t *keycodes_per_modifier)
{
	xcb_get_modifier_mapping_reply_t *modifier_mapping = xcb_get_modifier_mapping_reply(c, xcb_get_modifier_mapping(c), NULL);
	xcb_keycode_t *mod_keycodes = xcb_get_modifier_mapping_keycodes(modifier_mapping);
	*keycodes_per_modifier = modifier_mapping->keycodes_per_modifier;
	uint32_t buffer_size = sizeof(xcb_keycode_t) * 8 * modifier_mapping->keycodes_per_modifier;
	*modmap = malloc(buffer_size);
	memcpy(*modmap, mod_keycodes, buffer_size);

	free(modifier_mapping);

}

void x_keyboard_map_print(xcb_keycode_t keycode_min, uint8_t keycode_cnt, xcb_keysym_t * keymap, uint8_t keysyms_per_keycode)
{
	for(int i = 0; i < keycode_cnt; i++)
	{
		printf("%d\t", keycode_min + i);
		for(int j = 0; j < keysyms_per_keycode; j++)
		{
			printf("%4x ", keymap[i * keysyms_per_keycode + j]);
		}
		putchar('\n');
	}
}

void x_keyboard_modifiers_print(xcb_keycode_t *modifier_keycodes, uint8_t keycodes_per_modifier)
{
	for(int i = 0; i < 8; i++)
	{
		printf("%s\t", modifier_names[i]);
		for(int j = 0; j < keycodes_per_modifier; j++)
		{
			printf("%04x ", modifier_keycodes[i * keycodes_per_modifier + j]);
		}
		putchar('\n');
	}
}


xcb_keycode_t x_keyboard_keysym_to_keycode(xcb_keysym_t ks, const xcb_setup_t *setup, const x_keyboard_settings_t *keyboard_settings)
{
	for(int i = 0; i < setup->max_keycode - setup->min_keycode + 1; i++)
	{
		for(int j = 0; j < keyboard_settings->keysyms_per_keycode; j++)
		{
			if(keyboard_settings->keymap[i * keyboard_settings->keysyms_per_keycode + j] == ks)
				return i + setup->min_keycode;
		}
	}
	return 0;
}

/**
 * Given a keycode, look up the keysym using the rules specified in the X Window System Protocol specification, Ch. 5: Keyboards
 *
 * @Todo implement correct handling of MODE SWITCH keycode (Alt Gr)
 */
xcb_keysym_t x_keyboard_keypress_to_keysym(xcb_key_press_event_t *kp, const xcb_setup_t *setup, const x_keyboard_settings_t *keyboard_settings)
{
	xcb_keysym_t * keysyms = &keyboard_settings->keymap[(kp->detail - setup->min_keycode) * keyboard_settings->keysyms_per_keycode];

	if((kp->state & (1 << keyboard_settings->modifier_numlock)) && (x_keyboard_keysym_is_keypad(keysyms[1])))
	{
		if(kp->state & XCB_MOD_MASK_SHIFT)
		{
			return keysyms[0];
		}
		else
		{
			return keysyms[1];
		}
	}

	if(!(kp->state & (XCB_MOD_MASK_SHIFT | XCB_MOD_MASK_LOCK)))
	{
		return keysyms[0];
	}

	if((kp->state & XCB_MOD_MASK_LOCK) && ((1 << keyboard_settings->modifier_capslock) & XCB_MOD_MASK_LOCK))
	{
		if(!(kp->state & XCB_MOD_MASK_SHIFT))
		{
			if(toupper(keysyms[0]) == keysyms[0])
				return keysyms[0];
			else
				return keysyms[1];

		}
		else
		{
			if(toupper(keysyms[1]) == keysyms[1])
				return keysyms[1];
			else
				return keysyms[0];
		}
	}

	if((kp->state & XCB_MOD_MASK_SHIFT) || ((1 << keyboard_settings->modifier_shiftlock) & XCB_MOD_MASK_LOCK))
	{
		return keysyms[1];
	}

	return XK_VoidSymbol;
}

unsigned int x_keyboard_keysym_to_character(xcb_keysym_t keysym, char * dst)
{
	if(keysym >= 0x20 && keysym <= 0x7e)
		return unicode_codepoint_to_utf8(keysym, dst);

	if(keysym >= 0x01000100 && keysym <= 0x0110FFFF)
		return unicode_codepoint_to_utf8(keysym, dst);

	return 0;
}



uint16_t x_keyboard_keysym_to_modifier(xcb_keysym_t keysym, const xcb_setup_t *setup, const x_keyboard_settings_t *keyboard_settings)
{
	uint32_t modifier = 0;
	xcb_keycode_t keycode = x_keyboard_keysym_to_keycode(keysym, setup, keyboard_settings);

	if(keycode == 0)
		return 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < keyboard_settings->keycodes_per_modifier; j++)
		{
			if(keyboard_settings->modmap[i * keyboard_settings->keycodes_per_modifier + j] == keycode)
			{
				modifier = i;
			}
		}
	}
	return modifier;
}

bool x_keyboard_keysym_is_keypad(xcb_keysym_t keysym)
{
	return XK_KP_Space <= keysym && keysym <= XK_KP_9;
}

