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
*** \file    guimain.cpp
*** \author  Gilcescu-Ceia Claudiu, hazardous.dev@gmail.com
*** \brief   Source file for the GUI engine.
******************************************************************************/

#include "guimain.h"

extern GuiResources resources;
extern EditorMain editor;
extern TilesetMain tileset;
extern MinimapMain minimap;
extern volatile int allmap_exit;

GuiMain::GuiMain() {
    panelState = STATE_OPTIONS;
    currentLayer = LAYER_ONE;

    grid = collision = quit = false;
    layers = alpha = true;

    brush = BRUSH_DRAW;

    mouse_frame = MAIN_FRAME;
    gui_x = gui_y = 0;

    preview = false;
}

GuiMain::~GuiMain() {
}

void GuiMain::loadInterface() {

    //brush_bmp = create_bitmap(64, 64);
    //clear_to_color(brush_bmp, makecol(255, 255, 255));

    // Parent frame, basically a frame containing all the widgets on the screen
    // Note: It's called parent by the parent<->child relation isn't yet defined by code
    // Todo: See the above note
    // Todo: Add a menu, it's more convenient
    // Todo: Create a menu widget
    // Todo: Create a WYSIWYG GUI editor -> export data to .cfg/.xml/.dat
    frame.addFrame(0, 0, SCREEN_W, SCREEN_H, "No map loaded");
    frameAll = frame.getLastFrameID();

    // The map canvas/workspace. This is where the viewport is displayed. Also, here's where all the action
    // happens. Cool frame :D
    // Todo: The frame's final height is around 19.5 tiles, which makes the map displaying a little
    // awkward. It should be either 19 tiles, either 20.
    // Todo: Menu (see main frame)
    frame.addFrame(0, TILESIZE / 2, 768, TILESIZE * 19, "Workspace ");
    frameMain = frame.getLastFrameID();

    // The object selector
    // Note: For now only tiles and objects from tilesets (aka multiple tiles "in one shot") are
    // displayed here. Also, the frame only displays fixed width/height tilesets, which must be defined in code
    // Todo: Change minimap and tileset widget code to handle tilesets loaded at runtime or created at runtime
    // Todo: Option to import/export/edit custom tilesets
    // Todo: Option to save and load objects created from tilesets at run-time
    frame.addFrame(768, TILESIZE / 2, TILESIZE * 8, TILESIZE * 19, "Object Selector");
    frameTset = frame.getLastFrameID();

    // The minimap frame
    // Todo: Fix displaying of variable sized maps
    frame.addFrame(0, TILESIZE * 19 + TILESIZE / 2, 128  + TILESIZE / 2, 128 + TILESIZE / 2, "Minimap");
    frameMini = frame.getLastFrameID();

    // Settings frame
    // This holds all the settings available in game
    // Todo: A tab widget would be helpful here, instead of using buttons
    frame.addFrame(128  + TILESIZE / 2, TILESIZE * 19 + TILESIZE / 2, 768-128-TILESIZE/2, 128  + TILESIZE / 2, "Settings");
    frameSettings = frame.getLastFrameID();

    // Not many options for now, simply showing the current tile and coordinates/tileset
    // Make it scrollable for larger objects
    frame.addFrame(768, TILESIZE * 19 + TILESIZE / 2, 256, TILESIZE*4 + TILESIZE / 2, "Options");


    // This is the starting point for the buttons in the settings frame
    // Note: Will be replaced once I add the parent<->child relation in the GUI code, using relative to parent
    // coordinates.
    short button_x = 132 + TILESIZE/2;
    // New map button
    // Modified behaviour to be more tab-like
    button.addButton(button_x, TILESIZE * 20 + 4, "New map");
    buttonNewMap = button.getLastButtonID();

    // These only display when New Map button is active
    // Map name
    field.addField(button.getButtonPosX(buttonNewMap)+text_length(font, "Map name")+10, TILESIZE*20+34, 200, 10, "Map name");
    fieldNewName = field.getLastFieldID();

    // Layer number
    field.addField(button.getButtonPosX(buttonNewMap)+text_length(font, "Map name")+10, TILESIZE*20+64, 200, 10, "Layers");
    fieldNewLay = field.getLastFieldID();

    // X size (in tiles)
    field.addField(button.getButtonPosX(buttonNewMap)+text_length(font, "Map name")+10, TILESIZE*20+94, 95-text_length(font, "Y")/2, 10, "Size: X");
    fieldNewW = field.getLastFieldID();
    // Y size (in tiles)
    field.addField(button.getButtonPosX(buttonNewMap)+text_length(font, "Map name")+text_length(font, "Map size: X")+text_length(font, "Y")+28, TILESIZE*20+94, 90-text_length(font, "Y")/2, 10, "Y");
    fieldNewH = field.getLastFieldID();

    // The submit button
    button.addButton(button.getButtonPosX(buttonNewMap)+220+text_length(font, "Map name")+10, TILESIZE*20+29, "Create");
    buttonNewMapOK = button.getLastButtonID();

    // Next tab -> Load Map
    button_x += button.getButtonSizeW(buttonNewMap);
    button.addButton(button_x, TILESIZE * 20 + 4, "Load map");
    buttonLoadMap = button.getLastButtonID();

    // Map name
    // Todo: Add a file selector for loading/saving maps
    // Noto: Could as well be a listing of the Data directory
    field.addField(button.getButtonPosX(buttonNewMap)+text_length(font, "Map name")+10, TILESIZE*20+34, 200, 10, "Map name");
    fieldLoadName = field.getLastFieldID();
    // Submit button
    button.addButton(button.getButtonPosX(buttonNewMap)+220+text_length(font, "Map name")+10, TILESIZE*20+29, "Load");
    buttonLoadMapOK = button.getLastButtonID();

    // Save map tab
    button_x += button.getButtonSizeW(buttonLoadMap);
    button.addButton(button_x, TILESIZE * 20 + 4, "Save map");
    buttonSaveMap = button.getLastButtonID();

    // Same structure as before
    field.addField(button.getButtonPosX(buttonNewMap)+text_length(font, "Map name")+10, TILESIZE*20+34, 200, 10, "Map name");
    fieldSaveName = field.getLastFieldID();

    button.addButton(button.getButtonPosX(buttonNewMap)+220+text_length(font, "Map name")+10, TILESIZE*20+29, "Save");
    buttonSaveMapOK = button.getLastButtonID();

    // Options tab
    button_x += button.getButtonSizeW(buttonSaveMap);
    button.addButton(button_x, TILESIZE * 20 + 4, "Options");
    buttonOptions = button.getLastButtonID();

    // Layer label (it's followed by N buttons that change the current layer
    label.addLabel(button.getButtonPosX(buttonNewMap), TILESIZE*20+34, makecol(0,0,0), "Layer:");
    labelLayers = label.getLastLabelID();

    // The layer list.
    // Should be changed to be displayed programatically, easy task
    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+10, TILESIZE*20+34, "1");
    buttonLayerOne = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+30, TILESIZE*20+34, "2");
    buttonLayerTwo = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+50, TILESIZE*20+34, "3");
    buttonLayerThree = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+70, TILESIZE*20+34, "4");
    buttonLayerFour = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+90, TILESIZE*20+34, "5");
    buttonLayerFive = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+110, TILESIZE*20+34, "6");
    buttonLayerSix = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+130, TILESIZE*20+34, "7");
    buttonLayerSeven = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+150, TILESIZE*20+34, "8");
    buttonLayerEight = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+170, TILESIZE*20+34, "9");
    buttonLayerNine = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Layer:")+190, TILESIZE*20+34, "10");
    buttonLayerTen = button.getLastButtonID();

    // What visuals to display?
    label.addLabel(button.getButtonPosX(buttonNewMap), TILESIZE*20+64, makecol(0,0,0), "Display:");
    labelDisplay = label.getLastLabelID();

    // This displays a gray grid over the map, usefull when you need to see which tile is which
    // Note: In v0.1 of the editor this was a must. As new features are added to the current version, the need for
    // a grid drops drastically.Still, it's more than welcomed when certain tiles are cut weird (bottom or upper half
    // transparent
    short check_x = 30 + button.getButtonPosX(buttonNewMap) + text_length(font, "Display:") + text_length(font, "Grid");
    checkbox.addCheckbox(check_x, TILESIZE*20+64, 20,18, "Grid", 0, CHECKBOX_UNCHECKED);
    checkboxGrid = checkbox.getLastCheckboxID();

    // Display collision grid
    // Note: This was integrated with the "Show grid" button in v0.1
    check_x += 35+text_length(font, "Marks");
    checkbox.addCheckbox(check_x, TILESIZE*20+64, 20,18, "Marks", 0, CHECKBOX_UNCHECKED);
    checkboxCollision = checkbox.getLastCheckboxID();

    // Display the layers below the current one (extremely usefull when an object spans over multiple layers, checked
    // by default.
    // Todo: Add "Show layers above" option, sometimes is welcomed
    check_x += 35+text_length(font, "Layers below");
    checkbox.addCheckbox(check_x, TILESIZE*20+64, 20,18, "Layers below", 0, CHECKBOX_CHECKED);
    checkboxLayers = checkbox.getLastCheckboxID();

    // Preview mode freezes any editing and displays the final map as it should look in game
    check_x += 35+text_length(font, "Preview");
    checkbox.addCheckbox(check_x, TILESIZE*20+64, 20,18, "Preview", 0, CHECKBOX_UNCHECKED);
    checkboxPreview = checkbox.getLastCheckboxID();

    // Shouldn't be enabled on slow PCs because it's slow. It bassically displays layers below
    // the current one in a gradient transparency mode. Only check-able if the "Layers below" checkbox is marked.
    // Very usefull because the tiles on the current layer stand out, nice.
    // Todo: Limit this to the last 3 layers only. On a 10 layered map this option renders the editor unusable on
    // most PCs
    check_x += 35+text_length(font, "Enable alpha");
    checkbox.addCheckbox(check_x, TILESIZE*20+64, 20,18, "Enable alpha", 0, CHECKBOX_CHECKED);
    checkboxAlpha = checkbox.getLastCheckboxID();

    // The brush type
    // Note: This will get a tab of it's own... soon
    label.addLabel(button.getButtonPosX(buttonNewMap), TILESIZE*20+94, makecol(0,0,0), "Brush type:");
    labelBrush = label.getLastLabelID();

    // Simple copy->paste drawing from the tileset. This is the option that works for objects as well
    button.addButton(button.getButtonPosX(buttonNewMap)+text_length(font, "Brush type:")+20, TILESIZE*20+94, "Draw");
    buttonDraw = button.getLastButtonID();

    // Erase. As the title says, it replaces any selected tile with a _MAGIC_PINK_ one, rendering it transparent.
    // Todo: Make this customizable, it defaults to the [1][0] tile from the first tileset
    button.addButton(button.getButtonPosX(buttonDraw)+text_length(font,"Draw")+20, TILESIZE*20+94, "Erase");
    buttonErase = button.getLastButtonID();

    // This replaces all identical tiles on the current layer with the selected one.
    // Todo: handle enclosed tiles (islands of certain tiles
    button.addButton(button.getButtonPosX(buttonErase)+text_length(font,"Erase")+20, TILESIZE*20+94, "Flood fill");
    buttonFlood = button.getLastButtonID();

    // Add collision, simple enough
    // Todo: Make collision value adjustable, rather than 1/0 (for rough terrain)
    // Todo: Make collision tile size adjustable (exclude from the Tile structure, make separate layer)
    button.addButton(button.getButtonPosX(buttonFlood)+text_length(font,"Flood fill")+20, TILESIZE*20+94, "Collision");
    buttonCollision = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonNewMap) + text_length(font, "Options:") + 10, TILESIZE*20+64, "Add emitter");
    buttonPAdd = button.getLastButtonID();

    button.addButton(button.getButtonPosX(buttonPAdd) + button.getButtonSizeW(buttonPAdd) + 10, TILESIZE*20+64, "Add force");
    buttonFAdd = button.getLastButtonID();

    //! Work in progress
    // The particles tab, this should provide methods to add particle emitters on the map (cooler animated tiles)
    // That reminds me:
    // Todo: Animated tiles/objects
    button_x += button.getButtonSizeW(buttonOptions);
    button.addButton(button_x, TILESIZE * 20 + 4, "Particles");
    buttonParticles = button.getLastButtonID();

    // Simple play/pause options
    label.addLabel(button.getButtonPosX(buttonNewMap), TILESIZE*20+34, makecol(0,0,0), "Particle panel:");
    labelPPanel = label.getLastLabelID();

    // Pause
    button.addButton(button.getButtonPosX(buttonNewMap)+text_length(font, "Particle panel:")+20, TILESIZE*20+34, "Pause");
    buttonPPause = button.getLastButtonID();

    // Play
    button.addButton(button.getButtonPosX(buttonNewMap)+text_length(font, "Particle panel")+button.getButtonSizeW(buttonPPause)+30, TILESIZE*20+34, "Play");
    buttonPPlay = button.getLastButtonID();

    // Add a particle emitter
    // This should display a list of particle emitters instead of the tileset
    // A more advanced version should have the possibility to add user-created emitters (or this version?)
    label.addLabel(button.getButtonPosX(buttonNewMap), TILESIZE*20+64, makecol(0,0,0), "Options:");
    labelPOptions = label.getLastLabelID();


    // Quit, jus' like that
    button_x += button.getButtonSizeW(buttonParticles);
    button.addButton(button_x, TILESIZE * 20 + 4, "Quit");
    buttonQuit = button.getLastButtonID();

    // Object selector option
    // Scroll up
    // Todo: Fix the scrollbar code, use that
    button.addButton(768+4, TILESIZE*20+4, "Scroll up");
    buttonScrollUp = button.getLastButtonID();

    // Scroll down, see above
    button.addButton(768+4+text_length(font, "Scroll up")+10, TILESIZE*20+4, "Scroll down");
    buttonScrollDown = button.getLastButtonID();

    // Previous tileset / object list (see particle todo
    button.addButton(768+4+text_length(font, "Scroll up")+text_length(font, "Scroll down")+20, TILESIZE*20+4, " <<");
    buttonPrevious = button.getLastButtonID();

    // Next tileset, see above
    button.addButton(768+4+text_length(font, "Scroll up")+text_length(font, "Scroll down")+text_length(font, " <<")+30, TILESIZE*20+4, ">> ");
    buttonNext = button.getLastButtonID();
}

void GuiMain::updateInterface() {
    // This should be fixed once I add the parent->child code.
    // It basically hides/shows parts of the GUI as needed
    button.hideAllButtons();
    label.hideAllLabels();
    checkbox.hideAllCheckboxes();
    field.hideAllFields();

    button.showButton(buttonLoadMap);
    button.showButton(buttonNewMap);
    button.showButton(buttonQuit);
    button.showButton(buttonParticles);

    button.showButton(buttonNext);
    button.showButton(buttonOptions);
    button.showButton(buttonPrevious);
    button.showButton(buttonSaveMap);
    button.showButton(buttonScrollDown);
    button.showButton(buttonScrollUp);

    // Decide what to show taking the panelState variable into account
    switch (panelState) {
    case STATE_OPTIONS: {
        button.showButton(buttonCollision);
        button.showButton(buttonDraw);
        button.showButton(buttonFlood);
        button.showButton(buttonErase);

        // IDs are assigned incrementally. Ordered initialization of elements
        // should make code easier to manipulate
        short i = 0;
        while (i < editor.getMaxLayers()) {
            button.showButton(buttonLayerOne+i);
            i++;
        }

        label.showLabel(labelLayers);
        label.showLabel(labelBrush);
        label.showLabel(labelDisplay);

        checkbox.showCheckbox(checkboxAlpha);
        checkbox.showCheckbox(checkboxCollision);
        checkbox.showCheckbox(checkboxGrid);
        checkbox.showCheckbox(checkboxLayers);
        checkbox.showCheckbox(checkboxPreview);

        break;
    }
    case STATE_LOADMAP: {
        field.showField(fieldLoadName);
        button.showButton(buttonLoadMapOK);

        break;
    }
    case STATE_NEWMAP: {
        field.showField(fieldNewName);
        field.showField(fieldNewLay);
        field.showField(fieldNewW);
        field.showField(fieldNewH);
        button.showButton(buttonNewMapOK);

        break;
    }
    case STATE_SAVEMAP: {
        field.showField(fieldSaveName);
        button.showButton(buttonSaveMapOK);

        break;
    }
    case STATE_PARTICLES: {
        button.showButton(buttonPAdd);
        button.showButton(buttonFAdd);
        //button.showButton(buttonParticles);
        button.showButton(buttonPPause);
        button.showButton(buttonPPlay);
        label.showLabel(labelPPanel);
        label.showLabel(labelPOptions);

        break;
    }
    }

    // Update checkboxes, check states later
    checkbox.updateCheckboxes();

    // Text fields return values when ENTER is pressed. Handle this by passing button_pressed
    // the necessary ID to init an action when this behaviour is detected
    short button_pressed = -1;
    if (field.updateFields() != "") {
        short tmp_id = field.getActiveField();
        if (tmp_id != -1) {
            if (tmp_id == fieldLoadName) {
                button_pressed = buttonLoadMapOK;
            }
            if (tmp_id == fieldNewName || tmp_id == fieldNewH || tmp_id == fieldNewLay || tmp_id == fieldNewW) {
                button_pressed = buttonNewMapOK;
            }
            if (tmp_id == fieldSaveName) {
                button_pressed = buttonSaveMapOK;
            }
        }
    }

    // If the above conditions aren't valid, reinitialize the button_pressed variable
    // with the value returned by updateButtons() (if any)
    if (button_pressed == -1) button_pressed = button.updateButtons();

    // ESC key should trigger the quit button
    // Todo: Add more shortcuts, includeing key combos
    if (key[KEY_ESC]) button_pressed = buttonQuit;

    // So, a button was pressed
    if (button_pressed != -1) {
        // I use Allegro's dialogs for now
        // Todo: Add a dialog class
        if (button_pressed == buttonQuit) {
            bool sure = true;
            if (alert("The current map might be unsaved", "You can save it now to prevent", "data loss or something...", "Save", "Exit", 0, 0) == 1) {
                if (editor.saveMap(editor.getCurrentMap().c_str()) == -1) {
                    if (alert("Bad day for mapping", "Something went wrong while saving", "You can go back and try saving again", "Go back", "Spartans don't go back!", 0, 0) == 1) {
                        sure = false;
                    } else sure = true;
                } else alert("Just saved it!", "You can load it as", editor.getCurrentMap().c_str(), "Great! Now exit", NULL, 0, 0);
            }
            if (sure) quit = true;
        }
        if (button_pressed == buttonLoadMap) {
            panelState = STATE_LOADMAP;
        }
        if (button_pressed == buttonNewMap) {
            panelState = STATE_NEWMAP;
        }
        if (button_pressed == buttonOptions) {
            panelState = STATE_OPTIONS;
            brush = BRUSH_DRAW;
            button.setButtonActive(buttonDraw);
        }
        if (button_pressed == buttonSaveMap) {
            panelState = STATE_SAVEMAP;
        }
        if (button_pressed == buttonParticles) {
            //panelState = STATE_PARTICLES;
            //brush = BRUSH_EMITTER;
            //button.setButtonActive(buttonPAdd);
        }
        // Again, the advantage of the auto_increment ID assigning
        if (button_pressed >= buttonLayerOne && button_pressed <= buttonLayerTen) {
            currentLayer = button_pressed - buttonLayerOne;
        }
        if (button_pressed == buttonDraw) {
            brush = BRUSH_DRAW;
        }
        if (button_pressed == buttonErase) {
            brush = BRUSH_ERASE;
        }
        if (button_pressed == buttonCollision) {
            brush = BRUSH_COLLISION;
        }
        if (button_pressed == buttonFlood) {
            brush = BRUSH_FLOOD;
        }
        if (button_pressed == buttonPAdd) {
            brush = BRUSH_EMITTER;
        }
        if (button_pressed == buttonScrollDown) {
            tileset.scrollTileset(DOWN);
        }
        if (button_pressed == buttonScrollUp) {
            tileset.scrollTileset(UP);
        }
        if (button_pressed == buttonNext) {
            tileset.changeIndex(NEXT);
        }
        if (button_pressed == buttonPrevious) {
            tileset.changeIndex(PREVIOUS);
        }
        // Todo: Look into these, theyr buggish and wrong
        if ( button_pressed == buttonLoadMapOK ) {
            if (alert("The current map might be unsaved", "You can save it now to prevent", "data loss or something...", "Save", "Just go on", 0, 0) == 1) {
                if (editor.saveMap(editor.getCurrentMap().c_str()) == -1) {
                    alert("Bad day for mapping", "Something went wrong while saving", "You can go back and try saving again", "Go back", "Spartans don't go back!", 0, 0);
                } else alert("Just saved it!", "You can load it as", editor.getCurrentMap().c_str(), "Great!", NULL, 0, 0);
            }
            string tmp_name = field.getFieldText(fieldLoadName);
            if (editor.loadMap(tmp_name) == -1) {
                alert("Ooops!", "Looks like I couldn't find the map :|", "Please make sure you typed the correct name", "Whatever...", NULL, 0, 0);
                field.clearFieldText(fieldLoadName);
            } else {
                alert("Nice map!", "Loading finished", NULL, "Hmmm... button", NULL, 0, 0);
                minimap.updateMiniMapCoords();
                field.clearFieldText(fieldLoadName);
            }
        }

        if ( button_pressed == buttonNewMapOK ) {
            if (alert("The current map might be unsaved", "You can save it now to prevent", "data loss or something...", "Save", "Just go on", 0, 0) == 1) {
                if (editor.saveMap(editor.getCurrentMap().c_str()) == -1) {
                    alert("Bad day for mapping", "Something went wrong while saving", "You can go back and try saving again", "Go back", "Spartans don't go back!", 0, 0);
                } else alert("Just saved it!", "You can load it as", editor.getCurrentMap().c_str(), "Great!", NULL, 0, 0);
            }
            /*! \deprecated Creating a new map with default width and height (yeah, nasty)
            */
            /*
            if (editor.newMap(tmp_name) == -1) {
                alert("Damn!", "I couldn't create the map", "Please try again", "#%@$%... OK", NULL, 0, 0);
                field.clearFieldText(fieldNewName);
            } else {
                editor.loadMap(tmp_name);
                alert("Map created!", "I filled the first layer with grass.", "Nice huh?", "Just go on", NULL, 0, 0);
                field.clearFieldText(fieldNewName);
            }
            */

            string tmp_name = field.getFieldText(fieldNewName);
            string tmp_lay  = field.getFieldText(fieldNewLay);
            string tmp_w = field.getFieldText(fieldNewW);
            string tmp_h = field.getFieldText(fieldNewH);

            if (editor.newMap(tmp_name, tmp_lay, tmp_w, tmp_h) == -1) {
                alert("Damn!", "I couldn't create the map", "Please try again", "#%@$%... OK", NULL, 0, 0);
                field.clearFieldText(fieldNewName);
                field.clearFieldText(fieldNewLay);
                field.clearFieldText(fieldNewW);
                field.clearFieldText(fieldNewH);
            } else {
                editor.loadMap(tmp_name);
                minimap.updateMiniMapCoords();

                currentLayer = LAYER_ONE;
                grid = collision = quit = false;
                layers = alpha = true;
                brush = BRUSH_DRAW;
                gui_x = gui_y = 0;
                preview = false;

                alert("Map created!", "I filled the first layer with grass.", "Nice huh?", "Just go on", NULL, 0, 0);
                field.clearFieldText(fieldNewName);
                field.clearFieldText(fieldNewLay);
                field.clearFieldText(fieldNewW);
                field.clearFieldText(fieldNewH);
            }
        }

        if ( button_pressed == buttonSaveMapOK ) {
            string tmp_name = field.getFieldText(fieldSaveName);
            if (editor.saveMap(tmp_name) == -1) {
                alert("It wasn't me!", "Just couldn't save the map", "Please try again", "#%@$%... OK", NULL, 0, 0);
                field.clearFieldText(fieldSaveName);
            } else {
                editor.saveMap(tmp_name);
                if (alert("Map saved!", "Do you want to edit this map", "or the newly saved one?", "Oldies but goldies", "Old out, new in!", 0, 0) == 2) {
                    if (editor.loadMap(tmp_name) == -1) {
                        alert("Unlucky guy you are...", "I couldn't load the newly saved map, you named it", tmp_name.c_str(), "Hmmm...", NULL, 0, 0);
                    } else {
                        minimap.updateMiniMapCoords();
                    }
                }
                field.clearFieldText(fieldSaveName);
            }
        }
        if ( button_pressed == buttonPPause ) {
            editor.emitter_state = PAUSE_PARTICLES;
        }
        if ( button_pressed == buttonPPlay ) {
            editor.emitter_state = PLAY_PARTICLES;
        }
    }

    // Emphasize on tab-like behaviour
    switch (panelState) {
    case STATE_OPTIONS: {
        button.setButtonActive(buttonOptions);
        frame.setFrameLabel(frameSettings, "Settings - options");
        break;
    }
    case STATE_LOADMAP: {
        button.setButtonActive(buttonLoadMap);
        frame.setFrameLabel(frameSettings, "Settings - load map");
        break;
    }
    case STATE_NEWMAP: {
        button.setButtonActive(buttonNewMap);
        frame.setFrameLabel(frameSettings, "Settings - create map");
        break;
    }
    case STATE_SAVEMAP: {
        button.setButtonActive(buttonSaveMap);
        frame.setFrameLabel(frameSettings, "Settings - save map");
        break;
    }
    case STATE_PARTICLES: {
        button.setButtonActive(buttonParticles);
        frame.setFrameLabel(frameSettings, "Settings - particles");
        break;
    }
    }

    // Normalize the mouse coordinates (GUI relative)
    if (mouse_x > 4 && mouse_y > TILESIZE + 4 && mouse_x < 768 - 4 && mouse_y < TILESIZE * 19 + TILESIZE/2 - 4) mouse_frame = MAIN_FRAME;
    else if (mouse_x > 768+4 && mouse_y > TILESIZE + 4 && mouse_x < SCREEN_W - 4 && mouse_y < TILESIZE * 19 + TILESIZE / 2 - 4) mouse_frame = TSET_FRAME;
    else if (mouse_x > 0 && mouse_y > TILESIZE * 19 + TILESIZE+4 && mouse_x < 128-4 + TILESIZE / 2 && mouse_y < SCREEN_H - 4) mouse_frame = MINI_FRAME;
    else mouse_frame = NO_FRAME;

    switch (mouse_frame) {
    case MAIN_FRAME: {
        short normX = mouse_x;
        short normY = mouse_y - TILESIZE;

        gui_x = normX;
        gui_y = normY;
        break;
    }
    case TSET_FRAME: {
        short normX = mouse_x - 768;
        short normY = mouse_y - TILESIZE;

        gui_x = normX;
        gui_y = normY;
        break;
    }
    case MINI_FRAME: {
        short normX = mouse_x;
        short normY = mouse_y - TILESIZE * 19 - TILESIZE;

        gui_x = normX;
        gui_y = normY;
        break;
    }
    }

    // Happy code -> auto_increment ID
    button.setButtonActive(buttonLayerOne+currentLayer);
    button.setButtonActive(buttonDraw+brush);

    // Checkbox behaviour mixed with radio-button behaviour
    if (!preview) {
        grid = checkbox.getCheckboxState(checkboxGrid);
        collision = checkbox.getCheckboxState(checkboxCollision);
    }
    preview = checkbox.getCheckboxState(checkboxPreview);
    layers = checkbox.getCheckboxState(checkboxLayers);
    if (!layers) {
        checkbox.setCheckboxState(checkboxAlpha, CHECKBOX_UNCHECKED);
        alpha = false;
    } else {
        alpha = checkbox.getCheckboxState(checkboxAlpha);
    }

    // Change some titles here and there
    frame.setFrameLabel(frameAll, editor.getCurrentMap());
    frame.setFrameLabel(frameTset, tileset.getTilesetName());
}

void GuiMain::drawInterface(BITMAP *bmp) {

    // Simple draw method
    frame.drawFrames(bmp);

    short stat_x = TILESIZE*(editor.current_tile/TILESIZE);
    short stat_y = TILESIZE*(editor.current_tile%TILESIZE);

    rect (bmp, SCREEN_W-250, SCREEN_H-102, SCREEN_W-250+35, SCREEN_H-102+35, makecol(0,0,0));
    masked_blit((BITMAP*)editor.mapData[TILES1+editor.mouse_tileset].dat, bmp, stat_x, stat_y, SCREEN_W - 248, SCREEN_H - 100 ,TILESIZE,TILESIZE);

/*
    textprintf_ex(bmp, font, SCREEN_W-210, SCREEN_H-102+5, makecol(0,0,0),-1, "Current tile: %d",editor.current_tile);
    textprintf_ex(bmp, font, SCREEN_W-210, SCREEN_H-102+20, makecol(0,0,0),-1, "Current tileset: %d",editor.mouse_tileset);

    stat_x = 8 + text_length(font, "Canvas. Stats: ");
    stat_y = 24;

    if (mouse_frame == TSET_FRAME) {
        textprintf_ex(bmp, font, stat_x, stat_y, makecol(255,255,255), -1, "Tileset frame: X %d | Y %d", gui_x, gui_y);
    } else if (mouse_frame == MINI_FRAME) {
        textprintf_ex(bmp, font, stat_x, stat_y, makecol(255,255,255), -1, "Minimap frame: X %d | Y %d", gui_x, gui_y);
    } else if (mouse_frame == MAIN_FRAME) {
        textprintf_ex(bmp, font, stat_x, stat_y, makecol(255,255,255), -1, "Canvas frame: Tile X %d | Tile Y %d", gui_x/32+editor.viewport.scroll_x, gui_y/32+editor.viewport.scroll_y);
    } else textprintf_ex(bmp, font, stat_x, stat_y, makecol(255,255,255), -1, "No frame");
*/
    minimap.drawMiniMap(bmp);

    button.drawButtons(bmp);
    label.drawLabels(bmp);
    field.drawFields(bmp);
    checkbox.drawCheckboxes(bmp);
}

void GuiMain::clearInterface() {
    //destroy_bitmap(brush_bmp);
}
