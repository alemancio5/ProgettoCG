#ifndef PROGETTOCG_MENU_HPP
#define PROGETTOCG_MENU_HPP

// Enumeration for scene handler
enum SceneEnum {
    MENU,
    LEVEL1,
    LEVEL2
};

// External declarations for shared variables
extern bool closeApp;
extern SceneEnum currentScene;

void checkCurrentScene();

#endif //PROGETTOCG_MENU_HPP
