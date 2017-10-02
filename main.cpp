// Std. Includes
#include <string>

//Includes do fmod
#include <windows.h>    
#include <stdio.h>          
#include <fmod/fmod.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Ship_Player.h"
#include "Enemy.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement(Ship_Player *player);
void DrawModel(Model model, Shader shader, glm::mat4 ModelMat);
bool checkSpheresCollision(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat r1, 
						     GLfloat x2, GLfloat y2, GLfloat z2, GLfloat r2);
bool checkSquareCollision(GLfloat x1, GLfloat y1, GLfloat sx1, GLfloat sy1, 
						GLfloat x2, GLfloat y2, GLfloat s2x, GLfloat s2y);
GLuint loadTexture(GLchar* path);
GLuint loadCubemap(vector<const GLchar*> faces);
void InitFMOD ( );
void updatePlayerStatus(Ship_Player *player, Shader shader);
void updatePlayerShot(Ship_Player *player, Shader shader);
void display_message(int s);
void updateEnemyStatus(Shader shader, Ship_Player *player);
void set();
void updateGameStatus(Ship_Player *player);



// Camera
Camera camera(glm::vec3(1.7f, 0.02f, 0.02f), glm::vec3(0.0f, 1.0f, 0.0f), -182.25, 3);
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


//Vetores auxiliares
vector<Enemy> aliens;

//Vetores que representam direções
const glm::vec3 RIGHTC = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 LEFTC = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 UP = glm::vec3(0.0, 1.0f, 0.0f);
const glm::vec3 DOWN = glm::vec3(0.0f, -1.0f, 0.0f);
const glm::vec3 FRONTC = glm::vec3(-1.0, 0.0f, 0.0f);
const glm::vec3 BACKC = glm::vec3(1.0, 0.0f, 0.0f);

//Função main, a partir daqui nós criamos a nossa tela GLUT e rodamos o nosso game loop
int main()
{
	//Inicializando o GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//Criando a tela
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Space Invasors", NULL, NULL);
	glfwMakeContextCurrent(window);

	//Setando as funções de callback para os dispositivos de entrada
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Setando que a tela captura o cursor do mouse, fazendo com que ele não apareça na tela
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Inicializando o GLEW para montar o ponteiro para as funções do OpenGL
	glewExperimental = GL_TRUE;
	glewInit();

	//Definindo o tamanho da viewport, lembrando que aqui é o tamanho da area visivel e não da janela
	glViewport(0, 0, screenWidth, screenHeight);

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST); 

	
	//Setando e compilando o nosso shader com a ajuda da nossa clase Shader
	Shader shader("VertexShader.vs", "FragmentShader.frag");
	Shader skyboxShader("skyboxVertex.vs", "skyboxFragment.frag");
	
	//Carregando os models
	Model nave_player = Model();
	Model bullet = Model();
	Model alien_ship = Model();
	nave_player.loadModel("Interceptor/alieninterceptorflying.obj");
	bullet.loadModel("bullet.obj");
	alien_ship.loadModel("shuttle/shuttle.obj");
	
	//Carregar o player
	Ship_Player player = Ship_Player(nave_player,bullet,0.67f / 6);
	
	InitFMOD();
	
	//Carregar o inimigo
	Enemy alien = Enemy(alien_ship,bullet, 0.325f / 6, 4.1808f, glm::vec3(-2.0f, 0.0f, 0.0f));
	Enemy alien2 = Enemy(alien_ship,bullet, 0.325f / 6, 3.1808f, glm::vec3(-2.0f, 0.0f, 1.0f));
	Enemy alien3 = Enemy(alien_ship,bullet, 0.325f / 6, 4.1808f, glm::vec3(-2.0f, 0.0f, -1.0f));
	Enemy alien4 = Enemy(alien_ship,bullet, 0.325f / 6, 2.1808f, glm::vec3(-2.0f, 0.5f, 0.5f));
	Enemy alien5 = Enemy(alien_ship,bullet, 0.325f / 6, 5.1808f, glm::vec3(-2.0f, 0.5f, -0.5f));
	Enemy alien6 = Enemy(alien_ship,bullet, 0.325f / 6, 3.1808f, glm::vec3(-2.0f, -0.5f, 0.5f));
	Enemy alien7 = Enemy(alien_ship,bullet, 0.325f / 6, 2.1808f, glm::vec3(-2.0f, -0.5f, -0.5f));
	aliens.push_back(alien);
	aliens.push_back(alien2);
	aliens.push_back(alien3);
	aliens.push_back(alien4);
	aliens.push_back(alien5);
	aliens.push_back(alien6);
	aliens.push_back(alien7);
	
	//Criando a nossa matriz de view para uma camera estatica
	glm::mat4 View = camera.GetViewMatrix();

   	// Game loop
	///////////////////------------SKYBOX TEST----------------///////////////////
	
	//Vertices do skybox
	 GLfloat skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	
	//Setando o VAO do skybox
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    // Cubemap (Skybox)
    vector<const GLchar*> faces;
    faces.push_back("skybox/right.jpg");
    faces.push_back("skybox/left.jpg");
    faces.push_back("skybox/top.jpg");
    faces.push_back("skybox/bottom.jpg");
    faces.push_back("skybox/back.jpg");
    faces.push_back("skybox/front.jpg");
    GLuint cubemapTexture = loadCubemap(faces);
    	
	while(!glfwWindowShouldClose(window))
	{
		//Calculando o tempo de cada frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Checando se houve algum input
		glfwPollEvents();

		//Processa o input de acordo com o status
		Do_Movement(&player);

		//Limpando o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		//Desenhando a skybox
        glDepthMask(GL_FALSE);
        skyboxShader.Use();		
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        //Cube do skybox
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
		
		updatePlayerStatus(&player, shader);
		updatePlayerShot(&player, shader);
		updateEnemyStatus(shader, &player);
		updateGameStatus(&player);
		
		//Trocando os buffers
		glfwSwapBuffers(window);
								 
	}	

	//glfwTermin
	return 0;

}

#pragma region "User input"

//Realiza alteração na posição do player
void Do_Movement(Ship_Player *player)
{
	glm::vec3 Increment = glm::vec3();
	if(keys[GLFW_KEY_W])
	{
		//camera.ProcessKeyboard(FORWARD, deltaTime);
		Increment = UP * 0.5f * deltaTime;
		player->Position += Increment;
	}
	if(keys[GLFW_KEY_S])
	{
		//camera.ProcessKeyboard(BACKWARD, deltaTime);
		Increment = DOWN * 0.5f * deltaTime;
		player->Position += Increment;
	}
	if(keys[GLFW_KEY_A])
	{
		//camera.ProcessKeyboard(LEFT, deltaTime);
		Increment = LEFTC * 0.5f * deltaTime;
		player->Position += Increment;
	}
	if(keys[GLFW_KEY_D])
	{
		//camera.ProcessKeyboard(RIGHT, deltaTime);
		Increment = RIGHTC * 0.5f * deltaTime;
		player->Position += Increment;
	}
	if(keys[GLFW_KEY_SPACE])
		player->Shot();
		
}

// É chamada a cada vez que uma tecla é pressionada
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if(action == GLFW_PRESS)
		keys[key] = true;
	else if(action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void DrawModel(Model ourModel, Shader shader, glm::mat4 ModelMat)
{
	shader.Use();
	
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(ModelMat));
	ourModel.Draw(shader);
}


bool checkSpheresCollision(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat r1, 
						     GLfloat x2, GLfloat y2, GLfloat z2, GLfloat r2)
{
   return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) <= (r1+r2)*(r1+r2));
}

bool checkSquareCollision(GLfloat x1, GLfloat y1, GLfloat sx1, GLfloat sy1, 
						GLfloat x2, GLfloat y2, GLfloat sx2, GLfloat sy2) 
{
    //Colisão x
    bool collisionX = x1 + sx1 >= x2 && x2 + sx2 >= x1;
    //Colisão y 
    bool collisionY = y1 + sy1 >= y2 && y2 + sy2 >= y1;
    // Collision only if on both axes
    return collisionX && collisionY;
}

GLuint loadCubemap(vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width,height;
    unsigned char* image;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void InitFMOD ( )
{
    FSOUND_SAMPLE * son = NULL;                     // NULL is significant
    int channel;                                    // sound channel

    printf ( "Initializations ...\n" );
    FSOUND_SetOutput ( FSOUND_OUTPUT_DSOUND );      // DirectSound
    FSOUND_SetDriver ( 0 );                         // Sound Driver (default 0)
    FSOUND_SetMixer ( FSOUND_MIXER_AUTODETECT );    // Mixer
    FSOUND_Init ( 44100, 32, 0 );                   // 44.1 kHz and 32 channels
    
    // Loading of our sound (2D)
    printf ( "Loading ...\n" );
    son = FSOUND_Sample_Load ( FSOUND_FREE, "sound.mp3", FSOUND_HW2D, 0, 0);
    channel = FSOUND_PlaySoundEx ( FSOUND_FREE, son, NULL, FALSE );
    
    // Some informations
    printf ( "Playing ...\n" );
    printf ( "Frequence : %d Hz ", FSOUND_GetOutputRate ( ) );
    printf ( "OR %d Hz\n", FSOUND_GetFrequency( channel ));
}

void updatePlayerStatus(Ship_Player *player, Shader shader)
{
	//Desenhando player 
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, player->Position);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	DrawModel(player->ShipModel, shader, model);
}

void updatePlayerShot(Ship_Player *player, Shader shader)
{
	if(player->isShooting())
		{
			for(int i = 0; i < player->Actual_Desloc_Shot.size(); i++)
			{
				glm::mat4 model2 = glm::mat4();
				player->Actual_Desloc_Shot[i] += FRONTC * 0.5f;
				model2 = glm::translate(model2, player->ShotOrigin[i]);
				model2 = glm::translate(model2, player->Actual_Desloc_Shot[i]);
				model2 = glm::rotate(model2, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
				model2 = glm::scale(model2, glm::vec3(0.01f, 0.01f, 0.01f));
				DrawModel(player->ShotModel, shader, model2);
				glm::vec3 BulletPos = player->ShotOrigin[i] + player->Actual_Desloc_Shot[i];
				for(int j = 0; j < aliens.size(); j++)
				{
					if(checkSpheresCollision(BulletPos.z, BulletPos.y, BulletPos.x, 0.24f / 4 , 
						     aliens[j].Position.z, aliens[j].Position.y, aliens[j].Position.x, aliens[j].radius))
					{ 
						cout << "Entrou" << endl;
						player->Actual_Desloc_Shot.pop_back();
						player->ShotOrigin.pop_back();
						aliens.erase(aliens.begin() + j);
						break;			 	
					}   
				}
				player->destroy();
			}
		}
}

void updateEnemyStatus(Shader shader, Ship_Player *player)
{
	glm::mat4 Model;
	//Desenhando todos os inimigos
		for(int ind = 0; ind < aliens.size(); ind++)
		{
			if(aliens[ind].shotOrNot())
				aliens[ind].Shot();
			
			Model = glm::mat4();
			Model = glm::translate(Model, aliens[ind].Position);
			Model = glm::rotate(Model, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Model = glm::scale(Model,glm::vec3(0.002f, 0.002f, 0.002f));
			
			DrawModel(aliens[ind].ShipModel, shader, Model);
			
			if(aliens[ind].isShooting())
			{
				for(int i = 0; i < aliens[ind].Actual_Desloc_Shot.size(); i++)
				{
					glm::mat4 model2 = glm::mat4();
					aliens[ind].Actual_Desloc_Shot[i] += BACKC * 0.5f;
					model2 = glm::translate(model2, aliens[ind].ShotOrigin[i]);
					model2 = glm::translate(model2, aliens[ind].Actual_Desloc_Shot[i]);
					model2 = glm::rotate(model2, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
					model2 = glm::scale(model2, glm::vec3(0.01f, 0.01f, 0.01f));
					DrawModel(aliens[ind].ShotModel, shader, model2);
					glm::vec3 BulletPos = aliens[ind].ShotOrigin[i] + aliens[ind].Actual_Desloc_Shot[i];
					if(checkSpheresCollision(BulletPos.z, BulletPos.y, BulletPos.x, 0.24f / 4 , 
			     	 	 player->Position.z, player->Position.y, player->Position.x, player->radius))
						{ 
						   player->Lifes -= 1;
						}
					aliens[ind].destroy();		   
	       		}
			}  	
		}
		
}

void updateGameStatus(Ship_Player *player)
{
	//Checar se acabaram os inimigos
	if(aliens.size() == 0)
		exit(0);
	
	if(player->Lifes == 0)
		exit(1);
}

#pragma endregion
