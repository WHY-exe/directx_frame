#pragma once

class Graphics;

class BufferResource {
public:
  virtual ~BufferResource() = default;
  virtual void BindAsBuffer(Graphics &) noexcept(!IS_DEBUG) = 0;
  virtual void Resize(Graphics &, unsigned int, unsigned int){};
  virtual void BindAsBuffer(Graphics &,
                            BufferResource *) noexcept(!IS_DEBUG) = 0;
  virtual void Clear(Graphics &) noexcept(!IS_DEBUG) = 0;
};
