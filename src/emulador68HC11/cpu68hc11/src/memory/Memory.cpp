/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Memory.h"
#include "../bus/DataBus16.h"

RomWord Memory::fetchRom(uint16_t mpc) { return rom[mpc & 0x0FFF]; }

uint8_t Memory::readRam(uint16_t addr) const { return ram[addr]; }

const uint8_t *Memory::ramData() const { return ram; }
//
void Memory::read8(DataBus16 &bus, uint16_t addr, uint8_t nas, uint8_t nrw,
                   uint8_t bd) {
  if (nas != 0)
    return;
  if (nrw == 0)
    return;
  if (bd == 0) {
    bus.driveL(ram[addr]);
  } else {
    bus.driveH(ram[addr]);
  }
}
void Memory::write8(const DataBus16 &bus, uint16_t addr, uint8_t nas,
                    uint8_t nrw, uint8_t bd) {
  if (!(nas == 0 && nrw == 0))
    return;

  if (bd == 0) {
    writeRam(addr, bus.isDrivenL() ? bus.readL() : 127);
  } else {
    writeRam(addr, bus.isDrivenH() ? bus.readH() : 127);
  }
}

RomWord Memory::parseRomWord(const std::string &bits) {
  RomWord word{};
  word.low = 0;
  word.high = 0;

  // bits[0] = MSB del microcódigo
  // bits[ROM_BITS-1] = LSB

  for (size_t i = 0; i < bits.size(); ++i) {
    if (bits[i] != '1')
      continue;

    // posición del bit desde LSB
    size_t bitPos = bits.size() - 1 - i;

    if (bitPos < 64) {
      word.low |= (uint64_t(1) << bitPos);
    } else {
      word.high |= (uint64_t(1) << (bitPos - 64));
    }
  }

  return word;
}

bool Memory::loadRomFromVHDL(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "No se pudo abrir " << filename << "\n";
    std::cout << "LOAD ROM USING INTERNAL CODE\n";
    // Microprograma
    // ---------------------
    // RESET OR INIT
    // |prueba|vf|mi|liga|cri|eb|wb|ea|wa|selbus|dest|func|sourc|oe|dupa|
    // selmux|nex|x|enay|nera|ra|neap|ap|nepc|pc|cbd|int|nas|nrw|bd|dint|hint|
    // sirq|xirq|b9|b8b7b6|b5b4b3|b2b1|b0|cc|cn|cv|cz|ci|ch|cx|cs|nhb|accsec
    rom[0] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 4, 5, 7, 0, 1, 01, 1, 1, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    rom[1] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 3, 0, 7, 0, 0, 16, 1, 1, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    rom[2] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 7, 1, 5, 4, 1, 16, 0, 1, 1, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    rom[3] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 0, 0, 7, 0, 0, 16, 1, 1, 0, 0, 1, 1, 1,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0);
    rom[4] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 7, 0, 6, 3, 1, 16, 0, 1, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    rom[5] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    rom[6] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    // FETCH
    // |prueba|vf|mi|liga|cri|eb|wb|ea|wa|selbus|dest|func|sourc|oe|dupa|selmux|nex|
    // x|enay|nera|ra|neap|ap|nepc|pc|cbd|int|nas|nrw|bd|dint|hint|sirq|xirq|
    rom[8] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // HA
    rom[9] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
                         7, 0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // HA
    rom[10] = makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1,
                          0, 7, 0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // HA
    // MICROPROGRAM TEST
    // |prueba|vf|mi|liga|cri|eb|wb|ea|wa|selbus|dest|func|sourc|oe|dupa|selmux|nex|
    // x|enay|nera|ra|neap|ap|nepc|pc|cbd|int|nas|nrw|bd|dint|hint|sirq|xirq|
    rom[100] =
        makeRomWord(0x00, 0, 0, 0x0000, 1, 2, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 7,
                    0, 0, 7, 0, 7, 0, 4, 5, 1, 16, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // PC
    rom[101] =
        makeRomWord(0x00, 0, 1, 0x0003, 0, 0, 1, 3, 1, 1, 0, 2, 5, 1, 1, 1, 7,
                    0, 0, 7, 1, 7, 2, 4, 0, 1, 16, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // SC
    rom[102] =
        makeRomWord(0x00, 0, 2, 0x0000, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 7,
                    0, 0, 7, 0, 7, 0, 3, 0, 0, 16, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // RT
    rom[103] =
        makeRomWord(0x0F, 1, 3, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 7,
                    0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // RI
    rom[104] =
        makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 7,
                    0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // PC
    rom[105] =
        makeRomWord(0x00, 0, 0, 0x0000, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 7,
                    0, 0, 7, 0, 7, 0, 7, 0, 1, 16, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0); // HA
    return false;
  }

  std::regex lineRegex(
      R"((if|elsif)\s*\(\s*dir\s*=\s*X\"([0-9A-Fa-f]+)\"\s*\)\s*then\s*data\s*<=\s*\"([01]+)\")");

  std::string line;
  while (std::getline(file, line)) {
    // Eliminar comentarios
    size_t comment = line.find("--");
    if (comment != std::string::npos)
      line = line.substr(0, comment);

    std::smatch match;
    if (!std::regex_search(line, match, lineRegex))
      continue;

    uint16_t address = std::stoul(match[2], nullptr, 16);
    const std::string &bitString = match[3];

    if (address >= ROM_SIZE) {
      std::cerr << "Direccion fuera de rango: " << address << "\n";
      continue;
    }

    if (bitString.size() != ROM_BITS) {
      std::cerr << "Palabra con tamano incorrecto en dir " << address << " ("
                << bitString.size() << " bits)\n";
      continue;
    }

    rom[address] = parseRomWord(bitString);
  }

  return true;
}

bool Memory::loadRamFromMIF(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "No se pudo abrir " << filename << "\n";
    std::cout << "LOAD RAM USING INTERNAL CODE\n";
    int i = 0;
    writeRam(i++, 0xF0); // STORE
    writeRam(i++, 0x42); // valor
    writeRam(i++, 0x02); // addr high
    writeRam(i++, 0x00); // addr low

    writeRam(i++, 0xF0); // STORE
    writeRam(i++, 0xAA); // valor
    writeRam(i++, 0x02);
    writeRam(i++, 0x01);

    writeRam(i++, 0x3F);    // HALT
    writeRam(0x8000, 0x1B); // ABA
                            //
    // Vector RESET
    writeRam(0xFFFE, 0x80);
    writeRam(0xFFFF, 0x00);
    return false;
  }
  std::string line;

  // Regex
  std::regex single_addr(R"(^\s*(\d+)\s*:\s*([0-9A-Fa-f]+)\s*;)");
  std::regex range_addr(R"(^\s*\[(\d+)\.\.(\d+)\]\s*:\s*([0-9A-Fa-f]+)\s*;)");

  while (std::getline(file, line)) {
    // Eliminar comentarios
    size_t comment = line.find("--");
    if (comment != std::string::npos)
      line = line.substr(0, comment);

    std::smatch match;

    // Caso: dirección simple
    if (std::regex_search(line, match, single_addr)) {
      size_t addr = std::stoul(match[1]);
      uint8_t value = static_cast<uint8_t>(std::stoul(match[2], nullptr, 16));

      if (addr < RAM_SIZE)
        writeRam(static_cast<uint16_t>(addr), value);
    }
    // Caso: rango de direcciones
    else if (std::regex_search(line, match, range_addr)) {
      size_t start = std::stoul(match[1]);
      size_t end = std::stoul(match[2]);
      uint8_t value = static_cast<uint8_t>(std::stoul(match[3], nullptr, 16));

      for (size_t addr = start; addr <= end && addr < RAM_SIZE; ++addr)
        writeRam(static_cast<uint16_t>(addr), value);
    }
  }

  // Vector RESET
  // TODO: add this to .mif file
  // writeRam(0xFFFE, 0x80);
  // writeRam(0xFFFF, 0x00);

  return true;
}

void Memory::showRam(uint16_t start, uint16_t length) {

  for (uint16_t i = 0; i < length; i += 16) {
    uint16_t addr = start + i;

    // Dirección
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
              << addr << ": ";

    // Bytes hex
    for (int j = 0; j < 16; j++) {
      if (i + j < length) {
        std::cout << std::setw(2) << (int)ram[addr + j] << " ";
      } else {
        std::cout << "   ";
      }
    }

    // ASCII
    std::cout << " |";

    for (int j = 0; j < 16 && (i + j < length); j++) {
      uint8_t c = ram[addr + j];

      if (std::isprint(c))
        std::cout << (char)c;
      else
        std::cout << '.';
    }

    std::cout << "|" << std::endl;
  }
}

RomWord Memory::makeRomWord(
    uint8_t prueba, uint8_t vf, uint8_t mi, uint16_t liga, uint8_t ncri,
    uint8_t eb, uint8_t nwb, uint8_t ea, uint8_t nwa, uint8_t selbus,
    uint8_t dest, uint8_t functn, uint8_t source, uint8_t noeupa, uint8_t ndupa,
    uint8_t selmux, uint8_t nex, uint8_t x, uint8_t enay, uint8_t nera,
    uint8_t ra, uint8_t neap, uint8_t ap, uint8_t nepc, uint8_t pc,
    uint8_t ncbd, uint8_t interrupt, uint8_t nas, uint8_t nrw, uint8_t bd,
    uint8_t dint, uint8_t hint, uint8_t sirq, uint8_t sxirq, uint8_t b9,
    uint8_t b8b7b6, uint8_t b5b4b3, uint8_t b2b1, uint8_t b0, uint8_t cc,
    uint8_t cn, uint8_t cv, uint8_t cz, uint8_t ci, uint8_t ch, uint8_t cx,
    uint8_t cs, uint8_t nhb, uint8_t accsec) {
  RomWord w{};
  w.high |= (uint64_t(prueba & VECTOR5_MASK) << (PRUEBA_SHIFT - 64));
  w.high |= (uint64_t(vf & BIT_MASK) << (VF_SHIFT - 64));
  w.high |= (uint64_t(mi & VECTOR2_MASK) << (MI_SHIFT - 64));
  w.high |= (uint64_t(liga & VECTOR12_MASK) << (LIGA_SHIFT - 64));
  w.high |= (uint64_t(ncri & BIT_MASK) << (NCRI_SHIFT - 64));
  w.high |= (uint64_t(eb & VECTOR2_MASK) << (EB_SHIFT - 64));
  w.high |= (uint64_t(nwb & BIT_MASK) << (NWB_SHIFT - 64));
  w.high |= (uint64_t(ea & VECTOR2_MASK) << (EA_SHIFT - 64));
  w.high |= (uint64_t(nwa & BIT_MASK) << (NWA_SHIFT - 64));
  w.high |= (uint64_t(selbus & BIT_MASK) << (SELBUS_SHIFT - 64));
  w.high |= (uint64_t(dest & VECTOR3_MASK) << (DESTUPA_SHIFT - 64));
  w.high |= (uint64_t(functn & VECTOR3_MASK) << (FUNCTNUPA_SHIFT - 64));

  w.low |= (uint64_t(source & VECTOR4_MASK) << (SOURCEUPA_SHIFT));
  w.low |= (uint64_t(noeupa & BIT_MASK) << (NOEUPA_SHIFT));
  w.low |= (uint64_t(ndupa & BIT_MASK) << (NDUPA_SHIFT));
  w.low |= (uint64_t(selmux & BIT_MASK) << (SELMUX_SHIFT));
  w.low |= (uint64_t(nex & VECTOR3_MASK) << (NEX_SHIFT));
  w.low |= (uint64_t(x & VECTOR3_MASK) << (X_SHIFT));
  w.low |= (uint64_t(enay & BIT_MASK) << (ENAY_SHIFT));
  w.low |= (uint64_t(nera & VECTOR3_MASK) << (NERA_SHIFT));
  w.low |= (uint64_t(ra & VECTOR3_MASK) << (RA_SHIFT));
  w.low |= (uint64_t(neap & VECTOR3_MASK) << (NEAP_SHIFT));
  w.low |= (uint64_t(ap & VECTOR3_MASK) << (AP_SHIFT));
  w.low |= (uint64_t(nepc & VECTOR3_MASK) << (NEPC_SHIFT));
  w.low |= (uint64_t(pc & VECTOR3_MASK) << (PC_SHIFT));
  w.low |= (uint64_t(ncbd & BIT_MASK) << (NCBD_SHIFT));
  w.low |= (uint64_t(interrupt & VECTOR4_MASK) << (INTERRUPT_SHIFT));
  w.low |= (uint64_t(nas & BIT_MASK) << (NAS_SHIFT));
  w.low |= (uint64_t(nrw & BIT_MASK) << (NRW_SHIFT));
  w.low |= (uint64_t(bd & BIT_MASK) << (BD_SHIFT));
  w.low |= (uint64_t(dint & BIT_MASK) << (DINT_SHIFT));
  w.low |= (uint64_t(hint & BIT_MASK) << (HINT_SHIFT));
  w.low |= (uint64_t(sirq & BIT_MASK) << (SIRQ_SHIFT));
  w.low |= (uint64_t(sxirq & BIT_MASK) << (SXIRQ_SHIFT));
  w.low |= (uint64_t(b9 & BIT_MASK) << (B9_SHIFT));
  w.low |= (uint64_t(b8b7b6 & VECTOR3_MASK) << (B8B7B6_SHIFT));
  w.low |= (uint64_t(b5b4b3 & VECTOR3_MASK) << (B5B4B3_SHIFT));
  w.low |= (uint64_t(b2b1 & VECTOR2_MASK) << (B2B1_SHIFT));
  w.low |= (uint64_t(b0 & BIT_MASK) << (B0_SHIFT));
  w.low |= (uint64_t(cc & BIT_MASK) << (CC_SHIFT));
  w.low |= (uint64_t(cn & BIT_MASK) << (CN_SHIFT));
  w.low |= (uint64_t(cv & BIT_MASK) << (CV_SHIFT));
  w.low |= (uint64_t(cz & BIT_MASK) << (CZ_SHIFT));
  w.low |= (uint64_t(ci & BIT_MASK) << (CI_SHIFT));
  w.low |= (uint64_t(ch & BIT_MASK) << (CH_SHIFT));
  w.low |= (uint64_t(cx & BIT_MASK) << (CX_SHIFT));
  w.low |= (uint64_t(cs & BIT_MASK) << (CS_SHIFT));
  w.low |= (uint64_t(nhb & BIT_MASK) << (NHB_SHIFT));
  w.low |= (uint64_t(accsec & BIT_MASK) << (ACCSEC_SHIFT));
  return w;
}

void Memory::writeRam(uint16_t addr, uint8_t value) {
  if (ram[addr] == value)
    return;

  ram[addr] = value;
  if (!modifiedFlags[addr]) {
    modifiedFlags[addr] = true;
    modifiedAddresses.push_back(addr);
  }
}

void Memory::takeModifiedAddresses(std::vector<uint16_t> &addresses) {
  addresses.clear();
  addresses.swap(modifiedAddresses);
  for (uint16_t address : addresses)
    modifiedFlags[address] = false;
}

void Memory::writeFromEditor(uint16_t addr, uint8_t value) {
  writeRam(addr, value);
}
