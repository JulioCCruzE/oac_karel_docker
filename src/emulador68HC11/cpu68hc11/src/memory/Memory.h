/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "../common/Constants.h"
#include "../common/Types.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

class DataBus16; // forward declaration
                 //
class Memory {
public:
  Memory() = default;
  RomWord rom[4096]{};

  uint8_t readRam(uint16_t addr) const;
  const uint8_t *ramData() const;
  void writeRam(uint16_t addr, uint8_t value);
  void takeModifiedAddresses(std::vector<uint16_t> &addresses);

  RomWord fetchRom(uint16_t mpc);
  // uint8_t read8(uint16_t addr);
  // void write8(uint16_t addr, uint8_t value);
  void read8(DataBus16 &bus, uint16_t addr, uint8_t nas, uint8_t nrw,
             uint8_t bd);
  void write8(const DataBus16 &bus, uint16_t addr, uint8_t nas, uint8_t nrw,
              uint8_t bd);
  RomWord parseRomWord(const std::string &bits);

  bool loadRomFromVHDL(const std::string &filename);
  bool loadRamFromMIF(const std::string &filename);

  void showRam(uint16_t start, uint16_t length);

  RomWord makeRomWord(uint8_t prueba, uint8_t vf, uint8_t mi, uint16_t liga,
                      uint8_t ncri, uint8_t eb, uint8_t nwb, uint8_t ea,
                      uint8_t nwa, uint8_t selbus, uint8_t dest, uint8_t functn,
                      uint8_t source, uint8_t noeupa, uint8_t ndupa,
                      uint8_t selmux, uint8_t nex, uint8_t x, uint8_t enay,
                      uint8_t nera, uint8_t ra, uint8_t neap, uint8_t ap,
                      uint8_t nepc, uint8_t pc, uint8_t ncbd, uint8_t interrupt,
                      uint8_t nas, uint8_t nrw, uint8_t bd, uint8_t dint,
                      uint8_t hint, uint8_t sirq, uint8_t sxirq, uint8_t b9,
                      uint8_t b8b7b6, uint8_t b5b4b3, uint8_t b2b1, uint8_t b0,
                      uint8_t cc, uint8_t cn, uint8_t cv, uint8_t cz,
                      uint8_t ci, uint8_t ch, uint8_t cx, uint8_t cs,
                      uint8_t nhb, uint8_t accsec);

  void writeFromEditor(uint16_t addr, uint8_t value);

private:
  uint8_t ram[0x10000]{};
  std::array<bool, 0x10000> modifiedFlags{};
  std::vector<uint16_t> modifiedAddresses;
};
#endif // MEMORY_H
