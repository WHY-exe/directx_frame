#pragma once
#include "RenderGraphCompileException.h"
#include <memory>
#include <string>

class Bindable;
class BufferResource;

namespace Rgph {
class Source {
public:
  const std::string &GetName() const noexcept;
  virtual void PostLinkValidate() const = 0;
  virtual std::shared_ptr<Bindable> YieldBindable();
  virtual std::shared_ptr<BufferResource> YieldBuffer();
  virtual ~Source() = default;

protected:
  Source(std::string name);

private:
  std::string m_name;
};

template <class T> class DirectBufferSource : public Source {
public:
  static std::unique_ptr<DirectBufferSource> Make(std::string name,
                                                  std::shared_ptr<T> &buffer) {
    return std::make_unique<DirectBufferSource>(std::move(name), buffer);
  }
  DirectBufferSource(std::string name, std::shared_ptr<T> &buffer)
      : Source(std::move(name)), m_buffer(buffer) {}

  void PostLinkValidate() const override {}
  std::shared_ptr<BufferResource> YieldBuffer() override {
    if (m_bLink) {
      throw RGC_EXCEPTION("Mutable output bound twice: " + GetName());
    }
    m_bLink = true;
    return m_buffer;
  }

private:
  std::shared_ptr<T> &m_buffer;
  bool m_bLink = false;
};

template <class T> class DirectBindableSource : public Source {
public:
  static std::unique_ptr<DirectBindableSource>
  Make(std::string name, std::shared_ptr<T> &buffer) {
    return std::make_unique<DirectBindableSource>(std::move(name), buffer);
  }
  DirectBindableSource(std::string name, std::shared_ptr<T> &bind)
      : Source(std::move(name)), m_bind(bind) {}
  void PostLinkValidate() const override {}
  std::shared_ptr<Bindable> YieldBindable() override { return m_bind; }

private:
  std::shared_ptr<T> &m_bind;
};
} // namespace Rgph