#include "renderSys.h"
#include "controls.h"
#include "misc.h"
#include "game.h"
#include "include/gtk.h"

#include <time.h>


#define null NULL



int argc;
char** argv;
int windowW = 750;
int windowH = 750;
int cameraX = 0;
int cameraY = 0;
int squareSize = 10;
extern int seed;
GLFWwindow* window;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_size_callback(GLFWwindow* window, int width, int height);


int main(int argc,char** argv){

    if (!glfwInit()) return -1;

    glutInit(&argc,argv);   // glut used for text output
    
    gtk_init(&argc,&argv);

    menuInit(); // intilise the menu

    srand(time(0));

    seed = rand();  //generate seed which will be later used to store coordinates in a hashmap

    window = glfwCreateWindow(windowW, windowH, "GameOfLife", null, null);

    if (!window){
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetWindowSizeCallback(window,window_size_callback);
    setControls(window);

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