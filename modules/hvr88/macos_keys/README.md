# Pingato macOS Keys

Reusable QMK Community Module for macOS-specific keyboard actions.

## Keycodes

- `AFN(kc)` — Apple Fn + basic key on one mapping, same shape as `C(kc)` / `SCMD(kc)` (e.g. `AFN(KC_S)`)
- `MAC_KEY_GLOBE` — native Globe
- `MAC_KEY_FN` — native Apple Fn
- `MAC_FN_GLOBE` — firmware hybrid: tap for Globe, hold or chord for native Apple Fn (see below)
- `MAC_FN_QMK_FN` — native Apple Fn plus a QMK Fn layer (see below)
- `MAC_DICTATION` — Apple Dictation
- `MAC_DND` — Apple Do Not Disturb
- `MAC_MISSION_CONTROL` — Mission Control
- `MAC_APPS` — Apps
- `MAC_SCRNSHOT` — Shift-Command-3
- `MAC_SCRNSHOT_CB` — Control-Shift-Command-3
- `MAC_SCRNSHOT_AREA` — Shift-Command-4
- `MAC_SCRNSHOT_AREA_CB` — Control-Shift-Command-4
- `MAC_SCRNSHOT_OPT` — Shift-Command-5
- `MAC_SCRNSHOT_OPT_CB` — Control-Shift-Command-5
- `MAC_SIRI_AREA` — Shift-Command-6
- `MAC_SIRI_WINDOW` — Shift-Command-Space

Screenshot and Siri actions self-cancel with Escape when the same action is issued again within 2 seconds.

`MAC_MISSION_CONTROL` and `MAC_APPS` send the Mission Control and Apps consumer usages for as long as the key is held.

## VIA

VIA names custom keys only in the `QK_KB_0`–`QK_KB_31` range, which community module keycodes cannot use. Map VIA custom keycodes onto module keycodes in `config.h`. The `customKeycodes` array order in the VIA definition must match these indices:

| Index | Define | Module keycode |
|------:|--------|----------------|
| 0 | `MAC_MISSION_CONTROL_VIA_KEYCODE` | `MAC_MISSION_CONTROL` |
| 1 | `MAC_APPS_VIA_KEYCODE` | `MAC_APPS` |
| 2 | `MAC_KEY_GLOBE_VIA_KEYCODE` | `MAC_KEY_GLOBE` |
| 3 | `MAC_KEY_FN_VIA_KEYCODE` | `MAC_KEY_FN` |
| 4 | `MAC_FN_GLOBE_VIA_KEYCODE` | `MAC_FN_GLOBE` |
| 5 | `MAC_FN_QMK_FN_VIA_KEYCODE` | `MAC_FN_QMK_FN` |
| 6 | `MAC_DICTATION_VIA_KEYCODE` | `MAC_DICTATION` |
| 7 | `MAC_DND_VIA_KEYCODE` | `MAC_DND` |
| 8 | `MAC_SCRNSHOT_VIA_KEYCODE` | `MAC_SCRNSHOT` |
| 9 | `MAC_SCRNSHOT_CB_VIA_KEYCODE` | `MAC_SCRNSHOT_CB` |
| 10 | `MAC_SCRNSHOT_AREA_VIA_KEYCODE` | `MAC_SCRNSHOT_AREA` |
| 11 | `MAC_SCRNSHOT_AREA_CB_VIA_KEYCODE` | `MAC_SCRNSHOT_AREA_CB` |
| 12 | `MAC_SCRNSHOT_OPT_VIA_KEYCODE` | `MAC_SCRNSHOT_OPT` |
| 13 | `MAC_SCRNSHOT_OPT_CB_VIA_KEYCODE` | `MAC_SCRNSHOT_OPT_CB` |
| 14 | `MAC_SIRI_AREA_VIA_KEYCODE` | `MAC_SIRI_AREA` |
| 15 | `MAC_SIRI_WINDOW_VIA_KEYCODE` | `MAC_SIRI_WINDOW` |

Example:

```c
#define MAC_MISSION_CONTROL_VIA_KEYCODE  QK_KB_0
#define MAC_APPS_VIA_KEYCODE             QK_KB_1
#define MAC_KEY_GLOBE_VIA_KEYCODE        QK_KB_2
/* …through MAC_SIRI_WINDOW_VIA_KEYCODE QK_KB_15 */
```

A mapped VIA keycode behaves exactly like the module keycode, including with the Fn keys.

## Fn keys

Native Apple Fn is the vendor-defined HID usage `KC_APPLE_FN`. macOS only gives it native behavior for Apple-recognized VID/PIDs.

### `AFN(kc)`

Single-key Apple Fn chord, analogous to `C(KC_V)`:

```c
AFN(KC_S)   // Fn+S for as long as the key is held
```

Press registers Apple Fn then the basic key; release unregisters in reverse. `kc` must be a basic keycode (same class as the low byte of `C(kc)`).

The module offers three hold-key ways to use raw Apple Fn as well:

### `MAC_KEY_FN`

Native Apple Fn and nothing else. Press registers `KC_APPLE_FN`, release unregisters it. There is no QMK layer and no firmware Globe; standalone Fn behavior (Emoji, input source, Dictation, …) and Fn chords are left to macOS.

### `MAC_FN_QMK_FN`

Native Apple Fn plus the QMK layer selected by `MAC_FN_QMK_FN_LAYER`, both for as long as the key is held. There is no firmware Globe and no tap/hold distinction; standalone Fn behavior is left to macOS.

Firmware actions on that layer are the exception: for them Apple Fn is lifted (see [Firmware chords](#firmware-chords)) and stays lifted until the next key that reaches the host, which gets Apple Fn again.

Define the layer in `config.h`:

```c
#define MAC_FN_QMK_FN_LAYER 1
```

### `MAC_FN_GLOBE`

An optional hybrid created by this module, not Apple's own Fn behavior:

- Tap (released before the hold term, no other key) — the module sends a Globe tap.
- Held past the hold term — native Apple Fn is registered; releasing it sends nothing else.
- Held while pressing a key that reaches the host — Apple Fn is registered before that key, so macOS sees a native Fn chord.
- Held while pressing a firmware action (screenshot/Siri keys, or keys that never reach the host such as RGB or layer keys) — the combination is handled in firmware and Apple Fn is not exposed for it. Releasing the key afterwards sends no Globe.

The hold term defaults to `TAPPING_TERM`. Override it with `MAC_FN_GLOBE_HOLD_TERM`. `MAC_FN_GLOBE` does not activate a QMK layer.

## Firmware chords

While `MAC_FN_GLOBE` or `MAC_FN_QMK_FN` is held:

- `MAC_SCRNSHOT_AREA` sends Control-Shift-Command-4 (area to clipboard) instead. It self-cancels as the clipboard variant, separately from the plain screenshot.
- Screenshot and Siri chords are sent without Apple Fn. If Apple Fn is down, it is released while the chord's modifiers are held and before the chord's key, so the chord never carries Fn and macOS never sees a bare Fn press and release (which it treats as a Globe press).

`MAC_KEY_FN` is raw Apple Fn and is not changed by firmware chords.

## Core dependency

This module currently requires the QMK core additions used by HVR88's QMK fork: `KC_GLOBE` and `KC_APPLE_FN`. The additional basic Apple keycodes `KC_DICTATION` and `KC_DO_NOT_DISTURB` are available from that same core patch. `MAC_MISSION_CONTROL` and `MAC_APPS` use stock QMK consumer keycodes.
