////////////////////////////////////////////////////////////////
// Code Written By David White.
//
// Skeleton Code (consisting solely of code for the setup of a basic OpenGL window and the
// myRandom() and makeAxes() functions) was written by Arturs Bekasovs and Toby Howard.
//
// This code is licensed under the terms of the Creative Commons 
// Attribution 2.0 Generic (CC BY 3.0) License.
//
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#define X_VELOCITY_REFERENCE 5
#define Y_VELOCITY_REFERENCE 15
#define Z_VELOCITY_REFERENCE 5

#define DEFAULT_BIRTHRATE 5
#define DEFAULT_VELOCITY 10
#define DEFAULT_LIFETIME 100
#define DEFAULT_SIZE 30

#define DEFAULT_EMITTER_X 0
#define DEFAULT_EMITTER_Y 0
#define DEFAULT_EMITTER_Z 0

#define DEFAULT_X_LENGTH 100
#define DEFAULT_Y_LENGTH 50
#define DEFAULT_Z_LENGTH 50

#define DEFAULT_SPREAD 2

#define DEFAULT_GRAVITY 3.5

#define planeWidth 4000
#define planeLength 3000
#define planeHeight -200

#define fireworksPlaneWidth 750
#define fireworksPlaneLength 750
#define fireworksPlaneHeight -200

#define trailLifetime 10

#define trailSizeScale 0.75

#define explosionParticleAmount 50
#define explosionSpeed 5
#define explosionLifetime 50

#define RED 1
#define GREEN 2
#define BLUE 3

#define POINTS 1
#define STARS 2
#define QUADS 3
#define MaxMode 3

#define DEFAULT_TRANSPARENCY 0.5

#define DEFAULT_POINT_OF_DEATH 0.9

int particleMode = STARS;

int explosionType = STARS;

int fireworksMode = 0;

int completelyRandomColours = 0;

float planeDampening = 0.5;

int planeOn = 1;

int trailsOn = 0;
int explosionsOn = 0;

// Display list for coordinate axis 
GLuint axisList;

int AXIS_SIZE= 200;
int axisEnabled= 0;

float emitter_xcord = 0;
float emitter_ycord = 0;
float emitter_zcord = 0;

float emitter_box_x_length = 100;
float emitter_box_y_length = 50;
float emitter_box_z_length = 50;

float init_xvel = X_VELOCITY_REFERENCE;
float init_yvel = Y_VELOCITY_REFERENCE;
float init_zvel = Z_VELOCITY_REFERENCE;

float velocity_level = DEFAULT_VELOCITY;

float red_level = 1.0;
float green_level = 0.6;
float blue_level = 0.0;

float end_red_level = 1.0;
float end_green_level = 0.0;
float end_blue_level = 0.3;

float particleTransparency;

float point_of_death = 0.9; // I.e. start to fade 90% of way through life

float randomness = 0.1;

// Number of particles per tick
float particleBirthrate = DEFAULT_BIRTHRATE;

int particleLifetime = DEFAULT_LIFETIME;

int particleCount = 0;

float particleSize = DEFAULT_SIZE;

int particleSpread = DEFAULT_SPREAD;

// Definition of a particle
typedef struct particle
{
  float x_cord;
  float y_cord;
  float z_cord;
  float x_vel;
  float y_vel;
  float z_vel;
  float size;
  int age;
  int lifetime;

  int particleType;
  int isExplode;
  int isTrail;

  float red;
  float green;
  float blue;

  float end_red;
  float end_green;
  float end_blue;

  struct particle* previousParticle;
  struct particle* nextParticle;
}Particle;

Particle* particlesHead = NULL;
Particle* particlesTail = NULL;

///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////

// This function will print an estimation of the current particle count
void printParticleCount()
{
  printf("Average Particle Count: %.3f", particleBirthrate*particleLifetime);

  if (explosionsOn)
    printf(" +%d for every explosion", explosionParticleAmount);

  if (trailsOn)
    printf(" and +%d for each trail", trailLifetime);  
  
  printf(".\n");

  if(explosionsOn && !trailsOn)
  {
    printf(" - Therefore Max Count is: %.3f\n",
            particleBirthrate*particleLifetime 
            + particleBirthrate*particleLifetime*explosionParticleAmount);
  }
  else if (!explosionsOn && trailsOn)
  {
    printf(" - Therefore Actual Count is: %.3f\n",
            particleBirthrate*particleLifetime 
            + particleBirthrate*particleLifetime*trailLifetime);
  }
  else if (explosionsOn && trailsOn)
  {
    printf(" - Therefore Max Count is: %.3f\n",
            particleBirthrate*particleLifetime 
            + particleBirthrate*particleLifetime*explosionParticleAmount
            + particleBirthrate*particleLifetime*trailLifetime);
  }
}

// This function will increase or decrease the birthrate of the particles,
// depending on whether the function is called with a 1 or -1
void changeBirthrate(int direction)
{
  switch(direction)
  {
    case 1:
    if(particleBirthrate >= 1)
      particleBirthrate = particleBirthrate + 1;
    else
      particleBirthrate = particleBirthrate * 2;
    break;
    case -1:
    if(particleBirthrate > 1)
      particleBirthrate = particleBirthrate - 1;
    else
      particleBirthrate = particleBirthrate * 0.5;
    break;
  }

  printParticleCount();
}

// This function takes a positive or negative integer and changes the particle size
// by the specified amount - with a floor at size=2
void changeSize(int amount)
{
  particleSize = particleSize + amount;

  if(particleSize < 2)
    particleSize = 2;
}

// This function will increment/decrement the initial velocity of particles by the specified amount
// with a floor at 0 (i.e. velocity cannot become negative).
void changeVelocity(float amount)
{
  velocity_level = velocity_level + amount;
  if (velocity_level < 0)
    velocity_level = 0;
  init_xvel = X_VELOCITY_REFERENCE * velocity_level/10;
  init_yvel = Y_VELOCITY_REFERENCE * velocity_level/10;
  init_zvel = Z_VELOCITY_REFERENCE * velocity_level/10;
}

// This function will increase or decrease the lifetime of new particles by the specified amount
// with a floor at 0.
void changeLifetime(int amount)
{
  particleLifetime = particleLifetime + amount;

  if(particleLifetime < 0)
  {
    particleLifetime = 0;
  }

  printParticleCount();
}

// This function will change the particle birthrate to get an average particle count of the value
// specified.
void setParticleCount(int amount)
{
  particleBirthrate = (float)amount / (float)particleLifetime;

  printParticleCount();
}

// This function will cycle through all the different styles of particles - wrapping around
// when needed
void toggleParticleStyle()
{
  particleMode = particleMode + 1;

  if (particleMode > MaxMode)
    particleMode = 1;
}

// This function increases or decreases the spread of the particles by the amount specified - with
// a floor at 0.
void changeSpread(int amount)
{
  particleSpread = particleSpread + amount;

  if (particleSpread < 0)
    particleSpread = 0;
}

void changeColour(int colour, float amount)
{
  switch(colour)
  {
    case RED:
    red_level = red_level + amount;

    if (red_level > 1)
      red_level = 1;

    green_level = green_level - 0.5*amount;

    if (green_level < 0)
      green_level = 0;

    blue_level = blue_level - 0.5*amount;

    if(blue_level < 0)
      blue_level = 0;
    break;
    case GREEN:
    green_level = green_level + amount;

    if (green_level > 1)
      green_level = 1;

    red_level = red_level - 0.5*amount;

    if (red_level < 0)
      red_level = 0;

    blue_level = blue_level - 0.5*amount;

    if(blue_level < 0)
      blue_level = 0;
    break;
    case BLUE:
    blue_level = blue_level + amount;

    if (blue_level > 1)
      blue_level = 1;

    red_level = red_level - 0.5*amount;

    if (red_level < 0)
      red_level = 0;

    green_level = green_level - 0.5*amount;

    if(green_level < 0)
      green_level = 0;
  }
}

void changeEndColour(int colour, float amount)
{
  switch(colour)
  {
    case RED:
    end_red_level = end_red_level + amount;

    if (end_red_level > 1)
      end_red_level = 1;

    end_green_level = end_green_level - 0.5*amount;

    if (end_green_level < 0)
      end_green_level = 0;

    end_blue_level = end_blue_level - 0.5*amount;

    if(end_blue_level < 0)
      end_blue_level = 0;
    break;
    case GREEN:
    end_green_level = end_green_level + amount;

    if (end_green_level > 1)
      end_green_level = 1;

    end_red_level = end_red_level - 0.5*amount;

    if (end_red_level < 0)
      end_red_level = 0;

    end_blue_level = end_blue_level - 0.5*amount;

    if(end_blue_level < 0)
      end_blue_level = 0;
    break;
    case BLUE:
    end_blue_level = end_blue_level + amount;

    if (end_blue_level > 1)
      end_blue_level = 1;

    end_red_level = end_red_level - 0.5*amount;

    if (end_red_level < 0)
      end_red_level = 0;

    end_green_level = end_green_level - 0.5*amount;

    if(end_green_level < 0)
      end_green_level = 0;
  }
}

int i = 0;

int tickNumber = 0;

float gravityIntensity = DEFAULT_GRAVITY;
float gravityAcceleration = 0.0981;

void changeGravity(float amount)
{
  gravityIntensity = gravityIntensity + amount;
}

void resetSimulation()
{
  tickNumber = 0;

  gravityIntensity = DEFAULT_GRAVITY;
  particleLifetime = DEFAULT_LIFETIME;
  particleBirthrate = DEFAULT_BIRTHRATE;
  velocity_level = DEFAULT_VELOCITY;

  particleSpread = DEFAULT_SPREAD;
  particleSize = DEFAULT_SIZE;

  init_xvel = X_VELOCITY_REFERENCE;
  init_yvel = Y_VELOCITY_REFERENCE;
  init_zvel = Z_VELOCITY_REFERENCE;

  emitter_xcord = DEFAULT_EMITTER_X;
  emitter_ycord = DEFAULT_EMITTER_Y;
  emitter_zcord = DEFAULT_EMITTER_Z;

  emitter_box_x_length = DEFAULT_X_LENGTH;
  emitter_box_y_length = DEFAULT_Y_LENGTH;
  emitter_box_z_length = DEFAULT_Z_LENGTH;

  point_of_death = DEFAULT_POINT_OF_DEATH;

  trailsOn = 0;
  explosionsOn = 0;

  // Clear all particles on screen
  Particle* currentParticle = particlesHead;

  Particle* tempNext;

  fireworksMode = 0;

  while(currentParticle != NULL)
  {
    tempNext = currentParticle -> nextParticle;

    free(currentParticle);

    currentParticle = tempNext;
  }

  particleCount = 0;

  particlesHead = NULL;
  particlesTail = NULL;
  
  printParticleCount();
}

int temp;

void toggleFireworksMode()
{
  temp = fireworksMode;
  resetSimulation();
  fireworksMode = !temp;

  // If now in fireworks mode
  if (fireworksMode)
  {
    emitter_xcord = 0;
    emitter_zcord = 50;
    emitter_ycord = fireworksPlaneHeight;

    emitter_box_x_length = fireworksPlaneWidth;
    emitter_box_z_length = fireworksPlaneLength;
    emitter_box_y_length = 0;

    particleMode = POINTS;

    trailsOn = 1;
    explosionsOn = 1;

    // I.e. start fading halfway through explosion life
    point_of_death = 0.5;

    changeVelocity(3.5);
    particleSize = 15;
    setParticleCount(50);
    changeLifetime(-20);
  }
}

void menu (int menuentry) {
  switch (menuentry) {
  case 1: changeVelocity(1);
          break;
  case 2: changeVelocity(-1);
          break;
  case 3: red_level = myRandom();
          green_level = myRandom();
          blue_level = myRandom();

          end_red_level = myRandom();
          end_green_level = myRandom();
          end_blue_level = myRandom();
          break;
  case 4: changeGravity(0.5);
          break;
  case 5: changeGravity(-0.5);
          break;
  case 6: changeLifetime(-5);
          break;
  case 7: changeLifetime(5);
          break;
  case 8: setParticleCount(100000);
          break;
  case 9: setParticleCount(1000000);
          break;
  case 10: setParticleCount(10000);
          break;
  case 11: toggleParticleStyle();
          break;
  case 12: changeSize(-2);
          break;
  case 13: changeSize(2);
          break;
  case 14: planeOn = !planeOn;
          break;
  case 15: toggleFireworksMode();
          break;
  case 16: completelyRandomColours = !completelyRandomColours;
          break;
  case 17: explosionsOn = !explosionsOn;
            printParticleCount();
          break;
  case 18: trailsOn = !trailsOn;
            printParticleCount();
          break;
  case 19: setParticleCount(1000);
          break;
  }
}

void generateParticle()
{
  Particle* newParticlePtr = (Particle*)malloc(sizeof(Particle));

  newParticlePtr -> x_cord = emitter_box_x_length * 2 * (myRandom()-0.5) + emitter_xcord;
  newParticlePtr -> y_cord = emitter_box_y_length * 2 * (myRandom()-0.5) + emitter_ycord;
  newParticlePtr -> z_cord = emitter_box_z_length * 2 * (myRandom()-0.5) + emitter_zcord;

  newParticlePtr -> x_vel = init_xvel * particleSpread * (myRandom()-0.5);
  newParticlePtr -> y_vel = init_yvel + init_yvel*(randomness * 2 * (myRandom()-0.5));
  newParticlePtr -> z_vel = init_zvel * particleSpread * (myRandom()-0.5);

  newParticlePtr -> size = myRandom()*particleSize;

  newParticlePtr -> age = 0;
  newParticlePtr -> lifetime = particleLifetime;

  newParticlePtr -> particleType = particleMode;

  newParticlePtr -> isExplode = 0;
  newParticlePtr -> isTrail = 0;

  if (!completelyRandomColours)
  {
    newParticlePtr -> red = red_level;
    newParticlePtr -> green = green_level;
    newParticlePtr -> blue = blue_level;

    newParticlePtr -> end_red = end_red_level;
    newParticlePtr -> end_green = end_green_level;
    newParticlePtr -> end_blue = end_blue_level;
  }
  else
  {
    newParticlePtr -> red = myRandom();
    newParticlePtr -> green = myRandom();
    newParticlePtr -> blue = myRandom();

    newParticlePtr -> end_red = myRandom();
    newParticlePtr -> end_green = myRandom();
    newParticlePtr -> end_blue = myRandom();
  }

  newParticlePtr -> previousParticle = particlesTail;
  newParticlePtr -> nextParticle = NULL;

  if(particlesHead == NULL)
  {
    particlesHead = newParticlePtr;
  }

  if(particlesTail != NULL)
    particlesTail -> nextParticle = newParticlePtr;

  particlesTail = newParticlePtr;

  particleCount = particleCount + 1;
}

void explodeParticle(Particle* explodingParticle)
{
  Particle* newParticlePtr;
  for (i = 0; i < explosionParticleAmount; i++)
  {
    newParticlePtr = (Particle*)malloc(sizeof(Particle));

    newParticlePtr -> x_cord = explodingParticle -> x_cord;
    newParticlePtr -> y_cord = explodingParticle -> y_cord;
    newParticlePtr -> z_cord = explodingParticle -> z_cord;

    newParticlePtr -> x_vel = explosionSpeed * 2 *(myRandom()-0.5);
    newParticlePtr -> y_vel = explosionSpeed * 2 *(myRandom()-0.5);
    newParticlePtr -> z_vel = explosionSpeed * 2 *(myRandom()-0.5);

    newParticlePtr -> size = myRandom() * explodingParticle -> size;

    newParticlePtr -> age = 0;
    newParticlePtr -> lifetime = explosionLifetime;

    newParticlePtr -> particleType = explosionType;

    newParticlePtr -> isExplode = 1;
    newParticlePtr -> isTrail = 0;

    newParticlePtr -> red = explodingParticle -> red;
    newParticlePtr -> green = explodingParticle -> green;
    newParticlePtr -> blue = explodingParticle -> blue;

    newParticlePtr -> end_red = explodingParticle -> end_red;
    newParticlePtr -> end_green = explodingParticle -> end_green;
    newParticlePtr -> end_blue = explodingParticle -> end_blue;

    newParticlePtr -> previousParticle = particlesTail;
    newParticlePtr -> nextParticle = NULL;

    if(particlesHead == NULL)
    {
      particlesHead = newParticlePtr;
    }

    if(particlesTail != NULL)
      particlesTail -> nextParticle = newParticlePtr;

    particlesTail = newParticlePtr;

    particleCount = particleCount + 1;
  }
}

void generateTrailParticle(Particle* mainParticle)
{
  Particle* newParticlePtr = (Particle*)malloc(sizeof(Particle));

  newParticlePtr -> x_cord = mainParticle -> x_cord;
  newParticlePtr -> y_cord = mainParticle -> y_cord;
  newParticlePtr -> z_cord = mainParticle -> z_cord;

  newParticlePtr -> x_vel = 0;
  newParticlePtr -> y_vel = 0;
  newParticlePtr -> z_vel = 0;

  newParticlePtr -> size = mainParticle -> size * trailSizeScale;

  newParticlePtr -> age = 0;
  newParticlePtr -> lifetime = trailLifetime;

  newParticlePtr -> particleType = mainParticle -> particleType;

  newParticlePtr -> isExplode = 0;
  newParticlePtr -> isTrail = 1;

  newParticlePtr -> red = mainParticle -> red;
  newParticlePtr -> green = mainParticle -> green;
  newParticlePtr -> blue = mainParticle -> blue;

  newParticlePtr -> end_red = mainParticle -> end_red;
  newParticlePtr -> end_green = mainParticle -> end_green;
  newParticlePtr -> end_blue = mainParticle -> end_blue;

  newParticlePtr -> previousParticle = particlesTail;
  newParticlePtr -> nextParticle = NULL;

  if(particlesHead == NULL)
  {
    particlesHead = newParticlePtr;
  }

  if(particlesTail != NULL)
    particlesTail -> nextParticle = newParticlePtr;

  particlesTail = newParticlePtr;

  particleCount = particleCount + 1;
}

Particle* tempNextParticle = NULL;

Particle* updateParticle(Particle* currentParticle)
{
    if (currentParticle -> age + 1 > currentParticle -> lifetime)
    {
      if(explosionsOn && !currentParticle -> isExplode && !currentParticle -> isTrail)
      {
        explodeParticle(currentParticle);
      }

      if (currentParticle -> previousParticle != NULL)
      {
        (currentParticle -> previousParticle) -> nextParticle = currentParticle -> nextParticle;
      }
      else
      {
        particlesHead = currentParticle -> nextParticle;
      }

      if (currentParticle -> nextParticle != NULL)
      {
        (currentParticle -> nextParticle) -> previousParticle = currentParticle -> previousParticle;
      }
      else
      {
        particlesTail = currentParticle -> previousParticle;
      }

      tempNextParticle = currentParticle -> nextParticle;

      free(currentParticle);

      particleCount = particleCount - 1;

      return tempNextParticle;  
    }
    else
    {

      if (trailsOn && !currentParticle -> isExplode && !currentParticle -> isTrail)
      {
        generateTrailParticle(currentParticle);
      }

      currentParticle -> x_cord = currentParticle -> x_cord + currentParticle -> x_vel;
      currentParticle -> y_cord = currentParticle -> y_cord + currentParticle -> y_vel;
      currentParticle -> z_cord = currentParticle -> z_cord + currentParticle -> z_vel;

      if (planeOn && currentParticle -> y_cord - currentParticle -> size*0.5 < planeHeight
          && currentParticle -> y_vel < 0)
      {
        currentParticle -> y_vel = currentParticle -> y_vel * -1 * planeDampening;

        currentParticle -> x_vel = currentParticle -> x_vel * planeDampening;
        currentParticle -> z_vel = currentParticle -> z_vel * planeDampening;

        currentParticle -> y_cord = planeHeight + currentParticle -> size*0.5;
      }
      else
        currentParticle -> y_vel = currentParticle -> y_vel - gravityAcceleration*gravityIntensity;

      
      currentParticle -> age = currentParticle -> age + 1;

      return currentParticle -> nextParticle;
    }
}

float ageRatio;

void renderParticle(Particle* currentParticle)
{
    // To Stop particles fading or changing colour in fireworks mode
    if ((!explosionsOn && !trailsOn) || currentParticle -> isExplode || currentParticle -> isTrail)
      ageRatio = (float)currentParticle -> age / (float)currentParticle -> lifetime;
    else
      ageRatio = 0;

    if (ageRatio > point_of_death)
    {
      particleTransparency = DEFAULT_TRANSPARENCY * (1 - ((ageRatio - point_of_death) * 1.0/(1.0-point_of_death)));
    }
    else
    {
      particleTransparency = DEFAULT_TRANSPARENCY;
    }

    switch(currentParticle -> particleType)
    {
      case POINTS:
      glPointSize(currentParticle -> size);
      glBegin(GL_POINTS);
      glColor4f((currentParticle -> end_red - currentParticle -> red)*ageRatio + currentParticle -> red,
                  (currentParticle -> end_green - currentParticle -> green)*ageRatio + currentParticle -> green,
                  (currentParticle -> end_blue - currentParticle -> blue)*ageRatio + currentParticle -> blue,
                   particleTransparency);
      glVertex3f(currentParticle -> x_cord, currentParticle -> y_cord, currentParticle -> z_cord);
      glEnd();
      break;

      case STARS:
      glBegin(GL_TRIANGLES);

        glColor4f((currentParticle -> end_red - currentParticle -> red)*ageRatio + currentParticle -> red,
                  (currentParticle -> end_green - currentParticle -> green)*ageRatio + currentParticle -> green,
                  (currentParticle -> end_blue - currentParticle -> blue)*ageRatio + currentParticle -> blue,
                   particleTransparency);
         

        
        
        glVertex3f(currentParticle -> x_cord,
                   currentParticle -> y_cord - (currentParticle -> size),
                   currentParticle -> z_cord);

        glVertex3f(currentParticle -> x_cord + (currentParticle -> size),
                   currentParticle -> y_cord + (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);

        glVertex3f(currentParticle -> x_cord - (currentParticle -> size),
                   currentParticle -> y_cord + (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);

        glVertex3f(currentParticle -> x_cord - (currentParticle -> size),
                   currentParticle -> y_cord - (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);
        glVertex3f(currentParticle -> x_cord + (currentParticle -> size),
                   currentParticle -> y_cord - (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);
        glVertex3f(currentParticle -> x_cord,
                   currentParticle -> y_cord + (currentParticle -> size),
                   currentParticle -> z_cord);
      glEnd();
      break;

      case QUADS:
      glBegin (GL_QUADS);
        glColor4f((currentParticle -> end_red - currentParticle -> red)*ageRatio + currentParticle -> red,
                  (currentParticle -> end_green - currentParticle -> green)*ageRatio + currentParticle -> green,
                  (currentParticle -> end_blue - currentParticle -> blue)*ageRatio + currentParticle -> blue,
                   particleTransparency);

        glVertex3f(currentParticle -> x_cord - (currentParticle -> size)*0.5,
                   currentParticle -> y_cord - (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);
        glVertex3f(currentParticle -> x_cord + (currentParticle -> size)*0.5,
                   currentParticle -> y_cord - (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);
        glVertex3f(currentParticle -> x_cord + (currentParticle -> size)*0.5,
                   currentParticle -> y_cord + (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);
        glVertex3f(currentParticle -> x_cord - (currentParticle -> size)*0.5,
                   currentParticle -> y_cord + (currentParticle -> size)*0.5,
                   currentParticle -> z_cord);

      glEnd();
    }
}

void animate(void)
{
  if (particleBirthrate < 1)
  {
    // If tickNumber is multiple of birthrate^-1
    if (tickNumber % (int)(1.0/particleBirthrate) == 0)
    {
      generateParticle();
    }
  }
  else
  {
    for (i = 0; i < particleBirthrate; i++)
    {
      generateParticle();
    }
  }

  Particle* currentParticle = particlesHead;

  while (currentParticle != NULL)
  {
    currentParticle = updateParticle(currentParticle);
  }
  
  tickNumber = tickNumber + 1;

  glutPostRedisplay();
}

void display()
{
  glLoadIdentity();
  gluLookAt(0.0, 100.0, 1000.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);
  // If enabled, draw coordinate axis
  if(axisEnabled) glCallList(axisList);

  if(planeOn)
  {
    glBegin(GL_QUADS);
      glColor3f(0.15,0.15,0.15);
      glVertex3f(planeWidth*-0.5, planeHeight, planeLength*0.5);
      glVertex3f(planeWidth*0.5, planeHeight, planeLength*0.5);
      glVertex3f(planeWidth*0.5, planeHeight, planeLength*-0.5);
      glVertex3f(planeWidth*-0.5, planeHeight, planeLength*-0.5);
    glEnd();
  }  

  Particle* currentParticle = particlesHead;

  while (currentParticle != NULL)
  {
    renderParticle(currentParticle);

    currentParticle = currentParticle -> nextParticle;
  }

  glutSwapBuffers();
}

///////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 27: exit(0);
    break;
    case ' ': resetSimulation();
    break;
    case 'r': changeColour(RED, 0.1);
    break;
    case 'g': changeColour(GREEN, 0.1);
    break;
    case 'b': changeColour(BLUE, 0.1);
    break;
    case 't': changeEndColour(RED, 0.1);
    break;
    case 'h': changeEndColour(GREEN, 0.1);
    break;
    case 'n': changeEndColour(BLUE, 0.1);
    break;
    case ',': changeLifetime(-5);
    break;
    case '.': changeLifetime(5);
    break;
    case 's': toggleParticleStyle();
    break;
    case '\\': changeSize(-2);
    break;
    case '/': changeSize(2);
    break;
    case 'p': planeOn = !planeOn;
    break;
    case '\'': changeSpread(-1);
    break;
    case '#': changeSpread(1);
    break;
    case 'f': toggleFireworksMode();
    break;
    case '=': completelyRandomColours = !completelyRandomColours;
    break;
    case '1': explosionsOn = !explosionsOn;
              printParticleCount();
    break;
    case '2': trailsOn = !trailsOn;
              printParticleCount();
    break;
  }

  glutPostRedisplay();
}

void cursor_keys(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP: changeVelocity(0.5);
        break;
    case GLUT_KEY_DOWN: changeVelocity(-0.5);
        break;
    case GLUT_KEY_LEFT: changeGravity(-0.5);
        break;
    case GLUT_KEY_RIGHT: changeGravity(0.5);
        break;
    case GLUT_KEY_PAGE_UP: changeBirthrate(1);
        break;
    case GLUT_KEY_PAGE_DOWN: changeBirthrate(-1);
        break;
        
  }
} // cursor_keys()

///////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0.05, 0.05, 0.05, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////

void makeAxes() {
// Create a display list for drawing coord axis
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
      glLineWidth(2.0);
      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
  glEndList();
}

///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("COMP37111 Particles");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);

  // Allows transparency to work
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  makeAxes();

  // Add right click menu
  glutCreateMenu (menu);
  glutAddMenuEntry ("Increase Initial Velocity [UP]", 1);
  glutAddMenuEntry ("Decrease Initial Velocity [DOWN]", 2);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Select Random Colours", 3);
  glutAddMenuEntry ("(More Control With: [R],[G],[B])", 999);
  glutAddMenuEntry ("(Change End Colour: [T],[H],[N])", 999);
  glutAddMenuEntry ("Toggle Individually Random Colours [=]", 16);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Decrease Gravity [LEFT]", 5);
  glutAddMenuEntry ("Increase Gravity [RIGHT]", 4);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Decrease Lifetime [,]", 6);
  glutAddMenuEntry ("Increase Lifetime [.]", 7);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Set Particle Count = 1,000", 19);
  glutAddMenuEntry ("Set Particle Count = 10,000", 10);
  glutAddMenuEntry ("Set Particle Count = 100,000", 8);
  glutAddMenuEntry ("Set Particle Count = 1,000,000", 9);
  glutAddMenuEntry ("([PAGE UP] + [PAGE DOWN])", 999);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Toggle Particle Style [S]", 11);
  glutAddMenuEntry ("Decrease Particle Size [\\]", 12);
  glutAddMenuEntry ("Increase Particle Size [/]", 13);
  glutAddMenuEntry ("Change Particle Spread: [']+[#]", 999);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Toggle Ground Plane [P]", 14);
  glutAddMenuEntry ("Toggle Firework Mode [F]", 15);
  glutAddMenuEntry ("Toggle Explosions [1]", 17);
  glutAddMenuEntry ("Toggle Trails [2]", 18);
  glutAttachMenu (GLUT_RIGHT_BUTTON);

  resetSimulation();
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  double f;
  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_CULL_FACE);
  glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
  glutIdleFunc (animate);
  glutSpecialFunc (cursor_keys);
  glutMainLoop();
}