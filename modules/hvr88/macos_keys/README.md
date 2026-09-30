# Pingato macOS Keys

Reusable QMK Community Module for macOS-specific keyboard actions.

## Keycodes

- `MAC_GLOBE` — native Apple Globe
- `MAC_APPLE_FN` — native Apple Fn
- `MAC_DICTATION` — Apple Dictation
- `MAC_DND` — Apple Do Not Disturb
- `MAC_GLOBE_FN` — tap for Globe, hold for native Apple Fn
- `MAC_SCRNSHOT` — Shift-Command-3
- `MAC_SCRNSHOT_CB` — Control-Shift-Command-3
- `MAC_SCRNSHOT_AREA` — Shift-Command-4
- `MAC_SCRNSHOT_AREA_CB` — Control-Shift-Command-4
- `MAC_SCRNSHOT_OPT` — Shift-Command-5
- `MAC_SCRNSHOT_OPT_CB` — Control-Shift-Command-5
- `MAC_SIRI_AREA` — Shift-6
- `MAC_SIRI_WINDOW` — Shift-Command-Space

Screenshot and Siri actions self-cancel with Escape when the same action is issued twice consecutively.

Define `MACOS_KEYS_FN_LAYER` to a layer number if `MAC_GLOBE_FN` should also momentarily activate a QMK layer while held. If it is not defined, Globe/Fn still works without changing layers.

## Core dependency

This module currently requires the QMK core additions used by HVR88's QMK fork: `KC_GLOBE` and `KC_APPLE_FN`. The additional basic Apple keycodes `KC_DICTATION` and `KC_DO_NOT_DISTURB` are available from that same core patch.
