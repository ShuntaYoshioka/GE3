#include "Logger.h"

namespace Logger {
	//ロガー
	void Log(const std::string& message) {
		OutputDebugStringA(message.c_str());
	}

}