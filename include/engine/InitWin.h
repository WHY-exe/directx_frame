#pragma once
// include this file before include somthing include Windows.h
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>
// trun off windows switch that create by Windows.h
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMERTRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMERTICS
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOAPE

#define STRICT

#include <Windows.h>