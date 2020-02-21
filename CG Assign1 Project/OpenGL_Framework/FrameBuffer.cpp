#include "FrameBuffer.h"
#include "IO.h"

int FrameBuffer::_MacColorAttachments = 0;
bool FrameBuffer::_IsInitFrameBuffers = false;
VertexArrayObject FrameBuffer::_FullScreenQuad;

DepthTarget::~DepthTarget()
{
	unload();
}

void DepthTarget::unload()
{
	glDeleteTextures(1, &_Handle);
}

ColorTarget::~ColorTarget()
{
	unload();
}

void ColorTarget::unload()
{
	if (_NumAttachments)
	{
		glDeleteTextures(_NumAttachments, &_Handle.front());
		_Handle.resize(0);
		_Format.resize(0);
		_Buffer.resize(0);
	}
	_NumAttachments = 0;
}


FrameBuffer::~FrameBuffer()
{
	unload();
}

void FrameBuffer::unload()
{
	if (_FBO)
	{
		glDeleteFramebuffers(1, &_FBO);
		_IsInit = false;
	}
}

void FrameBuffer::init(unsigned width, unsigned height)
{
	_Width = width;
	_Height = height;
	SAT_ASSERT(_Width > 0 && _Height > 0, "Width and/or Heifht nor initialized!");
	SAT_ASSERT(!_IsInit, "Framebuffer already init!");

	glGenFramebuffers(1, &_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	if (_Depth._IsActive)
	{
		_ClearFlag |= GL_DEPTH_BUFFER_BIT; // _ClearFlag = _ClearFlag | GL_DEPTH_BUFFER_BIT;

		// Create Depth texture
		glGenTextures(1, &_Depth._Handle);
		glBindTexture(GL_TEXTURE_2D, _Depth._Handle);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, _Width, _Height);

		// Set texture parameters for depth
		glTextureParameteri(_Depth._Handle, GL_TEXTURE_MIN_FILTER, _Filter);
		glTextureParameteri(_Depth._Handle, GL_TEXTURE_MAG_FILTER, _Filter);
		glTextureParameteri(_Depth._Handle, GL_TEXTURE_WRAP_S, _Wrap);
		glTextureParameteri(_Depth._Handle, GL_TEXTURE_WRAP_T, _Wrap);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _Depth._Handle, 0);

		if (_Color._NumAttachments)
		{
			_ClearFlag |= GL_COLOR_BUFFER_BIT;

			glGenTextures(_Color._NumAttachments, &_Color._Handle.front());
			for (unsigned int i = 0; i < _Color._NumAttachments; i++)
			{
				glBindTexture(GL_TEXTURE_2D, _Color._Handle[i]);
				glTexStorage2D(GL_TEXTURE_2D, 1, _Color._Format[i], _Width, _Height);


				glTextureParameteri(_Color._Handle[i], GL_TEXTURE_MIN_FILTER, _Filter);
				glTextureParameteri(_Color._Handle[i], GL_TEXTURE_MAG_FILTER, _Filter);
				glTextureParameteri(_Color._Handle[i], GL_TEXTURE_WRAP_S, _Wrap);
				glTextureParameteri(_Color._Handle[i], GL_TEXTURE_WRAP_T, _Wrap);

				glFramebufferTexture2D(GL_FRAMEBUFFER, _Color._Buffer[i], GL_TEXTURE_2D, _Color._Handle[i], 0);
			}

			//glFramebufferTexture2D(GL_FRAMEBUFFER, _Color._Buffer[i], GL_TEXTURE_2D, _Color._Handle[i], 0);
		}
	}
	unbind();

	_IsInit = true;
}

void FrameBuffer::addDepthTarget()
{
	SAT_ASSERT(!_IsInit, "Framebuffer already initialized!");
	if (!_IsInit)
	{
		_Depth._IsActive = true;
	}
}

void FrameBuffer::addColorTarget(GLenum format)
{
	SAT_ASSERT(!_IsInit, "Framebuffer already initialized!");
	SAT_ASSERT(_Color._Handle.size() <= (size_t)_MacColorAttachments, "Maximum Color of attachment reach.");
	if (!_IsInit)
	{
		_Color._Handle.push_back(0);
		_Color._Format.push_back(format);
		_Color._Buffer.push_back(GL_COLOR_ATTACHMENT0 + _Color._Buffer.size());
		_Color._NumAttachments++;
	}
}

void FrameBuffer::resize(unsigned width, unsigned height)
{
	
}

void FrameBuffer::setViewport() const
{
	glViewport(0, 0, _Width, _Height);
}

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	if (_Color._NumAttachments)
		glDrawBuffers(_Color._NumAttachments, &_Color._Buffer.front());
}

void FrameBuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void FrameBuffer::rebderToFSQ() const
{
	SAT_ASSERT(_IsInitFrameBuffers, "FSQ not init!");
	setViewport();
	bind();
	drawFSQ();
	unbind();
}

void FrameBuffer::bindColorAsTexture(unsigned colorBuffer, int textureSlot) const
{
	SAT_ASSERT(_IsInit, "Framebuffer not initialiazed!");
	SAT_ASSERT(colorBuffer < _Color._Handle.size(), "Color Target doesn't exist!");

	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, _Color._Handle[colorBuffer]);
}

void FrameBuffer::bindDepthAsTexture(int textureSlot) const
{
	SAT_ASSERT(_IsInit, "Framebuffer not initialiazed!");
	SAT_ASSERT(_Depth._IsActive, "DepthBuffer doesn't exist!");

	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, _Depth._Handle);
}

void FrameBuffer::unbindTexture(int textureSlot) const
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void FrameBuffer::clear() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	glClear(_ClearFlag);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void FrameBuffer::initFrameBuffers()
{
	float VBO_Vertex[] =
	{
		-1.0f, -1.0f, 0.0f, // first
		 1.0f, -1.0f, 0.0f, // first second
		-1.0f,  1.0f, 0.0f, // first second
		 1.0f,  1.0f, 0.0f  //       second
	};

	float VBO_UV[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	VertexBufferData vertexData, uvData;

	vertexData.attributeType = VERTEX;
	vertexData.elementType = GL_FLOAT;
	vertexData.numElementsPerAttribute = 3;
	vertexData.numElements = 4 * vertexData.numElementsPerAttribute;
	vertexData.sizeOfElement = sizeof(float);
	vertexData.data = &VBO_Vertex[0];

	uvData.attributeType = TEXCOORD;
	uvData.elementType = GL_FLOAT;
	uvData.numElementsPerAttribute = 2;
	uvData.numElements = 4 * uvData.numElementsPerAttribute;
	uvData.sizeOfElement = sizeof(float);
	uvData.data = &VBO_UV[0];

	_FullScreenQuad.addVBO(vertexData);
	_FullScreenQuad.addVBO(uvData);
	_FullScreenQuad.setPrimitiveType(GL_TRIANGLE_STRIP);
	_FullScreenQuad.createVAO();
	_IsInitFrameBuffers = true;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &_MacColorAttachments);
	SAT_DEBUG_LOG_SAFE("Maxium Framebuffer Color Attachment %d", _MacColorAttachments);
}

void FrameBuffer::drawFSQ()
{
	SAT_ASSERT(_IsInitFrameBuffers, "FSQ not init!");
	_FullScreenQuad.draw();
}

