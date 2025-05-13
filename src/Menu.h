#pragma once
#include "Texture.h"
#include "structs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

namespace fs = std::filesystem;
class Button
{
public:
	Button(Texture texture, float aspect, int i_fromTop = 0)
		:fromTop(i_fromTop), txt(texture)
	{
		HUDvertex vertices[] =
		{
			{-0.5f, 0.05f, 0.0f, 0.0f},
			{0.5f, 0.05f, 1.0f, 0.0f},
			{-0.5f, 0.2f, 0.0f, 1.0f},
			{0.5f, 0.2f, 1.0f, 1.0f}
		};
		GLushort indices[] =
		{
			0,1,2,
			2,1,3
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
	}
	Button(Texture texture, float aspect, int i_fromTop, float xTrans)
		:fromTop(i_fromTop), txt(texture), transX(xTrans)
	{
		HUDvertex vertices[] =
		{
			{-0.5f, 0.05f, 0.0f, 0.0f},
			{0.5f, 0.05f, 1.0f, 0.0f},
			{-0.5f, 0.2f, 0.0f, 1.0f},
			{0.5f, 0.2f, 1.0f, 1.0f}
		};
		GLushort indices[] =
		{
			0,1,2,
			2,1,3
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
	}
	void Render(GLuint shaderProgram)
	{
		if (!visible) return;
		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, txt.ID);

		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);


		GLint transLoc = glGetUniformLocation(shaderProgram, "xTrans");
		glUniform1f(transLoc, transX);

		GLint yLoc = glGetUniformLocation(shaderProgram, "yTrans");
		float negative = fromTop * -0.2f;
		glUniform1f(yLoc, negative);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	}
	bool visible = true;
private:
	glm::mat4 projection;
	Texture txt;
	GLuint VAO, VBO, EBO;
	GLint fromTop = 0;
	GLfloat transX = 0;
	
};

class WorldSave
{
public:
	WorldSave(GLuint text, float aspect, GLushort iindex, std::string name)
		:txt(text), index(iindex), worldName(name)
	{
		HUDvertex vertices[] =
		{
			{-0.9f, 0.5f, 0.0f, 0.0f},
			{0.9f, 0.5f, 1.0f, 0.0f},
			{-0.9f, 0.8f, 0.0f, 1.0f},
			{0.9f, 0.8f, 1.0f, 1.0f}
		};
		GLushort indices[] =
		{
			0,1,2,
			2,1,3
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
	}
	void Render(GLuint shaderProgram)
	{
		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, txt);

		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);


		GLint transLoc = glGetUniformLocation(shaderProgram, "xTrans");
		glUniform1f(transLoc, 0);

		GLint yLoc = glGetUniformLocation(shaderProgram, "yTrans");
		float negative = index * -0.35f;
		glUniform1f(yLoc, negative);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	}
	unsigned short int index;
	std::string worldName;
private:
	
	GLuint txt;
	glm::mat4 projection;
	GLuint VAO, VBO, EBO;
};

class WorldManager
{
public:
	WorldManager(GLuint text, float aspect)
		:texture(text)
	{
		HUDvertex vertices[] =
		{
			{-1.0f, -0.5f, 0.0f, 0.0f},
			{1.0f, -0.5f, 1.0f, 0.0f},
			{-1.0f, 0.9f, 0.0f, 1.0f},
			{1.0f, 0.9f, 1.0f, 1.0f}
		};
		GLushort indices[] =
		{
			0,1,2,
			2,1,3
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
	}
	void LoadWorlds(GLuint worldTXT, float aspect)
	{
		fs::path dir = "saves";
		if (fs::exists(dir) && fs::is_directory(dir)) {
			// Iterate over the files in the directory
			for (const auto& entry : fs::directory_iterator(dir)) {
				if (fs::is_regular_file(entry.status())) {  // Ensure it's a file
					// Get the filename without path and extension
					std::string filename_without_extension = entry.path().stem().string();
					files.push_back(filename_without_extension);  // Add it to the vector
				}
			}

			
		}
		else {
			std::cerr << "The directory does not exist or is not a valid directory." << std::endl;
		}

		int i = 0;
		for (const auto& filename : files) {
			WorldSave world(worldTXT, aspect, i, filename);
			worlds.push_back(world);
			i++;
		}
	}
	void Render(GLuint shaderProgram)
	{
		if (!visible) return;
		glUseProgram(shaderProgram);

		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set projection matrix uniform
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);


		GLint transLoc = glGetUniformLocation(shaderProgram, "xTrans");
		glUniform1f(transLoc, 0);

		GLint yLoc = glGetUniformLocation(shaderProgram, "yTrans");
		glUniform1f(yLoc, 0);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

		for (auto& world : worlds)
		{
			world.Render(shaderProgram);
		}
	}
	bool visible = false;
	std::string selectedWorld = "";
	std::vector<WorldSave> worlds;
private:
	
	std::vector<std::string> files;
	GLuint VAO, VBO, EBO;
	glm::mat4 projection;
	GLuint texture;
};

class Menu
{
public:
	Menu(GLuint txt, GLuint txt2, float aspect)
		:texture(txt), texture2(txt2), activeTexture(txt)
	{
		std::cout << "main menu\n";
		HUDvertex menuVertices[4] =
		{
			{-aspect, -1.0f, 0.0f, 0.0f},
			{aspect, -1.0f, 1.0f, 0.0f},
			{-aspect, 1.0f, 0.0f, 1.0f},
			{aspect, 1.0f, 1.0f, 1.0f}
		};
		GLushort indices[6] =
		{
			0,1,2,
			2,3,1
		};
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(menuVertices), menuVertices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		
		
		projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
		GLint maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
		std::cout << "Max texture size: " << maxTextureSize << std::endl;

	}
	void Render(GLuint shaderProgram)
	{
		
		glUseProgram(shaderProgram);

		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, activeTexture);
		// Set projection matrix uniform
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
		

		GLint transLoc = glGetUniformLocation(shaderProgram, "xTrans");
		glUniform1f(transLoc, 0);
		
		GLint yLoc = glGetUniformLocation(shaderProgram, "yTrans");
		glUniform1f(yLoc, 0);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL); // ERROR

		
	}
	std::string Loop(GLuint shaderProgram, GLFWwindow* window, Texture buttonTexture, Texture wmantext, Texture wsavetxt, const GLFWvidmode* mode)
	{

		float aspect = (float)mode->width / (float)mode->height;

		Button newGame(buttonTexture, aspect);
		Button savedGame(buttonTexture, aspect, 1);
		Button exit(buttonTexture, aspect, 2);
		Button back(buttonTexture, aspect, 4, -0.6f);
		back.visible = false;
		Button enter(buttonTexture, aspect, 4, 0.6f);
		enter.visible = false;
		WorldManager wman(wmantext.ID, aspect);
		while (!glfwWindowShouldClose(window))
		{
			GLenum error = glGetError();
			glClear(GL_COLOR_BUFFER_BIT);
			
			Render(shaderProgram);
			newGame.Render(shaderProgram);
			savedGame.Render(shaderProgram);
			exit.Render(shaderProgram);
			back.Render(shaderProgram);
			enter.Render(shaderProgram);
			wman.Render(shaderProgram);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			{
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				float x_ndc = (2.0f * (float)xpos) / mode->width - 1.0f;
				float y_ndc = 1.0f - (2.0f * (float)ypos) / mode->height;
				if (x_ndc >= -0.5f && x_ndc <= 0.5f)
				{
					if (y_ndc >= 0.05f && y_ndc <= 0.2f && newGame.visible)
					{
						newGame.visible = false;
						savedGame.visible = false;
						exit.visible = false;
						activeTexture = texture2;
						back.visible = true;
						enter.visible = true;
						wman.visible = true;
					}

					else if (y_ndc >= -0.15f && y_ndc <= 0.0f && savedGame.visible)
					{
						wman.LoadWorlds(wsavetxt.ID,aspect);
						newGame.visible = false;
						savedGame.visible = false;
						exit.visible = false;
						activeTexture = texture2;
						back.visible = true;
						enter.visible = true;
						wman.visible = true;
					}

					else if (y_ndc >= -0.35f && y_ndc <= -0.2f && exit.visible)
					{
						glfwSetWindowShouldClose(window, true);
					}
				}
				if (y_ndc >= -0.75f && y_ndc <= -0.6f)
				{
					if (x_ndc >= -1.10f && x_ndc <= -0.1f && back.visible)
					{
						newGame.visible = true;
						savedGame.visible = true;
						exit.visible = true;
						activeTexture = texture;
						back.visible = false;
						enter.visible = false;
						wman.visible = false;

					}
					if (x_ndc >= 0.1f && x_ndc <= 1.1f && enter.visible && wman.selectedWorld != "")
					{
						back.visible = false;
						enter.visible = false;
						wman.visible = false;
						return wman.selectedWorld;
					}
				}
				if (wman.visible && x_ndc >= -0.9f && x_ndc <= 0.9f)
				{
					if (y_ndc >= 0.5f && y_ndc < 0.8f && wman.worlds.size() > 0)
					{
						wman.selectedWorld = wman.worlds[0].worldName;
					}
					else if (y_ndc >= 0.15f && y_ndc < 0.45f && wman.worlds.size() > 1)
					{
						wman.selectedWorld = wman.worlds[1].worldName;
					}
					else if (y_ndc >= -0.2f && y_ndc < 0.1f && wman.worlds.size() > 2)
					{
						wman.selectedWorld = wman.worlds[2].worldName;
					}
				}
			}

			return "";

			glfwSwapBuffers(window); // Swap buffers
			glfwPollEvents();        // Process events

		}
	}
private:
	GLuint activeTexture;
	GLuint texture;
	GLuint texture2;
	GLuint VAO, VBO, EBO;
	glm::mat4 projection;


};

