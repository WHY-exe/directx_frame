#include "DxgiInfoManager.h"
#include "WinException.h"
#include <memory>
#pragma comment(lib, "dxguid.lib")
DxgiInfoManager::DxgiInfoManager() {
  INIT_WND_EXCEPT;
  // define the function signature of DXGIGetDebugInterface
  typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void **);
  // load the dll library to get the function
  const auto hModDxgiDebug = LoadLibraryExW(
      L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32_NO_FORWARDER);
  if (!hModDxgiDebug) {
    throw WND_LAST_EXCEPT();
  }
  // Get the address of the function
  const auto DxgiGetDebugInterface =
      reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void *>(
          GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
  if (!DxgiGetDebugInterface) {
    throw WND_LAST_EXCEPT();
  }

  // call the function, fill the info queue
  WND_CALL(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &m_pDxgiInfoQueue));
}

void DxgiInfoManager::Set() noexcept {
  // call set before get message, so you only get the error message
  // that generated after the set is called
  next = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const {
  INIT_WND_EXCEPT;
  std::vector<std::string> v_szMsg;
  const auto end = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
  for (auto i = next; i < end; i++) {
    SIZE_T ulMsgLen = 0;
    // get the meassage of index i
    WND_CALL(
        m_pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &ulMsgLen));
    // allocate the memory for the message
    auto bytes = std::make_unique<std::byte[]>(ulMsgLen);
    auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE *>(bytes.get());
    // fill the memory with message in the queue at index i
    WND_CALL(
        m_pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &ulMsgLen));
    v_szMsg.emplace_back(pMessage->pDescription);
  }
  return v_szMsg;
}
