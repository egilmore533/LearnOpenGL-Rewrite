#include "mesh.h"

Mesh::Mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures)
{
	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;

	setup_mesh();
}

void Mesh::draw(Shader shader, bool use_textures)
{
	if (use_textures)
	{
		int diffuse_num = 1;
		int specular_num = 1;
		int emission_num = 1;
		int normal_num = 1;
		int height_num = 1;

		for (int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string name;

			switch (m_textures[i].type)
			{
			case DIFFUSE_MAP:
				name = "diffuse_map" + diffuse_num++;
				break;
			case SPECULAR_MAP:
				name = "specular_map" + specular_num++;
				break;
			case EMISSION_MAP:
				name = "emission_map" + emission_num++;
				break;
			case NORMAL_MAP:
				name = "normal_map" + normal_num++;
				break;
			case HEIGHT_MAP:
				name = "height_map" + height_num++;
				break;
			}

			shader.set_float("material." + name, i);
			glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		}

		glActiveTexture(GL_TEXTURE0);
	}

	// draw mesh
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setup_mesh()
{
	// create the buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// we are now using vao
	glBindVertexArray(vao);

	/////////// load data into vertex buffers

	/////////// we are now using this vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/////////// load the vertices into vbo
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), &m_vertices[0], GL_STATIC_DRAW);

	/////////// we are now using this ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	/////////// load the indices into the ebo
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	/////////// end of loading data



	/////////// set the vertex attribute pointers

	/////////// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

	/////////// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));

	/////////// vertex texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texture_coordinates));

	/////////// end of vertex attribute pointers

	// we are no longer using vao
	glBindVertexArray(0);
}

