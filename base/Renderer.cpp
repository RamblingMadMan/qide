/**
 *	QIDE - The Quake Mod IDE
 *	Copyright (C) 2021  Keith Hammond (RamblingMad)
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <memory>

#include "fmt/core.h"

#include "glbinding/glbinding.h"
#include "glbinding/gl43core/gl.h"
#include "glbinding-aux/Meta.h"

using namespace gl;

#include <glm/gtc/type_ptr.hpp>

#include "qide/RendererGL.hpp"
#include "qide/Camera.hpp"

#include "shaders.hpp"

using namespace qide;

RenderGroup::~RenderGroup(){}

Renderer::~Renderer(){}

Texture2D::~Texture2D(){}

Framebuffer::Framebuffer(std::vector<Framebuffer::AttachmentInfo> attachments_)
	: m_attachments(std::move(attachments_))
{
	for(const auto &a : m_attachments){
		// my eyes
		if(a.kind == Framebuffer::AttachmentKind::depth &&
		   (a.pixelKind < Texture2D::PixelKind::firstDepthFormat || a.pixelKind > Texture2D::PixelKind::lastDepthFormat)
		)
				throw std::runtime_error("invalid pixel kind for depth attachment");
		else if(
			a.kind == Framebuffer::AttachmentKind::color &&
			a.pixelKind >= Texture2D::PixelKind::firstDepthFormat && a.pixelKind <= Texture2D::PixelKind::lastDepthFormat
		)
			throw std::runtime_error("invalid pixel kind for color attachment");
	}
}

namespace {
	template<typename ... Fns>
	struct Overloaded: Fns...{
		using Fns::operator()...;
	};

	template<typename ... Fns>
	Overloaded(Fns&&...) -> Overloaded<Fns...>;

	template<typename ... Fns>
	auto overload(Fns &&... fns){
		return Overloaded{ std::forward<Fns>(fns)... };
	}

	GLenum faceKindToGL(Shape::FaceKind kind){
		using K = Shape::FaceKind;
		switch(kind){
			case K::tris: return GL_TRIANGLES;
			case K::triFan: return GL_TRIANGLE_FAN;
			default: throw std::runtime_error("invalid face kind");
		}
	}
}

struct RenderGroupGL43::GLDrawElementsIndirectCommand{
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
};

RenderGroupGL43::RenderGroupGL43(const shapes::Points &shape)
{
	glCreateBuffers(std::size(m_bufs), m_bufs);
	auto vec3BufSize = shape.numPoints() * sizeof(Vec3);
	auto vec2BufSize = shape.numPoints() * sizeof(Vec2);
	auto indicesSize = shape.numIndices() * sizeof(Nat32);
	glNamedBufferStorage(m_bufs[0], vec3BufSize, shape.vertices(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(m_bufs[1], vec3BufSize, shape.normals(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(m_bufs[2], vec2BufSize, shape.uvs(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(m_bufs[3], indicesSize, shape.indices(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(m_bufs[4], sizeof(GLDrawElementsIndirectCommand), nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

	m_drawKind = (Nat32)faceKindToGL(shape.faceKind());
	m_cmd = (GLDrawElementsIndirectCommand*)glMapNamedBufferRange(m_bufs[4], 0, sizeof(GLDrawElementsIndirectCommand), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

	m_cmd->count = shape.numIndices();
	m_cmd->instanceCount = 1;
	m_cmd->firstIndex = 0;
	m_cmd->baseVertex = 0;
	m_cmd->baseInstance = 0;

	glFlushMappedNamedBufferRange(m_bufs[4], 0, sizeof(GLDrawElementsIndirectCommand));

	glCreateVertexArrays(1, &m_vao);

	glVertexArrayVertexBuffer(m_vao, 0, m_bufs[0], 0, sizeof(Vec3));
	glVertexArrayVertexBuffer(m_vao, 1, m_bufs[1], 0, sizeof(Vec3));
	glVertexArrayVertexBuffer(m_vao, 2, m_bufs[2], 0, sizeof(Vec2));
	glVertexArrayElementBuffer(m_vao, m_bufs[3]);

	glEnableVertexArrayAttrib(m_vao, 0);
	glEnableVertexArrayAttrib(m_vao, 1);
	glEnableVertexArrayAttrib(m_vao, 2);

	glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(m_vao, 2, 2, GL_FLOAT, GL_FALSE, 0);

	glVertexArrayAttribBinding(m_vao, 0, 0);
	glVertexArrayAttribBinding(m_vao, 1, 1);
	glVertexArrayAttribBinding(m_vao, 2, 2);
}

RenderGroupGL43::~RenderGroupGL43(){
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(std::size(m_bufs), m_bufs);
}

Nat32 RenderGroupGL43::numInstances() const noexcept{
	return m_cmd->instanceCount;
}

void RenderGroupGL43::setNumInstances(Nat32 n){
	m_cmd->instanceCount = n;
	glFlushMappedNamedBufferRange(m_bufs[4], offsetof(GLDrawElementsIndirectCommand, instanceCount), sizeof(m_cmd->instanceCount));
}

void RenderGroupGL43::draw(){
	glBindVertexArray(m_vao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_bufs[4]);
	glDrawElementsIndirect((GLenum)m_drawKind, GL_UNSIGNED_INT, nullptr);
}

namespace {
	void glDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam){
		auto r = reinterpret_cast<const RendererGL43*>(userParam);

		(void)r;

		using M = glbinding::aux::Meta;

		fmt::print(stderr, "[{} @ {}] {}: {}\n", M::getString(type), M::getString(source), M::getString(id), std::string_view(message, length));
	}
}

RendererGL43::RendererGL43(Nat16 w, Nat16 h, GLGetProcFn getProc, void *ctx)
	: m_unitSquare(2.f)
	, m_unitCube(2.f)
	, m_w(w), m_h(h)
{
	if(ctx){
		glbinding::initialize(glbinding::ContextHandle(ctx), getProc);
	}
	else{
		glbinding::initialize(getProc);
	}

#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebug, this);
#endif

	std::vector<Framebuffer::AttachmentInfo> attachments = {
		{ Framebuffer::AttachmentKind::depth, Texture2D::PixelKind::d32f }, // depth
		{ Framebuffer::AttachmentKind::color, Texture2D::PixelKind::rgba8 }, // diffuse texture
		{ Framebuffer::AttachmentKind::color, Texture2D::PixelKind::rgba16f }, // lighting space texture
		{ Framebuffer::AttachmentKind::color, Texture2D::PixelKind::rgba8 } // final scene
	};

	m_fb = std::make_unique<FramebufferGL43>(w, h, std::move(attachments));

	m_shaders.reserve(4);

	auto fullbrightVert = m_shaders.emplace_back(std::make_unique<ShaderProgramGL43>(ShaderProgramGL43::Kind::vertex, shaders::fullbrightVert)).get();
	auto fullbrightFrag = m_shaders.emplace_back(std::make_unique<ShaderProgramGL43>(ShaderProgramGL43::Kind::fragment, shaders::fullbrightFrag)).get();
	m_pipelineFullbright = std::make_unique<ShaderPipelineGL43>(*fullbrightVert, *fullbrightFrag);

	auto axisVert = m_shaders.emplace_back(std::make_unique<ShaderProgramGL43>(ShaderProgramGL43::Kind::vertex, shaders::axisVert)).get();
	auto axisFrag = m_shaders.emplace_back(std::make_unique<ShaderProgramGL43>(ShaderProgramGL43::Kind::fragment, shaders::axisFrag)).get();
	m_pipelineAxis = std::make_unique<ShaderPipelineGL43>(*axisVert, *axisFrag);

	m_axisGroup = std::make_unique<RenderGroupGL43>(m_unitSquare);
}

RendererGL43::~RendererGL43(){}

void RendererGL43::resize(Nat16 w, Nat16 h){
	if(w > m_w || h > m_h){
		m_fb->resize(std::max(m_fb->width(), w), std::max(m_fb->height(), h));
	}

	m_w = w;
	m_h = h;
}

void RendererGL43::present(const Camera &cam){
	if(!m_cubeGroup){
		m_cubeGroup = createRenderGroup(m_unitCube);
		m_cubeGroup->setNumInstances(1);
	}

	glViewport(0, 0, m_w, m_h);

	glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
	glClearDepthf(0.f);
	glDepthFunc(GL_GREATER);
	glDepthRangef(1.f, 0.f);

	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);

	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	const float bgBrightness = 3.f/9.f;

	m_fb->use(true, true);

	GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(std::size(drawBufs), drawBufs);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glClearColor(bgBrightness, bgBrightness, bgBrightness, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto viewProj = cam.projection() * cam.view();
	auto invView = glm::inverse(cam.view());
	auto invProj = glm::inverse(cam.projection());

	if(m_drawAxis){
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		auto prog = m_shaders[2]->glHandle();

		auto axisViewProjLoc = glGetUniformLocation(prog, "viewProj");
		auto axisInvViewLoc = glGetUniformLocation(prog, "invView");
		auto axisInvProjLoc = glGetUniformLocation(prog, "invProj");
		glProgramUniformMatrix4fv(prog, axisViewProjLoc, 1, false, glm::value_ptr(viewProj));
		glProgramUniformMatrix4fv(prog, axisInvViewLoc, 1, false, glm::value_ptr(invView));
		glProgramUniformMatrix4fv(prog, axisInvProjLoc, 1, false, glm::value_ptr(invProj));

		m_pipelineAxis->use();

		m_axisGroup->draw();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	auto prog = m_shaders[0]->glHandle();

	auto viewProjLoc = glGetUniformLocation(prog, "viewProj");
	glProgramUniformMatrix4fv(prog, viewProjLoc, 1, false, glm::value_ptr(viewProj));

	m_pipelineFullbright->use();

	for(auto &&group : m_groups){
		group->draw();
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fb->glHandle());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBlitFramebuffer(0, 0, m_w, m_h, 0, 0, m_w, m_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

RenderGroupGL43 *RendererGL43::createRenderGroup(const shapes::Points &shape){
	auto ptr = std::make_unique<RenderGroupGL43>(shape);
	auto ret = ptr.get();

	auto insertIt = std::upper_bound(
		begin(m_groups), end(m_groups),
		ptr.get(),
		overload(
			[](void *lhs, const auto &rhs){ return lhs < rhs.get(); },
			[](const auto &lhs, void *rhs){ return lhs.get() < rhs; }
		)
	);

	auto res = m_groups.insert(insertIt, std::move(ptr));
	if(res != end(m_groups)){
		return ret;
	}
	else{
		return nullptr;
	}
}

bool RendererGL43::destroyRenderGroup(RenderGroup *group){
	auto resRange = std::equal_range(
		begin(m_groups), end(m_groups), group,
		overload(
			[](void *lhs, const auto &rhs){ return lhs < rhs.get(); },
			[](const auto &lhs, void *rhs){ return lhs.get() < rhs; }
		)
	);
	if(resRange.first == end(m_groups)) return false;

	m_groups.erase(resRange.first);
	return true;
}

namespace {
	GLenum shaderKindToGL(ShaderProgramGL43::Kind kind){
		using K = ShaderProgramGL43::Kind;
		switch(kind){
			case K::vertex: return GL_VERTEX_SHADER;
			case K::fragment: return GL_FRAGMENT_SHADER;
			case K::geometry: return GL_GEOMETRY_SHADER;
			default: throw std::runtime_error("invalid shader kind");
		}
	}

	UseProgramStageMask shaderKindToGLBit(ShaderProgramGL43::Kind kind){
		using K = ShaderProgramGL43::Kind;
		switch(kind){
			case K::vertex: return GL_VERTEX_SHADER_BIT;
			case K::fragment: return GL_FRAGMENT_SHADER_BIT;
			case K::geometry: return GL_GEOMETRY_SHADER_BIT;
			default: throw std::runtime_error("invalid shader kind");
		}
	}
}

ShaderProgramGL43::ShaderProgramGL43(ShaderProgramGL43::Kind kind_, const std::string &src)
	: m_kind(kind_)
{
	GLuint shad = glCreateShader(shaderKindToGL(kind_));

	const char *strs[] = { src.c_str() };
	const GLint strLens[] = { GLint(src.size()) };

	glShaderSource(shad, 1, strs, strLens);
	glCompileShader(shad);

	GLint res;
	glGetShaderiv(shad, GL_COMPILE_STATUS, &res);

	if(res != GLint(GL_TRUE)){
		glGetShaderiv(shad, GL_INFO_LOG_LENGTH, &res);
		std::string msg(res, '\0');
		glGetShaderInfoLog(shad, msg.size(), &res, msg.data());
		throw std::runtime_error(msg);
	}

	m_prog = glCreateProgram();

	glProgramParameteri(m_prog, GL_PROGRAM_SEPARABLE, GL_TRUE);

	glAttachShader(m_prog, shad);
	glLinkProgram(m_prog);
	glDeleteShader(shad);

	glGetProgramiv(m_prog, GL_LINK_STATUS, &res);

	if(res != GLint(GL_TRUE)){
		glGetProgramiv(m_prog, GL_INFO_LOG_LENGTH, &res);
		std::string msg(res, '\0');
		glGetProgramInfoLog(m_prog, msg.size(), &res, msg.data());
		throw std::runtime_error(msg);
	}
}

ShaderProgramGL43::~ShaderProgramGL43(){
	glDeleteProgram(m_prog);
}

ShaderPipelineGL43::ShaderPipelineGL43(){
	glCreateProgramPipelines(1, &m_pipeline);
}

ShaderPipelineGL43::~ShaderPipelineGL43(){
	glDeleteProgramPipelines(1, &m_pipeline);
}

void ShaderPipelineGL43::attach(const ShaderProgramGL43 &prog){
	glUseProgramStages(m_pipeline, shaderKindToGLBit(prog.kind()), prog.glHandle());
}

void ShaderPipelineGL43::use(){
	glBindProgramPipeline(m_pipeline);
}

namespace {
	inline GLenum pixelKindToGL(Texture2D::PixelKind kind){
		using K = Texture2D::PixelKind;
		switch(kind){
			case K::r8: return GL_R8;
			case K::rg8: return GL_RG8;
			case K::rgb8: return GL_RGB8;
			case K::rgba8: return GL_RGBA8;

			case K::r16: return GL_R16;
			case K::rg16: return GL_RG16;
			case K::rgb16: return GL_RGB16;
			case K::rgba16: return GL_RGBA16;

			case K::r16n: return GL_R16UI;
			case K::rg16n: return GL_RG16UI;
			case K::rgb16n: return GL_RGB16UI;
			case K::rgba16n: return GL_RGBA16UI;

			case K::r16i: return GL_R16I;
			case K::rg16i: return GL_RG16I;
			case K::rgb16i: return GL_RGB16I;
			case K::rgba16i: return GL_RGBA16I;

			case K::r16f: return GL_R16F;
			case K::rg16f: return GL_RG16F;
			case K::rgb16f: return GL_RGB16F;
			case K::rgba16f: return GL_RGBA16F;

			case K::r32n: return GL_R32UI;
			case K::rg32n: return GL_RG32UI;
			case K::rgb32n: return GL_RGB32UI;
			case K::rgba32n: return GL_RGBA32UI;

			case K::r32i: return GL_R32I;
			case K::rg32i: return GL_RG32I;
			case K::rgb32i: return GL_RGB32I;
			case K::rgba32i: return GL_RGBA32I;

			case K::r32f: return GL_R32F;
			case K::rg32f: return GL_RG32F;
			case K::rgb32f: return GL_RGB32F;
			case K::rgba32f: return GL_RGBA32F;

			case K::d16: return GL_DEPTH_COMPONENT16;
			case K::d32f: return GL_DEPTH_COMPONENT32F;
			case K::d24s8: return GL_DEPTH24_STENCIL8;

			default: throw std::runtime_error("invalid pixel kind"); // TODO: handle errors better
		}
	}

	inline GLenum pixelKindToGLType(Texture2D::PixelKind kind){
		using K = Texture2D::PixelKind;
		switch(kind){
			case K::r8:
			case K::rg8:
			case K::rgb8:
			case K::rgba8:
				return GL_UNSIGNED_BYTE;

			case K::r16:
			case K::rg16:
			case K::rgb16:
			case K::rgba16:
				return GL_UNSIGNED_SHORT;

			case K::r16n:
			case K::rg16n:
			case K::rgb16n:
			case K::rgba16n:
				return GL_UNSIGNED_SHORT;

			case K::r16i:
			case K::rg16i:
			case K::rgb16i:
			case K::rgba16i:
				return GL_SHORT;

			case K::r16f:
			case K::rg16f:
			case K::rgb16f:
			case K::rgba16f:
				return GL_FLOAT;

			case K::r32n:
			case K::rg32n:
			case K::rgb32n:
			case K::rgba32n:
				return GL_UNSIGNED_INT;

			case K::r32i:
			case K::rg32i:
			case K::rgb32i:
			case K::rgba32i:
				return GL_INT;

			case K::r32f:
			case K::rg32f:
			case K::rgb32f:
			case K::rgba32f:
				return GL_FLOAT;

			case K::d16: return GL_UNSIGNED_SHORT;
			case K::d32f: return GL_FLOAT;
			case K::d24s8: return GL_UNSIGNED_INT_24_8;

			default: throw std::runtime_error("invalid pixel kind"); // TODO: handle errors better
		}
	}

	void allocateTexGLStorage(Nat32 tex, Nat16 w, Nat16 h, Texture2D::PixelKind kind, const void *pixels, bool genMips){
		const auto numLevels = genMips ? 1 + std::floor(std::log2(std::max(w, h))) : 1;
		const auto texFormat = pixelKindToGL(kind);
		const auto texType = pixelKindToGLType(kind);

		glTextureStorage2D(tex, numLevels, texFormat, w, h);

		if(pixels){
			glTextureSubImage2D(tex, 0, 0, 0, w, h, texFormat, texType, pixels);
			if(genMips) glGenerateTextureMipmap(tex);
		}
	}
}

Texture2DGL43::Texture2DGL43(Nat16 w, Nat16 h, PixelKind kind, const void *pixels, bool genMips)
	: m_kind(kind), m_w(w), m_h(h)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_tex);

	allocateTexGLStorage(m_tex, w, h, kind, pixels, genMips);
}

Texture2DGL43::~Texture2DGL43(){
	glDeleteTextures(1, &m_tex);
}

FramebufferGL43::FramebufferGL43(Nat16 w, Nat16 h, std::vector<AttachmentInfo> attachments_)
	: Framebuffer(std::move(attachments_))
	, m_w(w)
	, m_h(h)
{
	auto numTexs = numAttachments();

	m_fbTexs.resize(numTexs);

	glCreateTextures(GL_TEXTURE_2D, numTexs, m_fbTexs.data());
	glCreateFramebuffers(1, &m_fb);

	bool hasDepth = false;
	Nat16 numColorAttachments = 0;

	for(std::size_t i = 0; i < numTexs; i++){
		auto tex = m_fbTexs[i];
		auto a = attachment(i); // TODO: sidestep nasty bounds check
		allocateTexGLStorage(tex, w, h, a.pixelKind, nullptr, false);

		if(a.kind == Framebuffer::AttachmentKind::color){
			glNamedFramebufferTexture(m_fb, GL_COLOR_ATTACHMENT0 + numColorAttachments, tex, 0);
			++numColorAttachments;
		}
		else{
			if(hasDepth){
				throw std::runtime_error("multiple depth attachments aren't supported with OpenGL");
			}

			auto attachmentType = a.pixelKind == Texture2D::PixelKind::d24s8 ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

			glNamedFramebufferTexture(m_fb, attachmentType, tex, 0);

			hasDepth = true;
		}
	}
}

FramebufferGL43::~FramebufferGL43(){
	glDeleteFramebuffers(1, &m_fb);
	glDeleteTextures(m_fbTexs.size(), m_fbTexs.data());
}

void FramebufferGL43::use(bool read, bool write){
	if(read && write){
		glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
	}
	else if(read){
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fb);
	}
	else if(write){
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fb);
	}
}

void FramebufferGL43::resize(Nat16 w, Nat16 h){
	if(!m_fbTexs.empty()){
		glDeleteTextures(m_fbTexs.size(), m_fbTexs.data());
	}

	auto numTexs = numAttachments();

	m_fbTexs.resize(numTexs);

	glCreateTextures(GL_TEXTURE_2D, numTexs, m_fbTexs.data());

	bool hasDepth = false;
	Nat16 numColorAttachments = 0;

	for(std::size_t i = 0; i < numTexs; i++){
		auto tex = m_fbTexs[i];
		auto a = attachment(i); // TODO: sidestep nasty bounds check
		allocateTexGLStorage(tex, w, h, a.pixelKind, nullptr, false);

		if(a.kind == Framebuffer::AttachmentKind::color){
			glNamedFramebufferTexture(m_fb, GL_COLOR_ATTACHMENT0 + numColorAttachments, tex, 0);
			++numColorAttachments;
		}
		else{
			if(hasDepth){
				throw std::runtime_error("multiple depth attachments aren't supported with OpenGL");
			}

			auto attachmentType = a.pixelKind == Texture2D::PixelKind::d24s8 ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

			glNamedFramebufferTexture(m_fb, attachmentType, tex, 0);

			hasDepth = true;
		}
	}
}
