//
// Created by dante on 26/03/22.
//

#include "model.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Model& m)
{
	os << "# vertices: " << m.vertices_.size() / 3 << std::endl;
	os << "# indices: " << m.indices_.size() / 3 << std::endl;
	os << "# normals: " << m.normals_.size() / 3 << std::endl;
	os << "# materials: " << m.materials_.size() << std::endl;
	os << "# shapes: " << m.shapes_.size() << std::endl;

	return os;
}

Model LoadObjFile(const char* filename, const char* basepath, bool triangulate)
{
	std::cout << "Loading " << filename << std::endl;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename,
								basepath, triangulate);

	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "ERR: " << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return Model();
	}

	Model m;
	m.vertices_ = std::move(attrib.vertices);
	m.normals_ = std::move(attrib.normals);
	m.tiny_materials_ = std::move(materials);

	// Indices
	for (const auto &shape : shapes)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			size_t fnum = shape.mesh.num_face_vertices[f];

			// For each vertex in the face
			for (size_t v = 0; v < fnum; v++)
			{
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
				m.indices_.push_back(idx.vertex_index);
			}

			if (shape.name == "floor_Mesh" || shape.name ==  "frontWall_Mesh" || shape.name ==  "ceiling_Mesh")
			{
				Material mat(MatType::DIFFUSE,
							 &m.tiny_materials_.at(shape.mesh.material_ids[f]),
							 PatternType::CHECKERBOARD);
				m.materials_.push_back(mat);
			}
			else if (shape.name == "light_Mesh")
			{
				Material mat(MatType::EMISSIVE, &m.tiny_materials_.at(shape.mesh.material_ids[f]));
				m.materials_.push_back(mat);
			}
			else
			{
				Material mat(MatType::DIFFUSE, &m.tiny_materials_.at(shape.mesh.material_ids[f]));
				m.materials_.push_back(mat);
			}

			//m.materials.push_back(mat);

			index_offset += fnum;
		}
	}

	m.shapes_ = std::move(shapes);

	return m;
}