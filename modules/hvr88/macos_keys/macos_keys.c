#include "quantum.h"
#include "community_modules.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

static uint16_t mac_globe_fn_timer;
static bool mac_globe_fn_pressed;
static bool mac_apple_fn_active;
static uint16_t mac_last_action_keycode;
static uint16_t mac_last_action_timer;

#define MACOS_KEYS_SELF_CANCEL_TIMEOUT 4000

static bool process_self_cancel_action(uint16_t keycode, uint16_t action) {
    if (mac_last_action_keycode == keycode &&
        timer_elapsed(mac_last_action_timer) < MACOS_KEYS_SELF_CANCEL_TIMEOUT) {
        tap_code(KC_ESC);
        mac_last_action_keycode = 0;
    } else {
        tap_code16(action);
        mac_last_action_keycode = keycode;
        mac_last_action_timer = timer_read();
    }
    return false;
}

bool process_record_macos_keys(uint16_t keycode, keyrecord_t *record) {
    if (mac_globe_fn_pressed &&
        record->event.pressed &&
        keycode != MAC_GLOBE_FN &&
        !mac_apple_fn_active) {
        register_code16(KC_APPLE_FN);
        mac_apple_fn_active = true;
    }

    switch (keycode) {
        case MAC_GLOBE:
            if (record->event.pressed) tap_code16(KC_GLOBE);
            return false;

        case MAC_APPLE_FN:
            if (record->event.pressed) {
                register_code16(KC_APPLE_FN);
            } else {
                unregister_code16(KC_APPLE_FN);
            }
            return false;

        case MAC_DICTATION:
            if (record->event.pressed) tap_code16(KC_DICTATION);
            return false;

        case MAC_DND:
            if (record->event.pressed) tap_code16(KC_DO_NOT_DISTURB);
            return false;

        case MAC_GLOBE_FN:
            if (record->event.pressed) {
                mac_globe_fn_timer = timer_read();
                mac_globe_fn_pressed = true;
                mac_apple_fn_active = false;
#ifdef MACOS_KEYS_FN_LAYER
                layer_on(MACOS_KEYS_FN_LAYER);
#endif
            } else {
#ifdef MACOS_KEYS_FN_LAYER
                layer_off(MACOS_KEYS_FN_LAYER);
#endif
                mac_globe_fn_pressed = false;

                if (mac_apple_fn_active) {
                    unregister_code16(KC_APPLE_FN);
                    mac_apple_fn_active = false;
                } else {
                    tap_code16(KC_GLOBE);
                }
            }
            return false;

        case MAC_SCRNSHOT:
            if (record->event.pressed) return process_self_cancel_action(keycode, SCMD(KC_3));
            return false;
        case MAC_SCRNSHOT_CB:
            if (record->event.pressed) return process_self_cancel_action(keycode, C(SCMD(KC_3)));
            return false;
        case MAC_SCRNSHOT_AREA:
            if (record->event.pressed) return process_self_cancel_action(keycode, SCMD(KC_4));
            return false;
        case MAC_SCRNSHOT_AREA_CB:
            if (record->event.pressed) return process_self_cancel_action(keycode, C(SCMD(KC_4)));
            return false;
        case MAC_SCRNSHOT_OPT:
            if (record->event.pressed) return process_self_cancel_action(keycode, SCMD(KC_5));
            return false;
        case MAC_SCRNSHOT_OPT_CB:
            if (record->event.pressed) return process_self_cancel_action(keycode, C(SCMD(KC_5)));
            return false;
        case MAC_ASKSIRI_AREA:
            if (record->event.pressed) return process_self_cancel_action(keycode, S(KC_6));
            return false;
        case MAC_SIRI_WINDOW:
            if (record->event.pressed) return process_self_cancel_action(keycode, SCMD(KC_SPC));
            return false;
    }

    return true;
}

void housekeeping_task_macos_keys(void) {
    if (mac_last_action_keycode &&
        timer_elapsed(mac_last_action_timer) >= MACOS_KEYS_SELF_CANCEL_TIMEOUT) {
        mac_last_action_keycode = 0;
    }

    if (mac_globe_fn_pressed &&
        !mac_apple_fn_active &&
        timer_elapsed(mac_globe_fn_timer) >= TAPPING_TERM) {
        register_code16(KC_APPLE_FN);
        mac_apple_fn_active = true;
    }
}
