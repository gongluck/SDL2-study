#ifndef __CSDL2_H__
#define __CSDL2_H__

#include <SDL.h>
#include <string>
#include <mutex>

class Csdl2
{
public:
    // 状态
    enum STATUS { STOP = 0b00, LOCKEDV = 0b01, LOCKEDA = 0b10, LOCKEDBOTH = 0b11 };

    // 全局的初始化
    bool global_init(Uint32 flags, std::string& err);
    // 全局的反初始化
    bool global_uninit(std::string& err);

    // 设置(windows)窗口
    bool set_window(const void* hwnd, std::string& err);
    // 设置图像格式(SDL_PIXELFORMAT_???)
    bool set_pix_fmt(Uint32 fmt, int w, int h, std::string& err);
    // 渲染数据，pitch是图像一行的字节大小，rect是渲染目标矩形，angle旋转角度，center旋转中心(在rect，{0,0}为左上)，flip翻转
    bool render(const void* data, int pitch, const SDL_Rect* rect, 
        const double angle, const SDL_Point* center, const SDL_RendererFlip flip, std::string& err);
    // 清理图像格式资源
    bool clear_pix_fmt(std::string& err);
    // 销毁关联资源
    bool detach_window(std::string& err);

    // 设置音频格式和处理回调
    bool set_audio_fmt(int freq, SDL_AudioFormat fmt, Uint8 channels, Uint16 samples, SDL_AudioCallback callback, void* userdata, std::string& err);
    // 开始音频播放
    bool start_audio(std::string& err);
    // 停止音频播放
    bool stop_audio(std::string& err);

private:
    STATUS status_ = STOP;
    std::recursive_mutex mutex_;

    SDL_Window* win_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;

    SDL_AudioSpec reqspec_ = { 0 };
    SDL_AudioSpec recspec_ = { 0 };
};

#endif//__CSDL2_H__