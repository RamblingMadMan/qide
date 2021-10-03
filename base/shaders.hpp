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

#ifndef QIDE_BASE_SHADERS_HPP
#define QIDE_BASE_SHADERS_HPP 1

namespace qide::shaders{
#define L(s) s "\n"
	constexpr char fullbrightVert[] =
		L("#version 430 core")

		L("out gl_PerVertex")
		L("{")
		L("	vec4 gl_Position;")
		L("	float gl_PointSize;")
		L("	float gl_ClipDistance[];")
		L("};")

		L("layout(location = 0) in vec3 pos;")
		L("layout(location = 1) in vec3 norm;")
		L("layout(location = 2) in vec2 uv;")

		L("out vec3 norm_v;")
		L("out vec2 uv_v;")

		L("uniform mat4 viewProj;")

		L("void main(){")
		L("	norm_v = norm;")
		L("	uv_v = uv;")
		L("	gl_Position = viewProj * vec4(pos, 1.0);")
		L("}")
	;

	constexpr char fullbrightFrag[] =
		L("#version 430 core")

		L("layout(location = 0) out vec4 colorOut;")

		L("in vec3 norm_v;")
		L("in vec2 uv_v;")

		L("uniform sampler2D tex;")

		L("void main(){")
		L(" colorOut = vec4(1.0);")
		L("}")
	;

	constexpr char axisVert[] =
		L("#version 430 core")

		L("out gl_PerVertex")
		L("{")
		L("	vec4 gl_Position;")
		L("	float gl_PointSize;")
		L("	float gl_ClipDistance[];")
		L("};")

		L("layout(location = 0) in vec3 pos;")
		L("layout(location = 2) in vec2 uv;")

		L("uniform vec3 camPos;")
		L("uniform mat4 viewProj;")

		L("out vec2 uv_v;")

		L("void main(){")
		L("	uv_v = uv;")
		L("	gl_Position = viewProj * vec4(camPos.x, 0.0, camPos.z, 1.0);")
		L("}")
	;

	constexpr char axisFrag[] =
		L("#version 430 core")

		L("layout(location = 0) out vec4 colorOut;")

		L("in vec2 uv_v;")

		L("void main(){")
		L("	vec2 d = mod(uv_v, vec2(1.0/16.0));")
		L("	vec2 lum = vec2(2.0/3.0);")
		L("	lum.x *= float(d.x < 0.00001);")
		L("	lum.y *= float(d.y < 0.00001);")
		L(" colorOut = vec4(vec3(lum, 1.0), 1.0);")
		L("}")
	;
#undef L
}

#endif // !QIDE_BASE_SHADERS_HPP
