#pragma once
#include <GL/glew.h>
#include <vector>
#include "VertexBufferObject.h"

struct DepthTarget
{
	~DepthTarget();
	void unload();
	GLuint _Handle;
	bool _IsActive = false;
};

struct ColorTarget
{
	~ColorTarget();
	void unload();
	std::vector<GLuint> _Handle;
	std::vector<GLenum> _Format;
	std::vector<GLenum> _Buffer;
	unsigned _NumAttachments = 0;
};

class FrameBuffer
{
public:
	~FrameBuffer();
	void unload();
	void init(unsigned width, unsigned height);
	void addDepthTarget();
	void addColorTarget(GLenum format);
	void resize(unsigned width, unsigned height);
	
	void setViewport() const;
	void bind() const;
	void unbind() const;
	void rebderToFSQ() const; // Full Screen Quad

	void bindColorAsTexture(unsigned colorBuffer, int textureSlot) const;
	void bindDepthAsTexture(int textureSlot) const;
	void unbindTexture(int textureSlot) const;

	void clear() const;

	GLuint _FBO; // Our handle to the framebuffer
	bool _IsInit = false;

	DepthTarget _Depth;
	ColorTarget _Color;

	unsigned _Width = 0;
	unsigned _Height = 0;

	GLenum _Filter = GL_NEAREST;
	GLenum _Wrap = GL_CLAMP_TO_EDGE;

	GLbitfield _ClearFlag = 0;

	static void initFrameBuffers();
	static void drawFSQ();

private:
	static int _MacColorAttachments;
	static bool _IsInitFrameBuffers;
	static VertexArrayObject _FullScreenQuad;
};