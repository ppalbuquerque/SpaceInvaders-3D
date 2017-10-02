#pragma once

//Includes do tiny loader obj
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

//Includes do std
#include <vector>
#include <iostream>
using namespace std;

//includes do GLUT
#include <GL/glew.h>

//Include da classe Mesh
#include "Mesh.h"

//Include do SOIL para carregar as imagens das texturas
#include <SOIL.h>

//Include da Math para calculo do centroide
#include <math.h>

//Prototipos das funções
bool checkContains(int i, vector<int> material_ids);
GLint TextureFromFile(const char* path, string directory);
glm::vec3 CentroidCalc();

//Variaveis auxiliares para calcula do centroide
float minX, maxX, minY, maxY, minZ, maxZ;
bool first;

class Model 
{
	public:
		tinyobj::attrib_t attrib;
		vector<tinyobj::shape_t> shapes;
     	vector<tinyobj::material_t> materials;
     	string pathFile;
		vector<Mesh> meshes;
		string directory;
     	Model()
     	{
     		//Inicializando variaveis auxiliares para calculo do centroide
		 	minX = 0;
			maxX = 0;
			minY = 0;
			maxY = 0;
			minZ = 0;
			maxZ = 0;
			first = true;	 	
		}
		glm::vec3 loadModel(string path)
		{
			string err;
			bool ret = tinyobj::LoadObj(&this->attrib, &this->shapes, &this->materials, &err, path.c_str());
			if (!err.empty())
			{ 
				cerr << err << std::endl;
			}
			if (!ret) 
			{
  	  	  	  exit(1);
			}
			
			this->directory = path.substr(0, path.find_last_of('/'));
			
			this->indexMesh();
			
			return CentroidCalc();
		}
		void Draw(Shader shader)
		{
			for(GLuint i = 0;i < this->meshes.size(); i++)
			{
				this->meshes[i].Draw(shader);
			}
		}
	private:
		vector<Texture> textures_loaded;
		void indexMesh()
		{
			for(size_t s = 0; s < shapes.size(); s++)
			{
				Mesh mesh = this->ProcessMesh(shapes[s]);
				this->meshes.push_back(mesh);	
			}
			
			cout << "Menor X: " << minX << " Maior X: " << maxX << endl;
			cout << "Menor Y: " << minY << " Maior Y: " << maxY << endl;
			cout << "Menor Z: " << minZ << " Maior Z: " << maxZ << endl;
			
		}
		Mesh ProcessMesh(tinyobj::shape_t shape)
		{
			//Aqui é um vetor que vai guardar todos os vertices dessa mesh
			vector<Vertex> vertices;
			//Esse é um vetor que vai guardar todos as texturas
			vector<Texture> textures;
			
			//Aqui é um offset que vai ser usado para calcular o indice
			size_t index_offset = 0;
			for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				//Essa váriavel guarda a quantidade de vertices em cada face
				int fv = shape.mesh.num_face_vertices[f];
												
				//Vamos iterar sobre cada vertice de cada face F
				for(size_t v = 0; v < fv; v++)
				{	
					//Váriavel auxiliar que está guardando o vertice atual
					Vertex vertice;
					
					//Essa conta me retorna um indice auxiliar para a iteração
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					
					//Guardando cada vertice de posição
					glm::vec3 vector;
					vector.x = attrib.vertices[3*idx.vertex_index + 0];
					vector.y = attrib.vertices[3*idx.vertex_index + 1];
					vector.z = attrib.vertices[3*idx.vertex_index + 2];
					vertice.Position = vector;
					
					//Se é o primeiro teste
					if(first)
					{
						minX = vector.x;
						minY = vector.y;
						minZ = vector.z;				
						first = false;
					}
					
					//Pegando o maior X do modelo
					if(vector.x >= maxX)
						maxX = vector.x;
						
					//Pegando o menor X do modelo
					if(vector.x <= minX)
						minX = vector.x;
					
					//Pegando o maior Y do modelo
					if(vector.y >= maxY)
						maxY = vector.y;
					
					//Pegando o menor Y do modelo
					if(vector.y <= minY)
						minY = vector.y;
					
					//Pegando o maior Z do modelo
					if(vector.z >= maxZ)
						maxZ = vector.z;
					
					//Pegando o menor Z do modelo
					if(vector.z <= minZ)
						minZ = vector.z;	  		
					
					//Se o arquivo conter normais agnt indexa elas também
					if(attrib.normals.size() > 0)
					{
						vector.x = attrib.normals[3*idx.normal_index + 0];
						vector.y = attrib.normals[3*idx.normal_index + 1];
						vector.z = attrib.normals[3*idx.normal_index + 2];
						vertice.Normal = vector;
					}
					else
					{
						vector = glm::vec3();
						vertice.Normal = vector;
					}
					//Se o arquivo conter as coordenadas de textura nos indexamos
					if(attrib.texcoords.size() > 0)
					{
						glm::vec2 vector = glm::vec2();
						vector.x = attrib.texcoords[2*idx.texcoord_index + 0];
						vector.y = attrib.texcoords[2*idx.texcoord_index + 1];
						vertice.TexCoord = vector;
					}
					else
					{
						glm::vec2 vector = glm::vec2();
						vertice.TexCoord = vector;
					}
					vertices.push_back(vertice);
				}																					
				index_offset += fv;
			}
			//Processar as texturas aqui
			//Primeiro vamos pegar todos os ids unicos por mesh
			vector<int> material_ids;
			for(size_t i = 0; i < shape.mesh.material_ids.size(); i++)
			{
				if(!checkContains(shape.mesh.material_ids[i], material_ids))
					material_ids.push_back(shape.mesh.material_ids[i]);
			}
			 // Print para ver quais são os ids de cada shape
			/*			 
			for(size_t i = 0; i < material_ids.size(); i++)
				cout << material_ids[i] << endl;
			*/
			for(size_t i = 0; i < material_ids.size(); i++)
			{
				if(material_ids[i] >= 0)
  	  	  	    {	
					if((strlen(materials[material_ids[i]].diffuse_texname.c_str())) > 0)
					{			
						Texture diffuseMap = this->loadMaterialTexture(material_ids[i], "texture_diffuse", 0);
						textures.push_back(diffuseMap);
					}
					if(strlen( materials[material_ids[i]].specular_texname.c_str()) > 0)
					{ 	
						Texture specularMap = this->loadMaterialTexture(material_ids[i], "texture_specular", 1);
						textures.push_back(specularMap);
					}
					if((strlen(materials[material_ids[i]].ambient_texname.c_str())) > 0)
					{
						Texture ambientMap = this->loadMaterialTexture(material_ids[i], "texture_ambient", 2);
						textures.push_back(ambientMap);
					}
					if((strlen(materials[material_ids[i]].bump_texname.c_str())) > 0)
					{
						Texture bumpMap = this->loadMaterialTexture(material_ids[i], "texture_bump", 3);
						textures.push_back(bumpMap);
					}
	  	  	    }	
			}
			return Mesh(vertices, textures);
		}
        Texture loadMaterialTexture(int mat, string typeName, int type)
		{
			Texture texture;
			string str;
			GLboolean skip = false;
			for(GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if(type == 0)
					str = this->directory + '/' + materials[mat].diffuse_texname.c_str();
				else if (type == 1)
					str =  this->directory + '/' + materials[mat].specular_texname.c_str();
				else if (type == 2)
					str = this->directory + '/' + materials[mat].ambient_texname.c_str();
				else if(type == 3)
					str = this->directory + '/' + materials[mat].bump_texname.c_str();
				if(textures_loaded[j].path == str)
				{
					texture = textures_loaded[j];
					skip = true;
					break;
				}
			}
			if(!skip)
			{
				if(type == 0)
				{
					texture.id = TextureFromFile(materials[mat].diffuse_texname.c_str(), this->directory);
					texture.path = this->directory + '/' + materials[mat].diffuse_texname.c_str();
				}
				else if(type == 1)
				{
					texture.id = TextureFromFile(materials[mat].specular_texname.c_str(), this->directory);
					texture.path = this->directory + '/' + materials[mat].specular_texname.c_str();	
				}
				else if(type == 2)
				{
					texture.id = TextureFromFile(materials[mat].ambient_texname.c_str(), this->directory);
					texture.path = this->directory + '/' + materials[mat].ambient_texname.c_str();	
				}
				else if(type == 3)
				{
					texture.id = TextureFromFile(materials[mat].bump_texname.c_str(), this->directory);
					texture.path = this->directory + '/' + materials[mat].bump_texname.c_str();
				}
				texture.type = typeName;
				this->textures_loaded.push_back(texture);
			}								
			return texture;
		}	
		
};

GLint TextureFromFile(const char* path, string directory)
{
	//Gerando a id da textura e carregando os seus dados
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	//Associando a textura ao ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//PArametros de configuração
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}


bool checkContains(int i, vector<int> material_ids)
{
	for(int j = 0; j < material_ids.size(); j++)
	{
		if(i == material_ids[j])
			return true;
	}
	return false;
}

glm::vec3 CentroidCalc()
{ 	
	float dX = maxX + minX;
	float dY = maxY + minY;
	float dZ = maxZ + minZ;
	
	glm::vec3 centroid = glm::vec3(dX / 2, dY / 2, dZ / 2);
	
	return centroid;
}