#include "GLFWRenderer.h"
#include <cmath>
#include <thread>
#include "LazyText.h"


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
	next_frame += frame_count * frame_duration(1);
	std::this_thread::sleep_until(next_frame);
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

	auto render_thread = std::thread([this]
	{
		glfwMakeContextCurrent(window);

		std::string text = "Lorem ipsum dolor sit amet,\n"
			"consectetur adipisicing elit,\n"
			"sed do eiusmod tempor incididunt\n"
			"ut labore et dolore magna aliqua.";
		int size = 48;

		LazyText t1("NotoSans-Regular", size);
		t1.setText(text);
		// t1.setAlign(LazyText::TextAlign::Center);
		// t1.setColor(1, 0.5, 0);
		// t1.setOpacity(90);
		// t1.setSpacing(2);

		LazyText t2("NotoSerif-Regular", size);
		t2.setText(text);
		// t2.setColor(0.5, 1, 1);
		// t2.setOpacity(60);
		// t2.setSpacing(2);

		LazyText t3("NotoMono-Regular", size);
		t3.setText(text);
		// t3.setColor(1, 0.5, 0.5);
		// t3.setOpacity(80);
		// t3.setSpacing(2);


		while (!glfwWindowShouldClose(window))
		{
			glfwMakeContextCurrent(window);

			// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			// glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			// glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
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

			drawGrid(4, 4);

			// t1.setOpacity((sin(glfwGetTime() * 4) + 4) * 20.0f);
			// t2.setOpacity((sin(glfwGetTime() * 4 + 1.0) + 4) * 20.0f);
			// t3.setOpacity((sin(glfwGetTime() * 4 + 2.0) + 4) * 20.0f);

			// static int frame_no = 1;
			// t1.setText(to_string(frame_no));
			// t2.setText(to_string(frame_no));
			// t3.setText(to_string(frame_no));
			// frame_no++;

			// t1.setAlign(LazyText::TextAlign::Left);
			t1.setAlign(LazyText::TextAlign::Center);
			// t1.setAlign(LazyText::TextAlign::Right);
			t1.setOrigin(0.5, 0.5);

			t1.drawAll(width / 2, height / 2);
			// t2.drawText(width / 2, 70 + size * 2);
			// t3.drawText(width / 2, 90 + size * 3);

			// t1.setSize(size);
			// t2.setSize(size);
			// t3.setSize(size);
			// size++;

			glfwSwapBuffers(window);
			sleepUntilNextFrame(15);
		}
	});

	while (!glfwWindowShouldClose(window))
	{
		glfwWaitEvents();
	}

	if (render_thread.joinable())
		render_thread.join();
}

void GLFWRenderer::drawGrid(int xdiv, int ydiv)
{
	float xres = width / xdiv;
	float yres = height / ydiv;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (int i = 1; i < xdiv; i++)
	{
		glBegin(GL_LINES);
			glVertex2f(i * xres, 0);
			glVertex2f(i * xres, height);
		glEnd();
	}
	for (int i = 1; i < ydiv; i++)
	{
		glBegin(GL_LINES);
			glVertex2f(0, i * yres);
			glVertex2f(width, i * yres);
		glEnd();
	}
}
