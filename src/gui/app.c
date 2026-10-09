#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "gui/app.h"

#define APP_FRAME_BUDGET 0.012

static const int speed_rates[] = { 1, 5, 20, 100, 1000, 10000, 100000, 1000000, 0 };
static const char* speed_labels[] = { "1/s", "5/s", "20/s", "100/s", "1k/s", "10k/s", "100k/s", "1M/s", "Máximo" };

const int app_speed_count = sizeof(speed_rates) / sizeof(speed_rates[0]);

/* Interpreter I/O */

static void app_output(void* userdata, tinybf_cell value)
{
    App* app = userdata;

    terminal_put(&app->term, value);
}

static tinybf_cell app_input(void* userdata)
{
    App* app = userdata;

    if (app->input_len == 0) {
        return 0;
    }

    tinybf_cell value = app->input[app->input_head];

    app->input_head = (app->input_head + 1) % APP_INPUT_SIZE;
    app->input_len--;

    return value;
}

void app_input_push(App* app, tinybf_byte value)
{
    if (app->input_len >= APP_INPUT_SIZE) {
        return;
    }

    app->input[(app->input_head + app->input_len) % APP_INPUT_SIZE] = value;
    app->input_len++;
}

void app_input_pop_last(App* app)
{
    if (app->input_len > 0) {
        app->input_len--;
    }
}

tinybf_byte app_input_peek(const App* app, int index)
{
    return app->input[(app->input_head + index) % APP_INPUT_SIZE];
}

/* Program lifecycle */

static bool is_opcode(tinybf_byte c)
{
    return c == '+' || c == '-' || c == '<' || c == '>' || c == '[' || c == ']' || c == '.' || c == ',';
}

static bool app_validate(App* app)
{
    long depth = 0;

    for (size_t i = 0; i < app->program_size; i++) {
        if (app->program[i] == '[') {
            if (++depth >= TINYBF_STACK_MAXINDEX) {
                snprintf(app->error, sizeof(app->error), "Aninhamento de '[' excede a pilha (posição %zu)", i);
                return false;
            }
        }
        else if (app->program[i] == ']') {
            if (--depth < 0) {
                snprintf(app->error, sizeof(app->error), "']' sem '[' correspondente (posição %zu)", i);
                return false;
            }
        }
    }

    if (depth != 0) {
        snprintf(app->error, sizeof(app->error), "%ld '[' sem ']' correspondente", depth);
        return false;
    }

    return true;
}

static void app_unload(App* app)
{
    free(app->program);
    free(app->code_index);
    free(app->breakpoints);

    app->program = NULL;
    app->program_size = 0;
    app->code_index = NULL;
    app->code_size = 0;
    app->breakpoints = NULL;
    app->error[0] = '\0';
}

void app_init(App* app)
{
    memset(app, 0, sizeof(*app));

    app->speed = app_speed_count - 1;
    app->code_follow = true;

    terminal_clear(&app->term);
}

void app_free(App* app)
{
    free(app->bf);
    app->bf = NULL;

    app_unload(app);
}

void app_reset(App* app)
{
    free(app->bf);
    app->bf = NULL;

    terminal_clear(&app->term);

    app->input_head = 0;
    app->input_len = 0;
    app->steps = 0;
    app->step_accum = 0;
    app->code_scroll = 0;
    app->code_follow = true;

    if (app->program == NULL || app->error[0]) {
        app->state = STATE_EMPTY;
        return;
    }

    app->bf = tinybf_create(app->program, app->program_size);
    app->bf->output = app_output;
    app->bf->input = app_input;
    app->bf->userdata = app;

    app->state = STATE_PAUSED;
}

void app_load(App* app, const char* path)
{
    int size = 0;
    unsigned char* data = LoadFileData(path, &size);

    app_unload(app);
    snprintf(app->path, sizeof(app->path), "%s", path);

    if (data == NULL) {
        snprintf(app->error, sizeof(app->error), "Não foi possível abrir o arquivo");
        app_reset(app);
        return;
    }

    app->program = malloc(size > 0 ? size : 1);
    memcpy(app->program, data, size);
    app->program_size = size;
    UnloadFileData(data);

    app->code_index = malloc(sizeof(size_t) * (app->program_size + 1));
    app->breakpoints = calloc(app->program_size + 1, sizeof(bool));

    for (size_t i = 0; i < app->program_size; i++) {
        if (is_opcode(app->program[i])) {
            app->code_index[app->code_size++] = i;
        }
    }

    app_validate(app);
    app_reset(app);
}

/* Execution */

static bool app_step(App* app, bool ignore_breakpoint)
{
    TinyBf* bf = app->bf;

    if (bf->ip < bf->program_size) {
        if (!ignore_breakpoint && app->breakpoints[bf->ip]) {
            app->state = STATE_PAUSED;
            return false;
        }

        if (!bf->skip_depth && bf->program[bf->ip] == ',' && app->input_len == 0) {
            app->resume_running = app->state == STATE_RUNNING;
            app->state = STATE_WAITING_INPUT;
            return false;
        }
    }

    tinybf_step(bf);

    if (!bf->running) {
        app->state = STATE_HALTED;
        return false;
    }

    app->steps++;

    return true;
}

static void app_run_frame(App* app, float frame_time)
{
    int rate = speed_rates[app->speed];
    long long count = LLONG_MAX;

    if (rate > 0) {
        app->step_accum += rate * frame_time;

        if (app->step_accum > rate) {
            app->step_accum = rate;
        }

        count = (long long) app->step_accum;
        app->step_accum -= count;
    }

    double deadline = GetTime() + APP_FRAME_BUDGET;

    for (long long i = 0; i < count; i++) {
        bool ignore = app->ignore_breakpoint;

        app->ignore_breakpoint = false;

        if (!app_step(app, ignore)) {
            break;
        }

        if ((i & 0xfff) == 0xfff && GetTime() > deadline) {
            break;
        }
    }
}

void app_update(App* app, float frame_time)
{
    if (app->state == STATE_WAITING_INPUT && app->input_len > 0) {
        if (app->resume_running) {
            app->state = STATE_RUNNING;
        }
        else {
            app->state = STATE_PAUSED;
            app_step(app, true);
        }
    }

    if (app->state == STATE_RUNNING) {
        app_run_frame(app, frame_time);
    }
}

void app_toggle_run(App* app)
{
    switch (app->state) {
        case STATE_PAUSED:
            app->state = STATE_RUNNING;
            app->ignore_breakpoint = true;
            app->step_accum = 0;
            app->code_follow = true;
            break;

        case STATE_RUNNING:
            app->state = STATE_PAUSED;
            break;

        case STATE_WAITING_INPUT:
            app->resume_running = !app->resume_running;
            break;

        default:
            break;
    }
}

void app_single_step(App* app)
{
    if (app->state != STATE_PAUSED) {
        return;
    }

    app->code_follow = true;
    app_step(app, true);
}

bool app_is_running(const App* app)
{
    return app->state == STATE_RUNNING || (app->state == STATE_WAITING_INPUT && app->resume_running);
}

const char* app_speed_label(const App* app)
{
    return speed_labels[app->speed];
}

size_t app_code_position(const App* app, size_t ip)
{
    size_t lo = 0;
    size_t hi = app->code_size;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;

        if (app->code_index[mid] < ip)
            lo = mid + 1;
        else
            hi = mid;
    }

    return lo;
}
