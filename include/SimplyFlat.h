#ifndef SIMPLYFLAT_MAIN_HEADER
#define SIMPLYFLAT_MAIN_HEADER

#ifdef _WIN32
  #include <windows.h>
#endif
#include <cmath>

#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <list>

#include <Defines.h>
#include <Text.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "../dep/SOIL/SOIL.h"

#ifndef _WIN32
  #include <GL/freeglut.h>
#endif

#ifndef M_PI
  #define M_PI 3.1415926f
#endif

#define KEY_COUNT 256

enum MouseButtons
{
    MOUSE_BUTTON_LEFT  = 0,
    MOUSE_BUTTON_RIGHT = 1,
    MOUSE_BUTTON_MAX
};

enum CustomImageLoadFlags
{
    IMAGELOAD_GREYSCALE  = 1 << 11,
};

#define MAKE_COLOR_RGB(r,g,b) ((uint32(r) << 24) | (uint32(uint8(g)) << 16) | (uint32(uint8(b)) << 8))
#define MAKE_COLOR_RGBA(r,g,b,a) ((uint32(r) << 24) | (uint32(uint8(g)) << 16) | (uint32(uint8(b)) << 8) | (uint8(a)))
#define GET_COLOR_R(c) uint8(c >> 24)
#define GET_COLOR_G(c) uint8(c >> 16)
#define GET_COLOR_B(c) uint8(c >> 8)
#define GET_COLOR_A(c) uint8(c)

// square vertex options boolean flags
// used i.e. for gradients
#define VERT_LU 1 << 0 // left upper
#define VERT_RU 1 << 1 // right upper
#define VERT_LL 1 << 2 // left lower
#define VERT_RL 1 << 3 // right lower
#define VERT_TOP     VERT_LU | VERT_RU
#define VERT_BOTTOM  VERT_LL | VERT_RL
#define VERT_LEFT    VERT_LU | VERT_LL
#define VERT_RIGHT   VERT_RU | VERT_RL

class SimplyFlat
{
    public:
        SimplyFlat();
        ~SimplyFlat();

#ifdef _WIN32
        bool CreateMainWindow(const char* title, uint32 width, uint32 height, uint8 colordepth, bool fullscreen = false, uint32 refreshrate = 60, LRESULT (CALLBACK *WndProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = NULL);
#else
        bool CreateMainWindow(int* orig_argc, char** orig_argv, const char* title, uint32 width, uint32 height, uint8 colordepth, bool fullscreen = false, uint32 refreshrate = 60, void (*drawingcallback)() = NULL);
        void Run();
#endif

        void DestroyMainWindow();
        void ResizeMainWindow(uint32 width, uint32 height);
        void InitDrawing();
        int32 BuildFont(const char* fontFile, uint32 height, uint16 bold = FW_DONTCARE, bool italic = false, bool underline = false, bool strikeout = false);

        uint32 GetScreenWidth() { return m_screenWidth; };
        uint32 GetScreenHeight() { return m_screenHeight; };

#ifdef _WIN32
        LRESULT CALLBACK SFWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#else
        //
#endif

        void BeforeDraw();
        void AfterDraw();

        // Interface struct
        struct t_Interface
        {
            public:
                t_Interface();
                ~t_Interface();
                bool IsKeyPressed(uint16 key) { if (key < KEY_COUNT) return keys[key]; else return false; };
                bool IsMouseButtonPressed(MouseButtons button) { return mousebuttons[button]; };
                bool HasHookedEvent(uint16 key) { if (key < KEY_COUNT) return (handlers[key] != NULL); else return true; };
                bool HasHookedMouseEvent() { return mousehandler != NULL; };
                void HookEvent(uint16 key, void (*handler)(uint16,bool));
                void HookMouseEvent(void (*handler)(bool,bool));
                void KeyEvent(uint16 key, bool press);
                void PrintableCharEvent(uint16 key);
                void SetMouseXY(uint32 x, uint32 y);
                uint32 GetMouseX();
                uint32 GetMouseY();

                void MouseEvent(bool left, bool press);

            private:
                bool keys[KEY_COUNT];
                bool mousebuttons[MOUSE_BUTTON_MAX];
                uint32 mouseXY[2];
                void (*handlers[KEY_COUNT])(uint16,bool);
                void (*mousehandler)(bool,bool);
                void (*allKeyHandler)(uint16,bool);
        } *Interface;

        // Drawing struct
        struct t_Drawing
        {
            public:
                t_Drawing();
                void DrawRectangle(int32 x, int32 y, uint32 width, uint32 height, uint32 color, uint32 texture = 0);
                void DrawCircle(uint32 center_x, uint32 center_y, float radius, uint32 color);
                void ClearColor(uint8 r, uint8 g, uint8 b);
                void PrintText(int32 fontId, int32 x, int32 y, uint8 feature, int32 wordWrapLimit, const wchar_t *fmt, ...);
                void PrintStyledText(uint32 x, uint32 y, int32 wordWrapLimit, StyledTextList* printList);

                void DrawRectangleGradient(int32 x, int32 y, uint32 width, uint32 height, uint32 colorSrc, uint32 colorDst, uint8 vertexOptions);

                uint32 fontDataMapSize() { return m_fontDataMap.size(); }
                void SetFontData(uint32 id, fontData* data) { if (fontDataMapSize() <= id) m_fontDataMap.resize(id+1); m_fontDataMap[id] = data; }

                uint32 GetTextWidth(int32 fontId, uint32 feature, const wchar_t *fmt, ...);
                uint32 GetTextAmountToWidth(int32 fontId, uint32 feature, uint32 width, const wchar_t *fmt, ...);
                uint32 GetTextAmountToWidthFromEnd(int32 fontId, uint32 feature, uint32 width, const wchar_t *fmt, ...);
                uint32 GetFontHeight(int32 fontId);

                void SetFontPrecache(bool on) { m_fontPrecache = on; };
                bool IsFontPrecaching() { return m_fontPrecache; };

                void SetPersistentScale(float scale, bool stack = false);
                float GetPersistentScale() { return m_persistentScale; };
                void ApplyPersistentScale();
                void DynamicOnetimeScale(float scale);

                template<typename T> void ApplyPersistentScaleTo(T &src, bool inversed = false);

                void PersistentScaleTempState(bool apply);

                void PushMatrix();
                void PopMatrix();

            private:
                std::vector<fontData*> m_fontDataMap;
                bool m_fontPrecache;
                float m_persistentScale;
        } *Drawing;

        struct t_TextureStorage
        {
            public:
                uint32 LoadTexture(const char* path, uint32 flags);
                uint32 GetGLTextureID(uint32 id);
                void DeleteTexture(uint32 id);

            private:
                std::vector<uint32> m_textureMap;
        } *TextureStorage;

    private:

#ifdef _WIN32
        // On Windows, we store this kind of stuff for later reuse
        HDC       hDC;
        HGLRC     hRC;
        HWND      hWnd;
        HINSTANCE hInstance;
#endif

        uint32    m_screenWidth;
        uint32    m_screenHeight;
        bool      m_fullscreen;
};

template <class T>
class SimplyFlatSingleton
{
    public:
        SimplyFlatSingleton()
        {
            m_instance = NULL;
        }
        ~SimplyFlatSingleton()
        {
            if (m_instance)
                delete m_instance;
            m_instance = NULL;
        }

        static T* instance()
        {
            if (m_instance)
                return m_instance;

            m_instance = new T;
            return m_instance;
        }

    private:
        static T* m_instance;
};

template <class T> T* SimplyFlatSingleton<T>::m_instance = NULL;

#define sSimplyFlat SimplyFlatSingleton<SimplyFlat>::instance()

#define SF sSimplyFlat
#define SFDrawing SF->Drawing
#define SFInterface SF->Interface

#endif
