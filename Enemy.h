#pragma once

//Include da classe Model para gerenciar o modelo 3d da nave
#include "Model.h"

//Includes do std
#include <string>
#include <vector>
#include <math.h>

//Includes do glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define delta 4.1808 

class Enemy
{
	public:
		Model ShipModel;
		Model ShotModel;
		int Lifes;
		vector<glm::vec3> Actual_Desloc_Shot;
		vector<glm::vec3> ShotOrigin;
		GLfloat radius;
		glm::vec3 Position;
		glm::vec3 Size;
		
		GLfloat delta;
		GLfloat actual_time;
		GLfloat last_time; 
		
		Enemy(Model model, Model model_bullet, GLfloat radius, GLfloat delta
					,glm::vec3 Position)
		{
			this->ShipModel = model;
			this->ShotModel = model_bullet;	
			this->Position = Position;
			
			this->radius = radius;
			
			this->shooting = false;
			
			this->delta = delta;
			last_time = glfwGetTime();
		}
		void Shot()
		{
			if(!isShooting())
				this->shooting = true;
			glm::vec3 positionShot = glm::vec3(0.0f,0.0f,0.0f);
			this->Actual_Desloc_Shot.push_back(positionShot);
			this->ShotOrigin.push_back(this->Position);   	
			last_time = glfwGetTime();			
		}
		bool isShooting()
		{
			return this->shooting;
		}
		void destroy()
		{
			for(int i = 0; i < this->Actual_Desloc_Shot.size(); i++)
			{
				if(this->Actual_Desloc_Shot[i].x <= 10.0f)
				{
					this->Actual_Desloc_Shot.pop_back();
					this->ShotOrigin.pop_back();
				}
			}
		}
		
		bool shotOrNot (){
			actual_time = glfwGetTime();
			if ((actual_time - last_time) > delta) {

				return true;
			}
			return false;
		}
	private:
		bool shooting;

};