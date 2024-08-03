#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "array.h"
#pragma clang attribute push (__attribute__((no_sanitize("undefined"))), apply_to=function)
#include "pcg_basic.h"
#include "pcg_basic.c"
#pragma clang attribute pop
#include "shuffle.h"

#include "mergesort_void.h"

typedef struct {
    drwav_int16 left;
    drwav_int16 right;
    size_t index;
} Sample;

typedef struct {
    Sample* data;
    size_t len;
    size_t capacity;
} Samples;


typedef struct {
    drwav_int16* data;
    size_t len;
    size_t capacity;
} OutSamples;


Ordering cmp_cb(void* userdata, void* a_void, void* b_void) {
    (void)userdata;
    Sample* a = (Sample*)a_void;
    Sample* b = (Sample*)b_void;
    if (a->index == b->index) {
        return Ordering_Equal;
    }
    else if (a->index < b->index) {
        return Ordering_LessThan;
    }
    else {
        return Ordering_GreaterThan;
    }
}

typedef struct {
    OutSamples* samples;
    unsigned int channels;
} Write_CB_Userdata;

void write_cb(void* userdata_void, void* ptr_void) {
    Write_CB_Userdata* userdata = (Write_CB_Userdata*)userdata_void;
    drwav_int16* ptr = (drwav_int16*)ptr_void;
    assert(userdata->channels == 2);
    append(userdata->samples, ptr[0]);
    append(userdata->samples, ptr[1]);
}


int main(void) {
    unsigned int channels;
    unsigned int sample_rate;
    drwav_uint64 total_frame_count;
    drwav_int16* wav_samples = drwav_open_file_and_read_pcm_frames_s16(
        "goddess.wav",
        &channels,
        &sample_rate,
        &total_frame_count,
        (void*)0
    );
    assert(wav_samples);
    printf("Read file with %d channels, %d sample_rate, %lld frames:\n", channels, sample_rate, total_frame_count);

    Samples samples = {0};
    //defer(free(samples));
    assert(channels == 2);
    
    for (size_t i = 0; i < total_frame_count * channels; i += channels) {
        assert(i/channels < total_frame_count);
        Sample sample = {
            .left = wav_samples[i],
            .right = wav_samples[i+1],
            .index= i/channels,
        };
        append(&samples, sample);
    }

    shuffle(samples.data, sizeof(Sample), total_frame_count);
    
    OutSamples out_samples = {0};
    Write_CB_Userdata write_cb_userdata = {
        .samples = &out_samples,
        .channels = channels,
    };
    mergesort((void*)&write_cb_userdata, 0, samples.data, sizeof(Sample), total_frame_count, write_cb, cmp_cb);
    printf("outsamples len: %zu, wav samples_len %zu\n", out_samples.len, total_frame_count*channels);
    drwav wav;
    drwav_data_format format = {
        .container=drwav_container_riff,
        .format=DR_WAVE_FORMAT_PCM,
        .channels=channels,
        .sampleRate=sample_rate,
        .bitsPerSample=16,
    };
    drwav_init_file_write(&wav, "goddess_out.wav", &format, (void*)0); 
    drwav_uint64 frames_written = drwav_write_pcm_frames(&wav, out_samples.len/channels, out_samples.data);
    assert(frames_written == out_samples.len/2);
    drwav_uninit(&wav);
}



