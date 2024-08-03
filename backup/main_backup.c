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
    drwav_int16* data;
    size_t len;
    size_t index;
} Chunk;

typedef struct {
    Chunk* data;
    size_t len;
    size_t capacity;
} Chunks;

typedef struct {
    drwav_int16* data;
    size_t len;
    size_t capacity;
} Samples;

int main(void) {
    unsigned int channels;
    unsigned int sample_rate;
    drwav_uint64 total_frame_count;
    drwav_int16* samples = drwav_open_file_and_read_pcm_frames_s16(
        "tada.wav",
        &channels,
        &sample_rate,
        &total_frame_count,
        (void*)0
    );
    assert(samples);
    printf("Read file with %d, %d, %lld:\n", channels, sample_rate, total_frame_count);
    uint64_t zeroes = 0;
    Chunks chunks = {0};
    Chunk current_chunk = {.data=samples, .len=0, .index=0};
    for (size_t i = 0; i < total_frame_count*channels; i++) {
        current_chunk.len += 1;
        if (samples[i] == 0) {
            zeroes += 1;
            append(&chunks, current_chunk);
            current_chunk.index += 1;
            current_chunk.data = samples+i;
            current_chunk.len = 0;
        }
    }
    if (samples[total_frame_count*channels-1] != 0) {
        append(&chunks, current_chunk);
    }

    printf("Zeroes %lld:\n", zeroes);
    printf("Chunks: %zu\n", chunks.len);
    shuffle(chunks.data, chunks.len);
    drwav_int16* new_samples = calloc(total_frame_count * channels, sizeof(drwav_int16));
    size_t total_len = 0;
    for(size_t i = 0; i < chunks.len; i++) {
        printf("chunks: index %zu len %zu\n", chunks.data[i].index, chunks.data[i].len);
        for (size_t j = 0; j < chunks.data[i].len; j++) {
            new_samples[total_len] = chunks.data[i].data[j];
            total_len++;
        }
    }


    printf("len: %zu, len: %zu \n", total_len, total_frame_count * channels);
    assert(total_len == total_frame_count * channels);
    drwav wav;
    drwav_data_format format = {
        .container=drwav_container_riff,
        .format=DR_WAVE_FORMAT_PCM,
        .channels=channels,
        .sampleRate=sample_rate,
        .bitsPerSample=16,
    };
    drwav_init_file_write(&wav, "tada_out.wav", &format, (void*)0); 
    drwav_uint64 frames_written = drwav_write_pcm_frames(&wav, total_frame_count, new_samples);
    assert(frames_written == total_frame_count);
    drwav_uninit(&wav);
}
