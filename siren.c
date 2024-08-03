#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include <assert.h>
#include <math.h>
#include "array.h"

int main(int argc, char** argv) {
    size_t sample_rate = 48000;
    size_t length = atoi(argv[1])*sample_rate;
    drwav_int16* samples = malloc(sizeof(drwav_int16) * length);
    float freq = 200;
    for (size_t i = 0; i < length; i++) {
        samples[i] = 1000*sinf((float)(i*freq)/(float)sample_rate);
        freq = ((float)i/sample_rate)*1000+200;
    }
    drwav wav;
    drwav_data_format format = {
        .container=drwav_container_riff,
        .format=DR_WAVE_FORMAT_PCM,
        .channels=1,
        .sampleRate=sample_rate,
        .bitsPerSample=16,
    };
    assert(drwav_init_file_write(&wav, "siren.wav", &format, (void*)0)); 
    drwav_uint64 frames_written = drwav_write_pcm_frames(&wav, length, samples);
    assert(frames_written == length);
    drwav_uninit(&wav);
    return 0;
}
