#ifndef Rose_TraceSemantics2_H
#define Rose_TraceSemantics2_H

#include "BaseSemantics2.h"
#include "threadSupport.h"

namespace BinaryAnalysis {                      // documented elsewhere
namespace InstructionSemantics2 {               // documented elsewhere

/** A semantics domain wrapper that prints and checks all RISC operators as they occur.
 *
 *  This semantics domain provides only a RiscOperators class, which chains most methods to a subdomain specified either
 *  with its constructor or via set_subdomain().  In order to add tracing to any domain, simply wrap that domain's
 *  RiscOperators object inside a TraceSemantics' RiscOperators:
 *
 * @code
 *  BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(....);
 *  ops = TraceSemantics::RiscOperators::instance(ops); // this turns on tracing
 * @endcode
 *
 *  When an instruction is processed, it will emit traces on standard output (by default; see set_stream()).  The messages
 *  look something like this (the exact format depends on the subdomain being traced):
 *
 * @code
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: startInstruction(mov    al, BYTE PTR ss:[ebp + 0x10])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: number_(32, 3) = 3[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: number_(32, 134512800) = 0x080480a0[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: add(0x080480a0[32], 3[32]) = 0x080480a3[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: writeRegister(eip, 0x080480a3[32])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: readRegister(ebp) = v3284[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: number_(8, 16) = 16[8]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: signExtend(16[8], 32) = 16[32]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: add(v3284[32], 16[32]) = (add[32] v3284[32] 16[32])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: boolean_(1) = 1[1]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: readMemory(ss, (add[32] v3284[32] 16[32]), 1[1], 8) = v3285[8]
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: writeRegister(al, v3285[8])
 *  Symbolic@0x28470a0 insn@0x080480a0[0]: finishInstruction(mov    al, BYTE PTR ss:[ebp + 0x10])
 * @endcode
 *
 *  The TraceSemantics also checks for problems with operand and return value widths and reports them in the output
 *  also. Tracing can be turned off either by specifying a NULL file pointer for set_stream(), or by unwrapping the subdomain's
 *  RISC operators, something along these lines:
 *
 * @code
 *  ops = TraceSemantics::RiscOperators::promote(ops)->get_subdomain();
 *  dispatcher->set_operators(ops);
 * @endcode
 */
namespace TraceSemantics {

/*******************************************************************************************************************************
 *                                      RISC Operators
 *******************************************************************************************************************************/

/** Smart pointer to a RiscOperators object.  RiscOperators objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Wraps RISC operators so they can be traced. */
class RiscOperators: public BaseSemantics::RiscOperators {
protected:
    class LinePrefix: public RTS_Message::Prefix {
    private:
        BaseSemantics::RiscOperatorsPtr ops;
        SgAsmInstruction *cur_insn;
        size_t ninsns;
    public:
        LinePrefix(): cur_insn(NULL), ninsns(0) {}
        void set_ops(const BaseSemantics::RiscOperatorsPtr &ops) { this->ops = ops; }
        void set_insn(SgAsmInstruction *insn);
        virtual void operator()(FILE*) /*override*/;
    };

    BaseSemantics::RiscOperatorsPtr subdomain;          // Domain to which all our RISC operators chain
    LinePrefix line_prefix;
    RTS_Message mesg;                                   // Formats tracing output


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors.
protected:
    // use the version that takes a subdomain instead of this c'tor
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(protoval, solver), mesg(stderr, &line_prefix) {
        set_name("Trace");
    }

    // use the version that takes a subdomain instead of this c'tor.
    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(state, solver), mesg(stderr, &line_prefix) {
        set_name("Trace");
    }

    explicit RiscOperators(const BaseSemantics::RiscOperatorsPtr &subdomain)
        : BaseSemantics::RiscOperators(subdomain->get_state(), subdomain->get_solver()), subdomain(subdomain),
          mesg(stderr, &line_prefix) {
        set_name("Trace");
        line_prefix.set_ops(subdomain);
    }

public:
    virtual ~RiscOperators() {
        mesg.mesg("operators destroyed");
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.
public:
    /** Instantiates a new RiscOperators object.  This domain does not create any of its own values--it only wraps another
     *  domains RISC operators. Therefore, the supplied protoval and solver are not actually used.  It is probably better to
     *  construct the TraceSemantics' RISC operators with the constructor that takes the subdomain's RISC operators. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiates a new RiscOperators object.  This domain does not manage any state--it only wraps another domains RISC
     *  operators. Therefore, the supplied protoval and solver are not actually used.  It is probably better to construct the
     *  TraceSemantics' RISC operators with the constructor that takes the subdomain's RISC operators. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }
    
    /** Instantiate a new RiscOperators object. The @p subdomain argument should be the RISC operators that we want to
     * trace. */
    static RiscOperatorsPtr instance(const BaseSemantics::RiscOperatorsPtr &subdomain) {
        assert(subdomain!=NULL);
        RiscOperatorsPtr self = subdomain->get_state()!=NULL ?
                                RiscOperatorsPtr(new RiscOperators(subdomain->get_state(), subdomain->get_solver())) :
                                RiscOperatorsPtr(new RiscOperators(subdomain->get_protoval(), subdomain->get_solver()));
        self->subdomain = subdomain;
        self->line_prefix.set_ops(subdomain);
        return self;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(state, solver);
    }

    /** Wraps a subdomain's RISC operators to add tracing. */
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::RiscOperatorsPtr &subdomain) {
        return instance(subdomain);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to trace operators. This is a checked conversion--it
     *  will fail if @p from does not point to a TraceSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        assert(retval!=NULL);
        return retval;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first defined at this level of the class hierarchy
public:
    /** Obtain a pointer to the subdomain's RISC operators.  The subdomain is the one that is being traced. */
    virtual BaseSemantics::RiscOperatorsPtr get_subdomain() const {
        return subdomain;
    }

    /** Set the subdomain that is to be traced. All our RISC operators will simply chain to the subdomain operators. */
    void set_subdomain(const BaseSemantics::RiscOperatorsPtr &subdomain) {
        this->subdomain = subdomain;
        line_prefix.set_ops(subdomain);
    }

    /** Check that we have a valid subdomain.  If the subdomain isn't value (hasn't been set) then throw an exception. */
    void check_subdomain() const {
        if (subdomain==NULL)
            throw BaseSemantics::Exception("subdomain is not set; nothing to trace", NULL);
    }

    /** Set the I/O stream to which tracing will be emitted.  The default is standard error. */
    void set_stream(FILE *f) { mesg.set_file(f); }

    /** Obtain the I/O stream to which tracing is being emitted. */
    FILE *get_stream() const { return mesg.get_file(); }
        

protected:
    std::string toString(const BaseSemantics::SValuePtr&);
    void check_equal_widths(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    const BaseSemantics::SValuePtr &check_width(const BaseSemantics::SValuePtr &a, size_t nbits,
                                                const std::string &what="result");
    std::string register_name(const RegisterDescriptor&);

    void before(const std::string&);
    void before(const std::string&, const RegisterDescriptor&);
    void before(const std::string&, const RegisterDescriptor&, const BaseSemantics::SValuePtr&);
    void before(const std::string&, const RegisterDescriptor&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                size_t);
    void before(const std::string&, const RegisterDescriptor&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                const BaseSemantics::SValuePtr&);
    void before(const std::string&, SgAsmInstruction*);
    void before(const std::string&, size_t);
    void before(const std::string&, size_t, uint64_t);
    void before(const std::string&, const BaseSemantics::SValuePtr&);
    void before(const std::string&, const BaseSemantics::SValuePtr&, size_t);
    void before(const std::string&, const BaseSemantics::SValuePtr&, size_t, size_t);
    void before(const std::string&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    void before(const std::string&, const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                const BaseSemantics::SValuePtr&);

    void after();
    const BaseSemantics::SValuePtr& after(const BaseSemantics::SValuePtr&);
    const BaseSemantics::SValuePtr& after(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    void after(const BaseSemantics::Exception&);
    void after_exception();
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we override from our super class
public:
    virtual BaseSemantics::SValuePtr get_protoval() const /*override*/;
    virtual void set_solver(SMTSolver*) /*override*/;
    virtual SMTSolver *get_solver() const /*override*/;
    virtual BaseSemantics::StatePtr get_state() /*override*/;
    virtual void set_state(const BaseSemantics::StatePtr&) /*override*/;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const /*override*/;
    virtual size_t get_ninsns() const /*override*/;
    virtual void set_ninsns(size_t n) /*override*/;
    virtual SgAsmInstruction *get_insn() const /*override*/;
    virtual void startInstruction(SgAsmInstruction*) /*override*/;
    virtual void finishInstruction(SgAsmInstruction*) /*override*/;
    
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) /*override*/;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) /*override*/;
    virtual BaseSemantics::SValuePtr boolean_(bool value) /*override*/;

    virtual BaseSemantics::SValuePtr filterCallTarget(const BaseSemantics::SValuePtr&) /*override*/;
    virtual BaseSemantics::SValuePtr filterReturnTarget(const BaseSemantics::SValuePtr&) /*override*/;
    virtual BaseSemantics::SValuePtr filterIndirectJumpTarget(const BaseSemantics::SValuePtr&) /*override*/;
    virtual void hlt() /*override*/;
    virtual void cpuid() /*override*/;
    virtual BaseSemantics::SValuePtr rdtsc() /*override*/;

    // The actual RISC operators. These are pure virtual in the base class
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr&, size_t begin_bit, size_t end_bit);
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                         const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr&, size_t nbits);
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr&, size_t nbits);
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&,
                                                    const BaseSemantics::SValuePtr&, BaseSemantics::SValuePtr&/*out*/);
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr&, const BaseSemantics::SValuePtr&);

    virtual void interrupt(int majr, int minr);

    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor&);
    virtual void writeRegister(const RegisterDescriptor&, const BaseSemantics::SValuePtr&);
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr&cond, size_t nbits);
    virtual void writeMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data, const BaseSemantics::SValuePtr &cond);
};

} // namespace
} // namespace
} // namespace

#endif
