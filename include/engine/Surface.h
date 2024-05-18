#pragma once
#include "InitWin.h"
#include <DirectXTex\DirectXTex.h>
#include <string>
#include <memory>
struct Color
{
	DWORD data;
	// special ctor
	constexpr Color() noexcept : data(0u) {};
	constexpr Color(const Color& color) : data(color.data) {};
	Color& operator=(const Color& color)
	{
		data = color.data;
		return *this;
	}
	// normal ctor
	constexpr Color(DWORD data) : data(data) {};
	constexpr Color(BYTE r, BYTE g, BYTE b, BYTE a) : data(a << 24 | r << 16 | g << 8 | b) {};
	constexpr Color(BYTE r, BYTE g, BYTE b) : data(255 << 25 | r << 16 | g << 8 | b) {};
	// Getter for color channel
	constexpr BYTE GetA() { return (data >> 24) & 0xFFu; };
	constexpr BYTE GetR() { return (data >> 16) & 0xFFu; };
	constexpr BYTE GetG() { return (data >> 8 ) & 0xFFu; };
	constexpr BYTE GetB() { return (data & 0xFFu); };
	// Setter for color channel
	constexpr void SetA(BYTE a) { data = (data & 0x00FFFFFF) | (a << 24); };
	constexpr void SetR(BYTE r) { data = (data & 0xFF00FFFF) | (r << 16); };
	constexpr void SetG(BYTE g) { data = (data & 0xFFFF00FF) | (g << 8); };
	constexpr void SetB(BYTE b) { data = (data & 0xFFFFFF00) | (b); };
};


class Surface
{
public:
	Surface(UINT width, UINT height);
	Surface(const std::string file_path);
	const BYTE* GetBufferPtr() const noexcept;
	void PutPixel(UINT col_idx, UINT row_idx, Color color) noexcept(!IS_DEBUG);
	Color GetPixel(UINT col_idx, UINT row_idx) const noexcept(!IS_DEBUG);
	size_t GetBytePitch() const noexcept(!IS_DEBUG);
	void SaveAsFile(const std::string& file_name) const noexcept(!IS_DEBUG);
	bool HasAlpha() const noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
private:
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	DirectX::ScratchImage m_scratchImage;
};
