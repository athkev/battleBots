#include <stdio.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>
#include "surfaceModeller.h"
#include "subdivcurve.h"
#include <iostream>
#include <vector>

#define PI 3.14159265358979323846


GLdouble worldLeft = -12;
GLdouble worldRight = 12;
GLdouble worldBottom = -9;
GLdouble worldTop = 9;
GLdouble worldCenterX = 0.0;
GLdouble worldCenterY = 0.0;
GLdouble wvLeft = -12;
GLdouble wvRight = 12;
GLdouble wvBottom = -9;
GLdouble wvTop = 9;

GLint glutWindowWidth  = 800;
GLint glutWindowHeight = 600;
GLint viewportWidth    = glutWindowWidth;
GLint viewportHeight   = glutWindowHeight;
int window2D, window3D;
int window3DSizeX = 800, window3DSizeY = 600;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;

int lastMouseX;
int lastMouseY;

GLdouble eyeXinit = -22.0, eyeYinit = 0.0, eyeZinit = 0.0;
GLdouble eyeX = eyeXinit, eyeY = eyeYinit, eyeZ = eyeZinit;
GLdouble zNear = 0.1, zFar = 70.0;
GLdouble fov = 60.0;


// Ground Mesh material
GLfloat groundMat_ambient[]    = {0.4, 0.4, 0.4, 1.0};
GLfloat groundMat_specular[]   = {0.01, 0.01, 0.01, 1.0};
GLfloat groundMat_diffuse[]   = {0.4, 0.4, 0.7, 1.0};
GLfloat groundMat_shininess[]  = {1.0};

// Two Lights
GLfloat light_position0[] = {4.0, 8.0, 8.0, 1.0};
GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};

GLfloat light_position1[] = {-4.0, 8.0, 8.0, 1.0};
GLfloat light_diffuse1[] = {1.0, 1.0, 1.0, 1.0};

GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat model_ambient[]  = {0.5, 0.5, 0.5, 1.0};


GLfloat mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat mat_shininess[] = { 32.0F };


// The profile curve is a subdivision curve

int numCirclePoints = 30;
double circleRadius = 0.2;
int hoveredCircle = -1;

int currentCurvePoint = 0;
int angle = 0;
int animate = 0;
int delay = 15; // milliseconds

int n_col = 5;
double n_curves = 360 / n_col;
int n_rows = 0;
bool changed = true;

int mouseXinit;
int mouseYinit;
int currentX;
int currentY;
int angleInitX = 0;
int angleInitY = 90;
int angleAzi = 0;
int angleEle = 90;
int r = 22;

SubdivisionCurve subcurve;
// Use circles to draw subdivision curve control points
Circle circles[MAXCONTROLPOINTS];


void allocVertexArray(void);
void drawQuadArray(void);

void keyboardHandler3D(unsigned char key, int x, int y);

int main(int argc, char* argv[])
{
	glutInit(&argc, (char **)argv); 

	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutWindowWidth,glutWindowHeight);
	glutInitWindowPosition(50,100);  
	
	// The 2D Window
	window2D = glutCreateWindow("Profile Curve"); 
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	// Initialize the 2D profile curve system
	init2DCurveWindow(); 
	// A few input handlers
	glutMouseFunc(mouseButtonHandler);
	glutMotionFunc(mouseMotionHandler);
	glutPassiveMotionFunc(mouseHoverHandler);
	glutMouseWheelFunc(mouseScrollWheelHandler);
	glutKeyboardFunc(keyboardHandler);
	glutSpecialFunc(specialKeyHandler);
	

	// The 3D Window
	window3D = glutCreateWindow("Surface of Revolution"); 
	glutPositionWindow(900,100);  
	glutDisplayFunc(display3D);
	glutReshapeFunc(reshape3D);
	glutMouseFunc(mouseButtonHandler3D);
	glutMouseWheelFunc(mouseScrollWheelHandler3D);
	glutMotionFunc(mouseMotionHandler3D);
	glutKeyboardFunc(keyboardHandler3D);
	// Initialize the 3D system
	init3DSurfaceWindow();

	// Annnd... ACTION!!
	glutMainLoop(); 

	return 0;
}

void init2DCurveWindow() 
{ 
	glLineWidth(3.0);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);
	initSubdivisionCurve();
	initControlPoints();
	
} 

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();
	gluOrtho2D(wvLeft, wvRight, wvBottom, wvTop);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	draw2DScene();	
	glutSwapBuffers();
}


void draw2DScene() 
{
	drawAxes();
	drawSubdivisionCurve();
	drawControlPoints();
	if (changed) {
		allocVertexArray();
		changed = false;
	}
}

void drawAxes()
{
	glPushMatrix();
	glColor3f(1.0, 0.0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0, 8.0, 0);
	glVertex3f(0, -8.0, 0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(-8, 0.0, 0);
	glVertex3f(8, 0.0, 0);
	glEnd();
	glPopMatrix();
}

void drawSubdivisionCurve() {
	
	// Subdivide the given curve based on control points
	computeSubdivisionCurve(&subcurve);
	
	// Draw it
	glColor3f(0.0, 1.0, 0.0);
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	for (int i=0; i<subcurve.numCurvePoints; i++)
	{
		glVertex3f(subcurve.curvePoints[i].x, subcurve.curvePoints[i].y, 0.0);
	}
	glEnd();
	glPopMatrix();
}

void drawControlPoints(){
	int i, j;
	for (i=0; i<subcurve.numControlPoints; i++){
		glPushMatrix();
		glColor3f(1.0f,0.0f,0.0f); 
		glTranslatef(circles[i].circleCenter.x, circles[i].circleCenter.y, 0);
		// for the hoveredCircle, draw an outline and change its colour
		if (i == hoveredCircle){ 
			// outline
			glColor3f(0.0, 1.0, 0.0);
			glBegin(GL_LINE_LOOP); 
			for(j=0; j < numCirclePoints; j++) {
				glVertex3f(circles[i].circlePoints[j].x, circles[i].circlePoints[j].y, 0); 
			}
			glEnd();
			// colour change
			glColor3f(0.5,0.0,1.0);
		}
		glBegin(GL_LINE_LOOP); 
		for(j=0; j < numCirclePoints; j++) {
			glVertex3f(circles[i].circlePoints[j].x, circles[i].circlePoints[j].y, 0); 
		}
		glEnd();
		glPopMatrix();
	}
}

void initSubdivisionCurve() {
	// Initialize 3 control points of the subdivision curve
	subcurve.controlPoints[0].x = 0;
	subcurve.controlPoints[1].x = 4;
	subcurve.controlPoints[2].x = 5;

	subcurve.controlPoints[0].y = 1.5;
	subcurve.controlPoints[1].y = 1.2;
	subcurve.controlPoints[2].y = 0;

	subcurve.numControlPoints = 3;
	subcurve.subdivisionSteps = 3;
}

void initControlPoints(){
	int i;
	int num = subcurve.numControlPoints;
	for (i=0; i < num; i++){
		constructCircle(circleRadius, numCirclePoints, circles[i].circlePoints);
		circles[i].circleCenter = subcurve.controlPoints[i];
	}
}

void screenToWorldCoordinates(int xScreen, int yScreen, GLdouble *xw, GLdouble *yw)
{
	GLdouble xView, yView;
	screenToCameraCoordinates(xScreen, yScreen, &xView, &yView);
	cameraToWorldCoordinates(xView, yView, xw, yw);
}

void screenToCameraCoordinates(int xScreen, int yScreen, GLdouble *xCamera, GLdouble *yCamera)
{
	*xCamera = ((wvRight-wvLeft)/glutWindowWidth)  * xScreen; 
	*yCamera = ((wvTop-wvBottom)/glutWindowHeight) * (glutWindowHeight-yScreen); 
}

void cameraToWorldCoordinates(GLdouble xcam, GLdouble ycam, GLdouble *xw, GLdouble *yw)
{
	*xw = xcam + wvLeft;
	*yw = ycam + wvBottom;
}

void worldToCameraCoordinates(GLdouble xWorld, GLdouble yWorld, GLdouble *xcam, GLdouble *ycam)
{
	double wvCenterX = wvLeft   + (wvRight - wvLeft)/2.0;
	double wvCenterY = wvBottom + (wvTop   - wvBottom)/2.0;
	*xcam = worldCenterX - wvCenterX + xWorld;
	*ycam = worldCenterY - wvCenterY + yWorld;
}

int currentButton;

void mouseButtonHandler(int button, int state, int xMouse, int yMouse)
{
	int i;
	
	currentButton = button;
	if (button == GLUT_LEFT_BUTTON)
	{  
		switch (state) {      
		case GLUT_DOWN:
			if (hoveredCircle > -1) {
				screenToWorldCoordinates(xMouse, yMouse, &circles[hoveredCircle].circleCenter.x, &circles[hoveredCircle].circleCenter.y);
				screenToWorldCoordinates(xMouse, yMouse, &subcurve.controlPoints[hoveredCircle].x, &subcurve.controlPoints[hoveredCircle].y);
			}
			break;
		case GLUT_UP:
			glutSetWindow(window3D);
			glutPostRedisplay();
			changed = true;
			break;
		}
	}    
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		switch (state) {      
		case GLUT_DOWN:
			break;
		case GLUT_UP:
			if (hoveredCircle == -1 && subcurve.numControlPoints < MAXCONTROLPOINTS){ 
				GLdouble newPointX;
				GLdouble newPointY;
				screenToWorldCoordinates(xMouse, yMouse, &newPointX, &newPointY);
				subcurve.controlPoints[subcurve.numControlPoints].x = newPointX;
				subcurve.controlPoints[subcurve.numControlPoints].y = newPointY;			
				constructCircle(circleRadius, numCirclePoints, circles[subcurve.numControlPoints].circlePoints);
				circles[subcurve.numControlPoints].circleCenter = subcurve.controlPoints[subcurve.numControlPoints];
				subcurve.numControlPoints++;
				changed = true;
			} else if (hoveredCircle > -1 && subcurve.numControlPoints > MINCONTROLPOINTS) {
				subcurve.numControlPoints--;
				for (i=hoveredCircle; i<subcurve.numControlPoints; i++){
					subcurve.controlPoints[i].x = subcurve.controlPoints[i+1].x;
					subcurve.controlPoints[i].y = subcurve.controlPoints[i+1].y;
					circles[i].circleCenter = circles[i+1].circleCenter;
				}
				changed = true;
			}
			
			glutSetWindow(window3D);
			glutPostRedisplay();
			break;
		}
	}

	glutSetWindow(window2D);
	glutPostRedisplay();
}

void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON) {  
		if (hoveredCircle > -1) {
			screenToWorldCoordinates(xMouse, yMouse, &circles[hoveredCircle].circleCenter.x, &circles[hoveredCircle].circleCenter.y);
			screenToWorldCoordinates(xMouse, yMouse, &subcurve.controlPoints[hoveredCircle].x, &subcurve.controlPoints[hoveredCircle].y);
		}
	}    
	else if (currentButton == GLUT_MIDDLE_BUTTON) {
	}
	glutPostRedisplay();
}

void mouseHoverHandler(int xMouse, int yMouse)
{
	hoveredCircle = -1;
	GLdouble worldMouseX, worldMouseY;
	screenToWorldCoordinates(xMouse, yMouse, &worldMouseX, &worldMouseY);
	int i;
	// see if we're hovering over a control point
	for (i=0; i<subcurve.numControlPoints; i++){
		GLdouble distToX = worldMouseX - circles[i].circleCenter.x;
		GLdouble distToY = worldMouseY - circles[i].circleCenter.y;
		GLdouble euclideanDist = sqrt(distToX*distToX + distToY*distToY);
		//printf("Dist from point %d is %.2f\n", i, euclideanDist);
		if (euclideanDist < 2.0){
			hoveredCircle = i;
		}
	}
	
	glutPostRedisplay();
}

void mouseScrollWheelHandler(int button, int dir, int xMouse, int yMouse)
{
	GLdouble worldViewableWidth;
	GLdouble worldViewableHeight;
	GLdouble cameraOnCenterX;
	GLdouble cameraOnCenterY;
	GLdouble anchorPointX, anchorPointY;
	double clipWindowWidth;
	double clipWindowHeight;
	double wvCenterX = wvLeft   + (wvRight - wvLeft)/2.0;
	double wvCenterY = wvBottom + (wvTop   - wvBottom)/2.0;
	double wvWidth   = wvRight - wvLeft;
	double wvHeight  = wvTop   - wvBottom;
	
	worldToCameraCoordinates(worldCenterX, worldCenterY, &cameraOnCenterX, &cameraOnCenterY);
	if (wvWidth >= (worldRight-worldLeft)*1.2){
		
		anchorPointX = cameraOnCenterX;
		anchorPointY = cameraOnCenterY;
	} else {
		// else, anchor the zoom to the mouse
		screenToWorldCoordinates(xMouse, yMouse, &anchorPointX, &anchorPointY);
	}
	GLdouble anchorToCenterX = anchorPointX - wvCenterX;
	GLdouble anchorToCenterY = anchorPointY - wvCenterY;

	// set up maximum shift
	GLdouble maxPosShift = 50;
	GLdouble maxNegShift = -50;	
	
	anchorToCenterX = (anchorToCenterX > maxPosShift)? maxPosShift : anchorToCenterX;
	anchorToCenterX = (anchorToCenterX < maxNegShift)? maxNegShift : anchorToCenterX;
	anchorToCenterY = (anchorToCenterY > maxPosShift)? maxPosShift : anchorToCenterY;
	anchorToCenterY = (anchorToCenterY < maxNegShift)? maxNegShift : anchorToCenterY;

	// move the world centre closer to this point.
	wvCenterX += anchorToCenterX/4;
	wvCenterY += anchorToCenterY/4;
	

	if (dir > 0) {
		// Zoom in to mouse point
		clipWindowWidth = wvWidth*0.8;
		clipWindowHeight= wvHeight*0.8;
		wvRight =  wvCenterX + clipWindowWidth/2.0;
		wvTop   =  wvCenterY + clipWindowHeight/2.0;
		wvLeft  =  wvCenterX - clipWindowWidth/2.0;
		wvBottom=  wvCenterY - clipWindowHeight/2.0;

	}
	else {
		// Zoom out
		clipWindowWidth = wvWidth*1.25;
		clipWindowHeight= wvHeight*1.25;
		wvRight =  wvCenterX + clipWindowWidth/2.0;
		wvTop   =  wvCenterY + clipWindowHeight/2.0;
		wvLeft  =  wvCenterX - clipWindowWidth/2.0;
		wvBottom=  wvCenterY - clipWindowHeight/2.0;
	}

	glutPostRedisplay();

}

void keyboardHandler(unsigned char key, int x, int y)
{
	int i;
	
	double clipWindowWidth;
	double clipWindowHeight;
	double wvCenterX = wvLeft   + (wvRight - wvLeft)/2.0;
	double wvCenterY = wvBottom + (wvTop   - wvBottom)/2.0;
	double wvWidth   = wvRight - wvLeft;
	double wvHeight  = wvTop   - wvBottom;

	switch(key){
	case 'q':
	case 'Q':
	case 27:
		// Esc, q, or Q key = Quit 
		exit(0);
		break;
	case 107:
	case '+':
		clipWindowWidth = wvWidth*0.8;
		clipWindowHeight= wvHeight*0.8;
		wvRight =  wvCenterX + clipWindowWidth/2.0;
		wvTop   =  wvCenterY + clipWindowHeight/2.0;
		wvLeft  =  wvCenterX - clipWindowWidth/2.0;
		wvBottom=  wvCenterY - clipWindowHeight/2.0;
		break;
	case 109:
	case '-':
		clipWindowWidth = wvWidth*1.25;
		clipWindowHeight= wvHeight*1.25;
		wvRight =  wvCenterX + clipWindowWidth/2.0;
		wvTop   =  wvCenterY + clipWindowHeight/2.0;
		wvLeft  =  wvCenterX - clipWindowWidth/2.0;
		wvBottom=  wvCenterY - clipWindowHeight/2.0;
		break;
	case 'f':
		printf("F1 Help: \n");
		printf("	On 2D windows: \n");
		printf("		left mouseclick: move control vertex \n");
		printf("		middle mouseclick: add or remove control vertex \n");
		printf("		+: zoom in \n");
		printf("		-: zoom out \n");
		printf("		q or Q: quit \n");
		printf("	On 3D windows: \n");
		printf("		e: toggle wireframe \n");
		printf("		left mouseclick: control camera \n");
		printf("		mouse wheel in: zoom in \n");
		printf("		mouse wheel out: zoom out \n");
		printf("		q or Q: quit \n");

		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void specialKeyHandler(int key, int x, int y)
{
	double clipWindowWidth;
	double clipWindowHeight;
	double wvCenterX = wvLeft   + (wvRight - wvLeft)/2.0;
	double wvCenterY = wvBottom + (wvTop   - wvBottom)/2.0;
	double wvWidth   = wvRight - wvLeft;
	double wvHeight  = wvTop   - wvBottom;

	switch(key)	{
	case GLUT_KEY_LEFT:
		wvLeft -= 5.0;
		wvRight-= 5.0;
		break;
	case GLUT_KEY_RIGHT:
		wvLeft += 5.0;
		wvRight+= 5.0;
		break;
	case GLUT_KEY_UP:
		wvTop   += 5.0;
		wvBottom+= 5.0;
		break;
	case GLUT_KEY_DOWN:
		wvTop   -= 5.0;
		wvBottom-= 5.0;
		break;
		// Want to zoom in/out and keep  aspect ratio = 2.0
	case GLUT_KEY_F1:
		clipWindowWidth = wvWidth*0.8;
		clipWindowHeight= wvHeight*0.8;
		wvRight =  wvCenterX + clipWindowWidth/2.0;
		wvTop   =  wvCenterY + clipWindowHeight/2.0;
		wvLeft  =  wvCenterX - clipWindowWidth/2.0;
		wvBottom=  wvCenterY - clipWindowHeight/2.0;
		break;
	case GLUT_KEY_F2:
		clipWindowWidth = wvWidth*1.25;
		clipWindowHeight= wvHeight*1.25;
		wvRight =  wvCenterX + clipWindowWidth/2.0;
		wvTop   =  wvCenterY + clipWindowHeight/2.0;
		wvLeft  =  wvCenterX - clipWindowWidth/2.0;
		wvBottom=  wvCenterY - clipWindowHeight/2.0;
		break;
	}
	glutPostRedisplay();
}


void reshape(int w, int h)
{
	glutWindowWidth = (GLsizei) w;
	glutWindowHeight = (GLsizei) h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(wvLeft, wvRight, wvBottom, wvTop);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



/************************************************************************************
 *
 *
 * 3D Window and Surface of Revolution Code 
 *
 * Fill in the code in the empty functions
 * Feel free to use your own functions or rename mine. Mine are just a guide.
 * Add whatever variables you think are necessary
 ************************************************************************************/

 //
 // Surface of Revolution consists of vertices and quads
 //
 // Set up lighting/shading and material properties for surface of revolution
GLfloat quadMat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat quadMat_specular[] = { 0.45, 0.55, 0.45, 1.0 };
GLfloat quadMat_diffuse[] = { 0.1, 0.35, 0.1, 1.0 };
GLfloat quadMat_shininess[] = { 10.0 };

// You might use something like this for your surface of revolution
// Feel free to add to it or use your own data structures


struct point {
	double x, y, z;

	point(double p_x, double p_y, double p_z) :
		x(p_x), y(p_y), z(p_z) {}

};

typedef struct Vertex
{
	GLdouble x, y, z;
	Vector3D normal; // vertex normal vector
} Vertex;

// Suggested quad structure
// Feel free to use your own or use C++
// Each quad shares vertices with other neighbor quads
// So build a quad array where each quad has 4 indices into the vertex array or 4 pointers
// into the vertex array - your choice


typedef struct Quad
{
	//int vertex[4]; // 4 indexes into vertex array e.g. 23, 45, 86, 71 means it uses vertices 23 in vertex array etc
	Vertex *vertex[4]; // 4 pointers to vertices in the vertex array
	Vector3D normal; // quad normal vector for this quad
} Quad;

// Use this form of pointer to the vertex array and quad array
// This means you must allocate memory for your vertex array and your quad array.
// Take a look at how I did this in the quadmesh code I gave you in assignment 1


Vertex **vertexArray;
Quad   **quadArray;

// Alternatively, use fixed size arrays like this:
// If you use fixed size arrays then this means you have to always use the same number of control points and
// never change it
//Vertex vertexArray[NUMROWS][NUMCOLS];// you need to define NUMROWS and NUMCOLS based on how many curvepoints in the 
                                       // profile curve and how many times you rotate the curvepoints
//vertexArray[360//n][n_points]
//Quad quadArray[NUMROWS][NUMCOLS];

void calcAzi(void);
bool wireframe = false;

void init3DSurfaceWindow()
{
	// uncomment once everything is working
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	printf("Press F on 2D or 3D window for help! \n");
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear

	glViewport(0, 0, (GLsizei)window3DSizeX, (GLsizei)window3DSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void reshape3D(int w, int h)
{
	glutWindowWidth = (GLsizei) w;
	glutWindowHeight = (GLsizei) h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,aspect,zNear,zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

}
void calcAzi()
{

	angleAzi = (angleInitX + mouseXinit - currentX);
	angleEle = -(angleInitY + mouseYinit - currentY);

	eyeX = r * cos((angleAzi)*PI / 180) * sin((angleEle)*PI / 180);
	eyeZ = r *sin((angleAzi) * PI / 180) * sin((angleEle) * PI/180);
	eyeY = r * cos((angleEle) * PI / 180);
	//printf("%f %f %f \n", eyeX, eyeY, eyeZ);
}

void display3D()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		
	drawGround();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);
	
	
	
	drawQuadArray();

	
	
	//printf("%f \n", quadArray[0][0].vertex[0]->x);
	//printf("%f \n", quadArray[0][0].vertex[1]->x);
	glutSwapBuffers();
}

void drawGround() {
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-22.0f, -4.0f, -22.0f);
	glVertex3f(-22.0f, -4.0f, 22.0f); 
	glVertex3f(22.0f, -4.0f, 22.0f);
	glVertex3f(22.0f, -4.0f, -22.0f);
	glEnd();
	glPopMatrix();
}


void mouseButtonHandler3D(int button, int state, int x, int y)
{
	currentButton = button;
	
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			mouseXinit = x;
			mouseYinit = y;
		}
		if (state == GLUT_UP)
		{
			angleInitX = angleAzi;
			angleInitY = angleEle;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{

		}
			
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
		{
			// add code here
			
		}
		
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse){
	
	if (dir > 0){		
		// add code here
		r -= 1;
		calcAzi();
		glutPostRedisplay();
	} else {
		// add code here
		r += 1;
		calcAzi();
		glutPostRedisplay();
	}
}

void mouseMotionHandler3D(int x, int y)
{
	if (currentButton == GLUT_LEFT_BUTTON) {
		
		currentX = x;
		currentY = y;
		calcAzi();
	}
	if (currentButton == GLUT_RIGHT_BUTTON) 
	{
		
	}
	else if (currentButton == GLUT_MIDDLE_BUTTON) 
	{
	}

	glutPostRedisplay();
}

void keyboardHandler3D(unsigned char key, int x, int y)
{


	switch (key) {
	case 'q':
	case 'Q':
	case 27:
		exit(0);
		break;
	case 'r':
		eyeX = eyeXinit;
		eyeY = eyeYinit;
		eyeZ = eyeZinit;
		angleInitX = 0;
		angleInitY = 90;
		angleAzi = 0;
		angleEle = 90;
		r = 22;
		break;
	case 'e':
		if (wireframe) wireframe = false;
		else wireframe = true;
		break;
	case 'f':
		printf("F1 Help: \n");
		printf("	On 2D windows: \n");
		printf("		left mouseclick: move control vertex \n");
		printf("		middle mouseclick: add or remove control vertex \n");
		printf("		+: zoom in \n");
		printf("		-: zoom out \n");
		printf("		q or Q: quit \n");
		printf("	On 3D windows: \n");
		printf("		e: toggle wireframe \n");
		printf("		left mouseclick: control camera \n");
		printf("		mouse wheel in: zoom in \n");
		printf("		mouse wheel out: zoom out \n");
		printf("		q or Q: quit \n");

		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void allocVertexArray()
{
	vertexArray = new Vertex * [n_curves];
	for (int i = 0; i < n_curves; i++) {
		vertexArray[i] = new Vertex[subcurve.numCurvePoints];
	}

	quadArray = new Quad * [n_curves];
	for (int i = 0; i < n_curves; i++)
	{
		quadArray[i] = new Quad[subcurve.numCurvePoints];
	}


	buildVertexArray();
	buildQuadArray();
	//printf("%f \n", quadArray[0][0].vertex[0]->x);
}
void buildVertexArray()
{
	// build a new vertex array based on all the profile curves created so far by rotation
	//
	//for j in 0 to 360/5
	//	for k in 0 to subcurve.numControlPoints // goes through each vertex in a curve
	//		x = xcos(angle) + zsin(angle)
	//		z = zcos(angle) - xsin(angle)
	//		save x and z coord to vertexArray[j][k]

	//n_curves : 360/5
	for (int i = 0; i < n_curves; i++)
	{
		//glPushMatrix();
		//glBegin(GL_LINE_STRIP);
		for (int j = 0; j < subcurve.numCurvePoints; j++)
		{
			Vertex curV;
			curV.x = subcurve.curvePoints[j].x * cos(i * n_col * PI / 180);
			curV.z = - subcurve.curvePoints[j].x * sin(i * n_col * PI / 180);
			curV.y = subcurve.curvePoints[j].y;
			vertexArray[i][j]= curV;	
			//glVertex3f(curV.x, curV.y, curV.z);
			//printf("%i %f \n", i, curV.x);

			//printf("%i %i \n", i, j);
			n_rows += 1;
		}
		//glEnd();
		//glPopMatrix();
		/*
		glColor3f(0.0, 1.0, 0.0);
		glPushMatrix();
		glRotatef(i * n_col, 0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < subcurve.numCurvePoints; i++)
		{
			glVertex3f(subcurve.curvePoints[i].x, subcurve.curvePoints[i].y, 0.0);
		}
		glEnd();
		glPopMatrix();
		
		*/
	}

}

Vector3D rotateAroundY(double x, double z, double theta)
{	
	Vector3D v;
	v.x = v.y = v.z = 0;
	// takes the x,z values of a point and rotates in around the y axis by theta degrees
	// use the cos/sin formula from the lecture slides where we derived 2D rotation
	return v;
}



void buildQuadArray()
{
	// for all profile curves (obtained by copying the original curve from the 2D window 
	// and rotating the curve points around the y axis by, for example, 5, 10, 15, 20, 30 degrees ... etc
	// construct quads based on the current curve and the previous curve 
	// don't forget the special case at the end where you construct quads using the last curve and the very
	// first curve

	//subcurve.controlPoints

	for (int i = 0; i < n_curves; i++)
	{
		//printf("%f %f %f %f \n", vertexArray[i][0].x, vertexArray[i + 1][0].x, vertexArray[i + 1][1].x, vertexArray[i][1].x);
		for (int j = 0; j < subcurve.numCurvePoints - 1; j++)
		{
			quadArray[i][j].vertex[0] = &vertexArray[i][j];
			quadArray[i][j].vertex[1] = &vertexArray[i+1][j];
			quadArray[i][j].vertex[2] = &vertexArray[i+1][j + 1];
			quadArray[i][j].vertex[3] = &vertexArray[i][j + 1];

			
		}
	}
	
	
	
}

void drawQuadArray() {
	computeQuadNormals();
	computeVertexNormals();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);

	if (!wireframe) {
		glPushMatrix();
		glBegin(GL_QUADS);
		for (int i = 0; i < n_curves; i++)
		{
			for (int j = 0; j < subcurve.numCurvePoints - 1; j++)
			{
				if (i == n_curves - 1) {
					glNormal3f(quadArray[i][j].normal.x, quadArray[i][j].normal.y, quadArray[i][j].normal.z);
					glVertex3f(vertexArray[i][j].x, vertexArray[i][j].y, vertexArray[i][j].z);
					glNormal3f(quadArray[0][j].normal.x, quadArray[0][j].normal.y, quadArray[0][j].normal.z);
					glVertex3f(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z);
					glNormal3f(quadArray[0][j+1].normal.x, quadArray[0][j+1].normal.y, quadArray[0][j+1].normal.z);
					glVertex3f(vertexArray[0][j + 1].x, vertexArray[0][j + 1].y, vertexArray[0][j + 1].z);
					glNormal3f(quadArray[i][j+1].normal.x, quadArray[i][j+1].normal.y, quadArray[i][j+1].normal.z);
					glVertex3f(vertexArray[i][j + 1].x, vertexArray[i][j + 1].y, vertexArray[i][j + 1].z);

				}
				else {
					glNormal3f(quadArray[i][j].normal.x, quadArray[i][j].normal.y, quadArray[i][j].normal.z);
					glVertex3f(quadArray[i][j].vertex[0]->x, quadArray[i][j].vertex[0]->y, quadArray[i][j].vertex[0]->z);

					glNormal3f(quadArray[i+1][j].normal.x, quadArray[i+1][j].normal.y, quadArray[i+1][j].normal.z);
					glVertex3f(quadArray[i][j].vertex[1]->x, quadArray[i][j].vertex[1]->y, quadArray[i][j].vertex[1]->z);
					glNormal3f(quadArray[i+1][j + 1].normal.x, quadArray[i+1][j + 1].normal.y, quadArray[i+1][j + 1].normal.z);
					glVertex3f(quadArray[i][j].vertex[2]->x, quadArray[i][j].vertex[2]->y, quadArray[i][j].vertex[2]->z);

					glNormal3f(quadArray[i][j+1].normal.x, quadArray[i][j+1].normal.y, quadArray[i][j+1].normal.z);
					glVertex3f(quadArray[i][j].vertex[3]->x, quadArray[i][j].vertex[3]->y, quadArray[i][j].vertex[3]->z);

				}

			}
		}
		glEnd();
		glPopMatrix();
	}
	else //wireframe
	{
		for (int i = 0; i < n_curves; i++) //vertical
		{
			glPushMatrix();
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < subcurve.numCurvePoints; j++)
			{
				glNormal3f(quadArray[i][j].normal.x, quadArray[i][j].normal.y, quadArray[i][j].normal.z);
				glVertex3f(vertexArray[i][j].x, vertexArray[i][j].y, vertexArray[i][j].z);
			}
			glEnd();
			glPopMatrix();
		}



		for (int j = 0; j < subcurve.numCurvePoints; j++)
		{
			glPushMatrix();
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < n_curves; i++)
			{
				glNormal3f(quadArray[i][j].normal.x, quadArray[i][j].normal.y, quadArray[i][j].normal.z);
				glVertex3f(vertexArray[i][j].x, vertexArray[i][j].y, vertexArray[i][j].z);
			}
			glEnd();
			glPopMatrix();
		}
		

	}
}

point normal(const std::vector<point>& pts) {
	double normal_x = 0.0;
	double normal_y = 0.0;
	double normal_z = 0.0;

	int i, j;
	for (i = 0, j = 1; i < pts.size(); i++, j++) {
		if (j == pts.size()) j = 0;
		point pi = pts[i];
		point pj = pts[j];

		normal_x += (((pi.z) + (pj.z)) *
			((pj.y) - (pi.y)));

		normal_y += (((pi.x) + (pj.x)) *
			((pj.z) - (pi.z)));

		normal_z += (((pi.y) + (pj.y)) *
			((pj.x) - (pi.x)));

	}

	return point(normal_x, normal_y, normal_z);
}

void computeQuadNormals() 
{	
	std::vector<point> poly;
	poly.push_back(point(0, 0, 0));
	poly.push_back(point(1, 0, 0));
	poly.push_back(point(1, 1, 0));
	poly.push_back(point(0, 1, 0));

	point norm = normal(poly);
	// compute one normal per quad polygon and store in the quad structure
	// Use Newell's Method - see http://www.dillonbhuff.com/?p=284
	for (int i = 0; i < n_curves; i++)
	{
		for (int j = 0; j < subcurve.numCurvePoints; j++)
		{
			std::vector<point> poly;
			Vector3D norm3D;

			if (j == subcurve.numCurvePoints - 1)
			{
				if (i == n_curves - 1)
				{
					poly.push_back(point(vertexArray[i][j].x, vertexArray[i][j].y, vertexArray[i][j].z));
					poly.push_back(point(vertexArray[i][j - 1].x, vertexArray[i][j - 1].y, vertexArray[i][j - 1].z));
					poly.push_back(point(vertexArray[0][j - 1].x, vertexArray[0][j - 1].y, vertexArray[0][j - 1].z));
					poly.push_back(point(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z));
				}
				else {
					poly.push_back(point(vertexArray[i][j].x, vertexArray[i][j].y, vertexArray[i][j].z));
					poly.push_back(point(vertexArray[i][j - 1].x, vertexArray[i][j - 1].y, vertexArray[i][j - 1].z));
					poly.push_back(point(vertexArray[i + 1][j - 1].x, vertexArray[i + 1][j - 1].y, vertexArray[i + 1][j - 1].z));
					poly.push_back(point(vertexArray[i + 1][j].x, vertexArray[i + 1][j].y, vertexArray[i + 1][j].z));
				}
			}
			else {
				if (i == n_curves - 1)
				{
					poly.push_back(point(vertexArray[i][j].x, vertexArray[i][j].y, vertexArray[i][j].z));
					poly.push_back(point(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z));
					poly.push_back(point(vertexArray[0][j + 1].x, vertexArray[0][j + 1].y, vertexArray[0][j + 1].z));
					poly.push_back(point(vertexArray[i][j + 1].x, vertexArray[i][j + 1].y, vertexArray[i][j + 1].z));
				}
				else
				{
					//quadArray[i][j].normal =

					poly.push_back(point(quadArray[i][j].vertex[0]->x, quadArray[i][j].vertex[0]->y, quadArray[i][j].vertex[0]->z));
					poly.push_back(point(quadArray[i][j].vertex[1]->x, quadArray[i][j].vertex[1]->y, quadArray[i][j].vertex[1]->z));
					poly.push_back(point(quadArray[i][j].vertex[2]->x, quadArray[i][j].vertex[2]->y, quadArray[i][j].vertex[2]->z));
					poly.push_back(point(quadArray[i][j].vertex[3]->x, quadArray[i][j].vertex[3]->y, quadArray[i][j].vertex[3]->z));
				}
			}

			point norm = normal(poly);
			norm3D.x = norm.x;
			norm3D.y = norm.y;
			norm3D.z = norm.z;
			quadArray[i][j].normal = norm3D;
			
		}
		
	}
}

void computeVertexNormals()
{
	// compute a normal for each vertex of the surface by averaging the 4 normals of 
	// the 4 quads that share the vertex
	// be careful at the bottom and top of the surface where only 2 quads share a vertex
	// Hint on one way to do this: after computeQuadNormals(),  go through all quads and *add* the quad normal to each 
	// of its 4 vertices. Once this is done, go through all vertices and normalize the normal vector
	
	for (int i = 0; i < n_curves; i++)
	{
		for (int j = 0; j < subcurve.numCurvePoints - 1; j++)
		{
			if (i = n_curves - 1) {}
			else {
				quadArray[i][j].vertex[0]->normal = quadArray[i][j].normal;
				quadArray[i][j].vertex[1]->normal = quadArray[i][j].normal;
				quadArray[i][j].vertex[2]->normal = quadArray[i][j].normal;
				quadArray[i][j].vertex[3]->normal = quadArray[i][j].normal;
			}
		}
	}
}



Vector3D crossProduct(Vector3D a, Vector3D b){
	Vector3D cross;
	
	cross.x = a.y * b.z - b.y * a.z;
	cross.y = a.x * b.z - b.x * a.z;
	cross.z = a.x * b.y - b.x * a.y;
	
	return cross;
}

Vector3D fourVectorAverage(Vector3D a, Vector3D b, Vector3D c, Vector3D d){
	Vector3D average;
	average.x = (a.x + b.x + c.x + d.x)/4.0;
	average.y = (a.y + b.y + c.y + d.y)/4.0;
	average.z = (a.z + b.z + c.z + d.z)/4.0;
	return average;
}

Vector3D normalize(Vector3D a){
	GLdouble norm = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	Vector3D normalized;
	normalized.x = a.x/norm;
	normalized.y = a.y/norm;
	normalized.z = a.z/norm;
	return normalized;
}







