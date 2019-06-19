#include "Csdl2.h"

// 返回成功
#define OPTSUCCEED()\
{\
    err = "opt succeed.";\
    return true;\
}

// 返回失败
#define OPTFAILED()\
{\
    err = SDL_GetError();\
    return false;\
 }

// 判断结果，并返回(必定退出函数!!!)
#define CHECKSDLRET(ret)\
if(ret == 0)\
{\
    OPTSUCCEED();\
}\
else\
{\
    OPTFAILED();\
}

bool Csdl2::global_init(Uint32 flags, std::string& err)
{
    if (SDL_Init(flags) < 0)
    {
        OPTFAILED();
    }
    else
    {
        OPTSUCCEED();
    }
}

bool Csdl2::global_uninit(std::string& err)
{
    SDL_Quit();
    OPTSUCCEED();
}

bool Csdl2::set_window(const void* hwnd, std::string& err)
{
    detach_window(err);
    win_ = SDL_CreateWindowFrom(hwnd);
    if (win_ != nullptr)
    {
        renderer_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer_ != nullptr)
        {
            OPTSUCCEED();
        }
        else
        {
            std::string e;
            detach_window(e);
            OPTFAILED();
        }
    }
    else
    {
        OPTFAILED();
    }
}

bool Csdl2::set_pix_fmt(Uint32 fmt, int w, int h, std::string& err)
{
    clear_pix_fmt(err);
    if (renderer_ == nullptr)
    {
        err = "renderer is nullptr.";
        return false;
    }
    texture_ = SDL_CreateTexture(renderer_, fmt, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (texture_ != nullptr)
    {
        OPTSUCCEED();
    }
    else
    {
        OPTFAILED();
    }
}

bool Csdl2::render(const void* data, int pitch, const SDL_Rect* rect, 
    const double angle, const SDL_Point* center, const SDL_RendererFlip flip, 
    std::string& err)
{
    if (texture_ == nullptr || renderer_ == nullptr)
    {
        err = texture_ == nullptr ? "texture is nullptr." : "renderer is nullptr.";
        return false;
    }
    if (SDL_UpdateTexture(texture_, nullptr, data, pitch) != 0)
    {
        OPTFAILED();
    }
    else
    {
        if (SDL_RenderClear(renderer_) != 0)
        {
            OPTFAILED();
        }
        else
        {
            if (SDL_RenderCopyEx(renderer_, texture_, nullptr, rect, angle, center, flip) != 0)
            {
                OPTFAILED();
            }
            else
            {
                SDL_RenderPresent(renderer_);
                OPTSUCCEED();
            }
        }
    }
}

bool Csdl2::clear_pix_fmt(std::string& err)
{
    if (texture_ != nullptr)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    OPTSUCCEED();
}

bool Csdl2::detach_window(std::string& err)
{
    if (renderer_ != nullptr)
    {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (win_ != nullptr)
    {
        SDL_DestroyWindow(win_);
        win_ = nullptr;
    }
    OPTSUCCEED();
}
