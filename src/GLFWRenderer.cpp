#include <cmath>
#include <thread>
#include "GLFWRenderer.h"

using namespace std;

GLFWRenderer::GLFWRenderer(int w, int h) : width { w }, height { h }
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(w, h, glfwGetVersionString(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewInit();
}

GLFWRenderer::~GLFWRenderer()
{
	glfwMakeContextCurrent(window);
	glfwDestroyWindow(window);
}

bool GLFWRenderer::isRunning()
{
	return glfwWindowShouldClose(window);
}

void GLFWRenderer::sleepUntilNextFrame(int frame_count)
{
	next_frame += frame_duration(1);
	this_thread::sleep_until(next_frame);
}

void drop_cb(GLFWwindow* window, int count, const char** names)
{
	printf("Drop!\n");
	for (int i = 0; i < count; ++i)
	{
		printf("%s\n", names[i]);
	}
}

void mouse_cb(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		printf("Click!\n");
}

void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}
		printf("Stroke!\n");
	}
}

void GLFWRenderer::run()
{
	glfwSetDropCallback(window, drop_cb);
	glfwSetKeyCallback(window, key_cb);
	glfwSetMouseButtonCallback(window, mouse_cb);

	auto render_thread = thread([this]
	{
		glfwMakeContextCurrent(window);

		wstring text = L"Lorem ipsum dolor sit amet, ...";
		int size = 48;

		StaticText t1("Lora/Lora-Regular", size);
		t1.setText(text);
		t1.setColor(1, 0.5, 0);
		// t1.setSpacing(-5);

		StaticText t2("NoticiaText/NoticiaText-Regular", size);
		t2.setText(text);
		t2.setColor(0, 1, 0);
		t2.setOpacity(50);
		// t2.setSpacing(-5);

		StaticText t3("KaushanScript/KaushanScript-Regular", size);
		t3.setText(text);
		t3.setColor(1, 0.5, 1);
		t3.setOpacity(70);
		// t3.setSpacing(-5);

		while (!glfwWindowShouldClose(window))
		{
			glfwMakeContextCurrent(window);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			// glFrustum(fNear *(-fFov * ratio + headX),
			// 		  fNear *(fFov * ratio + headX),
			// 		  fNear *(-fFov + headY),
			// 		  fNear *(fFov + headY),
			// 		  fNear, fFar);
			glOrtho(0, width, height, 0, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// t1.setOpacity((sin(glfwGetTime() * 4) + 4) * 20.0f);
			// t2.setOpacity((sin(glfwGetTime() * 4 + 1.0) + 4) * 20.0f);
			// t3.setOpacity((sin(glfwGetTime() * 4 + 2.0) + 4) * 20.0f);

			// static int frame_no = 1;
			// t1.setText(to_wstring(frame_no));
			// t2.setText(to_wstring(frame_no));
			// t3.setText(to_wstring(frame_no));
			// frame_no++;

			t1.drawText(40, 50);
			t2.drawText(50, 100);
			t3.drawText(60, 150);

			glfwSwapBuffers(window);
			sleepUntilNextFrame();
		}
	});

	while (!glfwWindowShouldClose(window))
	{
		glfwWaitEvents();
	}

	if (render_thread.joinable())
		render_thread.join();
}
