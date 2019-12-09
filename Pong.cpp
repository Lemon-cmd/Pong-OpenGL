#include <iostream>
#include <string>
#include <math.h>
#include <sstream>

#ifdef __APPLE__
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <OpenGL/glext.h>

#else
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#pragma comment(lib, "glew32.lib") 
#endif
#define PI 3.14159265358979323846

#include "getbmp.h"


//Define W, R and S keys
#define VK_W 0x57
#define VK_R 0x52
#define VK_S 0x53

using namespace std;

bool hitR = false;
bool hitL = false;

//60 fps
int interval = 1000 / 60;

int score_left = 0;
int score_right = 0;

// paddle metadata 
int paddle_width = 1;
int paddle_height = 5;
float paddle_speed = 0.3;

// left pad position
float paddle_left_x = -9.0f;
float paddle_left_y = -1.0f;

// right pad position
float paddle_right_x = 8;
float paddle_right_y = -1;


// ball
float ball_pos_x = 0;
float ball_pos_y = 0;

float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;

float ball_width = .5;
float ball_height = .5;

float ball_speed = .3;

GLdouble radius = 1;

// Set lighting intensity and color
GLfloat qaAmbientLight[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat qaDiffuseLight[] = { 1.0, 0.8, 0.8, 1.0 };
GLfloat qaSpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };
// Light source position
GLfloat qaLightPosition[] = { 0.5, 0, -3.5, 0.5 };

static unsigned int texture[2]; // Array of texture ids.

//control pts
GLfloat ctrlpts[5][3] = {
		{ 0, 0, 0.0}, { 2.0, 0.0, 0.0},
		{0.0, -4.0, 0.0}, {2.0, 4.0, 0.0} , {1, 2, 0} };


void loadWood()
{
	// Create texture object texture[0]. 
	// Local storage for bmp image data.
	BitMapFile* image[1];

	// Load the image.
	image[0] = getbmp("wood.bmp");
	
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	
	// Specify image data for currently active texture object.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void loadBanana()
{
	// Create texture object texture[0]. 
	// Local storage for bmp image data.
	BitMapFile* image[1];

	// Load the image.
	image[0] = getbmp("banana.bmp");

	glBindTexture(GL_TEXTURE_2D, texture[1]);

	// Specify image data for currently active texture object.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void drawText(float x, float y, std::string text) {
	// a method for outputting text
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}

void Rect(float x, float y, float width, float height, int texture) {
	if (texture != -1) {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texture);
		//Method for drawing paddle and ball
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2f(x, y);
		glTexCoord2f(1.0, 0.0); glVertex2f(x + width, y);
		glTexCoord2f(1.0, 1.0); glVertex2f(x + width, y + height);
		glTexCoord2f(0.0, 1.0); glVertex2f(x, y + height);
		glEnd();
		glPopMatrix();
	}
	else if (texture == -111){
		glPushMatrix();
		glColor3f(0, 255, 0);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2f(x, y);
		glTexCoord2f(1.0, 0.0); glVertex2f(x + width, y);
		glTexCoord2f(1.0, 1.0); glVertex2f(x + width, y + height);
		glTexCoord2f(0.0, 1.0); glVertex2f(x, y + height);
		glEnd();
		glPopMatrix();
	}

}

void DrawEars()
{
	//use Beezier Curve to draw Ear
	glPushMatrix();
	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 6, &ctrlpts[0][0]);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= 100; i++)
		glEvalCoord1f((GLfloat)i / 100.0);
	glEnd();
	glPopMatrix();
}

void Eye(float X, float Y, float R)
{

	int numVertices = 1000;
	float t = 0;

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < numVertices; ++i)
	{
		glColor3f((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
		glVertex3f(X + R * cos(t), Y + R * sin(t), 0.0);
		glVertex3f(X + R * cos(t), Y + R * sin(t), 1.0);
		
		t += 2 * PI / numVertices;
	}
	glVertex3f(X + R * cos(0), Y + R * sin(0), 0.0);
	
	glEnd();

}

void openM(float X, float Y, float R)
{
	//draw half of a circle 

	int numVertices = 1000;
	float t = 0;

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < numVertices; ++i)
	{
		glColor3f((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
		glVertex3f(X + R * cos(t), Y + R * sin(t), 0.0);
		glVertex3f(X + R * cos(t), Y + R * sin(t), 1.0);

		t += PI / numVertices;
	}
	//glVertex3f(X + R * cos(0), Y + R * sin(0), 0.0);

	glEnd();


}

void closedM()
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(-1, -4, 0);
	glVertex3f(0, -4, 0);
	glEnd();
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glColor3f(0.0, 0.0, 0.0);
	glLoadIdentity();

	// Move everything back 10 units
	glTranslatef(0.0, 0.0, -10.0);
	
	glPushMatrix();
	glTranslatef(.5, 1.0, 0.0);

	//right ear
	DrawEars();
	//left ear
	glPushMatrix();
	glTranslatef(-1, 0, 0);
	glRotatef(180, 0, 1, 0);
	DrawEars();
	glPopMatrix();

	//draw eyes
	glPushMatrix();
	glTranslatef(-.7, -1, 0);
	Eye(-.5, 0, .4);
	Eye(1, 0, .4);
	glPopMatrix();

	//pupils
	glPushMatrix();
	if (hitL == true) {
		glTranslatef(-.20, 0, 0);
	}

	Eye(-1.1, -1, .2);
	Eye(.4, -1, .2);
	glPopMatrix();

	//Nose
	glPushMatrix();
	glTranslatef(-.5, -1.5, 0);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 0.0); glVertex3f(0, 0, 0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-.3, -1, 0);
	glTexCoord2f(1.0, 1.0); glVertex3f(.3, -1, 0);
	glEnd();
	glPopMatrix();

	//draw mouth
	closedM();
	//right paddle collision; open mouth
	if (hitR == true) {
		openM(-.5, -3.7, .45);
	}
	glPopMatrix();

	//walls
	glPushMatrix();
	Rect(-10, 9, 20, 1, -111);
	Rect(-10, -10, 20, 1, -111);
	glPopMatrix();

	// draw paddle
	Rect(paddle_left_x, paddle_left_y, paddle_width, paddle_height, texture[0]);
	Rect(paddle_right_x, paddle_right_y, paddle_width, paddle_height, texture[0]);

	//draw ball
	Rect(ball_pos_x , ball_pos_y , ball_width, ball_height, -111);

	
	// draw score
	glColor3f(255, 0, 0);
	drawText(0, 8, std::to_string(score_left) + ":" + std::to_string(score_right));

	//Guide
	drawText(-1.5, 7, "RIGHT: USE W AND S");
	drawText(-2.5, 6, "LEFT: USE UP AND DOWN ARROWS");

	//display who won
	if (score_left == 5) {
		drawText(-1, 2, "LEFT WON !!!");
	}
	else if (score_right == 5) {
		drawText(-1, 2, "RIGHT WON !!!");
	}

	glutSwapBuffers();

}


// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	
	// Create texture ids.
	glGenTextures(2, texture);

	loadWood();
	loadBanana();
	

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_MAP1_VERTEX_3);
	
	// Turn on OpenGL texturing.
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);

}


void initLighting()
{
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Set lighting intensity and color
	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

	// Set the light position
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);

}

void keyboard() {
	// left pad action
	if (GetAsyncKeyState(VK_W)) {
		//stop paddle from moving past top wall 
		if (paddle_left_y < (9 - paddle_height)) {
			paddle_left_y += paddle_speed;
		}
	}

	if (GetAsyncKeyState(VK_S)) {
		//stop paddle from moving past bottom wall 
		if (paddle_left_y > -9) {
			paddle_left_y -= paddle_speed;
		}
	}

	// right pad action
	if (GetAsyncKeyState(VK_UP)) {
		//stop paddle from moving past top wall 
		if (paddle_right_y < (9 - paddle_height)) {
			paddle_right_y += paddle_speed;
		}
	}

	if (GetAsyncKeyState(VK_DOWN)) {
		//stop paddle from moving past bottom wall 
		if (paddle_right_y > -9) {
			paddle_right_y -= paddle_speed;
		}
	}


}


// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

void updateBall() {
	//start the ball flight
	ball_pos_x += ball_dir_x * ball_speed;
	ball_pos_y += ball_dir_y * ball_speed;

	// left paddle collision
	if (ball_pos_x < paddle_left_x + paddle_width && ball_pos_x > paddle_left_x &&
		ball_pos_y < paddle_left_y + paddle_height &&
		ball_pos_y > paddle_left_y)  {
		hitL = true;
		hitR = false;
		ball_speed += .01;
		float t = ((ball_pos_y - paddle_left_y) / paddle_height) - 0.3f;
		ball_dir_x = fabs(ball_dir_x); // force it to be positive
		ball_dir_y = t;

	}

	// right paddle collision
	if (ball_pos_x < paddle_right_x + paddle_width && ball_pos_x > paddle_right_x &&
		ball_pos_y < paddle_right_y + paddle_height &&
		ball_pos_y > paddle_right_y) {
		hitR = true;
		hitL = false;
		ball_speed += .01;
		float t = ((ball_pos_y - paddle_right_y) / paddle_height) - 0.3f;
		ball_dir_x = -fabs(ball_dir_x); // force it to be negative
		ball_dir_y = t;
	}

	// top wall collision
	if (ball_pos_y > 8.5) {
		ball_dir_y = -fabs(ball_dir_y); // force it to be negative
	}

	// bottom wall collision
	if (ball_pos_y < -9) {
		ball_dir_y = fabs(ball_dir_y); // force it to be positive
	}

	// score left
	if (ball_pos_x < -10) {
		++score_right;
		ball_pos_x = 0;
		ball_pos_y = .5;
		ball_dir_x = fabs(ball_dir_x); // force it to be positive
		ball_dir_y = 0;
		
		ball_speed = .3;

		paddle_right_x = 8;
		paddle_right_y = 0;

		paddle_left_x = -9.0f;
		paddle_left_y = 0;

	}

	// score right
	if (ball_pos_x > 10) {
		++score_left;
		ball_pos_x = 0;
		ball_pos_y = .5;
		ball_dir_x = -fabs(ball_dir_x); // force it to be negative
		ball_dir_y = 0;

		ball_speed = .3;

		paddle_right_x = 8;
		paddle_right_y = 0;

		paddle_left_x = -9.0f;
		paddle_left_y = 0;

	}

	//FIRST TO 5 WIN ; stop the ball movement
	if (score_left == 5 || score_right == 5) {
		ball_speed = 0;
	
	}


}

void update(int value) {

	//input handling 
	keyboard();
	
	//BALL MOVEMENT
	updateBall();
	//loadExternalTextures();

	// Call update() again in 'interval' milliseconds
	glutTimerFunc(interval, update, 0);

	// Redisplay frame
	glutPostRedisplay();
}

// Main routine.
int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(1600, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("pong.cpp");
	initLighting();

	glutDisplayFunc(drawScene);
	glutTimerFunc(interval, update, 0);
	glutReshapeFunc(resize);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

