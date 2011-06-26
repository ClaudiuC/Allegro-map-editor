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
*** \file    editormain.h
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Header file for the editor engine.
***
*** This code provides the API used to manage map operations like creating,
*** saving, loading, drawing and rendering
***
*** \note This code uses the following libraries:
***   -# Allegro 4.2.2, http://www.allegro.cc/
******************************************************************************/

#ifndef EDITORMAIN_H
#define EDITORMAIN_H

#include <allegro.h>

#include <malloc.h>
#include <iostream>
#include "..\gui\guimain.h"
#include "mapData.h"
#include "..\utils\dataformat.h"
#include "..\input\inputmouse.h"
#include "particleemitter.h"

using namespace std;

/** \def This defines the tilesize used through-out the editor
*** \todo Make the TILESIZE a member of Tile so the editor can use more
****      tilesizes
**/
#define TILESIZE 32

/** \def These define the editor current file IO state. The editor
***      shouldn't perform any reading/writing operations on the
***      EditorMain#Map private member while IO operations are beeing
***      performed
**/
//@{
#define ACCESS_WRITE_ONLY 0
#define ACCESS_READ_ONLY  1
#define ACCESS_FREE       2
//@}

#define PLAY_PARTICLES  0
#define PAUSE_PARTICLES 1

/** \struct Tile editormain.h "src\editor\editormain.h"
*** \brief The Tile structure defines a map tile. The Map is defined
***        as an array of the form Map[layers][mapHeight][mapWidth]
**/
typedef struct Tile {
    short index;        /**< short variable, defines the index of the tile from the tileset **/
    short tileset;      /**< short variable, defines the index of the tileset **/
    short collision;    /**< short variable, defines the walkability status **/

    short emitter;      /**< keep it simple for starters, should hold the value of the particle type (this will indicate a file in ParticleEmitter later on... **/
    //short force;
} Tile;

/** \struct Camera editormain.h "src\editor\editormain.h"
*** \brief The Camera structure defines the EditorMain#viewport
***
*** This is used in order to save memory and CPU usage by only
*** rendering the area of the map that we're currently looking at.
*** It also offers flexibility by defining the position and size of
*** the viewport, both in pixels or TILESIZE
**/
typedef struct Camera {
    /** The position variables **/
    //@{
    short tile_x, tile_y;
    short pixel_x, pixel_y;
    //@}

    /** The size variables **/
    //@{
    int tile_w, tile_h;
    int pixel_w, pixel_h;
    //@}

    /** Size variables used in EditorMain#resetViewport **/
    //@{
    int init_w, init_h;
    //@}

    /** Viewport position on screen **/
    //@{
    int pos_x, pos_y;
    //@}

    /** Scroll variables used with EditorMain#scrollMap() **/
    //@{
    int scroll_x, scroll_y;
    //@}
} Camera;

/** \class EditorMain editormain.h "src\editormain.h"
*** \brief This provides the methods used to edit and render tiled maps
*** \todo More return functions
*** \todo Better file IO methods
*** \todo Better OOP concepts implementation
*** \todo !IMPORTANT! Better error-checking routines
**/
class EditorMain {
    /** These classes use some of EditorMain private members **/
    //@{
    friend class TilesetMain;
    friend class GuiMain;
    friend class MinimapMain;
    //@}
public:
    EditorMain();
    ~EditorMain();

    /** \name initEditor()
    *** \brief This function is responsible with loading the resources,
    ***        reading the settings from editor.ini and creating the Map
    ***        array.
    *** \sa restartEditor()
    **/
    void initEditor();

    /** \name restartEditor()
    *** \brief Contains mostly the same code as initEditor, just that it
    ***        only triggers if the Map has to be resized.
    *** \param lays Number of layers
    *** \param width Width of the map
    *** \param height Map height
    *** \sa initEditor()
    **/
    void restartEditor(int lays, int width, int height);

    /** \name Map file IO functions
    *** \brief A set of functions that are responsible with the IO operations
    ***        on the HDD. Some are deprecated, see the additional comments
    *** \return -1 if there were any errors, 0 otherwise
    **/
    //@{
    //! Creates a default map; Map[3][100][100], should only be used with the load methods
    short newMap();
    short newMap(string name, string lay, string width, string height);
    //! Deprecated, creates a default map but with a custom name
    short newMap(string name);

    //! Loads the default map
    short loadMap();
    short loadMap(string name);
    //! Deprecated, I don't know why I created this
    short loadMap(string name, string lay, string w, string h);

    //! Saves a map with the default test_map.dat filename
    short saveMap();
    short saveMap(string name);
    //@}

    /** \name getCurrentMap()
    *** \brief Returns the filename of the current map
    *** \return Current map's name
    **/
    string getCurrentMap() {
        return current_map;
    }

    /** \name getMaxLayers()
    *** \brief Returns the number of layers of the current map.
    ***        Used to display the "Current Layer" button
    *** \return Current map's number of layers
    **/
    short getMaxLayers() {
        return layers;
    }

    /** \name setViewport()
    *** \brief Inits the viewport with a custom size and
    ***        position on screen
    *** \param px Position on the X axis, in pixels
    *** \param py Position on the Y axis, in pixels
    *** \param w Width in tiles
    *** \param h Height in tiles
    **/
    void setViewport(int px, int py, int w, int h);

    /** \name resetViewport()
    *** \brief Resizes the viewport in case it exceeds the Map or screen size,
    ***        whichever is smaller.
    **/
    void resetViewport();

    /** \name scrollMap()
    *** \brief Takes keyboard input and scrolls the map accordingly
    **/
    void scrollMap();

    /** \name setBrushSize()
    *** \brief Sets the brush size, controlled by the mouse wheel.
    ***
    *** Both the map canvas and tileset frame use the same function
    *** to draw the selector. The tileset selector should only change
    *** size based on wheter a tile or an object is drawn. For this reason,
    *** the brush size is saved once the cursor hovers over the tileset and
    *** restored when it hovers back on the canvas.
    **/
    void setBrushSize(int size) { exMouseZ = size; restoreBrush = true; }

    /** \name Draw functions
    *** \brief This set of methods reads Map informations and plots the
    ***        specified graphics to the screen.
    **/
    //@{
    void drawLayer(int lay);
    void drawTransLayer(int lay);
    void drawGrid();
    void drawCollision();
    /** drawSelector() draws a rectangle around the cursor. In case the pointer
    *** hovers over the map Canvas, it draws a rectangle the size of the brush,
    *** on the tileset it draws a rectangle around the selected/focused tile/object
    **/
    void drawSelector(BITMAP *bmp, int x1, int y1, int x2, int y2, short type);
    //@}

    /** \name Map editing methods
    *** \brief These functions are used to draw a new tile/object to the map
    *** \param x1 The position on the X axis, using TILESIZE
    *** \param y1 The position on the Y axis, using TILESIZE
    **/
    //@{
    void drawTile(int x1, int y1);
    void drawObject(int x1, int y1);
    void floodFill(int x1, int y1);
    //@}

    /** \name editorEngine()
    *** \brief This method is responsible with managing the actions that occur during
    ***        the editing of the map
    *** \return Error code
    **/
    short editorEngine();

    /** \name renderMap()
    *** \brief This method plots the current viewport of the map on the specified BITMAP
    *** \param bmp The BITMAP to draw the map to
    **/
    void renderMap(BITMAP *bmp);

    // Extras from freeEditor() - clean the ***Map
    void freeMap();

    /** \name freeEditor()
    *** \brief Unloads any resources loaded with initEditor() or restartEditor()
    **/
    void freeEditor();


    /***************************************************************************************/
    /** Emitter code                                                                      **/
    /***************************************************************************************/
    void addEmitter(int x, int y);

    void playEmitters();


    void drawEmitterGrid();

    int getScrollX() {return viewport.scroll_x;}
    int getScrollY() {return viewport.scroll_y;}
protected:
private:
    /** Beeing a dynamic map structure, these define the map size
    **/
    //@{
    short layers, mapWidth, mapHeight;
    //@}

    /** Used with the drawTile() method
    **/
    //@{
    short current_tile, mouse_tileset;
    //@}

    int brush_size; //!< The current brush size

    string current_map; //!< The current map's filename, used for various operations

    Tile ***Map; //!< The dynamic 3D array used to hold the map data
    //Tile map_debugger[3][100][20];
    Camera viewport;

    /** Allegro defined structures. The tileset datafile and an intermediary BITMAP used to draw the map
    *** \note Added an extra BITMAP member. Moved from the drawTransLayer() to save memory and mem usage
    **/
    //@{
    DATAFILE *mapData;
    BITMAP *map;
    BITMAP *transTile;
    //@}

    /** Used with the setBrushSize() method, for the transition between the tileset frame and the map
    *** canvas.
    **/
    //@{
    short exMouseZ;
    bool restoreBrush;
    //@}

    /** These variables are used with the drawObject() method. They define the area and tileset to crop
    *** from. isObject determines wheter we're currently drawing an object or not.
    **/
    //@{
    short current_object_x1, current_object_x2, current_object_y1, current_object_y2, object_tileset;
    bool isObject;
    //@}

    /** A flag that determines wheter the editor is currently performing any file IO operations **/
    short dataAccessState;

    short emitter_state;
    ParticleEmitter particleEmitter;
};

#endif // EDITORMAIN_H
