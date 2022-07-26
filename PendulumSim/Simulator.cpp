#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "own_shaders.h"
#include <vector>
#include <boost/numeric/odeint.hpp>
#include "particle_generator.h"
#include "spring_pendulum.h"
#include "double_pendulum.h"
#include "opencv2\opencv.hpp"

typedef std::vector< float > state_type;

constexpr int NUM_STEPS = 100;
constexpr float initial_angle = glm::radians(160.0f);
constexpr float initial_angle2 = glm::radians(177.0f);
constexpr float gravity = 9.81f;
constexpr float spring_constant = 75.0f;
constexpr int fps_factor = 32;
constexpr float time_constant = 1.0f / fps_factor;
constexpr int window_size = 1000;
constexpr int fps = 30;
constexpr float deltaTime = time_constant / fps;
constexpr int video_length = 30; // length of the video, in seconds

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void real_pendulum(const state_type& x, state_type& dxdt, const double /* t */);
void spring_pendulum(const state_type& x, state_type& dxdt, const double /* t */);
glm::vec3 hsl_to_rgb(float h, float s, float l);
float hue_to_rgb(float p, float q, float t);
void saveFramebuffer(GLFWwindow* window);
void test_func(void);

float vertices[] = {
	 0.005f,  0.5f, 0.0f,  // top right
	 0.005f, -0.5f, 0.0f,  // bottom right
	-0.005f, -0.5f, 0.0f,  // bottom left
	-0.005f,  0.5f, 0.0f   // top left 
};

const float angular_frequency = sqrt(gravity);

unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

float lastFrame = 0.0f;
float elapsed_time = 0.0f;

glm::vec3 color1(1.0f, 0.0f, 0.0f);
glm::vec3 color2(0.0f, 0.0f, 1.0f);
glm::vec3 color3(0.0f, 1.0f, 0.0f);

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 10); // anti-aliasing
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(window_size, window_size, "Pendulum Simulator", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window. Something went wrong." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initiate GLAD. Something went wrong." << std::endl;
		return -1;
	}

	state_type theta(2); // theta, theta'
	theta[0] = initial_angle;
	theta[1] = 0.0f;

	state_type spring_state(4); // x, x', theta, theta'
	spring_state[0] = 0.0f; // initial length always 1 to make scaling easier
	spring_state[1] = 0.0f;
	spring_state[2] = initial_angle;
	spring_state[3] = 0.0f;

	state_type double_state(4, 0.0f); // theta1, theta2, p1, p2
	double_state[0] = initial_angle;
	double_state[1] = initial_angle2;

	glViewport(0, 0, window_size, window_size);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader* ourShader = new Shader("shader_practice.vert", "shader_practice.frag");
	Shader* particleShader = new Shader("particle.vert", "particle.frag");

	// setup classes before we even create any instances of them
	ParticleGenerator::setup();
	Pendulum::init();
	
	ParticleGenerator* Particles;
	Particles = new ParticleGenerator(particleShader, 5000, color3, 0.1f);

	std::vector<Pendulum*> pendulums;
	int num_pendulums = 1000;
	/*for (int i = 0; i < num_pendulums; ++i) {
		pendulums.push_back(new SpringPendulum(ourShader, particleShader, spring_state, spring_constant, hsl_to_rgb(((float) i) / num_pendulums, 1.0f, 0.5f)));
		spring_state[0] -= 0.0001f;
	}*/

	for (int i = 0; i < num_pendulums; ++i) {
		pendulums.push_back(new DoublePendulum(ourShader, particleShader, double_state, hsl_to_rgb(((float)i) / num_pendulums, 1.0f, 0.5f)));
		double_state[1] += glm::radians(0.00001f);
	}

	/*unsigned int VAO, VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);*/
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		//lastFrame = (float)time_constant * glfwGetTime();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// solve diffeq
		//boost::numeric::odeint::integrate(real_pendulum, theta, time - deltaTime, time, deltaTime / NUM_STEPS);
		//boost::numeric::odeint::integrate(spring_pendulum, spring_state, time - deltaTime, time, deltaTime / NUM_STEPS);

		//// pendulum with "physics I" equation
		glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(0.0f, 0.5f, 0.0f));
		//trans = glm::rotate(trans, initial_angle*cos(angular_frequency * time), glm::vec3(0.0f, 0.0f, 1.0f));
		//trans = glm::translate(trans, glm::vec3(0.0f, -0.5f, 0.0f));

		unsigned int transformLoc = glGetUniformLocation(ourShader->ID, "transform");
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		unsigned int colorLoc = glGetUniformLocation(ourShader->ID, "ourColor");
		//glUniform3fv(colorLoc, 1, glm::value_ptr(color1));

		ourShader->use();
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//// pendulum with diffeq
		//trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(0.0f, 0.5f, 0.0f));
		//trans = glm::rotate(trans, theta[0], glm::vec3(0.0f, 0.0f, 1.0f));
		//trans = glm::translate(trans, glm::vec3(0.0f, -0.5f, 0.0f));

		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		//glUniform3fv(colorLoc, 1, glm::value_ptr(color2));

		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		//// spring pendulum
		//trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(0.0f, 0.5f, 0.0f));
		//trans = glm::rotate(trans, spring_state[2], glm::vec3(0.0f, 0.0f, 1.0f));
		//trans = glm::scale(trans, glm::vec3(1.0f, 1.0f + spring_state[0], 1.0f));
		//trans = glm::translate(trans, glm::vec3(0.0f, -0.5f, 0.0f));

		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		//glUniform3fv(colorLoc, 1, glm::value_ptr(color3));

		//// glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Particles->Update(deltaTime, glm::vec2((1.0f + spring_state[0]) * sin(spring_state[2]), 0.5f - (1.0f + spring_state[0]) * cos(spring_state[2])), 1);
		//Particles->Draw();
		// 
		// get changes in time
		//deltaTime = (float)(time_constant * glfwGetTime() - lastFrame);
		elapsed_time += 0.0f;
		// time updates must be done as close to each other as possible to avoid "skipped" time
		
		for (Pendulum*& p : pendulums) {
			p->update(0.0f, deltaTime);
			p->draw();
		}

		saveFramebuffer(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void real_pendulum(const state_type& x, state_type& dxdt, const double /* t */) {
	dxdt[0] = x[1];
	dxdt[1] = -gravity * sin(x[0]);
}

void spring_pendulum(const state_type& x, state_type& dxdt, const double /* t */) {
	dxdt[0] = x[1];
	dxdt[1] = (1.0f + x[0]) * x[3] * x[3] - spring_constant * x[0] + gravity * cos(x[2]); // mass 1
	dxdt[2] = x[3];
	dxdt[3] = -gravity * sin(x[2]) / (1.0f + x[0]) - 2 * x[1] * x[3] / (1.0f + x[0]);
}

glm::vec3 hsl_to_rgb(float h, float s, float l) {
	if (s == 0.0f) return glm::vec3(1.0f, 1.0f, 1.0f);

	float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
	float p = 2 * l - q;

	return glm::vec3(hue_to_rgb(p, q, h + 1.0f / 3), hue_to_rgb(p, q, h), hue_to_rgb(p, q, h - 1.0f / 3));
}

float hue_to_rgb(float p, float q, float t) {
	if (t < 0.0f) t += 1;
	else if (t > 1.0f) t -= 1;
	if (t < 1.0f / 6) return p + (q - p) * 6 * t;
	if (t < 1.0f / 2) return q;
	if (t < 2.0f / 3) return p + (q - p) * (2.0f / 3 - t) * 6;
	return p;
}

void saveFramebuffer(GLFWwindow* window) {

	static int fid = 0;
	static cv::VideoWriter outputVideo;
	static double start_time;
	//static int height, width;
	int const total_frame = video_length * fps * fps_factor;

	//int const total_frame = 120;
	if (fid == 0) {
		/*RECT WindowRect;
		GetWindowRect(hWnd, &WindowRect);
		width = WindowRect.right - WindowRect.left;
		height = WindowRect.bottom - WindowRect.top;*/
		outputVideo.open("video.avi",  /*Video Name*/
			cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),                         /* fourcc */
			(double)fps,                      /*Yuchen: Frame Rate */
			cv::Size(window_size, window_size),  /*Yuchen: Frame Size of the Video  */
			true);                      /*Yuchen: Is Color                 */
		start_time = glfwGetTime();
	}
	else if (fid < total_frame) {
		cv::Mat pixels( /* num of rows */ window_size, /* num of cols */ window_size, CV_8UC3);
		glReadPixels(0, 0, window_size, window_size, GL_BGR, GL_UNSIGNED_BYTE, pixels.data);
		//cv::Mat cv_pixels( /* num of rows */ window_size, /* num of cols */ window_size, CV_8UC3);
		//for (int y = 0; y < window_size; y++) for (int x = 0; x < window_size; x++)
		//{
		//	cv_pixels.at<cv::Vec3b>(y, x)[2] = pixels.at<cv::Vec3b>(window_size - y - 1, x)[0];
		//	cv_pixels.at<cv::Vec3b>(y, x)[1] = pixels.at<cv::Vec3b>(window_size - y - 1, x)[1];
		//	cv_pixels.at<cv::Vec3b>(y, x)[0] = pixels.at<cv::Vec3b>(window_size - y - 1, x)[2];
		//}
		outputVideo << pixels;
		if (fid % 1000 == 0) {
			std::cout << fid << " out of " << total_frame << " frames generated.\tProgress: " << fid * 100.0 / total_frame << "%.\tEstimated time left: " << 1000 * ((glfwGetTime() - start_time) / fid) * (total_frame * 1.0 - fid) / (60 * 1000) << " minutes." << std::endl;
		}
	}
	else if (fid == total_frame) {
		std::cout << "Total rendering time : " << (glfwGetTime() - start_time) / 60 << " minutes." << std::endl;
		outputVideo.release();
		glfwSetWindowShouldClose(window, true);
	}
	fid++;
}

void test_func(void) {
	std::cout << "Hello World!" << std::endl;
}