/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "CPU68HC11.h"

uint32_t CPU68HC11::getBits(const RomWord &w, int shift, uint32_t mask) {
  if (shift >= 64) {
    return (w.high >> (shift - 64)) & mask;
  } else {
    return (w.low >> shift) & mask;
  }
}

void CPU68HC11::reset() {
  // PC = (memory.read8(0xFFFE) << 8) | memory.read8(0xFFFF);
  mPC = 0x0000;
  cycleCount = 0;
  PC.reset();
  // Reset each register is not necesary because states 0 to 7 make the
  // necessary resets.
  // For class, alumns not use this states, jump from 0 to 7, check that
  // A.reset();
  // B.reset();
  // X.reset();
  // A.set(134);
  // B.set(140);
}

void CPU68HC11::step() {
  // uint16_t pc_before = PC;
  // uint8_t opcode = memory.read8(PC++);
  // execute(opcode, pc_before);
  mPC_before = mPC;
  RomWord state = memory.fetchRom(mPC);
  executeState(state, mPC_before);
  ++cycleCount;
}

std::uint64_t CPU68HC11::getCycleCount() const { return cycleCount; }

void CPU68HC11::executeState(const RomWord &state, uint16_t mpc_before) {

  // Get Control Lines
  prueba = getBits(state, PRUEBA_SHIFT, VECTOR5_MASK);
  vf = getBits(state, VF_SHIFT, BIT_MASK);
  mi = getBits(state, MI_SHIFT, VECTOR2_MASK);
  liga = getBits(state, LIGA_SHIFT, VECTOR12_MASK);
  ncri = getBits(state, NCRI_SHIFT, BIT_MASK);
  eb = getBits(state, EB_SHIFT, VECTOR2_MASK);
  nwb = getBits(state, NWB_SHIFT, BIT_MASK);
  ea = getBits(state, EA_SHIFT, VECTOR2_MASK);
  nwa = getBits(state, NWA_SHIFT, BIT_MASK);
  selbus = getBits(state, SELBUS_SHIFT, BIT_MASK);
  dest = getBits(state, DESTUPA_SHIFT, VECTOR3_MASK);
  functn = getBits(state, FUNCTNUPA_SHIFT, VECTOR3_MASK);
  // Low part of my structure
  source = getBits(state, SOURCEUPA_SHIFT, VECTOR4_MASK);
  noeupa = getBits(state, NOEUPA_SHIFT, BIT_MASK);
  ndupa = getBits(state, NDUPA_SHIFT, BIT_MASK);
  selmux = getBits(state, SELMUX_SHIFT, BIT_MASK);
  nex = getBits(state, NEX_SHIFT, VECTOR3_MASK);
  x = getBits(state, X_SHIFT, VECTOR3_MASK);
  enay = getBits(state, ENAY_SHIFT, BIT_MASK);
  nera = getBits(state, NERA_SHIFT, VECTOR3_MASK);
  ra = getBits(state, RA_SHIFT, VECTOR3_MASK);
  neap = getBits(state, NEAP_SHIFT, VECTOR3_MASK);
  ap = getBits(state, AP_SHIFT, VECTOR3_MASK);
  nepc = getBits(state, NEPC_SHIFT, VECTOR3_MASK);
  pc = getBits(state, PC_SHIFT, VECTOR3_MASK);
  ncbd = getBits(state, NCBD_SHIFT, BIT_MASK);
  interrupt = getBits(state, INTERRUPT_SHIFT, VECTOR4_MASK);
  nas = getBits(state, NAS_SHIFT, BIT_MASK);
  nrw = getBits(state, NRW_SHIFT, BIT_MASK);
  bd = getBits(state, BD_SHIFT, BIT_MASK);
  dint = getBits(state, DINT_SHIFT, BIT_MASK);
  hint = getBits(state, HINT_SHIFT, BIT_MASK);
  sirq = getBits(state, SIRQ_SHIFT, BIT_MASK);
  sxirq = getBits(state, SXIRQ_SHIFT, BIT_MASK);
  b9 = getBits(state, B9_SHIFT, BIT_MASK);
  b8b7b6 = getBits(state, B8B7B6_SHIFT, VECTOR3_MASK);
  b5b4b3 = getBits(state, B5B4B3_SHIFT, VECTOR3_MASK);
  b2b1 = getBits(state, B2B1_SHIFT, VECTOR2_MASK);
  b0 = getBits(state, B2B1_SHIFT, BIT_MASK);
  cc = getBits(state, CC_SHIFT, BIT_MASK);
  cn = getBits(state, CN_SHIFT, BIT_MASK);
  cv = getBits(state, CV_SHIFT, BIT_MASK);
  cz = getBits(state, CZ_SHIFT, BIT_MASK);
  ci = getBits(state, CI_SHIFT, BIT_MASK);
  ch = getBits(state, CH_SHIFT, BIT_MASK);
  cx = getBits(state, CX_SHIFT, BIT_MASK);
  cs = getBits(state, CS_SHIFT, BIT_MASK);
  nhb = getBits(state, NHB_SHIFT, BIT_MASK);
  accsec = getBits(state, ACCSEC_SHIFT, BIT_MASK);

  // if (mpc_before < 11) {

  // LOAD BUSES: databus16, databusupa, databusqueries, etc.
  A.outputToBus(bus, busUpaA, ccr, ea, nwa, false);
  B.outputToBus(bus, busUpaB, ccr, eb, nwb, true);
  upa.outputToBus(bus, busq, ccr, noeupa);
  X.outputToBus(bus, abus, busq, ccr, nex, x);
  RA.outputToBus(bus, abus, ccr, nera, ra, 0);
  AP.outputToBus(bus, abus, ccr, neap, ap, 2);
  PC.outputToBus(bus, abus, ccr, nepc, pc, 1);
  memory.read8(bus, rdir.read(), nas, nrw, bd);
  uci.outputToBus(bus, busq, interrupt);
  ccr.outputToBus(bus, busq, nhb);

  // UCI functions
  uci.enableInt(dint, hint);
  uci.enableXI(sxirq, nxirq, sirq, nirq); // TODO: Update nxirq and nirq
  // TODO: Update with nreset
  // test getX and getI
  uci.handleInt(nreset, ccr.getX(), ccr.getI());

  // UPA OPERATIONS
  upa.source(bus, busUpaA, busUpaB, selbus, source);
  upa.functn(functn, selmux, accsec); // TODO: Test ACCSEC
  upa.dest(busUpaA, busUpaB, dest, ndupa);

  // CCR control
  ccr.loadCCR(bus, b9, b8b7b6, b5b4b3, b2b1, b0, cc, cn, cv, cz, ci, ch, cx, cs,
              nhb);

  // get next state
  logicainterna.incrementador(mPC);
  logicainterna.setLiga(liga);

  // Print some data test
  std::cout << "[ePresente " << mPC << "]" << " liga=" << liga;
  // std::cout << " nwa=" << int(nwa);
  // std::cout << " ea=" << int(ea);
  // std::cout << " nwb=" << int(nwb);
  // std::cout << " eb=" << int(eb);
  std::cout << " prueba=" << int(prueba);
  std::cout << " vf=" << int(vf);
  std::cout << " ncc=" << int(selectorprueba.getNcc(prueba, vf, busq.read()));

  mPC = logicainterna.esiguiente(
      mi, selectorprueba.getNcc(prueba, vf, busq.read()), ir, uci);
  std::cout << " esiguiente=$" << std::hex << mPC << "\n";

  // LOAD REGISTERS: microprosesor's next state
  A.loadFromBus(bus, ea, nwa);
  B.loadFromBus(bus, eb, nwb);
  X.functn(bus, x);
  X.indexRS(enay, logicainterna.readInstDone()); // TODO: test instr_done
  RA.functn(bus, ra);
  AP.functn(bus, ap);
  PC.functn(bus, pc);
  ir.loadFromBus(bus, ncri);
  memory.write8(bus, rdir.read(), nas, nrw, bd);
  rdir.loadFromBus(abus, ncbd);

  // FLUSH: databus16, etc.
  bus.clear();
  abus.clear();
  busUpaA.clear();
  busUpaB.clear();
}

uint8_t CPU68HC11::getA() const { return A.read(); }
uint8_t CPU68HC11::getB() const { return B.read(); }
uint16_t CPU68HC11::getX(uint8_t enay) const { return X.read(enay); }
uint16_t CPU68HC11::getRA() const { return RA.read(); }
uint16_t CPU68HC11::getAP() const { return AP.read(); }
uint16_t CPU68HC11::getPC() const { return PC.read(); }
uint8_t CPU68HC11::getZA() const { return A.readZ(); }
uint8_t CPU68HC11::getNA() const { return A.readN(); }
uint8_t CPU68HC11::getZB() const { return B.readZ(); }
uint8_t CPU68HC11::getNB() const { return B.readN(); }
uint8_t CPU68HC11::getZX() const { return X.readZ(); }
uint8_t CPU68HC11::getNX() const { return X.readN(); }
uint8_t CPU68HC11::getZPC() const { return PC.readZ(); }
uint8_t CPU68HC11::getNPC() const { return PC.readN(); }
uint16_t CPU68HC11::getRDIR() const { return rdir.read(); }
uint8_t CPU68HC11::getCCR() const { return ccr.read(); }
uint8_t CPU68HC11::getDBH() const { return bus.readH(); }
uint8_t CPU68HC11::getDBL() const { return bus.readL(); }

uint16_t CPU68HC11::getmPC() const { return mPC; }
uint16_t CPU68HC11::getOldmPC() const { return mPC_before; }

uint8_t CPU68HC11::getCUPA() const { return upa.getC(); }
uint8_t CPU68HC11::getVUPA() const { return upa.getV(); }
uint8_t CPU68HC11::getZUPA() const { return upa.getZ(); }
uint8_t CPU68HC11::getNUPA() const { return upa.getN(); }
uint8_t CPU68HC11::getHUPA() const { return upa.getH(); }
uint8_t CPU68HC11::getYUPA() const { return upa.getY(); }
uint8_t CPU68HC11::getQUPA() const { return upa.getQ(); }
uint8_t CPU68HC11::getCounterUPA() const { return upa.getCounter(); }
uint8_t CPU68HC11::getRUPA() const { return upa.getR(); }
uint8_t CPU68HC11::getSUPA() const { return upa.getS(); }
uint8_t CPU68HC11::getFUPA() const { return upa.getF(); }
uint8_t CPU68HC11::getOpUPA() const { return upa.getOp(); }
uint8_t CPU68HC11::getCinUPA() const { return upa.getCin(); }

uint8_t CPU68HC11::get_prueba() const { return prueba; }
uint8_t CPU68HC11::get_vf() const { return vf; }
uint8_t CPU68HC11::get_mi() const { return mi; }
uint16_t CPU68HC11::get_liga() const { return liga; }
uint8_t CPU68HC11::get_ncri() const { return ncri; }
uint8_t CPU68HC11::get_eb() const { return eb; }
uint8_t CPU68HC11::get_nwb() const { return nwb; }
uint8_t CPU68HC11::get_ea() const { return ea; }
uint8_t CPU68HC11::get_nwa() const { return nwa; }
uint8_t CPU68HC11::get_selbus() const { return selbus; }
uint8_t CPU68HC11::get_dest() const { return dest; }
uint8_t CPU68HC11::get_functn() const { return functn; }

uint8_t CPU68HC11::get_source() const { return source; }
uint8_t CPU68HC11::get_noeupa() const { return noeupa; }
uint8_t CPU68HC11::get_ndupa() const { return ndupa; }
uint8_t CPU68HC11::get_selmux() const { return selmux; }
uint8_t CPU68HC11::get_nex() const { return nex; }
uint8_t CPU68HC11::get_x() const { return x; }
uint8_t CPU68HC11::get_enay() const { return enay; }
uint8_t CPU68HC11::get_nera() const { return nera; }
uint8_t CPU68HC11::get_ra() const { return ra; }
uint8_t CPU68HC11::get_neap() const { return neap; }
uint8_t CPU68HC11::get_ap() const { return ap; }
uint8_t CPU68HC11::get_nepc() const { return nepc; }
uint8_t CPU68HC11::get_pc() const { return pc; }
uint8_t CPU68HC11::get_ncbd() const { return ncbd; }
uint8_t CPU68HC11::get_interrupt() const { return interrupt; }
uint8_t CPU68HC11::get_nas() const { return nas; }
uint8_t CPU68HC11::get_nrw() const { return nrw; }
uint8_t CPU68HC11::get_bd() const { return bd; }
uint8_t CPU68HC11::get_dint() const { return dint; }
uint8_t CPU68HC11::get_hint() const { return hint; }
uint8_t CPU68HC11::get_sirq() const { return sirq; }
uint8_t CPU68HC11::get_sxirq() const { return sxirq; }
uint8_t CPU68HC11::get_b9() const { return b9; }
uint8_t CPU68HC11::get_b8b7b6() const { return b8b7b6; }
uint8_t CPU68HC11::get_b5b4b3() const { return b5b4b3; }
uint8_t CPU68HC11::get_b2b1() const { return b2b1; }
uint8_t CPU68HC11::get_b0() const { return b0; }
uint8_t CPU68HC11::get_cc() const { return cc; }
uint8_t CPU68HC11::get_cn() const { return cn; }
uint8_t CPU68HC11::get_cv() const { return cv; }
uint8_t CPU68HC11::get_cz() const { return cz; }
uint8_t CPU68HC11::get_ci() const { return ci; }
uint8_t CPU68HC11::get_ch() const { return ch; }
uint8_t CPU68HC11::get_cx() const { return cx; }
uint8_t CPU68HC11::get_cs() const { return cs; }
uint8_t CPU68HC11::get_nhb() const { return nhb; }
uint8_t CPU68HC11::get_accsec() const { return accsec; }
