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
		L(" colorOut = mix(vec4(1.f, 1.f, 0.f, 1.f), texture(tex, uv_v), 0.5);")
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

		L("uniform mat4 viewProj;")

		L("void main(){")
		L("	gl_Position = vec4(pos, 1.0);")
		L("}")
	;

	constexpr char axisFrag[] =
		L("#version 430 core")

		L("layout(location = 0) out vec4 colorOut;")

		L("void main(){")
		L(" colorOut = vec4(1.f, 1.f, 0.f, 1.f);")
		L("}")
	;
#undef L
}

#endif // !QIDE_BASE_SHADERS_HPP
