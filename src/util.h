#pragma once

template<typename T>
inline constexpr T clamp_value(T __val, T __lo, T __hi) 
{
      return (__val < __lo) ? __lo : (__hi < __val) ? __hi : __val;
}