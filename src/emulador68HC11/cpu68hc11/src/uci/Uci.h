/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef UCI_H
#define UCI_H

#include <cstdint>

class DataBus16;      // forward declaration
class DataBusQueries; // forward declaration

class Uci {
public:
  Uci() = default;

  void reset();

  void outputToBus(DataBus16 &bus, DataBusQueries &busq,
                   uint8_t interrupt) const;

  void handleInt(uint8_t nreset, uint8_t xccr, uint8_t iccr);

  // interrupt vector are the lines nex, nwx, nei and nwi that i don't use
  // uint16_t readVectorAddress(const DataBus16 &bus, uint8_t interrupt,
  //                            uint8_t nreset, uint8_t nxirq, uint8_t nirq)
  //                            const;

  void enableInt(uint8_t dint, uint8_t hint);

  void enableXI(uint8_t set_xirq, uint8_t nxirq, uint8_t set_irq, uint8_t nirq);

  uint16_t readOpcode();

private:
  // Think flip flops RS are not useful with my new way of enable interruptions
  // however I stil set it for class
  // Default is false, enable in state 007
  bool enableRS = false;
  bool xRS = true;
  bool iRS = true;

  // TODO: opcodes are hard code, look in documentation what opcodes are
  // available, I think XIRQ, IRQ, SWI, etc. have the same opcode.
  // After investigation I realise reset starts at 0
  // same taht initial setup (000 to 007 states)
  // effective opcode
  uint16_t opcodeROM[4] = {
      0x000, // RESET
      0x400, // XIRQ
      0x500, // IRQ
      0x600  // SWI
  };

  uint16_t opcode = 0x000;

  bool INT = false;

  enum MuxInput { qint = 15 };

  // Interrupt vector table
  uint16_t vectorROM[4] = {
      0xFFFE, // RESET
      0xFFF4, // XIRQ
      0xFFF2, // IRQ
      0xFFFA  // SWI
  };

  uint16_t vectorAddr = 0xFFFE;
};

#endif // UCI_H
