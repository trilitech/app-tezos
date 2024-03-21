/* Tezos Ledger application - Public key APDU instruction handling

   Copyright 2024 TriliTech <contact@trili.tech>
   Copyright 2024 Functori <contact@functori.com>
   Copyright 2023 Ledger
   Copyright 2019 Obsidian Systems

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#include "apdu_pubkey.h"

#include "apdu.h"
#include "cx.h"
#include "globals.h"
#include "keys.h"
#include "protocol.h"
#include "to_string.h"
#include "ui.h"
#include "ui_pubkey.h"
#include "baking_auth.h"

#include <string.h>

/**
 * @brief Sends apdu response with the public key
 *
 * @return true
 */
static bool pubkey_ok(void) {
    cx_ecfp_public_key_t public_key = {0};
    generate_public_key(&public_key,
                        global.path_with_curve.derivation_type,
                        &global.path_with_curve.bip32_path);
    provide_pubkey(&public_key);
    return true;
}

/**
 * @brief Authorizes the public key
 *
 *        Sends apdu response with the public key
 *
 * @return true
 */
static bool baking_ok(void) {
    authorize_baking(global.path_with_curve.derivation_type, &global.path_with_curve.bip32_path);
    return pubkey_ok();
}

int handle_get_public_key(buffer_t *cdata,
                          derivation_type_t derivation_type,
                          bool authorize,
                          bool prompt) {
    check_null(cdata);

    global.path_with_curve.derivation_type = derivation_type;

    if ((cdata->size == 0u) && authorize) {
        copy_bip32_path_with_curve(&global.path_with_curve, &N_data.baking_key);
    } else {
        read_bip32_path(&global.path_with_curve.bip32_path, cdata->ptr, cdata->size);
        if (global.path_with_curve.bip32_path.length == 0u) {
            THROW(EXC_WRONG_LENGTH_FOR_INS);
        }
    }

    cx_ecfp_public_key_t public_key = {0};
    generate_public_key(&public_key,
                        global.path_with_curve.derivation_type,
                        &global.path_with_curve.bip32_path);

    if (!prompt) {
        return provide_pubkey(&public_key);
    } else {
        // INS_PROMPT_PUBLIC_KEY || INS_AUTHORIZE_BAKING
        ui_callback_t cb;
        bool bake;
        if (authorize) {
            cb = baking_ok;
            bake = true;
        } else {
            // INS_PROMPT_PUBLIC_KEY
            cb = pubkey_ok;
            bake = false;
        }
        return prompt_pubkey(bake, cb, reject);
    }
}
