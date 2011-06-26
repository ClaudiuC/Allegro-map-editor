///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2009 by Hazardous Gaming
//                         All Rights Reserved
//
// This code is licensed under the MIT License. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.opensource.org/licenses/mit-license.php for details.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
*** \file    particleemitter.h
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Header file for the Particle Emitter class
***
*** This code provides a simple particle emitter API. Should be worked on and
*** developed further so it would be able to give relatively nice effects
***
*** \note Works with separate methods for each type of particle. Should be built
*** upon so it can accept pointers to functions. That would allow further
*** development so it can accept "scripted" particles. The callback function
*** should be called inside the update method. Also, a simple particle creator
*** utility would be great, thus allowing modifications on the script code and
*** viewing of the emitter at run-time.
***
*** \note This code uses the following libraries:
***   -# Allegro 4.2.2, http://www.allegro.cc/
******************************************************************************/

#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <math.h>
#include <allegro.h>
#include <iostream>

using namespace std;

#define RAIN_PARTICLE  0
#define SNOW_PARTICLE1 1
#define SNOW_PARTICLE2 2
#define SMOKE_PARTICLE 3
#define SOME_PARTICLE  4

typedef struct FORCE {
    double fx, fy;
    double pos_x, pos_y;
    double spread_x, spread_y;

    bool active, dead;
} FORCE;

typedef struct PARTICLE {
    double x,y,             //!< Position on screen
    dx,dy,                  //!< Speed and direction of movement
    rx,ry,                  //!< Movement randomness
    tx,ty;                  //!< Last particle position
    int color,              //!< Particle color
    type;                   //!< Particle type
    double life;            //!< Particle life
    double speed;

    struct FORCE force;
    struct PARTICLE *next;  //!< Pointer to the next particle, a simple linked list should do
    struct PARTICLE *previous;  //!< Pointer to the next particle, a simple linked list should do
} PARTICLE;

// TODO: #P1: Add PARTICLE as a private member (Like in the GUI code);
// TODO: #P2: Better PARTICLE structure (note that there should be "global" particles and "local" particles)
// TODO: #P3: More methods for verifing particle states, how long they've been active etc.
// TODO: #P4: Point-based particles
// TODO: #P5: PARTICLE GUI editor -> callback functions -> scripting



// Custom proc
void drawCustomParticle(BITMAP *bmp, PARTICLE p);
void createCustomType(PARTICLE p);

class ParticleEmitter {
    friend class EditorMain;
public:
    ParticleEmitter();
    ~ParticleEmitter();

    void createParticles(double x, double y, double dx, double dy, double rx, double ry, double life, int color, double speed, short type);
    void createParticles(double x, double y, short type, void(*proc)(PARTICLE *p));

    short updateParticles();
    void initForces(int forces);
    void applyForce(double posx, double posy, double spread_x, double spread_y, double fx, double fy);

    void drawParticle(BITMAP *bmp, PARTICLE *p);
    void drawParticles(BITMAP *bmp);

    void drawParticles(BITMAP *bmp, void(*proc)(BITMAP *bmp, PARTICLE p));

    int getParticleCount() { return particle_index; }
protected:
private:
    PARTICLE *particles;
    PARTICLE *firstParticle;
    PARTICLE *lastParticle;

    short particle_index;
    short force_index;
};

#endif // PARTICLEEMITTER_H
