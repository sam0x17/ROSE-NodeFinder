#ifndef ROSE_DispatcherX86_H
#define ROSE_DispatcherX86_H

#include "BaseSemantics2.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {

typedef boost::shared_ptr<class DispatcherX86> DispatcherX86Ptr;

class DispatcherX86: public BaseSemantics::Dispatcher {
protected:
    explicit DispatcherX86(const BaseSemantics::RiscOperatorsPtr &ops): BaseSemantics::Dispatcher(ops) {
        set_register_dictionary(RegisterDictionary::dictionary_i386());
        regcache_init();
        iproc_init();
    }

    /** Loads the iproc table with instruction processing functors. This normally happens from the constructor. */
    void iproc_init();

    /** Load the cached register descriptors.  This happens at construction and on set_register_dictionary() calls. */
    void regcache_init();

public:
    /** Cached register. This register is cached so that there are not so many calls to Dispatcher::findRegister(). The
     *  register descriptor is updated only when the register dictionary is changed (see set_register_dictionary()).
     * @{ */
    RegisterDescriptor REG_EAX, REG_EBX, REG_ECX, REG_EDX, REG_EDI, REG_EIP, REG_ESI, REG_ESP, REG_EBP;
    RegisterDescriptor REG_AX, REG_CX, REG_DX, REG_AL, REG_AH;
    RegisterDescriptor REG_EFLAGS, REG_AF, REG_CF, REG_DF, REG_OF, REG_PF, REG_SF, REG_ZF;
    RegisterDescriptor REG_DS, REG_ES, REG_SS;
    /** @}*/

    /** Constructor. */
    static DispatcherX86Ptr instance(const BaseSemantics::RiscOperatorsPtr &ops) {
        return DispatcherX86Ptr(new DispatcherX86(ops));
    }

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr &ops) const /*override*/ {
        return instance(ops);
    }

    /** Dynamic cast to a DispatcherX86Ptr with assertion. */
    static DispatcherX86Ptr promote(const BaseSemantics::DispatcherPtr &d) {
        DispatcherX86Ptr retval = boost::dynamic_pointer_cast<DispatcherX86>(d);
        assert(retval!=NULL);
        return retval;
    }

    virtual void set_register_dictionary(const RegisterDictionary *regdict) /*override*/;

    virtual int iproc_key(SgAsmInstruction *insn_) const /*override*/ {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(insn_);
        assert(insn!=NULL);
        return insn->get_kind();
    }

    /** Set parity, sign, and zero flags appropriate for result value. */
    virtual void setFlagsForResult(const BaseSemantics::SValuePtr &result);

    /** Conditionally set parity, sign, and zero flags appropriate for result value. */
    virtual void setFlagsForResult(const BaseSemantics::SValuePtr &result, const BaseSemantics::SValuePtr &cond);

    /** Returns true if byte @p v has an even number of bits set; false for an odd number */
    virtual BaseSemantics::SValuePtr parity(const BaseSemantics::SValuePtr &v);

    /** Conditionally invert the bits of @p value.  The bits are inverted if @p maybe is true, otherwise @p value is returned. */
    virtual BaseSemantics::SValuePtr invertMaybe(const BaseSemantics::SValuePtr &value, bool maybe);

    /** Determines whether @p value is greater than or equal to ten. */
    virtual BaseSemantics::SValuePtr greaterOrEqualToTen(const BaseSemantics::SValuePtr &value);

    /** Return a Boolean for the specified flag combo for an instruction. */
    virtual BaseSemantics::SValuePtr flagsCombo(X86InstructionKind k);

    /** Enters a loop for a REP-, REPE-, or REPNE-prefixed instruction.  The return value is true if ECX is non-zero or the
     *  instruction doesn't have repeat prefix, and false otherwise. Use this in conjunction with repLeave(). */
    virtual BaseSemantics::SValuePtr repEnter(X86RepeatPrefix);

    /** Leave a loop for a REP-, REPE-, or REPNE-prefixed instruction.  The @p in_loop argument is the Boolean that indicates
     *  whether we just executed the instruction, and is usually the return value from the previous repEnter() call. If @p
     *  in_loop is false then this function is a no-op. Otherwise, the ECX register is decremented and, if it is non-zero and
     *  the repeat condition (true, equal, or not-equal) is satisified, then the EIP register is reset to the specified
     *  instruction address causing the instruction to be repeated. Use this in conjunction with repEnter(). */
    virtual void repLeave(X86RepeatPrefix, const BaseSemantics::SValuePtr &in_loop, rose_addr_t insn_va);

    /** Adds two values and adjusts flags.  This method can be used for subtraction if @p b is two's complement and @p
     *  invertCarries is set.  If @p cond is supplied, then the addition and flag adjustments are conditional.
     * @{ */
    virtual BaseSemantics::SValuePtr doAddOperation(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                    bool invertCarries, const BaseSemantics::SValuePtr &carryIn);
    virtual BaseSemantics::SValuePtr doAddOperation(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                    bool invertCarries, const BaseSemantics::SValuePtr &carryIn,
                                                    const BaseSemantics::SValuePtr &cond);
    /** @}*/

    /** Increments or decrements a value and adjusts flags.  If @p dec is set then the value is decremented instead of
     *  incremented. If @p setCarry is set then the CF flag is affected. */
    virtual BaseSemantics::SValuePtr doIncOperation(const BaseSemantics::SValuePtr &a, bool dec, bool setCarry);

    /** Implements the RCL, RCR, ROL, and ROR instructions for various operand sizes.  The rotate amount is always 8 bits wide
     * in the instruction, but the semantics mask off all but the low-order bits, keeping 5 bits in 32-bit mode and 6 bits in
     * 64-bit mode (indicated by the rotateSignificantBits argument). */
    virtual BaseSemantics::SValuePtr doRotateOperation(X86InstructionKind kind,
                                                       const BaseSemantics::SValuePtr &operand,
                                                       const BaseSemantics::SValuePtr &total_rotate,
                                                       size_t rotateSignificantBits);

    /** Implements the SHR, SAR, SHL, SAL, SHRD, and SHLD instructions for various operand sizes.  The shift amount is always 8
     *  bits wide in the instruction, but the semantics mask off all but the low-order bits, keeping 5 bits in 32-bit mode and
     *  7 bits in 64-bit mode (indicated by the @p shiftSignificantBits argument).  The semantics of SHL and SAL are
     *  identical (in fact, ROSE doesn't even define x86_sal). The @p source_bits argument contains the bits to be shifted into
     *  the result and is used only for SHRD and SHLD instructions. */
    virtual BaseSemantics::SValuePtr doShiftOperation(X86InstructionKind kind,
                                                      const BaseSemantics::SValuePtr &operand,
                                                      const BaseSemantics::SValuePtr &source_bits,
                                                      const BaseSemantics::SValuePtr &total_shift,
                                                      size_t shiftSignificantBits);
};
        
} // namespace
} // namespace
#endif
