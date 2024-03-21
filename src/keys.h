/* Tezos Ledger application - Key handling

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

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "exception.h"
#include "memory.h"
#include "os_cx.h"
#include "types.h"

/**
 * @brief Raw representation of bip32 path
 *
 */
struct bip32_path_wire {
    uint8_t length;          ///< length of the path
    uint32_t components[0];  ///< pointer to the component array
} __attribute__((packed));

/**
 * @brief Reads a bip32_path
 *
 *        Can throw exception
 *
 * @param out: bip32_path output
 * @param in: input data
 * @param in_size: input size
 * @return size_t: number of byte read
 */
size_t read_bip32_path(bip32_path_t *const out, uint8_t const *const in, size_t const in_size);

/**
 * @brief Generates a private/public key pair from a bip32 path and a curve
 *
 * @param key_pair: private/public key pair output
 * @param derivation_type: curve
 * @param bip32_path: bip32 path
 * @return int: error, 0 if none
 */
int generate_key_pair(key_pair_t *key_pair,
                      derivation_type_t const derivation_type,
                      bip32_path_t const *const bip32_path);

/**
 * @brief Extract the public key hash from a public key and a curve
 *
 *        Is non-reentrant
 *
 * @param hash_out: public key hash output
 * @param hash_out_size: output size
 * @param compressed_out: compressed public key output
 *                        pass NULL if this value is not desired
 * @param derivation_type: curve
 * @param public_key: public key
 */
void public_key_hash(uint8_t *const hash_out,
                     size_t const hash_out_size,
                     cx_ecfp_public_key_t *const compressed_out,
                     derivation_type_t const derivation_type,
                     cx_ecfp_public_key_t const *const restrict public_key);

/**
 * @brief Signs a message  with a key
 *
 * @param out: signature output
 * @param out_size: output size
 * @param derivation_type: key derivation_type
 * @param key: key
 * @param in: message input
 * @param in_size: input size
 * @return size_t: size of the signature
 */
size_t sign(uint8_t *const out,
            size_t const out_size,
            derivation_type_t const derivation_type,
            key_pair_t const *const key,
            uint8_t const *const in,
            size_t const in_size);

/**
 * @brief Reads a curve code from wire-format and parse into `deviration_type`
 *
 * @param curve_code: curve code
 * @return derivation_type_t: derivation_type result
 */
static inline derivation_type_t parse_derivation_type(uint8_t const curve_code) {
    switch (curve_code) {
        case 0:
            return DERIVATION_TYPE_ED25519;
        case 1:
            return DERIVATION_TYPE_SECP256K1;
        case 2:
            return DERIVATION_TYPE_SECP256R1;
        case 3:
            return DERIVATION_TYPE_BIP32_ED25519;
        default:
            return DERIVATION_TYPE_UNSET;
    }
}

/**
 * @brief Converts `derivation_type` to wire-format.
 *
 * @param derivation_type: curve
 * @return uint8_t: curve code result
 */
static inline uint8_t unparse_derivation_type(derivation_type_t const derivation_type) {
    switch (derivation_type) {
        case DERIVATION_TYPE_ED25519:
            return 0;
        case DERIVATION_TYPE_SECP256K1:
            return 1;
        case DERIVATION_TYPE_SECP256R1:
            return 2;
        case DERIVATION_TYPE_BIP32_ED25519:
            return 3;
        default:
            THROW(EXC_REFERENCED_DATA_NOT_FOUND);
    }
}

/**
 * @brief Converts `derivation_type` to `signature_type`
 *
 * @param derivation_type: derivation_type
 * @return signature_type_t: signature_type result
 */
static inline signature_type_t derivation_type_to_signature_type(
    derivation_type_t const derivation_type) {
    switch (derivation_type) {
        case DERIVATION_TYPE_SECP256K1:
            return SIGNATURE_TYPE_SECP256K1;
        case DERIVATION_TYPE_SECP256R1:
            return SIGNATURE_TYPE_SECP256R1;
        case DERIVATION_TYPE_ED25519:
            return SIGNATURE_TYPE_ED25519;
        case DERIVATION_TYPE_BIP32_ED25519:
            return SIGNATURE_TYPE_ED25519;
        default:
            return SIGNATURE_TYPE_UNSET;
    }
}

/**
 * @brief Converts `signature_type` to `cx_curve`
 *
 * @param signature_type: signature_type
 * @return cx_curve_t: curve result
 */
static inline cx_curve_t signature_type_to_cx_curve(signature_type_t const signature_type) {
    switch (signature_type) {
        case SIGNATURE_TYPE_SECP256K1:
            return CX_CURVE_SECP256K1;
        case SIGNATURE_TYPE_SECP256R1:
            return CX_CURVE_SECP256R1;
        case SIGNATURE_TYPE_ED25519:
            return CX_CURVE_Ed25519;
        default:
            return CX_CURVE_NONE;
    }
}

/**
 * @brief Generates a public key from a bip32 path and a curve
 *
 * @param public_key: public key output
 * @param derivation_type: curve
 * @param bip32_path: bip32 path
 * @return int: error, 0 if none
 */
int generate_public_key(cx_ecfp_public_key_t *public_key,
                        derivation_type_t const derivation_type,
                        bip32_path_t const *const bip32_path);
