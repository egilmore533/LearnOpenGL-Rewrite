#ifndef __MODEL_H__
#define __MODEL_H__

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

/**
* @brief	loads a texture with SOIL and returns the texture's ID
* @param *filename		the filename of the texture
* @param &directory		the directory path to the model files	
*/
unsigned int load_texture_from_filepath(const char *filename, const std::string &directory);

/**
* @class
*/
class Model
{
public:

	/**
	* @brief	constructor	loads from the given filepath using load_model
	*			load_model recursivly traverses the aiScene object's nodes using process_mesh to load the actual data
	*			process_mesh creates the actual mesh and process_node will add it to the m_mesh list
	* @param *filepath		the path to the model, also used to gain the director path
	*/
	Model(char *filepath);

	/**
	* @brief	simple draw loops over each of the meshes to call their respective Draw function
	* @param shader			the shade to use to draw the Meshes.
	* @param use_textures	flag to turn off binding textures when drawing the meshes
	*/
	void draw(Shader shader, bool use_textures);

private:
	// Model Data
	std::vector<texture> m_textures_loaded;		/**< stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once. */
	std::vector<Mesh> m_meshes;					/**< list of every mesh in this Model */
	std::string m_directory;					/**< the directory that this model is inside of, this system will presume that all the textures are in the same directory */

	/**
	* @brief	called by the constructor to start the process of loading using Assimp
	*			Loads the aiScene and then saves the m_director from the given filepath.
	*			Then starts the recursive loading using process_node
	* @param filepath		the filepath to the model, also used to save the directory of the model
	*/
	void load_model(std::string filepath);

	/**
	* @brief	recursive function to load all the meshes in the model. 
	*			Processes this node's meshes first then goes into its children nodes.
	* @param *node		the aiNode object that we are currently working with
	* @param *scene		the aiScene object that contains all the data for the model, needed for process_mesh to get the mesh's material data
	*/
	void process_node(aiNode *node, const aiScene *scene);

	/**
	* @brief	loads all the mesh data from the Assimp object into my Mesh object and returns it
	* @param *mesh		contains all the vertex data (position, normals, texture coordinates) and the indices list (called faces in the aiMesh) 
	* @param *scene		contains all the material and texture data for the Mesh
	*/
	Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

	/**
	* @brief	iterates over all the texture locations of the given texture type, retrieves the texture's file path and checks all material 
	*			textures of a given type and loads the textures if they're not loaded yet. generates the texture and stores the information in a texture struct
	* @param *material		the material that contains the filepaths to the textures
	* @param type			the texture type, needed to get the texture filepath from the material
	* @param type_name		the texture type as an enumeration
	*/
	std::vector<texture> load_material_textures(aiMaterial *material, aiTextureType type, Texture_Type type_name);


};


#endif
