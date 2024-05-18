#pragma once
#include <DirectXMath.h>
#include <cmath>
#define PI (float)3.14159265f
#define PI_D (double)3.1415926535897932

namespace math_tool {
template <typename T> T wrap_angle(T theta) noexcept {
  constexpr T twoPi = (T)2.0 * (T)PI_D;
  const T mod = (T)fmod(theta, twoPi);
  if (mod > (T)PI_D) {
    return mod - twoPi;
  } else if (mod < -(T)PI_D) {
    return mod + twoPi;
  }
  return mod;
};
template <typename T> T square(T x) noexcept { return x * x; };
template <typename T> T gauss(T x, T sigma) noexcept {
  // ��������Ϊ0
  return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * square(sigma))) *
         exp(-square(x) / ((T)2.0 * square(sigma)));
};
inline float rsqrt(float num) {
  long i;
  float x2, y;
  x2 = num / 2;
  y = num;
  i = *(long *)&y;
  // that's why we call Carmack a genius
  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (1.5f - x2 * y * y);
  y = y * (1.5f - x2 * y * y);
  y = y * (1.5f - x2 * y * y);
  return y;
};
inline float sqrt(float num) { return 1.0f / rsqrt(num); };
int gcd(int a, int b);
DirectX::XMFLOAT3 ExtraEulerAngle(const DirectX::XMFLOAT4X4 matrix);
DirectX::XMFLOAT3 ExtraTranslation(const DirectX::XMFLOAT4X4 matrix);
DirectX::XMFLOAT3 ExtraScaling(const DirectX::XMFLOAT4X4 matrix);
} // namespace math_tool