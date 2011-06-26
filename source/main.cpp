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

#include "gui\guimain.h"
#include "editor\editormain.h"
#include "editor\tilesetmain.h"
#include "editor\minimapmain.h"
#include "utils\dataformat.h"

//BUG: Editor returns -1 if it doesn't find the map file, check the loadMap() function

BITMAP *buffer;

GuiResources resources;
InputMouse mouse;
GuiMain gui;
EditorMain editor;
TilesetMain tileset;
MinimapMain minimap;
DataFormat convert;

volatile int allmap_exit = FALSE;

void close_button_handler(void) {
    allmap_exit = TRUE;

}
END_OF_FUNCTION(close_button_handler)

int main(void) {

    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();

    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 1024, 768,0,0)!=0)
        return 1;

    set_window_title("AllMap Editor v 0.3");

    LOCK_FUNCTION(close_button_handler);
    set_close_button_callback(close_button_handler);

    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    clear_to_color(buffer, makecol(255, 255, 255));

    resources.loadResources();
    mouse.initMouse();

    gui.loadInterface();

    editor.initEditor();
    editor.setViewport(0, 32+4, 24, 20);

    minimap.initMinimap();
    minimap.updateMiniMapCoords();

    while (!allmap_exit) {
        show_mouse(NULL);
        gui.updateInterface();
        allmap_exit = gui.getQuit();

        tileset.updateTilesetActions();
        minimap.moveMiniMap();

        editor.editorEngine();


        if (gui.getGrid()) editor.drawGrid();

        if (gui.getOtherGrids()) {
            editor.drawCollision();
            editor.drawEmitterGrid();
        }

        editor.renderMap(buffer);
        tileset.drawTileset(buffer, 768, 32, 256, TILESIZE*19+TILESIZE/2);

        gui.drawInterface(buffer);

        //show_mouse(buffer);
        mouse.draw(buffer);
        acquire_screen();
        masked_blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        release_screen();

        clear_to_color(buffer, makecol(255, 255, 255));
    }

    //show_mouse(NULL);
    mouse.freeMouse();
    resources.freeResources();
    minimap.freeMinimap();
    editor.freeEditor();
    destroy_bitmap(buffer);
    return 0;
}
END_OF_MAIN()
