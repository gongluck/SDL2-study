#include "Csdl2.h"
#include <iostream>
#include <fstream>
#include <Windows.h>

#define TESTCHECKRET(ret)\
if(!ret)\
{\
    std::cerr << err << std::endl;\
    std::cout << "input to end." << std::endl;\
    getchar();\
    return SDL_Error(SDL_LASTERROR);\
}

Csdl2 g_test;
void SDLCALL SDL_AudioCB(void* userdata, Uint8* stream, int len)
{
    static std::ifstream f("in.pcm", std::ios::binary);
    SDL_memset(stream, 0, len);
    void* buf = malloc(len);
    f.read((char*)buf, len);
    SDL_MixAudio(stream, (const Uint8*)buf, len, SDL_MIX_MAXVOLUME);
    free(buf);
    if (f.eof())
    {
        std::cout << "end" << std::endl;
        f.close();
        std::string err;
        g_test.stop_audio(err);
    }    
}

int main(int argc, char* argv[])
{
    std::string err;
    RECT rect = { 0 };
    SDL_Point p = { 0, 50 };
    std::ifstream file("in.rgb", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "open file failed " << std::endl;
        getchar();
        return 0;
    }
    int size = 320 * 240 * 3;
    void* buf = malloc(size);

    file.read(static_cast<char*>(buf), size);

    TESTCHECKRET(g_test.global_init(SDL_INIT_VIDEO | SDL_INIT_AUDIO, err));

    HWND hwnd = CreateWindow(TEXT("static"), TEXT("test csdl2"), WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 500, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    //SDL_Window* hwnd = SDL_CreateWindow("test csdl2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, SDL_WINDOW_SHOWN);
    if (hwnd == nullptr)
    {
        std::cerr << "create window failed " << GetLastError() << std::endl;
        goto END;
    }
    TESTCHECKRET(g_test.set_window(hwnd, err));
    TESTCHECKRET(g_test.set_pix_fmt(SDL_PIXELFORMAT_RGB24, 320, 240, err));
    TESTCHECKRET(g_test.render(buf, size / 240, nullptr, 2, &p, SDL_FLIP_NONE, err));
    std::cout << "render succeed." << std::endl;

    TESTCHECKRET(g_test.set_audio_fmt(44100, AUDIO_F32, 2, 1024, SDL_AudioCB, nullptr, err));
    TESTCHECKRET(g_test.start_audio(err));
    std::cout << "open audio succeed." << std::endl;

END:
    std::cout << "input to end." << std::endl;
    getchar();
    TESTCHECKRET(g_test.clear_pix_fmt(err));
    TESTCHECKRET(g_test.detach_window(err));
    if (hwnd != nullptr)
    {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
    TESTCHECKRET(g_test.stop_audio(err));
    TESTCHECKRET(g_test.global_uninit(err));
    if (buf != nullptr)
    {
        free(buf);
        buf = nullptr;
    }
    return 0;
}