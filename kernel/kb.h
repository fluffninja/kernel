#ifndef _INC_KB
#define _INC_KB 1

enum {
    KB_KEY_NONE = 128,
    KB_KEY_ESCAPE,
    KB_KEY_ENTER,
    KB_KEY_BACKSPACE,
    KB_KEY_CAPSLOCK,
    KB_KEY_NUMLOCK,
    KB_KEY_SCROLLLOCK,
    KB_KEY_F1,
    KB_KEY_F2,
    KB_KEY_F3,
    KB_KEY_F4,
    KB_KEY_F5,
    KB_KEY_F6,
    KB_KEY_F7,
    KB_KEY_F8,
    KB_KEY_F9,
    KB_KEY_F10,
    KB_KEY_F11,
    KB_KEY_F12,

    // Make key modifiers easier to detect by giving them easy values
    KB_KEY_LSHIFT = 250,
    KB_KEY_RSHIFT = 251,
    KB_KEY_LCTRL = 252,
    KB_KEY_RCTRL = 253,
    KB_KEY_LALT = 254,
    KB_KEY_RALT = 255,
};

enum {
    KB_PRESS,
    KB_RELEASE,
};

enum {
    KB_MOD_NONE = 0x00,
    KB_MOD_SHIFT = 0x01,
    KB_MOD_CTRL = 0x02,
    KB_MOD_ALT = 0x04,
};

struct kb_key {
    u8  raw;
    u8  scancode;
    u8  keycode;
    int event;
    int modifiers;
};

typedef int (*kb_listener_func)(const struct kb_key *);

int kb_init(void);
int kb_set_typematic_config(int repeat_rate, int typematic_delay);
int kb_add_listener(int (*func)(const struct kb_key *));
int kb_remove_listener(int (*func)(const struct kb_key *));

#endif /* _INC_KB */
