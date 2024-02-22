#include "apdu_pubkey.h"

#include "apdu.h"
#include "cx.h"
#include "globals.h"
#include "keys.h"
#include "protocol.h"
#include "to_string.h"
#include "ui.h"
#include "baking_auth.h"

#include <string.h>

static bool pubkey_ok(void) {
    cx_ecfp_public_key_t public_key = {0};
    generate_public_key(&public_key,
                        global.path_with_curve.derivation_type,
                        &global.path_with_curve.bip32_path);
    delayed_send(provide_pubkey(G_io_apdu_buffer, &public_key));
    return true;
}

static bool baking_ok(void) {
    authorize_baking(global.path_with_curve.derivation_type, &global.path_with_curve.bip32_path);
    pubkey_ok();
    return true;
}

char const *const *get_baking_prompts() {
    static const char *const baking_prompts[] = {
        PROMPT("Authorize Baking"),
        PROMPT("Public Key Hash"),
        NULL,
    };
    return baking_prompts;
}

size_t handle_apdu_get_public_key(uint8_t instruction, volatile uint32_t *flags) {
    uint8_t *dataBuffer = G_io_apdu_buffer + OFFSET_CDATA;

    if (G_io_apdu_buffer[OFFSET_P1] != 0) THROW(EXC_WRONG_PARAM);

    // do not expose pks without prompt through U2F (permissionless legacy comm in browser)
    if (instruction == INS_GET_PUBLIC_KEY) require_permissioned_comm();

    global.path_with_curve.derivation_type = parse_derivation_type(G_io_apdu_buffer[OFFSET_CURVE]);

    size_t const cdata_size = G_io_apdu_buffer[OFFSET_LC];

    if (cdata_size == 0 && instruction == INS_AUTHORIZE_BAKING) {
        copy_bip32_path_with_curve(&global.path_with_curve, &N_data.baking_key);
    } else {
        read_bip32_path(&global.path_with_curve.bip32_path, dataBuffer, cdata_size);
        if (global.path_with_curve.bip32_path.length == 0) THROW(EXC_WRONG_LENGTH_FOR_INS);
    }

    cx_ecfp_public_key_t public_key = {0};
    generate_public_key(&public_key,
                        global.path_with_curve.derivation_type,
                        &global.path_with_curve.bip32_path);

    if (instruction == INS_GET_PUBLIC_KEY) {
        return provide_pubkey(G_io_apdu_buffer, &public_key);
    } else {
        // instruction == INS_PROMPT_PUBLIC_KEY || instruction == INS_AUTHORIZE_BAKING
        ui_callback_t cb;
        bool bake;
        if (instruction == INS_AUTHORIZE_BAKING) {
            cb = baking_ok;
            bake = true;
        } else {
            // INS_PROMPT_PUBLIC_KEY
            cb = pubkey_ok;
            bake = false;
        }
        prompt_address(bake, cb, delay_reject);
        *flags = IO_ASYNCH_REPLY;
        return 0;
    }
}
