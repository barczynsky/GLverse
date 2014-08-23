#include <cstdlib>
#include "GLFWRenderer.h"

int main(int argc, char **argv)
{
	int w = 0;
	int h = 0;
	if (argc == 3)
	{
		w = atoi(argv[1]);
		h = atoi(argv[2]);
	}
	if (w == 0) w = 1920 / 2;
	if (h == 0) h = 1080 / 2;

	if (true)
	{
		GLFWRenderer renderer(w, h);
		renderer.run();
	}
}
