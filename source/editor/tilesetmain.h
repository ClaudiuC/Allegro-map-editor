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
*** \file    tilesetmain.h
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Header file for the Tileset widget
***
*** This code provides the API used to create, display and use the Tilesets
***
*** \note This code uses the following libraries:
***   -# Allegro 4.2.2, http://www.allegro.cc/
******************************************************************************/

#ifndef TILESETMAIN_H
#define TILESETMAIN_H

#include "editormain.h"
#include "..\gui\guimain.h"
#include "..\input\inputmouse.h"

/** \def PREVIOUS, NEXT
*** \brief Used for code readabilty
*** \sa TilesetMain#changeIndex(short direction)
**/
//@{
#define NEXT     0
#define PREVIOUS 1
//@}

/** \def UP, DOWN
*** \brief Used for code readability
*** \sa TilesetMain#scrollTileset(short direction)
**/
#define UP   0
#define DOWN 1

/** \class TilesetMain tilesetmain.h "src\editor\tilesetmain.h"
*** \brief This class provides the methods to browse the tileset datafile
***        and grab tiles/objects from a tileset BITMAP
**/
class TilesetMain
{
    friend class EditorMain;
    public:
        TilesetMain();
        ~TilesetMain();

        /** \name initTileset()
        *** \brief Temporary blank
        **/
        void initTileset();

        /** \name changeIndex()
        *** \brief Changes the current tileset
        *** \param direction Short value, defined as NEXT or PREVIOUS
        **/
        void changeIndex(short direction);

        /** \name scrollTileset()
        *** \brief Ussualy tilesets don't fit on the screen. This method is used
        ***        to scroll the tileset in a given direction
        **/
        void scrollTileset(short direction);

        /** \name getTilesetName()
        *** \brief Returns the current tileset name (from the DATAFILE)
        *** \return Tileset name
        **/
        string getTilesetName();

        /** \name updateTilesetActions()
        *** \brief Check if the user grabbed a tile/object
        **/
        void updateTilesetActions();

        /** \name drawTileset()
        *** \brief Draws the current tileset frame, at the specified position,
        ***        on the specified BITMAP
        *** \param bmp Destination BITMAP
        *** \param x Destination position on the X axis
        *** \param y Destination position on the Y axis
        *** \param w Tileset width
        *** \param h Tileset height
        **/
        void drawTileset(BITMAP *bmp, short x, short y, short w, short h);

        /** \name freeTileset()
        *** \brief Temporary blank
        **/
        void freeTileset();
    protected:
    private:
        /** Variables used to determine which tileset is in use and what area
        *** to display.
        **/
        //@{
        short tilesetIndex;
        short scroll_y;
        //@}

        /** Check if the brush is enlarged
        **/
        bool isSetMouseZ;

        /** Variable used to determine the area the selected object is
        *** on.
        **/
        short object_x, object_y, object_x2, object_y2;
        /** Check if the user is in the process of selecting an object
        **/
        bool object_selected;
};

#endif // TILESETMAIN_H
