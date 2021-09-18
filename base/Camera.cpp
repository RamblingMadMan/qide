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

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "qide/Camera.hpp"

using namespace qide;

Camera::Camera(Perspective, float fovy, float aspect, float nearz, float farz)
	: m_proj(glm::perspectiveRH(fovy, aspect, nearz, farz))
	, m_view(Mat4(1.f))
{}

const Mat4 &Camera::view() const noexcept{
	if(m_dirty){
		m_angles = glm::mod(m_angles, 2.f * float(M_PI));

		auto pitch = glm::angleAxis(m_angles.x, Vec3(1.f, 0.f, 0.f));
		auto yaw = glm::angleAxis(m_angles.y, Vec3(0.f, 1.f, 0.f));
		auto roll = glm::angleAxis(m_angles.z, Vec3(0.f, 0.f, 1.f));

		auto orientation = pitch * yaw * roll;

		auto forward = orientation * glm::vec3(0.f, 0.f, 1.f);
		auto up = orientation * glm::vec3(0.f, 1.f, 0.f);

		m_view = glm::lookAtRH(m_pos, m_pos + forward, up);

		m_dirty = false;
	}

	return m_view;
}

void Camera::setProjMat(Perspective, float fovy, float aspect, float nearz, float farz) noexcept{
	m_proj = glm::perspectiveRH(fovy, aspect, nearz, farz);
}
