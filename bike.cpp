
#include <GL/glut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SOIL/SOIL.h>

// all data needed for bike
#define PI 3.14159
#define WIN_WIDTH 1000
#define WIN_HEIGHT 800
#define CYCLE_LENGTH 3.3f
#define ROD_RADIUS 0.05f
#define NUM_SPOKES 20
#define SPOKE_ANGLE 18
#define RADIUS_WHEEL 1.0f
#define TUBE_WIDTH 0.08f
#define RIGHT_ROD 1.6f
#define RIGHT_ANGLE 48.0f
#define MIDDLE_ROD 1.7f
#define MIDDLE_ANGLE 106.0f
#define BACK_CONNECTOR 0.5f
#define LEFT_ANGLE 50.0f
#define WHEEL_OFFSET 0.11f
#define WHEEL_LEN 1.1f
#define TOP_LEN 1.5f
#define CRANK_ROD 0.7f
#define CRANK_RODS 1.12f
#define CRANK_ANGLE 8.0f
#define HANDLE_ROD 1.2f
#define FRONT_INCLINE 70.0f
#define HANDLE_LIMIT 70.0f

#define INC_STEERING 2.0f
#define INC_SPEED 0.05f

// -- Global Variables -- //
GLfloat pedalAngle, speed, steering;

// Camera/view
GLfloat camx, camy, camz;
GLfloat anglex, angley, anglez;

// mouse
int prevx, prevy;
GLenum Mouse;

// positioning
GLfloat xpos, zpos, direction;

// clouds
GLfloat position = 0.0f;
GLfloat position3 = 0.0f;
GLfloat speed3 = 0.015f; // car2

// grass texture
GLuint texture;

void ZCylinder(GLfloat radius, GLfloat length);
void XCylinder(GLfloat radius, GLfloat length);

// declaring tire, seat, init, and reset functions
void Tire(void);
void drawSeat(void);
void init(void);
void reset(void);

// float variables
GLfloat Abs(GLfloat);
GLfloat degrees(GLfloat);
GLfloat radians(GLfloat);
GLfloat angleSum(GLfloat, GLfloat);

// z-axis cylinder
void ZCylinder(GLfloat radius, GLfloat length)
{
   GLUquadricObj *cylinder;
   cylinder = gluNewQuadric();
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, 0.0f);
   gluCylinder(cylinder, radius, radius, length, 15, 5);
   glPopMatrix();
}

// x-axis cylinder
void XCylinder(GLfloat radius, GLfloat length)
{
   glPushMatrix();
   glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
   ZCylinder(radius, length);
   glPopMatrix();
}

// update function
void updateScene()
{
   GLfloat xDelta, zDelta;
   GLfloat rotation;
   GLfloat sin_steering, cos_steering;

   // if the bike is not moving then do nothing
   if (-INC_SPEED < speed && speed < INC_SPEED)
      return;

   if (speed < 0.0f)
      pedalAngle = speed = 0.0f;

   // if not calculate new position and and steering
   xDelta = speed * cos(radians(direction + steering));
   zDelta = speed * sin(radians(direction + steering));
   xpos += xDelta;
   zpos -= zDelta;
   pedalAngle = degrees(angleSum(radians(pedalAngle), speed / RADIUS_WHEEL));

   // we'll be using sin(steering) and cos(steering) more than once
   // so calculate the values one time for efficiency
   sin_steering = sin(radians(steering));
   cos_steering = cos(radians(steering));

   // see the assignment 3 "Hint"
   rotation = atan2(speed * sin_steering, CYCLE_LENGTH + speed * cos_steering);
   direction = degrees(angleSum(radians(direction), rotation));
}

// float for sum of two angles
GLfloat angleSum(GLfloat a, GLfloat b)
{
   a += b;
   if (a < 0)
      return a + 2 * PI;
   else if (a > 2 * PI)
      return a - 2 * PI;
   else
      return a;
}

// draw bike frame
void BikeFrame()
{
   glColor3f(0.0f, 0.0f, 1.0f);
   glPushMatrix();
   glPushMatrix();

   glColor3f(0.0f, 0.0f, 1.0f);
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, 0.06f);
   glRotatef(-2 * pedalAngle, 0.0f, 0.0f, 1.0f);

   glPopMatrix();
   // color of frame
   glColor3f(0.0f, 0.0f, 1.0f);
   glTranslatef(0.0f, 0.0f, -0.2f);
   ZCylinder(0.08f, 0.32f);
   glPopMatrix();
   // right cylinder of frame
   glRotatef(RIGHT_ANGLE, 0.0f, 0.0f, 1.0f);
   XCylinder(ROD_RADIUS, RIGHT_ROD);

   // center cylinder of frame
   glRotatef(MIDDLE_ANGLE - RIGHT_ANGLE, 0.0f, 0.0f, 1.0f);
   XCylinder(ROD_RADIUS, MIDDLE_ROD);

   // draw seat
   glColor3f(1.0f, 0.5f, 0.0f);
   glTranslatef(MIDDLE_ROD, 0.0f, 0.0f);
   glRotatef(-MIDDLE_ANGLE, 0.0f, 0.0f, 1.0f);
   glScalef(0.3f, ROD_RADIUS, 0.25f);
   drawSeat();
   glColor3f(0.0f, 0.0f, 1.0f);
   glPopMatrix();

   // draw another cylinder for frame and back wheel
   glPushMatrix();
   glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
   XCylinder(ROD_RADIUS, BACK_CONNECTOR);

   // draw cylinders connecting to wheels
   glPushMatrix();
   glTranslatef(0.5f, 0.0f, WHEEL_OFFSET);
   XCylinder(ROD_RADIUS, RADIUS_WHEEL + TUBE_WIDTH);
   glPopMatrix();
   glPushMatrix();
   glTranslatef(0.5f, 0.0f, -WHEEL_OFFSET);
   XCylinder(ROD_RADIUS, RADIUS_WHEEL + TUBE_WIDTH);
   glPopMatrix();
   glPopMatrix();

   glPushMatrix();
   glTranslatef(-(BACK_CONNECTOR + RADIUS_WHEEL + TUBE_WIDTH), 0.0f, 0.0f);
   // translate cylinders to back wheel

   glPushMatrix();
   glRotatef(-2 * pedalAngle, 0.0f, 0.0f, 1.0f);
   Tire();
   glColor3f(0.0f, 0.0f, 1.0f);
   glPopMatrix();
   glRotatef(LEFT_ANGLE, 0.0f, 0.0f, 1.0f);

   // draw more cylinders connecting back wheel
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, -WHEEL_OFFSET);
   XCylinder(ROD_RADIUS, WHEEL_LEN);
   glPopMatrix();
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, WHEEL_OFFSET);
   XCylinder(ROD_RADIUS, WHEEL_LEN);
   glPopMatrix();

   // draw another cylinder for back of frame
   glTranslatef(WHEEL_LEN, 0.0f, 0.0f);
   XCylinder(ROD_RADIUS, CRANK_ROD);

   // draw top cylinder for upper frame
   glTranslatef(CRANK_ROD, 0.0f, 0.0f);
   glRotatef(-LEFT_ANGLE, 0.0f, 0.0f, 1.0f);
   XCylinder(ROD_RADIUS, TOP_LEN);

   // draw handle
   glTranslatef(TOP_LEN, 0.0f, 0.0f);
   glRotatef(-FRONT_INCLINE, 0.0f, 0.0f, 1.0f);
   glPushMatrix();
   glTranslatef(-0.1f, 0.0f, 0.0f);
   XCylinder(ROD_RADIUS, 0.45f);
   glPopMatrix();

   // steering for handle
   glPushMatrix();
   glRotatef(-steering, 1.0f, 0.0f, 0.0f);
   glTranslatef(-0.3f, 0.0f, 0.0f);

   glPushMatrix();
   glRotatef(FRONT_INCLINE, 0.0f, 0.0f, 1.0f);

   glPushMatrix();
   glTranslatef(0.0f, 0.0f, -HANDLE_ROD / 2);
   ZCylinder(ROD_RADIUS, HANDLE_ROD);
   glPopMatrix();

   glPushMatrix();
   glColor3f(0.0f, 0.0f, 1.0f);
   glTranslatef(0.0f, 0.0f, -HANDLE_ROD / 2);
   ZCylinder(0.07f, HANDLE_ROD / 4);
   glTranslatef(0.0f, 0.0f, HANDLE_ROD * 3 / 4);
   ZCylinder(0.07f, HANDLE_ROD / 4);
   glColor3f(0.0f, 0.0f, 1.0f);
   glPopMatrix();
   glPopMatrix();

   glPushMatrix();

   // draw main big cylinder
   XCylinder(ROD_RADIUS, CRANK_ROD);
   glTranslatef(CRANK_ROD, 0.0f, 0.0f);
   glRotatef(CRANK_ANGLE, 0.0f, 0.0f, 1.0f);

   // draw two rods connecting to front wheel
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, WHEEL_OFFSET);
   XCylinder(ROD_RADIUS, CRANK_RODS);
   glPopMatrix();
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, -WHEEL_OFFSET);
   XCylinder(ROD_RADIUS, CRANK_RODS);
   glPopMatrix();

   // draw wheel
   glTranslatef(CRANK_RODS, 0.0f, 0.0f);
   glRotatef(-2 * pedalAngle, 0.0f, 0.0f, 1.0f);
   Tire();
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();
}

void drawSeat()
{

   // draw top of seat
   glBegin(GL_POLYGON);
   glVertex3f(-0.1f, 1.0f, -0.5f);
   glVertex3f(1.0f, 1.0f, -0.3f);
   glVertex3f(1.0f, 1.0f, 0.3f);
   glVertex3f(-0.1f, 1.0f, 0.5f);
   glVertex3f(-0.5f, 1.0f, 1.0f);
   glVertex3f(-1.0f, 1.0f, 1.0f);
   glVertex3f(-1.0f, 1.0f, -1.0f);
   glVertex3f(-0.5f, 1.0f, -1.0f);
   glEnd();

   // draw bottom of seat
   glBegin(GL_POLYGON);
   glVertex3f(-0.1f, -1.0f, -0.5f);
   glVertex3f(1.0f, -1.0f, -0.3f);
   glVertex3f(1.0f, -1.0f, 0.3f);
   glVertex3f(-0.1f, -1.0f, 0.5f);
   glVertex3f(-0.5f, -1.0f, 1.0f);
   glVertex3f(-1.0f, -1.0f, 1.0f);
   glVertex3f(-1.0f, -1.0f, -1.0f);
   glVertex3f(-0.5f, -1.0f, -1.0f);
   glEnd();

   // draw sides of seat
   glBegin(GL_QUADS);
   glVertex3f(1.0f, 1.0f, -0.3f);
   glVertex3f(1.0f, 1.0f, 0.3f);
   glVertex3f(1.0f, -1.0f, 0.3f);
   glVertex3f(1.0f, -1.0f, -0.3f);

   glVertex3f(1.0f, 1.0f, 0.3f);
   glVertex3f(-0.1f, 1.0f, 0.5f);
   glVertex3f(-0.1f, -1.0f, 0.5f);
   glVertex3f(1.0f, -1.0f, 0.3f);

   glVertex3f(1.0f, 1.0f, -0.3f);
   glVertex3f(-0.1f, 1.0f, -0.5f);
   glVertex3f(-0.1f, -1.0f, -0.5f);
   glVertex3f(1.0f, -1.0f, -0.3f);

   glVertex3f(-0.1f, 1.0f, 0.5f);
   glVertex3f(-0.5f, 1.0f, 1.0f);
   glVertex3f(-0.5f, -1.0f, 1.0f);
   glVertex3f(-0.1f, -1.0f, 0.5f);

   glVertex3f(-0.1f, 1.0f, -0.5f);
   glVertex3f(-0.5f, 1.0f, -1.0f);
   glVertex3f(-0.5f, -1.0f, -1.0f);
   glVertex3f(-0.1f, -1.0f, -0.5f);

   glVertex3f(-0.5f, 1.0f, 1.0f);
   glVertex3f(-1.0f, 1.0f, 1.0f);
   glVertex3f(-1.0f, -1.0f, 1.0f);
   glVertex3f(-0.5f, -1.0f, 1.0f);

   glVertex3f(-0.5f, 1.0f, -1.0f);
   glVertex3f(-1.0f, 1.0f, -1.0f);
   glVertex3f(-1.0f, -1.0f, -1.0f);
   glVertex3f(-0.5f, -1.0f, -1.0f);

   glVertex3f(-1.0f, 1.0f, 1.0f);
   glVertex3f(-1.0f, 1.0f, -1.0f);
   glVertex3f(-1.0f, -1.0f, -1.0f);
   glVertex3f(-1.0f, -1.0f, 1.0f);

   glEnd();
}

// function to draw tire
void Tire(void)
{
   int i;
   // draw The Rim
   glColor3f(0.0f, 0.0f, 0.0f);
   glutSolidTorus(0.06f, 0.92f, 4, 30);

   glColor3f(0.0f, 0.0f, 0.0f);
   glPushMatrix();
   glTranslatef(0.0f, 0.0f, -0.06f);
   ZCylinder(0.02f, 0.12f);
   glPopMatrix();
   glutSolidTorus(0.02f, 0.02f, 3, 20);

   //   Draw The Spokes
   glColor3f(0.0f, 0.0f, 1.0f);
   for (i = 0; i < NUM_SPOKES; ++i)
   {
      glPushMatrix();
      glRotatef(i * SPOKE_ANGLE, 0.0f, 0.0f, 1.0f);
      glBegin(GL_LINES);
      glVertex3f(0.0f, 0.02f, 0.0f);
      glVertex3f(0.0f, 0.86f, 0.0f);
      glEnd();
      glPopMatrix();
   }

   //   Draw The Tyre
   glColor3f(0.0f, 0.0f, 0.0f);
   glutSolidTorus(TUBE_WIDTH, RADIUS_WHEEL, 10, 30);
   glColor3f(0.0f, 0.0f, 0.0f);
}

void grass(void)
{
   // color of grass
   glColor3f(0.0f, 1.0f, 0.0f);
   glPushMatrix();
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glBegin(GL_QUADS);

   glTexCoord2f(0, 0);
   glVertex3f(50.0f, -RADIUS_WHEEL, 50.0f);
   glTexCoord2f(1, 0);
   glVertex3f(-50.0f, -RADIUS_WHEEL, 50.0f);
   glTexCoord2f(1, 1);
   glVertex3f(-50, -RADIUS_WHEEL, -50.0f);
   glTexCoord2f(0, 1);
   glVertex3f(50, -RADIUS_WHEEL, -50.0f);
   // }
   glEnd();

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

void drawTree()
{
   GLUquadricObj *quadratic;
   quadratic = gluNewQuadric();

   // begin Push
   glPushMatrix();

   // tree top 1
   glPushMatrix();
   glColor3f(0.0, 0.7, 0.0); // tree is green
   glTranslatef(-5.0f, 1.0f, 0.0f);

   glPushMatrix();
   glRotatef(270.0, 1.0, 0.0, 0.0); // rotate to point along -y

   glPushMatrix();
   glRotatef(200.0, 0.0, 0.0, 1.0);
   glutSolidCone(1.0, 3.0, 10, 0); // 4 = 2
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

   // tree top 2
   glPushMatrix();
   glColor3f(0.0, 0.7, 0.0); // tree is green
   glTranslatef(-5.0f, 2.0f, 0.0f);

   glPushMatrix();
   glRotatef(270.0, 1.0, 0.0, 0.0); // rotate to point along -y

   glPushMatrix();
   glRotatef(200.0, 0.0, 0.0, 1.0);
   glutSolidCone(1.0, 3.0, 10, 0); // 4 = 2
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

   // tree stump
   glPushMatrix();
   glColor3f(1.0, 0.5, 0.0); // brown
   glTranslatef(-5.0f, -1.0f, 0.0f);

   glPushMatrix();
   glRotatef(270.0, 1.0, 0.0, 0.0); // rotate to point along -y

   glPushMatrix();
   glRotatef(130.0, 0.0, 0.0, 1.0); // rotate to point along -y
   gluCylinder(quadratic, 0.1f, 0.1f, 2.0f, 20, 1);
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

   // end Pop
   glPopMatrix();
}

void drawClouds()
{
   // begin Push
   glPushMatrix();

   // cloud 1
   glPushMatrix();
   glColor3f(1.0, 1.0, 1.0); // white
   glTranslatef(-5.0f, 30.0f, 0.0f);

   glPushMatrix();
   glRotatef(270.0, 1.0, 0.0, 0.0); // rotate to point along -y

   glPushMatrix();
   glRotatef(130.0, 0.0, 0.0, 1.0); // rotate to point along -y
   glutSolidSphere(3.0f, 20, 20);
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

   // cloud 2
   glPushMatrix();
   glColor3f(1.0, 1.0, 1.0); // white
   glTranslatef(-2.0f, 30.0f, 0.0f);

   glPushMatrix();
   glRotatef(270.0, 1.0, 0.0, 0.0); // rotate to point along -y

   glPushMatrix();
   glRotatef(130.0, 0.0, 0.0, 1.0); // rotate to point along -y
   glutSolidSphere(3.0f, 20, 20);
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

   // cloud 3
   glPushMatrix();
   glColor3f(1.0, 1.0, 1.0); // white
   glTranslatef(-3.5f, 35.0f, 0.0f);

   glPushMatrix();
   glRotatef(270.0, 1.0, 0.0, 0.0); // rotate to point along -y

   glPushMatrix();
   glRotatef(130.0, 0.0, 0.0, 1.0); // rotate to point along -y
   glutSolidSphere(3.0f, 20, 20);
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

   // end Pop
   glPopMatrix();
}

void drawSun()
{
   // begin Push
   glPushMatrix();

   // sun
   glPushMatrix();
   glColor3f(1.0, 1.0, 0.0); // white
   glTranslatef(30.5f, -2.0f, -100.0f);

   glPushMatrix();
   glRotatef(270.0, 1.0, 0.0, 0.0); // rotate to point along -y

   glPushMatrix();
   glRotatef(170.0, 0.0, 0.0, 1.0); // rotate to point along -y
   glutSolidSphere(20.0f, 20, 20);
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

   // end Pop
   glPopMatrix();
}

void init()
{
   GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat mat_shininess[] = {100.0};
   GLfloat light_directional[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat light_positional[] = {1.0, 1.0, 1.0, 0.0};
   GLfloat light_diffuse[] = {1.0, 1.0, 1.0};

   reset();

   glShadeModel(GL_SMOOTH);

   glLightfv(GL_LIGHT0, GL_POSITION, light_directional);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_diffuse);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glColorMaterial(GL_FRONT, GL_DIFFUSE);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_COLOR_MATERIAL);
   glEnable(GL_DEPTH_TEST);
}

void display(void)
{
   int i, j;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClearColor(0.0, 0.7, 1.0, 1.0); // sky color is light blue
   glEnable(GL_NORMALIZE);

   glPushMatrix();

   // rotation for camera
   glRotatef(angley, 1.0f, 0.0f, 0.0f);
   glRotatef(anglex, 0.0f, 1.0f, 0.0f);
   glRotatef(anglez, 0.0f, 0.0f, 1.0f);

   // draw sun
   drawSun();

   // draw grass plane
   grass();

   // Draw clouds
   glPushMatrix();
   for (i = -5; i < 5; i++)
      for (j = -10; j < 10; j++)
      {
         glPushMatrix();
         glTranslatef(i * 30.5, 0.0, j * 30.5); // 7.5
         drawClouds();
         glPopMatrix();
      }
   glPopMatrix();

   // Draw Trees
   glPushMatrix();
   for (i = -10; i < 10; i++)
      for (j = -10; j < 10; j++)
      {
         glPushMatrix();
         glTranslatef(i * 14.5, 0.0, j * 14.5); // 7.5
         drawTree();
         glPopMatrix();
      }
   glPopMatrix();

   // set bike location
   glPushMatrix();
   glTranslatef(xpos, 0.0f, -5.0f);
   glRotatef(direction, 0.0f, 1.0f, 0.0f);

   // draw bike
   BikeFrame();
   glPopMatrix();

   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(camx, camy, camz, camx, 0.0, 0.0, 0.0, 1.0, 0.0);

   glutSwapBuffers();
}

// absolute value of a float number
GLfloat Abs(GLfloat a)
{
   if (a < 0.0f)
      return -a;
   else
      return a;
}

// value of given angle in degrees
GLfloat degrees(GLfloat a)
{
   return a * 180.0f / PI;
}

// value of given angle in radians
GLfloat radians(GLfloat a)
{
   return a * PI / 180.0f;
}

// idle function
void idle(void)
{
   updateScene();
   glutPostRedisplay();
}

// arrow key function
void special(int key, int x, int y)
{
   switch (key)
   {
   case GLUT_KEY_UP:
      camz -= 0.1f;
      break;
   case GLUT_KEY_DOWN:
      camz += 0.1f;
      break;
   case GLUT_KEY_LEFT:
      camx -= 0.1f;
      break;
   case GLUT_KEY_RIGHT:
      camx += 0.1f;
      break;
   }
   glutPostRedisplay();
}

// reset scene function
void reset()
{
   anglex = angley = anglez = 0.0f;
   pedalAngle = steering = 0.0f;
   Mouse = GLUT_UP;
   pedalAngle = speed = steering = 0.0f;
   camx = camy = 0.0f;
   camz = 5.0f;
   xpos = zpos = 0.0f;
   direction = 0.0f;
}

// keyboard function for reseting scene, changing steering angle, moving and stopping bike
void keyboard(unsigned char key, int x, int y)
{
   GLfloat r = 0.0f;

   switch (key)
   {
   case 's':
   case 'S':
      reset();
      break;
   case 'l':
      if (steering < HANDLE_LIMIT)
         steering += INC_STEERING;
      break;
   case 'r':
      if (steering > -HANDLE_LIMIT)
         steering -= INC_STEERING;
      break;
   case '+':
      speed += INC_SPEED;
      break;
   case '-':
      speed -= INC_SPEED;
      break;
   case 27:
      exit(1);
   }

   // error conditions
   pedalAngle += speed;
   if (speed < 0.0f)
      speed = 0.0f;
   if (pedalAngle < 0.0f)
      pedalAngle = 0.0f;
   if (pedalAngle >= 360.0f)
      pedalAngle -= 360.0f;

   glutPostRedisplay();
}

// move scene with mouse function
void mouse(int button, int state, int x, int y)
{
   switch (button)
   {
   case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN)
      {
         Mouse = GLUT_DOWN;
         prevx = x;
         prevy = y;
      }
      if (state == GLUT_UP)
      {
         Mouse = GLUT_UP;
      }
      break;
   case GLUT_RIGHT_BUTTON:
      break;
   }
   glutPostRedisplay();
}

void passive(int x, int y)
{
   // do nothing
}

// mouse motion
void motion(int x, int y)
{
   if (Mouse == GLUT_DOWN)
   {
      int deltax, deltay;
      deltax = prevx - x;
      deltay = prevy - y;
      anglex += 0.5 * deltax;
      angley += 0.5 * deltay;
      if (deltax != 0 && deltay != 0)
         anglez += 0.5 * sqrt(deltax * deltax + deltay * deltay);

      if (anglex < 0)
         anglex += 360.0;
      if (angley < 0)
         angley += 360.0;
      if (anglez < 0)
         anglez += 360.0;

      if (anglex > 360.0)
         anglex -= 360.0;
      if (angley > 360.0)
         angley -= 360.0;
      if (anglez > 360.0)
         anglez -= 360.0;
   }
   else
   {
      Mouse = GLUT_UP;
   }
   prevx = x;
   prevy = y;
   glutPostRedisplay();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei)w, (GLsizei)h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(camx, camy, camz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void callFunctions(void)
{
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   glutSpecialFunc(special);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(passive);
   glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}

void instructions(void)
{
   printf("Use keys below:\n");
   printf("'+' to increase the speed\n");
   printf("'-' to decrease the speed\n");
   printf("'l' to rotate the handle left\n");
   printf("'r' to rotate the handle right\n");
   printf("'s' or 'S' to reset the scene\n");
   printf("Arrow keys to move the camera\n");
   printf("Mouse to move the scene\n");
}

int main(int argc, char *argv[])
{
   instructions();
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowPosition(100, 100);
   glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
   glutCreateWindow("BiCycle");

   texture = SOIL_load_OGL_texture("grass1.jpg", SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB);

   init();
   callFunctions();
   glutMainLoop();
}
