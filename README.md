
# Matching Original Apple Keyboards Fully with QMK

See fauxpark's GIST here: https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4?permalink_comment_id=6397510#gistcomment-6397510
<br>
<br>
> [!IMPORTANT]
>### Fn support, and other non-standard (vendor-defined) HID usages, are hard-coded into macOS.
>#### They're enabled by macOS only for specific Apple VID/PID combinations recognized during USB enumeration.


### The Two Hard Requirements:
* **Apple VID 0x05AC and a compatible PID (examples: 0x021D or 0x0320 or ...)**
* **Apple Fn in QMK: Must be `FF/03`, not Globe**

---
### The Details:

#### When implementing an Apple-style **Fn** key in QMK, the accurate HID usage is:
    Usage Page: 0xFF   (AppleVendorTopCase)
    Usage:      0x03   (KeyboardFn)

#### In other words:
    AppleVendorTopCase / KeyboardFn = FF/03

This is distinct/different from the **Globe** key.

(Some) Modern Apple keyboards label the physical key `fn` / 🌐, which can make it tempting to treat Globe and Fn as interchangeable - _they're not_. At the HID level, Apple's Fn processing only recognizes `FF/03` as the real Apple Fn key for Apple keyboards, while Globe is a standard usage code accepted for third-party keyboards

### Why this matters

With a keyboard configuration that macOS recognizes through an appropriate Apple keyboard personality, `FF/03` participates in the native Apple Fn behavior, with the mappings below. Globe does not.

> [!NOTE]
>
> | Keys | Results |
> | --- | --- |
> | **F1 to F12** | Apple's normal media-keys (except DICTATION and DO NOT DISTURB) **\*** |
> | **Fn + F1 to F12** | Real Function Keys |
> | **Fn + Left** | Home |
> | **Fn + Right** | End |
> | **Fn + Up** | Page Up |
> | **Fn + Down** | Page Down |
> | **Fn + Delete (Backspace)** | DEL (Forward Delete) |
> | **Fn + A** | Dock |
> | **Fn + C** | Control Center |
> | **Fn + D** | Dictation |
> | **Fn + E** | Emoji & Symbols |
> | **Fn + F** | Full Screen |
> | **Fn + H** | Show Desktop |
> | **Fn + M** | Mission Control |
> | **Fn + N** | Notification Center |
> | **Fn + Q** | Quick Note |
> | **Fn + S** | Search / Siri |>


**An Apple VID/PID is required for native Fn behavior - there's no way around it.**

On a generic VID/PID, macOS does not provide the Apple FnKeyboardUsageMap / FnFunctionUsageMap. With Apple VID/PID, macOS applies an appropriate Apple keyboard personality and supplies the mappings to enable the Fn modifier.

### Requirements Summary:

1. **Fn key representation:** `AppleVendorTopCase / KeyboardFn` (`FF/03`) - _this can be controlled in keyboard firmware_
2. **macOS keyboard configuration:** a driver/personality that supplies Apple's Fn mappings - _this can only be controlled by system software_

---

> [!NOTE]
> <strong>*</strong> the only two codes that are not automatically handled by the system are DICTATION (F5) and DO_NO_DISTURB (F6) - they just aren't enabled by default on any known Apple VID/PID combination. To get them, use drashna's extra_extra_key module: https://github.com/drashna/qmk_modules/tree/main/extra_extra_key

