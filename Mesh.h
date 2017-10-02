#pragma once

//includes do std;
#include <iostream>
#include <vector>
using namespace std;

//Include de algumas funcionalidades do GLUT
#include <GL/glew.h>

//Include do glm para os vetores
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Incluindo o shader
#include "Shader.h"

struct Vertex
{
	//A Posição de cada vertice
	glm::vec3 Position;
	//A normal dele
	glm::vec3 Normal;
	//A coordenada da textura
	glm::vec2 TexCoord;
};

//Definição de como a textura está armazenada
struct Texture
{
	//Identificação da textura
	GLuint id;   	
	//O tipo dela se é difusa, especular, entre outras
	string type;
	//O caminho até o arquivo de textura
	string path;
};

class Mesh
{
	public:
		vector<Vertex> vertices;
		vector<Texture> textures;
		
		Mesh(vector<Vertex> vertices, vector<Texture> textures)
		{
			 this->vertices = vertices;	
			 this->textures = textures;
			 
			 this->setupMesh();
		}
		void Draw(Shader shader)
		{
			//Associe as texturas apropriadas
			GLuint diffuseNr = 1;
			GLuint specularNr = 1;
			GLuint ambientNr = 1;
			GLuint bumpNr = 1;
			for(GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); //Ativa a textura certa antes de associar;
				//Recupera o numero da textura(o n em diffuse_textureN)
				stringstream ss;
				string number;
				string name = this->textures[i].type;
				if(name == "texture_diffuse")
					ss << diffuseNr++; // Transfere o Gluint para a stream
				else if(name == "texture_specular")
					ss << specularNr++; //Transfere o GLuint para a stream
				else if(name == "texture_ambient")
					ss << ambientNr++;
				else if(name == "texture_bump")
					ss << bumpNr++;
				number = ss.str();
				//Agora nós vamos setar o sample com a textura correta
				glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
				//E finalmente associamos a textura
				glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
			}
			//Aqui nós vamos setar o valor da "brilhosidade" de cada mesh para algo padrão
			glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);
			
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES,0,this->vertices.size());
			glBindVertexArray(0);
			
			// Apenas setando tudo para o padrão de novo
			for(GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	private:
		GLuint VBO, VAO;
		void setupMesh()
		{
			 //Gera o buffer para o VAO
			 glGenVertexArrays(1, &this->VAO);
			 //Gera o buffer para o VBO
			 glGenBuffers(1, &this->VBO);
			 
			 //Associando o meu VAO
			 glBindVertexArray(this->VAO);
			 //Associando o meu buffer atual ao VBO
			 glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			 
			 //Passa os dados dos vertices para o buffer
			 glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
				&this->vertices[0], GL_STATIC_DRAW);
			
				
			//Aqui eu começo a descrever meu buffer	
					
			//Posição dos vertíces
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)0);
			
			//Normais dos vertices
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, sizeof(Vertex),
						(GLvoid*)offsetof(Vertex, Normal));
						
			//Coordenadas das texturas
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						(GLvoid*)offsetof(Vertex, TexCoord));
						
			glBindVertexArray(0);		
		}	
};