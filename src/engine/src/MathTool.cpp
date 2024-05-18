#include "MathTool.h"
#include <cmath>
namespace math_tool {
int gcd(int a, int b) {
  int r;
  r = a % b;
  while (r != 0) {
    a = b;
    b = r;
    r = a % b;
  }
  return b;
}
// usually you would only use it for extracting rotation from id matrix
DirectX::XMFLOAT3 ExtraEulerAngle(const DirectX::XMFLOAT4X4 matrix) {
  DirectX::XMFLOAT3 result;
  result.x = asinf(-matrix._32);
  if (sinf(result.x) > 0.99f) {
    result.y = 0.0f;
    result.z = atan2f(matrix._21, matrix._11);

  } else if (sinf(result.x) < -0.99f) {
    result.y = 0.0f;
    result.z = atan2f(-matrix._21, matrix._11);
  } else {
    result.y = atan2f(matrix._31, matrix._33);
    result.z = atan2f(matrix._12, matrix._22);
  }
  return result;
}
DirectX::XMFLOAT3 ExtraTranslation(const DirectX::XMFLOAT4X4 matrix) {
  return {matrix._41, matrix._42, matrix._43};
}
DirectX::XMFLOAT3 ExtraScaling(const DirectX::XMFLOAT4X4 matrix) {
  return {
      math_tool::sqrt(matrix._11 * matrix._11 + matrix._21 * matrix._21 +
                      matrix._31 * matrix._31),
      math_tool::sqrt(matrix._12 * matrix._12 + matrix._22 * matrix._22 +
                      matrix._32 * matrix._32),
      math_tool::sqrt(matrix._13 * matrix._13 + matrix._23 * matrix._23 +
                      matrix._33 * matrix._33),
  };
}
} // namespace math_tool