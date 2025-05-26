#pragma once
#include "Text.h"
#include <vector>
#include <string>
#include <sstream>
#include <glfw/glfw3.h>
enum LOG
{
	NO_PASSX, NO_PASSZ, OPENGL_ERROR, DEBUG, FALLING, BLOCKABOVE, BLOCKBELOW, XINCHUNK, ZINCHUNK
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
			if (l.type == type) return;
		}
		Log newlog = {type, (unsigned short)logs.size(), "new log"};
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
		case NO_PASSX: return "NO_PASSX";
		case NO_PASSZ: return "NO_PASSZ";
		case OPENGL_ERROR: return "OPENGL_ERROR";
		case DEBUG: return "DEBUG";
		case FALLING: return "FALLING";
		case BLOCKABOVE: return "BLOCKABOVE";
		case BLOCKBELOW: return "BLOCKBELOW";
		case XINCHUNK: return "CHUNK_X";
		case ZINCHUNK: return "CHUNK_Z";
		default: return "Unknown";
		}
	}


	void Display(GLuint shader)
	{
		unsigned int i = 0;
		for (auto& l : logs)
		{
			m_tRend->RenderText(shader, l.contents, -1.5f, (-0.7f + i * 0.1f), 0.0012f, {1.0f, 1.0f, 1.0f});
			i++;
			
		}
	}

	
private:
	TextRenderer* m_tRend;
	std::vector<Log> logs;
};