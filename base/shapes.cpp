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

#include <glm/gtx/normal.hpp>

#include "qide/shapes.hpp"

using namespace qide;

shapes::Quad::Quad(
	const Vec2 &tl, const Vec2 &tr,
	const Vec2 &bl, const Vec2 &br
)
	: Points(
		FaceKind::tris,
		{Vec3(bl, 0.f), Vec3(br, 0.f), Vec3(tr, 0.f), Vec3(tl, 0.f)},
		{Vec3(0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f)},
		{Vec2(0.f, 0.f), Vec2(1.f, 0.f), Vec2(1.f, 1.f), Vec2(0.f, 1.f)},
		{0, 1, 2, 0, 2, 3}
	)
{}

shapes::Rect::Rect(float w, float h)
	: Quad(
		Vec2(-w * 0.5f,  h * 0.5f),
		Vec2( w * 0.5f,  h * 0.5f),
		Vec2(-w * 0.5f, -h * 0.5f),
		Vec2( w * 0.5f, -h * 0.5f)
	)
{}

namespace {
	std::vector<Vec3> cuboidVertices(float w, float h, float d){
		std::vector<Vec3> ret;
		ret.reserve(4 * 6);

		const float hw = w * 0.5f;
		const float hh = h * 0.5f;
		const float hd = d * 0.5f;

		return {
			// front face
			{ -hw, -hh, -hd },
			{  hw, -hh, -hd },
			{  hw,  hh, -hd },
			{ -hw,  hh, -hd },

			// right side face
			{  hw, -hh, -hd },
			{  hw, -hh,  hd },
			{  hw,  hh,  hd },
			{  hw,  hh, -hd },

			// back face
			{  hw, -hh,  hd },
			{ -hw, -hh,  hd },
			{ -hw,  hh,  hd },
			{  hw,  hh,  hd },

			// left face
			{ -hw, -hh,  hd },
			{ -hw, -hh,	-hd },
			{ -hw,  hh, -hd },
			{ -hw,  hh,  hd },

			// top face
			{ -hw,  hh, -hd },
			{  hw,  hh, -hd },
			{  hw,  hh,  hd },
			{ -hw,  hh,  hd },

			// bottom face
			{ -hw, -hh,  hd },
			{  hw, -hh,  hd },
			{  hw, -hh, -hd },
			{ -hw, -hh,	-hd }
		};
	}

	std::vector<Vec3> cuboidNormals(){
		return {
			// front face
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f },

			// right face
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },

			// back face
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },

			// left face
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },

			// top face
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },

			// bottom face
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f }
		};
	}

	std::vector<Vec2> cuboidUvs(){
		std::vector<Vec2> ret;
		ret.reserve(4 * 6);

		for(int i = 0; i < 6; i++){
			ret.emplace_back(Vec2(0.f, 0.f));
			ret.emplace_back(Vec2(1.f, 0.f));
			ret.emplace_back(Vec2(1.f, 1.f));
			ret.emplace_back(Vec2(0.f, 1.f));
		}

		return ret;
	}

	std::vector<Nat32> cuboidIndices(){
		std::vector<Nat32> ret;
		ret.reserve(6 * 6);

		for(int i = 0; i < 6; i++){
			const auto idx = i * 4;

			ret.emplace_back(idx);
			ret.emplace_back(idx + 1);
			ret.emplace_back(idx + 2);

			ret.emplace_back(idx + 0);
			ret.emplace_back(idx + 2);
			ret.emplace_back(idx + 3);
		}

		return ret;
	}
}

shapes::Cuboid::Cuboid(float w, float h, float d)
	: Points(FaceKind::tris, cuboidVertices(w, h, d), cuboidNormals(), cuboidUvs(), cuboidIndices())
{}
