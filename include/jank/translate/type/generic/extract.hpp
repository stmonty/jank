#pragma once

#include <experimental/optional>

#include <jank/parse/cell/cell.hpp>

namespace jank
{
  namespace translate
  {
    namespace type
    {
      namespace generic
      {
        /* Tries to find a generic specialization. */
        std::tuple
        <
          std::experimental::optional<parse::cell::list>,
          parse::cell::list::type::const_iterator
        > extract
        (
          parse::cell::list::type::const_iterator const begin,
          parse::cell::list::type::const_iterator const end
        );
      }
    }
  }
}
