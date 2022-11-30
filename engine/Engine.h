//
// Created by Иван Ильин on 14.01.2021.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "io/Screen.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "World.h"
#include "Camera.h"
#include "utils/Log.h"

class Engine {
private:
    std::string _name;

    int _triPerSec = 0;
    bool _updateWorld = true;
    bool _showDebugInfo = Consts::SHOW_DEBUG_INFO;
    bool _useOpenGL = Consts::USE_OPEN_GL;

    void printDebugInfo() const;

protected:
    const std::shared_ptr<Screen> screen = std::make_shared<Screen>();
    const std::shared_ptr<Keyboard> keyboard = std::make_shared<Keyboard>();
    const std::shared_ptr<Mouse> mouse = std::make_shared<Mouse>(screen);

    const std::shared_ptr<World> world = std::make_shared<World>();
    const std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    virtual void start() {};

    virtual void update() {};

    [[nodiscard]] bool showDebugInfo() const { return _showDebugInfo; }
    void setDebugInfo(bool value) { _showDebugInfo = value; }

    void setUpdateWorld(bool value) { _updateWorld = value; }

    void setGlEnable(bool value) { _useOpenGL = value; }
    [[nodiscard]] bool glEnable() const { return _useOpenGL; }

    virtual void gui() {}

public:
    Engine();

    virtual ~Engine() = default;

    //АК комент: выключая вертикальную синхронизацию (verticalSync = false)- 
    // - можно увеличить FPS (сделать FPS больше чем может монитор).
    //У меня - в включенным OpenGL и включенной вертикальной синхронизацией - дает 59 FPS. 
    //А с в включенным OpenGL и выключенной вертикальной синхронизацией - дает 250 FPS.
    //Но если FPS меньше чем может монитор, то выключени вертикальной синхронизации не поднимет FPS.
    //То есть нет смысла отключать вертикальную синхронизацию.
    //Можно отключить - если хочется посмотреть какой запас по производительности (какой FPS может выдать компьютер).
    void create(int screenWidth = Consts::STANDARD_SCREEN_WIDTH, int screenHeight = Consts::STANDARD_SCREEN_HEIGHT,
                const std::string &name = Consts::PROJECT_NAME, bool verticalSync = true,
                sf::Color background = Consts::BACKGROUND_COLOR, sf::Uint32 style = sf::Style::Default);

    void exit();
};


#endif //INC_3DZAVR_TDZAVR_H
