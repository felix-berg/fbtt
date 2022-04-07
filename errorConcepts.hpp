#pragma once

#include <concepts>
#include <exception>

template <typename E>
concept ErrorType = std::derived_from<E, std::exception>;

template <typename E>
concept OptionalError = 
   ErrorType<E> || std::same_as<E, void>;
