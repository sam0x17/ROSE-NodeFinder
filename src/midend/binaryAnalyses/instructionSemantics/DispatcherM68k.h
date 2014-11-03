#ifndef ROSE_DispatcherM68k_H
#define ROSE_DispatcherM68k_H

#include "BaseSemantics2.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

typedef boost::shared_ptr<class DispatcherM68k> DispatcherM68kPtr;

class DispatcherM68k: public BaseSemantics::Dispatcher {
protected:
    // prototypical constructor
    DispatcherM68k() {}

    explicit DispatcherM68k(const BaseSemantics::RiscOperatorsPtr &ops): BaseSemantics::Dispatcher(ops) {
        set_register_dictionary(RegisterDictionary::dictionary_coldfire_emac());
        regcache_init();
        iproc_init();
    }

    /** Loads the iproc table with instruction processing functors. This normally happens from the constructor. */
    void iproc_init();

    /** Load the cached register descriptors.  This happens at construction and on set_register_dictionary() calls. */
    void regcache_init();

public:
    /** Cached register.
     *
     *  This register is cached so that there are not so many calls to Dispatcher::findRegister(). Changing the register
     *  dictionary via set_register_dictionary() invalidates all entries of the cache.
     *
     * @{ */
    RegisterDescriptor REG_D[8], REG_A[8], REG_FP[8], REG_PC, REG_CCR_C, REG_CCR_V, REG_CCR_Z, REG_CCR_N, REG_CCR_X;
    RegisterDescriptor REG_MACSR_SU, REG_MACSR_FI, REG_MACSR_N, REG_MACSR_Z, REG_MACSR_V, REG_MACSR_C, REG_MAC_MASK;
    RegisterDescriptor REG_MACEXT0, REG_MACEXT1, REG_MACEXT2, REG_MACEXT3, REG_SSP, REG_SR_S, REG_SR, REG_VBR;
    /** @} */

    /** Construct a prototypical dispatcher.  The only thing this dispatcher can be used for is to create another dispatcher
     *  with the virtual @ref create method. */
    static DispatcherM68kPtr instance() {
        return DispatcherM68kPtr(new DispatcherM68k);
    }

    /** Constructor. */
    static DispatcherM68kPtr instance(const BaseSemantics::RiscOperatorsPtr &ops) {
        return DispatcherM68kPtr(new DispatcherM68k(ops));
    }

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr &ops) const ROSE_OVERRIDE {
        return instance(ops);
    }

    /** Dynamic cast to DispatcherM68kPtr with assertion. */
    static DispatcherM68kPtr promote(const BaseSemantics::DispatcherPtr &d) {
        DispatcherM68kPtr retval = boost::dynamic_pointer_cast<DispatcherM68k>(d);
        ASSERT_not_null(retval);
        return retval;
    }

    virtual void set_register_dictionary(const RegisterDictionary *regdict) ROSE_OVERRIDE;

    virtual int iproc_key(SgAsmInstruction *insn_) const ROSE_OVERRIDE {
        SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(insn_);
        ASSERT_not_null(insn);
        return insn->get_kind();
    }

    virtual BaseSemantics::SValuePtr read(SgAsmExpression*, size_t value_nbits, size_t addr_nbits=32) ROSE_OVERRIDE;

    /** Determines if an instruction should branch. */
    BaseSemantics::SValuePtr condition(M68kInstructionKind, BaseSemantics::RiscOperators*);
};

} // namespace
} // namespace
} // namespace

#endif
