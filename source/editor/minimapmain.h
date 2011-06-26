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
*** \file    minimapmain.h
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Header file for the Minimap widget
***
*** This code provides the API used to create, display and use the MiniMap
***
*** \note This code uses the following libraries:
***   -# Allegro 4.2.2, http://www.allegro.cc/
******************************************************************************/

#ifndef MINIMAPMAIN_H
#define MINIMAPMAIN_H

#include "editormain.h"
#include "..\gui\guimain.h"

#define MAX_SIZE 100

/** \class MinimapMain minimapmain.h "src\editor\minimapmain.h"
*** \brief This class provides the methods to work with the Minimap
**/
class MinimapMain {
public:
    MinimapMain();
    ~MinimapMain();

    /** \name initMinimap()
    *** \brief Creates the Minimap BITMAP
    **/
    void initMinimap();

    /** \name updateMiniMapCoords()
    *** \brief Centers the minimap widget
    **/
    void updateMiniMapCoords();

    /** \name drawMiniMap()
    *** \brief Renders the Minimap on the specified BITMAP
    *** \param bmp The destination BITMAP
    **/
    void drawMiniMap(BITMAP *bmp);

    /** \name moveMiniMap()
    *** \brief This method allows map scrolling with the mouse
    ***        by dragging the viewport on the Minimap. The viewport
    ***        is displayed as a blue rectangle.
    **/
    void moveMiniMap();

    /** \name freeMinimap()
    *** \brief Destroys the resources created using the initMinimap method
    **/
    void freeMinimap();
protected:
private:
    /** These Allegro defined structures are used to hold the minimap GFX
    **/
    //@{
    BITMAP *mini;
    BITMAP *minimap;
    //@}

    int minimap_x, minimap_y; //!< Minimap widget position in pixels
    int aux_resize; //!< Used to resize the minimap widget if the map size is to big
};

#endif // MINIMAPMAIN_H
