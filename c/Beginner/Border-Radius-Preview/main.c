#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
const int MAX_POINTS = 128;

typedef struct {
    SDL_FRect bg_bar;
    SDL_FRect cursor;
    SDL_FRect bg_current_value;
} BarLocale;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_TextEngine* engine;
    TTF_Text* texts[4];
    int playing;
    int active_bar;
    BarLocale bars[4];
    SDL_FRect box;
    float tl;
    float tr;
    float br;
    float bl;
} AppState;

static SDL_Texture* create_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if(!surface) {
        SDL_Log("Erro ao carregar imagem: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(!texture) {
        SDL_Log("Erro ao carregar textura: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(texture);
        return NULL;
    }

    SDL_DestroySurface(surface);
    return texture;
}

void slider_mouse_down(AppState *state, float mouse_x, float mouse_y) {
    for(uint8_t bar = 0; bar < 4; ++bar) {
        SDL_FRect cursor = state->bars[bar].cursor;

        if(mouse_x >= cursor.x &&
            mouse_x <= cursor.x + cursor.w &&
            mouse_y >= cursor.y &&
            mouse_y <= cursor.y + cursor.h) {
            state->active_bar = bar;
            break;
        }
    }
}

void slider_mouse_motion(AppState *state, float mouse_x, float mouse_y) {
    BarLocale* bar = &state->bars[state->active_bar];

    float new_x = mouse_x - bar->cursor.w / 2;

    float min_x = bar->bg_bar.x - bar->cursor.w / 2;
    float max_x = bar->bg_bar.x + bar->bg_bar.w - bar->cursor.w / 2;

    if(new_x < min_x)
        new_x = min_x;

    if(new_x > max_x)
        new_x = max_x;

    bar->cursor.x = new_x;

    bar->bg_current_value.w = (bar->cursor.x + bar->cursor.w / 2) - bar->bg_bar.x;
}

void slider_mouse_up(AppState *state) { state->active_bar = -1; }

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState* state = (AppState*)appstate;
    switch(event->type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            slider_mouse_down(state, event->button.x, event->button.y);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if(state->active_bar == -1) break;
            slider_mouse_motion(state, event->motion.x, event->motion.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            slider_mouse_up(state);
            break;
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState* state = (AppState*)appstate;

    if(state) {
        for(int i = 0; i < 4; i++) {
            TTF_DestroyText(state->texts[i]);
            state->texts[i] = NULL;
        }

        TTF_CloseFont(state->font);
        state->font = NULL;
        TTF_DestroyRendererTextEngine(state->engine);
        state->engine = NULL;
        SDL_DestroyRenderer(state->renderer);
        state->renderer = NULL;
        SDL_DestroyWindow(state->window);
        state->window = NULL;
        SDL_free(state);
    }

    TTF_Quit();
    SDL_Quit();
}

void add_arc(SDL_FPoint *points, int* count, float cx, float cy, float radius, float start, float end) {
    const int segments = 16;

    if(radius <= 0)
        return;

    for(int i = 0; i <= segments; ++i) {
        float t = (float)i / segments;

        float angle = start + (end - start) * t;

        points[(*count)++] = (SDL_FPoint) {
            cx + SDL_cosf(angle) * radius,
            cy + SDL_sinf(angle) * radius
        };
    }
}

void draw_rounded_rect(AppState *state) {
    float max_rad = SDL_min(state->box.w, state->box.h) / 2.0f;

    if(state->tl > max_rad) state->tl = max_rad;
    if(state->tr > max_rad) state->tr = max_rad;
    if(state->bl > max_rad) state->bl = max_rad;
    if(state->br > max_rad) state->br = max_rad;

    SDL_FPoint points[MAX_POINTS];
    int point_count = 0;

    add_arc(
        points,
        &point_count,
        state->box.x + state->tl,
        state->box.y + state->tl,
        state->tl,
        SDL_PI_F,
        SDL_PI_F * 1.5f
    );

    add_arc(
        points,
        &point_count,
        state->box.x + state->box.w - state->tr,
        state->box.y + state->tr,
        state->tr,
        SDL_PI_F * 1.5f,
        SDL_PI_F * 2.0f
    );

    add_arc(
        points,
        &point_count,
        state->box.x + state->box.w - state->br,
        state->box.y + state->box.h - state->br,
        state->br,
        0.0f,
        SDL_PI_F * 0.5f
    );

    add_arc(
        points,
        &point_count,
        state->box.x + state->bl,
        state->box.y + state->box.h - state->bl,
        state->bl,
        SDL_PI_F * 0.5f,
        SDL_PI_F
    );

    SDL_Vertex verts[MAX_POINTS + 1];

    float center_x = state->box.x + state->box.w / 2.0f;
    float center_y = state->box.y + state->box.h / 2.0f;

    verts[0].position.x = center_x;
    verts[0].position.y = center_y;
    verts[0].color = (SDL_FColor){ 0, 0, 1, 1 };
    verts[0].tex_coord = (SDL_FPoint){ 0, 0 };

    for(int i = 0; i < point_count; ++i) {
        verts[i + 1].position = points[i];
        verts[i + 1].color = (SDL_FColor){ 0, 0, 1, 1 };
        verts[i + 1].tex_coord = (SDL_FPoint){ 0, 0 };
    }

    int indexes[MAX_POINTS * 3];
    int indexes_count = 0;

    for(int i = 1; i < point_count; ++i) {
        indexes[indexes_count++] = 0;
        indexes[indexes_count++] = i;
        indexes[indexes_count++] = i + 1;
    }

    indexes[indexes_count++] = 0;
    indexes[indexes_count++] = point_count;
    indexes[indexes_count++] = 1;

    SDL_RenderGeometry(
        state->renderer,
        NULL,
        verts,
        point_count + 1,
        indexes,
        indexes_count
    );
}

void update(AppState *state) {
    state->tl = (state->bars[0].cursor.x + 5) - state->bars[0].bg_bar.x;
    state->tr = (state->bars[1].cursor.x + 5) - state->bars[1].bg_bar.x;
    state->br = (state->bars[2].cursor.x + 5) - state->bars[2].bg_bar.x;
    state->bl = (state->bars[3].cursor.x + 5) - state->bars[3].bg_bar.x;

    if(state->tl <= 0) state->tl = 1;
    if(state->bl <= 0) state->bl = 1;
    if(state->tr <= 0) state->tr = 1;
    if(state->br <= 0) state->br = 1;

    char buffer[16];

    SDL_snprintf(buffer, sizeof(buffer), "%.0f px", state->tl);
    TTF_SetTextString(state->texts[0], buffer, 0);

    SDL_snprintf(buffer, sizeof(buffer), "%.0f px", state->tr);
    TTF_SetTextString(state->texts[1], buffer, 0);

    SDL_snprintf(buffer, sizeof(buffer), "%.0f px", state->br);
    TTF_SetTextString(state->texts[2], buffer, 0);

    SDL_snprintf(buffer, sizeof(buffer), "%.0f px", state->bl);
    TTF_SetTextString(state->texts[3], buffer, 0);
}

void render(AppState *state) {
    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    for(uint8_t bar = 0; bar < 4; ++bar) {
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(state->renderer, &state->bars[bar].bg_bar);
        SDL_SetRenderDrawColor(state->renderer, 144, 238, 144, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(state->renderer, &state->bars[bar].bg_current_value);
        SDL_SetRenderDrawColor(state->renderer, 211, 211, 211, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(state->renderer, &state->bars[bar].cursor);
        TTF_DrawRendererText(
            state->texts[bar],
            state->bars[bar].bg_bar.x +
            state->bars[bar].bg_bar.w + 15,
            state->bars[bar].bg_bar.y - 4
        );
    }

    SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);

    draw_rounded_rect(state);
    
    SDL_RenderPresent(state->renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState* state = (AppState*)appstate;
    update(state);
    render(state);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[]) {
    AppState* state = (AppState*)SDL_malloc(sizeof(AppState));
    *appstate = state;

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Erro ao inicializar drivers de video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!TTF_Init()) {
        SDL_Log("Erro ao inicializar drivers de texto: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 18);
    if(!state->font) {
        SDL_Log("Erro ao inicializar a fonte: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->window = SDL_CreateWindow("Border-Radius-Viewer", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if(!state->window) {
        SDL_Log("Erro ao inicializar a janela: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->renderer = SDL_CreateRenderer(state->window, NULL);
    if(!state->renderer) {
        SDL_Log("Erro ao inicializar o renderizador: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->engine = TTF_CreateRendererTextEngine(state->renderer);
    if(!state->engine) {
        SDL_Log("Erro ao inicializar o text engine: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    for(uint8_t bar = 0; bar < 4; ++bar) {
        state->bars[bar].bg_bar = (SDL_FRect){ WINDOW_WIDTH / 2 - 75, WINDOW_HEIGHT / 2 - 250 + (bar * 20) + 4, 150, 10};
        state->bars[bar].cursor = (SDL_FRect){ WINDOW_WIDTH / 2 - 5, WINDOW_HEIGHT / 2 - 252 + (bar * 20) + 4, 10, 14 };
        state->bars[bar].bg_current_value = (SDL_FRect){ WINDOW_WIDTH / 2 - 75, WINDOW_HEIGHT / 2 - 250 + (bar * 20) + 4, 
            state->bars[bar].cursor.x - (WINDOW_WIDTH / 2 - 75) + 5, 10};
    }

    for(uint8_t i = 0; i < 4; ++i){
        state->texts[i] = TTF_CreateText(state->engine, state->font, "0", 0);
        TTF_SetTextColor(
            state->texts[i],
            50,
            50,
            50,
            255
        );
    }

    state->box = (SDL_FRect) {
        WINDOW_WIDTH / 2 - 100,
        WINDOW_HEIGHT / 2 - 50,
        200, 200
    };

    state->active_bar = -1;

    return SDL_APP_CONTINUE;
}
