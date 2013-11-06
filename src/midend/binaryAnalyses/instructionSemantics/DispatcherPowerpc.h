// Semantics for 32-bit Motorola-IBM PowerPC microprocessors using ROSE instruction semantics API2
// This code was derived from $ROSE/projects/assemblyToSourceAst/powerpcInstructionSemantics.h,
// which is mostly a copy of $ROSE/projects/SemanticSignatureVectors/powerpcInstructionSemantics.h
//
// The ROSE style guide indicates that PowerPC, when used as part of a symbol in ROSE source code,
// should be capitalized as "Powerpc" (e.g., "DispatcherPowerpc", the same rule that consistently
// capitializes x86 as "DispatcherX86").

#ifndef ROSE_DispatcherPpc_H
#define ROSE_DispatcherPpc_H

#include "BaseSemantics2.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {

typedef boost::shared_ptr<class DispatcherPowerpc> DispatcherPowerpcPtr;

class DispatcherPowerpc: public BaseSemantics::Dispatcher {
protected:
    explicit DispatcherPowerpc(const BaseSemantics::RiscOperatorsPtr &ops): BaseSemantics::Dispatcher(ops) {
        set_register_dictionary(RegisterDictionary::dictionary_powerpc());
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
    RegisterDescriptor REG_IAR, REG_LR, REG_XER, REG_CR, REG_CR0, REG_CTR;
    /** @}*/

    /** Constructor. */
    static DispatcherPowerpcPtr instance(const BaseSemantics::RiscOperatorsPtr &ops) {
        return DispatcherPowerpcPtr(new DispatcherPowerpc(ops));
    }

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr &ops) const /*override*/ {
        return instance(ops);
    }

    /** Dynamic cast to a DispatcherPowerpcPtr with assertion. */
    static DispatcherPowerpcPtr promote(const BaseSemantics::DispatcherPtr &d) {
        DispatcherPowerpcPtr retval = boost::dynamic_pointer_cast<DispatcherPowerpc>(d);
        assert(retval!=NULL);
        return retval;
    }

    virtual void set_register_dictionary(const RegisterDictionary *regdict) /*override*/;

    virtual int iproc_key(SgAsmInstruction *insn_) const /*override*/ {
        SgAsmPowerpcInstruction *insn = isSgAsmPowerpcInstruction(insn_);
        assert(insn!=NULL);
        return insn->get_kind();
    }

    /** Write status flags for result. */
    virtual void record(const BaseSemantics::SValuePtr &result);
};
        
} /*namespace*/
} /*namespace*/
#endif
