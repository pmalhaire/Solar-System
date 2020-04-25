// Copyright 2020 me
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include "./tga.h"

static int day = 0, month = 0, year = 0;

struct Star {
  float x, y;
  unsigned char r, g, b, a;
};
std::vector<Star> stars;

TGA *sun;
TGA *earth;
TGA *moon;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

void init(void) {
  // During init, enable debug output
  glewInit();
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  sun = new TGA("images/sun.tga");
  moon = new TGA("images/moon.tga");
  earth = new TGA("images/earth.tga");

  // Lighting set up
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  // Set lighting intensity and color
  GLfloat qaAmbientLight[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat qaDiffuseLight[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat qaSpecularLight[] = {1.0, 1.0, 1.0, 1.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

  // Set the light position
  GLfloat qaLightPosition[] = {0.0, 0.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
}

void create_sphere(GLint slice, GLint stacks, GLdouble radius) {
  GLUquadricObj *quadric = gluNewQuadric();
  gluQuadricTexture(quadric, true);
  gluQuadricNormals(quadric, GLU_SMOOTH);
  gluSphere(quadric, radius, slice, stacks);
}

void draw_sun(void) {
  // define the precision of sphere
  GLint slice, stacks = (60, 60);
  GLdouble radius = 4;

  glBindTexture(GL_TEXTURE_2D, sun->getTextureHandle());

  create_sphere(slice, stacks, radius);
}

void draw_earth(void) {
  // define the precision of sphere
  GLint slice, stacks = (20, 20);
  GLdouble radius = 0.7;
  // Draw Earth

  glBindTexture(GL_TEXTURE_2D, earth->getTextureHandle());

  create_sphere(slice, stacks, radius);
}

void draw_moon(void) {
  // define the precision of sphere
  GLint slice, stacks = (10, 10);
  GLdouble radius = 0.2;

  glBindTexture(GL_TEXTURE_2D, moon->getTextureHandle());

  create_sphere(slice, stacks, radius);
}

void draw_stars(void) {
  glColor3ub(255, 255, 255);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(2, GL_FLOAT, sizeof(Star), &stars[0].x);
  glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Star), &stars[0].r);
  glPointSize(3.0);
  glDrawArrays(GL_POINTS, 0, stars.size());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

void display(void) {
  glPushMatrix();
  // Set material properties
  GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0};

  glMaterialfv(GL_FRONT, GL_AMBIENT, qaWhite);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, qaWhite);
  glMaterialfv(GL_FRONT, GL_SPECULAR, qaWhite);
  glMaterialf(GL_FRONT, GL_SHININESS, 60.0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_stars();
  glEnable(GL_TEXTURE_2D);

  glRotatef((GLfloat)day, 0.0, 1.0, 0.0);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  draw_sun();
  glRotatef(-90.0, 1.0, 0.0, 0.0);
  glRotatef(-(GLfloat)day, 0.0, 1.0, 0.0);

  glRotatef((GLfloat)day, 0.0, 1.0, 0.0);
  glTranslatef(10.0, 0.0, 0.0);
  draw_earth();

  glRotatef((GLfloat)((day % 30) * 12.0), 0.0, 1.0, 0.0);
  glTranslatef(1.0, 0.0, 0.0);
  draw_moon();

  glPopMatrix();
  glFlush();
  glutSwapBuffers();
}

// adapt geometry to window
void reshape(int w, int h) {
  glLoadIdentity();
  glViewport(0, 0, w, h);
  gluPerspective(50.0, w / (GLfloat)h, 3.0, 90.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(2.0, 4.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void idleFunc() {
  day = day % 360 + 1;
  month = day / 12;
  if (day == 360)
    ++year;

  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(1200, 800);
  glutInitWindowPosition(300, 150);
  glutCreateWindow("Solar system");

  // Stars
  for (size_t i = 0; i < 1000; ++i) {
    Star star;
    star.x = -50 + (rand() % 100);
    star.y = -50 + (rand() % 100);
    star.r = 255;
    star.g = 255;
    star.b = 255;
    star.a = 255;
    stars.push_back(star);
  }

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idleFunc);
  glEnable(GL_DEPTH_TEST);
  glutMainLoop();
  return 0;
}
