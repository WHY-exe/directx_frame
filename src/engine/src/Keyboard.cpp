#include "Keyboard.h"

void Keyboard::ClearState() { m_arrKeyState.reset(); }

Keyboard::Event Keyboard::ReadKey() noexcept {
  if (!m_qKeyBuffer.empty()) {
    Keyboard::Event e = m_qKeyBuffer.front();
    m_qKeyBuffer.pop();
    return e;
  }
  return Keyboard::Event();
}

char Keyboard::ReadChar() noexcept {
  if (!m_qCharBuffer.empty()) {
    char character = m_qCharBuffer.front();
    m_qCharBuffer.pop();
    return character;
  }
  return 0;
}

bool Keyboard::KeyIsPressed(unsigned char uKeyCode) const noexcept {
  return m_arrKeyState[uKeyCode];
}

void Keyboard::OnKeyDown(unsigned char uKeyCode) noexcept {
  m_arrKeyState[uKeyCode] = true;
  m_qKeyBuffer.push(Event(uKeyCode, Event::Status::PRESSED));
  BufferContorl(m_qKeyBuffer);
}

void Keyboard::OnKeyUp(unsigned char uKeyCode) noexcept {
  m_arrKeyState[uKeyCode] = false;
  m_qKeyBuffer.push(Event(uKeyCode, Event::Status::RELEASED));
  BufferContorl(m_qKeyBuffer);
}
void Keyboard::OnChar(char Char) noexcept {
  m_qCharBuffer.push(Char);
  BufferContorl(m_qCharBuffer);
}
bool Keyboard::AutoRepeatIsEnable() const noexcept { return m_bAutoRepeat; }
void Keyboard::ClearQKeyCode() noexcept { m_qKeyBuffer = std::queue<Event>(); }
void Keyboard::ClearQKeyChar() noexcept { m_qCharBuffer = std::queue<char>(); }
template <typename T>
void Keyboard::BufferContorl(std::queue<T> &buffer) noexcept {
  while (buffer.size() > m_MaxBufferSize) {
    buffer.pop();
  }
}
