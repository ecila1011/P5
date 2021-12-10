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
int name[MAX_PHYSICAL_REGS];

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
int insert_spill(int pr, ILOCInsn *prev_insn, ILOCInsn *local_allocator)
{
    /* adjust stack frame size to add new spill slot */
    int bp_offset = local_allocator->op[1].imm - WORD_SIZE;
    local_allocator->op[1].imm = bp_offset;

    /* create store instruction */
    ILOCInsn *new_insn = ILOCInsn_new_3op(STORE_AI,
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
void insert_load(int bp_offset, int pr, ILOCInsn *prev_insn)
{
    /* create load instruction */
    ILOCInsn *new_insn = ILOCInsn_new_3op(LOAD_AI,
                                          base_register(), int_const(bp_offset), register_with_id(pr));

    /* insert into code */
    new_insn->next = prev_insn->next;
    prev_insn->next = new_insn;
}

/**
 * @brief Replace a virtual register id with a physical register id
 * 
 * Every copy of "vr" will be replaced by "pr" in the given instruction.
 * Note that in our implementation, we do not distinguish between virutal
 * and physical registers explicitly.
 * 
 * @param vr Virtual register id that should be replaced
 * @param pr Physical register id that it should be replaced by
 * @param isnsn Instruction to modify
 */
void replace_register(int vr, int pr, ILOCInsn *insn)
{
    for (int i = 0; i < 3; i++)
    {
        if (insn->op[i].type == VIRTUAL_REG && insn->op[i].id == vr)
        {
            insn->op[i].id = pr;
        }
    }
}

// ****** beginning of new code added by Alice ****** //

int allocate(int vr)
{
    for (int i = 0; i < MAX_PHYSICAL_REGS; i++)
    {
        // Check if there's a free physical register
        // Invalid Im guessing is less than 0 but there's no INVALID enum
        if (name[i] == 0)
        {
            // Set name[pr] to vr
            name[i] = vr;
            // Return the physical register
            return i;
        }
        else
        {
            // find pr that maximizes dist(name[pr])
            // spill(pr)
            // name[pr] = vr
            // return pr
        }
    }

    return 0;
    // **Ignoring Spilling RN

    // If there's no free registers, then spill the physical register ya want to use to stack
    // Set the name[pr] to the vr id
    // Return the physical register
}

int ensure(int vr)
{
    // Loop through physical registers (In the name array)
    for (int i = 0; i < MAX_PHYSICAL_REGS; i++)
    {
        // If the vr is in there (check the ID), then return the physical register
        if (name[i] == vr)
        {
            return name[i];
        }
        else
        {
            int pr = allocate(vr);
            if (offset[vr] != EMPTY) 
            {
                insert_load(offset[vr], pr, NULL);
            }
        }
    }

    // If not... allocate a physical register using allocate()
    // **Ignoring Spilling RN
    return allocate(vr);
    // If vr had to be spilled, then load it into a physical register using offset[vr id]
    // Return the physical register
}

// Spill Method

// Dist method
int dist(Operand vr)
{
    return 9999;
}

/**
 * @brief main function to be used for register allocation
 * 
 * @param list list of ILOC instructions (ILOCInsn*)
 * @param num_physical_registers number of phyiscal registers that can be used
 */
void allocate_registers(InsnList *list, int num_physical_registers)
{
    // for each instruction i in program
    FOR_EACH(ILOCInsn *, i, list)
    {

        // save reference to stack allocator instruction if i is a call label
        if (i->form == CALL)
        {
            // idk how to implement this lol
            // reset name and offset
        }

        if (i->form == JUMP) 
        {
            // reset name and offset
        }

        // also dont know how to do this

        // get read registers and make a list of operands
        ILOCInsn *readregs = ILOCInsn_get_read_registers(i);
        Operand* list = readregs->op;
        int count = -1;

        // for each read register in i
        for (int index = 0; index < 3; index++)
        {
            if (list[index].type != EMPTY)
            {
                count += 1;

                int pr = ensure(list[index].id);          // make sure vr is in a phys reg
                i->op[index].id = pr;                    // change register id

                // if no future use
                if (dist(list[index]) >= 9999) {
                    // then free pr
                    name[pr] = -1; 
                }
            }
        }

        // get write register
        Operand writereg = ILOCInsn_get_write_register(i);

        if (writereg.type != EMPTY)
        {
            int pr = allocate(writereg.id);          // make sure pr is available
            //i->op[count+1].id = pr;                    // change register id

            // if no future use
            if (dist(writereg) >= 9999) {
                // then free pr
                name[pr] = -1; 
            }
        }


        // ILOCInsn *readregs = ILOCInsn_get_read_registers(i);
        // Operand* list = readregs->op;
        // for (int index = 0; index < 3; index++)
        // {
        //     if (list[index].type != EMPTY)
        //     {
        //         Operand pr = allocate(list[index]);
        //         // replace_register(list[index].id, pr.id, i);
        //     }
        // }
    }
}
