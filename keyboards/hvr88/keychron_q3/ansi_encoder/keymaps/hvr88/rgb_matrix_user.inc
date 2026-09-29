RGB_MATRIX_EFFECT(HVR88_REACTIVE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

extern bool hvr88_red_base;

bool HVR88_REACTIVE(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    const uint8_t count = g_last_hit_tracker.count;

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t reactive = 0;

        for (uint8_t j = 0; j < count; j++) {
            const int16_t dx = g_led_config.point[i].x - g_last_hit_tracker.x[j];
            const int16_t dy = g_led_config.point[i].y - g_last_hit_tracker.y[j];
            const uint8_t dist = sqrt16(dx * dx + dy * dy);

            const uint16_t tick = scale16by8(
                g_last_hit_tracker.tick[j],
                qadd8(rgb_matrix_config.speed, 1)
            );

            uint16_t effect = tick + dist * 5;
            if (effect > 255) {
                effect = 255;
            }

            reactive = qadd8(reactive, 255 - effect);
        }

        reactive = scale8(reactive, rgb_matrix_config.hsv.v);

        hsv_t active_hsv = rgb_matrix_config.hsv;
        active_hsv.v = reactive;
        const rgb_t active = rgb_matrix_hsv_to_rgb(active_hsv);

        if (hvr88_red_base) {
            /*
             * Caps Lock or Shift:
             *   idle       = red
             *   key effect = the normal configured reactive color over red
             */
            const uint8_t mix = reactive;
            const uint8_t r = qadd8(scale8(200, 255 - mix), scale8(active.r, mix));
            const uint8_t g = scale8(active.g, mix);
            const uint8_t b = scale8(active.b, mix);

            rgb_matrix_set_color(i, r, g, b);
        } else {
            /*
             * Normal mode: same black-background reactive rendering as
             * SOLID_REACTIVE_MULTIWIDE.
             */
            rgb_matrix_set_color(i, active.r, active.g, active.b);
        }
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
