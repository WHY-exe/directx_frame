#include "Surface.h"
#include "InitWin.h"
#include "StrManager.h"
#include "WinException.h"
#include <DirectXTex\DirectXTex.h>
#include <sstream>

Surface::Surface(UINT width, UINT height) {
  INIT_WND_EXCEPT;
  WND_CALL(m_scratchImage.Initialize2D(format, width, height, 1u, 1u));
}

Surface::Surface(const std::string file_path) {
  INIT_WND_EXCEPT;
  const std::string img_format = SplitString(file_path, ".")[1];
  if (img_format == "tga") {
    WND_CALL(DirectX::LoadFromTGAFile(ANSI_TO_UTF8_STR(file_path).c_str(),
                                      nullptr, m_scratchImage));
  } else {
    WND_CALL(DirectX::LoadFromWICFile(ANSI_TO_UTF8_STR(file_path).c_str(),
                                      DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr,
                                      m_scratchImage));
  }
  if (m_scratchImage.GetImage(0, 0, 0)->format != format) {
    DirectX::ScratchImage convert;
    WND_CALL(DirectX::Convert(*m_scratchImage.GetImage(0, 0, 0), format,
                              DirectX::TEX_FILTER_DEFAULT,
                              DirectX::TEX_THRESHOLD_DEFAULT, convert));
    m_scratchImage = std::move(convert);
  }
}

const BYTE *Surface::GetBufferPtr() const noexcept {
  return m_scratchImage.GetPixels();
}

void Surface::PutPixel(UINT col_idx, UINT row_idx,
                       Color color) noexcept(!IS_DEBUG) {
  assert(row_idx >= 0);
  assert(col_idx >= 0);
  assert(row_idx <= GetHeight());
  assert(col_idx <= GetWidth());
  auto pixel_data = m_scratchImage.GetImage(0, 0, 0);
  reinterpret_cast<Color *>(
      &pixel_data->pixels[row_idx * pixel_data->rowPitch])[col_idx] = color;
}

Color Surface::GetPixel(UINT col_idx, UINT row_idx) const noexcept(!IS_DEBUG) {
  assert(row_idx >= 0);
  assert(col_idx >= 0);
  assert(row_idx <= GetHeight());
  assert(col_idx <= GetWidth());
  auto pixel_data = m_scratchImage.GetImage(0, 0, 0);
  return reinterpret_cast<Color *>(
      &pixel_data->pixels[row_idx * pixel_data->rowPitch])[col_idx];
}

size_t Surface::GetBytePitch() const noexcept(!IS_DEBUG) {
  return m_scratchImage.GetImage(0, 0, 0)->rowPitch;
}

void Surface::SaveAsFile(const std::string &file_name) const
    noexcept(!IS_DEBUG) {
  INIT_WND_EXCEPT;
  const auto GetWICID = [](const std::string &file_name) {
    const std::string img_format = SplitString(file_name, ".")[1];
    if (img_format == "png") {
      return DirectX::WIC_CODEC_PNG;
    }
    if (img_format == "jpg") {
      return DirectX::WIC_CODEC_JPEG;
    }
    if (img_format == "bmp") {
      return DirectX::WIC_CODEC_BMP;
    }
    throw Exception::Exception(__LINE__, __FILE__,
                               "Unsupported image format type");
  };
  WND_CALL(DirectX::SaveToWICFile(*m_scratchImage.GetImage(0, 0, 0),
                                  DirectX::WIC_FLAGS_NONE,
                                  GetWICCodec(GetWICID(file_name)),
                                  ANSI_TO_UTF8_STR(file_name.c_str()).c_str()));
}

bool Surface::HasAlpha() const noexcept {
  return !m_scratchImage.IsAlphaAllOpaque();
}

UINT Surface::GetWidth() const noexcept {
  return (UINT)m_scratchImage.GetMetadata().width;
}

UINT Surface::GetHeight() const noexcept {
  return (UINT)m_scratchImage.GetMetadata().height;
}