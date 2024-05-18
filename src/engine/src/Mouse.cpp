#include "Mouse.h"

bool Mouse::IsEmpty() const noexcept { return m_qMouseEvent.empty(); }

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept {
  if (!m_qRawDelta.empty()) {
    RawDelta e = m_qRawDelta.front();
    m_qRawDelta.pop();
    return e;
  }
  return std::nullopt;
}

Mouse::Event::LMRStatus Mouse::lmrButtonStatus() const noexcept {
  return m_lrStatus;
}

const POINTS &Mouse::GetMousePos() const noexcept { return m_ptMouse; }

Mouse::Event Mouse::ReadEvent() noexcept {
  if (!m_qMouseEvent.empty()) {
    Event e = m_qMouseEvent.front();
    m_qMouseEvent.pop();
    return e;
  }
  return Event();
}

bool Mouse::IsInWindow() const noexcept { return m_bInWindow; }

int Mouse::GetWheelDelta() const noexcept { return m_wheelDeltaCarry; }

void Mouse::ClearEventQueue() noexcept { m_qMouseEvent = std::queue<Event>(); }

void Mouse::OnLButtonDown(LPARAM lParam) noexcept {
  m_ptMouse = MAKEPOINTS(lParam);
  m_qMouseEvent.push(Event(Event::Status::LPressed, m_ptMouse,
                           Event::LMRStatus(true, false, false)));
  m_lrStatus.m_LIsPressed = true;
  QueueControl();
}

void Mouse::OnLButtonUp(LPARAM lParam) noexcept {
  m_ptMouse = MAKEPOINTS(lParam);
  m_qMouseEvent.push(
      Event(Event::Status::LRelease, m_ptMouse, Event::LMRStatus()));
  m_lrStatus.m_LIsPressed = false;
  QueueControl();
}

void Mouse::OnMButtonDown(LPARAM lParam) noexcept {
  m_ptMouse = MAKEPOINTS(lParam);
  m_qMouseEvent.push(Event(Event::Status::MPressed, m_ptMouse,
                           Event::LMRStatus(false, true, false)));
  m_lrStatus.m_MIsPressed = true;
  QueueControl();
}

void Mouse::OnMButtonUp(LPARAM lParam) noexcept {
  m_ptMouse = MAKEPOINTS(lParam);
  m_qMouseEvent.push(
      Event(Event::Status::MRelease, m_ptMouse, Event::LMRStatus()));
  m_lrStatus.m_MIsPressed = false;
  QueueControl();
}

void Mouse::OnRButtonDown(LPARAM lParam) noexcept {
  m_ptMouse = MAKEPOINTS(lParam);
  m_qMouseEvent.push(Event(Event::Status::RPressed, m_ptMouse,
                           Event::LMRStatus(false, false, true)));
  m_lrStatus.m_RIsPressed = true;
  QueueControl();
}

void Mouse::OnRButtonUp(LPARAM lParam) noexcept {
  m_ptMouse = MAKEPOINTS(lParam);
  m_qMouseEvent.push(
      Event(Event::Status::RRelease, m_ptMouse, Event::LMRStatus()));
  m_lrStatus.m_RIsPressed = false;
  QueueControl();
}

void Mouse::OnRawDelta(int dx, int dy) noexcept {
  m_qRawDelta.push({dx, dy});
  QueueControl();
}

void Mouse::OnMouseMove(LPARAM lParam) noexcept {
  m_ptMouse = MAKEPOINTS(lParam);
  m_qMouseEvent.push(Event(Event::Status::Move, m_ptMouse, Event::LMRStatus()));
  QueueControl();
}

void Mouse::OnMouseEnter() noexcept {
  m_bInWindow = true;
  m_qMouseEvent.push(
      Event(Event::Status::Enter, m_ptMouse, Event::LMRStatus()));
  QueueControl();
}

void Mouse::OnMouseLeave() noexcept {
  m_bInWindow = false;
  m_qMouseEvent.push(
      Event(Event::Status::Leave, m_ptMouse, Event::LMRStatus()));
  QueueControl();
}

void Mouse::OnMouseWheelUp() noexcept {
  m_qMouseEvent.push(
      Event(Event::Status::WheelUp, m_ptMouse, Event::LMRStatus()));
  QueueControl();
}

void Mouse::OnMouseWheelDown() noexcept {
  m_qMouseEvent.push(
      Event(Event::Status::WheelDown, m_ptMouse, Event::LMRStatus()));
  QueueControl();
}

void Mouse::OnMouseWheel(WPARAM wParam) noexcept {
  int delta = GET_WHEEL_DELTA_WPARAM(wParam);
  m_wheelDeltaCarry += delta;
  if (m_wheelDeltaCarry >= WHEEL_DELTA) {
    OnMouseWheelUp();
  } else if (m_wheelDeltaCarry <= -WHEEL_DELTA) {
    OnMouseWheelDown();
  }
}

void Mouse::QueueControl() noexcept {
  while (m_qMouseEvent.size() > m_MaxQueueSize) {
    m_qMouseEvent.pop();
  }
  while (m_qRawDelta.size() > m_MaxQueueSize) {
    m_qRawDelta.pop();
  }
}
