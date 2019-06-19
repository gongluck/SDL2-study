#ifndef __CSDL2_H__
#define __CSDL2_H__

#include <SDL.h>
#include <string>
#include <mutex>

class Csdl2
{
public:
    // ״̬
    enum STATUS { STOP = 0b00, LOCKEDV = 0b01, LOCKEDA = 0b10, LOCKEDBOTH = 0b11 };

    // ȫ�ֵĳ�ʼ��
    bool global_init(Uint32 flags, std::string& err);
    // ȫ�ֵķ���ʼ��
    bool global_uninit(std::string& err);

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

    // ������Ƶ��ʽ�ʹ���ص�
    bool set_audio_fmt(int freq, SDL_AudioFormat fmt, Uint8 channels, Uint16 samples, SDL_AudioCallback callback, void* userdata, std::string& err);
    // ��ʼ��Ƶ����
    bool start_audio(std::string& err);
    // ֹͣ��Ƶ����
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