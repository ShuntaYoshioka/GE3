#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	//DorectINPUTインスタンス生成
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput,
		nullptr
	);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput->CreateDevice(
		GUID_SysKeyboard,
		&keyboard,
		NULL
	);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御の設定
	result = keyboard->SetCooperativeLevel(
		hwnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));

};
void Input::Update() {
	//キーボードの状態取得
	keyboard->Acquire();

	//キーボード入力保存用
	BYTE key[256] = {};
	keyboard->GetDeviceState(sizeof(key), key);


};

