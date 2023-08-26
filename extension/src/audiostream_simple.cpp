#include "audiostream_simple.h"

#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

enum {
    // Native Godot sample rate (use AudioStreamPlaybackResampled for other values)
    MIX_RATE=44100,
    // A buffer of about 93ms (at 44100 mix rate)
    PCM_BUFFER_SIZE = 4096,
    // TODO Document this (see core implementations). Note that 4096=2^13
    MIX_FRAC_BITS = 13
};

AudioStreamSimple::AudioStreamSimple()
    : mix_rate(MIX_RATE), stereo(false), hz(639) {
}

Ref<AudioStreamPlayback> AudioStreamSimple::_instantiate_playback() const {
    Ref<AudioStreamPlaybackSimple> playback;
    playback.instantiate();
    playback->audioStream = Ref<AudioStreamSimple>(this);
    return playback;
}

void AudioStreamSimple::set_position(uint64_t p) {
    pos = p;
}

void AudioStreamSimple::_bind_methods() {
    // Required by GDCLASS macro
}

#define zeromem(to, count) memset(to, 0, count)

AudioStreamPlaybackSimple::AudioStreamPlaybackSimple()
    : active(false) {
    // TODO Is locking actually required?
    AudioServer::get_singleton()->lock();
    pcm_buffer = memalloc(PCM_BUFFER_SIZE);
    zeromem(pcm_buffer, PCM_BUFFER_SIZE);
    AudioServer::get_singleton()->unlock();
}

AudioStreamPlaybackSimple::~AudioStreamPlaybackSimple() {
    if(pcm_buffer) {
        memfree(pcm_buffer);
        pcm_buffer = NULL;
    }
}

void AudioStreamPlaybackSimple::_bind_methods() {
    // Required by GDCLASS macro
}

void AudioStreamPlaybackSimple::_start(double from_pos) {
    _seek(from_pos);
    active = true;
}

void AudioStreamPlaybackSimple::_stop() {
    active = false;
    audioStream->set_position(0);
}

void AudioStreamPlaybackSimple::_seek(double position) {
    if (position < 0) {
        position = 0;
    }

    // TODO What does this mean? What is the unit of "position"?
    // Note that set_position expects "samples"
    audioStream->set_position(uint64_t(position * audioStream->mix_rate) << MIX_FRAC_BITS);
}

bool AudioStreamPlaybackSimple::_is_playing() const {
    return active;
}

int32_t AudioStreamPlaybackSimple::_mix(AudioFrame *buffer, double rate_scale, int32_t frames) {
    ERR_FAIL_COND_V(!active, 0);

    // TODO What is the max possible value for "frames"?
    ERR_FAIL_COND_V(frames > PCM_BUFFER_SIZE, 0);

    // Generate 16 bits PCM samples in "buf"
    zeromem(pcm_buffer, PCM_BUFFER_SIZE);
    int16_t *buf = (int16_t *)pcm_buffer;
    audioStream->gen_tone(buf, frames);

    // Convert samples to Godot format (floats in [-1; 1])
    for(int i = 0; i < frames; i++) {
        float sample = float(buf[i]) / 32767.0;
        buffer[i] = { sample, sample };
    }

    return frames;
}

void AudioStreamSimple::gen_tone(int16_t *pcm_buf, int size) {
    // Normalized angular frequency: the angular increment (phase) per sample, in radians
    // See page 40 of BasicSynth (Daniel R. Mitchell), or https://dsp.stackexchange.com/a/53503
    double phaseIncrement = 2.0 * Math_PI * double(hz) / (double(mix_rate));
    for (int i = 0; i < size; i++) {
        pcm_buf[i] = 32767.0 * sin(phaseIncrement * double(pos + i));
    }
    pos += size;
}
