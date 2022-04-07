#pragma once

#include <concepts>
#include <exception>

namespace fbtt {
   template <typename E>
   concept ErrorType = 
      std::derived_from<E, std::exception>;

   struct NoError : public std::exception { };

   template <typename E>
   concept OptionalError = 
      ErrorType<E> || std::same_as<E, NoError>;
};
