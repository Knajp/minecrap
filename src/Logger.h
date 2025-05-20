#pragma once
#include "Text.h"
#include <vector>
#include <string>
#include <sstream>
#include <glfw/glfw3.h>
enum LOG
{
	NO_PASS, OPENGL_ERROR, DEBUG
};

struct Log
{
	void operator=(std::string s)
	{
		contents = s;
	}
	LOG type;
	unsigned short int index;
	std::string contents;
};

class Logger
{
public:
	Logger() = default;
	Logger(TextRenderer* tRend)
		:m_tRend(tRend)
	{

	}

	void createLog(LOG type)
	{
		for (auto& l : logs)
		{
			if (l.type = type) return;
		}
		Log newlog = {type, logs.size(), "new log"};
		logs.push_back(newlog);
	}

	template <typename T>
	void updateLog(LOG type, T content)
	{
		std::stringstream ss;
		ss << logToString(type) << " | " << glfwGetTime() << " - " << content;
		for (auto& l : logs)
		{
			if (l.type == type) l = ss.str();
		}
	}
	
	const char* logToString(LOG l) {
		switch (l) {
		case NO_PASS: return "NO_PASS";
		case OPENGL_ERROR: return "OPENGL_ERROR";
		case DEBUG: return "DEBUG";
		default: return "Unknown";
		}
	}


	void Display(GLuint shader)
	{
		unsigned int i = 0;
		for (auto& l : logs)
		{
			m_tRend->RenderText(shader, l.contents, -1.5f, (-0.7f + i * 0.1f), 0.001f, {1.0f, 1.0f, 1.0f});
			i++;
		}
		
	}

	
private:
	TextRenderer* m_tRend;
	std::vector<Log> logs;
};