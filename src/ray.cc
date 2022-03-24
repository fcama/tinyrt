//
// Created by dante on 24/03/22.
//

#include "ray.h"

glm::vec3 Ray::At(float t) const {
	return o_ + t_*d_;
}
