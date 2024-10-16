/*
    pybind/operator.h: Metatemplates for operator overloading

    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <pybind/pybind.h>
#include <type_traits>

NAMESPACE_BEGIN(pybind)
NAMESPACE_BEGIN(detail)

/// Enumeration with all supported operator types
enum op_id : int {
    op_add, op_sub, op_mul, op_div, op_mod, op_divmod, op_pow, op_lshift,
    op_rshift, op_and, op_xor, op_or, op_neg, op_pos, op_abs, op_invert,
    op_int, op_long, op_float, op_str, op_cmp, op_gt, op_ge, op_lt, op_le,
    op_eq, op_ne, op_iadd, op_isub, op_imul, op_idiv, op_imod, op_ilshift,
    op_irshift, op_iand, op_ixor, op_ior, op_complex, op_bool, op_nonzero,
    op_repr, op_truediv
};

enum op_type : int {
    op_l, /* base type on left */
    op_r, /* base type on right */
    op_u  /* unary operator */
};

struct self_t { };
static const self_t self = self_t();

/// Type for an unused type slot
struct undefined_t { };

/// Don't warn about an unused variable
inline self_t __self() { return self; }

/// base template of operator implementations
template <op_id, op_type, typename B, typename L, typename R> struct op_impl { };

/// Operator implementation generator
template <op_id id, op_type ot, typename L, typename R> struct op_ {
    template <typename Base, typename Holder, typename... Extra> void execute(pybind::class_<Base, Holder> &class_, Extra&&... extra) const {
        typedef typename std::conditional<std::is_same<L, self_t>::value, Base, L>::type L_type;
        typedef typename std::conditional<std::is_same<R, self_t>::value, Base, R>::type R_type;
        typedef op_impl<id, ot, Base, L_type, R_type> op;
        class_.def(op::name(), &op::execute, std::forward<Extra>(extra)...);
    }
    template <typename Base, typename Holder, typename... Extra> void execute_cast(pybind::class_<Base, Holder> &class_, Extra&&... extra) const {
        typedef typename std::conditional<std::is_same<L, self_t>::value, Base, L>::type L_type;
        typedef typename std::conditional<std::is_same<R, self_t>::value, Base, R>::type R_type;
        typedef op_impl<id, ot, Base, L_type, R_type> op;
        class_.def(op::name(), &op::execute_cast, std::forward<Extra>(extra)...);
    }
};

#define PYBIND_BINARY_OPERATOR(id, rid, op, expr)                                      \
template <typename B, typename L, typename R> struct op_impl<op_##id, op_l, B, L, R> { \
    static char const* name() { return "__" #id "__"; }                                \
    static auto execute(const L &l, const R &r) -> decltype(expr) { return (expr); }   \
    static B execute_cast(const L &l, const R &r) { return B(expr); }                  \
};                                                                                     \
template <typename B, typename L, typename R> struct op_impl<op_##id, op_r, B, L, R> { \
    static char const* name() { return "__" #rid "__"; }                               \
    static auto execute(const R &r, const L &l) -> decltype(expr) { return (expr); }   \
    static B execute_cast(const R &r, const L &l) { return B(expr); }                  \
};                                                                                     \
inline op_<op_##id, op_l, self_t, self_t> op(const self_t &, const self_t &) {         \
    return op_<op_##id, op_l, self_t, self_t>();                                       \
};                                                                                     \
template <typename T> op_<op_##id, op_l, self_t, T> op(const self_t &, const T &) {    \
    return op_<op_##id, op_l, self_t, T>();                                            \
};                                                                                     \
template <typename T> op_<op_##id, op_r, T, self_t> op(const T &, const self_t &) {    \
    return op_<op_##id, op_r, T, self_t>();                                            \
};

#define PYBIND_INPLACE_OPERATOR(id, op, expr)                                          \
template <typename B, typename L, typename R> struct op_impl<op_##id, op_l, B, L, R> { \
    static char const* name() { return "__" #id "__"; }                                \
    static auto execute(L &l, const R &r) -> decltype(expr) { return expr; }           \
    static B execute_cast(L &l, const R &r) { return B(expr); }                        \
};                                                                                     \
template <typename T> op_<op_##id, op_l, self_t, T> op(const self_t &, const T &) {    \
    return op_<op_##id, op_l, self_t, T>();                                            \
};

#define PYBIND_UNARY_OPERATOR(id, op, expr)                                            \
template <typename B, typename L> struct op_impl<op_##id, op_u, B, L, undefined_t> {   \
    static char const* name() { return "__" #id "__"; }                                \
    static auto execute(const L &l) -> decltype(expr) { return expr; }                 \
    static B execute_cast(const L &l) { return B(expr); }                              \
};                                                                                     \
inline op_<op_##id, op_u, self_t, undefined_t> op(const self_t &) {                    \
    return op_<op_##id, op_u, self_t, undefined_t>();                                  \
};

PYBIND_BINARY_OPERATOR(sub,       rsub,         operator-,    l - r)
PYBIND_BINARY_OPERATOR(add,       radd,         operator+,    l + r)
PYBIND_BINARY_OPERATOR(mul,       rmul,         operator*,    l * r)
#if PY_MAJOR_VERSION >= 3
PYBIND_BINARY_OPERATOR(truediv,   rtruediv,     operator/,    l / r)
#else
PYBIND_BINARY_OPERATOR(div,       rdiv,         operator/,    l / r)
#endif
PYBIND_BINARY_OPERATOR(mod,       rmod,         operator%,    l % r)
PYBIND_BINARY_OPERATOR(lshift,    rlshift,      operator<<,   l << r)
PYBIND_BINARY_OPERATOR(rshift,    rrshift,      operator>>,   l >> r)
PYBIND_BINARY_OPERATOR(and,       rand,         operator&,    l & r)
PYBIND_BINARY_OPERATOR(xor,       rxor,         operator^,    l ^ r)
PYBIND_BINARY_OPERATOR(eq,        eq,           operator==,   l == r)
PYBIND_BINARY_OPERATOR(ne,        ne,           operator!=,   l != r)
PYBIND_BINARY_OPERATOR(or,        ror,          operator|,    l | r)
PYBIND_BINARY_OPERATOR(gt,        lt,           operator>,    l > r)
PYBIND_BINARY_OPERATOR(ge,        le,           operator>=,   l >= r)
PYBIND_BINARY_OPERATOR(lt,        gt,           operator<,    l < r)
PYBIND_BINARY_OPERATOR(le,        ge,           operator<=,   l <= r)
//PYBIND_BINARY_OPERATOR(pow,       rpow,         pow,          std::pow(l,  r))
PYBIND_INPLACE_OPERATOR(iadd,     operator+=,   l += r)
PYBIND_INPLACE_OPERATOR(isub,     operator-=,   l -= r)
PYBIND_INPLACE_OPERATOR(imul,     operator*=,   l *= r)
PYBIND_INPLACE_OPERATOR(idiv,     operator/=,   l /= r)
PYBIND_INPLACE_OPERATOR(imod,     operator%=,   l %= r)
PYBIND_INPLACE_OPERATOR(ilshift,  operator<<=,  l <<= r)
PYBIND_INPLACE_OPERATOR(irshift,  operator>>=,  l >>= r)
PYBIND_INPLACE_OPERATOR(iand,     operator&=,   l &= r)
PYBIND_INPLACE_OPERATOR(ixor,     operator^=,   l ^= r)
PYBIND_INPLACE_OPERATOR(ior,      operator|=,   l |= r)
PYBIND_UNARY_OPERATOR(neg,        operator-,    -l)
PYBIND_UNARY_OPERATOR(pos,        operator+,    +l)
PYBIND_UNARY_OPERATOR(abs,        abs,          std::abs(l))
PYBIND_UNARY_OPERATOR(invert,     operator~,    ~l)
PYBIND_UNARY_OPERATOR(bool,       operator!,    !!l)
PYBIND_UNARY_OPERATOR(int,        int_,         (int) l)
PYBIND_UNARY_OPERATOR(float,      float_,       (double) l)

#undef PYBIND_BINARY_OPERATOR
#undef PYBIND_INPLACE_OPERATOR
#undef PYBIND_UNARY_OPERATOR
NAMESPACE_END(detail)

using detail::self;

NAMESPACE_END(pybind)
