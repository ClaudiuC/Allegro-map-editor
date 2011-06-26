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
*** \file    tilesetmain.cpp
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Source file for the tileset widget.
******************************************************************************/

#include "tilesetmain.h"

extern EditorMain editor;
extern GuiMain gui;
extern GuiResources resources;
extern InputMouse mouse;

TilesetMain::TilesetMain() {
    tilesetIndex = TILES1;
    scroll_y = 0;

    isSetMouseZ = false;

    object_x = object_y = object_x2 = object_y2 = 0;
    object_selected = false;
}

TilesetMain::~TilesetMain() {
}

void TilesetMain::initTileset() {

}

void TilesetMain::freeTileset() {

}

void TilesetMain::changeIndex(short direction) {

    if (direction == NEXT) {
        if (tilesetIndex <= 5) tilesetIndex++;
        else tilesetIndex = 0;
    } else if (direction == PREVIOUS) {
        if (tilesetIndex > TILES1) tilesetIndex--;
        else tilesetIndex = 6;
    }
}

void TilesetMain::scrollTileset(short direction) {

    if (direction == DOWN) {
        if (scroll_y <= 768/2 + 32) scroll_y += 32;
    } else {
        if (scroll_y - 32 >= 0) scroll_y -= 32;
    }

}

string TilesetMain::getTilesetName() {
    return (get_datafile_property(editor.mapData + tilesetIndex, DAT_ID('N','A','M','E')));
}

void TilesetMain::updateTilesetActions() {
    // Only grab tiles when in edit mode
    if (!gui.getPreview()) {
        if (gui.getMouseFrame() == TSET_FRAME) {
            mouse.setCursor(GROUP);
            if (!isSetMouseZ) {
                editor.setBrushSize(mouse_z);
                isSetMouseZ = true;
            }
            position_mouse_z(-2);


            if (mouse_b & 1) {
                if (gui.getBrush() == BRUSH_COLLISION || gui.getBrush() == BRUSH_ERASE) {
                    gui.setBrush(BRUSH_DRAW);
                }

                short x1 = gui.getMouseX()/32;
                short y1 = (gui.getMouseY()+scroll_y)/32;

                editor.current_tile = x1*TILESIZE+y1;
                editor.mouse_tileset = tilesetIndex;
                editor.isObject = false;

                object_x = object_y = object_x2 = object_y2 = 0;
                object_selected = false;
            }

            if (key[KEY_1] && !object_selected) {
                gui.setBrush(BRUSH_DRAW);

                object_x = gui.getMouseX()/32;
                object_y = (gui.getMouseY()+scroll_y)/32;

                editor.current_tile = object_x*32+object_y;
                editor.mouse_tileset = tilesetIndex;

                object_x2 = object_x;
                object_y2 = object_y;

                object_selected = true;

                editor.current_object_x1 = object_x;
                editor.current_object_y1 = object_y;

                editor.object_tileset = tilesetIndex;
                editor.isObject = true;
            }

            if (key[KEY_2] && object_selected && tilesetIndex == editor.object_tileset) {
                object_x2 = gui.getMouseX()/32;
                object_y2 = (gui.getMouseY()+scroll_y)/32;
                object_selected = false;

                editor.current_object_x2 = object_x2;
                editor.current_object_y2 = object_y2;

                if (object_x > object_x2) {
                    short aux = object_x;
                    object_x = object_x2;
                    object_x2 = aux;
                }
                if (object_y > object_y2) {
                    short aux = object_y;
                    object_y = object_y2;
                    object_y2 = aux;
                }
            }

        } else {
            isSetMouseZ = false;
            if (gui.getMouseFrame() != MAIN_FRAME) {
                mouse.setCursor(CURSOR);
            }
        }
    }
}

void TilesetMain::drawTileset(BITMAP *bmp, short x, short y, short w, short h) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 19; j++) {
            blit((BITMAP*)resources.data[TRANS_BK].dat, bmp, 0, 0, (i*32) + 768, (j*32) + 32, 32, 32);
        }
    }

    masked_blit((BITMAP*)editor.mapData[tilesetIndex].dat, bmp, 0, 0+scroll_y, x, y, w, h);

    if (gui.getMouseFrame() == TSET_FRAME) {
        short x1, y1, x2, y2;

        if (!editor.isObject) {
            x1=TILESIZE*(mouse_x/TILESIZE);
            y1=TILESIZE*(mouse_y/TILESIZE);
            x2=x1+TILESIZE;
            y2=y1+TILESIZE;
            editor.drawSelector(bmp, x1, y1, x2, y2, BRUSH_DRAW);
        } else {
            if (tilesetIndex == editor.object_tileset) {
                x1 = object_x*32 + 769;
                x2 = object_x2*32 + 768+32;
                y1 = object_y*32 + 32  - scroll_y;
                y2 = object_y2*32 + 64 - scroll_y;

                if (y1 > 32*20) y1 = 32*20;
                if (y2 > 32*20) y2 = 32*20;

                drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                rectfill(bmp, x1, y1, x2, y2, makecol(180,180,180));
                drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

                rect(bmp, x1, y1, x2, y2, makecol(128,128,180));
                //rectfill(bmp, x1, y1, x2, y2, makecol(128, 128, 128));
                //floodfill(bmp, x1, y1, 2);
            }
        }
    }
}
