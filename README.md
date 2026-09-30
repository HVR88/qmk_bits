
# Matching Original Apple Keyboards Fully with QMK

See fauxpark's GIST here: https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4?permalink_comment_id=6397510#gistcomment-6397510


## Requirement: Apple VID 0x05AC and compatible PID (examples: 0x021D or 0x0320 or ...)

## Apple Fn in QMK: Must be `FF/03`, not Globe

When implementing an Apple-style **Fn** key in QMK, the accurate HID usage is:

    Usage Page: 0xFF   (AppleVendorTopCase)
    Usage:      0x03   (KeyboardFn)

In other words:

    AppleVendorTopCase / KeyboardFn = FF/03

This is distinct/different from the **Globe** key.

(Some) Modern Apple keyboards label the physical key `fn` / 🌐, which can make it tempting to treat Globe and Fn as interchangeable - they're not. At the HID level, Apple's Fn processing only recognizes `FF/03` as the real Apple Fn key.

### Why this matters

With a keyboard configuration that macOS recognizes through an appropriate Apple keyboard personality, `FF/03` participates in the native Apple Fn behavior, including mappings such as:

    F1 to F12  → all of Apple's normal media-keys
    Fn + F1 to F12  → Real Function Keys

    Fn + Left   → Home
    Fn + Right  → End
    Fn + Up     → Page Up
    Fn + Down   → Page Down

    Fn + n → Notification Center
    etc...

**An Apple VID/PID is required for native Fn behavior - there's no way around it.**

On a generic VID/PID, macOS does not provide the Apple FnKeyboardUsageMap / FnFunctionUsageMap. With a compatible Apple VID/PID, macOS applies the appropriate Apple keyboard personality and supplies those mappings.

With real Apple VID/PID, the keyboard matches an suitable Apple keyboard personality, macOS supplies those Fn maps and `FF/03` functions as the native Fn modifier.

So there are two separate requirements:

1. **Fn key representation:** `AppleVendorTopCase / KeyboardFn` (`FF/03`)
2. **macOS keyboard configuration:** a driver/personality that supplies Apple's Fn mappings

#### For QMK configurations using an Apple-compatible VID/PID, `FF/03` is the required HID representation for a physical Apple-style Fn key. Using Globe is not equivalent and will never work to match real Apple keyboard Fn behavior.
