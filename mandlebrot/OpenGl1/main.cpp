#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <algorithm>

#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"


float vertices[] =
{
	//    x      y      z   
		-1.0f, -1.0f, -0.0f,
		 1.0f,  1.0f, -0.0f,
		-1.0f,  1.0f, -0.0f,
		 1.0f, -1.0f, -0.0f
};

unsigned int indices[] =
{
	//  2---,1
	//  | .' |
	//  0'---3
		0, 1, 2,
		0, 3, 1
};
int num_frames{};
float last_time{};
int width = 1080, height = 1080;
float center_x{ 0.0f };
float center_y{ 0.0f };
float zoom{ 1.0 };

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		center_y = center_y + 0.005f * zoom;
		if (center_y > 1.0f)
		{
			center_y = 1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		center_y = center_y - 0.005f * zoom;
		if (center_y < -1.0f)
		{
			center_y = -1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		center_x = center_x - 0.005f * zoom;
		if (center_x < -1.0f)
		{
			center_x = -1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		center_x = center_x + 0.005f * zoom;
		if (center_x > 1.0f)
		{
			center_x = 1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		zoom = zoom * 1.02f;
		if (zoom > 1.0f)
		{
			zoom = 1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		zoom = zoom * 0.98f;
		if (zoom < 0.00001f)
		{
			zoom = 0.00001f;
		}
	}
}
float randomNormalized()
{
	return ((double)rand() / (RAND_MAX));
}
void set_float(GLuint programID, const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}


void set_vec4(GLuint programID, const std::string& name, glm::vec4 vec)
{
	glUniform4f(glGetUniformLocation(programID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}
glm::vec4 find_ranges(std::vector<float>& data)
{
	std::sort(data.begin(), data.end());
	GLint lowest = 0;
	while (data[lowest] == 0.0f && lowest < 1000)
	{
		++lowest;
	}

	GLint size = data.size();
	GLint length = size - lowest;
	//glm::vec4 ranges = glm::vec4(0, 0, 0,  0);
	glm::vec4 ranges = glm::vec4(data[lowest], data[lowest + length * 0.75f - 1], data[lowest + length * 0.875f - 1], data[size - 1]);
	return ranges;
}
void PrintInfo()
{
	double current_time = glfwGetTime();
	num_frames++;
	if (current_time - last_time >= 1.0)
	{
		std::cout << "Millis/frame: " << 1000.0 / num_frames << "\t FPS: " << num_frames << "\t";
		std::cout << "Zoom factor: " << zoom << "\t X: " << center_x << "\t Y: " << center_y << "\n";
		
		num_frames = 0;
		last_time += 1.0;
	}
}
glm::vec4 generateRandomColor(glm::vec4 mix) {
	
	float red = randomNormalized();
	float green = randomNormalized();
	float blue = randomNormalized();

	red = (red + mix.x) / 2;
	green = (green + mix.y) / 2;
	blue = (blue + mix.z) / 2;

	
	return glm::vec4(red,green,blue,1);
}
void printColor(glm::vec4 c) {
	std::cout << "(R: "<<c.x<<"; G: "<<c.y<<"; B: " << c.z <<"; A: "<< c.w << ")\n";
}
int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of x by y pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "Fractal viewer", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x , y 
	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert","default.frag");



	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	std::vector<float> pixel_data(width * height, 0.0f);
	glm::vec4 ranges = glm::vec4(0.0001f, 0.33333f, 0.66667f, 1.00f);

	// genero e setto colori 
	srand(time(NULL));

	glm::vec4 color_0 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 color_1 = glm::vec4(randomNormalized()+0.1f, randomNormalized()+0.1f, randomNormalized()+0.1f, 1.0f);
	glm::vec4 color_2 = generateRandomColor(generateRandomColor(color_1));
	glm::vec4 color_3 = generateRandomColor(generateRandomColor(color_2));
	
	std::cout << "Palette di colori generata:\n";
	std::cout << "Colore principale del frattale: ";
	printColor(color_0);
	std::cout << "Colore del range #1: ";
	printColor(color_1);
	std::cout << "Colore del range #2: ";
	printColor(color_2);
	std::cout << "Colore del range #3: ";
	printColor(color_3);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		process_input(window);
		PrintInfo();
		


		set_float(shaderProgram.ID, "zoom", zoom);
		set_float(shaderProgram.ID, "center_x", center_x);
		set_float(shaderProgram.ID, "center_y", center_y);
		set_vec4(shaderProgram.ID, "color_ranges", ranges);

		set_vec4(shaderProgram.ID, "color_0", color_0);
		set_vec4(shaderProgram.ID, "color_1", color_1);
		set_vec4(shaderProgram.ID, "color_2", color_2);
		set_vec4(shaderProgram.ID, "color_3", color_3);
		// Tell OpenGL which Shader Program we want to use
		// [OLD] glUseProgram(shaderProgram);
		shaderProgram.Activate();

		// Bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);
		// Draw the triangle using the GL_TRIANGLES primitive
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();

		glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, pixel_data.data());
		ranges = find_ranges(pixel_data);
	}



	// Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	//glDeleteProgram(shaderProgram);
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
