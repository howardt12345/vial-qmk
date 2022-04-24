/*
Copyright 2022 Howard Tseng

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "numpad.h"
#include "oled_functions.c"
#include <stdio.h>

#ifdef OLED_ENABLE

/* Placement information for display elements */
#    define NUMLOCK_DISPLAY_X 0
#    define NUMLOCK_DISPLAY_Y 0

#    define CAPSLOCK_DISPLAY_X 26
#    define CAPSLOCK_DISPLAY_Y 0

#    define SCROLLLOCK_DISPLAY_X 52
#    define SCROLLLOCK_DISPLAY_Y 0

#    define LAYER_DISPLAY_X 0
#    define LAYER_DISPLAY_Y 0
uint8_t prev_layer = 255;

// RGB info variables
#    if defined RGB_MATRIX_ENABLE || defined RGBLIGHT_ENABLE
char rgb_str[10];
bool rgb_on;
#        define RGB_INFO_DISPLAY_X 80
#        define RGB_INFO_DISPLAY_Y 2
#    endif

// RGB light specific variables
#    ifdef RGBLIGHT_ENABLE
// Following line allows macro to read current RGB lighting settings
extern rgblight_config_t rgblight_config;

uint8_t bkl_mode;
uint8_t bkl_hsv_h;
uint8_t bkl_hsv_s;
uint8_t bkl_hsv_v;
#    endif

// RGB Matrix specific variables
#    ifdef RGB_MATRIX_ENABLE
uint8_t matrix_mode;
uint8_t matrix_hsv_h;
uint8_t matrix_hsv_s;
uint8_t matrix_hsv_v;
#    endif

bool rgb_has_changed(void) {
#    ifdef RGBLIGHT_ENABLE
    if (bkl_mode != rgblight_config.mode || bkl_hsv_h != rgblight_config.hue || bkl_hsv_s != rgblight_config.sat || bkl_hsv_v != rgblight_config.val) {
        return true;
    }
#    endif

#    ifdef RGB_MATRIX_ENABLE
    if (matrix_mode != rgb_matrix_config.mode || matrix_hsv_h != rgb_matrix_config.hsv.h || matrix_hsv_s != rgb_matrix_config.hsv.s || matrix_hsv_v != rgb_matrix_config.hsv.v) {
        return true;
    }
#    endif

    if(rgb_on != (rgblight_is_enabled() || rgb_matrix_config.enable)) {
        rgb_on = rgblight_is_enabled() || rgb_matrix_config.enable;
        return true;
    }

    return false;
}

// gets the text of the last changed RGB setting
void get_rgb_matrix_change(void) {
#    ifdef RGBLIGHT_ENABLE
    if (bkl_mode != rgblight_config.mode) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "BL M:%3d", rgblight_config.mode);
    } else if (bkl_hsv_h != rgblight_config.hue) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "BL H:%3d", rgblight_config.hue);
    } else if (bkl_hsv_s != rgblight_config.sat) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "BL S:%3d", rgblight_config.sat);
    } else if (bkl_hsv_v != rgblight_config.val) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "BL V:%3d", rgblight_config.val);
    }

    bkl_mode  = rgblight_config.mode;
    bkl_hsv_h = rgblight_config.hue;
    bkl_hsv_s = rgblight_config.sat;
    bkl_hsv_v = rgblight_config.val;
#    endif

#    ifdef RGB_MATRIX_ENABLE
    if (matrix_mode != rgb_matrix_config.mode) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "MX M:%3d", rgb_matrix_config.mode);
    } else if (matrix_hsv_h != rgb_matrix_config.hsv.h) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "MX H:%3d", rgb_matrix_config.hsv.h);
    } else if (matrix_hsv_s != rgb_matrix_config.hsv.s) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "MX S:%3d", rgb_matrix_config.hsv.s);
    } else if (matrix_hsv_v != rgb_matrix_config.hsv.v) {
        snprintf(rgb_str, sizeof(rgb_str) + 1, "MX V:%3d", rgb_matrix_config.hsv.v);
    }

    matrix_mode  = rgb_matrix_config.mode;
    matrix_hsv_h = rgb_matrix_config.hsv.h;
    matrix_hsv_s = rgb_matrix_config.hsv.s;
    matrix_hsv_v = rgb_matrix_config.hsv.v;
#    endif
}

bool layer_has_changed(void) {
    uint8_t current_layer = get_highest_layer(layer_state);
    return prev_layer != current_layer;
}

// draws indicators for the current keyboard layer
void draw_keyboard_layers(void) {
    uint8_t highest_layer = get_highest_layer(layer_state);
    prev_layer = highest_layer;
    if (highest_layer < 4) {
        draw_rect_filled_soft(LAYER_DISPLAY_X + highest_layer * 12, LAYER_DISPLAY_Y, 11, 11, true);
    } else {
        // indicator for layers above 3
        draw_rect_filled_soft(LAYER_DISPLAY_X + 48, LAYER_DISPLAY_Y, 11, 11, true);
        write_char_at_pixel_xy(LAYER_DISPLAY_X + 3 + 48, LAYER_DISPLAY_Y + 2, highest_layer + 0x30, highest_layer > 3);
    }

    // indicators for layers 0-3
    write_char_at_pixel_xy(LAYER_DISPLAY_X + 3, LAYER_DISPLAY_Y + 2, '0', highest_layer == 0);
    write_char_at_pixel_xy(LAYER_DISPLAY_X + 3 + 12, LAYER_DISPLAY_Y + 2, '1', highest_layer == 1);
    write_char_at_pixel_xy(LAYER_DISPLAY_X + 3 + 24, LAYER_DISPLAY_Y + 2, '2', highest_layer == 2);
    write_char_at_pixel_xy(LAYER_DISPLAY_X + 3 + 36, LAYER_DISPLAY_Y + 2, '3', highest_layer == 3);
}

// draws the num, caps, and scroll lock indicators
void draw_keyboard_locks(void) {
    led_t led_state = host_keyboard_led_state();
    draw_text_rectangle(NUMLOCK_DISPLAY_X, NUMLOCK_DISPLAY_Y, 5 + (3 * 6), "NUM", led_state.num_lock);
    draw_text_rectangle(CAPSLOCK_DISPLAY_X, CAPSLOCK_DISPLAY_Y, 5 + (3 * 6), "CAP", led_state.caps_lock);
    draw_text_rectangle(SCROLLLOCK_DISPLAY_X, SCROLLLOCK_DISPLAY_Y, 5 + (3 * 6), "SCR", led_state.scroll_lock);
}

// draws the last changed RGB setting
#    if defined RGBLIGHT_ENABLE || defined RGB_MATRIX_ENABLE
void draw_rgb_matrix_change(void) {
    get_rgb_matrix_change();
    if (rgblight_is_enabled() || rgb_matrix_config.enable) {
        write_chars_at_pixel_xy(RGB_INFO_DISPLAY_X, RGB_INFO_DISPLAY_Y, rgb_str, false);
    } else {
        write_chars_at_pixel_xy(RGB_INFO_DISPLAY_X, RGB_INFO_DISPLAY_Y, " RGB:OFF", false);
    }
}
#    endif

bool should_redraw(void) {
    return layer_has_changed() || rgb_has_changed();
}

// OLED draw function
void redraw_oled(void) {
    oled_clear();
    draw_keyboard_layers();
    // draw dividing line
    draw_line_h(0, 15, 128, true);

#    if defined RGBLIGHT_ENABLE || defined RGB_MATRIX_ENABLE
    draw_rgb_matrix_change();
#    endif
}

bool oled_task_kb(void) {
    if(should_redraw()) {
        redraw_oled();
    }

    return false;
}

#endif
