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
*** \file    guiscrollbar.cpp
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Source file for the scrollbar API.
******************************************************************************/

#include "guiscrollbar.h"

extern InputMouse mouse;

GuiScrollbar::GuiScrollbar() {
    firstScrollbar = lastScrollbar = scrollbarAttributes = NULL;
    auto_increment_id = 0;
}

GuiScrollbar::~GuiScrollbar() {
    SCROLLBAR_MASK* thisScrollbar = firstScrollbar;
    SCROLLBAR_MASK* destroyedScrollbar = NULL;

    while (thisScrollbar != NULL) {
        destroyedScrollbar = thisScrollbar;
        thisScrollbar = thisScrollbar->next;
        delete destroyedScrollbar;
    }
    firstScrollbar = NULL;
    lastScrollbar = NULL;
}

SCROLLBAR_MASK *GuiScrollbar::getScrollbarByID(short scrollbar_id) {
    SCROLLBAR_MASK* thisScrollbar = firstScrollbar;
    SCROLLBAR_MASK* foundScrollbar = NULL;

    while (thisScrollbar != NULL) {
        if (thisScrollbar->ID != scrollbar_id) thisScrollbar = thisScrollbar->next;
        else {
            foundScrollbar = thisScrollbar;
            return foundScrollbar;
        }
    }
    return NULL;
}

void GuiScrollbar::addScrollbar(short x, short y, short w, short h, short abstract_value, short step, short type) {
    if (firstScrollbar==NULL) {
        firstScrollbar = new SCROLLBAR_MASK;
        lastScrollbar = firstScrollbar;
        lastScrollbar->next = NULL;
        lastScrollbar->previous = NULL;
    } else {
        lastScrollbar->next = new SCROLLBAR_MASK;
        lastScrollbar->next->previous = lastScrollbar;
        lastScrollbar = lastScrollbar->next;
        lastScrollbar->next = NULL;
    }
    lastScrollbar->x = x;
    lastScrollbar->y = y;
    lastScrollbar->w = w;
    lastScrollbar->h = h;
    lastScrollbar->abstract_value = abstract_value;
    lastScrollbar->type = type;
    lastScrollbar->step = step;
    lastScrollbar->current_position = 0;
    switch (lastScrollbar->type) {
    case SCROLLBAR_HORIZONTAL: {
        lastScrollbar->real_value = lastScrollbar->w - lastScrollbar->step;
        break;
    }
    case SCROLLBAR_VERTICAL: {
        lastScrollbar->real_value = lastScrollbar->h - lastScrollbar->step;
        break;
    }
    }

    lastScrollbar->destroyed = false;
    lastScrollbar->displayed = true;
    lastScrollbar->mouse_in = false;

    lastScrollbar->ID = auto_increment_id;

    auto_increment_id++;

    scrollbarAttributes = lastScrollbar;
}

short GuiScrollbar::getLastScrollbarID() {
    return lastScrollbar->ID;
}


short GuiScrollbar::updateScrollbars() {
    SCROLLBAR_MASK *thisScrollbar = firstScrollbar;

    while (thisScrollbar != NULL) {
        thisScrollbar->mouse_in = mouse.getMouseFocus(thisScrollbar->x, thisScrollbar->y, thisScrollbar->x + thisScrollbar->w, thisScrollbar->y + thisScrollbar->h);
        if (thisScrollbar->mouse_in) {
            if (mouse_b & 1) {
                switch (thisScrollbar->type) {
                case SCROLLBAR_HORIZONTAL: {
                    if ((mouse_x + thisScrollbar->step/2 < thisScrollbar->x + thisScrollbar->w) && (mouse_x > thisScrollbar->x + thisScrollbar->step/2)) {
                        thisScrollbar->current_position = mouse_x-thisScrollbar->step/2;
                    }
                    break;
                }
                case SCROLLBAR_VERTICAL: {
                    if ((mouse_y + thisScrollbar->step/2 < thisScrollbar->y + thisScrollbar->h) && (mouse_y > thisScrollbar->y + thisScrollbar->step/2)) {
                        thisScrollbar->current_position = mouse_y-thisScrollbar->step/2-32;
                    }
                    break;
                }
                }
                break;
            }
        }
        thisScrollbar = thisScrollbar->next;
    }


    if (thisScrollbar != NULL) return thisScrollbar->ID;
    else return -1;
}

void GuiScrollbar::drawScrollbarImage(BITMAP *bmp, short scrollbar_id) {
    SCROLLBAR_MASK *thisScrollbar = getScrollbarByID(scrollbar_id);

    rect(bmp, thisScrollbar->x, thisScrollbar->y, thisScrollbar->x + thisScrollbar->w, thisScrollbar->y + thisScrollbar->h, makecol(0, 0, 0));
    switch (thisScrollbar->type) {
    case SCROLLBAR_HORIZONTAL: {
        rectfill(bmp, thisScrollbar->x + thisScrollbar->current_position+1, thisScrollbar->y+1, thisScrollbar->x + thisScrollbar->current_position + thisScrollbar->step-1, thisScrollbar->y + thisScrollbar->h-1, makecol(120, 120, 120));
        break;
    }
    case SCROLLBAR_VERTICAL: {
        rectfill(bmp, thisScrollbar->x+1, thisScrollbar->y + thisScrollbar->current_position+1, thisScrollbar->x + thisScrollbar->w-1, thisScrollbar->y + thisScrollbar->current_position + thisScrollbar->step-1, makecol(120, 120, 120));
        break;
    }
    }
}

void GuiScrollbar::drawScrollbars(BITMAP *bmp) {

    SCROLLBAR_MASK* thisScrollbar = firstScrollbar;
    SCROLLBAR_MASK* destroyedScrollbar = NULL;

    while (thisScrollbar != NULL) {
        if (thisScrollbar->destroyed) {
            destroyedScrollbar = thisScrollbar;
            thisScrollbar = thisScrollbar->next;

            if (firstScrollbar == destroyedScrollbar) {
                firstScrollbar = thisScrollbar;
                if (thisScrollbar != NULL) {
                    thisScrollbar->previous = NULL;
                }
            } else {
                destroyedScrollbar->previous->next=thisScrollbar;
                if (thisScrollbar != NULL) {
                    thisScrollbar->previous=destroyedScrollbar->previous;
                }
            }
            if (lastScrollbar == destroyedScrollbar) {
                lastScrollbar = destroyedScrollbar->previous;
            }
            delete destroyedScrollbar;
        } else {
            if (thisScrollbar->displayed) {
                drawScrollbarImage(bmp, thisScrollbar->ID);
            }
            thisScrollbar=thisScrollbar->next;
        }
    }
}
