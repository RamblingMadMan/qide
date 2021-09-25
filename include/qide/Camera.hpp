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

#ifndef QIDE_CAMERA_HPP
#define QIDE_CAMERA_HPP 1

#include "types.hpp"

namespace qide{
	class Camera{
		public:
			struct Perspective{};
			//struct Orthographic{};

			Camera(Perspective, float fovy, float aspect, float nearz, float farz);

			const Mat4 &projection() const noexcept{ return m_proj; }
			const Mat4 &view() const noexcept;
			const Vec3 &position() const noexcept{ return m_pos; }
			const Vec3 &forward() const noexcept{ view(); return m_forward; }
			const Vec3 &right() const noexcept{ view(); return m_right; }
			const Vec3 &up() const noexcept{ view(); return m_up; }

			void translate(const Vec3 &v) noexcept{
				m_pos += v;
				m_dirty = true;
			}

			void rotate(const Vec3 &axis, float radians) noexcept{
				m_angles += axis * radians;
				m_dirty = true;
			}

			void setProjMat(const Mat4 &proj) noexcept{ m_proj = proj; }
			void setProjMat(Perspective, float fovy, float aspect, float nearz, float farz) noexcept;

			void setPosition(const Vec3 &p) noexcept{
				m_pos = p;
				m_dirty = true;
			}

		private:
			Mat4 m_proj;
			mutable Mat4 m_view;
			Vec3 m_pos;
			mutable Vec3 m_angles, m_forward, m_right, m_up;
			mutable bool m_dirty = true;
	};
}

#endif // !QIDE_CAMERA_HPP
