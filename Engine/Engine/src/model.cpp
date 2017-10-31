#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"


Model::Model(char *filepath)
{
	load_model(filepath);
}

void Model::draw(Shader shader, bool use_textures)
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].draw(shader, use_textures);
	}
}

void Model::load_model(std::string filepath)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

	// check if the scene and the root node of the scene are not null and check one of its flags to see if the returned data is incomplete
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("ERROR::ASSIMP::%s\n", import.GetErrorString());
		return;
	}

	// niffty use of substr to get the directory of the model from the filepath
	m_directory = filepath.substr(0, filepath.find_last_of('/')); 

	// recursively traverse Assimp's nodes
	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene)
{
	// process all the meshes from this node first
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(process_mesh(mesh, scene));
	}

	// after we've processed all the meshes then recursively process the children nodes
	for (int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex v;
		glm::vec3 temp_vector; // a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class
		
		// position
		temp_vector.x = mesh->mVertices[i].x;
		temp_vector.y = mesh->mVertices[i].y;
		temp_vector.z = mesh->mVertices[i].z;
		v.position = temp_vector;

		// normals
		temp_vector.x = mesh->mNormals[i].x;
		temp_vector.y = mesh->mNormals[i].y;
		temp_vector.z = mesh->mNormals[i].z;
		v.normal = temp_vector;

		// texture coordinates
		// check if the mesh contains texture coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec3 vec;
			
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			v.texture_coordinates = vec;
		}
		else
		{
			v.texture_coordinates = glm::vec2(0.0f, 0.0f);
		}

		// we could save the tangent and bitangent here, but I don't need these or fully understand what they are for
		// so I'll skip them for now and stop here
		vertices.push_back(v);
	}

	// now walk through each of the mesh's faces (a face of a mesh is its triangle) and retrieve the corresponding vertex indices.
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// get all the indices of the face and store them in the indices vector
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process this mesh's material
	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. "<type>_map<N>" where N is 
	// a sequential number ranging from 1 to MAX_SAMPLER_NUMBER
	// the following list summarizes :
	// diffuse: "diffuse_map<N>"
	// specular: "specular_map<N>"
	// normal: "normal_map<N>"

	// 1. diffuse maps
	std::vector<texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, DIFFUSE_MAP);
	textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
	// 2. specular maps
	std::vector<texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, SPECULAR_MAP);
	textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	// 3. normal maps
	std::vector<texture> normal_maps = load_material_textures(material, aiTextureType_HEIGHT, NORMAL_MAP);
	textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
	// 4. height maps
	std::vector<texture> height_maps = load_material_textures(material, aiTextureType_AMBIENT, HEIGHT_MAP);
	textures.insert(textures.end(), height_maps.begin(), height_maps.end());

	return Mesh(vertices, indices, textures);
}

std::vector<texture> Model::load_material_textures(aiMaterial *material, aiTextureType type, Texture_Type type_name)
{
	std::vector<texture> textures;

	for (int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString s;
		material->GetTexture(type, i, &s);

		// check if texture already loaded, if so skip
		bool skip = false;
		for (int j = 0; j < m_textures_loaded.size(); j++)
		{
			if (std::strcmp(m_textures_loaded[j].path.C_Str(), s.C_Str()) == 0)
			{
				textures.push_back(m_textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			texture t;
			t.id = load_texture_from_filepath(s.C_Str(), m_directory);
			t.type = type_name;
			t.path = s;
			textures.push_back(t);
			m_textures_loaded.push_back(t);
		}
	}

	return textures;
}


unsigned int load_texture_from_filepath(const char *filename, const std::string &directory)
{
	std::string filepath = std::string(filename);
	filepath = directory + '/' + filepath;

	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, nr_components;
	unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nr_components, 0);
	if (data)
	{
		GLenum format;
		if (nr_components == 1)
			format = GL_RED;
		else if (nr_components == 3)
			format = GL_RGB;
		else if (nr_components == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		printf("Texture failed to load at path: %s\n", filepath);
		stbi_image_free(data);
	}

	return texture_id;
}