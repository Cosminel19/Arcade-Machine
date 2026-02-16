#pragma once

#include "utils/glm_utils.h"


namespace transform2D
{
    // Translate matrix
    inline glm::mat3 Translate(float tx, float ty)
    {
        return glm::transpose(
            glm::mat3(1, 0, tx,
                      0, 1, ty,
                      0, 0, 1)
            );
    }

    // Scale matrix
    inline glm::mat3 Scale(float sx, float sy)
    {
        // TODO(student): Implement the scaling matrix
        return glm::transpose(
            glm::mat3(sx, 0, 0,
                      0, sy, 0,
                      0, 0, 1)
            );

    }

    // Rotate matrix
    inline glm::mat3 Rotate(float radians)
    {
        // TODO(student): Implement the rotation matrix
        return glm::transpose(
            glm::mat3(cos(radians), -sin(radians), 0,
                      sin(radians), cos(radians), 0,
                      0, 0, 1)
            );

    }
}   // namespace transform2D
