#include "RoarEngine.h"

#include <iostream>
#include <vector>
#include <string>

struct {
    int x = 0;
    int y = 0;
} state;

void init()
{
    Roar::g_Plugins.push_back("TestPlugin.dll");
}

void frame() {}

int main(int argc, char *argv) {
    Roar::AppRun(Roar::AppData{
        .init = &init,
        .frame = &frame
    });
}
