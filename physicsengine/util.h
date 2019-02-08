/* =================================
 *  Util
 *  Miscellaneous utility functions that don't fit anywhere else
 * ================================= */
#pragma once

#include <glm/glm.hpp>
#include <vector3.h>
#include <vector4.h>
#include <matrix4.h>

// converts a Vector3 to a glm::vec3
glm::vec3 toVec3(Vector3 const& v);
// converts a Vector4 to a glm::vec4
glm::vec4 toVec4(Vector4 const& v);

// converts a Matrix4 to a glm::mat4
glm::mat4 toMat4(Matrix4 const& m);
// converts a glm::mat4 to a Matrix4
Matrix4 fromMat4(glm::mat4 const& m);
