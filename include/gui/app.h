#ifndef TINYBF_GUI_APP_H
#define TINYBF_GUI_APP_H

#include "tinybf.h"
#include "gui/terminal.h"

#define APP_INPUT_SIZE  4096
#define APP_PATH_SIZE   1024
#define APP_ERROR_SIZE  256

typedef enum AppState {
    STATE_EMPTY,
    STATE_PAUSED,
    STATE_RUNNING,
    STATE_WAITING_INPUT,
    STATE_HALTED,
}
AppState;

typedef struct App {
    char path[APP_PATH_SIZE];
    char error[APP_ERROR_SIZE];

    tinybf_byte* program;
    size_t program_size;

    size_t* code_index;
    size_t code_size;
    bool* breakpoints;

    TinyBf* bf;
    AppState state;
    bool resume_running;
    bool ignore_breakpoint;
    unsigned long long steps;

    int speed;
    double step_accum;

    int code_scroll;
    bool code_follow;

    Terminal term;

    tinybf_byte input[APP_INPUT_SIZE];
    int input_head;
    int input_len;
}
App;

extern const int app_speed_count;

void app_init(App* app);
void app_free(App* app);

void app_load(App* app, const char* path);
void app_reset(App* app);

void app_update(App* app, float frame_time);
void app_toggle_run(App* app);
void app_single_step(App* app);
bool app_is_running(const App* app);

const char* app_speed_label(const App* app);

void app_input_push(App* app, tinybf_byte value);
void app_input_pop_last(App* app);
tinybf_byte app_input_peek(const App* app, int index);

size_t app_code_position(const App* app, size_t ip);

#endif
