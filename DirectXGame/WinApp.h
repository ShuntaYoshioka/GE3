#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp
{

public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
		WPARAM wparam, LPARAM lparam);

	void initialize();

	void Update();

	void finalize();

	//メッセージ処理
	bool ProcessMessage();

	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	//gettor
	HWND GetHwnd() { return hwnd; }

	HINSTANCE GetHinstance() {
		return wc.hInstance;
	}

private:
	HWND hwnd = nullptr;

	WNDCLASS wc{};
};

