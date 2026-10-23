/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef CPU68HC11_H
#define CPU68HC11_H

#include <cstdint>
#include <iostream>

#include "bus/AddressBus16.h"
#include "bus/DataBus16.h"
#include "bus/DataBusQueries.h"
#include "bus/DataBusUpa.h"
#include "common/Constants.h"
#include "common/Types.h"
#include "memory/AccessRegister.h"
#include "memory/Memory.h"
#include "register/Ccr.h"
#include "register/Counter16.h"
#include "register/Index16.h"
#include "register/Registro8.h"
#include "sequencer/InstructionRegister.h"
#include "sequencer/LogicaInterna.h"
#include "sequencer/SelectorPrueba.h"
#include "uci/Uci.h"
#include "upa/Upa.h"

class CPU68HC11 {
public:
  // constructor
  CPU68HC11(Memory &mem, LogicaInterna &li, SelectorPrueba &sp)
      : memory(mem), logicainterna(li), selectorprueba(sp) {}

  // utils
  uint32_t getBits(const RomWord &w, int shift, uint32_t mask);

  // execution
  void reset();
  void step();
  void executeState(const RomWord &state, uint16_t mpc_before);
  std::uint64_t getCycleCount() const;

  // get content
  uint8_t getA() const;
  uint8_t getB() const;
  uint16_t getX(uint8_t enay) const;
  uint16_t getRA() const;
  uint16_t getAP() const;
  uint16_t getPC() const;
  uint8_t getZA() const;
  uint8_t getNA() const;
  uint8_t getZB() const;
  uint8_t getNB() const;
  uint8_t getZX() const;
  uint8_t getNX() const;
  uint8_t getZPC() const;
  uint8_t getNPC() const;
  uint16_t getRDIR() const;
  uint8_t getCCR() const;
  uint8_t getDBH() const;
  uint8_t getDBL() const;

  uint16_t getmPC() const;
  uint16_t getOldmPC() const;

  uint8_t getCUPA() const;
  uint8_t getVUPA() const;
  uint8_t getZUPA() const;
  uint8_t getNUPA() const;
  uint8_t getHUPA() const;

  uint8_t getYUPA() const;
  uint8_t getQUPA() const;
  uint8_t getCounterUPA() const;
  uint8_t getRUPA() const;
  uint8_t getSUPA() const;
  uint8_t getFUPA() const;
  uint8_t getOpUPA() const;
  uint8_t getCinUPA() const;

  // get Hight RomWord
  uint8_t get_prueba() const;
  uint8_t get_vf() const;
  uint8_t get_mi() const;
  uint16_t get_liga() const;
  uint8_t get_ncri() const;
  uint8_t get_eb() const;
  uint8_t get_nwb() const;
  uint8_t get_ea() const;
  uint8_t get_nwa() const;
  uint8_t get_selbus() const;
  uint8_t get_dest() const;
  uint8_t get_functn() const;
  // get Low RomWord
  uint8_t get_source() const;
  uint8_t get_noeupa() const;
  uint8_t get_ndupa() const;
  uint8_t get_selmux() const;
  uint8_t get_nex() const;
  uint8_t get_x() const;
  uint8_t get_enay() const;
  uint8_t get_nera() const;
  uint8_t get_ra() const;
  uint8_t get_neap() const;
  uint8_t get_ap() const;
  uint8_t get_nepc() const;
  uint8_t get_pc() const;
  uint8_t get_ncbd() const;
  uint8_t get_interrupt() const;
  uint8_t get_nas() const;
  uint8_t get_nrw() const;
  uint8_t get_bd() const;
  uint8_t get_dint() const;
  uint8_t get_hint() const;
  uint8_t get_sirq() const;
  uint8_t get_sxirq() const;
  uint8_t get_b9() const;
  uint8_t get_b8b7b6() const;
  uint8_t get_b5b4b3() const;
  uint8_t get_b2b1() const;
  uint8_t get_b0() const;
  uint8_t get_cc() const;
  uint8_t get_cn() const;
  uint8_t get_cv() const;
  uint8_t get_cz() const;
  uint8_t get_ci() const;
  uint8_t get_ch() const;
  uint8_t get_cx() const;
  uint8_t get_cs() const;
  uint8_t get_nhb() const;
  uint8_t get_accsec() const;

private:
  enum MuxInput {
    Q7 = 1,
    Q1,
    Y7,
    Y0,
    IY15,
    IY0,
    IX15,
    IX0,
    RA15,
    RA0,
    AP15,
    AP0,
    PC15,
    PC0,
    FC,
    INT
  };

  uint16_t mPC = 0x0000;
  uint16_t mPC_before = 0x0000;
  std::uint64_t cycleCount = 0;
  bool halted = false;
  bool nreset = true;
  bool nxirq = true;
  bool nirq = true;

  // BASIC SEQUENCER
  Memory &memory;
  LogicaInterna &logicainterna;
  SelectorPrueba &selectorprueba;
  InstructionRegister ir;

  // DATABUSES
  DataBus16 bus;
  AddressBus16 abus;
  DataBusUpa busUpaA;
  DataBusUpa busUpaB;
  DataBusQueries busq;

  // REGISTERS
  Registro8 A;
  Registro8 B;
  Index16 X;
  Counter16 RA;
  Ccr ccr;

  // UNITS
  Upa upa;
  Counter16 AP;
  Counter16 PC;
  Uci uci;

  // MEMORY
  AccessRegister rdir;

  // High RomWord
  uint8_t prueba = 0;
  uint8_t vf = 0;
  uint8_t mi = 0;
  uint16_t liga = 0;
  uint8_t ncri = 0;
  uint8_t eb = 0;
  uint8_t nwb = 0;
  uint8_t ea = 0;
  uint8_t nwa = 0;
  uint8_t selbus = 0;
  uint8_t dest = 0;
  uint8_t functn = 0;
  // Low part of my structure
  uint8_t source = 0;
  uint8_t noeupa = 0;
  uint8_t ndupa = 0;
  uint8_t selmux = 0;
  uint8_t nex = 0;
  uint8_t x = 0;
  uint8_t enay = 0;
  uint8_t nera = 0;
  uint8_t ra = 0;
  uint8_t neap = 0;
  uint8_t ap = 0;
  uint8_t nepc = 0;
  uint8_t pc = 0;
  uint8_t ncbd = 0;
  uint8_t interrupt = 0;
  uint8_t nas = 0;
  uint8_t nrw = 0;
  uint8_t bd = 0;
  uint8_t dint = 0;
  uint8_t hint = 0;
  uint8_t sirq = 0;
  uint8_t sxirq = 0;
  uint8_t b9 = 0;
  uint8_t b8b7b6 = 0;
  uint8_t b5b4b3 = 0;
  uint8_t b2b1 = 0;
  uint8_t b0 = 0;
  uint8_t cc = 0;
  uint8_t cn = 0;
  uint8_t cv = 0;
  uint8_t cz = 0;
  uint8_t ci = 0;
  uint8_t ch = 0;
  uint8_t cx = 0;
  uint8_t cs = 0;
  uint8_t nhb = 0;
  uint8_t accsec = 0;
};

#endif // CPU68HC11_H
