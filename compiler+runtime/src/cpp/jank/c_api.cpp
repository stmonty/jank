#include <jank/c_api.h>

using namespace jank;
using namespace jank::runtime;

template <typename Is>
struct make_closure_arity;

template <size_t I>
struct make_closure_arity_arg
{
  using type = object *;
};

template <size_t... Is>
struct make_closure_arity<std::index_sequence<Is...>>
{
  using type = object *(*)(void *, typename make_closure_arity_arg<Is>::type...);
};

template <>
struct make_closure_arity<std::index_sequence<>>
{
  using type = object *(*)(void *);
};

template <size_t N>
using closure_arity = typename make_closure_arity<std::make_index_sequence<N>>::type;

extern "C"
{
  jank_object_ptr jank_eval(jank_object_ptr const s)
  {
    auto const s_obj(try_object<obj::persistent_string>(reinterpret_cast<object *>(s)));
    return __rt_ctx->eval_string(s_obj->data);
  }

  jank_object_ptr jank_read_string(jank_object_ptr const s)
  {
    auto const s_obj(try_object<obj::persistent_string>(reinterpret_cast<object *>(s)));
    return __rt_ctx->read_string(s_obj->data);
  }

  jank_object_ptr jank_var_intern(jank_object_ptr const ns, jank_object_ptr const name)
  {
    auto const ns_obj(try_object<obj::persistent_string>(reinterpret_cast<object *>(ns)));
    auto const name_obj(try_object<obj::persistent_string>(reinterpret_cast<object *>(name)));
    return erase(__rt_ctx->intern_var(ns_obj->data, name_obj->data).expect_ok());
  }

  jank_object_ptr jank_var_bind_root(jank_object_ptr const var, jank_object_ptr const val)
  {
    auto const var_obj(try_object<runtime::var>(reinterpret_cast<object *>(var)));
    auto const val_obj(reinterpret_cast<object *>(val));
    return erase(var_obj->bind_root(val_obj));
  }

  jank_object_ptr jank_deref(jank_object_ptr const o)
  {
    auto const o_obj(reinterpret_cast<object *>(o));
    return behavior::deref(o_obj);
  }

  jank_object_ptr jank_call0(jank_object_ptr const f)
  {
    auto const f_obj(reinterpret_cast<object *>(f));
    return dynamic_call(f_obj);
  }

  jank_object_ptr jank_call1(jank_object_ptr const f, jank_object_ptr const a1)
  {
    auto const f_obj(reinterpret_cast<object *>(f));
    auto const a1_obj(reinterpret_cast<object *>(a1));
    return dynamic_call(f_obj, a1_obj);
  }

  jank_object_ptr
  jank_call2(jank_object_ptr const f, jank_object_ptr const a1, jank_object_ptr const a2)
  {
    auto const f_obj(reinterpret_cast<object *>(f));
    auto const a1_obj(reinterpret_cast<object *>(a1));
    auto const a2_obj(reinterpret_cast<object *>(a2));
    return dynamic_call(f_obj, a1_obj, a2_obj);
  }

  jank_object_ptr jank_call3(jank_object_ptr const f,
                             jank_object_ptr const a1,
                             jank_object_ptr const a2,
                             jank_object_ptr const a3)
  {
    auto const f_obj(reinterpret_cast<object *>(f));
    auto const a1_obj(reinterpret_cast<object *>(a1));
    auto const a2_obj(reinterpret_cast<object *>(a2));
    auto const a3_obj(reinterpret_cast<object *>(a3));
    return dynamic_call(f_obj, a1_obj, a2_obj, a3_obj);
  }

  jank_object_ptr jank_nil()
  {
    return erase(obj::nil::nil_const());
  }

  jank_object_ptr jank_true()
  {
    return erase(obj::boolean::true_const());
  }

  jank_object_ptr jank_false()
  {
    return erase(obj::boolean::false_const());
  }

  jank_object_ptr jank_integer_create(jank_native_integer const i)
  {
    return erase(make_box(i));
  }

  jank_object_ptr jank_real_create(jank_native_real const r)
  {
    return erase(make_box(r));
  }

  jank_object_ptr jank_string_create(char const *s)
  {
    assert(s);
    return erase(make_box(s));
  }

  jank_arity_flags jank_function_build_arity_flags(uint8_t const highest_fixed_arity,
                                                   jank_native_bool const is_variadic,
                                                   jank_native_bool const is_variadic_ambiguous)
  {
    return (is_variadic << 7) | (is_variadic_ambiguous << 6) | highest_fixed_arity;
  }

  jank_object_ptr jank_function_create(jank_arity_flags const arity_flags)
  {
    return erase(make_box<obj::jit_closure>(arity_flags));
  }

  void jank_function_set_arity0(jank_object_ptr const fn, jank_object_ptr (* const f)())
  {
  }

  void
  jank_function_set_arity1(jank_object_ptr const fn, jank_object_ptr (* const f)(jank_object_ptr))
  {
  }

  void jank_function_set_arity2(jank_object_ptr const fn,
                                jank_object_ptr (* const f)(jank_object_ptr, jank_object_ptr))
  {
  }

  void jank_function_set_arity3(jank_object_ptr const fn,
                                jank_object_ptr (* const f)(jank_object_ptr,
                                                            jank_object_ptr,
                                                            jank_object_ptr))
  {
  }

  jank_object_ptr jank_closure_create(jank_arity_flags const arity_flags, void * const context)
  {
    return erase(make_box<obj::jit_closure>(arity_flags, context));
  }

  void jank_closure_set_arity0(jank_object_ptr const fn, jank_object_ptr (* const f)())
  {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
    auto const fn_obj(reinterpret_cast<object *>(fn));
    try_object<obj::jit_closure>(fn_obj)->arity_0 = reinterpret_cast<closure_arity<0>>(f);
#pragma clang diagnostic pop
  }

  void
  jank_closure_set_arity1(jank_object_ptr const fn, jank_object_ptr (* const f)(jank_object_ptr))
  {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
    auto const fn_obj(reinterpret_cast<object *>(fn));
    try_object<obj::jit_closure>(fn_obj)->arity_1 = reinterpret_cast<closure_arity<1>>(f);
#pragma clang diagnostic pop
  }

  void jank_closure_set_arity2(jank_object_ptr const fn,
                               jank_object_ptr (* const f)(jank_object_ptr, jank_object_ptr))
  {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
    auto const fn_obj(reinterpret_cast<object *>(fn));
    try_object<obj::jit_closure>(fn_obj)->arity_2 = reinterpret_cast<closure_arity<2>>(f);
#pragma clang diagnostic pop
  }

  void jank_closure_set_arity3(jank_object_ptr const fn,
                               jank_object_ptr (* const f)(jank_object_ptr,
                                                           jank_object_ptr,
                                                           jank_object_ptr))
  {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
    auto const fn_obj(reinterpret_cast<object *>(fn));
    try_object<obj::jit_closure>(fn_obj)->arity_3 = reinterpret_cast<closure_arity<3>>(f);
#pragma clang diagnostic pop
  }

  jank_native_bool jank_truthy(jank_object_ptr const o)
  {
    auto const o_obj(reinterpret_cast<object *>(o));
    return static_cast<jank_native_bool>(truthy(o_obj));
  }

  jank_native_bool jank_equal(jank_object_ptr const l, jank_object_ptr const r)
  {
    auto const l_obj(reinterpret_cast<object *>(l));
    auto const r_obj(reinterpret_cast<object *>(r));
    return static_cast<jank_native_bool>(equal(l_obj, r_obj));
  }

  jank_native_hash jank_to_hash(jank_object_ptr const o)
  {
    auto const o_obj(reinterpret_cast<object *>(o));
    return to_hash(o_obj);
  }

  void jank_throw(jank_object_ptr o)
  {
    throw runtime::object_ptr{ reinterpret_cast<object *>(o) };
  }
}