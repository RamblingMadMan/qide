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

#ifndef QIDE_SHAPES_HPP
#define QIDE_SHAPES_HPP 1

#include <vector>

#include "types.hpp"

namespace qide{
	class Shape{
		public:
			enum class FaceKind{
				lines, tris, triFan,

				count
			};

			virtual FaceKind faceKind() const noexcept = 0;

			virtual Nat32 numPoints() const noexcept = 0;
			virtual const Vec3 *vertices() const noexcept = 0;
			virtual const Vec3 *normals() const noexcept = 0;
			virtual const Vec2 *uvs() const noexcept = 0;

			virtual Nat32 numIndices() const noexcept = 0;
			virtual const Nat32 *indices() const noexcept = 0;
	};

	namespace shapes{
		class Points: public Shape{
			public:
				Points(
					FaceKind faceKind_,
					std::vector<Vec3> verts, std::vector<Vec3> norms, std::vector<Vec2> uvs_,
					std::vector<Nat32> indices_
				)
					: m_faceKind(faceKind_)
					, m_verts(std::move(verts))
					, m_norms(std::move(norms))
					, m_uvs(std::move(uvs_))
					, m_indices(std::move(indices_))
				{
					const auto numVerts = m_verts.size();
					assert((m_norms.size() == numVerts) && (m_uvs.size() == numVerts));
				}

				FaceKind faceKind() const noexcept override{ return m_faceKind; }

				Nat32 numPoints() const noexcept override{ return m_verts.size(); }
				const Vec3 *vertices() const noexcept override{ return m_verts.data(); }
				const Vec3 *normals() const noexcept override{ return m_norms.data(); }
				const Vec2 *uvs() const noexcept override{ return m_uvs.data(); }

				Nat32 numIndices() const noexcept override{ return m_indices.size(); }
				const Nat32 *indices() const noexcept override{ return m_indices.data(); }

			protected:
				FaceKind m_faceKind;
				std::vector<Vec3> m_verts;
				std::vector<Vec3> m_norms;
				std::vector<Vec2> m_uvs;
				std::vector<Nat32> m_indices;
		};

		class Quad: public Points{
			public:
				Quad(
					const Vec2 &tl, const Vec2 &tr,
					const Vec2 &bl, const Vec2 &br
				);
		};

		class Rect: public Quad{
			public:
				Rect(float w, float h);

				float width() const noexcept{ return m_verts[1].x * 2.f; }
				float height() const noexcept{ return m_verts[2].y * 2.f; }

				virtual float area() const noexcept{ return width() * height(); }
		};

		class Square: public Rect{
			public:
				explicit Square(float l): Rect(l, l){}

				float area() const noexcept{ return std::pow(width(), 2.f); }
		};

		class Cuboid: public Points{
			public:
				Cuboid(float w, float h, float d);

				float width() const noexcept{ return m_verts[1].x * 2.f; }
				float height() const noexcept{ return m_verts[2].y * 2.f; }
				float depth() const noexcept{ return m_verts[4].z * 2.f; }

				virtual float volume() const noexcept{ return width() * height() * depth(); }
		};

		class Cube: public Cuboid{
			public:
				explicit Cube(float l): Cuboid(l, l, l){}

				float volume() const noexcept override{ return std::pow(width(), 3.f); }
		};
	}
}

#endif // !QIDE_SHAPES_HPP
