#include "MainScreen.hpp"

void MainScreen::Render() {
    display->Clear();
    display->ShowText(0, 0, "Pico Timer 2");
    display->ShowText(1, 0, "Press Encoder to");
    display->ShowText(2, 0, "Open Menu");
    display->ShowText(3, 0, "Hatikvah melody");
    display->ShowText(4, 0, "on Alarm Off");
}
