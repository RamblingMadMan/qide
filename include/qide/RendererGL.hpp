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

#ifndef QIDE_RENDERERGL_HPP
#define QIDE_RENDERERGL_HPP 1

#include <set>

#include "Renderer.hpp"

namespace qide{
	class Texture2DGL43: public Texture2D{
		public:
			Texture2DGL43(Nat16 w, Nat16 h, PixelKind kind, const void *pixels = nullptr, bool genMips = true);
			~Texture2DGL43();

			PixelKind pixelKind() const noexcept override{ return m_kind; }

			Nat16 width() const noexcept override{ return m_w; }
			Nat16 height() const noexcept override{ return m_h; }

			Nat32 glHandle() const noexcept{ return m_tex; }

		private:
			PixelKind m_kind;
			Nat16 m_w, m_h;
			Nat32 m_tex;
	};

	using Texture2DGL = Texture2DGL43;

	class FramebufferGL43: public Framebuffer{
		public:
			FramebufferGL43(Nat16 w, Nat16 h, std::vector<AttachmentInfo> attachments_);
			~FramebufferGL43();

			Nat32 attachmentTex(std::size_t idx) const{ return m_fbTexs.at(idx); }

			Nat16 width() const noexcept override{ return m_w; }
			Nat16 height() const noexcept override{ return m_h; }

			Nat32 glHandle() const noexcept{ return m_fb; }

			void use(bool read, bool write);

			void resize(Nat16 w, Nat16 h);

		private:
			Nat16 m_w, m_h;
			Nat32 m_fb;
			std::vector<Nat32> m_fbTexs;
	};

	using FramebufferGL = FramebufferGL43;

	class ShaderProgramGL43{
		public:
			enum class Kind{
				vertex, fragment, geometry,
				count
			};

			ShaderProgramGL43(Kind kind_, const std::string &src);
			~ShaderProgramGL43();

			Kind kind() const noexcept{ return m_kind; }

			Nat32 glHandle() const noexcept{ return m_prog; }

		private:
			Kind m_kind;
			Nat32 m_prog;
	};

	using ShaderProgramGL = ShaderProgramGL43;

	class ShaderPipelineGL43{
		public:
			ShaderPipelineGL43();

			template<typename ... Programs>
			explicit ShaderPipelineGL43(const ShaderProgramGL43 &prog0, Programs &&... progs)
				: ShaderPipelineGL43()
			{
				attach(prog0);
				(attach(std::forward<Programs>(progs)), ...);
			}

			~ShaderPipelineGL43();

			void attach(const ShaderProgramGL43 &prog);

			void use();

			Nat32 glHandle() const noexcept{ return m_pipeline; }

		private:
			Nat32 m_pipeline;
	};

	using ShaderPipelineGL = ShaderPipelineGL43;

	class RenderGroupGL43: public RenderGroup{
		public:
			explicit RenderGroupGL43(const shapes::Points &shape);
			~RenderGroupGL43();

			Nat32 numInstances() const noexcept override;
			void setNumInstances(Nat32 n) override;

			void draw();

			// return the vertex array object
			Nat32 glHandle() const noexcept{ return m_vao; }

		private:
			struct GLDrawElementsIndirectCommand;

			Nat32 m_vao, m_bufs[5];
			Nat32 m_drawKind;
			GLDrawElementsIndirectCommand *m_cmd;
	};

	class RendererGL43: public Renderer{
		public:
			RendererGL43(Nat16 w, Nat16 h, GLGetProcFn procFn, void *ctx = nullptr);
			~RendererGL43();

			Kind kind() const noexcept override{ return Kind::gl43; }

			Nat16 width() const noexcept override{ return m_w; }
			Nat16 height() const noexcept override{ return m_h; }

			void resize(Nat16 w, Nat16 h) override;
			void present(const Camera &cam) override;

			RenderGroupGL43 *createRenderGroup(const shapes::Points &shape) override;
			bool destroyRenderGroup(RenderGroup *group) override;

			void setDrawAxis(bool enabled){ m_drawAxis = enabled; }

		private:
			shapes::Square m_unitSquare;
			shapes::Cube m_unitCube;
			Nat16 m_w, m_h;
			std::unique_ptr<FramebufferGL43> m_fb;
			std::vector<std::unique_ptr<ShaderProgramGL43>> m_shaders;
			std::unique_ptr<ShaderPipelineGL43> m_pipelineFullbright;
			std::unique_ptr<ShaderPipelineGL43> m_pipelineAxis;
			std::vector<std::unique_ptr<RenderGroupGL43>> m_groups;
			bool m_drawAxis = false;
			std::unique_ptr<RenderGroupGL43> m_axisGroup;
			RenderGroupGL43 *m_cubeGroup;
	};

	using RendererGL = RendererGL43;
}

#endif // !QIDE_RENDERERGL_HPP
