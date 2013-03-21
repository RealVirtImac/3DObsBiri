/***************************************************************************
									Object.cpp
                             --------------------
    begin                : Feb 1 2013
    copyright            : (C) 2013 by R. Bertozzi & S. Bougeois
    email                : romain.bertozzi@gmail.com s.bougeois@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

/*!
 * \file Object.cpp
 * \brief Object that can be instanced in the scene
 * \author R. Bertozzi & S. Bougeois 
 */

#include "../include/Object.hpp"

Object::Object(const char* filename, const char* texture_path) throw (int)
{

	//~ Creating the scene of the model
	const aiScene* scene = aiImportFile(filename,aiProcess_FlipUVs | aiProcess_Triangulate);
	if(!scene)
	{
		throw(0);
	}
	
	//~ Parsing its meshes
	for (unsigned int index_mesh = 0; index_mesh < scene->mNumMeshes; ++index_mesh) 
	{
		//~ Selecting a mesh
		const aiMesh* mesh = scene->mMeshes[index_mesh];
		//~ Parsing its vertices
		for (unsigned int index_vertice = 0; index_vertice < mesh->mNumVertices; ++index_vertice) 
		{
			//~ Vertices
			if (mesh->HasPositions())
			{
				const aiVector3D* vp = &(mesh->mVertices[index_vertice]);
				m_vertices.push_back(glm::vec3(vp->x,vp->y,vp->z));
			}
			//~ Normals
			if (mesh->HasNormals())
			{
				const aiVector3D* vn = &(mesh->mNormals[index_vertice]);
				m_normals.push_back(glm::vec3(vn->x,vn->y,vn->z));
			}
			//~ UVs
			if (mesh->HasTextureCoords(0)) 
			{
				const aiVector3D* vt = &(mesh->mTextureCoords[0][index_vertice]);
				m_uvs.push_back(glm::vec2(vt->x,vt->y));
			}
		}
	}
	
	//~ Freeing the memory
	aiReleaseImport(scene);
	//~ Initializing model matrix
	m_model_matrix = glm::mat4(1.0);
	//~ Creating buffers
	create_buffers();
	//~ Load texture
	m_texture_path = texture_path;
	load_textures();
}

Object::~Object()
{
	if(m_texture_path != NULL)
	{
		glDeleteTextures(1, &m_diffuse_texture);
	}
	
	glDeleteBuffers(1,&m_object_vertices_vbo);
	glDeleteBuffers(1,&m_object_normals_vbo);
	glDeleteBuffers(1,&m_object_uvs_vbo);
	glDeleteVertexArrays(1, &m_object_vao);
}

void Object::create_buffers()
{
	// Generating Vertex Array
	glGenVertexArrays(1, &m_object_vao);
	// Generating buffers
	glGenBuffers(1, &m_object_vertices_vbo);
	glGenBuffers(1, &m_object_normals_vbo);
	glGenBuffers(1, &m_object_uvs_vbo);
	// Binding vao
	glBindVertexArray(m_object_vao);
	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_object_vertices_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);
    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, m_object_normals_vbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);
    // Uvs
    glBindBuffer(GL_ARRAY_BUFFER, m_object_uvs_vbo);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) , (void*)0);
    glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2), &m_uvs[0], GL_STATIC_DRAW);
	// Unbinding
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Object::load_textures()
{
	//~ Declarating values : width, height, components per pixel
	int w, h, comp;
	//~ Calling stbi
	unsigned char* diffuse = stbi_load(m_texture_path, &w, &h, &comp, 3);
	//~ Processing texture
	glGenTextures(1, &m_diffuse_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	stbi_image_free(diffuse);
}

glm::vec3 Object::computeBarycentre()
{
    glm::vec3 sum = glm::vec3(0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < m_vertices.size(); ++i)
    {
        sum += m_vertices.at(i);
    }

    return sum /= m_vertices.size();
}

float Object::computeAvgDistToBarycentre()
{
    glm::vec3 bar = computeBarycentre();
    float sumDist = 0;
    float avgDist = 0;

    for (unsigned int i = 0; i < m_vertices.size(); ++i)
    {
        sumDist += sqrt((m_vertices.at(i).x - bar.x)*(m_vertices.at(i).x - bar.x) +
                        (m_vertices.at(i).y - bar.y)*(m_vertices.at(i).y - bar.y) +
                        (m_vertices.at(i).z - bar.z)*(m_vertices.at(i).z - bar.z));
    }
    avgDist = sumDist / m_vertices.size();
    return avgDist;
}

float Object::computeStandardDeviation()
{
    glm::vec3 moyPt;
    moyPt = computeBarycentre();
    float sumDist = 0;
    float moyDist = 0;

    for (unsigned int i = 0; i < m_vertices.size(); ++i)
    {
        sumDist += sqrt((m_vertices.at(i).x - moyPt.x)*(m_vertices.at(i).x - moyPt.x) +
                        (m_vertices.at(i).y - moyPt.y)*(m_vertices.at(i).y - moyPt.y) +
                        (m_vertices.at(i).z - moyPt.z)*(m_vertices.at(i).z - moyPt.z));
    }
    moyDist = sumDist / m_vertices.size();

    float sumValMoinsMoy = 0;
    float var = 0;
    for (unsigned int i = 0; i < m_vertices.size(); ++i)
    {
        sumValMoinsMoy += pow(sqrt((m_vertices.at(i).x - moyPt.x)*(m_vertices.at(i).x - moyPt.x) +
                               (m_vertices.at(i).y - moyPt.y)*(m_vertices.at(i).y - moyPt.y) +
                               (m_vertices.at(i).z - moyPt.z)*(m_vertices.at(i).z - moyPt.z))
                          - moyDist, 2);
    }
    var = sumValMoinsMoy / m_vertices.size();

    return sqrt(var);
}

//~ Setters
void Object::set_model_matrix(const glm::mat4 input_matrix)
{
	m_model_matrix = input_matrix;
}

//~ Getters
GLuint Object::get_vao() const
{
	return m_object_vao;
}

unsigned int Object::get_size() const
{
	return m_vertices.size();
}

glm::mat4 Object::get_model_matrix() const
{
	return m_model_matrix;
}

GLuint Object::get_diffuse_texture() const
{
	return m_diffuse_texture;
}

const char* Object::get_texture_path() const
{
	return m_texture_path;
}
