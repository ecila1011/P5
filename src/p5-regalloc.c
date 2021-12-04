/**
 * @file p5-regalloc.c
 * @brief Compiler phase 5: register allocation
 * Team Lima: Alice Robertson and Alexander Bain
 */
#include "p5-regalloc.h"

/**
 * @brief Maximum number of physical registers
 */
#define MAX_PHYSICAL_REGS 32

/**
 * Adding this from the given pseudocode
 */

// main physical/virtual mapping (INVALID indicates empty register)
// name : physical register id => virtual register id
Operand name[MAX_PHYSICAL_REGS];

// tracks stack offset for spilled virtual registers
// offset : virtual register id => int
int offset[MAX_VIRTUAL_REGS];

/**
 * @brief Insert a store instruction to spill a register to the stack
 * 
 * We need to allocate a new slot in the stack from for the current
 * function, so we need a reference to the local allocator instruction.
 * This instruction will always be the third instruction in a function
 * and will be of the form "add SP, -X => SP" where X is the current
 * stack frame size.
 * 
 * @param pr Physical register id that should be spilled
 * @param prev_insn Reference to an instruction; the new instruction will be
 * inserted directly after this one
 * @param local_allocator Reference to the local frame allocator instruction
 * @returns BP-based offset where the register was spilled
 */
int insert_spill(int pr, ILOCInsn* prev_insn, ILOCInsn* local_allocator)
{
    /* adjust stack frame size to add new spill slot */
    int bp_offset = local_allocator->op[1].imm - WORD_SIZE;
    local_allocator->op[1].imm = bp_offset;

    /* create store instruction */
    ILOCInsn* new_insn = ILOCInsn_new_3op(STORE_AI,
            register_with_id(pr), base_register(), int_const(bp_offset));

    /* insert into code */
    new_insn->next = prev_insn->next;
    prev_insn->next = new_insn;

    return bp_offset;
}

/**
 * @brief Insert a load instruction to load a spilled register
 * 
 * @param bp_offset BP-based offset where the register value is spilled
 * @param pr Physical register where the value should be loaded
 * @param prev_insn Reference to an instruction; the new instruction will be
 * inserted directly after this one
 */
void insert_load(int bp_offset, int pr, ILOCInsn* prev_insn)
{
    /* create load instruction */
    ILOCInsn* new_insn = ILOCInsn_new_3op(LOAD_AI,
            base_register(), int_const(bp_offset), register_with_id(pr));

    /* insert into code */
    new_insn->next = prev_insn->next;
    prev_insn->next = new_insn;
}

// ****** beginning of new code added by Alice ****** //

/**
 * @brief main function to be used for register allocation
 * 
 * @param list list of ILOC instructions (ILOCInsn*)
 * @param num_physical_registers number of phyiscal registers that can be used
 */
void allocate_registers (InsnList* list, int num_physical_registers)
{
    // for each instruction i in program
    FOR_EACH(ILOCInsn*, i, list)
    {
        // save reference to stack allocator instruction if i is a call label
        if (i->form == CALL)
        {
            // idk how to implement this lol
            
        }

        // reset name[] and offset[] if i is a leader
        // also dont know how to do this

        ILOCInsn* readregs = ILOCInsn_get_read_registers(i);
        Operand* list = readregs->op;

        // FOR_EACH(Operand*, read, list)
        // {

        // }

    }
}
