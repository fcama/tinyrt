//
// Created by dante on 26/03/22.
//

#ifndef TINYRT_SRC_MODEL_H_
#define TINYRT_SRC_MODEL_H_

#include <vector>
#include <tinyobjloader/tiny_obj_loader.h>

#include "material.h"

class Model {
 public:
	Model() = default;

	std::vector<unsigned> indices_;
	std::vector<float> vertices_;
	std::vector<float> normals_;
	std::vector<tinyobj::material_t> tiny_materials_;
	std::vector<Material> materials_;
	std::vector<tinyobj::shape_t> shapes_;

	friend std::ostream& operator<<(std::ostream& os, const Model& m);
};

Model LoadObjFile(const char* filename, const char* basepath = nullptr, bool triangulate = true);

#endif //TINYRT_SRC_MODEL_H_
