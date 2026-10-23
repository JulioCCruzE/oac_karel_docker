/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Upa.h"
#include "../bus/DataBus16.h"
#include "../bus/DataBusQueries.h"
#include "../bus/DataBusUpa.h"
#include "../register/Ccr.h"

void Upa::reset() {
  Y = 0;
  Q = 0;
  contador = 0;
  R = 0;
  S = 0;
  F = 0;
  FC = false;
  Q0 = false;
  Q7 = false;
  Y7 = false;
  Y0 = false;
  C = false;
  V = false;
  Z = false;
  N = false;
  H = false;
}

void Upa::outputToBus(DataBus16 &bus, DataBusQueries &busq, Ccr &ccr,
                      uint8_t noeupa) const {

  // drive data bus queries
  busq.setInput(q7, Q7);
  busq.setInput(q0, Q0);
  busq.setInput(y7, Y7);
  busq.setInput(y0, Y0);
  busq.setInput(fc, FC);

  // drive flags
  ccr.loadMuxUpa(C, V, Z, N, H);

  // drive data bus 16
  if (noeupa == 1)
    return;
  bus.driveL(Y);
  bus.driveH(Y);
}

void Upa::source(DataBus16 &bus, DataBusUpa &busa, DataBusUpa &busb,
                 uint8_t selbus, uint8_t source) {
  switch (source) {
  case 0:
    R = busa.isDriven() ? busa.read() : 0;
    S = 0;
    break;
  case 1:
    R = busa.isDriven() ? busa.read() : 127;
    S = busb.isDriven() ? busb.read() : 127;
    break;
  case 2:
    R = Q;
    S = 0;
    break;
  case 3:
    R = busb.isDriven() ? busb.read() : 127;
    S = 0;
    break;
  case 4:
    R = 0;
    S = busa.isDriven() ? busa.read() : 127;
    break;
  case 5:
    if (selbus == 0)
      R = bus.isDrivenL() ? bus.readL() : 127;
    else
      R = bus.isDrivenH() ? bus.readH() : 127;
    S = busa.isDriven() ? busa.read() : 127;
    break;
  case 6:
    if (selbus == 0)
      R = bus.isDrivenL() ? bus.readL() : 127;
    else
      R = bus.isDrivenH() ? bus.readH() : 127;
    S = Q;
    break;
  case 7:
    if (selbus == 0)
      R = bus.isDrivenL() ? bus.readL() : 127;
    else
      R = bus.isDrivenH() ? bus.readH() : 127;
    S = 0;
    break;
  case 8:
    if (selbus == 0)
      R = bus.isDrivenL() ? bus.readL() : 127;
    else
      R = bus.isDrivenH() ? bus.readH() : 127;
    S = busb.isDriven() ? busb.read() : 127;
    break;
  case 9:
    R = Q;
    S = busb.isDriven() ? busb.read() : 127;
    break;
  case 10:
    contador = 0;
    break;
  case 11:
    contador = (contador < 7) ? contador + 1 : 0;
    FC = (contador == 7) ? true : false;
    break;
  case 12:
    R = Q;
    S = busa.isDriven() ? busa.read() : 127;
  default:
    break;
  }
}

void Upa::functn(uint8_t functn, uint8_t selmux, uint8_t accsec) {
  // Get cin
  uint8_t ctemp = (selmux == 0) ? C : accsec;
  cin = (ctemp != 0) ? true : false;
  lastOp = functn;

  // Aux variables to get flags
  bool R7, S7, F7, R3, S3, F3;
  // get this from book, only for addition
  // V = R7S7¬F7 v ¬R7¬S7F7
  // C = R7S7 v S7¬F7 v R7¬F7
  // H = R3S3 v S3¬F3 v R3¬F3
  // get this from book, only for substraction R - S
  // V = R7¬S7¬F7 v ¬R7S7F7
  // C = ¬R7S7 v S7F7 v ¬R7F7
  // H = ¬R3S3 v S3F3 v ¬R7F3

  switch (functn) {
  case 0:
    F = R + S + cin;
    R7 = (R & 0x80) != 0;
    S7 = (S & 0x80) != 0;
    F7 = (F & 0x80) != 0;
    R3 = (R & 0x08) != 0;
    S3 = (S & 0x08) != 0;
    F3 = (F & 0x08) != 0;
    V = (R7 & S7 & !F7) | (!R7 & !S7 & F7);
    C = (R7 & S7) | (S7 & !F7) | (R7 & !F7);
    H = (R3 & S3) | (S3 & !F3) | (R3 & !F3);
    break;
  case 1:
    F = S - R - !cin;
    R7 = (R & 0x80) != 0;
    S7 = (S & 0x80) != 0;
    F7 = (F & 0x80) != 0;
    R3 = (R & 0x08) != 0;
    S3 = (S & 0x08) != 0;
    F3 = (F & 0x08) != 0;
    V = (S7 & !R7 & !F7) | (!S7 & R7 & F7);
    C = (!S7 & R7) | (R7 & F7) | (!S7 & F7);
    H = (!S3 & R3) | (R3 & F3) | (!S3 & F3);
    break;
  case 2:
    F = R - S - !cin;
    R7 = (R & 0x80) != 0;
    S7 = (S & 0x80) != 0;
    F7 = (F & 0x80) != 0;
    R3 = (R & 0x08) != 0;
    S3 = (S & 0x08) != 0;
    F3 = (F & 0x08) != 0;
    V = (R7 & !S7 & !F7) | (!R7 & S7 & F7);
    C = (!R7 & S7) | (S7 & F7) | (!R7 & F7);
    H = (!R3 & S3) | (S3 & F3) | (!R3 & F3);
    break;
  case 3:
    F = R | S;
    break;
  case 4:
    F = R & S;
    break;
  case 5:
    F = ~R & S;
    break;
  case 6:
    F = R ^ S;
    break;
  case 7:
    F = ~(R ^ S);
  default:
    break;
  }
  // get the rest of flags
  Z = (F == 0);
  N = (F & 0x80) != 0;
  // Get H don´t works with this code, for exmpale:
  // if you have H = 1 and (¬R4 == S4) then
  //  (Cout, S) = R4 + S4 + H = (1, 0)
  //  therefore F4 = S = 0,
  // So the code below assign 0 to H
  // H = (F & 0x10) != 0;
}

void Upa::dest(DataBusUpa &busa, DataBusUpa &busb, uint8_t dest,
               uint8_t ndupa) {
  if (ndupa == 0)
    return;

  switch (dest) {
  case 0:
    Y = F;
    Q = F;
    break;
  case 1:
    Y = F;
    break;
  case 2:
    Y = busa.isDriven() ? busa.read() : 127;
    break;
  case 3:
    Y = busb.isDriven() ? busb.read() : 127;
    break;
  case 4:
    Y = Y >> 1;
    // set Y7 = C
    Y = C ? Y | 0x80 : Y & 0x7F;
    break;
  case 5:
    Q = Q >> 1;
    // set Q7 = Y0
    Q |= (uint8_t(Y & 0x01) << 7);
    break;
  case 6:
    Y = Y << 1;
    // set Y0 = Q7
    Y |= (uint8_t(Q & 0x80) >> 7);
    break;
  case 7:
    Q = Q << 1;
    // set Q0 = C
    Q = C ? Q | 0x01 : Q & 0xFE;
    break;
  default:
    break;
  }

  // get Y7, Y0, Q7, Q0
  Y7 = (Y & 0x80) != 0;
  Y0 = (Y & 0x01) != 0;
  Q7 = (Q & 0x80) != 0;
  Q0 = (Q & 0x01) != 0;
}

bool Upa::getC() const { return C; };
bool Upa::getV() const { return V; };
bool Upa::getZ() const { return Z; };
bool Upa::getN() const { return N; };
bool Upa::getH() const { return H; };

uint8_t Upa::getY() const { return Y; }
uint8_t Upa::getQ() const { return Q; }
uint8_t Upa::getCounter() const { return contador; }
uint8_t Upa::getR() const { return R; }
uint8_t Upa::getS() const { return S; }
uint8_t Upa::getF() const { return F; }
uint8_t Upa::getOp() const { return lastOp; }
uint8_t Upa::getCin() const { return cin; }
