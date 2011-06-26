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
*** \file    guimain.h
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Header file for the GUI engine.
***
*** This code provides the interface to the GUI engine.
***
*** \note This code uses the following libraries:
***   -# Allegro 4.2.2, http://www.allegro.cc/
******************************************************************************/

#ifndef GUIMAIN_H
#define GUIMAIN_H

#include "guibutton.h"
#include "guiframe.h"
#include "guicheckbox.h"
#include "guiscrollbar.h"
#include "..\input\inputmouse.h"
#include "guitextfield.h"
#include "guiresources.h"
#include "guilabel.h"
#include "..\editor\editormain.h"
#include "..\editor\tilesetmain.h"
#include "..\editor\minimapmain.h"

#define STATE_NEWMAP    0
#define STATE_LOADMAP   1
#define STATE_SAVEMAP   2
#define STATE_OPTIONS   3
#define STATE_PARTICLES 4

#define LAYER_ONE     0
#define LAYER_TWO     1
#define LAYER_THREE   2
#define LAYER_FOUR    3
#define LAYER_FIVE    4
#define LAYER_SIX     5
#define LAYER_SEVEN   6
#define LAYER_EIGHT   7
#define LAYER_NINE    8
#define LAYER_TEN     9

#define BRUSH_DRAW      0
#define BRUSH_ERASE     1
#define BRUSH_FLOOD     2
#define BRUSH_COLLISION 3
#define BRUSH_EMITTER   4

#define MAIN_FRAME    9
#define TSET_FRAME   10
#define MINI_FRAME   11
#define NO_FRAME     12

#define TILESIZE     32

class GuiMain
{
    friend class EditorMain;
    public:
        GuiMain();
        ~GuiMain();

        void loadInterface();
        void updateInterface();
        void drawInterface(BITMAP *bmp);

        short getPanelState() { return panelState; }
        short getMouseFrame() { return mouse_frame; }
        int getMouseX() { return gui_x; }
        int getMouseY() { return gui_y; }

        short getCurrentLayer() { return currentLayer; }
        bool isFieldActive() {
            if (field.getActiveField() == -1) return false;
                else return true;
        }
        bool getGrid() { return grid; }
        bool getOtherGrids() { return collision; }
        void setCollision(short state) { checkbox.setCheckboxState(checkboxCollision, state); }
        bool getLayers() { return layers; }
        bool getPreview() { return preview; }
        bool getAlpha() { return alpha; }

        short getBrush() { return brush; }
        void setBrush(short type) { brush = type; }

        bool getQuit() { return quit; }
        void clearInterface();
    protected:
    private:
        short panelState;
        short currentLayer;
        bool grid, collision, layers, quit;
        short brush;
        bool preview, alpha;

        short buttonNewMap,
              buttonLoadMap,
              buttonSaveMap,
              buttonOptions,
              buttonQuit,
              buttonParticles,
              buttonLayerOne,
              buttonLayerTwo,
              buttonLayerThree,
              checkboxGrid,
              checkboxCollision,
              checkboxLayers,
              buttonDraw,
              buttonErase,
              buttonCollision,
              frameMain,
              frameTset,
              frameMini,
              frameAll,
              checkboxPreview,
              checkboxAlpha,
              buttonScrollUp,
              buttonScrollDown,
              buttonNext,
              buttonPrevious,
              labelLayers,
              labelDisplay,
              labelBrush,

              buttonLoadMapOK,
              fieldLoadName,

              buttonNewMapOK,
              fieldNewName,
              fieldNewLay,
              fieldNewW,
              fieldNewH,

              buttonSaveMapOK,
              fieldSaveName,

              frameSettings,
              buttonLayerFour,
              buttonLayerFive,
              buttonLayerSix,
              buttonLayerSeven,
              buttonLayerEight,
              buttonLayerNine,
              buttonLayerTen,

              buttonFlood,

              labelPPanel,
              buttonPPause,
              buttonPPlay,
              buttonPAdd,
              buttonFAdd,
              labelPOptions
              ;

        short mouse_frame;
        int gui_x, gui_y;

        GuiButton button;
        GuiFrame frame;
        GuiCheckbox checkbox;
        GuiScrollbar scrollbar;
        GuiField field;
        GuiLabel label;

        //BITMAP *brush_bmp;
};

#endif // GUIMAIN_H
