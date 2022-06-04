#include <iostream>

#include <jank/analyze/processor.hpp>
#include <jank/analyze/expr/list.hpp>

namespace jank::analyze
{
  context::context(runtime::context &ctx, std::string const &label, option<std::reference_wrapper<context>> p)
    : debug_label{ label }, parent{ p }, runtime_ctx{ ctx }
  { }

  expression processor::analyze_def(runtime::obj::list_ptr const &l)
  {
    auto const length(l->count());
    if(length != 3)
    {
      /* TODO: Error handling. */
      throw "invalid def";
    }

    auto const sym_obj(*l->data.rest().first());
    auto const sym(sym_obj->as_symbol());
    if(sym == nullptr)
    {
      /* TODO: Error handling. */
      throw "invalid def";
    }
    else if(!sym->ns.empty())
    {
      /* TODO: Error handling. */
      throw "invalid def";
    }

    auto const value(l->data.rest().rest().first()->get());
    if(value == nullptr)
    {
      /* TODO: Error handling. */
      throw "invalid def";
    }

    return { expr::def<expression>{ boost::static_pointer_cast<runtime::obj::symbol>(sym_obj), value } };
  }
  expression processor::analyze_symbol(runtime::obj::symbol_ptr const &sym)
  {
    if(!sym->ns.empty())
    {
      auto const ns(ctx.runtime_ctx.namespaces.find(runtime::obj::symbol::create(sym->ns)));
      if(ns == ctx.runtime_ctx.namespaces.end())
      {
        /* TODO: Error handling. */
        throw "unbound namespace for sym";
      }
      auto const var(ns->second->vars.find(sym));
      if(var == ns->second->vars.end())
      {
        /* TODO: Error handling. */
        throw "unbound sym";
      }

      return { expr::var_deref<expression>{ var->second } };
    }
    else
    {
      auto const &vars(ctx.runtime_ctx.current_ns->root->as_ns()->vars);
      auto const var(vars.find(sym));
      if(var == vars.end())
      {
        /* TODO: Error handling. */
        throw "unbound sym";
      }

      return { expr::var_deref<expression>{ var->second } };
    }

    return {};
  }
  expression processor::analyze_fn(runtime::obj::list_ptr const &)
  { return {}; }
  expression processor::analyze_let(runtime::obj::list_ptr const &)
  { return {}; }
  expression processor::analyze_if(runtime::obj::list_ptr const &)
  { return {}; }
  expression processor::analyze_list(runtime::obj::list_ptr const &o)
  {
    expr::list<expression> ret;
    ret.data = o;
    ret.exprs.reserve(o->count());

    for(auto const &e : o->data)
    /* TODO: Disable eval while analyzing these. */
    { ret.exprs.push_back(analyze(e)); }

    return {};
  }

  expression processor::analyze_call(runtime::obj::list_ptr const &o)
  {
    /* An empty list evaluates to a list, not a call. */
    if(o->count() == 0)
    { return analyze_list(o); }

    auto const first(o->seq()->first());
    if(auto const * const sym = first->as_symbol())
    {
      auto const found_special(specials.find(boost::static_pointer_cast<runtime::obj::symbol>(first)));
      if(found_special != specials.end())
      { return found_special->second(o); }

      // TODO: Normal fn call
    }
    else if(auto const * const callable = first->as_callable())
    {
      // TODO: Callable
    }

    /* TODO: Proper error handling. */
    assert(false);
  }

  processor::processor(runtime::context &rt_ctx)
    : ctx{ rt_ctx, "root", none }
  {
    using runtime::obj::symbol;
    auto const make_fn = [this](auto const fn) -> decltype(specials)::mapped_type
    {
      return [this, fn](auto const &list)
      { return (this->*fn)(list); };
    };
    specials =
    {
      { symbol::create("def"), make_fn(&processor::analyze_def) },
      { symbol::create("fn*"), make_fn(&processor::analyze_fn) },
      { symbol::create("let*"), make_fn(&processor::analyze_let) },
      { symbol::create("if"), make_fn(&processor::analyze_if) },
    };
  }

  expression processor::analyze(runtime::object_ptr const &o)
  {
    assert(o);

    if(o->as_list())
    { return analyze_call(boost::static_pointer_cast<runtime::obj::list>(o)); }
    else if(auto * const vector = o->as_vector())
    {
    }
    else if(auto * const map = o->as_map())
    {
    }
    else if(auto * const set = o->as_set())
    {
    }
    else if(auto * const number = o->as_number())
    {
    }
    else if(auto * const string = o->as_string())
    {
    }
    else if(o->as_symbol())
    { return analyze_symbol(boost::static_pointer_cast<runtime::obj::symbol>(o)); }
    else if(auto * const nil = o->as_nil())
    {
    }
    else
    {
      std::cout << "unsupported analysis of " << o->to_string() << std::endl;
      assert(false);
    }

    return {};
  }
}
