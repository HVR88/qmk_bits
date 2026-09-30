#include "quantum.h"
#include "community_modules.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

#ifndef MAC_FN_GLOBE_HOLD_TERM
#    define MAC_FN_GLOBE_HOLD_TERM TAPPING_TERM
#endif

#define MACOS_KEYS_SELF_CANCEL_TIMEOUT 2000

// Apple Fn is a single HID bit shared by MAC_FN_KEY, MAC_FN_GLOBE and MAC_FN_QMK_FN.
// It is registered when the first owner takes it and unregistered when the last lets go.
enum {
    MAC_FN_OWNER_KEY    = 1 << 0,
    MAC_FN_OWNER_GLOBE  = 1 << 1,
    MAC_FN_OWNER_QMK_FN = 1 << 2,
};

// MAC_FN_GLOBE states. It owns Apple Fn in MAC_FN_NATIVE and only there.
typedef enum {
    MAC_FN_IDLE,     // not held
    MAC_FN_PENDING,  // held, nothing sent to the host yet; release before the hold term is a Globe tap
    MAC_FN_NATIVE,   // held, Apple Fn is down on the host
    MAC_FN_CONSUMED, // held, used by a firmware action; Apple Fn is not owned
} mac_fn_state_t;

static uint8_t        mac_apple_fn_owners;
static uint8_t        mac_fn_key_held;
static mac_fn_state_t mac_fn_state = MAC_FN_IDLE;
static uint8_t        mac_fn_globe_keys_held;
static uint16_t       mac_fn_globe_timer;
static uint8_t        mac_qmk_fn_keys_held;
static uint16_t       mac_last_action_keycode;
static uint16_t       mac_last_action_timer;

static void mac_apple_fn_take(uint8_t owner) {
    if (!mac_apple_fn_owners) register_code(KC_APPLE_FN);
    mac_apple_fn_owners |= owner;
}

static void mac_apple_fn_drop(uint8_t owners) {
    if (!(mac_apple_fn_owners & owners)) return;
    mac_apple_fn_owners &= ~owners;
    if (!mac_apple_fn_owners) unregister_code(KC_APPLE_FN);
}

// Firmware chords sent by this module. Zero means the keycode is not one.
static uint16_t mac_chord(uint16_t keycode) {
    switch (keycode) {
        case MAC_SCRNSHOT:         return SCMD(KC_3);
        case MAC_SCRNSHOT_CB:      return C(SCMD(KC_3));
        case MAC_SCRNSHOT_AREA:    return SCMD(KC_4);
        case MAC_SCRNSHOT_AREA_CB: return C(SCMD(KC_4));
        case MAC_SCRNSHOT_OPT:     return SCMD(KC_5);
        case MAC_SCRNSHOT_OPT_CB:  return C(SCMD(KC_5));
        case MAC_SIRI_AREA:        return S(KC_6);
        case MAC_SIRI_WINDOW:      return SCMD(KC_SPC);
        default:                   return 0;
    }
}

// Action performed instead while MAC_FN_GLOBE or MAC_FN_QMK_FN is held.
static uint16_t mac_fn_variant(uint16_t keycode) {
    switch (keycode) {
        case MAC_SCRNSHOT_AREA: return MAC_SCRNSHOT_AREA_CB;
        default:                return keycode;
    }
}

// Keys whose events reach the host as HID usages. Only these expose Apple Fn
// from MAC_FN_GLOBE or restore it for MAC_FN_QMK_FN; anything else pressed while
// those are held is a firmware combination.
static bool mac_reaches_host(uint16_t keycode) {
    switch (keycode) {
        case KC_NO:
            return false;
        case MAC_GLOBE:
        case MAC_DICTATION:
        case MAC_DND:
            return true;
    }
    return IS_QK_BASIC(keycode) || IS_QK_MODS(keycode) || IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode) ||
           IS_QK_LAYER_MOD(keycode) || IS_QK_ONE_SHOT_MOD(keycode);
}

// macOS treats an Apple Fn down/up with nothing in between as a Globe press,
// and a chord carrying Apple Fn no longer matches its shortcut. So Apple Fn held
// by MAC_FN_GLOBE or MAC_FN_QMK_FN is lifted while the chord's modifiers are
// down, before the key. MAC_FN_KEY is left alone: it is raw Apple Fn.
static void mac_send_chord(uint16_t chord) {
    uint8_t mods = QK_MODS_GET_MODS(chord);
    uint8_t lift = mac_apple_fn_owners & (MAC_FN_OWNER_GLOBE | MAC_FN_OWNER_QMK_FN);

    if (!lift || !mods) {
        tap_code16(chord);
        return;
    }

    uint8_t mods8 = (mods & 0x10) ? (uint8_t)((mods & 0x0F) << 4) : mods;
    register_weak_mods(mods8);
    mac_apple_fn_drop(lift);
    if (lift & MAC_FN_OWNER_GLOBE) {
        mac_fn_state = MAC_FN_CONSUMED;
    }
    tap_code(QK_MODS_GET_BASIC_KEYCODE(chord));
    unregister_weak_mods(mods8);
}

static void mac_self_cancel_action(uint16_t keycode, uint16_t chord) {
    if (mac_last_action_keycode == keycode && timer_elapsed(mac_last_action_timer) < MACOS_KEYS_SELF_CANCEL_TIMEOUT) {
        mac_send_chord(KC_ESC);
        mac_last_action_keycode = 0;
    } else {
        mac_send_chord(chord);
        mac_last_action_keycode = keycode;
        mac_last_action_timer   = timer_read();
    }
}

static void mac_fn_key_press(void) {
    if (!mac_fn_key_held++) mac_apple_fn_take(MAC_FN_OWNER_KEY);
}

static void mac_fn_key_release(void) {
    if (!mac_fn_key_held || --mac_fn_key_held) return;
    mac_apple_fn_drop(MAC_FN_OWNER_KEY);
}

static void mac_fn_globe_press(void) {
    if (mac_fn_globe_keys_held++) return;
    mac_fn_state       = MAC_FN_PENDING;
    mac_fn_globe_timer = timer_read();
}

static void mac_fn_globe_release(void) {
    if (!mac_fn_globe_keys_held || --mac_fn_globe_keys_held) return;

    switch (mac_fn_state) {
        case MAC_FN_PENDING:
            if (timer_elapsed(mac_fn_globe_timer) < MAC_FN_GLOBE_HOLD_TERM) {
                tap_code16(KC_GLOBE);
            }
            break;
        case MAC_FN_NATIVE:
            mac_apple_fn_drop(MAC_FN_OWNER_GLOBE);
            break;
        default:
            break;
    }
    mac_fn_state = MAC_FN_IDLE;
}

static void mac_qmk_fn_press(void) {
    if (mac_qmk_fn_keys_held++) return;
    mac_apple_fn_take(MAC_FN_OWNER_QMK_FN);
#ifdef MAC_FN_QMK_FN_LAYER
    layer_on(MAC_FN_QMK_FN_LAYER);
#endif
}

static void mac_qmk_fn_release(void) {
    if (!mac_qmk_fn_keys_held || --mac_qmk_fn_keys_held) return;
#ifdef MAC_FN_QMK_FN_LAYER
    layer_off(MAC_FN_QMK_FN_LAYER);
#endif
    mac_apple_fn_drop(MAC_FN_OWNER_QMK_FN);
}

// Another key is pressed while MAC_FN_GLOBE and/or MAC_FN_QMK_FN may be held.
static void mac_fn_other_key_pressed(uint16_t keycode) {
    if (keycode == KC_NO) return;

    bool to_host = mac_reaches_host(keycode);

    if (mac_fn_state == MAC_FN_PENDING || mac_fn_state == MAC_FN_CONSUMED) {
        if (to_host) {
            mac_apple_fn_take(MAC_FN_OWNER_GLOBE);
            mac_fn_state = MAC_FN_NATIVE;
        } else {
            mac_fn_state = MAC_FN_CONSUMED;
        }
    }

    // Apple Fn lifted for a firmware chord comes back only with the next host key,
    // so releasing MAC_FN_QMK_FN afterwards is never a bare Fn press and release.
    if (to_host && mac_qmk_fn_keys_held && !(mac_apple_fn_owners & MAC_FN_OWNER_QMK_FN)) {
        mac_apple_fn_take(MAC_FN_OWNER_QMK_FN);
    }
}

bool process_record_macos_keys(uint16_t keycode, keyrecord_t *record) {
    bool pressed = record->event.pressed;

    switch (keycode) {
        case MAC_FN_KEY:
            if (pressed) mac_fn_key_press(); else mac_fn_key_release();
            return false;

        case MAC_FN_GLOBE:
            if (pressed) mac_fn_globe_press(); else mac_fn_globe_release();
            return false;

        case MAC_FN_QMK_FN:
            if (pressed) mac_qmk_fn_press(); else mac_qmk_fn_release();
            return false;
    }

    if (mac_chord(keycode)) {
        if (pressed) {
            if (mac_fn_state != MAC_FN_IDLE || mac_qmk_fn_keys_held) {
                keycode = mac_fn_variant(keycode);
            }
            if (mac_fn_state == MAC_FN_PENDING) {
                mac_fn_state = MAC_FN_CONSUMED;
            }
            mac_self_cancel_action(keycode, mac_chord(keycode));
        }
        return false;
    }

    if (pressed) {
        mac_fn_other_key_pressed(keycode);
    }

    switch (keycode) {
        case MAC_GLOBE:
            if (pressed) tap_code16(KC_GLOBE);
            return false;

        case MAC_DICTATION:
            if (pressed) tap_code16(KC_DICTATION);
            return false;

        case MAC_DND:
            if (pressed) tap_code16(KC_DO_NOT_DISTURB);
            return false;
    }

    return true;
}

void housekeeping_task_macos_keys(void) {
    if (mac_last_action_keycode && timer_elapsed(mac_last_action_timer) >= MACOS_KEYS_SELF_CANCEL_TIMEOUT) {
        mac_last_action_keycode = 0;
    }

    if (mac_fn_state == MAC_FN_PENDING && timer_elapsed(mac_fn_globe_timer) >= MAC_FN_GLOBE_HOLD_TERM) {
        mac_apple_fn_take(MAC_FN_OWNER_GLOBE);
        mac_fn_state = MAC_FN_NATIVE;
    }
}
