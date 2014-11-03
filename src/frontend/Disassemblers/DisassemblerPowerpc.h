/* Disassembly specific to the PowerPC architecture. */

#ifndef ROSE_DISASSEMBLER_POWERPC_H
#define ROSE_DISASSEMBLER_POWERPC_H

#include "integerOps.h"
#include "sageBuilderAsm.h"

namespace rose {
namespace BinaryAnalysis {

/** Disassembler for the PowerPC architecture. */
class DisassemblerPowerpc: public Disassembler {
public:
    DisassemblerPowerpc(): ip(0), insn(0) { init(); }
    DisassemblerPowerpc(const DisassemblerPowerpc& other): Disassembler(other), ip(other.ip), insn(other.insn) {}
    virtual ~DisassemblerPowerpc() {}
    virtual DisassemblerPowerpc *clone() const { return new DisassemblerPowerpc(*this); }
    virtual bool can_disassemble(SgAsmGenericHeader*) const;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors=NULL);
    virtual void assembleOne(SgAsmInstruction*, SgUnsignedCharList&) {abort();}
    virtual SgAsmInstruction *make_unknown_instruction(const Exception&);
private:
    /** Same as Disassembler::Exception except with a different constructor for ease of use in DisassemblerPowerpc. This
     *  constructor should be used when an exception occurs during disassembly of an instruction; it is not suitable for
     *  errors that occur before or after (use superclass constructors for that case). */
    class ExceptionPowerpc: public Exception {
    public:
        ExceptionPowerpc(const std::string &mesg, const DisassemblerPowerpc *d, size_t bit=0)
            : Exception(mesg, d->ip) {
            /* Convert four-byte instruction to big-endian buffer. Note that PowerPC is big-endian, but PowerPC can support
             * both big- and little-endian processor modes (with much weirdness; e.g. PDP endian like propoerties). */
            bytes.push_back((d->insn>>24) & 0xff);
            bytes.push_back((d->insn>>16) & 0xff);
            bytes.push_back((d->insn>>8) & 0xff);
            bytes.push_back(d->insn & 0xff);
            ASSERT_require(bit<=32);
            this->bit = 8*(4-(bit/8)) + bit%8; /*convert from native uint32_t bit position to big-endian*/
        }
    };

    SgAsmRegisterReferenceExpression *
    makeRegister(PowerpcRegisterClass reg_class, int reg_number,
                 PowerpcConditionRegisterAccessGranularity reg_grainularity = powerpc_condreggranularity_whole) const;

    static SgAsmPowerpcInstruction *makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic,
                                                                   PowerpcInstructionKind kind, uint32_t insn);

    /** Helper function to use field definitions (counted with bits from left and inclusive on both sides) from manual */
    template <size_t First, size_t Last> uint32_t fld() const;

    /* Decoded fields from section 1.7.16 of the v2.01 UISA */
    bool AA() const {
        return fld<30, 30>();
    }
    SgAsmRegisterReferenceExpression *BA() const {
        return makeRegister(powerpc_regclass_cr, fld<11, 15>(), powerpc_condreggranularity_bit);
    }
    SgAsmRegisterReferenceExpression *BB() const {
        return makeRegister(powerpc_regclass_cr, fld<16, 20>(), powerpc_condreggranularity_bit);
    }
    uint64_t BD() const {
        return IntegerOps::signExtend<16, 64>((uint64_t)insn & 0xfffc);
    }
    SgAsmRegisterReferenceExpression* BF_cr() const {
        return makeRegister(powerpc_regclass_cr, fld<6, 8>(), powerpc_condreggranularity_field);
    }
    SgAsmRegisterReferenceExpression* BF_fpscr() const {
        return makeRegister(powerpc_regclass_fpscr, fld<6, 8>(), powerpc_condreggranularity_field);
    }
    SgAsmRegisterReferenceExpression* BFA_cr() const {
        return makeRegister(powerpc_regclass_cr, fld<11, 13>(), powerpc_condreggranularity_field);
    }
    SgAsmRegisterReferenceExpression* BFA_fpscr() const {
        return makeRegister(powerpc_regclass_fpscr, fld<11, 13>(), powerpc_condreggranularity_field);
    }
    SgAsmValueExpression* BH() const {
        return SageBuilderAsm::buildValueU8(fld<19, 20>());
    }
    SgAsmRegisterReferenceExpression* BI() const {
        return BA();
    }
    SgAsmValueExpression* BO() const {
        return SageBuilderAsm::buildValueU8(fld<6, 10>());
    }
    SgAsmRegisterReferenceExpression* BT() const {
        return makeRegister(powerpc_regclass_cr, fld<6, 10>(), powerpc_condreggranularity_bit);
    }
    SgAsmValueExpression* D() const {
        return SageBuilderAsm::buildValueU64(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>()));
    }
    SgAsmValueExpression* DS() const {
        return SageBuilderAsm::buildValueU64(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>() & 0xfffc));
    }
    SgAsmValueExpression* FLM() const {
        return SageBuilderAsm::buildValueU8(fld<7, 14>());
    }
    SgAsmRegisterReferenceExpression* FRA() const {
        return makeRegister(powerpc_regclass_fpr, fld<11, 15>());
    }
    SgAsmRegisterReferenceExpression* FRB() const {
        return makeRegister(powerpc_regclass_fpr, fld<16, 20>());
    }
    SgAsmRegisterReferenceExpression* FRC() const {
        return makeRegister(powerpc_regclass_fpr, fld<21, 25>());
    }
    SgAsmRegisterReferenceExpression* FRS() const {
        return makeRegister(powerpc_regclass_fpr, fld<6, 10>());
    }
    SgAsmRegisterReferenceExpression* FRT() const {
        return FRS();
    }
    SgAsmValueExpression* FXM() const {
        return SageBuilderAsm::buildValueU8(fld<12, 19>());
    }
          
    SgAsmValueExpression* L_10() const {
        return SageBuilderAsm::buildValueU8(fld<10, 10>());
    }
    SgAsmValueExpression* L_15() const {
        return SageBuilderAsm::buildValueU8(fld<15, 15>());
    }
    uint8_t L_sync() const {
        return fld<9, 10>();
    }
    SgAsmValueExpression* LEV() const {
        return SageBuilderAsm::buildValueU8(fld<20, 26>());
    }
    uint64_t LI() const {
        return IntegerOps::signExtend<26, 64>(uint64_t(fld<6, 29>() * 4));
    }
    bool LK() const {
        return fld<31, 31>();
    }
    SgAsmValueExpression* MB_32bit() const {
        return SageBuilderAsm::buildValueU8(fld<21, 25>());
    }
    SgAsmValueExpression* ME_32bit() const {
        return SageBuilderAsm::buildValueU8(fld<26, 30>());
    }
    SgAsmValueExpression* MB_64bit() const {
        return SageBuilderAsm::buildValueU8(fld<21, 26>()); // FIXME check for splitting
    } 
    SgAsmValueExpression* ME_64bit() const {
        return SageBuilderAsm::buildValueU8(fld<21, 26>()); // FIXME check for splitting
    }   
    SgAsmValueExpression* NB() const {
        return SageBuilderAsm::buildValueU8(fld<16, 20>() == 0 ? 32 : fld<16, 20>());
    }
    bool OE() const {
        return fld<21, 21>();
    }
    SgAsmRegisterReferenceExpression* RA() const {
        return makeRegister(powerpc_regclass_gpr, fld<11, 15>());
    }
    SgAsmExpression* RA_or_zero() const {
        return fld<11, 15>() == 0 ? (SgAsmExpression*)SageBuilderAsm::buildValueU8(0) : RA();
    }
    SgAsmRegisterReferenceExpression* RB() const {
        return makeRegister(powerpc_regclass_gpr, fld<16, 20>());
    }
    bool Rc() const {
        return fld<31, 31>();
    }
    SgAsmRegisterReferenceExpression* RS() const {
        return makeRegister(powerpc_regclass_gpr, fld<6, 10>());
    }
    SgAsmRegisterReferenceExpression* RT() const {
        return RS();
    }
    SgAsmValueExpression* SH_32bit() const {
        return SageBuilderAsm::buildValueU8(fld<16, 20>());
    }
    SgAsmValueExpression* SH_64bit() const {
        return SageBuilderAsm::buildValueU8(fld<16, 20>() + fld<30, 30>() * 32); // FIXME check
    } 
    SgAsmValueExpression* SI() const {
        return D();
    }
    SgAsmRegisterReferenceExpression* SPR() const {
        return makeRegister(powerpc_regclass_spr, fld<16, 20>() * 32 + fld<11, 15>());
    }
    SgAsmRegisterReferenceExpression* SR() const {
        return makeRegister(powerpc_regclass_sr, fld<12, 15>());
    }
    SgAsmRegisterReferenceExpression* TBR() const {
        return makeRegister(powerpc_regclass_tbr, fld<16, 20>() * 32 + fld<11, 15>());
    }
    SgAsmValueExpression* TH() const {
        return SageBuilderAsm::buildValueU8(fld<9, 10>());
    }
    SgAsmValueExpression* TO() const {
        return SageBuilderAsm::buildValueU8(fld<6, 10>());
    }
    SgAsmValueExpression* U() const {
        return SageBuilderAsm::buildValueU8(fld<16, 19>());
    }
    SgAsmValueExpression* UI() const {
        return SageBuilderAsm::buildValueU64(fld<16, 31>());
    }

    SgAsmMemoryReferenceExpression* memref(SgAsmType* t) const {
            return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(), D()), NULL, t);
    }
    SgAsmMemoryReferenceExpression* memrefx(SgAsmType* t) const {
            return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(), RB()),
                                                                  NULL, t);
    }
    SgAsmMemoryReferenceExpression* memrefu(SgAsmType* t) const {
        if (fld<11, 15>() == 0)
            throw ExceptionPowerpc("bits 11-15 must be nonzero", this);
        return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA(), D()), NULL, t);
    }
    SgAsmMemoryReferenceExpression* memrefux(SgAsmType* t) const {
        if (fld<11, 15>() == 0)
            throw ExceptionPowerpc("bits 11-15 must be nonzero", this);
        return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA(), RB()), NULL, t);
    }

    /* There are 15 different forms of PowerPC instructions, but all are 32-bit (fixed length instruction set). */
    SgAsmPowerpcInstruction* decode_I_formInstruction();
    SgAsmPowerpcInstruction* decode_B_formInstruction();
    SgAsmPowerpcInstruction* decode_SC_formInstruction();
    SgAsmPowerpcInstruction* decode_DS_formInstruction();
    SgAsmPowerpcInstruction* decode_X_formInstruction_00();
    SgAsmPowerpcInstruction* decode_X_formInstruction_1F();
    SgAsmPowerpcInstruction* decode_X_formInstruction_3F();
    SgAsmPowerpcInstruction* decode_XL_formInstruction();
    SgAsmPowerpcInstruction* decode_XS_formInstruction();
    SgAsmPowerpcInstruction* decode_A_formInstruction_00();
    SgAsmPowerpcInstruction* decode_A_formInstruction_04();
    SgAsmPowerpcInstruction* decode_A_formInstruction_3B();
    SgAsmPowerpcInstruction* decode_A_formInstruction_3F();
    SgAsmPowerpcInstruction* decode_MD_formInstruction();
    SgAsmPowerpcInstruction* decode_MDS_formInstruction();

    SgAsmIntegerValueExpression* makeBranchTarget( uint64_t targetAddr ) const;

    SgAsmPowerpcInstruction* disassemble();

    /** Initialize instances of this class. Called by constructor. */
    void init();
    
    /** Resets disassembler state to beginning of an instruction. */
    void startInstruction(rose_addr_t start_va, uint32_t c) {
        ip = start_va;
        insn = c;
    }
    
    /* Per-instruction data members (mostly set by startInstruction()) */
    uint64_t ip;                                /**< Instruction pointer */
    uint32_t insn;                              /**< 4-byte instruction word */
};

} // namespace
} // namespace

#endif
