#include <iostream>
#include <vector>
#include <thread>
#define MINIAUDIO_IMPLEMENTATION
#include "../include/miniaudio/miniaudio.h"

#include "xengine/log.hpp"
#include "../include/audio.hpp"

namespace XEngine {

    constexpr auto MAX_DEVICES = 2;
    constexpr auto MAX_SOUNDS = 32;

    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;
    bool AudioManager::m_is_active = false;

    void AudioManager::initialize() {
        //Initialize library and report version.
        m_is_active = true;
        LOG_INFO("MiniAudio initialized.");
    }

    void AudioManager::print_host_info() {
        //deviceConfig = ma_device_config_init(ma_device_type_playback);
        ////Input debug info.
        //printf("Audio | MiniAudio\n");
        //printf("    Input device #%d:\n", *deviceConfig.capture.pDeviceID);
        //printf("        Channels: %g s\n", deviceConfig.capture.channels);
        //printf("        Format: %g s\n", deviceConfig.capture.format);
        ////Output debug info.
        //printf("    Output device #%d:\n", *deviceConfig.playback.pDeviceID);
        //printf("        Channels: %g s\n", deviceConfig.playback.channels);
        //printf("        Format: %g s\n", deviceConfig.playback.format);
    }

    bool AudioManager::is_active() {
        return m_is_active;
    }

    void AudioManager::remove() {
        m_is_active = false;
        LOG_INFO("Shutting down MiniAudio.");
    }

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
        if(pDecoder == NULL) return;
        ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
        (void)pInput;
    }


    Audio::Audio(const char* t_source, bool t_loop, const AudioLayer t_layer, bool t_play_on_awake)
        : m_source(t_source), m_loop(t_loop), m_layer(t_layer) {
        if(t_play_on_awake) play();
    }

    bool Audio::is_playing() const {
        return ma_device_get_state(&device) != ma_device_state_stopped 
           && ma_device_get_state(&device) != ma_device_state_uninitialized;
    }

    int Audio::get_length() {
        ma_uint64 length;
        ma_data_source_get_length_in_pcm_frames(decoder.pBackend, &length);
        return static_cast<int>(length);
    }

    void Audio::play() {
        if(!AudioManager::is_active()) return;
        if(is_playing()) stop();
        result = ma_decoder_init_file(m_source, NULL, &decoder);
        if (result != MA_SUCCESS) {
            LOG_ERRR("Couldn't initialize decoder.");
            return;
        }
        ma_data_source_set_looping(&decoder, m_loop ? MA_TRUE : MA_FALSE);
        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = decoder.outputFormat;
        deviceConfig.playback.channels = decoder.outputChannels;
        deviceConfig.sampleRate = decoder.outputSampleRate;
        deviceConfig.dataCallback = data_callback;
        deviceConfig.pUserData = &decoder;
        if(ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
            LOG_ERRR("Failed to open playback device.");
            ma_decoder_uninit(&decoder);
            return;
        }
        ma_device_set_master_volume(&device, m_layer.volume / 100.f);
        if(ma_device_start(&device) != MA_SUCCESS) {
            ma_device_uninit(&device);
            ma_decoder_uninit(&decoder);
            LOG_ERRR("Failed to start playback device.");
            return;
        }
    }

    void Audio::stop() {
        if(ma_device_get_state(&device) != ma_device_state_started) return;
        if(ma_device_stop(&device) != MA_SUCCESS) {
            LOG_ERRR("Failed to stop playback device.");
            return;
        }
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
    }

    void Audio::remove() {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
    }

    void Audio::set_source(const char* t_source) {
        stop();
        m_source = t_source;
    }

}