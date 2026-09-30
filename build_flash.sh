#!/bin/bash
set -e

BITS="$HOME/Code/QMK/qmk_bits"
QMK="$HOME/Code/QMK/qmk_firmware"
KEYBOARD="hvr88/keychron_q3/ansi_encoder"
KEYMAP="hvr88"

pull() {
    echo "==> Pulling qmk_bits"
    git -C "$BITS" pull
}

copy() {
    echo "==> Copying keyboard and modules"
    cp -R "$BITS/keyboards/hvr88/." "$QMK/keyboards/hvr88/"
    mkdir -p "$QMK/modules"
    cp -R "$BITS/modules/." "$QMK/modules/"
}

compile() {
    echo "==> Compiling"
    cd "$QMK"
    qmk compile -kb "$KEYBOARD" -km "$KEYMAP"
}

flash() {
    echo "==> Flashing"
    BIN="$QMK/hvr88_keychron_q3_ansi_encoder_hvr88.bin"

    if [ ! -f "$BIN" ]; then
        echo "ERROR: Firmware binary not found: $BIN"
        echo "Run --compile or --all first."
        exit 1
    fi

    echo "Waiting for STM32 DFU bootloader (Ctrl+C to cancel)..."
    until dfu-util -l 2>/dev/null | grep -q '0483:df11'; do
        sleep 0.5
    done

    dfu-util -a 0 -s 0x08000000:leave -D "$BIN"
}

case "${1:---all}" in
    --flash)
        flash
        ;;
    --pull)
        pull
        ;;
    --compile)
        pull
        copy
        compile
        ;;
    --all)
        pull
        copy
        compile
        flash
        ;;
    *)
        echo "Usage: $(basename "$0") [--flash|--pull|--compile|--all]"
        exit 1
        ;;
esac
