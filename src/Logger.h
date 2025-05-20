#pragma once
#include "Text.h"

enum LOG
{
	NO_PASS, OPENGL_ERROR, DEBUG
};

class Logger
{
public:
	Logger(TextRenderer* tRend)
		:m_tRend(tRend)
	{

	}

	void Log(LOG type);
	void Log(LOG type, int errorCode);
	void Log(LOG type, std::string log);

	void Display();
private:
	TextRenderer* m_tRend;
};