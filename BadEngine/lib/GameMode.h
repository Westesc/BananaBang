#pragma once
class GameMode {
public:
    enum GameStruct {
        Debug,
        Start,
        Game,
        Menu
    };
    GameMode() : currentMode(GameStruct::Debug) {}

    void setMode(GameStruct newMode) {
        currentMode = newMode;
    }

    GameStruct getMode() const {
        return currentMode;
    }
private:

    GameStruct currentMode;
};