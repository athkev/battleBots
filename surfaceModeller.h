typedef struct Vector2D
{
	GLdouble x, y;
	GLdouble nx, ny;
	GLdouble u; // running distance
} Vector2D;

typedef struct Vector3D
{
	GLdouble x, y, z;
} Vector3D;

// Struct prototypes
Vector3D crossProduct(Vector3D a, Vector3D b);
Vector3D fourVectorAverage(Vector3D a, Vector3D b, Vector3D c, Vector3D d);

typedef struct SubdivisionCurve SubdivisionCurve;

// Function prototypes
void init2DCurveWindow();
void display();
void reshape(int w, int h);
void mouseButtonHandler(int button, int state, int x, int y);
void mouseMotionHandler(int x, int y);
void keyboardHandler(unsigned char key, int x, int y);
void screenToCameraCoordinates(int xScreen, int yScreen, GLdouble *xCamera, GLdouble *yCamera);
void cameraToWorldCoordinates(GLdouble xcam, GLdouble ycam, GLdouble *xw, GLdouble *yw);
void screenToWorldCoordinates(int xScreen, int yScreen, GLdouble *xw, GLdouble *yw);
void draw2DScene();
void drawAxes();
void drawSubdivisionCurve();

Vector2D getPoint(Vector2D *currentPoints, int numCurrentPoints, int j) ;
Vector2D subdivide(Vector2D v1, Vector2D v2, Vector2D v3, Vector2D v4) ;
void computeSubdivisionCurve(SubdivisionCurve *subcurvePointer) ;

void specialKeyHandler(int key, int x, int y);
void initSubdivisionCurve();
void constructCircle(double radius, int numPoints, Vector2D* circlePts);
void drawControlPoints();
void initControlPoints();
void mouseHoverHandler(int x, int y);
void mouseScrollWheelHandler(int button, int dir, int xMouse, int yMouse);
void worldToCameraCoordinates(GLdouble xWorld, GLdouble yWorld, GLdouble *xcam, GLdouble *ycam);


void init3DSurfaceWindow();
void display3D();
void reshape3D(int w, int h);
void mouseButtonHandler3D(int button, int state, int x, int y);
void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse);
void mouseMotionHandler3D(int xMouse, int yMouse);

void buildVertexArray();
Vector3D rotateAroundY(double x, double z, double theta);
void drawGround();
void buildQuadArray();
Vector3D normalize(Vector3D a);
void computeQuadNormals();
void computeVertexNormals();

