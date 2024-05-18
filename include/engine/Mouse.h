#pragma once
#include "InitWin.h"
#include <optional>
#include <queue>
class Mouse
{
	friend class Window;
public:
	struct RawDelta 
	{
		int x, y;
	};
	struct Event
	{
		struct LMRStatus
		{
			bool m_LIsPressed = false;
			bool m_MIsPressed = false;
			bool m_RIsPressed = false;
			LMRStatus() = default;
			LMRStatus(bool LIsPressed, bool MISPressed,bool RIsPressed)
				:
				m_LIsPressed(LIsPressed),
				m_MIsPressed(MISPressed),
				m_RIsPressed(RIsPressed)
			{};
			LMRStatus(bool LIsPressed, bool RIsPressed)
				:
				m_LIsPressed(LIsPressed),
				m_RIsPressed(RIsPressed)
			{};
		};
		enum class Status
		{
			LPressed,
			LRelease,
			MPressed,
			MRelease,
			RPressed,
			RRelease,
			WheelUp,
			WheelDown,
			Enter,
			Leave,
			Move,
			Invalid
		};
		Status m_status;
		POINTS m_ptMouse;
		LMRStatus m_lrStatus;
		
		Event()
			:
			m_status(Status::Invalid),
			m_ptMouse({ 0, 0 })
		{};
		Event(Status status, POINTS pt, LMRStatus lrStatus)
			:
			m_status(status),
			m_ptMouse(pt),
			m_lrStatus(lrStatus)
		{};
	};
public:
	bool IsEmpty() const noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;
	Event::LMRStatus lmrButtonStatus() const noexcept;
	const POINTS& GetMousePos() const noexcept;
	Mouse::Event ReadEvent() noexcept;
	bool IsInWindow() const noexcept;
	int GetWheelDelta() const noexcept;
public:
	void ClearEventQueue() noexcept;
private:
	void OnLButtonDown(LPARAM lParam) noexcept;
	void OnLButtonUp(LPARAM lParam) noexcept;
	void OnMButtonDown(LPARAM lParam) noexcept;
	void OnMButtonUp(LPARAM lParam) noexcept;
	void OnRButtonDown(LPARAM lParam) noexcept;
	void OnRButtonUp(LPARAM lParam) noexcept;
	void OnRawDelta(int dx, int dy) noexcept;
	void OnMouseMove(LPARAM lParam) noexcept;
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseWheelUp() noexcept;
	void OnMouseWheelDown() noexcept;
	void OnMouseWheel(WPARAM wParam) noexcept;
	void QueueControl() noexcept;
private:
	static constexpr int m_MaxQueueSize = 16;
	int m_wheelDeltaCarry = 0;
	POINTS m_ptMouse;
	Event::LMRStatus m_lrStatus;
	bool m_bInWindow;
	std::queue<Event> m_qMouseEvent;
	std::queue<RawDelta> m_qRawDelta;
};

