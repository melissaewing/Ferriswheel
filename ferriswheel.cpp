// Melissa EWing
// Draws a ferris wheel with hierarchical modeling and allows the user to move around 
// to view it at various positions, change the light position, and affect the speed and direction rotation

#include <GL/glui.h>
#include <GL/glut.h>
#include <math.h>

#include <stddef.h>

#include <ctime>

//important constants for scene
#define PI 3.14159265 
#define FrameRate 60
#define WIDTH 1000
#define HEIGHT 1000

//constants for ferris wheel size and specifications
#define RADIUS 40
#define NUM_CHAIRS 18 
#define WHEEL_POSX 0
#define WHEEL_POSY 2
#define WHEEL_POSZ 0
#define CHAIR_WIDTH 4.4 
#define CHAIR_LENGTH 2
#define CHAIR_HEIGHT 2.5
#define CHAIR_THICKNESS .4

//viewer's x, y, and z coordinates of location
GLfloat X = 65; 
GLfloat Y = 32; 
GLfloat Z = -117;

//angle of rotation and incrementing angle for speed of rotation
GLfloat theta = 0; 
GLfloat alpha = 0; 
//direction of rotation -- 0 for clockwise, 1 for counterclockwise
int direction = 0;
//used for idle function
GLfloat oldTime = 0;
//speed of rotation--used for callback
int speed = 0;
//boolean to check is wheel is rotating
bool isRotating = false; 

//instantiate quadrics
GLUquadricObj *p = gluNewQuadric();
GLUquadricObj *q = gluNewQuadric();

//material struct to define properties for materials
typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess;
} materialStruct;

materialStruct redPlasticMaterials = {
  {0.3, 0.0, 0.0, 1.0},
  {0.8, 0.0, 0.0, 1.0},
  {0.9, 0.9, 0.9, 1.0},
  64.0};

materialStruct blackPlasticMaterials = {
  {0.3, 0.0, 0.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.9, 0.9, 0.9, 1.0},
  64.0};

materialStruct darkBrownPlasticMaterials = {
  {0.3, 0.0, 0.0, 1.0},
  {0.4, 0.15, 0.05, 1.0},
  {0.9, 0.9, 0.9, 1.0},
  64.0};

materialStruct lightBrownPlasticMaterials = {
  {0.3, 0.0, 0.0, 1.0},
  {0.8, 0.5, 0.25, 1.0},
  {0.9, 0.9, 0.9, 1.0},
  64.0};

//light position
GLfloat lightPos[4] = {-208, 17.7, -173, 1.0};

//struct to define light's properties
typedef struct lightingStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
} lightingStruct;

//white light used for the scene
lightingStruct whiteLighting = {
  {0.0, 0.0, 0.0, 1.0},
  {1.0, 1.0, 1.0, 1.0},
  {1.0, 1.0, 1.0, 1.0} };

//variables to store current lighting and current material being used
materialStruct *currentMaterials;
lightingStruct *currentLighting;

int main_window;

//initializes scene
void init() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glColor3f(0.0, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(X, Y, Z, 2.0, 0.0, 0.0, 0.0,  1.0, 0.0);
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);

  float aspect = w*1.0/h;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0, aspect, 5, 800.0);
  glMatrixMode(GL_MODELVIEW);
  glutSetWindow(main_window);
  glutPostWindowRedisplay(main_window);  
}

void myGlutIdle(void) {
  if (glutGetWindow() != main_window)
    glutSetWindow(main_window);
  glutPostRedisplay();
}

//changes the material of an object
void changeMaterial(materialStruct material) {
  currentMaterials = &material;
  glMaterialfv(GL_FRONT, GL_AMBIENT, currentMaterials->ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, currentMaterials->diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, currentMaterials->specular);
  glMaterialf(GL_FRONT, GL_SHININESS, currentMaterials->shininess);
}

//draws a single chair
void drawChair(int col, GLfloat chairTheta, GLfloat x, GLfloat y, GLfloat z, GLfloat length, GLfloat height, GLfloat thickness, GLfloat width) {
  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(chairTheta, 0.0, 0.0, 1.0);
  if (isRotating) {
    glRotatef(-theta, 0.0, 0.0, 1.0);
  }
    //draws back of chair
  glPushMatrix();
  glTranslatef(thickness/4, -height/2, width/2);
  glScalef(thickness, height, width);
  glutSolidCube(1.0);
  glPopMatrix();
  glPushMatrix();
     //draws seat of chair
  glTranslatef(length/2 - thickness/4, thickness/2-height, width/2);
  glScalef(length, thickness, width);
  glutSolidCube(1.0);
  glPopMatrix();
  glPopMatrix();
}

//draws a single wheel
void drawWheel(int col, GLfloat radius, GLfloat x, GLfloat y, GLfloat z) {
  glPushMatrix();
  glTranslatef(x, y, z); 
  //if boolean is set to true, then draw wheel with rotation theta
  if (isRotating == 1) {
    glRotatef(theta, 0.0, 0.0, 1.0);
  }
  glTranslatef(0, 0, .25);
  glPushMatrix();
  //draw concentric rings around wheel
  changeMaterial(darkBrownPlasticMaterials);
  glutSolidTorus(.5, radius, 70, 70);
  glutSolidTorus(.5, 2*radius/3, 70, 70);
  glutSolidTorus(.5, radius/3, 70, 70);
  int circles = (int) radius/3;
  //draw smaller concentric rings (outermost 3rd section of wheel)
  for (int j = 0; j < circles; j+=3) {
  	if (j%2 == 1)
		changeMaterial(blackPlasticMaterials);  
	else
		changeMaterial(lightBrownPlasticMaterials);
	glutSolidTorus(.2, 2*radius/3 + j, 70, 70);
  }
  glPopMatrix();
  GLfloat angle =  0;
  glRotatef(90, 90, 0, 1);
  //draw middle 3rd of wheel in pie-slice shaped sections based on number of chairs on ferris wheel
  for (int i = 0; i < NUM_CHAIRS/3; i++) {
        glRotatef(angle, 0, angle, 1.0);
	changeMaterial(darkBrownPlasticMaterials);
	gluCylinder(p, .4, .4, radius, 10, 10);
	GLfloat angle2 = (360/(NUM_CHAIRS/3)/3); 
	GLfloat angle3 = 2*angle2/3;
	GLfloat radius2 = 2*radius/3;
	//law of cosines to determine side length of larger spoke based on surrounding spokes!
	GLfloat C = sqrt(pow(radius2/2, 2) + pow(radius2, 2) - 2*(pow(radius2, 2))*cos(angle3*PI/180)/2);
	//law of sines to determine corresponding angle!
	GLfloat B = asin(radius2*sin(angle3*PI/180)/C)*180/PI;
	//law of cosines for smaller spoke
        GLfloat C2 = sqrt(pow(3*radius2/4, 2) + pow(radius2, 2) - 6*(pow(radius2, 2))*cos(angle3*PI/360)/4);
        GLfloat B2 = asin(radius2*sin(angle3*PI/360)/C2)*180/PI;
	glPushMatrix();
        glPushMatrix();
        glPushMatrix();
	//draw individual spokes in a section
	glTranslatef(0, 0, radius/3);
	glRotatef(B, 0, B, 1.0);
        gluCylinder(q, .2, .2, C, 10, 10);
	glPopMatrix();
        glPushMatrix();
	changeMaterial(darkBrownPlasticMaterials);
        glRotatef(B2, 0, B2, 1.0);
        gluCylinder(q, .2, .2, 2*radius/3, 10, 10);
	changeMaterial(lightBrownPlasticMaterials);
        glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0, radius/2);
        glRotatef(B2, 0, B2, 1.0);
        gluCylinder(q, .2, .2, C2, 10, 10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0, radius/3);
	glRotatef(360-B, 0, 360-B, 1.0);
        gluCylinder(q, .2, .2, C, 10, 10);
	glPopMatrix();
	glPushMatrix();
	changeMaterial(darkBrownPlasticMaterials);
        glRotatef(360-B2, 0, 360-B2, 1.0);
	gluCylinder(q, .2, .2, 2*radius/3, 10, 10);
	changeMaterial(lightBrownPlasticMaterials);
	glPopMatrix();
	glPushMatrix();
        glTranslatef(0, 0, radius/2);
        glRotatef(360-B2, 0, 360-B2, 1.0);
        gluCylinder(q, .2, .2, C2, 10, 10);
        glPopMatrix();
	glPopMatrix();
	//draw smaller individual spokes
	for (int k = 0; k < NUM_CHAIRS/9; k++) {
		glRotatef(angle2, 0, angle2, 1.0);
		gluCylinder(q, .2, .2, 2*radius/3, 10, 10);
                glPushMatrix();
	        glRotatef(0, 0, angle2/3, 1.0);
                glTranslatef(0, 0, radius/3);
                glRotatef(B, 0, B, 1.0);
                gluCylinder(q, .2, .2, C, 10, 10);
                glPopMatrix();
       		glPushMatrix();
        	glRotatef(B2, 0, B2, 1.0);
       	 	gluCylinder(q, .2, .2, 2*radius/3, 10, 10);
        	glPopMatrix();
		glPushMatrix();
        	glTranslatef(0, 0, radius/2);
        	glRotatef(B2, 0, B2, 1.0);
        	gluCylinder(q, .2, .2, C2, 10, 10);
        	glPopMatrix();
		glPushMatrix();
        	glTranslatef(0, 0, radius/3);
	        glRotatef(360-B, 0, 360-B, 1.0);
	        gluCylinder(q, .2, .2, C, 10, 10);
	        glPopMatrix();
        	glPushMatrix();
        	glRotatef(360-B2, 0, 360-B2, 1.0);
        	gluCylinder(q, .2, .2, 2*radius/3, 10, 10);
	        glPopMatrix();
		glPushMatrix();
	        glTranslatef(0, 0, radius/2);
	        glRotatef(360-B2, 0, 360-B2, 1.0);
	        gluCylinder(q, .2, .2, C2, 10, 10);
	        glPopMatrix();
	}
 	glPopMatrix();
	//angle of rotation to start drawing next "pie slice" in next iteration of the for-loop
	angle = 360/(NUM_CHAIRS/3);
  }
  glPopMatrix();
}
  
//method to draw the ferris wheel in its entirety
void drawFerrisWheel() {
  //draw two wheels the distance of the chair's width apart
  drawWheel(1, RADIUS, WHEEL_POSX, WHEEL_POSY, WHEEL_POSZ);
  drawWheel(1, RADIUS, WHEEL_POSX, WHEEL_POSY, WHEEL_POSZ + CHAIR_WIDTH);
  GLfloat angle = 0; //angle used to keep track of current chair's position (rotation) relative to static wheel
  glPushMatrix();
  glTranslatef(WHEEL_POSX/2, WHEEL_POSY/2, 0);
  changeMaterial(redPlasticMaterials);
  //draw chairs around the edge of the ferris wheel
  for (int i = 0; i < NUM_CHAIRS; i++) {
    glPushMatrix();
    //if ferris wheel is rotating, add theta (ferris wheel's current rotation) to angle of chair's position
    if (isRotating == 1) {
      glRotatef(theta + angle, 0.0, 0.0, 1.0);
    }
    //otherwise, just draw chair based on its position relative to static wheel
    else {
      glRotatef(angle, 0.0, 0.0, 1.0);
    }
    //draw chair radius distance from current position on the y axis
    drawChair(2, -angle, 0, RADIUS, 0, CHAIR_LENGTH, CHAIR_HEIGHT, CHAIR_THICKNESS, CHAIR_WIDTH);
    glPopMatrix();
    //increment angle to draw next chair on next iteration of for-loop
    angle += 360.0/NUM_CHAIRS;
  }
  glPopMatrix();
}

void display() {
  glutSetWindow(main_window);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  drawFerrisWheel();
  glutSwapBuffers(); 
}

//method to increment rotation of ferris wheel 
void spinDisplay() {
  //if boolean is set to true, then theta is incremented/decremented and  wheel rotates 
  if(isRotating) {
	//if counterclockwise, subtract alpha from theta -- larger alpha moves wheel at faster speed
	if (direction == 1) {
	    theta -= alpha;
	    //if rotation exceeds degrees in circle, then set to 0
	    if (theta <= -360) {
	    	theta = 0;
	    }
	}
	//otherwise, if clockwise, add alpha to theta
	else if (direction == 0) {
	    theta += alpha;
	    if (theta >= 360) {
	      theta = 0;
	    }
	}
  }
  display();
}

//function to smooth motion based on frame rate
void idle() {
  long currentTime;
  currentTime = clock();
  if ((currentTime-oldTime) > CLOCKS_PER_SEC/FrameRate) {
    spinDisplay;
    oldTime=currentTime;
  }
}

//viewer callback to change eye position
void V_callback(int ID) {
  init();
  display();
}

//callback to change light position
void light_callback(int ID) {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  display();
  glutSwapBuffers();
}

//callback to change speed of rotation
void speed_callback(int ID) {
  switch (speed) {
  case 0:
    isRotating = false;
    alpha = 0;
    break;
  case 1:
    isRotating = true;
    alpha = .1;
    break;
  case 2:
    isRotating = true;
    alpha = .5;
    break;
  case 3:
    isRotating = true;
    alpha = 1.0;
    break;
  case 4:
    isRotating = true;
    alpha = 3.0;
    break;
  }	
  spinDisplay();  
}

//callback to change direction of rotation
void direction_callback(int ID) {
  switch (direction) {
  case 0:
     //clockwise rotation
     direction = 0;
     break;
  case 1:
     //counterclockwise rotation
     direction = 1;
     break;
  }
  spinDisplay();
}

int main(int argc, char **argv) {
 
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(50, 50);
  main_window = glutCreateWindow("HW3");
 
  glutReshapeFunc(reshape);
  glutDisplayFunc(display); 
  init();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  currentLighting = &whiteLighting;
  glLightfv(GL_LIGHT0, GL_AMBIENT, currentLighting->ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, currentLighting->diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, currentLighting->specular);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  GLUI *control_panel = GLUI_Master.create_glui( "Controls", 0, 550, 50);

  new GLUI_Button(control_panel, "Quit", 0, (GLUI_Update_CB)exit);

  new GLUI_Column(control_panel, true);
 
  GLUI_Rollout *light_rollout = new GLUI_Rollout(control_panel, "Light Properties", false );
  GLUI_Rollout *light_position_rollout = new GLUI_Rollout(light_rollout, "Light Position", false );
  GLUI_Spinner *x_light=new GLUI_Spinner(light_position_rollout, "X", GLUI_SPINNER_FLOAT, &(lightPos[0]), 1, light_callback);
  x_light->set_float_limits(-750.0, 750.0, GLUI_LIMIT_CLAMP);
  GLUI_Spinner *y_light=new GLUI_Spinner(light_position_rollout, "Y", GLUI_SPINNER_FLOAT, &(lightPos[1]), 2, light_callback);
  y_light->set_float_limits(-750.0, 750.0, GLUI_LIMIT_CLAMP);
  GLUI_Spinner *z_light=new GLUI_Spinner(light_position_rollout, "Z", GLUI_SPINNER_FLOAT, &(lightPos[2]), 3, light_callback);
  z_light->set_float_limits(-750.0, 750.0, GLUI_LIMIT_CLAMP);

  GLUI_Rollout *eye_position_rollout = new GLUI_Rollout(control_panel, "Eye Position", false );
  GLUI_Spinner *x_spinner = new GLUI_Spinner(eye_position_rollout, "X", GLUI_SPINNER_FLOAT, &X, 0, V_callback);
  x_spinner->set_float_limits(-750.0, 750.0, GLUI_LIMIT_CLAMP);
  GLUI_Spinner *y_spinner = new GLUI_Spinner(eye_position_rollout, "Y", GLUI_SPINNER_FLOAT, &Y, 0, V_callback);
  y_spinner->set_float_limits(-750.0, 750.0, GLUI_LIMIT_CLAMP);
  GLUI_Spinner *z_spinner = new GLUI_Spinner(eye_position_rollout, "Z", GLUI_SPINNER_FLOAT, &Z, 0, V_callback);
  z_spinner->set_float_limits(-750.0, 750.0, GLUI_LIMIT_CLAMP);
 
  GLUI_Rollout *rotate_rollout = new GLUI_Rollout(control_panel, "Rotate", false);

  GLUI_Rollout *box_rollout1 = new GLUI_Rollout(rotate_rollout, "Direction", false );
  GLUI_RadioGroup *group1 = new GLUI_RadioGroup(box_rollout1, &direction, 3, direction_callback);
  new GLUI_RadioButton(group1, "Clockwise" );
  new GLUI_RadioButton(group1, "Counterclockwise" );

  GLUI_Rollout *box_rollout2 = new GLUI_Rollout(rotate_rollout, "Speed", false );
  GLUI_RadioGroup *group2 = new GLUI_RadioGroup(box_rollout2, &speed, 3, speed_callback);
  new GLUI_RadioButton(group2, "off" );
  new GLUI_RadioButton(group2, "1" );
  new GLUI_RadioButton(group2, "2" );
  new GLUI_RadioButton(group2, "3" );
  new GLUI_RadioButton(group2, "4" );
 
  control_panel->set_main_gfx_window(main_window);
  GLUI_Master.set_glutIdleFunc(spinDisplay);
  glutMainLoop();
  return 0;
}
