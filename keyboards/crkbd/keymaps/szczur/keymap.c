#include QMK_KEYBOARD_H
#include <stdio.h>
#include <stdlib.h>
#include "arasaka_gfx.h"
#include "anime_nod.h"

// --- YOUR 3X5 KEYMAP ---
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x5_3(
        KC_Q,             KC_W,             KC_E,             KC_R,             KC_T,                               KC_Y,             KC_U,             KC_I,             KC_O,             KC_P,
        LGUI_T(KC_A),     LALT_T(KC_S),     LCTL_T(KC_D),     LSFT_T(KC_F),     KC_G,                               KC_H,             RSFT_T(KC_J),     RCTL_T(KC_K),     RALT_T(KC_L),     LGUI_T(KC_M),
        KC_Z,             KC_X,             KC_C,             KC_V,             KC_B,                               KC_N,             KC_SCLN,          KC_COMM,          KC_DOT,           KC_SLSH,
                                              LT(1, KC_SPC),    LT(2, KC_TAB),    LT(3, KC_ESC),                    LT(3, KC_DEL),    LT(2, KC_BSPC),   LT(1, KC_ENT)
    ),
    [1] = LAYOUT_split_3x5_3(
        KC_EXLM,          KC_AT,            KC_HASH,          KC_LPRN,          KC_RPRN,                            KC_GRV,           KC_TILD,          KC_MINS,          KC_UNDS,          KC_NO,
        KC_DLR,           KC_PERC,          KC_CIRC,          KC_LBRC,          KC_RBRC,                            KC_LEFT,          KC_DOWN,          KC_UP,            KC_RGHT,          KC_QUOT,
        KC_AMPR,          KC_ASTR,          KC_PLUS,          KC_LCBR,          KC_RCBR,                            KC_BSLS,          KC_PIPE,          KC_EQL,           KC_DQUO,          KC_NO,
                                              KC_NO,            KC_NO,            KC_NO,                             KC_NO,            KC_NO,            KC_NO
    ),
    [2] = LAYOUT_split_3x5_3(
        KC_NO,            KC_1,             KC_2,             KC_3,             KC_NO,                              KC_NO,            KC_NO,            KC_NO,            KC_NO,            KC_NO,
        KC_NO,            KC_4,             KC_5,             KC_6,             KC_NO,                              KC_NO,            KC_RSFT,          KC_RCTL,          KC_LALT,          KC_LGUI,
        KC_NO,            KC_7,             KC_8,             KC_9,             KC_NO,                              KC_NO,            KC_NO,            KC_NO,            KC_NO,            KC_NO,
                                              KC_0,             KC_NO,            KC_NO,                             KC_NO,            KC_NO,            KC_NO
    ),
    [3] = LAYOUT_split_3x5_3(
        KC_NO,            KC_F1,            KC_F2,            KC_F3,            KC_F10,                             KC_NO,            KC_NO,            KC_NO,            KC_NO,            KC_NO,
        KC_NO,            KC_F4,            KC_F5,            KC_F6,            KC_F11,                             KC_NO,            KC_NO,            KC_NO,            KC_NO,            KC_NO,
        KC_NO,            KC_F7,            KC_F8,            KC_F9,            KC_F12,                             KC_NO,            KC_NO,            KC_NO,            KC_NO,            KC_NO,
                                              KC_NO,            KC_NO,            KC_NO,                             KC_NO,            KC_NO,            KC_NO
    )
};

// --- ANIMATION STATE ---
static bool glitch = true;
static bool dirty = false;
static uint8_t frame_count = 15;
static bool show_text = true;
static uint16_t arasaka_timer;

static uint32_t nod_timer = 0;
static uint8_t nod_frame = 0;

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

void arasaka_text_clean(void) {
    oled_write_raw_P(text_clean, frame_size);
}

void arasaka_text_glitch_dirty(void) {
    oled_write_raw_P(text_glitch_dirty[rand() % text_glitch_dirty_count], frame_size);
}

void arasaka_text_glitch(bool can_be_dirty) {
    uint8_t frame = can_be_dirty
        ? rand() % (text_glitch_count + text_glitch_dirty_count)
        : rand() % text_glitch_count;

    if (frame < text_glitch_count) {
        oled_write_raw_P(text_glitch[frame], frame_size);
        return;
    }

    arasaka_text_glitch_dirty();
}

void arasaka_logo_clean(void) {
    oled_write_raw_P(logo_clean, frame_size);
}

void arasaka_logo_glitch_dirty(void) {
    oled_write_raw_P(logo_glitch_dirty[rand() % logo_glitch_dirty_count], frame_size);
}

void arasaka_logo_glitch(bool can_be_dirty) {
    uint8_t frame = can_be_dirty
        ? rand() % (logo_glitch_count + logo_glitch_dirty_count)
        : rand() % logo_glitch_count;

    if (frame < logo_glitch_count) {
        oled_write_raw_P(logo_glitch[frame], frame_size);
        return;
    }

    arasaka_logo_glitch_dirty();
}

void arasaka_draw(void) {
    uint16_t timer = timer_elapsed(arasaka_timer);

    if (timer < 150) {
        show_text ? arasaka_text_glitch_dirty() : arasaka_logo_glitch_dirty();
        return;
    }

    if (timer < 250) {
        show_text ? arasaka_text_glitch(true) : arasaka_logo_glitch(true);
        return;
    }

    if (timer > 9750 && timer < 9850) {
        show_text ? arasaka_text_glitch(true) : arasaka_logo_glitch(true);
        return;
    }

    if (timer > 9850 && timer < 10000) {
        show_text ? arasaka_text_glitch_dirty() : arasaka_logo_glitch_dirty();
        return;
    }

    if (timer > 10000) {
        show_text = !show_text;
        arasaka_timer = timer_read();
    }

    if (glitch && frame_count != 0) {
        frame_count--;
        show_text ? arasaka_text_glitch(true) : arasaka_logo_glitch(true);
        return;
    }

    glitch = false;
    dirty = false;

    show_text ? arasaka_text_clean() : arasaka_logo_clean();

    if (1 == rand() % 60) {
        glitch = true;
        frame_count = 1 + rand() % 4;
        return;
    }

    if (1 == rand() % 60) {
        glitch = true;
        frame_count = 1 + rand() % 10;
        dirty = frame_count > 5;
    }
}

void render_anime_nod(void) {
    uint8_t current_wpm = get_current_wpm();

    uint16_t nod_delay = 500 - (current_wpm * 4);
    if (nod_delay < 50) {
        nod_delay = 50;
    }

    if (timer_elapsed32(nod_timer) > nod_delay) {
        nod_timer = timer_read32();
        nod_frame = (nod_frame + 1) % ANIM_FRAME_COUNT;
    }

    oled_write_raw_P(
        (const char *)pgm_read_ptr(&animation_frames[nod_frame]),
        ANIM_FRAME_SIZE
    );
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        arasaka_draw();
    } else {
        render_anime_nod();
    }
    return false;
}
