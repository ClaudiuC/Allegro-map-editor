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
*** \file    editormain.cpp
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Source file for the editor engine.
******************************************************************************/

#include "editormain.h"

extern GuiMain gui;
extern MinimapMain minimap;
extern GuiResources resources;
extern DataFormat convert;
extern InputMouse mouse;
extern TilesetMain tileset;

// TODO: Fix the bug that appears when selecting collision after defining an object
// TODO: ^The object bug seems to appear when selecting any other brush. Add a clearObject() method.

EditorMain::EditorMain() {

    // This stores the brush size
    // Used only to display the correct selector between Canvas and Tileset
    exMouseZ = 0;

    brush_size = 0;

    // A flag to tell us wheter we can keep the current size of the brush or we have to
    // restore it using exMouseZ
    restoreBrush = false;

    // Used to draw a portion of the tileset, rather than just a tile
    // These define the image boundaries (upper - lower corners) and the tileset to crop from
    current_object_x1 = current_object_x2 = current_object_y1 = current_object_y2 = object_tileset = 0;
    // In case we're currently selecting an object
    isObject = false;

    // Should be used to set a "busy" flag
    // while writing or reading data to/from disk/memory
    dataAccessState = ACCESS_FREE;


    emitter_state = PAUSE_PARTICLES;

} // EditorMain::EditorMain()

EditorMain::~EditorMain() {
    // Just empty like that
    freeMap();
} // EditorMain::~EditorMain()

// Editor initializer
void EditorMain::initEditor() {

    // Load start-up data from the config file
    // This is subject to change, I should at least re-write
    // the quit code to write the latest data to editor.ini
    // TODO: EditorMain::initEditor() Proper use of config data
    // TODO: EditorMain::initEditor() Should use a config wrapper class
    push_config_state();
    set_config_file("editor.ini");

    layers = get_config_int("mapdata", "layers", 1);
    mapWidth = get_config_int("mapdata", "width", 1);
    mapHeight = get_config_int("mapdata", "height", 1);

    // First Map initialization
    // Allocate memory for the first dimension of the matrix
    Map = new Tile**[layers];
    for (short i = 0; i < layers; i++) {
        // Allocate memory for the columns
        Map[i]= new Tile*[mapHeight];
    }

    for (short i = 0; i < layers; i++) {
        for (short j = 0; j < mapHeight; j++) {
            // And rows
            Map[i][j]= new Tile[mapWidth];
        }
    }

    // Load the map from disk (set the access flag to read-only as well)
    dataAccessState = ACCESS_READ_ONLY;
    loadMap();
    //saveMap("test_map.dat");

    // Create the map buffer
    map = create_bitmap(768, TILESIZE*19+TILESIZE/2);
    // Drop some paint
    clear_to_color(map, makecol(255, 0, 255));

    // Load the map datafile
    mapData = load_datafile("Data\\Map\\mapData.dat");
    // Set the transparency blender
    set_trans_blender(128, 128, 128, 128);

    transTile = create_bitmap(TILESIZE, TILESIZE);
    clear_to_color(transTile, makecol(128,128,128));

    // Well, now we can go nuts with it, we got access
    dataAccessState = ACCESS_FREE;

} // void EditorMain::initEditor()

// This should reinitialize the weird ***Map when loading a different-sized map
// from the disk
void EditorMain::restartEditor(int lays, int width, int height) {

    // Don't allow other functions to run while we have the flag set
    // Isn't there a method to auto-magically lock the other methods
    // while we have dataAccessState != ACCESS_FREE?
    dataAccessState = ACCESS_WRITE_ONLY;
    //cout << "bleah";
    // Delete the Map and unload any other data
    if (Map) {
        freeMap();

        // Set the new map variables accordingly
        /*
        layers = lays;
        mapWidth = width;
        mapHeight = height;
        */

        // Reset the viewport in case the map is smaller than
        // the viewport size
        resetViewport();

        // See the initEditor() comments for questions
        Map = new Tile**[layers];
        for (short i = 0; i < layers; i++) {
            Map[i]= new Tile*[mapHeight];
        }

        for (short i = 0; i < layers; i++) {
            for (short j = 0; j < mapHeight; j++) {
                Map[i][j]= new Tile[mapWidth];
            }
        }

        // TODO: EditorMain::restartEditor() Handle dynamic resolution
        // Recreate the bitmaps and datafiles
        /*
        map = create_bitmap(768, TILESIZE*19);
        clear_to_color(map, makecol(255, 0, 255));

        mapData = load_datafile("Data\\Map\\mapData.dat");
        set_trans_blender(128, 128, 128, 128);
        */
    }


    // Yeah, yeah, do whatever you want now
    dataAccessState = ACCESS_FREE;
} // void EditorMain::restartEditor(int lays, int width, int height)

// DEPRECATED: Creates a new map named test_map.dat
// having the same size and layer number as the map
// currently in use. Fills the first layer with grass
// and the second with "magic pink" tiles (given the
// default tileset
// TODO: EditorMain Remove deprecated functions
short EditorMain::newMap() {


    // Set the access flag
    dataAccessState = ACCESS_WRITE_ONLY;

    PACKFILE *pfile;
    pfile = pack_fopen("Data\\Map\\test_map.dat", "wp");

    int i, j, l;

    // Write map sizes
    pack_iputl(layers, pfile);
    pack_iputl(mapWidth, pfile);
    pack_iputl(mapHeight, pfile);

    // Fill the first layer with the first tile of the first tileset
    for (i = 0; i < mapWidth; i++) {
        for (j = 0; j < mapHeight; j++) {
            pack_iputl(0, pfile);
            pack_iputl(0, pfile);
            pack_iputl(-1, pfile);
        }
    }

    // Fill the other layers with the first tile on the second row from
    // the first tileset
    for (l = 1; l < layers; l++) {
        for (i = 0; i < mapWidth; i++) {
            for (j = 0; j < mapHeight; j++) {
                pack_iputl(1, pfile);
                pack_iputl(0, pfile);
                pack_iputl(0, pfile);

                pack_iputl(-1, pfile);
            }
        }
    }

    // Close the map file, set the current_map name and release access
    pack_fclose(pfile);
    current_map = "test_map.dat";
    dataAccessState = ACCESS_FREE;

    return 0;

} // short EditorMain::newMap()

// DEPRECATED: Creates a new custom-name map using
// the current map's attributes. Works preety much
// as the map above
// SEE: Check the comments for the newMap() function
short EditorMain::newMap(string name) {


    dataAccessState = ACCESS_WRITE_ONLY;

    PACKFILE *pfile;
    string path = "Data\\Map\\";
    // I should check what extension the current
    // filename has and only add/replace it with
    // .dat if that's necessary, but I'm lazy
    // string ext =".dat";
    // TODO: EditorMain Proper filename handling
    string final = path+name;

    pfile = pack_fopen(final.c_str(), "wp");

    if (pfile == NULL) {
        return -1;
    }

    pack_iputl(layers, pfile);
    pack_iputl(mapWidth, pfile);
    pack_iputl(mapHeight, pfile);

    int i, j, l;

    for (i = 0; i < mapWidth; i++) {
        for (j = 0; j < mapHeight; j++) {
            pack_iputl(0, pfile);
            pack_iputl(0, pfile);
            pack_iputl(0, pfile);

            pack_iputl(-1, pfile);
        }
    }

    for (l = 1; l < layers; l++) {
        for (i = 0; i < mapWidth; i++) {
            for (j = 0; j < mapHeight; j++) {
                pack_iputl(1, pfile);
                pack_iputl(0, pfile);
                pack_iputl(0, pfile);

                pack_iputl(-1, pfile);
            }
        }
    }
    pack_fclose(pfile);
    current_map = name;
    dataAccessState = ACCESS_FREE;

    return 0;

} // short EditorMain::newMap(string name)

// Create a new map beeing given custom attributes. Returns 0 on success, -1
// otherwise.
// NOTE: It receives strings as parameters as that's what I get from the input
// boxes
// TODO: Create the overloaded variants
short EditorMain::newMap(string name, string lay, string width, string height) {

    dataAccessState = ACCESS_WRITE_ONLY;
    short returnValue = -1;
    int lays, w, h;

    // stoi basically applies c_str() on the string and converts it using atoi()
    // Nothing fancy but it works for me (a new map should have a size and number
    // of layers
    lays = convert.stoi(lay);
    w = convert.stoi(width);
    h = convert.stoi(height);

    // Check if the parameters are correct
    if (lays == 0 || lays > 10) return returnValue;
    if (w == 0) return returnValue;
    if (h == 0) return returnValue;

    // Simply follow the same algorithms as in the methods above
    PACKFILE *pfile;
    string path = "Data\\Map\\";
    // string ext =".dat";
    string final = path+name;

    pfile = pack_fopen(final.c_str(), "wp");

    if (pfile == NULL) {
        return -1;
    }

    layers = lays;
    mapWidth = w;
    mapHeight = h;

    // Should write map size to the packfile, duh
    pack_iputl(layers, pfile);
    pack_iputl(mapWidth, pfile);
    pack_iputl(mapHeight, pfile);

    for (short l = 0; l < layers; l++) {
        for (short i = 0; i < mapWidth; i++) {
            for (short j = 0; j < mapHeight; j++) {
                pack_iputl(0, pfile);
                pack_iputl(0, pfile);
                pack_iputl(0, pfile);

                pack_iputl(-1, pfile);
            }
        }
    }
    pack_fclose(pfile);
    current_map = name;
    dataAccessState = ACCESS_FREE;
    return 0;

} // short EditorMain::newMap(string name, string lay, string width, string height)

// DEPRECATED: loads the default map
short EditorMain::loadMap() {

    dataAccessState = ACCESS_WRITE_ONLY;

    PACKFILE *pfile;
    pfile = pack_fopen("Data\\Map\\test_map.dat", "rp");

    // If we couldn't open the file, try and create it
    // Should be verified better, what if the second attempt
    // to open the file fails as well?
    if (pfile == NULL) {
        newMap();
        //pfile = pack_fopen("Data\\Map\\test_map.dat", "rp");
        loadMap();
    }
    //cout << "done";
    short load_layers = pack_igetl(pfile);
    short load_mapWidth = pack_igetl(pfile);
    short load_mapHeight = pack_igetl(pfile);
    /*
    if (load_layers != layers || load_mapHeight != mapHeight || load_mapWidth != mapWidth) {
        layers = load_layers;
        mapWidth = load_mapWidth;
        mapHeight = load_mapHeight;

        restartEditor(layers, mapWidth, mapHeight);
        resetViewport();
    }
    */
    layers = load_layers;
    mapWidth = load_mapWidth;
    mapHeight = load_mapHeight;

    restartEditor(layers, mapWidth, mapHeight);
    resetViewport();

    //cout << "here";
    int i, j, l;
    for (l = 0; l < layers; l++) {
        for (i = 0; i < mapWidth; i++) {
            for (j = 0; j < mapHeight; j++) {
                //cout << j <<endl;
                Map[l][j][i].index = pack_igetl(pfile);
                Map[l][j][i].tileset = pack_igetl(pfile);
                Map[l][j][i].collision = pack_igetl(pfile);

                Map[l][j][i].emitter = pack_igetl(pfile);
            }
        }
    }
    pack_fclose(pfile);
    current_map = "test_map.dat";
    dataAccessState = ACCESS_FREE;
    return 0;

} // short EditorMain::loadMap()


// Loads any map found as "name"
short EditorMain::loadMap(string name) {

    dataAccessState = ACCESS_READ_ONLY;

    PACKFILE *pfile;
    string path = "Data\\Map\\";
    string final = path+name;

    pfile = pack_fopen(final.c_str(), "rp");

    if (pfile == NULL) {
        return -1;
    }

    short load_layers = pack_igetl(pfile);
    short load_mapWidth = pack_igetl(pfile);
    short load_mapHeight = pack_igetl(pfile);
    /*
    if (load_layers != layers || load_mapHeight != mapHeight || load_mapWidth != mapWidth) {
        layers = load_layers;
        mapWidth = load_mapWidth;
        mapHeight = load_mapHeight;

        restartEditor(layers, mapWidth, mapHeight);
        resetViewport();
    }
    */
    layers = load_layers;
    mapWidth = load_mapWidth;
    mapHeight = load_mapHeight;

    restartEditor(layers, mapWidth, mapHeight);
    resetViewport();

    int i, j, l;
    for (l = 0; l < layers; l++) {
        for (i = 0; i < mapWidth; i++) {
            for (j = 0; j < mapHeight; j++) {
                cout << j <<endl;
                Map[l][j][i].index = pack_igetl(pfile);
                Map[l][j][i].tileset = pack_igetl(pfile);
                Map[l][j][i].collision = pack_igetl(pfile);

                Map[l][j][i].emitter = pack_igetl(pfile);
            }
        }
    }

    pack_fclose(pfile);
    current_map = name;
    dataAccessState = ACCESS_FREE;

    return 0;

} // short EditorMain::loadMap(string name)

// TODO: Why the hell do I have a loadMap(name, layers, mapWidth, mapHeight) method?!
short EditorMain::loadMap(string name, string lay, string w, string h) {

    int lays = convert.stoi(lay);
    int width = convert.stoi(w);
    int height = convert.stoi(h);

    if (lays == 0) return -1;
    if (width == 0) return -1;
    if (height == 0) return -1;

    PACKFILE *pfile;
    string path = "Data\\Map\\";
    string final = path+name;

    pfile = pack_fopen(final.c_str(), "rp");

    if (pfile == NULL) {
        return -1;
    }

    restartEditor(lays, width, height);
    dataAccessState = ACCESS_WRITE_ONLY;

    for (short l = 0; l < layers; l++) {
        for (short i = 0; i < mapWidth; i++) {
            for (short j = 0; j < mapHeight; j++) {
                Map[l][j][i].index = pack_igetl(pfile);
                Map[l][j][i].tileset = pack_igetl(pfile);
                Map[l][j][i].collision = pack_igetl(pfile);

                Map[l][j][i].emitter = pack_igetl(pfile);
            }
        }
    }
    pack_fclose(pfile);
    current_map = name;

    //cout << "loaded like a motha fucka'";
    dataAccessState = ACCESS_FREE;

    return 0;

} // short EditorMain::loadMap(string name, string lay, string w, string h)


// DEPRECATED: Saves a map as the current default map
// TODO: EditorMain Try to work-around these stupid methods
//       so you can eliminate them from the editor code
/*
short EditorMain::saveMap() {

    PACKFILE *pfile;
    pfile = pack_fopen("Data\\Map\\test_map.dat", "wp");

    if (pfile == NULL) {
        return -1;
    }

    pack_iputl(layers, pfile);
    pack_iputl(mapWidth, pfile);
    pack_iputl(mapHeight, pfile);

    int i, j, l;

    for (l = 0; l < layers; l++) {
        for (i = 0; i < mapWidth; i++) {
            for (j = 0; j < mapHeight; j++) {
                pack_iputl(Map[l][j][i].index, pfile);
                pack_iputl(Map[l][j][i].tileset, pfile);
                pack_iputl(Map[l][j][i].collision, pfile);
            }
        }
    }
    pack_fclose(pfile);
    current_map = "test_map.dat";
    return 0;

} // short EditorMain::saveMap()
*/

// Save the current map under a given name
// TODO: The current_map name allocation in the IO methods fails under conditions of usability.
//       Eg: I work on the abc.dat map, I save it as cba.dat but continue working on the abc version.
//       The current_map will now become cba.dat, altough I'm not working on it.
short EditorMain::saveMap(string name) {

    PACKFILE *pfile;
    string path = "Data\\Map\\";
    string final = path+name;
    pfile = pack_fopen(final.c_str(), "wp");

    if (pfile == NULL) {
        return -1;
    }

    pack_iputl(layers, pfile);
    pack_iputl(mapWidth, pfile);
    pack_iputl(mapHeight, pfile);

    int i, j, l;

    for (l = 0; l < layers; l++) {
        for (i = 0; i < mapWidth; i++) {
            for (j = 0; j < mapHeight; j++) {
                pack_iputl(Map[l][j][i].index, pfile);
                pack_iputl(Map[l][j][i].tileset, pfile);
                pack_iputl(Map[l][j][i].collision, pfile);

                pack_iputl(Map[l][j][i].emitter, pfile);
            }
        }
    }
    pack_fclose(pfile);
    current_map = current_map;
    return 0;

} //short EditorMain::saveMap(string name)

// Sets the viewport using the passed parameters
void EditorMain::setViewport(int px, int py, int w, int h) {

    viewport.tile_x = 0;
    viewport.tile_y = 0;
    viewport.tile_w = viewport.init_w = w;
    viewport.tile_h = viewport.init_h = h;

    viewport.pos_x = px;
    viewport.pos_y = py;

    viewport.scroll_x = viewport.scroll_y = 0;
} // void EditorMain::setViewport(int px, int py, int w, int h)

// Check wheter the viewport is too large for the current display options
// and chenge it accordingly.
// TODO: EditorMain::resetViewport() Add SCREEN_H and SCREEN_W for comparison
void EditorMain::resetViewport() {
    if (viewport.tile_w > mapWidth) viewport.tile_w = mapWidth-1;
    else if (viewport.tile_w != viewport.init_w) viewport.tile_w = viewport.init_w;
    if (viewport.tile_h > mapHeight) viewport.tile_h = mapHeight-1;
    else if (viewport.tile_h != viewport.init_h) viewport.tile_h = viewport.init_h;
} // void EditorMain::resetViewport()


// Draw an overlay gray grid
void EditorMain::drawGrid() {

    int grid_x, grid_y, grid_x1, grid_y1;
    for (int i = viewport.tile_x; i < viewport.tile_w; i++) {
        for (int j = viewport.tile_y; j < viewport.tile_h+2; j++) {

            grid_x = i*TILESIZE + viewport.pos_x;
            grid_x1 = grid_x+TILESIZE-1;
            grid_y = j*TILESIZE + viewport.pos_y;
            grid_y1 = grid_y+TILESIZE-1;

            rect(map, grid_x, grid_y, grid_x1, grid_y1, makecol(128, 128, 128));
        }
    }
} // void EditorMain::drawGrid()

// Draw the collision mask
// Works very much like the drawGrid() method just that it also checks wheter a tile has
// a collision flag or not.
void EditorMain::drawCollision() {

    int grid_x, grid_y, grid_x1, grid_y1;
    int collision;

    for (int i = viewport.tile_x; i < viewport.tile_w; i++) {
        for (int j = viewport.tile_y; j < viewport.tile_h; j++) {
            collision = Map[gui.getCurrentLayer()][j+viewport.scroll_y][i+viewport.scroll_x].collision;


            grid_x = i*TILESIZE + viewport.pos_x;
            grid_x1 = grid_x+TILESIZE-1;
            grid_y = j*TILESIZE + viewport.pos_y;
            grid_y1 = grid_y+TILESIZE-1;

            if (collision > 0)
                rect(map, grid_x, grid_y, grid_x1, grid_y1, makecol(255, 0, 0));
        }
    }
} // void EditorMain::drawCollision()

void EditorMain::drawEmitterGrid() {

    int grid_x, grid_y, grid_x1, grid_y1;
    int collision;

    for (int i = viewport.tile_x; i < viewport.tile_w; i++) {
        for (int j = viewport.tile_y; j < viewport.tile_h; j++) {
            collision = Map[gui.getCurrentLayer()][j+viewport.scroll_y][i+viewport.scroll_x].emitter;


            grid_x = i*TILESIZE + viewport.pos_x;
            grid_x1 = grid_x+TILESIZE-1;
            grid_y = j*TILESIZE + viewport.pos_y;
            grid_y1 = grid_y+TILESIZE-1;

            if (collision > 0)
                rect(map, grid_x, grid_y, grid_x1, grid_y1, makecol(0, 0, 255));
        }
    }
} // void EditorMain::drawCollision()

// Draws a selector according to the action the user is currently doing
void EditorMain::drawSelector(BITMAP *bmp, int x1, int y1, int x2, int y2, short type) {
    int size_alter;
    int tmp_col;

    // In preview mode the cursor should be set to a magnifier no matter what
    if (gui.getPreview()) {
        //if (gui.getMouseFrame() != MAIN_FRAME || gui.getMouseFrame() != TSET_FRAME) {
        mouse.setCursor(PREVIEW);
        //}
    } else {
        // Make brush selector blue
        if (type != BRUSH_COLLISION) tmp_col = makecol(0,0,255);
        // And collision selector red
        else tmp_col = makecol(255,0,0);

        // ********* (1) Simple one-tiler, collision or erase mask

        // If we're not in the process of drawing/selecting an object then check if the mouse pointer hovers over the
        // MAIN_FRAME, if it does then restore the brush, if necessary.
        if (!isObject) {
            if (gui.getMouseFrame() == MAIN_FRAME) {
                if (restoreBrush == true) {
                    position_mouse_z(exMouseZ);
                    restoreBrush = false;
                }
            } else {
                if (gui.getMouseFrame() != TSET_FRAME) {
                    mouse.setCursor(CURSOR);
                }
            }

            // If we're drawing or marking collisions
            if (type != BRUSH_ERASE) {
                switch (type) {
                case BRUSH_DRAW: {
                    mouse.setCursor(PAINTBRUSH);
                    break;
                }
                case BRUSH_FLOOD: {
                    position_mouse_z(0);
                    mouse.setCursor(FLOODFILL);
                    break;
                }
                case BRUSH_COLLISION: {
                    mouse.setCursor(COLLISION);
                    break;
                }
                case BRUSH_EMITTER: {
                    mouse.setCursor(WAND);
                    break;
                }
                }
                // If the brush isn't enlarged, draw a 2px bordered rectangle holding the selected tile at the mouse position
                if (mouse_z < 2) {

                    if (type == BRUSH_DRAW || type == BRUSH_FLOOD) {
                        masked_blit((BITMAP*)mapData[TILES1+mouse_tileset].dat, map, TILESIZE*(current_tile/TILESIZE), TILESIZE*(current_tile%TILESIZE),
                                    TILESIZE*(mouse_x/TILESIZE)+viewport.pos_x,TILESIZE*(mouse_y/TILESIZE)+viewport.pos_y-32,TILESIZE,TILESIZE);
                    }
                    rect(bmp, x1, y1, x2, y2, tmp_col);
                    rect(bmp, x1-1, y1+1, x2+1, y2-1, tmp_col);

                    if (type == BRUSH_COLLISION || type == BRUSH_EMITTER) {
                        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                        rectfill(bmp, x1, y1, x2, y2, tmp_col);
                        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
                    }
                    // TODO: GUI tooltip class
                    x2 = mouse_x + 20;
                    y1 = mouse_y;

                    rectfill(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999"), y1 + 5, makecol(255, 255, 128));
                    rect(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999"), y1 + 5, makecol(255, 200, 128));

                    short tooltip_x, tooltip_y;

                    if (gui.getMouseFrame() == MAIN_FRAME) {
                        tooltip_x = gui.gui_x/32+viewport.scroll_x;
                        tooltip_y = gui.gui_y/32+viewport.scroll_y;
                    } else {
                        tooltip_x = gui.gui_x/32;
                        tooltip_y = gui.gui_y/32 + tileset.scroll_y/32;
                    }

                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font)*2, tmp_col, -1, "X:%d", tooltip_x);
                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font), tmp_col, -1, "Y:%d", tooltip_y);

                    // Otherwise, fill the brush area with images of the same tile and draw the 2px bordered rectangle, keeping mouse position
                    // in the middle of the brush
                } else if ((mouse_z > 1)) {

                    if (mouse_z % 2 != 0) size_alter = mouse_z-1;
                    else size_alter = mouse_z;

                    if (type == BRUSH_DRAW) {
                        for (short tmpx = 0; tmpx < size_alter+1; tmpx++) {
                            for (short tmpy = 0; tmpy < size_alter+1; tmpy++) {
                                masked_blit((BITMAP*)mapData[TILES1+mouse_tileset].dat, map, TILESIZE*(current_tile/TILESIZE), TILESIZE*(current_tile%TILESIZE),
                                            x1+1+viewport.pos_x - (size_alter*32)/2 + tmpx*32, y1-(size_alter*32)/2 + tmpy*32,TILESIZE,TILESIZE);
                            }
                        }
                    }
                    rect(bmp, x1 - (size_alter*32)/2, y1 - (size_alter*32)/2, x2 + (size_alter*32)/2, y2 + (size_alter*32)/2, tmp_col);
                    rect(bmp, x1 - (size_alter*32)/2 - 1, y1 - (size_alter*32)/2 + 1, x2 + (size_alter*32)/2 + 1, y2 + (size_alter*32)/2 - 1, tmp_col);

                    if (type == BRUSH_COLLISION || type == BRUSH_EMITTER) {
                        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                        rectfill(bmp,  x1 - (size_alter*32)/2, y1 - (size_alter*32)/2, x2 + (size_alter*32)/2, y2 + (size_alter*32)/2, tmp_col);
                        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
                    }

                    x2 = mouse_x + 20;
                    y1 = mouse_y;

                    short tooltip_x, tooltip_y, tooltip_x2, tooltip_y2;
                    tooltip_x = tooltip_y = tooltip_x2 = tooltip_y2 = 0;

                    if (gui.getMouseFrame() == MAIN_FRAME) {
                        tooltip_x = gui.gui_x/32+viewport.scroll_x-(size_alter)/2;
                        tooltip_x2 = gui.gui_x/32+viewport.scroll_x+(size_alter)/2;

                        tooltip_y = gui.gui_y/32+viewport.scroll_y-(size_alter)/2;
                        tooltip_y2 = gui.gui_y/32+viewport.scroll_y+(size_alter)/2;
                    }

                    rectfill(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999, 999"), y1 + 5, makecol(255, 255, 128));
                    rect(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999, 999"), y1 + 5, makecol(255, 200, 128));

                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font)*2, tmp_col, -1, "X:%d, %d", tooltip_x, tooltip_x2);
                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font), tmp_col, -1, "Y:%d, %d", tooltip_y, tooltip_y2);
                }

                // Follow the exactly same procedures in case we want to erase a tile, the only difference beeing that we should
                // draw a semi-transparent rectangle, instead of the current tile
            } else if (type == BRUSH_ERASE) {
                if (mouse_z < 2) {
                    for (int i = 0; i < 2; i++) {
                        for (int j = 0; j < 2; j++) {
                            draw_trans_sprite(map, (BITMAP*)resources.data[TRANS_BK].dat, i + x1-1, j + y1-1);
                        }
                    }
                    rect(bmp, x1, y1, x2, y2, tmp_col);
                    rect(bmp, x1-1, y1+1, x2+1, y2-1, tmp_col);

                    x2 = mouse_x + 20;
                    y1 = mouse_y;

                    rectfill(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999"), y1 + 5, makecol(255, 255, 128));
                    rect(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999"), y1 + 5, makecol(255, 200, 128));

                    short tooltip_x, tooltip_y;

                    if (gui.getMouseFrame() == MAIN_FRAME) {
                        tooltip_x = gui.gui_x/32+viewport.scroll_x;
                        tooltip_y = gui.gui_y/32+viewport.scroll_y;
                    } else {
                        tooltip_x = gui.gui_x/32;
                        tooltip_y = gui.gui_y/32 + tileset.scroll_y/32;
                    }

                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font)*2, tmp_col, -1, "X:%d", tooltip_x);
                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font), tmp_col, -1, "Y:%d", tooltip_y);

                } else if (mouse_z > 1) {
                    if (mouse_z % 2 != 0) size_alter = mouse_z-1;
                    else size_alter = mouse_z;

                    for (int i = 0; i <= size_alter; i++) {
                        for (int j = 0; j <= size_alter; j++) {
                            //draw_trans_sprite(map, (BITMAP*)resources.data[TRANS_BK].dat, i*32 + x1-1, j*32 + y1-1);
                            draw_trans_sprite(map, (BITMAP*)resources.data[TRANS_BK].dat,x1+1+viewport.pos_x - (size_alter*32)/2 + i*32, y1-(size_alter*32)/2 + j*32);
                        }
                    }
                    /*
                    rect(bmp, x1, y1, x2+size_alter*TILESIZE-TILESIZE, y2+size_alter*TILESIZE-TILESIZE, tmp_col);
                    rect(bmp, x1-1, y1+1, x2+1+size_alter*TILESIZE-TILESIZE, y2-1+size_alter*TILESIZE-TILESIZE, tmp_col);
                    */
                    rect(bmp, x1 - (size_alter*32)/2, y1 - (size_alter*32)/2, x2 + (size_alter*32)/2, y2 + (size_alter*32)/2, tmp_col);
                    rect(bmp, x1 - (size_alter*32)/2 - 1, y1 - (size_alter*32)/2 + 1, x2 + (size_alter*32)/2 + 1, y2 + (size_alter*32)/2 - 1, tmp_col);

                    short tooltip_x, tooltip_y, tooltip_x2, tooltip_y2;
                    tooltip_x = tooltip_y = tooltip_x2 = tooltip_y2 = 0;

                    if (gui.getMouseFrame() == MAIN_FRAME) {
                        tooltip_x = gui.gui_x/32+viewport.scroll_x-(size_alter)/2;
                        tooltip_x2 = gui.gui_x/32+viewport.scroll_x+(size_alter)/2;

                        tooltip_y = gui.gui_y/32+viewport.scroll_y-(size_alter)/2;
                        tooltip_y2 = gui.gui_y/32+viewport.scroll_y+(size_alter)/2;
                    }

                    x2 = mouse_x + 20;
                    y1 = mouse_y;

                    rectfill(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999, 999"), y1 + 5, makecol(255, 255, 128));
                    rect(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999, 999"), y1 + 5, makecol(255, 200, 128));

                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font)*2, tmp_col, -1, "X:%d, %d", tooltip_x, tooltip_x2);
                    textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font), tmp_col, -1, "Y:%d, %d", tooltip_y, tooltip_y2);
                }

            }
            // ********* (2) Object selector
        } else {

            // Check the corners input, and normalize the coordinates so that (x1)(y1) stand for the upper-left corner
            // and the (x2)(y2) coordinates stand for the bottom-right corner
            if (current_object_x1 > current_object_x2) {
                short aux = current_object_x1;
                current_object_x1 = current_object_x2;
                current_object_x2 = aux;
            }
            if (current_object_y1 > current_object_y2) {
                short aux = current_object_y1;
                current_object_y1 = current_object_y2;
                current_object_y2 = aux;
            }

            mouse.setCursor(GROUP);
            // Blit the selected object to the screen, surrounded by a 2px bordered rectangle
            // TODO: Draw the object selector correctly
            for (short i = 0; i <= current_object_x2-current_object_x1; i++) {
                for (short j = 0; j <= current_object_y2-current_object_y1; j++) {
                    masked_blit((BITMAP*)mapData[TILES1+object_tileset].dat, map, TILESIZE*i+current_object_x1*32, TILESIZE*j+current_object_y1*32,
                                x1+i*32,y1+j*32,TILESIZE,TILESIZE);
                }
            }
            short delta_x = (current_object_x2-current_object_x1)*TILESIZE;
            short delta_y = (current_object_y2-current_object_y1)*TILESIZE;

            rect(bmp, x1, y1, x2+delta_x, y2+delta_y, tmp_col);
            rect(bmp, x1+1, y1+1, x2+delta_x-1, y2+delta_y-1, tmp_col);

            short tooltip_x, tooltip_y, tooltip_x2, tooltip_y2;
            tooltip_x = tooltip_y = tooltip_x2 = tooltip_y2 = 0;

            if (gui.getMouseFrame() == MAIN_FRAME) {
                tooltip_x = gui.gui_x/32+viewport.scroll_x;
                tooltip_x2 = gui.gui_x/32+viewport.scroll_x+delta_x/32;

                tooltip_y = gui.gui_y/32+viewport.scroll_y;
                tooltip_y2 = gui.gui_y/32+viewport.scroll_y+delta_y/32;
            }

            x2 = mouse_x + 20;
            y1 = mouse_y;

            rectfill(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999, 999"), y1 + 5, makecol(255, 255, 128));
            rect(bmp, x2 + 5, y1 - text_height(font)*2-5, x2 + 10 + text_length(font, "X: 999, 999"), y1 + 5, makecol(255, 200, 128));

            textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font)*2, tmp_col, -1, "X:%d, %d", tooltip_x, tooltip_x2);
            textprintf_ex(bmp, font, x2+text_length(font, "X:00")/2, y1-text_height(font), tmp_col, -1, "Y:%d, %d", tooltip_y, tooltip_y2);

        }
    }
} // void EditorMain::drawSelector(BITMAP *bmp, int x1, int y1, int x2, int y2, short type)

// Assign the proper index and tileset values on the Map array
void EditorMain::drawTile(int x1, int y1) {
    /*
     if (mouse_z % 2 != 0) brush_size = mouse_z -1;
        else brush_size = mouse_z;
    */
    if (((((y1/TILESIZE+viewport.scroll_y-1) > 0)) && (x1/TILESIZE+viewport.scroll_x) > -1) &&
            (((x1/TILESIZE+viewport.scroll_x) <= mapWidth) && ((y1/TILESIZE+viewport.scroll_y) <= mapHeight))) {
        Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].index = current_tile;
        Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].tileset = mouse_tileset;
    }
} // void EditorMain::drawTile(int x1, int y1)


// Assign the proper index and tileset to each tile necessary to draw the object
void EditorMain::drawObject(int x1, int y1) {
    for (short i = 0; i <= current_object_x2-current_object_x1; i++) {
        for (short j = 0; j <= current_object_y2-current_object_y1; j++) {
            if ((y1/TILESIZE+viewport.scroll_y-2+j) > -1 && (y1/TILESIZE+viewport.scroll_y-2+j) < mapHeight &&
                    (x1/TILESIZE+viewport.scroll_x+i) > -1 && (x1/TILESIZE+viewport.scroll_x+i) < mapWidth) {
                Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2+j][x1/TILESIZE+viewport.scroll_x+i].index = (i+current_object_x1)*TILESIZE+j+current_object_y1;
                Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2+j][x1/TILESIZE+viewport.scroll_x+i].tileset = object_tileset;
            }
        }
    }

} // void EditorMain::drawObject(int x1, int y1)

// Replace all similar tiles on map
void EditorMain::floodFill(int x1, int y1) {

    short flooded_tile = Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].index;
    short flooded_tset = Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].tileset;

    for (short i = 0; i < mapWidth; i++) {
        for (short j = 0; j < mapHeight; j++) {
            if (Map[gui.getCurrentLayer()][j][i].index == flooded_tile &&
                    Map[gui.getCurrentLayer()][j][i].tileset == flooded_tset) {
                Map[gui.getCurrentLayer()][j][i].index = current_tile;
                Map[gui.getCurrentLayer()][j][i].tileset = mouse_tileset;
            }
        }
    }
} // void EditorMain::floodFill(int x1, int y1)


void drawCustomParticle(BITMAP *bmp, PARTICLE p) {

    circlefill(bmp, (int)p.x, (int)p.y, 3, p.color);
}

void createCustomType(PARTICLE *p) {

        int rcolor = rand()%1;
        int color = makecol(75+rcolor, 75+rcolor, 75+rcolor);

        short life = rand()%15+500;

        p->dx = 0;
        p->dy = -1;
        p->rx = 1;
        p->ry = 1;
        p->life = life;
        p->color = color;
        p->speed = 1;
}

void EditorMain::playEmitters() {
    for (short l=0; l<layers; l++) {
        for (short i = viewport.scroll_x; i < viewport.scroll_x+viewport.tile_w; i++) {
            for (short j = viewport.scroll_y; j < viewport.scroll_y+viewport.tile_h; j++) {
                if (Map[l][j][i].emitter > 0) {


                    double x = i*TILESIZE+TILESIZE/2;
                    double y = j*TILESIZE+TILESIZE;

                    //particleEmitter.createParticles(x-viewport.scroll_x*32, y-viewport.scroll_y*32, 0.2, -0.7, 2, 1, life, color, 1, Map[l][j][i].emitter);
                    particleEmitter.createParticles(x-viewport.scroll_x*32, y-viewport.scroll_y*32, Map[l][j][i].emitter, createCustomType);
                    particleEmitter.applyForce(13*32, 4*32, 100, 10, -0.7, 0.2);
                    //particleEmitter.applyForce(7*32, 2*32, 32, 10, 0, -0.5);
                    //particleEmitter.applyForce(5*32, 4*32, 200, 200, 0, -2);

                    particleEmitter.updateParticles();
                    particleEmitter.drawParticles(map, &drawCustomParticle);

                    textprintf_ex(map, font, (int)x-viewport.scroll_x*32-TILESIZE/2, (int)y-viewport.scroll_y*32+32+text_height(font), makecol(0, 0, 255), -1, "%d particles", particleEmitter.getParticleCount());
                }
            }
        }
    }
}

// The editor engine function
short EditorMain::editorEngine() {
    int lay ;
    int x1, x2, y1, y2;

    // ********* (1) Check editor state and draw the layer(s) accordingly

    // If the editor is in preview mode, draw every layer in order
    if (gui.getPreview()) {
        for (int i=0; i<layers;i++) {
            drawLayer(i);
        }
    } else {
        // If the editor is in the layered mode ("Show layers" checkbox)
        if (gui.getLayers()) {
            for (int i=0; i < gui.getCurrentLayer(); i++) {
                // If the editor is in the Alpha mode ("Enable alpha" checkbox)
                if (gui.getAlpha()) {
                    // Draw all the layers with transparency up to the current layer
                    drawTransLayer(i);
                    // else draw every layer, up to the current one, in normal mode
                } else drawLayer(i);
            }
            // No matter what, the current layer should be drawn without transparency
            drawLayer(gui.getCurrentLayer());
            // If the editor isn't in the layered mode, draw the current layer only
        } else drawLayer(gui.getCurrentLayer());
    }

    if (gui.getPanelState() == STATE_PARTICLES) {
        drawEmitterGrid();
    }

    if (emitter_state == PLAY_PARTICLES) {
        //playEmitters();
    }
    // ********* (2) Update editor actions in case we're drawing a one-tiler, collision or erasing

    // Only edit the map if we're not in the preview mode
    if (!gui.getPreview()) {
        // Check if the mouse hovers over the canvas
        if (gui.getMouseFrame() == MAIN_FRAME) {

            x1=TILESIZE*(mouse_x/TILESIZE)-1;
            y1=TILESIZE*(mouse_y/TILESIZE) + viewport.pos_y-31;
            x2=x1+TILESIZE-3;
            y2=y1+TILESIZE-3;

            x1 ++;
            y1 += 31;
            if (gui.getPanelState() != STATE_PARTICLES) {
                // Make sure we're not drawing an object
                if (!isObject) {
                    // If we clicked the canvas and we're not in collision mode
                    if ((mouse_b & 1) && gui.getMouseFrame() == MAIN_FRAME && gui.getBrush() != BRUSH_COLLISION) {
                        if (gui.getBrush() != BRUSH_FLOOD) {
                            // TODO: Create a special erase tile that's not tileset-dependent
                            if (gui.getBrush() == BRUSH_ERASE) {
                                current_tile = 1;
                                mouse_tileset = 0;
                            }

                            // For a simple tile just make a call to the drawTile() method
                            if (mouse_z < 2) {
                                drawTile(x1, y1);
                                // Otherwise plot the tiles as necessary
                            } else if (mouse_z > 1) {
                                // TODO: Make transformations globally
                                if (mouse_z % 2 != 0) brush_size = mouse_z -1;
                                else brush_size = mouse_z;

                                for (int i = 0; i < brush_size; i++) {
                                    for (int j = 0; j < brush_size; j++) {

                                        drawTile(x1 + (i*TILESIZE)/2+TILESIZE, y1+ (j*TILESIZE)/2+TILESIZE);
                                        drawTile(x1 - (i*TILESIZE)/2, y1- (j*TILESIZE)/2);

                                        drawTile(x1 - (i*TILESIZE)/2, y1+ (j*TILESIZE)/2+TILESIZE);
                                        drawTile(x1 + (i*TILESIZE)/2+TILESIZE, y1- (j*TILESIZE)/2);
                                    }
                                }
                            }
                        } else {
                            floodFill(x1, y1);
                        }
                    }

                    // If the collision flag is set and we click the canvas
                    // TODO: Different collision according to the layer (useful for passing under/on bridges etc.
                    if ((mouse_b & 1) && gui.getMouseFrame() == MAIN_FRAME && gui.getBrush() == BRUSH_COLLISION) {

                        gui.setCollision(CHECKBOX_CHECKED);

                        // If we have an enlarged brush
                        if (mouse_z > 1) {
                            if (mouse_z % 2 != 0) brush_size = mouse_z -1;
                            else brush_size = mouse_z;
                            // Plot the collision mask as necessary
                            for (lay = 0; lay < layers; lay++) {
                                for (int i = 0; i < brush_size; i++) {
                                    for (int j = 0; j < brush_size; j++) {
                                        Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].collision = 1;
                                        Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].collision = 1;
                                        Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].collision = 1;
                                        Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].collision = 1;
                                    }
                                }
                            }
                            // For one-tiler collisions
                        } else {
                            for (lay = 0; lay < layers; lay++) {
                                Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].collision = 1;
                            }
                        }
                    }

                    // A right click in collision mode means that we're making an area passable
                    // TODO: Add variable brush-size support
                    if ((mouse_b & 2) && gui.getMouseFrame() == MAIN_FRAME && gui.getBrush() == BRUSH_COLLISION) {
                        gui.setCollision(CHECKBOX_CHECKED);

                        // If we have an enlarged brush
                        if (mouse_z > 1) {
                            if (mouse_z % 2 != 0) brush_size = mouse_z -1;
                            else brush_size = mouse_z;
                            // Plot the collision mask as necessary
                            for (lay = 0; lay < layers; lay++) {
                                for (int i = 0; i < brush_size; i++) {
                                    for (int j = 0; j < brush_size; j++) {
                                        Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].collision = 0;
                                        Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].collision = 0;
                                        Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].collision = 0;
                                        Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].collision = 0;
                                    }
                                }
                            }
                            // For one-tiler collisions
                        } else {
                            for (lay = 0; lay < layers; lay++) {
                                Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].collision = 0;
                            }
                        }

                    }
                    // ********* (3) Drawing objects
                } else if ((mouse_b & 1) && gui.getMouseFrame() == MAIN_FRAME && gui.getBrush() != BRUSH_COLLISION) {
                    // Set the values to pass to the drawObject() method
                    x1=TILESIZE*(mouse_x/TILESIZE)-1;
                    y1=TILESIZE*(mouse_y/TILESIZE) + viewport.pos_y-31;
                    x2=x1+TILESIZE-3;
                    y2=y1+TILESIZE-3;

                    x1 ++;
                    y1 += 31;

                    drawObject(x1, y1);
                }

            } else {
                if ((mouse_b & 1) && gui.getMouseFrame() == MAIN_FRAME && gui.getBrush() == BRUSH_EMITTER) {

                    gui.setCollision(CHECKBOX_CHECKED);

                    // If we have an enlarged brush
                    if (mouse_z > 1) {
                        if (mouse_z % 2 != 0) brush_size = mouse_z -1;
                        else brush_size = mouse_z;
                        // Plot the collision mask as necessary
                        for (int i = 0; i < brush_size; i++) {
                            for (int j = 0; j < brush_size; j++) {
                                Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].emitter = 1;
                                Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].emitter = 1;
                                Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].emitter = 1;
                                Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].emitter = 1;
                            }
                        }

                        // For one-tiler collisions
                    } else {
                        Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].emitter = 1;

                    }
                }

                // A right click in collision mode means that we're making an area passable
                // TODO: Add variable brush-size support
                if ((mouse_b & 2) && gui.getMouseFrame() == MAIN_FRAME && gui.getBrush() == BRUSH_EMITTER) {
                    gui.setCollision(CHECKBOX_CHECKED);

                    // If we have an enlarged brush
                    if (mouse_z > 1) {
                        if (mouse_z % 2 != 0) brush_size = mouse_z -1;
                        else brush_size = mouse_z;
                        // Plot the collision mask as necessary
                        for (int i = 0; i < brush_size; i++) {
                            for (int j = 0; j < brush_size; j++) {
                                Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].emitter = 0;
                                Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].emitter = 0;
                                Map[gui.getCurrentLayer()][(y1+(j*TILESIZE)/2)/TILESIZE+viewport.scroll_y-1][(x1 - (i*TILESIZE)/2)/TILESIZE+viewport.scroll_x].emitter = 0;
                                Map[gui.getCurrentLayer()][(y1-(j*TILESIZE)/2-TILESIZE)/TILESIZE+viewport.scroll_y-1][(x1 + (i*TILESIZE)/2+TILESIZE)/TILESIZE+viewport.scroll_x].emitter = 0;
                            }
                        }

                        // For one-tiler collisions
                    } else {
                        Map[gui.getCurrentLayer()][y1/TILESIZE+viewport.scroll_y-2][x1/TILESIZE+viewport.scroll_x].emitter = 0;

                    }

                }
            }
        }
    }
    // ********* (4) Scroll the map if necessary

    scrollMap();

    return 0;
} // short EditorMain::editorEngine()


// Draw the specified layer in the normal mode
void EditorMain::drawLayer(int lay) {

    short index, tileset;
    int posx, posy;

    // Check if we should reset the viewport
    resetViewport();

    // If we're not performing any file IO operations
    if (dataAccessState == ACCESS_FREE) {
        // Draw every tile that should appear in the current viewport
        for (int i = viewport.tile_x; i < viewport.tile_w; i++) {
            for (int j = viewport.tile_y; j < viewport.tile_h; j++) {
                index = Map[lay][j+viewport.scroll_y][i+viewport.scroll_x].index;
                tileset = Map[lay][j+viewport.scroll_y][i+viewport.scroll_x].tileset;

                posx = TILESIZE * (index / TILESIZE);
                posy = TILESIZE * (index % TILESIZE);

                masked_blit((BITMAP*)mapData[TILES1 + tileset].dat, map, posx, posy,
                            i*TILESIZE + viewport.pos_x, j*TILESIZE + viewport.pos_y, TILESIZE, TILESIZE);
            }
        }
    }
} // void EditorMain::drawLayer(int lay)


// Draw a semi-transparent layer
void EditorMain::drawTransLayer(int lay) {

    short index, tileset;
    int posx, posy;

    // Reset the viewport if that's the case
    resetViewport();

    // If we have access to the Map array
    if (dataAccessState == ACCESS_FREE) {
        for (int i = viewport.tile_x; i < viewport.tile_w; i++) {
            for (int j = viewport.tile_y; j < viewport.tile_h; j++) {
                index = Map[lay][j+viewport.scroll_y][i+viewport.scroll_x].index;
                tileset = Map[lay][j+viewport.scroll_y][i+viewport.scroll_x].tileset;

                posx = TILESIZE * (index / TILESIZE);
                posy = TILESIZE * (index % TILESIZE);

                // Draw the tile on the transparent tile
                masked_blit((BITMAP*)mapData[0 + tileset].dat, transTile, posx, posy, 0, 0, TILESIZE, TILESIZE);
                // Draw the transparent tile on the specified BITMAP
                draw_trans_sprite(map, transTile, i*TILESIZE + viewport.pos_x, j*TILESIZE + viewport.pos_y);
                // Fill the transparent tile with a medium gray
                clear_to_color(transTile, makecol(128,128,128));
            }
        }
    }
} // void EditorMain::drawTransLayer(int lay)

// Scroll map using the arrow keys
void EditorMain::scrollMap() {
    if (!gui.isFieldActive()) {
        if (key[KEY_RIGHT]) {
            if (viewport.scroll_x < mapWidth - viewport.tile_w) viewport.scroll_x++;
        }

        if (key[KEY_LEFT]) {
            if (viewport.scroll_x > 0 ) viewport.scroll_x--;
        }

        if (key[KEY_UP]) {
            if (viewport.scroll_y > 0) viewport.scroll_y--;
        }

        if (key[KEY_DOWN]) {
            if (viewport.scroll_y < mapHeight - viewport.tile_h) viewport.scroll_y++;
        }
    }
} // void EditorMain::scrollMap()

// Render the map to the specified BITMAP
// Renders the correponding selector as well
void EditorMain::renderMap(BITMAP* bmp) {

    if (gui.getMouseFrame() == MAIN_FRAME) {

        short x1=TILESIZE*(mouse_x/TILESIZE);
        short y1=TILESIZE*(mouse_y/TILESIZE)+3;
        short x2=x1+TILESIZE-1;
        short y2=y1+TILESIZE+1;

        drawSelector(map, x1, y1, x2, y2, gui.getBrush());
    }
    masked_blit(map, bmp, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    if (!gui.getAlpha()) {
        clear_to_color(map, makecol(255,255,255));
    } else {
        clear(map);
        for (int i = viewport.tile_x; i < viewport.tile_w; i++) {
            for (int j = viewport.tile_y; j < viewport.tile_h; j++) {
                masked_blit((BITMAP*)resources.data[TRANS_BK].dat, map, 0, 0, i*TILESIZE + viewport.pos_x, j*TILESIZE + viewport.pos_y, TILESIZE, TILESIZE);
            }
        }
    }
} // void EditorMain::renderMap(BITMAP* bmp)

void EditorMain::freeMap() {
    delete[] Map;
}

// Clear the memory
// TODO: Look into the Map de-allocating code, make sure there are no memory-leaks
void EditorMain::freeEditor() {
    destroy_bitmap(map);
    unload_datafile(mapData);
    destroy_bitmap(transTile);
} // EditorMain::freeEditor()
