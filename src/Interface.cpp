#include <Defines.h>
#include <SimplyFlat.h>

SimplyFlat::t_Interface::t_Interface()
{
    for (uint32 i = 0; i < KEY_COUNT; i++)
    {
        keys[i] = false;
        handlers[i] = NULL;
    }

    for (uint32 i = 0; i < MOUSE_BUTTON_MAX; i++)
        mousebuttons[i] = false;

    mousehandler = NULL;
}

SimplyFlat::t_Interface::~t_Interface()
{
}

void SimplyFlat::t_Interface::HookEvent(uint16 key, void (*handler)(uint16,bool))
{
    if (handler == NULL)
        return;

    if (key > KEY_COUNT)
        return;

    handlers[key] = handler;
}

void SimplyFlat::t_Interface::KeyEvent(uint16 key, bool press)
{
    if (key > KEY_COUNT)
        return;

    if (keys[key] != press)
    {
        keys[key] = press;

        if (handlers[key] != NULL)
            (handlers[key])(key,press);
    }
}

void SimplyFlat::t_Interface::MouseEvent(bool left, bool press)
{
    mousebuttons[left?MOUSE_BUTTON_LEFT:MOUSE_BUTTON_RIGHT] = press;

    if (mousehandler)
        mousehandler(left,press);
}

void SimplyFlat::t_Interface::HookMouseEvent(void (*handler)(bool, bool))
{
    mousehandler = handler;
}