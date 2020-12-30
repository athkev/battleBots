/*******************************************************************
		   Hierarchical Multi-Part Model Example
********************************************************************/
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define GLEW_STATIC
#include <gl/glew.h>
#ifdef _WIN32
#include <GL/wglew.h> // For wglSwapInterval
#endif
#include <gl/glut.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>

#include <fstream>
#include <ctime>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "cube.h"
#include "QuadMesh.h"
#include "Robot3D.h"
#include "surfaceModeller.h"
#include "subdivcurve.h"
#include <iostream>

#include "RGBpixmap.h"
#include <SOIL.h>

#define PI 3.14159265

RGBpixmap pix1[1];
RGBpixmap pix2[1];
RGBpixmap pix3[1];

const int vWidth = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

// Note how everything depends on robot body dimensions so that can scale entire robot proportionately
// just by changing robot body scale
float robotBodyWidth = 8.0;
float robotBodyLength = 10.0;
float robotBodyDepth = 6.0;
float headWidth = 0.4 * robotBodyWidth;
float headLength = headWidth;
float headDepth = headWidth;
float upperArmLength = robotBodyLength;
float upperArmWidth = 0.125 * robotBodyWidth;
float gunLength = upperArmLength / 4.0;
float gunWidth = upperArmWidth;
float gunDepth = upperArmWidth;
float stanchionLength = robotBodyLength;
float stanchionRadius = 0.1 * robotBodyDepth;
float baseWidth = 2 * robotBodyWidth;
float baseLength = 0.25 * stanchionLength;

// Control Robot body rotation on base
float robotAngle = 0.0;

// Control arm rotation
float shoulderAngle = -40.0;
float gunAngle = -25.0;

// Spin Cube Mesh
float cubeAngle = 0.0;

float wheelAngleL = 0.0;
float wheelAngleR = 0.0;

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat robotArm_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat robotArm_mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat robotArm_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat robotArm_mat_shininess[] = { 50.0f };


// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


// Mouse button
int currentButton;

// A template cube mesh
CubeMesh* cubeMesh = createCubeMesh();

// A flat open mesh
QuadMesh* groundMesh = NULL;

// Structure defining a bounding box, currently unused
typedef struct BoundingBox {
	VECTOR3D min;
	VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void spinAnimation(int param);
void spinAnimationAI(int param);
void botMovement(void);

//func for project
void drawFront(float x, float y, float z);
void drawBlade(float x, float y, float z, float rot);
void drawWheel(float x, float y, float z, bool LoR);
void drawAxle(float locx, float locy, float locz, float length);



float X = 0;
float Y = 0;
float Z = 0;

float pivotX = 0;
float pivotY = 0;
float pivotZ = 0;

float tempX = 0;
float tempZ = 0;

float X_AI = 0;
float Y_AI = 0;
float Z_AI = 0;

GLdouble eyeXinit = 0.0, eyeYinit = 20.0, eyeZinit = 50.0;
GLdouble eyeXfp = 0.0, eyeYfp = 0.0, eyeZfp = 0.0;
bool fp = false;

//preset for size, loc, etc
float bodyLocz = 0.0f;

float bodySizex = 8;
float bodySizey = 2;
float bodySizez = 5;

float bodyAngle = 45;
float bodyAngle_AI = -45;

bool moving = false;
bool turning = false;
bool turnEnd = false;
bool LoR = true; //true: going left, false: going right
bool LoRTemp = true;
float bladeAngle = 0;
float bladeAngle_AI = 0;

GLUquadricObj* myCylinder;




///A2 rev  stuff //////////////////////
//assignment 2 stuff

GLuint texture0;

SubdivisionCurve subcurve;
void allocVertexArray(void);
void drawQuadArray(void);

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

GLfloat quadMat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat quadMat_specular[] = { 0.45, 0.55, 0.45, 1.0 };
GLfloat quadMat_diffuse[] = { 0.1, 0.35, 0.1, 1.0 };
GLfloat quadMat_shininess[] = { 10.0 };

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

typedef struct Quad
{
	//int vertex[4]; // 4 indexes into vertex array e.g. 23, 45, 86, 71 means it uses vertices 23 in vertex array etc
	Vertex* vertex[4]; // 4 pointers to vertices in the vertex array
	Vector3D normal; // quad normal vector for this quad
} Quad;

Vertex** vertexArray;
Quad** quadArray;


void initSubdivisionCurve() {
	// Initialize 3 control points of the subdivision curve
	subcurve.controlPoints[0].x = 0;
	subcurve.controlPoints[1].x = 3;
	subcurve.controlPoints[2].x = 4;

	subcurve.controlPoints[0].y = 2;
	subcurve.controlPoints[1].y = 1.8;
	subcurve.controlPoints[2].y = 0;

	subcurve.numControlPoints = 3;
	subcurve.subdivisionSteps = 3;
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
}

void buildVertexArray()
{

	for (int i = 0; i < n_curves; i++)
	{
		//glPushMatrix();
		//glBegin(GL_LINE_STRIP);
		for (int j = 0; j < subcurve.numCurvePoints; j++)
		{
			Vertex curV;
			curV.x = subcurve.curvePoints[j].x * cos(i * n_col * PI / 180);
			curV.z = -subcurve.curvePoints[j].x * sin(i * n_col * PI / 180);
			curV.y = subcurve.curvePoints[j].y;
			vertexArray[i][j] = curV;
			//glVertex3f(curV.x, curV.y, curV.z);
			//printf("%i %f \n", i, curV.x);

			//printf("%i %i \n", i, j);
			n_rows += 1;
		}

	}

}

void buildQuadArray()
{
	for (int i = 0; i < n_curves; i++)
	{
		for (int j = 0; j < subcurve.numCurvePoints - 1; j++)
		{
			quadArray[i][j].vertex[0] = &vertexArray[i][j];
			quadArray[i][j].vertex[1] = &vertexArray[i + 1][j];
			quadArray[i][j].vertex[2] = &vertexArray[i + 1][j + 1];
			quadArray[i][j].vertex[3] = &vertexArray[i][j + 1];


		}
	}



}


void drawQuadArray() {
	computeQuadNormals();
	computeVertexNormals();


	glPushMatrix();
	glTranslatef(0, 1, 1.5);
	glBegin(GL_QUADS);
	for (int i = 0; i < n_curves; i++)
	{
		for (int j = 0; j < subcurve.numCurvePoints - 1; j++)
		{
			if (i == n_curves - 1) {
				glNormal3f(quadArray[i][j].normal.x, quadArray[i][j].normal.y, quadArray[i][j].normal.z);
				glTexCoord2f(0.5 + vertexArray[i][j].x / 4 * 0.5, 0.5 + vertexArray[i][j].z / 4 * 0.5);
				glVertex3f(vertexArray[i][j].x, vertexArray[i][j].y, vertexArray[i][j].z);

				glNormal3f(quadArray[0][j].normal.x, quadArray[0][j].normal.y, quadArray[0][j].normal.z);
				glTexCoord2f(0.5 + vertexArray[0][j].x / 4 * 0.5, 0.5 + vertexArray[0][j].z / 4 * 0.5);
				glVertex3f(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z);

				glNormal3f(quadArray[0][j + 1].normal.x, quadArray[0][j + 1].normal.y, quadArray[0][j + 1].normal.z);
				glTexCoord2f(0.5 + vertexArray[0][j+1].x / 4 * 0.5, 0.5 + vertexArray[0][j+1].z / 4 * 0.5);
				glVertex3f(vertexArray[0][j + 1].x, vertexArray[0][j + 1].y, vertexArray[0][j + 1].z);

				glNormal3f(quadArray[i][j + 1].normal.x, quadArray[i][j + 1].normal.y, quadArray[i][j + 1].normal.z);
				glTexCoord2f(0.5 + vertexArray[i][j + 1].x / 4 * 0.5, 0.5 + vertexArray[i][j + 1].z / 4 * 0.5);
				glVertex3f(vertexArray[i][j + 1].x, vertexArray[i][j + 1].y, vertexArray[i][j + 1].z);

			}
			else {
				glNormal3f(quadArray[i][j].normal.x, quadArray[i][j].normal.y, quadArray[i][j].normal.z);
				glTexCoord2f(0.5 + vertexArray[i][j].x / 4 * 0.5, 0.5 + vertexArray[i][j].z / 4 * 0.5);
				glVertex3f(quadArray[i][j].vertex[0]->x, quadArray[i][j].vertex[0]->y, quadArray[i][j].vertex[0]->z);

				glNormal3f(quadArray[i + 1][j].normal.x, quadArray[i + 1][j].normal.y, quadArray[i + 1][j].normal.z);
				glTexCoord2f(0.5 + vertexArray[i+1][j].x / 4 * 0.5, 0.5 + vertexArray[i+1][j].z / 4 * 0.5);
				glVertex3f(quadArray[i][j].vertex[1]->x, quadArray[i][j].vertex[1]->y, quadArray[i][j].vertex[1]->z);

				glNormal3f(quadArray[i + 1][j + 1].normal.x, quadArray[i + 1][j + 1].normal.y, quadArray[i + 1][j + 1].normal.z);
				glTexCoord2f(0.5 + vertexArray[i+1][j+1].x / 4 * 0.5, 0.5 + vertexArray[i+1][j+1].z / 4 * 0.5);
				glVertex3f(quadArray[i][j].vertex[2]->x, quadArray[i][j].vertex[2]->y, quadArray[i][j].vertex[2]->z);

				glNormal3f(quadArray[i][j + 1].normal.x, quadArray[i][j + 1].normal.y, quadArray[i][j + 1].normal.z);
				glTexCoord2f(0.5 + vertexArray[i][j + 1].x / 4 * 0.5, 0.5 + vertexArray[i][j + 1].z / 4 * 0.5);
				glVertex3f(quadArray[i][j].vertex[3]->x, quadArray[i][j].vertex[3]->y, quadArray[i][j].vertex[3]->z);

			}

		}
	}
	glEnd();
	glPopMatrix();

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



Vector3D crossProduct(Vector3D a, Vector3D b) {
	Vector3D cross;

	cross.x = a.y * b.z - b.y * a.z;
	cross.y = a.x * b.z - b.x * a.z;
	cross.z = a.x * b.y - b.x * a.y;

	return cross;
}

Vector3D fourVectorAverage(Vector3D a, Vector3D b, Vector3D c, Vector3D d) {
	Vector3D average;
	average.x = (a.x + b.x + c.x + d.x) / 4.0;
	average.y = (a.y + b.y + c.y + d.y) / 4.0;
	average.z = (a.z + b.z + c.z + d.z) / 4.0;
	return average;
}

Vector3D normalize(Vector3D a) {
	GLdouble norm = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	Vector3D normalized;
	normalized.x = a.x / norm;
	normalized.y = a.y / norm;
	normalized.z = a.z / norm;
	return normalized;
}


///////////SHADERS/////////////
GLuint g_SimpleShaderProgram = 0;
GLuint g_TexturedDiffuseShaderProgram = 0;

GLint g_uniformMVP = -1;
GLint g_uniformModelMatrix = -1;
GLint g_uniformEyePosW = -1;

GLint g_uniformColor = -1;

// Light uniform variables.
GLint g_uniformLightPosW = -1;
GLint g_uniformLightColor = -1;
GLint g_uniformAmbient = -1;

// Material properties.
GLint g_uniformMaterialEmissive = -1;
GLint g_uniformMaterialDiffuse = -1;
GLint g_uniformMaterialSpecular = -1;
GLint g_uniformMaterialShininess = -1;

GLuint LoadTexture(const std::string& file)
{
	GLuint textureID = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint CreateShaderProgram(std::vector<GLuint> shaders)
{
	// Create a shader program.
	GLuint program = glCreateProgram();

	// Attach the appropriate shader objects.
	for (GLuint shader : shaders)
	{
		glAttachShader(program, shader);
	}

	// Link the program
	glLinkProgram(program);

	// Check the link status.
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];

		glGetProgramInfoLog(program, logLength, NULL, infoLog);

#ifdef _WIN32
		OutputDebugString(infoLog);
#else
		std::cerr << infoLog << std::endl;
#endif

		delete infoLog;
		return 0;
	}

	return program;
}

GLuint LoadShader(GLenum shaderType, const std::string& shaderFile)
{
	std::ifstream ifs;

	// Load the shader.
	ifs.open(shaderFile);

	if (!ifs)
	{
		std::cerr << "Can not open shader file: \"" << shaderFile << "\"" << std::endl;
		return 0;
	}

	std::string source(std::istreambuf_iterator<char>(ifs), (std::istreambuf_iterator<char>()));
	ifs.close();

	// Create a shader object.
	GLuint shader = glCreateShader(shaderType);

	// Load the shader source for each shader object.
	const GLchar* sources[] = { source.c_str() };
	glShaderSource(shader, 1, sources, NULL);

	// Compile the shader.
	glCompileShader(shader);

	// Check for errors
	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetShaderInfoLog(shader, logLength, NULL, infoLog);

#ifdef _WIN32
		OutputDebugString(infoLog);
#else
		std::cerr << infoLog << std::endl;
#endif
		delete infoLog;
		return 0;
	}

	return shader;
}







void InitGLEW()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "There was a problem initializing GLEW. Exiting..." << std::endl;
		exit(-1);
	}

	// Check for 3.3 support.
	// I've specified that a 3.3 forward-compatible context should be created.
	// so this parameter check should always pass if our context creation passed.
	// If we need access to deprecated features of OpenGL, we should check
	// the state of the GL_ARB_compatibility extension.
	if (!GLEW_VERSION_3_3)
	{
		std::cerr << "OpenGL 3.3 required version support not present." << std::endl;
		exit(-1);
	}

#ifdef _WIN32
	if (WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0); // Disable vertical sync
	}
#endif
}

GLuint textureId;
GLuint metalBladeTexture;
GLuint metalBaseTexture;
int main(int argc, char** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("3D Hierarchical Example");

	// Initialize GL
	initOpenGL(vWidth, vHeight);
	InitGLEW();







	GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, "simpleShader.vert");

	GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, "simpleShader.frag");

	std::vector<GLuint> shaders;
	shaders.push_back(vertexShader);
	shaders.push_back(fragmentShader);

	g_SimpleShaderProgram = CreateShaderProgram(shaders);
	assert(g_SimpleShaderProgram);

	// Set the color uniform variable in the simple shader program to white.
	g_uniformColor = glGetUniformLocation(g_SimpleShaderProgram, "color");

	vertexShader = LoadShader(GL_VERTEX_SHADER, "texturedDiffuse.vert");
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, "texturedDiffuse.frag");

	shaders.clear();

	shaders.push_back(vertexShader);
	shaders.push_back(fragmentShader);
	g_TexturedDiffuseShaderProgram = CreateShaderProgram(shaders);
	assert(g_TexturedDiffuseShaderProgram);

	g_uniformMVP = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "ModelViewProjectionMatrix");
	g_uniformModelMatrix = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "ModelMatrix");
	g_uniformEyePosW = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "EyePosW");

	// Light properties.
	g_uniformLightPosW = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "LightPosW");
	g_uniformLightColor = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "LightColor");

	// Global ambient.
	g_uniformAmbient = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "Ambient");

	// Material properties.
	g_uniformMaterialEmissive = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "MaterialEmissive");
	g_uniformMaterialDiffuse = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "MaterialDiffuse");
	g_uniformMaterialSpecular = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "MaterialSpecular");
	g_uniformMaterialShininess = glGetUniformLocation(g_TexturedDiffuseShaderProgram, "MaterialShininess");







	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);


	//
	glEnable(GL_TEXTURE_2D);
	pix1[0].readBMPFile("Mandrill.bmp");
	glGenTextures(1, &textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // store pixels by byte	
	glBindTexture(GL_TEXTURE_2D, textureId); // select current texture (0)
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( // initialize texture
		GL_TEXTURE_2D, // texture is 2-d
		0, // resolution level 0
		GL_RGB, // internal format
		pix1[0].nCols, // image width
		pix1[0].nRows, // image height
		0, // no border
		GL_RGB, // my format
		GL_UNSIGNED_BYTE, // my type
		pix1[0].pixel); // the pixels

	pix2[0].readBMPFile("metal10.bmp");
	glGenTextures(1, &metalBladeTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // store pixels by byte	
	glBindTexture(GL_TEXTURE_2D, metalBladeTexture); // select current texture (0)
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( // initialize texture
		GL_TEXTURE_2D, // texture is 2-d
		0, // resolution level 0
		GL_RGB, // internal format
		pix2[0].nCols, // image width
		pix2[0].nRows, // image height
		0, // no border
		GL_RGB, // my format
		GL_UNSIGNED_BYTE, // my type
		pix2[0].pixel); // the pixels

	pix3[0].readBMPFile("metal10.bmp");
	glGenTextures(1, &metalBaseTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // store pixels by byte	
	glBindTexture(GL_TEXTURE_2D, metalBaseTexture); // select current texture (0)
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( // initialize texture
		GL_TEXTURE_2D, // texture is 2-d
		0, // resolution level 0
		GL_RGB, // internal format
		pix3[0].nCols, // image width
		pix3[0].nRows, // image height
		0, // no border
		GL_RGB, // my format
		GL_UNSIGNED_BYTE, // my type
		pix3[0].pixel); // the pixels

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}
bool collision = false;
bool bladeCollision = false;
bool bladeCollisionAI = false;

float circleColSize = 5.0f;

//blades
//located at 0,0,bodySizez * 1.5 with r=5
float pBladeX, pBladeZ;
float bBladeX, bBladeZ;

//HP
int pHP = 100, bHP = 100;

bool checkCollision(void);
bool checkCollision()
{

	//if player and bot interfere with circle collider
	if (sqrt(pow(((X-20)- (X_AI + 20)), 2) + pow((Z- Z_AI), 2)) < 8) return true;
	else return false;
}

bool checkCollisionBlade(void);
bool checkCollisionBlade()
{
	

	pBladeX = X - 20 + bodySizez * 1.5 *sinf(bodyAngle * 2.0 * PI / 180);
	pBladeZ = Z + bodySizez * 1.5 * cosf(bodyAngle * 2.0 * PI / 180);

	//Players blade is hitting AI's body
	if (sqrt(pow((X_AI + 20 - pBladeX), 2) + pow((Z_AI - pBladeZ), 2)) < 10) {
		return true;
	}
	else return false;
	
	

}

bool checkCollisionAIBlade(void);
bool checkCollisionAIBlade()
{
	bBladeX = X_AI + 20 + bodySizez * 1.5 * sinf(bodyAngle_AI * 2.0 * PI / 180);
	bBladeZ = Z_AI + bodySizez * 1.5 * cosf(bodyAngle_AI * 2.0 * PI / 180);

	//bots blade is hitting player base
	if (sqrt(pow((bBladeX - (X - 20)), 2) + pow((bBladeZ - Z), 2)) < 10)
	{
		return true;
	}
	else return false;
}

bool stopBlade = true;
void removeHP(int param);
void removeHP(int param)
{
	static GLfloat u = 0.0;
	u += 0.01;
	if (collision) {
		printf("collided each other \n");
		pHP -= rand() % 2 + 1;
		bHP -= rand() % 2 + 1;
	}
	if (bladeCollision)
	{
		if (!stopBlade) {
			bHP -= rand() % 5 + 1;
			printf("blade is spinning \n");
		}
	}
	if (bladeCollisionAI)
	{
		pHP -= rand() % 5 + 1;
	}
	glutPostRedisplay();
	if (pHP > 0 && bHP > 0) {
		printf("PLAYER HP: %i         AI HP: %i \n", pHP, bHP);
		glutTimerFunc(500, removeHP, 0);
	}
	else if (pHP <= 0) printf("AI WINS! \n");
	else if (bHP <= 0) printf("PLAYER WINS! \n");
	else printf("DRAW! \n");
}

// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	printf("Press F1 for help! \n");
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// Other initializatuion
	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-16.0f, 0.0f, 16.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 32.0);
	groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);
	
	myCylinder = gluNewQuadric();
	gluQuadricNormals(myCylinder, GLU_SMOOTH);

	initSubdivisionCurve();
	computeSubdivisionCurve(&subcurve);
	allocVertexArray();

	glutTimerFunc(10, spinAnimationAI, 0);
	glutTimerFunc(5, removeHP, 0);
}

bool enableBotMovement = true;


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	if (!fp) gluLookAt(eyeXinit, eyeYinit, eyeZinit, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	else {
		eyeXfp = X - 20 + 5 * sinf(bodyAngle * 2.0 * PI / 180);
		eyeYfp = Y + 2;
		eyeZfp = Z + 5 * cosf(bodyAngle * 2.0 * PI / 180);
		gluLookAt(eyeXfp, eyeYfp, eyeZfp, eyeXfp + 10 * sinf(bodyAngle * 2.0 * PI / 180), 0.0, eyeZfp + 10 * cosf(bodyAngle * 2.0 * PI / 180), 0.0, 1.0, 0.0);
	}

	if (enableBotMovement && bHP > 0)botMovement();
	collision = checkCollision();
	bladeCollision = checkCollisionBlade();
	bladeCollisionAI = checkCollisionAIBlade();

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glRotatef(cubeAngle, 0.0, 1.0, 0.0);

	glPushMatrix(); 
	glTranslatef(8.0, 0, 3.0);
	drawCubeMesh(cubeMesh);
	glPopMatrix();

	//////////PLAYER ROBOT
	glPushMatrix();

	//just rotation
	glTranslatef(X-20, Y, Z);
	glRotatef(bodyAngle*2, 0, 1, 0);
	
	//A2 part
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	drawQuadArray();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, metalBaseTexture);

	//draw rob body
	glPushMatrix();

	glTranslatef(0.0, 0.0, bodyLocz);
	glScalef(bodySizex/2, bodySizey/2, bodySizez/2);
	glBegin(GL_QUADS);  

	// Top
	glNormal3f(0, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom 
	glNormal3f(0, -1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Front 
	glNormal3f(0, 0, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back 
	glNormal3f(0, 0, -1);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left 
	glNormal3f(-1, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right 
	glNormal3f(1, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();
	glPopMatrix();

	
	//draw fronts
	drawFront(0.0, 0.8, bodyLocz+bodySizez/2);
	drawFront(0.0, -0.8, bodyLocz+bodySizez/2);

	//draw front cylinder
	glPushMatrix();
	glTranslatef(0, 0.95, bodySizez*1.5);
	glRotatef(90, 1, 0, 0);
	gluQuadricDrawStyle(myCylinder, GLU_LINE);
	gluCylinder(myCylinder, 0.3f, 0.3f, 1.5f, 50, 50);
	glPopMatrix();

	//draw blade
	glBindTexture(GL_TEXTURE_2D, metalBladeTexture);
	drawBlade(0, 0, bodySizez*1.5, bladeAngle);

	//draw wheels
	drawWheel(-bodySizex / 2 - 1, 0, 0, false); //right wheel
	drawWheel(bodySizex / 2 + 0.5, 0, 0, true); //left wheel

	//draw axle
	drawAxle(0, 0, 0, 9);
	
	glPopMatrix();

















	///////AI ROBOT
	glPushMatrix();

	//just rotation
	glTranslatef(X_AI+20, Y_AI, Z_AI);
	glRotatef(bodyAngle_AI * 2, 0, 1, 0);

	//A2 part
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	drawQuadArray();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, metalBaseTexture);

	//draw rob body
	glPushMatrix();

	glTranslatef(0.0, 0.0, bodyLocz);
	glScalef(bodySizex / 2, bodySizey / 2, bodySizez / 2);
	glBegin(GL_QUADS);

	// Top
	glNormal3f(0, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom 
	glNormal3f(0, -1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Front 
	glNormal3f(0, 0, 1);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back 
	glNormal3f(0, 0, -1);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left 
	glNormal3f(-1, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right 
	glNormal3f(1, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1, -1);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();
	glPopMatrix();


	//draw fronts
	drawFront(0.0, 0.8, bodyLocz + bodySizez / 2);
	drawFront(0.0, -0.8, bodyLocz + bodySizez / 2);

	//draw front cylinder
	glPushMatrix();
	glTranslatef(0, 0.95, bodySizez * 1.5);
	glRotatef(90, 1, 0, 0);
	gluQuadricDrawStyle(myCylinder, GLU_LINE);
	gluCylinder(myCylinder, 0.3f, 0.3f, 1.5f, 50, 50);
	glPopMatrix();

	//draw blade
	glBindTexture(GL_TEXTURE_2D, metalBladeTexture);
	drawBlade(0, 0, bodySizez * 1.5, bladeAngle_AI);

	//draw wheels
	drawWheel(-bodySizex / 2 - 1, 0, 0, false); //right wheel
	drawWheel(bodySizex / 2 + 0.5, 0, 0, true); //left wheel

	//draw axle
	drawAxle(0, 0, 0, 9);

	glPopMatrix();





	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -2, 0.0);
	glScalef(3, 1, 3);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}



void drawFront(float locx, float locy, float locz)
{

	glPushMatrix();
	glTranslatef(locx, locy, locz);
	glBegin(GL_QUADS);
	//behind
	glNormal3f(0, 0, -1);
	glTexCoord2f(-1, 1);
	glVertex3f(-4.0f, 0.2f, 0.0f);
	glTexCoord2f(1, 1);
	glVertex3f(4.0f, 0.2f, 0.0f);
	glTexCoord2f(1, -1);
	glVertex3f(4.0f, -0.2f, 0.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-4.0f, -0.2f, 0.0f);

	//left
	glNormal3f(-6, 0, 4);
	glTexCoord2f(-1, 1);
	glVertex3f(-4.0f, 0.2f, 0.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(-4.0f, -0.2f, 0.0f);
	glTexCoord2f(1, -1);
	glVertex3f(0.0f, -0.2f, 6.0f);
	glTexCoord2f(1, 1);
	glVertex3f(0.0f, 0.2f, 6.0f);

	//right
	glNormal3f(6, 0, 4);
	glTexCoord2f(1, 1);
	glVertex3f(4.0f, 0.2f, 0.0f);
	glTexCoord2f(-1, 1);
	glVertex3f(0.0f, 0.2f, 6.0f);
	glTexCoord2f(-1, -1);
	glVertex3f(0.0f, -0.2f, 6.0f);
	glTexCoord2f(1, -1);
	glVertex3f(4.0f, -0.2f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3f(0, 1, 0);
	glTexCoord2f(-1, 1);
	glVertex3f(-4.0f, 0.2f, 0.0f);
	glTexCoord2f(0, -1);
	glVertex3f(0.0f, 0.2f, 6.0f);
	glTexCoord2f(1, 1);
	glVertex3f(4.0f, 0.2f, 0.0f);

	glNormal3f(0, -1, 0);
	glTexCoord2f(-1, 1);
	glVertex3f(-4.0f, -0.2f, 0.0f);
	glTexCoord2f(1, 1);
	glVertex3f(4.0f, -0.2f, 0.0f);
	glTexCoord2f(0, -1);
	glVertex3f(0.0f, -0.2f, 6.0f);
	glEnd();

	glPopMatrix();

}

void drawBlade(float locx, float locy, float locz, float rot)
{
	glPushMatrix();
	glTranslatef(locx, locy, locz);
	glRotatef(rot, 0, 1, 0);
	glBegin(GL_QUADS);
	
	//top
	glNormal3f(0, 1, 0);
	glTexCoord2f(1, -1);
	glVertex3f(4.5, 0.15, -0.5); //A
	glTexCoord2f(-1, 1);
	glVertex3f(-4.5, 0.15, -0.5); //B
	glTexCoord2f(-1, -1);
	glVertex3f(-4.5, 0.15, 0.5); //C
	glTexCoord2f(1, -1);
	glVertex3f(4.5, 0.15, 0.5); //D

	//bottom
	glNormal3f(0, -1, 0);
	glTexCoord2f(1, -1);
	glVertex3f(5, -0.15, 0.75);	//E
	glTexCoord2f(-1, 1);
	glVertex3f(-5, -0.15, 0.75); //F
	glTexCoord2f(-1, -1);
	glVertex3f(-5, -0.15, -0.75); //G
	glTexCoord2f(1, -1);
	glVertex3f(5, -0.15, -0.75); //H

	//left CBGF
	glNormal3f(-0.3, 0.5, 0);
	glTexCoord2f(1, -1);
	glVertex3f(-4.5, 0.15, 0.5); //C
	glTexCoord2f(-1, 1);
	glVertex3f(-4.5, 0.15, -0.5); //B
	glTexCoord2f(-1, -1);
	glVertex3f(-5, -0.15, -0.75); //G
	glTexCoord2f(1, -1);
	glVertex3f(-5, -0.15, 0.75); //F

	//right ADEH
	glNormal3f(0.3, 0.5, 0);
	glTexCoord2f(1, -1);
	glVertex3f(4.5, 0.15, -0.5); //A
	glTexCoord2f(-1, 1);
	glVertex3f(4.5, 0.15, 0.5); //D
	glTexCoord2f(-1, -1);
	glVertex3f(5, -0.15, 0.75);	//E
	glTexCoord2f(1, -1);
	glVertex3f(5, -0.15, -0.75); //H

	//front CFED
	glNormal3f(0, 2.5, 3);
	glTexCoord2f(1, -1);
	glVertex3f(-4.5, 0.15, 0.5); //C
	glTexCoord2f(-1, 1);
	glVertex3f(-5, -0.15, 0.75); //F
	glTexCoord2f(-1, -1);
	glVertex3f(5, -0.15, 0.75);	//E
	glTexCoord2f(1, -1);
	glVertex3f(4.5, 0.15, 0.5); //D

	//back AHGB
	glNormal3f(0, 2.5, -3);
	glTexCoord2f(1, -1);
	glVertex3f(4.5, 0.15, -0.5); //A
	glTexCoord2f(-1, 1);
	glVertex3f(5, -0.15, -0.75); //H
	glTexCoord2f(-1, -1);
	glVertex3f(-5, -0.15, -0.75); //G
	glTexCoord2f(1, -1);
	glVertex3f(-4.5, 0.15, -0.5); //B

	glEnd();

	


	glPopMatrix();
}

void drawWheel(float locx, float locy, float locz, bool LeftorRight)
{
	gluQuadricTexture(myCylinder, true);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);
	glTranslatef(locx, locy, locz);

	if (moving)
	{
		glRotatef(wheelAngleL, 1, 0, 0);
		glRotatef(wheelAngleR, 1, 0, 0);
	}
	else if (turning) { //correct wheel rotates when moving
		if (LeftorRight) glRotatef(wheelAngleL, 1, 0, 0);
		else glRotatef(wheelAngleR, 1, 0, 0);
	}

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	gluQuadricDrawStyle(myCylinder, GLU_LINE);
	gluCylinder(myCylinder, 1.5f, 1.5f, 0.5f, 50, 50);
	glPopMatrix();

	//small parts for sides
	glPushMatrix();
	if (!LeftorRight) glTranslatef(0, 0.7, 0.7);
	else glTranslatef(0.5, 0.7, 0.7);
	glutSolidSphere(0.3, 10, 10);
	glPopMatrix();

	glPushMatrix();
	if (!LeftorRight) glTranslatef(0, -0.7, -0.7);
	else glTranslatef(0.5, -0.7, -0.7);
	glutSolidSphere(0.3, 10, 10);
	glPopMatrix();

	//side faces
	glPushMatrix();
	glRotatef(90, 0, -1, 0);
	gluQuadricDrawStyle(myCylinder, GLU_LINE);
	gluDisk(myCylinder, 0.0f, 1.5f, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, 0, 0);
	glRotatef(90, 0, 1, 0);
	gluQuadricDrawStyle(myCylinder, GLU_LINE);
	gluDisk(myCylinder, 0.0f, 1.5f, 50, 50);
	glPopMatrix();
	glPopMatrix();
}

void drawAxle(float locx, float locy, float locz, float length)
{
	glPushMatrix();
	glTranslatef(locx-length/2, locy, locz);
	glRotatef(90, 0, 1, 0);
	gluQuadricDrawStyle(myCylinder, GLU_LINE);
	gluCylinder(myCylinder, 0.2f, 0.2f, length, 50, 50);
	glPopMatrix();
}


void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
		if (key == 'e') {
			stop = false;
			glutTimerFunc(10, animationHandler, 0);
		}
		else if (key == 'E') {
			stop = true;
		}
		else if (key == ' ') {
			stopBlade = false;
			glutTimerFunc(10, spinAnimation, 0);
		}
		else if (key == 'f') {
			stopBlade = true;
		}
		else if (key == 'v') //First Person Camera
		{
			if (fp) fp = false;
			else fp = true;
		}
		else if (key == 'b')
		{
			if (enableBotMovement) enableBotMovement = false;
			else enableBotMovement = true;
		}
		else if (pHP > 0) {
			if (key == 'w' && (!collision)) {
				tempX = X;
				tempZ = Z;
				X -= 0.5 * sinf(-bodyAngle * 2.0 * PI / 180);
				Y += 0;
				Z += 0.5 * cosf(-bodyAngle * 2.0 * PI / 180);
				moving = true;
				turning = false;
				wheelAngleL += 20;
				wheelAngleR += 20;
				glutPostRedisplay();
			}
			else if (key == 's') {
				tempX = X;
				tempZ = Z;
				X += 0.5 * sinf(-bodyAngle * 2.0 * PI / 180);
				Y -= 0;
				Z -= 0.5 * cosf(-bodyAngle * 2.0 * PI / 180);
				moving = true;
				turning = false;
				wheelAngleL -= 20;
				wheelAngleR -= 20;
				glutPostRedisplay();
			}

			else if (key == 'a') {
				bodyAngle += 2;
				wheelAngleR += 20;
				moving = false;
				turning = true;
				LoR = true;
				glutPostRedisplay();
			}
			else if (key == 'd') {
				bodyAngle -= 2;
				wheelAngleL += 20;
				moving = false;
				turning = true;
				LoR = false;
				glutPostRedisplay();
			}
		}
		else
		{
			moving = false;
			turning = false;
		}

	glutPostRedisplay();   // Trigger a window redisplay
}

int state = 0;
int targetAngle;
bool AIturn = false;
int bodyAngle_AIinit = 0;
int AIdistance = 0;
int targetDistance;
float botSpeed = 0.5;
bool collideInit = false;
bool forw = true;
int t = 50;

void botMovement()
{
	//bodyAngle_AI
	//X_AI, Y_AI, Z_AI
	if (bHP < 50) botSpeed = 0.2;
	if (collision)
	{
		if (!collideInit)
		{
			collideInit = true;
			targetDistance = (rand() % 3 + 1) * 20;
			AIdistance = 0;
			if (X-20 < X_AI+20)	forw = true;
			else forw = false;
		}
		if (t <= 0)
		{
			if (forw)
			{
				X_AI -= botSpeed * sinf(-bodyAngle_AI * 2.0 * PI / 180);
				Z_AI += botSpeed * cosf(-bodyAngle_AI * 2.0 * PI / 180);

				AIdistance += 1;
			}
			else
			{
				X_AI -= -botSpeed * sinf(-bodyAngle_AI * 2.0 * PI / 180);
				Z_AI += -botSpeed * cosf(-bodyAngle_AI * 2.0 * PI / 180);

				AIdistance += 1;
			}

			if (AIdistance >= targetDistance) state = 0;
		}
		else t -= 1;
	}
	else
	{
		t = 50;
		collideInit = false;
		if (state == 1) { //bot is out of bounds
			//turn around
			if (AIturn && targetAngle != bodyAngle_AIinit)
			{
				bodyAngle_AI += 2;
				bodyAngle_AIinit += 2;
			}
			//move
			else
			{
				X_AI -= botSpeed * sinf(-bodyAngle_AI * 2.0 * PI / 180);
				Z_AI += botSpeed * cosf(-bodyAngle_AI * 2.0 * PI / 180);

				AIdistance += 1;
				if (AIdistance >= targetDistance) state = 0;
			}

		}
		else if (X_AI >35 || X_AI < -35 || Z_AI > 40 || Z_AI < -40) { //if bot is located out of bounds
			state = 1;
			//targetAngle = sinf(X_AI / Z_AI) * 180 / PI;
			bodyAngle_AIinit = 0;
			targetAngle = 90;
			AIturn = true;
			AIdistance = 0;
			targetDistance = (rand() % 3 + 1) * 20;
		}
		else if (state == 0) { //bot is turning

		
			if (!AIturn)
			{
				//if idle, either turn
				AIturn = true;
				bodyAngle_AIinit = 0;
				targetAngle = (rand() % 90 - 45) * 2; //random angle from -180 to 180, incrementing by 2

			}

			if (AIturn && targetAngle != bodyAngle_AIinit)
			{
				if (0 < targetAngle) {
					bodyAngle_AI += 2;
					bodyAngle_AIinit += 2;
				}

				else if (targetAngle == 0) AIturn = false;
				else {
					bodyAngle_AI -= 2;
					bodyAngle_AIinit -= 2;
				}
			}
			else if (bodyAngle_AIinit == targetAngle)
			{
				AIturn = false;
				state = 2; //move bot after done turning
				AIdistance = 0;
				targetDistance = (rand() % 5 + 2) * 15;
			}
		}
		else if (state == 2) { //moving bot

			//made the bot move slower
			X_AI -= botSpeed * sinf(-bodyAngle_AI * 2.0 * PI / 180);
			Z_AI += botSpeed * cosf(-bodyAngle_AI * 2.0 * PI / 180);

			AIdistance += 1;
			if (AIdistance >= targetDistance) state = 0; //back to turning after moving		
		}
	}
	
}

void animationHandler(int param)
{
	if (!stop)
	{
		shoulderAngle += 1.0;
		cubeAngle += 2.0;
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}

void spinAnimation(int param)
{
	if (!stopBlade)
	{
		bladeAngle += 10.0;
	}
	glutPostRedisplay();
	glutTimerFunc(10, spinAnimation, 0);
}

void spinAnimationAI(int param)
{
	bladeAngle_AI += 40.0;
	glutPostRedisplay();
	glutTimerFunc(10, spinAnimationAI, 0);
}

void spinAnimationAIBody(int param)
{
	//bodyAngle_AI 
}


// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("F1 Help: \n");
		printf("	w: forward \n");
		printf("	a: turn left \n");
		printf("	s: baclward \n");
		printf("	d: turn right");
		printf("\n");
		printf("	Spacebar: rotate blade (press multiple times for faster rotation)\n");
		printf("	f: stop blade rotation \n");
		printf("	v: Toggle first person view camera \n");
		printf("\n");
		printf("	e: rotate scene\n");
		printf("	E: stop scene rotation \n");
		printf("	b: stop AI movements \n");
		printf("\n");
		printf("	Uppercase matters! \n");

	}
	else if (key == GLUT_KEY_LEFT)
	{

	}
	else if (key == GLUT_KEY_RIGHT)
	{
		   
	}
	else if (key == GLUT_KEY_UP)
	{

	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}