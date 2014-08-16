#pragma once
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "StaticText.h"

class GLFWRenderer
{
private:
	GLFWwindow *window;
	int width;
	int height;

private:
	typedef std::chrono::steady_clock clock;
	typedef std::chrono::duration<int64_t, std::ratio<1, 6000000000>> clock_resolution;
	typedef std::chrono::duration<int64_t, std::ratio<1, 60>> frame_duration;
	std::chrono::time_point<clock, clock_resolution> next_frame{ clock::now() };

public:
	GLFWRenderer(int width, int height);
	~GLFWRenderer();

	void run();
	bool isRunning();

	void sleepUntilNextFrame(int frame_count = 1);

};
