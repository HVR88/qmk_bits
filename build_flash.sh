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
    cd "$QMK"
    qmk flash -kb "$KEYBOARD" -km "$KEYMAP"
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
