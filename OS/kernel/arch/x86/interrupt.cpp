//
// Created on 5/19/20.
//

#include <common.h>
#include <interrupt.h>
#include <utilities.h>

namespace PIC
{
  // 8259 Programmable interrupt controller

  #define PIC1_COMM 0x20
  #define PIC1_DATA 0x21
  #define PIC2_COMM 0xA0
  #define PIC2_DATA 0xA1
  #define IRQ_MASTER_OFFSET 0x20 // 32
  #define IRQ_SLAVE_OFFSET  0x28 // 40

  // refer https://www.eeeguide.com/8259-programmable-interrupt-controller/
  // ICW1
  #define ICW1_ICW4NEEDED 0x1  // 0x1 needed, 0x0 unneeded
  #define ICW1_MODE       0x0  // 0x2 single, 0x0 cascade
  #define ICW1_ADI        0x0  // 0x4 interval of 4, 0x0 interval of 8
  #define ICW1_TRIGGER    0x0  // 0x8 level, 0x0 edge
  #define ICW1_INIT       0x10 // initialize PIC

  // ICW2
  // Upper five bits is master interrupt vector offset
  #define ICW2_MASTER_OFFSET IRQ_MASTER_OFFSET
  #define ICW2_SLAVE_OFFSET  IRQ_SLAVE_OFFSET

  // ICW3
  #define ICW3_MASTER_SLAVE_POS 0x4 // tells master that slave is IRQ2 (3rd position)
  #define ICW3_SLAVE_ID         0x2 // tells slave that its identity is IRQ2

  // ICW4
  #define ICW4_CPU 0x1 // 0x1 8086/8088 mode, 0x0 MCS 80/85 mode


  void Remap ()
  {
    constexpr uint8_t ICW1_Config =
      ICW1_ICW4NEEDED
      | ICW1_MODE
      | ICW1_ADI
      | ICW1_TRIGGER
      | ICW1_INIT;

    out8(PIC1_COMM, ICW1_Config); // init flag asks PIC1 to read ICW2,3,4
    out8(PIC2_COMM, ICW1_Config); // init flag asks PIC2 to read ICW2,3,4
    out8(PIC1_DATA, ICW2_MASTER_OFFSET); // set offset of IRQ1
    out8(PIC2_DATA, ICW2_SLAVE_OFFSET);  // set offset of IRQ2
    out8(PIC1_DATA, ICW3_MASTER_SLAVE_POS); // tells IRQ1 position of IRQ2 in cascade
    out8(PIC2_DATA, ICW3_SLAVE_ID);         // tells IRQ2 its identity
    out8(PIC1_DATA, ICW4_CPU); // set intel CPU mode
    out8(PIC2_DATA, ICW4_CPU); // set intel CPU mode

    out8(PIC1_DATA, 0xff); // disable interrupts
    out8(PIC2_DATA, 0xff); // disable interrupts
    out8(PIC1_DATA, ~ICW3_MASTER_SLAVE_POS); // enable IRQ2 as cascade on IRQ1
  }
}

namespace INIT
{
  void Idt ()
  {
    PIC::Remap ();
    // create and register idt
  }
}

