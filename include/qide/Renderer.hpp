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

#ifndef QIDE_RENDERER_HPP
#define QIDE_RENDERER_HPP 1

#include <memory>
#include <functional>
#include <list>

#include "Camera.hpp"
#include "shapes.hpp"

namespace qide{
	class RenderGroup{
		public:
			virtual ~RenderGroup();

			virtual Nat32 numInstances() const noexcept = 0;
			virtual void setNumInstances(Nat32 n) = 0;
	};

	class Renderer{
		public:
			enum class Kind{
				null,
				gl43,
				count
			};

			virtual ~Renderer();

			virtual Kind kind() const noexcept = 0;
			virtual Nat16 width() const noexcept = 0;
			virtual Nat16 height() const noexcept = 0;

			virtual void resize(Nat16 w, Nat16 h) = 0;
			virtual void present(const Camera &cam) = 0;

			virtual RenderGroup *createRenderGroup(const shapes::Points &shape) = 0;
			virtual bool destroyRenderGroup(RenderGroup *group) = 0;
	};

	using GLProcAddr = void(*)();
	using GLGetProcFn = std::function<GLProcAddr(const char*)>;

	class Texture2D{
		public:
			enum class PixelKind{
				r8, rg8, rgb8, rgba8,
				r16, rg16, rgb16, rgba16,
				r16n, rg16n, rgb16n, rgba16n,
				r16i, rg16i, rgb16i, rgba16i,
				r16f, rg16f, rgb16f, rgba16f,
				r32n, rg32n, rgb32n, rgba32n,
				r32i, rg32i, rgb32i, rgba32i,
				r32f, rg32f, rgb32f, rgba32f,

				d16, firstDepthFormat = d16,
				d32f,
				d24s8, lastDepthFormat = d24s8,

				count
			};

			virtual ~Texture2D();

			virtual PixelKind pixelKind() const noexcept = 0;
			virtual Nat16 width() const noexcept = 0;
			virtual Nat16 height() const noexcept = 0;
	};

	class Framebuffer{
		public:
			enum class AttachmentKind{
				depth, color,
				count
			};

			struct AttachmentInfo{
				AttachmentKind kind;
				Texture2D::PixelKind pixelKind;
			};

			explicit Framebuffer(std::vector<AttachmentInfo> attachments_);

			virtual Nat16 width() const noexcept = 0;
			virtual Nat16 height() const noexcept = 0;

			std::size_t numAttachments() const noexcept{ return m_attachments.size(); }

			const AttachmentInfo &attachment(std::size_t idx) const{
				return m_attachments.at(idx);
			}

		private:
			std::vector<AttachmentInfo> m_attachments;
	};
}

#endif // !QIDE_RENDERER_HPP
