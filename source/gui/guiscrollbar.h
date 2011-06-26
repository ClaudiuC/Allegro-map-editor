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
*** \file    guiscrollbar.h
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Header file for the scrollbar API.
***
*** This code provides the API used to manage GUI scrollbars using a doubly linked
*** list.
***
*** \note This code uses the following libraries:
***   -# Allegro 4.2.2, http://www.allegro.cc/
******************************************************************************/

#ifndef GUISCROLLBAR_H
#define GUISCROLLBAR_H

#include <string>
#include "..\input\inputmouse.h"

using namespace std;

#define SCROLLBAR_HORIZONTAL 0
#define SCROLLBAR_VERTICAL   1

typedef struct SCROLLBAR_MASK {
    short x, y;
    short w, h;

    short real_value, abstract_value, step, current_position;
    short type;

    bool destroyed, displayed;

    short ID;

    bool mouse_in;

    SCROLLBAR_MASK *next;
    SCROLLBAR_MASK *previous;
} SCROLLBAR_MASK;

class GuiScrollbar {
public:
    GuiScrollbar();
    ~GuiScrollbar();

    void addScrollbar(short x, short y, short w, short h, short abstract_value, short step, short type);
    short getLastScrollbarID();
    SCROLLBAR_MASK* getScrollbarByID(short scrollbar_id);
    short updateScrollbars();
    void drawScrollbarImage(BITMAP *bmp, short scrollbar_id);
    void drawScrollbars(BITMAP *bmp);

    short getScrollbarValue(short scrollbar_id) {
        return ((getScrollbarByID(scrollbar_id)->current_position-getScrollbarByID(scrollbar_id)->x)/(getScrollbarByID(scrollbar_id)->real_value/getScrollbarByID(scrollbar_id)->abstract_value));
    }
    short getScrollbarType(short scrollbar_id)   {
        return getScrollbarByID(scrollbar_id)->type;
    }
    short getScrollbarAbstractValue(short scrollbar_id)   {
        return getScrollbarByID(scrollbar_id)->abstract_value;
    }
    short getScrollbarRealValue(short scrollbar_id)   {
        return getScrollbarByID(scrollbar_id)->real_value;
    }
    short getScrollbarPosX(short scrollbar_id)   {
        return getScrollbarByID(scrollbar_id)->x;
    }
    short getScrollbarPosY(short scrollbar_id)   {
        return getScrollbarByID(scrollbar_id)->y;
    }
    short getScrollbarSizeW(short scrollbar_id)  {
        return getScrollbarByID(scrollbar_id)->w;
    }
    short getScrollbarSizeH(short scrollbar_id)  {
        return getScrollbarByID(scrollbar_id)->h;
    }
    short getScrollbarID(short scrollbar_id)     {
        return getScrollbarByID(scrollbar_id)->ID;
    }

    bool getScrollbarMouseIn(short scrollbar_id) {
        return getScrollbarByID(scrollbar_id)->mouse_in;
    }
protected:
private:
    SCROLLBAR_MASK *scrollbarAttributes;
    SCROLLBAR_MASK *firstScrollbar;
    SCROLLBAR_MASK *lastScrollbar;

    short auto_increment_id;
};

#endif // GUISCROLLBAR_H
