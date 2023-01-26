#include "renderSys.h"
#include "controls.h"
#include "misc.h"
#include "game.h"
#include "include/gtk.h"

#include <pthread.h>
#include <unistd.h>


#define turnPerSec 1000/5


int windowW = 750;
int windowH = 750;
int cameraX = 0;
int cameraY = 0;
int squareSize = 10;
GLFWwindow* window;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_size_callback(GLFWwindow* window, int width, int height);
void* runGame(void* arg);


int main(int argc,char** argv){

	if (!glfwInit()) return -1;

	glutInit(&argc,argv);   // glut used for text output
	
	gtk_init(&argc,&argv);

	menuInit(); // intilise the menu

	window = glfwCreateWindow(windowW, windowH, "GameOfLife",NULL, NULL);

	if (!window){
		glfwTerminate();
		return -1;
	};

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	glfwSetWindowSizeCallback(window,window_size_callback);
	setControls(window);

	pthread_t gameLoop;

	pthread_create(&gameLoop, NULL, runGame , NULL);

	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	};

	glfwTerminate();
	return 0;
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0,0,width,height);
};

void window_size_callback(GLFWwindow* window, int width, int height){
	glfwGetWindowSize(window,&width,&height);
	windowW = width;
	windowH = height;
};

void* runGame(void* arg){

	while(1){
		usleep(turnPerSec);
		playTurn();
	};
};
