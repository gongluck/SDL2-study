#include "Csdl2.h"

// µÝ¹éËø
#define LOCKCSDL2() std::lock_guard<std::recursive_mutex> _lock(this->mutex_)

// ¼ì²éÍ£Ö¹×´Ì¬
#define CHECKCSDL2STOP(err) \
if(this->status_ != STOP)\
{\
    err = "status is not stop.";\
    return false;\
}
// ¼ì²éÊÓÆµÍ£Ö¹
#define CHECKCSDL2STOPV(err) \
if(this->status_ & 1 != 0)\
{\
    err = "statusv is not stop.";\
    return false;\
}
// ¼ì²éÒôÆµÍ£Ö¹
#define CHECKCSDL2STOPA(err) \
if((this->status_ >> 1) & 1 != 0)\
{\
    err = "statusa is not stop.";\
    return false;\
}
// ¼ì²éÊÓÆµÎ´Í£Ö¹
#define CHECKCSDL2NSTOPV(err) \
if(this->status_ & 1 == 0)\
{\
    err = "statusv is stop.";\
    return false;\
}
// ¼ì²éÒôÆµÎ´Í£Ö¹
#define CHECKCSDL2NSTOPA(err) \
if((this->status_ >> 1) & 1 == 0)\
{\
    err = "statusa is stop.";\
    return false;\
}

// ·µ»Ø³É¹¦
#define OPTSUCCEED()\
{\
    err = "opt succeed.";\
    return true;\
}

// ·µ»ØÊ§°Ü
#define OPTFAILED()\
{\
    err = SDL_GetError();\
    return false;\
 }

// ÅÐ¶Ï½á¹û£¬²¢·µ»Ø(±Ø¶¨ÍË³öº¯Êý!!!)
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
    LOCKCSDL2();
    CHECKCSDL2STOP(err);
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
    LOCKCSDL2();
    CHECKCSDL2STOP(err);
    SDL_Quit();
    OPTSUCCEED();
}

bool Csdl2::set_window(const void* hwnd, std::string& err)
{
    LOCKCSDL2();
    CHECKCSDL2STOPV(err);
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
    LOCKCSDL2();
    CHECKCSDL2STOPV(err);
    clear_pix_fmt(err);
    if (renderer_ == nullptr)
    {
        err = "renderer is nullptr.";
        return false;
    }
    texture_ = SDL_CreateTexture(renderer_, fmt, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (texture_ != nullptr)
    {
        status_ = static_cast<STATUS>(static_cast<unsigned int>(status_) | LOCKEDV);
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
    LOCKCSDL2();
    CHECKCSDL2NSTOPV(err);
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
    LOCKCSDL2();
    if (texture_ != nullptr)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    status_ = static_cast<STATUS>(static_cast<unsigned int>(status_) & ~LOCKEDV);
    OPTSUCCEED();
}

bool Csdl2::detach_window(std::string& err)
{
    LOCKCSDL2();
    CHECKCSDL2STOPV(err);
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

bool Csdl2::set_audio_fmt(int freq, SDL_AudioFormat fmt, Uint8 channels, Uint16 samples, SDL_AudioCallback callback, void* userdata, std::string& err)
{
    LOCKCSDL2();
    CHECKCSDL2STOPA(err);
    reqspec_ = { 0 };
    recspec_ = { 0 };
    reqspec_.freq = freq;
    reqspec_.format = fmt;
    reqspec_.channels = channels;
    reqspec_.samples = samples;
    reqspec_.callback = callback;
    reqspec_.userdata = userdata;
    if (SDL_OpenAudio(&reqspec_, &recspec_) != 0)
    {
        OPTFAILED();
    }
    else
    {
        status_ = static_cast<STATUS>(static_cast<unsigned int>(status_) | LOCKEDA);
        OPTSUCCEED();
    }
}

bool Csdl2::start_audio(std::string& err)
{
    LOCKCSDL2();
    CHECKCSDL2NSTOPA(err);
    SDL_PauseAudio(0);
    OPTSUCCEED();
}

bool Csdl2::stop_audio(std::string& err)
{
    LOCKCSDL2();
    CHECKCSDL2NSTOPA(err);
    SDL_PauseAudio(1);
    status_ = static_cast<STATUS>(static_cast<unsigned int>(status_) & ~LOCKEDA);
    OPTSUCCEED();
}