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
*** \file    minimapmain.cpp
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Source file for the minimap widget
******************************************************************************/

#include "minimapmain.h"

extern EditorMain editor;
extern GuiMain gui;
extern InputMouse mouse;

MinimapMain::MinimapMain() {
    minimap_x = 4;
    minimap_y = 644;
    aux_resize = 1;

    updateMiniMapCoords();
}

MinimapMain::~MinimapMain() {
}

void MinimapMain::initMinimap() {

    mini = create_bitmap(56,32);

    if (minimap != NULL) {
        destroy_bitmap(minimap);
    }

    minimap = create_bitmap(50, 50);
    clear(minimap);

    stretch_blit((BITMAP*)editor.mapData[TILES1].dat,mini,0,0,256,1024,0,0,8,32);
    stretch_blit((BITMAP*)editor.mapData[TILES2].dat,mini,0,0,256,1024,8,0,8,32);
    stretch_blit((BITMAP*)editor.mapData[TILES3].dat,mini,0,0,256,1024,16,0,8,32);
    stretch_blit((BITMAP*)editor.mapData[TILES4].dat,mini,0,0,256,1024,24,0,8,32);
    stretch_blit((BITMAP*)editor.mapData[TILES5].dat,mini,0,0,256,1024,32,0,8,32);
    stretch_blit((BITMAP*)editor.mapData[TILES6].dat,mini,0,0,256,1024,40,0,8,32);
    stretch_blit((BITMAP*)editor.mapData[TILES7].dat,mini,0,0,256,1024,48,0,8,32);
}

void MinimapMain::updateMiniMapCoords() {

    if (editor.mapWidth != editor.mapHeight) {
        if (editor.mapWidth > MAX_SIZE && editor.mapWidth > editor.mapHeight) {
            aux_resize = editor.mapWidth/MAX_SIZE;
            minimap_x = (minimap_x+64) - ((editor.mapWidth/aux_resize)/2);
            minimap_y = (minimap_y+64) - ((editor.mapHeight/aux_resize)/2);
        }
        if (editor.mapHeight > MAX_SIZE && editor.mapWidth < editor.mapHeight) {
            aux_resize = editor.mapHeight/MAX_SIZE;
            minimap_y = minimap_y + ((editor.mapHeight/aux_resize)/2);
            minimap_x = minimap_x + (editor.mapWidth/2);
        }
    } else {
        if (editor.mapWidth > MAX_SIZE) {
            aux_resize = editor.mapWidth/MAX_SIZE;
        }
    }
}

void MinimapMain::drawMiniMap(BITMAP *bmp) {
    int i,j, l;
    int val, ts;
    int x,y;
    int r, g, b, color;

    editor.resetViewport();

    rectfill(bmp, minimap_x, minimap_y, minimap_x+editor.mapWidth/aux_resize, minimap_y+editor.mapHeight/aux_resize, makecol(0, 0, 0));

    for (l = 0; l < editor.layers; l++) {
        for (i = 0; i < editor.mapWidth; i++) {
            for (j = 0; j < editor.mapHeight; j++) {
                val = editor.Map[l][j][i].index;
                ts = editor.Map[l][j][i].tileset;

                x = val / TILESIZE;
                y = val % TILESIZE;
                if ((color = getpixel(mini, ts*8+x, y)) != makecol(255, 0, 255)) {
                    r = getr(color);
                    g = getg(color);
                    b = getb(color);
                    putpixel(bmp, minimap_x+i/aux_resize, minimap_y+j/aux_resize, makecol(r, g, b));
                }

            }
        }
    }
    rect(bmp, minimap_x, minimap_y, minimap_x + editor.mapWidth/aux_resize,
         minimap_y + editor.mapHeight/aux_resize, makecol(128,128,128));
    rect(bmp, minimap_x + editor.viewport.scroll_x/aux_resize, minimap_y + editor.viewport.scroll_y/aux_resize,
         minimap_x + editor.viewport.scroll_x/aux_resize + editor.viewport.tile_w/aux_resize,
         minimap_y + editor.viewport.scroll_y/aux_resize + editor.viewport.tile_h/aux_resize, makecol(0,0,255));

}


void MinimapMain::moveMiniMap() {

    editor.resetViewport();

    if (gui.getMouseFrame() == MINI_FRAME) {
        if (mouse_b & 1) {
            mouse.setCursor(NONE);
            editor.viewport.scroll_x = (mouse_x - minimap_x - editor.viewport.tile_w/2)*aux_resize;
            editor.viewport.scroll_y = (mouse_y - minimap_y - editor.viewport.tile_h/2)*aux_resize;

            if (editor.viewport.scroll_x < 0) editor.viewport.scroll_x = 1;
            if (editor.viewport.scroll_x > (editor.mapWidth - editor.viewport.tile_w))
                editor.viewport.scroll_x = editor.mapWidth - editor.viewport.tile_w;

            if (editor.viewport.scroll_y < 1) editor.viewport.scroll_y = 1;
            if (editor.viewport.scroll_y > (editor.mapHeight - editor.viewport.tile_h))
                editor.viewport.scroll_y = editor.mapHeight - editor.viewport.tile_h;
        }
    }
}

void MinimapMain::freeMinimap() {
    destroy_bitmap(mini);
    destroy_bitmap(minimap);
}
