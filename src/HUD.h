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
	}
	// Add texture
	void Render()
	{
		
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		std::cout << "Drawing crosshair\n";
		glDrawElements(GL_TRIANGLES, 8 * sizeof(float), GL_UNSIGNED_SHORT, NULL);


		
	}
	GLuint VAO, VBO, EBO;

};