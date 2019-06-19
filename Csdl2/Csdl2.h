#ifndef __CSDL2_H__
#define __CSDL2_H__

#include <SDL.h>
#include <string>

class Csdl2
{
public:
    // ȫ�ֵĳ�ʼ��
    static bool global_init(Uint32 flags, std::string& err);
    // ȫ�ֵķ���ʼ��
    static bool global_uninit(std::string& err);

    // ����(windows)����
    bool set_window(const void* hwnd, std::string& err);
    // ����ͼ���ʽ(SDL_PIXELFORMAT_???)
    bool set_pix_fmt(Uint32 fmt, int w, int h, std::string& err);
    // ��Ⱦ���ݣ�pitch��ͼ��һ�е��ֽڴ�С��rect����ȾĿ����Σ�angle��ת�Ƕȣ�center��ת����(��rect��{0,0}Ϊ����)��flip��ת
    bool render(const void* data, int pitch, const SDL_Rect* rect, 
        const double angle, const SDL_Point* center, const SDL_RendererFlip flip, std::string& err);
    // ����ͼ���ʽ��Դ
    bool clear_pix_fmt(std::string& err);
    // ���ٹ�����Դ
    bool detach_window(std::string& err);

private:
    SDL_Window* win_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;
};

#endif//__CSDL2_H__