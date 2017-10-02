#pragma once

//Std includes
#include <vector>

//GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Definindo alguns movimentos possiveis para a camera
enum Camera_Movement{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//Valores default para a camera
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

//Uma classe abstrata que manipula a camera
class Camera{
	public:
		//Atributos da camera
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		//Angulos de Euler
		GLfloat Yaw;
		GLfloat Pitch;
		//Opções da camera
		GLfloat MovementSpeed;
		GLfloat MouseSensitivity;
		GLfloat Zoom;
		
		//Construtor com vetores
		Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f), 
				GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
			this->Position = position;
			this->WorldUp = up;
			this->Yaw = yaw;
			this->Pitch = pitch;
			this->updateCameraVectors();			
		}
		
		//Construtor com valores escalares
		Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			this->Position = glm::vec3(posX, posY, posZ);
			this->WorldUp = glm::vec3(upX, upY, upZ);
			this->Yaw = yaw;
			this->Pitch = pitch;
			this->updateCameraVectors();
		} 
		
		//Retorna a matriz view calculando usando os angulos de Euler a função LookAt
		glm::mat4 GetViewMatrix(){
			return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
		}
		
		//Processa o input recebido pelo teclado
		void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime){
			GLfloat velocity = this->MovementSpeed * deltaTime;
			if(direction == FORWARD)
				this->Position += this->Front * velocity;
			if(direction == BACKWARD)
				this->Position -= this->Front * velocity;
			if(direction == LEFT)
				this->Position -= this->Right * velocity;
			if(direction == RIGHT)
				this->Position += this->Right * velocity;
		}
		
		//Processa o input do mouse
		void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true){
			
			xoffset *= this->MouseSensitivity;
			yoffset *= this->MouseSensitivity;
			
			this->Yaw += xoffset;
			this->Pitch += yoffset;
			
			//Garantindo que o pitch não saia dos limites
			if(constrainPitch){
				if(this->Pitch >89.0f)
					this->Pitch = 89.0f;
				if(this->Pitch < -89.0f)
					this->Pitch = -89.0f;
			}			
			
			this->updateCameraVectors();
		}
		
		//Processa o input do scroll do mouse
		void ProcessMouseScroll(GLfloat yoffset){
			if(this->Zoom >= 1.0f && this->Zoom <= 45.0f)
				this->Zoom -= yoffset;
			if(this->Zoom <= 1.0f)
				this->Zoom = 1.0f;
			if(this->Zoom >= 45.0f)
				this->Zoom = 45.0f;
		}
		
	private:
		
		//Calcula o vetor front da camera
		void updateCameraVectors(){
			
			glm::vec3 front;
			front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
			front.y = sin(glm::radians(this->Pitch));
			front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
			this->Front = glm::normalize(front);
			
			//Também recalculamos o vetor up e right
			this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
			this->Up = glm::normalize(glm::cross(this->Right, this->Front));
		}		  		
};