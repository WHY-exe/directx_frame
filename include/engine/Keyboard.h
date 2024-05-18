#pragma once
#include <bitset>
#include <queue>

class Keyboard {
  friend class Window;

public:
  struct Event {
    enum class Status { PRESSED, RELEASED, INVAILD };
    Status m_keyStatus;
    unsigned int m_keyCode;
    Event(unsigned int keyCode, Status status) noexcept
        : m_keyStatus(status), m_keyCode(keyCode){};
    Event() noexcept : m_keyStatus(Status::INVAILD), m_keyCode(0){};
  };

public:
  Keyboard() = default;
  Keyboard(const Keyboard &) = delete;
  Keyboard &operator=(const Keyboard &) = delete;

public:
  void ClearState();
  Keyboard::Event ReadKey() noexcept;
  char ReadChar() noexcept;
  bool KeyIsPressed(unsigned char uKeyCode) const noexcept;
  void ClearQKeyCode() noexcept;
  void ClearQKeyChar() noexcept;
  bool AutoRepeatIsEnable() const noexcept;

private:
  void OnKeyDown(unsigned char uKeyCode) noexcept;
  void OnKeyUp(unsigned char uKeyCode) noexcept;
  void OnChar(char Char) noexcept;

private:
  static constexpr unsigned int m_uKeyCodeNum = 256u;
  static constexpr unsigned int m_MaxBufferSize = 16u;
  std::bitset<m_uKeyCodeNum> m_arrKeyState;
  std::queue<Event> m_qKeyBuffer;
  std::queue<char> m_qCharBuffer;
  bool m_bAutoRepeat = false;

private:
  template <typename T>
  static void BufferContorl(std::queue<T> &buffer) noexcept;
};
