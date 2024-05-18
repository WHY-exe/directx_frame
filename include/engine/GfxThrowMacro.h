#pragma once
#include "Graphics.h"
#ifndef NDEBUG
// 使用宏IMPORT_INFORMAN来使用Graphics类中的DxgiInfoManager
#define IMPORT_INFOMAN(gfx)\
    HRESULT hr = S_OK;\
    DxgiInfoManager& infoManager = GetInfoMan(gfx)
#define INIT_GFX_EXCEPTION\
    HRESULT hr = S_OK;
#define GFX_THORW(hrcall)\
	if(FAILED(hr = hrcall))\
		throw Graphics::GfxExcepion( __LINE__, __FILE__, hr, infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall)\
    if (FAILED(hr = hrcall))\
        infoManager.Set();\
     GFX_THORW(hr)
#define GFX_DEVICE_REMOVED_CALL(hrcall) throw Graphics::GfxExcepion( __LINE__, __FILE__, (hrcall), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call)\
    infoManager.Set();\
    (call);\
    {\
        auto v = infoManager.GetMessages();\
        if (!v.empty())\
        {\
            throw Graphics::GfxInfoOnlyException(__LINE__, __FILE__, v);\
        }\
    }
#else
#define IMPORT_INFOMAN(gfx)\
    HRESULT hr = S_OK;
#define INIT_GFX_EXCEPTION\
    HRESULT hr = S_OK;
#define GFX_THORW(hrcall) hrcall
#define GFX_THROW_INFO(hrcall) GFX_THORW(hrcall)
#define GFX_DEVICE_REMOVED_CALL(hrcall)
#define GFX_THROW_INFO_ONLY(call) call
#endif 