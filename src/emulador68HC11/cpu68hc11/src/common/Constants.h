/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>
#include <cstdint>

constexpr size_t ROM_SIZE = 4096;
constexpr size_t ROM_BITS = 98;
constexpr size_t RAM_SIZE = 0x10000;

// ROM CONSTANTS ELEMENTS

constexpr int PRUEBA_SHIFT = 93;

constexpr int VF_SHIFT = 92;

constexpr int MI_SHIFT = 90;

constexpr int LIGA_SHIFT = 78;

constexpr int NCRI_SHIFT = 77;

constexpr int EB_SHIFT = 75;

constexpr int NWB_SHIFT = 74;

constexpr int EA_SHIFT = 72;

constexpr int NWA_SHIFT = 71;

constexpr int SELBUS_SHIFT = 70;

constexpr int DESTUPA_SHIFT = 67;

constexpr int FUNCTNUPA_SHIFT = 64;

constexpr int SOURCEUPA_SHIFT = 60;

constexpr int NOEUPA_SHIFT = 59;

constexpr int NDUPA_SHIFT = 58;

constexpr int SELMUX_SHIFT = 57;

constexpr int NEX_SHIFT = 54;

constexpr int X_SHIFT = 51;

constexpr int ENAY_SHIFT = 50;

constexpr int NERA_SHIFT = 47;

constexpr int RA_SHIFT = 44;

constexpr int NEAP_SHIFT = 41;

constexpr int AP_SHIFT = 38;

constexpr int NEPC_SHIFT = 35;

constexpr int PC_SHIFT = 32;

constexpr int NCBD_SHIFT = 31;

constexpr int INTERRUPT_SHIFT = 27;

constexpr int NAS_SHIFT = 26;

constexpr int NRW_SHIFT = 25;

constexpr int BD_SHIFT = 24;

constexpr int DINT_SHIFT = 23;

constexpr int HINT_SHIFT = 22;

constexpr int SIRQ_SHIFT = 21;

constexpr int SXIRQ_SHIFT = 20;

constexpr int B9_SHIFT = 19;

constexpr int B8B7B6_SHIFT = 16;

constexpr int B5B4B3_SHIFT = 13;

constexpr int B2B1_SHIFT = 11;

constexpr int B0_SHIFT = 10;

constexpr int CC_SHIFT = 9;

constexpr int CN_SHIFT = 8;

constexpr int CV_SHIFT = 7;

constexpr int CZ_SHIFT = 6;

constexpr int CI_SHIFT = 5;

constexpr int CH_SHIFT = 4;

constexpr int CX_SHIFT = 3;

constexpr int CS_SHIFT = 2;

constexpr int NHB_SHIFT = 1;

constexpr int ACCSEC_SHIFT = 0;

constexpr int BIT_MASK = 0x01;
constexpr int VECTOR2_MASK = 0x03;
constexpr int VECTOR3_MASK = 0x07;
constexpr int VECTOR4_MASK = 0x0F;
constexpr int VECTOR5_MASK = 0x1F;
constexpr int VECTOR12_MASK = 0xFFF;

#endif
