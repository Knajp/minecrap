#pragma once
#include "Texture.h"
#include "structs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
	void Render(GLuint shaderProgram)
	{
		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, txt.ID);

		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);


		GLint transLoc = glGetUniformLocation(shaderProgram, "xTrans");
		glUniform1f(transLoc, 0);

		GLint yLoc = glGetUniformLocation(shaderProgram, "yTrans");
		float negative = fromTop * -0.2f;
		glUniform1f(yLoc, negative);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	}
private:
	glm::mat4 projection;
	Texture txt;
	GLuint VAO, VBO, EBO;
	GLint fromTop = 0;
};

class Menu
{
public:
	Menu(GLuint txt, float aspect)
		:texture(txt)
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
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL); // ERROR

		
	}
	void Loop(GLuint shaderProgram, GLFWwindow* window, Texture buttonTexture, float aspect)
	{

		Button newGame(buttonTexture, aspect);
		Button savedGame(buttonTexture, aspect, 1);
		Button exit(buttonTexture, aspect, 2);
		while (!glfwWindowShouldClose(window))
		{
			GLenum error = glGetError();
			glClear(GL_COLOR_BUFFER_BIT);
			
			Render(shaderProgram);
			newGame.Render(shaderProgram);
			savedGame.Render(shaderProgram);
			exit.Render(shaderProgram);
			glfwSwapBuffers(window); // Swap buffers
			glfwPollEvents();        // Process events

		}
	}
private:
	GLuint texture;
	GLuint VAO, VBO, EBO;
	glm::mat4 projection;


};

