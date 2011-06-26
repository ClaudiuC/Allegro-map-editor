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

#include "particleemitter.h"

extern volatile int particle_timer;

ParticleEmitter::ParticleEmitter() {
    //particle = NULL;

    firstParticle = lastParticle = particles = NULL;
    particle_index = 0;
    force_index = 0;
}

ParticleEmitter::~ParticleEmitter() {
    PARTICLE *thisParticle = firstParticle;
    PARTICLE *destroyedParticle = NULL;

    while (thisParticle != NULL) {
        destroyedParticle = thisParticle;
        thisParticle = thisParticle->next;
        delete destroyedParticle;
    }
    firstParticle = NULL;
    lastParticle = NULL;
}

void ParticleEmitter::createParticles(double x, double y, double dx, double dy, double rx, double ry, double life, int color, double speed, short type) {
    if (firstParticle==NULL) {
        firstParticle = new PARTICLE;
        lastParticle = firstParticle;
        lastParticle->next = NULL;
        lastParticle->previous = NULL;
    } else {
        lastParticle->next = new PARTICLE;
        lastParticle->next->previous = lastParticle;
        lastParticle = lastParticle->next;
        lastParticle->next = NULL;
    }


    lastParticle->x = x;
    lastParticle->y = y;


    lastParticle->dx = dx;
    lastParticle->dy = dy;

    lastParticle->rx = rx;
    lastParticle->ry = ry;

    lastParticle->life = life;

    lastParticle->color = color;

    lastParticle->speed = speed;
    lastParticle->type = type;

    lastParticle->force.fx = 0;
    lastParticle->force.fy = 0;

    lastParticle->force.spread_x = 0;
    lastParticle->force.spread_y = 0;

    lastParticle->force.pos_x = 0;
    lastParticle->force.pos_y = 0;

    lastParticle->force.active = false;

    particle_index++;

    particles = lastParticle;

}


void ParticleEmitter::createParticles(double x, double y, short type, void(*proc)(PARTICLE *p)) {
    if (firstParticle==NULL) {
        firstParticle = new PARTICLE;
        lastParticle = firstParticle;
        lastParticle->next = NULL;
        lastParticle->previous = NULL;
    } else {
        lastParticle->next = new PARTICLE;
        lastParticle->next->previous = lastParticle;
        lastParticle = lastParticle->next;
        lastParticle->next = NULL;
    }


    lastParticle->x = x;
    lastParticle->y = y;

    lastParticle->force.fx = 0;
    lastParticle->force.fy = 0;

    lastParticle->force.spread_x = 0;
    lastParticle->force.spread_y = 0;

    lastParticle->force.pos_x = 0;
    lastParticle->force.pos_y = 0;

    lastParticle->force.active = false;

    particle_index++;

    lastParticle->type = type;
    proc(lastParticle);
    particles = lastParticle;

}

short ParticleEmitter::updateParticles() {
    PARTICLE *thisParticle = firstParticle;

    while (thisParticle != NULL) {

        //thisParticle->dx = thisParticle->life;

        thisParticle->x +=thisParticle->dx;
        thisParticle->y +=thisParticle->dy;

        thisParticle->tx = thisParticle->x;
        thisParticle->ty = thisParticle->y;

        if (thisParticle->rx>0) thisParticle->x += rand() % (int)(thisParticle->rx*2) - thisParticle->rx;
        if (thisParticle->ry>0) thisParticle->y += rand() % (int)(thisParticle->ry*2) - thisParticle->ry;

        if (thisParticle->life > thisParticle->speed) thisParticle->life -= thisParticle->speed;
        else thisParticle->life = 0;

        if ((thisParticle->x + thisParticle->force.spread_x > thisParticle->force.pos_x) &&
                (thisParticle->x - thisParticle->force.spread_x < thisParticle->force.pos_x) &&
                (thisParticle->y + thisParticle->force.spread_y > thisParticle->force.pos_y) &&
                (thisParticle->y - thisParticle->force.spread_y < thisParticle->force.pos_y)) {

            if (!lastParticle->force.active) {
                double aux = thisParticle->dx;
                thisParticle->dx = thisParticle->force.fx;
                thisParticle->force.fx = aux;

                aux = thisParticle->dy;
                thisParticle->dy = thisParticle->force.fy;
                thisParticle->force.fy = aux;

                lastParticle->force.active = true;
            }

        } else {
            if (lastParticle->force.active) {
                thisParticle->dx = thisParticle->force.fx;
                thisParticle->dy = thisParticle->force.fy;

                lastParticle->force.active = false;
            }
        }

        thisParticle = thisParticle->next;
    }

    return 0;
}

/*
void ParticleEmitter::initForces(int forces) {
    PARTICLE *thisParticle = firstParticle;

    while (thisParticle != NULL) {
        if ((abs(thisParticle->x - posx) < SCREEN_W) || (abs(thisParticle->y - posy) < SCREEN_H)) {
            thisParticle->force.fx = fx;
            thisParticle->force.fy = fy;

            thisParticle->force.spread_x = spread_x;
            thisParticle->force.spread_y = spread_y;

            thisParticle->force.pos_x = posx;
            thisParticle->force.pos_y = posy;
        }

        thisParticle = thisParticle->next;
    }
}
*/

void ParticleEmitter::applyForce(double posx, double posy, double spread_x, double spread_y, double fx, double fy) {
    PARTICLE *thisParticle = firstParticle;

    while (thisParticle != NULL) {
        if ((abs((int)thisParticle->x - (int)posx) < SCREEN_W) || (abs((int)thisParticle->y - (int)posy) < SCREEN_H)) {
            thisParticle->force.fx = fx;
            thisParticle->force.fy = fy;

            thisParticle->force.spread_x = spread_x;
            thisParticle->force.spread_y = spread_y;

            thisParticle->force.pos_x = posx;
            thisParticle->force.pos_y = posy;
        }

        thisParticle = thisParticle->next;
    }

}

void ParticleEmitter::drawParticle(BITMAP *bmp, PARTICLE *p) {
    switch (p->type) {
    case RAIN_PARTICLE:
        line(bmp, (int)p->x, (int)p->y, (int)p->tx, (int)p->ty, p->color);
        break;

    case SNOW_PARTICLE1:
        putpixel(bmp, (int)p->x, (int)p->y, p->color);
        break;

    case SNOW_PARTICLE2:
        putpixel(bmp, (int)p->x, (int)p->y, p->color);
        putpixel(bmp, (int)p->x-1, (int)p->y, p->color);
        putpixel(bmp, (int)p->x+1, (int)p->y, p->color);
        putpixel(bmp, (int)p->x, (int)p->y-1, p->color);
        putpixel(bmp, (int)p->x, (int)p->y+1, p->color);
        break;

    case SMOKE_PARTICLE: {

        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        short aux = 0;

        if (p->life < 40) aux = 1;
        if (p->life < 20) aux = 2;
        if (p->life < 5) aux = 3;

        set_trans_blender(p->color, p->color, p->color, 10-aux);
        circlefill(bmp, (int)p->x, (int)p->y, 1, p->color);


        set_trans_blender(p->color, p->color, p->color, 8-aux);

        circlefill(bmp, (int)p->x, (int)p->y, 2, p->color);


        set_trans_blender(p->color, p->color, p->color, 6-aux);

        circlefill(bmp, (int)p->x, (int)p->y, 12, p->color);


        set_trans_blender(p->color, p->color, p->color, 5-aux);

        circlefill(bmp, (int)p->x, (int)p->y, 15, p->color);


        set_trans_blender(p->color, p->color, p->color, 4-aux);

        circlefill(bmp, (int)p->x, (int)p->y, 18, p->color);

        set_trans_blender(p->color, p->color, p->color, 3-aux);
        circlefill(bmp, (int)p->x, (int)p->y, 20, p->color);

        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
        set_trans_blender(128, 128, 128, 128);

        break;
    }
    case SOME_PARTICLE:
        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        short aux = 0;
        if (p->life < 400) aux = 5;
        if (p->life < 300) aux = 10;
        if (p->life < 200) aux = 20;
        if (p->life < 100) aux = 30;

        set_trans_blender(128, 128, 128, 100-aux);
        line(bmp, (int)p->x, (int)p->y, (int)p->tx-3, (int)p->ty, p->color);


        set_trans_blender(p->color, p->color, p->color, 80-aux);

        line(bmp, (int)p->x, (int)p->y, (int)p->tx-2, (int)p->ty, p->color);


        set_trans_blender(p->color, p->color, p->color, 60-aux);

        line(bmp, (int)p->x, (int)p->y, (int)p->tx-1, (int)p->ty, p->color);


        set_trans_blender(p->color, p->color, p->color, 50-aux);

        line(bmp, (int)p->x, (int)p->y, (int)p->tx+1, (int)p->ty, p->color);


        set_trans_blender(p->color, p->color, p->color, 40-aux);

        line(bmp, (int)p->x, (int)p->y, (int)p->tx+2, (int)p->ty, p->color);

        set_trans_blender(p->color, p->color, p->color, 30-aux);
        line(bmp, (int)p->x, (int)p->y, (int)p->tx+3, (int)p->ty, p->color);

        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

        break;

        break;
    }
}

void ParticleEmitter::drawParticles(BITMAP *bmp, void(*proc)(BITMAP *bmp, PARTICLE p)) {
    PARTICLE *thisParticle = firstParticle;
    PARTICLE *destroyedParticle = NULL;

    while (thisParticle != NULL) {
        if (thisParticle->life <= 0) {
            destroyedParticle = thisParticle;
            thisParticle = thisParticle->next;

            if (firstParticle == destroyedParticle) {
                firstParticle = thisParticle;
                if (thisParticle != NULL) {
                    thisParticle->previous = NULL;
                }
            } else {
                destroyedParticle->previous->next=thisParticle;
                if (thisParticle != NULL) {
                    thisParticle->previous=destroyedParticle->previous;
                }
            }
            if (lastParticle == destroyedParticle) {
                lastParticle = destroyedParticle->previous;
            }
            delete destroyedParticle;
            particle_index--;
        } else {
            proc(bmp, *thisParticle);
        }
        thisParticle=thisParticle->next;
    }
}

void ParticleEmitter::drawParticles(BITMAP *bmp) {

    PARTICLE *thisParticle = lastParticle;
    PARTICLE *destroyedParticle = NULL;

    while (thisParticle != NULL) {
        if (thisParticle->life <= 0) {
            destroyedParticle = thisParticle;
            thisParticle = thisParticle->next;

            if (firstParticle == destroyedParticle) {
                firstParticle = thisParticle;
                if (thisParticle != NULL) {
                    thisParticle->previous = NULL;
                }
            } else {
                destroyedParticle->previous->next=thisParticle;
                if (thisParticle != NULL) {
                    thisParticle->previous=destroyedParticle->previous;
                }
            }
            if (lastParticle == destroyedParticle) {
                lastParticle = destroyedParticle->previous;
            }
            delete destroyedParticle;
            particle_index--;
        } else {
            drawParticle(bmp, thisParticle);
        }
        thisParticle=thisParticle->previous;
    }
}
