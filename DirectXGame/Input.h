#pragma once
#include <Windows.h>
#include <wrl.h>

#define DERECTINPUT_VERSION 0x0800
#include <dinput.h>

class Input
{
public:

	//namespace	省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	void Initialize(HINSTANCE hInstance, HWND hwnd);
	void Update();

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);

private:
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput;

	//キーボード入力保存用
	BYTE key[256] = {};
	BYTE preKey[256] = {};

};

