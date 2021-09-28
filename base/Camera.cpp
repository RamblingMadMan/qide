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

#include <cmath>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "qide/Camera.hpp"

using namespace qide;

Camera::Camera(Perspective, float fovy, float aspect, float nearz, float farz)
	: m_proj(glm::perspectiveRH_ZO(fovy, aspect, nearz, farz))
	, m_view(Mat4(1.f))
	, m_pos(Vec3(0.f))
	, m_angles(Vec3(0.f, 0.f, 0.f))
	, m_forward(Vec3(0.f, 0.f, 1.f))
	, m_right(Vec3(1.f, 0.f, 0.f))
	, m_up(Vec3(0.f, 1.f, 0.f))
{}

const Mat4 &Camera::view() const noexcept{
	if(m_dirty){
		/*
		const auto pi2 = 2.f * M_PIf32;

		for(int axis = 0; axis < 3; axis++){
			if(m_angles[axis] <= -pi2){
				m_angles[axis] = std::fmod(m_angles[axis], -pi2);
			}
			else if(m_angles[axis] >= pi2){
				m_angles.x = std::fmod(m_angles[axis], pi2);
			}
		}
		//m_angles = glm::mod(m_angles, 2.f * float(3.1415f));
		*/

		auto pitch = glm::angleAxis(m_angles.x, Vec3(1.f, 0.f, 0.f));
		auto yaw   = glm::angleAxis(m_angles.y, Vec3(0.f, 1.f, 0.f));
		//auto roll  = glm::angleAxis(m_angles.z, Vec3(0.f, 0.f, 1.f));

		auto orientation = glm::normalize(pitch * yaw);
		auto rot = glm::mat4_cast(orientation);
		auto trans = glm::translate(glm::mat4(1.f), -m_pos);
		m_view = rot * trans;

		m_right   = orientation * Vec3(1.f, 0.f, 0.f);
		m_up      = orientation * Vec3(0.f, 1.f, 0.f);
		m_forward = orientation * Vec3(0.f, 0.f, 1.f);

		//m_view = glm::lookAt(m_pos, m_pos + m_forward, glm::vec3(0.f, 1.f, 0.f));

		m_dirty = false;
	}

	return m_view;
}

void Camera::setProjMat(Perspective, float fovy, float aspect, float nearz, float farz) noexcept{
	m_proj = glm::perspectiveRH_ZO(fovy, aspect, nearz, farz);
}
