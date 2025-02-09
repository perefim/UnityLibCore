#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#endif

#include "libretro.h"

#define UNITY_EXECUTABLE "./UnityGame.x86"  // Changed to 32-bit Unity build

static bool game_running = false;
#ifdef _WIN32
static HANDLE unity_process = NULL;
#else
static int unity_process = 0; // Ensure 32-bit compatibility
#endif

// Libretro Callbacks
static retro_environment_t environ_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_batch_t audio_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_init(void) {
    printf("[Libretro-Unity] Core Initialized\n");
}

void retro_deinit(void) {
    printf("[Libretro-Unity] Core Deinitialized\n");
    if (game_running) {
#ifdef _WIN32
        TerminateProcess(unity_process, 0);
        CloseHandle(unity_process);
#else
        kill((pid_t)unity_process, SIGTERM);
#endif
        game_running = false;
    }
}

unsigned retro_api_version(void) {
    return RETRO_API_VERSION;
}

void retro_get_system_info(struct retro_system_info* info) {
    info->library_name = "Libretro-Unity";
    info->library_version = "0.1";
    info->valid_extensions = "unity";
    info->need_fullpath = false;
    info->block_extract = false;
}

void retro_get_system_av_info(struct retro_system_av_info* info) {
    info->geometry.base_width = 640;
    info->geometry.base_height = 480;
    info->geometry.max_width = 640;
    info->geometry.max_height = 480;
    info->timing.fps = 60.0;
    info->timing.sample_rate = 44100.0;
}

void retro_set_environment(retro_environment_t cb) { environ_cb = cb; }
void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

void retro_reset(void) {
    if (game_running) {
#ifdef _WIN32
        TerminateProcess(unity_process, 0);
        CloseHandle(unity_process);
#else
        kill((pid_t)unity_process, SIGTERM);
#endif
        game_running = false;
    }
}

void retro_run(void) {
    if (!game_running) {
#ifdef _WIN32
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcess(UNITY_EXECUTABLE, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            printf("[Libretro-Unity] Failed to launch Unity game\n");
        }
        else {
            unity_process = pi.hProcess;
            game_running = true;
            printf("[Libretro-Unity] Launched Unity game\n");
        }
#else
        unity_process = fork();
        if (unity_process == 0) {
            execl(UNITY_EXECUTABLE, UNITY_EXECUTABLE, NULL);
            exit(1);
        }
        else if (unity_process > 0) {
            game_running = true;
            printf("[Libretro-Unity] Launched Unity game\n");
        }
#endif
    }
    video_cb(NULL, 640, 480, 640 * 4); // Adjusted resolution for 32-bit devices
}

size_t retro_serialize_size(void) { return 0; }
bool retro_serialize(void* data, size_t size) { return false; }
bool retro_unserialize(const void* data, size_t size) { return false; }

bool retro_load_game(const struct retro_game_info* game) {
    printf("[Libretro-Unity] Loading Unity game\n");
    return true;
}

void retro_unload_game(void) {
    if (game_running) {
#ifdef _WIN32
        TerminateProcess(unity_process, 0);
        CloseHandle(unity_process);
#else
        kill((pid_t)unity_process, SIGTERM);
#endif
        game_running = false;
    }
}

unsigned retro_get_region(void) { return RETRO_REGION_NTSC; }

void* retro_get_memory_data(unsigned id) { return NULL; }
size_t retro_get_memory_size(unsigned id) { return 0; }

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

