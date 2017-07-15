#pragma once
#include "OpenGL.h"


class BaseTextRendererGL2
{
public:
	struct Point
	{
		GLfloat x{};
		GLfloat y{};
		operator GLfloat*() const { return (GLfloat*)&x; }
		Point(GLfloat x, GLfloat y)
			: x{ x }
			, y{ y }
			{}
		Point(GLint x, GLint y)
			: x{ static_cast<GLfloat>(x) }
			, y{ static_cast<GLfloat>(y) }
			{}
	};

	struct Rect
	{
		GLfloat x{};
		GLfloat y{};
		GLfloat w{};
		GLfloat h{};
		operator GLfloat*() const { return (GLfloat*)&x; }
		Rect(GLfloat x, GLfloat y, GLfloat w, GLfloat h)
			: x{ x }
			, y{ y }
			, w{ w }
			, h{ h }
			{}
		Rect(GLint x, GLint y, GLint w, GLint h)
			: x{ static_cast<GLfloat>(x) }
			, y{ static_cast<GLfloat>(y) }
			, w{ static_cast<GLfloat>(w) }
			, h{ static_cast<GLfloat>(h) }
			{}
	};

	struct Color
	{
		GLfloat r{};
		GLfloat g{};
		GLfloat b{};
		GLfloat a{};
		operator GLfloat*() const { return (GLfloat*)&r; }
		Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
			: r{ r }
			, g{ g }
			, b{ b }
			, a{ a }
			{}
		Color(GLint r, GLint g, GLint b, GLint a)
			: r{ static_cast<GLfloat>(r) }
			, g{ static_cast<GLfloat>(g) }
			, b{ static_cast<GLfloat>(b) }
			, a{ static_cast<GLfloat>(a) }
			{}
	};

public:
	static void drawTexture(GLuint texture, Rect r, Color c)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glColor4fv(c);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(r.x, r.y);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(r.x + r.w, r.y);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(r.x + r.w, r.y + r.h);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(r.x, r.y + r.h);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	static void drawRect(Rect r, Color c, float line_width = 1)
	{
		glColor4fv(c);
		glLineWidth(line_width);
		glBegin(GL_LINE_LOOP);
			glVertex2f(r.x, r.y);
			glVertex2f(r.x + r.w, r.y);
			glVertex2f(r.x + r.w, r.y + r.h);
			glVertex2f(r.x, r.y + r.h);
		glEnd();
	}

	static void drawLine(Point p1, Point p2, Color c, float line_width = 1)
	{
		glColor4fv(c);
		glLineWidth(line_width);
		glBegin(GL_LINES);
			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
		glEnd();
	}

	static void drawCrosshair(Point p, float r, Color c, float line_width = 1)
	{
		glColor4fv(c);
		glLineWidth(line_width);
		glBegin(GL_LINES);
			glVertex2f(p.x - r, p.y);
			glVertex2f(p.x + r, p.y);
			glVertex2f(p.x, p.y - r);
			glVertex2f(p.x, p.y + r);
		glEnd();
	}

};
