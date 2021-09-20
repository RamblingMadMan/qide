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

#include <numbers>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "qide/Camera.hpp"

using namespace qide;

Camera::Camera(Perspective, float fovy, float aspect, float nearz, float farz)
	: m_proj(glm::perspective(fovy, aspect, nearz, farz))
	, m_view(Mat4(1.f))
{}

const Mat4 &Camera::view() const noexcept{
	using namespace std::numbers;

	if(m_dirty){
		m_angles = glm::mod(m_angles, 2.f * float(pi));

		auto yaw = glm::angleAxis(m_angles.y, Vec3(0.f, 1.f, 0.f));
		auto pitch = glm::angleAxis(m_angles.x, Vec3(1.f, 0.f, 0.f));
		auto roll = glm::angleAxis(m_angles.z, Vec3(0.f, 0.f, 1.f));

		auto orientation = yaw * pitch * roll;

		//glm::quat reverseOrient = glm::conjugate(orientation);
		//glm::mat4 rot = glm::mat4_cast(reverseOrient);
		//glm::mat4 trans = glm::translate(glm::mat4(1.0), m_pos);

		m_forward = orientation * Vec3(0.f, 0.f, 1.f);
		m_right = orientation * Vec3(1.f, 0.f, 0.f);
		m_up = orientation * Vec3(0.f, 1.f, 0.f);

		m_view = glm::lookAt(m_pos, m_pos + m_forward, m_up);
		//m_view = rot * trans;

		m_dirty = false;
	}

	return m_view;
}

void Camera::setProjMat(Perspective, float fovy, float aspect, float nearz, float farz) noexcept{
	m_proj = glm::perspective(fovy, aspect, nearz, farz);
}
