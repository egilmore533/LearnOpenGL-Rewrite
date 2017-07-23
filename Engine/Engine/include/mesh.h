#ifndef __MESH_H__
#define __MESH_H__

#include <vector>

#include <glad/glad.h>

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

/**
* @enum Texture_Type
* @brief Defines the different types of textures we can use in our Mesh
*/
enum Texture_Type
{
	DIFFUSE_MAP = 0,
	SPECULAR_MAP,
	EMISSION_MAP,
	NORMAL_MAP,
	HEIGHT_MAP
};

/**
* @struct contains the data for a vertex (triangle corner/point/)
*/
struct vertex 
{
	glm::vec3 position;					/**< position data for this vertex */
	glm::vec3 normal;					/**< normal data for this vertex (a vector perpendicular to this vertex, used for lighting) */
	glm::vec2 texture_coordinates;		/**< texture coordinates for this vertex (specifies what part of the texture image to sample from) */
};

/**
* @struct contains the data for a texture
*/
struct texture 
{
	unsigned int id;		/**< the id to reference the texture object  */
	Texture_Type type;		/**< the texture type, which is when sending the texture to the shader program */
	aiString path;			/**< the path of the texture to compare with other textures */
};

/**
* @class Mesh
* @brief	A simple Mesh class which will be used to load and draw Mesh data
*			uses indexed drawing and can handle multiple textures per mesh
*/
class Mesh 
{
public:

	/**
	* @brief	constructor gives the mesh all the necesarry data from the input parameters
	*			also calls setup_mesh to initialize the buffers
	* @param vertices	each vertex that makes up this Mesh
	* @param indices	the indices to draw in order (each index refers to an individual vertex inside m_vertices)
	* @param textures	all of the textures corresponding to this Mesh (diffuse, specular, and emission maps)
	*/
	Mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures);

	/**
	* @brief	draws the mesh with the given shader program by binding each texture and using glDrawElements
	* @param shader		the shader program to draw this Mesh
	*/
	void draw(Shader shader);

	//Mesh Data
	std::vector<vertex> m_vertices; 			/**< a vector of all the vertices in this Mesh, each containing position, normal, and texture_coordinates */
	std::vector<unsigned int> m_indices;		/**< a vector of all the vertex indices to be drawn (using glDrawElements) or this Mesh */
	std::vector<texture> m_textures;			/**< all the textures for this Mesh */

private:

	/**
	* @brief initializes the buffers for drawing and specifies the shader layout with vertex attribute pointers 
	*/
	void setup_mesh();

	//Mesh buffers
	unsigned int vao;		/**< the vertex array object with the attribute data */
	unsigned int vbo;		/**< the vertex buffer object with all the vertex data */
	unsigned int ebo;		/**< the element buffer object, which stores which vertices to draw */

};



#endif
