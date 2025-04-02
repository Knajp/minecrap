#pragma once
#include "structs.h"
#include "Texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Crosshair
{
public:
	Crosshair()
	{
		HUDvertex vertices[] = {
			{-0.05f, -0.05f, 0.0f, 0.0f},
			{0.05f, -0.05f, 1.0f, 0.0f},
			{-0.05f, 0.05f, 0.0f, 1.0f},
			{0.05f, 0.05f, 1.0f, 1.0f}
		};
		GLushort indices[] = {
			3, 1, 0,
			0, 2, 3,
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
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	}
	// Add texture
	void Render(GLuint shaderProgram)
	{
		glUseProgram(shaderProgram);

		// Set projection matrix uniform
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 8 * sizeof(float), GL_UNSIGNED_SHORT, NULL);

		
	}
	GLuint VAO, VBO, EBO;
	glm::mat4 projection;
};
class Hotbar {
public:
	Hotbar()
	{
		HUDvertex vertices[] = {
			{-0.65f, -1.0f, 0.0f, 0.0f},
			{0.65f, -1.0f, 1.0f, 0.0f},
			{-0.65f, -0.85f, 0.0f, 1.0f},
			{0.65f, -0.85f, 1.0f, 1.0f}
		};
		HUDvertex sVertices[] = {
			{-0.65f, -1.0f, 0.0f, 0.0f},
			{-0.51f, -1.0f, 1.0f, 0.0f},
			{-0.65f, -0.85f, 0.0f, 1.0f},
			{-0.51f, -0.85f, 1.0f, 1.0f}
		};

		GLushort indices[] = {
			3, 1, 0,
			0, 2, 3,
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
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		glGenVertexArrays(1, &sVAO);
		glBindVertexArray(sVAO);

		glGenBuffers(1, &sVBO);
		glGenBuffers(1, &sEBO);

		glBindBuffer(GL_ARRAY_BUFFER, sVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sVertices), sVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HUDvertex), (void*)(2 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(sVAO);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	}
	void Render(GLuint shaderProgram, float xTrans)
	{
		glUseProgram(shaderProgram);

		// Set projection matrix uniform
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

		GLint transLoc = glGetUniformLocation(shaderProgram, "xTrans");

		glUniform1f(transLoc, xTrans * 0.145f);

		glBindTexture(GL_TEXTURE_2D, selectTexture.ID);

		glBindVertexArray(sVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sEBO);

		glDrawElements(GL_TRIANGLES, 8 * sizeof(float), GL_UNSIGNED_SHORT, NULL);

		glUniform1f(transLoc, 0.0f);

		glBindTexture(GL_TEXTURE_2D, hotbarTexture.ID);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 8 * sizeof(float), GL_UNSIGNED_SHORT, NULL);

		
	}
private:
	GLuint VAO, VBO, EBO;
	GLuint sVAO, sVBO, sEBO;
	Texture selectTexture = Texture("src/texture/selection.png");
	Texture hotbarTexture = Texture("src/texture/hotbar.png");
	glm::mat4 projection;
};