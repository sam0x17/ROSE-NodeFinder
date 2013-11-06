#ifndef Rose_InsnSemanticsExpr_H
#define Rose_InsnSemanticsExpr_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

/** Cache hash values in nodes.  If this is defined, then the @p hashval data member member is used to store a hash for the
 *  node and its children.  The hash can be used to prove that two expressions are not structurally equivalent, thus avoiding a
 *  more expensive traversal of an expression tree. */
#define InsnSemanticsExpr_USE_HASHES

class SMTSolver;

/** Namespace supplying types and functions for symbolic expressions. These are used by certain instruction semantics policies
 *  and satisfiability modulo theory (SMT) solvers. These expressions are tailored to bit-vector and integer difference logics,
 *  whereas the expression nodes in other parts of ROSE have different goals. */
namespace InsnSemanticsExpr {

/** Operators for internal nodes of the expression tree. Commutative operators generally take one or more operands.  Operators
 *  such as shifting, extending, and truncating have the size operand appearing before the bit vector on which to operate (this
 *  makes the output more human-readable since the size operand is often a constant). */
enum Operator 
    {
    OP_ADD,                 /**< Addition. One or more operands, all the same width. */
    OP_AND,                 /**< Boolean AND. Operands are all Boolean (1-bit) values. See also OP_BV_AND. */
    OP_ASR,                 /**< Arithmetic shift right. Operand B shifted by A bits; 0 <= A < width(B). */
    OP_BV_AND,              /**< Bitwise AND. One or more operands, all the same width. */
    OP_BV_OR,               /**< Bitwise OR. One or more operands, all the same width. */
    OP_BV_XOR,              /**< Bitwise exclusive OR. One or more operands, all the same width. */
    OP_CONCAT,              /**< Concatenation. Operand A becomes high-order bits. Any number of operands. */
    OP_EQ,                  /**< Equality. Two operands, both the same width. */
    OP_EXTRACT,             /**< Extract subsequence of bits. Extract bits [A..B) of C. 0 <= A < B <= width(C). */
    OP_INVERT,              /**< Boolean inversion. One operand. */
    OP_ITE,                 /**< If-then-else. A must be one bit. Returns B if A is set, C otherwise. */
    OP_LSSB,                /**< Least significant set bit or zero. One operand. */
    OP_MSSB,                /**< Most significant set bit or zero. One operand. */
    OP_NE,                  /**< Inequality. Two operands, both the same width. */
    OP_NEGATE,              /**< Arithmetic negation. One operand. */
    OP_NOOP,                /**< No operation. Used only by the default constructor. */
    OP_OR,                  /**< Boolean OR. Operands are all Boolean (1-bit) values. See also OP_BV_OR. */
    OP_READ,                /**< Read a value from memory.  Arguments are the memory state and the address expression. */
    OP_ROL,                 /**< Rotate left. Rotate bits of B left by A bits.  0 <= A < width(B). */
    OP_ROR,                 /**< Rotate right. Rotate bits of B right by A bits. 0 <= B < width(B).  */
    OP_SDIV,                /**< Signed division. Two operands, A/B. Result width is width(A). */
    OP_SEXTEND,             /**< Signed extension at msb. Extend B to A bits by replicating B's most significant bit. */
    OP_SGE,                 /**< Signed greater-than-or-equal. Two operands of equal width. Result is Boolean. */
    OP_SGT,                 /**< Signed greater-than. Two operands of equal width. Result is Boolean. */
    OP_SHL0,                /**< Shift left, introducing zeros at lsb. Bits of B are shifted by A, where 0 <=A < width(B). */
    OP_SHL1,                /**< Shift left, introducing ones at lsb. Bits of B are shifted by A, where 0 <=A < width(B). */
    OP_SHR0,                /**< Shift right, introducing zeros at msb. Bits of B are shifted by A, where 0 <=A <width(B). */
    OP_SHR1,                /**< Shift right, introducing ones at msb. Bits of B are shifted by A, where 0 <=A <width(B). */
    OP_SLE,                 /**< Signed less-than-or-equal. Two operands of equal width. Result is Boolean. */
    OP_SLT,                 /**< Signed less-than. Two operands of equal width. Result is Boolean. */
    OP_SMOD,                /**< Signed modulus. Two operands, A%B. Result width is width(B). */
    OP_SMUL,                /**< Signed multiplication. Two operands A*B. Result width is width(A)+width(B). */
    OP_UDIV,                /**< Signed division. Two operands, A/B. Result width is width(A). */
    OP_UEXTEND,             /**< Unsigned extention at msb. Extend B to A bits by introducing zeros at the msb of B. */
    OP_UGE,                 /**< Unsigned greater-than-or-equal. Two operands of equal width. Boolean result. */
    OP_UGT,                 /**< Unsigned greater-than. Two operands of equal width. Result is Boolean. */
    OP_ULE,                 /**< Unsigned less-than-or-equal. Two operands of equal width. Result is Boolean. */
    OP_ULT,                 /**< Unsigned less-than. Two operands of equal width. Result is Boolean (1-bit vector). */
    OP_UMOD,                /**< Unsigned modulus. Two operands, A%B. Result width is width(B). */
    OP_UMUL,                /**< Unsigned multiplication. Two operands, A*B. Result width is width(A)+width(B). */
    OP_WRITE,               /**< Write (update) memory with a new value. Arguments are memory, address and value. */
    OP_ZEROP,               /**< Equal to zero. One operand. Result is a single bit, set iff A is equal to zero. */
};

const char *to_str(Operator o);

class TreeNode;
class InternalNode;
class LeafNode;

typedef boost::shared_ptr<const TreeNode> TreeNodePtr;
typedef boost::shared_ptr<const InternalNode> InternalNodePtr;
typedef boost::shared_ptr<const LeafNode> LeafNodePtr;
typedef std::vector<TreeNodePtr> TreeNodes;

/** Controls formatting of expression trees when printing. */
struct Formatter {
    typedef Map<uint64_t, uint64_t> RenameMap;
    enum ShowComments {
        CMT_SILENT,                             /**< Do not show comments. */
        CMT_AFTER,                              /**< Show comments after the node. */
        CMT_INSTEAD,                            /**< Like CMT_AFTER, but show comments instead of variable names. */
    };
    Formatter(): show_comments(CMT_INSTEAD), do_rename(false), add_renames(true) {}
    ShowComments show_comments;                 /**< Show node comments when printing? */
    bool do_rename;                             /**< Use the @p renames map to rename variables to shorter names? */
    bool add_renames;                           /**< Add additional entries to the @p renames as variables are encountered? */
    RenameMap renames;                          /**< Map for renaming variables to use smaller integers. */
};

/** Base class for visiting nodes during expression traversal. */
class Visitor {
public:
    virtual ~Visitor() {}
    virtual void operator()(const TreeNodePtr&) = 0;
};

/** Any node of an expression tree for instruction semantics, from which the InternalNode and LeafNode classes are derived.
 *  Every node has a specified number of significant bits that is constant over the life of the node.
 *
 *  In order that subtrees can be freely assigned as children of other nodes (provided the structure as a whole remains a
 *  lattice and not a graph with cycles), tree nodes are always referenced through boost::shared_ptr<const T> where T is one of
 *  the tree node types: TreeNode, InternalNode, or LeafNode.  For convenience, we define TreeNodePtr, InternalNodePtr, and
 *  LeafNodePtr typedefs.  The shared_ptr owns the pointer to the tree node and thus the tree node pointer should never be
 *  deleted explicitly. */
class TreeNode: public boost::enable_shared_from_this<TreeNode> {
protected:
    size_t nbits;               /**< Number of significant bits. Constant over the life of the node. */
    mutable std::string comment; /**< Optional comment. Only for debugging; not significant for any calculation. */
    mutable uint64_t hashval;   /**< Optional hash used as a quick way to indicate that two expressions are different. */
public:
    TreeNode(size_t nbits, std::string comment=""): nbits(nbits), comment(comment), hashval(0) { assert(nbits>0); }

    /** Returns true if two expressions must be equal (cannot be unequal).  If an SMT solver is specified then that solver is
     * used to answer this question, otherwise equality is established by looking only at the structure of the two
     * expressions. Two expressions can be equal without being the same width (e.g., a 32-bit constant zero is equal to a
     * 16-bit constant zero). */
    virtual bool must_equal(const TreeNodePtr& other, SMTSolver*) const = 0;

    /** Returns true if two expressions might be equal, but not necessarily be equal. */
    virtual bool may_equal(const TreeNodePtr& other, SMTSolver*) const = 0;

    /** Tests two expressions for structural equivalence.  Two leaf nodes are equivalent if they are the same width and have
     *  equal values or are the same variable. Two internal nodes are equivalent if they are the same width, the same
     *  operation, have the same number of children, and those children are all pairwise equivalent. */
    virtual bool equivalent_to(const TreeNodePtr& other) const = 0;

    /** Substitute one value for another. Finds all occurrances of @p from in this expression and replace them with @p to. If a
     * substitution occurs, then a new expression is returned. The matching of @p from to sub-parts of this expression uses
     * structural equivalence, the equivalent_to() predicate. The @p from and @p to expressions must have the same width. */
    virtual TreeNodePtr substitute(const TreeNodePtr &from, const TreeNodePtr &to) const = 0;

    /** Returns true if the expression is a known value.
     *
     *  FIXME: The current implementation returns true only when @p this node is leaf node with a known value. Since
     *         InsnSemanticsExpr does not do constant folding, this is of limited use. [RPM 2010-06-08]. */
    virtual bool is_known() const = 0;

    /** Returns the integer value of a node for which is_known() returns true.  The high-order bits, those beyond the number of
     *  significant bits returned by get_nbits(), are guaranteed to be zero. */
    virtual uint64_t get_value() const = 0;

    /** Accessors for the comment string associated with a node. Comments can be changed after a node has been created since
     *  the comment is not intended to be used for anything but annotation and/or debugging. I.e., comments are not
     *  considered significant for comparisons, computing hash values, etc.
     * @{ */
    const std::string& get_comment() const { return comment; }
    void set_comment(const std::string &s) const { comment=s; }
    /** @} */

    /** Returns the number of significant bits.  An expression with a known value is guaranteed to have all higher-order bits
     *  cleared. */
    size_t get_nbits() const { return nbits; }

    /** Traverse the expression.  The expression is traversed in a depth-first visit, invoking the functor at each node of the
     *  expression tree. */
    virtual void depth_first_visit(Visitor*) const = 0;

    /** Returns the variables appearing in the expression. */
    std::set<LeafNodePtr> get_variables() const;

    /** Dynamic cast of this object to an internal node. */
    InternalNodePtr isInternalNode() const {
        return boost::dynamic_pointer_cast<const InternalNode>(shared_from_this());
    }

    /** Dynamic cast of this object to a leaf node. */
    LeafNodePtr isLeafNode() const {
        return boost::dynamic_pointer_cast<const LeafNode>(shared_from_this());
    }

    /** Returns true if this node has a hash value computed and cached. The hash value zero is reserved to indicate that no
     *  hash has been computed; if a node happens to actually hash to zero, it will not be cached and will be recomputed for
     *  every call to hash(). */
    bool is_hashed() const { return hashval!=0; }

    /** Returns (and caches) the hash value for this node.  If a hash value is not cached in this node, then a new hash value
     *  is computed and cached. */
    uint64_t hash() const;

    /** A node with formatter. See the with_format() method. */
    class WithFormatter {
    private:
        TreeNodePtr node;
        Formatter &formatter;
    public:
        WithFormatter(const TreeNodePtr &node, Formatter &formatter): node(node), formatter(formatter) {}
        void print(std::ostream &stream) const { node->print(stream, formatter); }
    };

    /** Combines a node with a formatter for printing.  This is used for convenient printing with the "<<" operator. For
     *  instance:
     *
     * @code
     *  Formatter fmt;
     *  fmt.show_comments = Formatter::CMT_AFTER; //show comments after the variable
     *  TreeNodePtr expression = ...;
     *  std::cout <<"method 1: "; expression->print(std::cout, fmt); std::cout <<"\n";
     *  std::cout <<"method 2: " <<expression->with_format(fmt) <<"\n";
     *  std::cout <<"method 3: " <<*expression+fmt <<"\n";
     * 
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) const { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) const { return with_format(fmt); }
    /** @} */

    /** Print the expression to a stream.  The output is an S-expression with no line-feeds. */
    void print(std::ostream &stream) const { Formatter formatter; print(stream, formatter); }

    /** Print the expression to a stream.  The format of the output is controlled by the Formatter argument. */
    virtual void print(std::ostream&, Formatter&) const = 0;

};

/** Operator-specific simplification methods. */
class Simplifier {
public:
    virtual ~Simplifier() {}

    /** Constant folding. The range @p begin (inclusive) to @p end (exclusive) must contain at least two nodes and all of
     *  the nodes must be leaf nodes with known values.  This method returns a new folded node if folding is possible, or
     *  the null pointer if folding is not possible. */
    virtual TreeNodePtr fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const {
        return TreeNodePtr();
    }

    /** Rewrite the entire expression to something simpler. Returns the new node if the node can be simplified, otherwise
     *  returns null. */
    virtual TreeNodePtr rewrite(const InternalNode*) const {
        return TreeNodePtr();
    }
};

struct AddSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const /*override*/;
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct AndSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const /*override*/;
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct OrSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const /*override*/;
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct XorSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const /*override*/;
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SmulSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const /*override*/;
};
struct UmulSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const /*override*/;
};
struct ConcatSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const /*override*/;
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct ExtractSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct AsrSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct InvertSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct NegateSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct IteSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct NoopSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct RolSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct RorSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct UextendSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SextendSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SgeSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SgtSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SleSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SltSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct UgeSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct UgtSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct UleSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct UltSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct ZeropSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SdivSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct SmodSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct UdivSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct UmodSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct ShlSimplifier: Simplifier {
    bool newbits;
    ShlSimplifier(bool newbits): newbits(newbits) {}
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct ShrSimplifier: Simplifier {
    bool newbits;
    ShrSimplifier(bool newbits): newbits(newbits) {}
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct LssbSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};
struct MssbSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const /*override*/;
};

/** Internal node of an expression tree for instruction semantics. Each internal node has an operator (constant for the life of
 *  the node and obtainable with get_operator()) and zero or more children. Children are added to the internal node during the
 *  construction phase. Once construction is complete, the children should only change in ways that don't affect the value of
 *  the node as a whole (since this node might be pointed to by any number of expressions). */
class InternalNode: public TreeNode {
private:
    Operator op;
    TreeNodes children;

    // Constructors should not be called directly.  Use the create() class method instead. This is to help prevent
    // accidently using pointers to these objects -- all access should be through boost::shared_ptr<>.
    InternalNode(size_t nbits, Operator op, const std::string comment="")
        : TreeNode(nbits, comment), op(op) {}
    InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, std::string comment="")
        : TreeNode(nbits, comment), op(op) {
        add_child(a);
    }
    InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, std::string comment="")
        : TreeNode(nbits, comment), op(op) {
        add_child(a);
        add_child(b);
    }
    InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, const TreeNodePtr &c,
                 std::string comment="")
        : TreeNode(nbits, comment), op(op) {
        add_child(a);
        add_child(b);
        add_child(c);
    }
    InternalNode(size_t nbits, Operator op, const TreeNodes &children, std::string comment="")
        : TreeNode(nbits, comment), op(op) {
        for (size_t i=0; i<children.size(); ++i)
            add_child(children[i]);
    }

public:
    /** Create a new expression node. Although we're creating internal nodes, the simplification process might replace it with
     *  a leaf node. Use these class methods instead of c'tors. */
    static TreeNodePtr create(size_t nbits, Operator op, const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, comment));
        return retval->simplifyTop();
    }
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, a, comment));
        return retval->simplifyTop();
    }
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b,
                                  const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, a, b, comment));
        return retval->simplifyTop();
    }
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, const TreeNodePtr &c,
                                  const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, a, b, c, comment));
        return retval->simplifyTop();
    }
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodes &children, const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, children, comment));
        return retval->simplifyTop();
    }
    /** @} */

    /* see superclass, where these are pure virtual */
    virtual void print(std::ostream&, Formatter&) const;
    virtual bool must_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool may_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool equivalent_to(const TreeNodePtr &other) const;
    virtual TreeNodePtr substitute(const TreeNodePtr &from, const TreeNodePtr &to) const;
    virtual bool is_known() const {
        return false; /*if it's known, then it would have been folded to a leaf*/
    }
    virtual uint64_t get_value() const { ROSE_ASSERT(!"not a constant value"); return 0;}
    virtual void depth_first_visit(Visitor*) const;

    /** Returns the number of children. */
    size_t size() const { return children.size(); }

    /** Returns the specified child. */
    TreeNodePtr child(size_t idx) const { assert(idx<children.size()); return children[idx]; }

    /** Returns all children. */
    TreeNodes get_children() const { return children; }

    /** Returns the operator. */
    Operator get_operator() const { return op; }

    /** Simplifies the specified internal node. Returns a new node if necessary, otherwise returns this. */
    TreeNodePtr simplifyTop() const;

    /** Perform constant folding.  This method returns either a new expression (if changes were mde) or the original
     *  expression. The simplifier is specific to the kind of operation at the node being simplified. */
    TreeNodePtr constant_folding(const Simplifier &simplifier) const;

    /** Simplifies non-associative operators by flattening the specified internal node with its children that are the same
     *  internal node type. Call this only if the top node is a truly non-associative. A new node is returned only if
     *  changed. When calling both nonassociative and commutative, it's usually more appropriate to call nonassociative
     *  first. */
    InternalNodePtr nonassociative() const;

    /** Simplifies commutative operators by sorting arguments. The arguments are sorted so that all the internal nodes come
     *  before the leaf nodes. Call this only if the top node is truly commutative.  A new node is returned only if
     *  changed. When calling both nonassociative and commutative, it's usually more appropriate to call nonassociative
     *  first. */
    InternalNodePtr commutative() const;

    /** Simplifies involutary operators.  An involutary operator is one that is its own inverse.  This method should only be
     *  called if this node is an internal node whose operator has the involutary property (such as invert or negate). Returns
     *  either a new expression that is simplified, or the original expression. */
    TreeNodePtr involutary() const;

    /** Removes identity arguments. Returns either a new expression or the original expression. */
    TreeNodePtr identity(uint64_t ident) const;

    /** Replaces a binary operator with its only argument. Returns either a new expression or the original expression. */
    TreeNodePtr unaryNoOp() const;

    /** Simplify an internal node. Returns a new node if this node could be simplified, otherwise returns this node. When
     *  the simplification could result in a leaf node, we return an OP_NOOP internal node instead. */
    TreeNodePtr rewrite(const Simplifier &simplifier) const;

protected:
    /** Appends @p child as a new child of this node. The modification is done in place, so one must be careful that this node
     *  is not part of other expressions.  It is safe to call add_child() on a node that was just created and not used anywhere
     *  yet. */
    void add_child(const TreeNodePtr &child);
};

/** Leaf node of an expression tree for instruction semantics.
 *
 *  A leaf node is either a known bit vector value, a free bit vector variable, or a memory state. */
class LeafNode: public TreeNode {
private:
    enum LeafType { CONSTANT, BITVECTOR, MEMORY };
    LeafType leaf_type;
    union {
        uint64_t ival;                  /**< Integer (unsigned) value when 'known' is true; unused msb are zero */
        uint64_t name;                  /**< Variable ID number when 'known' is false. */
    };

    // Private to help prevent creating pointers to leaf nodes.  See create_* methods instead.
    LeafNode(std::string comment=""): TreeNode(32, comment), leaf_type(CONSTANT), ival(0) {}

    static uint64_t name_counter;

public:
    /** Construct a new free variable with a specified number of significant bits. */
    static LeafNodePtr create_variable(size_t nbits, std::string comment="");

    /** Construct a new integer with the specified number of significant bits. Any high-order bits beyond the specified size
     *  will be zeroed. */
    static LeafNodePtr create_integer(size_t nbits, uint64_t n, std::string comment="");

    /** Construct a new memory state.  A memory state is a function that maps a 32-bit address to a value of specified size. */
    static LeafNodePtr create_memory(size_t nbits, std::string comment="");

    /* see superclass, where these are pure virtual */
    virtual bool is_known() const;
    virtual uint64_t get_value() const;
    virtual void print(std::ostream&, Formatter&) const;
    virtual bool must_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool may_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool equivalent_to(const TreeNodePtr &other) const;
    virtual TreeNodePtr substitute(const TreeNodePtr &from, const TreeNodePtr &to) const;
    virtual void depth_first_visit(Visitor*) const;

    /** Is the node a bitvector variable? */
    virtual bool is_variable() const;

    /** Does the node represent memory? */
    virtual bool is_memory() const;

    /** Returns the name of a free variable.  The output functions print variables as "vN" where N is an integer. It is this N
     *  that this method returns.  It should only be invoked on leaf nodes for which is_known() returns false. */
    uint64_t get_name() const;

};

std::ostream& operator<<(std::ostream &o, const TreeNode&);
std::ostream& operator<<(std::ostream &o, const TreeNode::WithFormatter&);

} // namespace
#endif
