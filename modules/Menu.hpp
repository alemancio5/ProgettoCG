#ifndef PROGETTOCG_MENU_HPP
#define PROGETTOCG_MENU_HPP

// Enumeration for scene handler
enum SceneEnum {
    MENU,
    LEVEL1,
    LEVEL2
};

// External declarations for shared variables
bool closeApp = false;
SceneEnum currentScene = MENU;

void checkCurrentScene() {
    if (currentScene == MENU) {
        closeApp = true;
    } else {
        currentScene = MENU;
    }
};

#endif //PROGETTOCG_MENU_HPP
