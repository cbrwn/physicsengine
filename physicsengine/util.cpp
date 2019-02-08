/* =================================
 *  Util
 *  Miscellaneous utility functions that don't fit anywhere else
 * ================================= */
#include "util.h"

#include <glm/ext.hpp>

glm::vec3 toVec3(Vector3 const& v)
{
	return glm::vec3(v.x, v.y, v.z);
}

glm::vec4 toVec4(Vector4 const& v)
{
	return glm::vec4(v.x, v.y, v.z, v.w);
}

glm::mat4 toMat4(Matrix4 const& m)
{
	return glm::make_mat4((float*)((Matrix4)m));
}

Matrix4 fromMat4(glm::mat4 const& m)
{
	Matrix4 result;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result[i][j] = m[i][j];
	return result;
}
