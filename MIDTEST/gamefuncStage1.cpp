#include "gamefunc.h"
#include "gamefuncIntro.h"
#include "gamefuncEnding.h"
#include "gamefuncStage1.h"
#include <windows.h>
#include <queue>
#include "SDL.h"
#include <atlstr.h>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;

int g_input;
int g_elapsed_time_ms;
Uint32 g_start_time_ms = 0;

//비행기
int f_state;
int f_x;
int f_y;
string f_output;

//미사일
int m_state;
int m_x;
int m_y;
string m_output;

//미사일 배열의 인덱스
bool bullet[30];
int cnt;
short m_pos[30][2];

int score;

SDL_Rect g_destination_rect; // 비행기 위치
SDL_Rect g_source_rect; // 비행기 잘라올 곳의 사각형
SDL_Texture* g_airplane_texture; // 비행기 복사받을 곳

SDL_Rect g_bullet_destination_rect[30]; // 미사일의 현재 위치
SDL_Rect g_bullet_source_rect; // 미사일 잘라올 곳의 사각형
SDL_Texture* g_bullet_texture; //미사일 복사받을 곳

SDL_Rect g_bg_destination_rect; // 배경
SDL_Rect g_bg_source_rect; // 배경 잘라올 곳의 사각형
SDL_Texture* g_bg_texture; //배경 복사받을 곳

Mix_Music* g_bg_mus;
Mix_Chunk* g_missile_sound;

//점수 :
SDL_Texture* g_score_kr;
SDL_Rect g_score_kr_rect;

//score
SDL_Texture* g_scoreN_kr;
SDL_Rect g_scoreN_kr_rect;

//3초카운트
SDL_Texture* g_count;
SDL_Rect g_count_rect;

void printscore(int score) {

    //점수를 문자열로 반환해서 5자리보다 작으면 0채워주기
    string now_score = to_string(score);
    string print_score = string(5 - now_score.length(), '0') + now_score;

    TTF_Font* font1 = TTF_OpenFont("../resources/DungGeunMo.ttf", 30);
    string text = print_score;
    SDL_Color red = { 255, 0, 0, 0 }; //RGB, 투명도
    SDL_Surface* tmp_surface2 = TTF_RenderUTF8_Blended(font1, text.c_str(), red);

    //score
    g_scoreN_kr_rect.x = 0;
    g_scoreN_kr_rect.y = 0;
    g_scoreN_kr_rect.w = tmp_surface2->w;
    g_scoreN_kr_rect.h = tmp_surface2->h;

    g_scoreN_kr = SDL_CreateTextureFromSurface(g_renderer, tmp_surface2);

    SDL_FreeSurface(tmp_surface2);
}

//프로그램 시작 시 처음 한번 실행되는 함수
Stage1::Stage1()
{
    g_input = 0;
    g_elapsed_time_ms = 0;
    g_flag_running = true;

    score = 0;
    cnt = 0;

    //비행기그림
    SDL_Surface* airplane_surface = IMG_Load("../resources/airplane_image.png");
    // SDL_SetColorKey(airplane_surface, SDL_TRUE, SDL_MapRGB(airplane_surface->format, 0, 0, 0)); //옆 배경 투명색으로
    g_airplane_texture = SDL_CreateTextureFromSurface(g_renderer, airplane_surface);
    SDL_FreeSurface(airplane_surface);

    g_source_rect = { 0, 0, 512, 512 };
    g_destination_rect = { 360, 500, 60, 50 };

    //배경그림
    SDL_Surface* bg_surface = IMG_Load("../resources/bg.jpg");
    g_bg_texture = SDL_CreateTextureFromSurface(g_renderer, bg_surface);
    SDL_FreeSurface(bg_surface); //필요없울듯??!

    g_bg_source_rect = { 0, 0, 600, 900 };
    g_bg_destination_rect = { 0, 0, 800, 600 };

    //미사일
    g_bullet_source_rect = { 0, 0, 640, 1280 };
    SDL_Surface* bullet_surface = IMG_Load("../resources/bullet.png");
    g_bullet_texture = SDL_CreateTextureFromSurface(g_renderer, bullet_surface);
    SDL_FreeSurface(bullet_surface);

    //폰트
    TTF_Font* font1 = TTF_OpenFont("../resources/DungGeunMo.ttf", 30);
    SDL_Color black = { 0, 0, 0, 0 };
    SDL_Surface* tmp_surface = TTF_RenderUTF8_Blended(font1, CW2A(L"점수 : ", CP_UTF8), black);

    //점수:
    g_score_kr_rect.x = 0;
    g_score_kr_rect.y = 0;
    g_score_kr_rect.w = tmp_surface->w;
    g_score_kr_rect.h = tmp_surface->h;

    g_score_kr = SDL_CreateTextureFromSurface(g_renderer, tmp_surface);
    printscore(0);

    //브금은 여기서 플레이, wav는 미사일 발사하면 플레이
    g_bg_mus = Mix_LoadMUS("../resources/bgm.mp3");
    Mix_VolumeMusic(100);
    Mix_PlayMusic(g_bg_mus, -1);
    g_missile_sound = Mix_LoadWAV("../resources/missile.wav");

    //메모리해제
    SDL_FreeSurface(tmp_surface);
    TTF_CloseFont(font1);
    system("cls");
}

Stage1::~Stage1() {
}

void Stage1::Update() {
    if (f_state == 1) {
        g_destination_rect.x -= 10;
    }
    else if (f_state == 2) {
        g_destination_rect.x += 10;
    }
    else if (f_state == 3) {
        g_destination_rect.y -= 10;
    }
    else if (f_state == 4) {
        g_destination_rect.y += 10;
    }
    else {
        g_destination_rect.x = g_destination_rect.x;
        g_destination_rect.y = g_destination_rect.y;
    }

    //범위바깥으로 나가지못하게하기
    if (g_destination_rect.x > 740)
        g_destination_rect.x = 740;
    else if (g_destination_rect.x < 0)
        g_destination_rect.x = 0;

    if (g_destination_rect.y > 550)
        g_destination_rect.y = 550;
    else if (g_destination_rect.y < 0)
        g_destination_rect.y = 0;

    g_elapsed_time_ms += 33;
}

void Stage1::Render() {
    //배경
    SDL_RenderCopy(g_renderer, g_bg_texture, &g_bg_source_rect, &g_bg_destination_rect);

    //비행기
    SDL_RenderCopy(g_renderer, g_airplane_texture, &g_source_rect, &g_destination_rect);

    // 미사일 그리기
    for (int i = 0; i < 30; i++) {
        if (bullet[i]) {
            if (g_bullet_destination_rect[i].y < 0) bullet[i] = false;

            g_bullet_destination_rect[i].y -= 10;  //속도
            SDL_RenderCopy(g_renderer, g_bullet_texture, &g_bullet_source_rect, &g_bullet_destination_rect[i]);
        }
    }

    //점수 : (위치)
    SDL_Rect tmp_r;
    tmp_r.x = 15;
    tmp_r.y = 15;
    tmp_r.w = g_score_kr_rect.w;
    tmp_r.h = g_score_kr_rect.h;
    SDL_RenderCopy(g_renderer, g_score_kr, &g_score_kr_rect, &tmp_r);

    //score (위치)
    SDL_Rect tmp_r2;
    tmp_r2.x = 130;
    tmp_r2.y = 16;
    tmp_r2.w = g_scoreN_kr_rect.w;
    tmp_r2.h = g_scoreN_kr_rect.h;
    SDL_RenderCopy(g_renderer, g_scoreN_kr, &g_scoreN_kr_rect, &tmp_r2);

    //321 카운트다운위치
    SDL_Rect tmp_r3;
    tmp_r3.x = 500;
    tmp_r3.y = 500;
    tmp_r3.w = g_count_rect.w;
    tmp_r3.h = g_count_rect.h;

    SDL_RenderPresent(g_renderer);
}

void Stage1::HandleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {

        case SDL_QUIT:
            g_flag_running = false;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_LEFT) {
                f_state = 1;
            }
            else if (event.key.keysym.sym == SDLK_RIGHT) {
                f_state = 2;
            }
            else if (event.key.keysym.sym == SDLK_UP) {
                f_state = 3;
            }
            else if (event.key.keysym.sym == SDLK_DOWN) {
                f_state = 4;
            }
            else if (event.key.keysym.sym == SDLK_SPACE) {
                bullet[cnt] = true;
                g_bullet_destination_rect[cnt] = { g_destination_rect.x + 15, g_destination_rect.y + 20 - 50, 30, 30 }; //미사일 위치, 크기조정
                m_pos[cnt][1] = f_y;
                cnt++;
                score += 10;
                printscore(score);
                Mix_PlayChannel(1, g_missile_sound, 0);
                if (cnt > 29) cnt = 0;
            }
            break;

        case SDL_KEYUP:
            f_state = 0;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                score = 0;
                g_start_time_ms = SDL_GetTicks();

                SDL_Texture* g_count;
                SDL_Rect g_count_rect;
                SDL_Color orange = { 255, 127, 40, 0 };
                TTF_Font* count = TTF_OpenFont("../resources/DungGeunMo.ttf", 70);

                int count_down = 3;
                while (count_down > 0) {
                    SDL_Surface* count_surface = TTF_RenderUTF8_Blended(count, to_string(count_down).c_str(), orange);
                    g_count = SDL_CreateTextureFromSurface(g_renderer, count_surface);
                    g_count_rect = { 0, 0, count_surface->w, count_surface->h };
                    
                    //321 카운트다운위치
                    SDL_Rect tmp_r3;
                    tmp_r3.x = 380;
                    tmp_r3.y = 250;
                    tmp_r3.w = g_count_rect.w;
                    tmp_r3.h = g_count_rect.h;

                    //점수불러오기 위해 다시 저장
                    SDL_Rect tmp_r;
                    tmp_r.x = 15;
                    tmp_r.y = 15;
                    tmp_r.w = g_score_kr_rect.w;
                    tmp_r.h = g_score_kr_rect.h;

                    //score 점수 불러오기 위해 다시 저장
                    SDL_Rect tmp_r2;
                    tmp_r2.x = 130;
                    tmp_r2.y = 16;
                    tmp_r2.w = g_scoreN_kr_rect.w;
                    tmp_r2.h = g_scoreN_kr_rect.h;

                    SDL_RenderCopy(g_renderer, g_bg_texture, nullptr, nullptr);
                    SDL_RenderCopy(g_renderer, g_airplane_texture, &g_source_rect, &g_destination_rect);
                    //SDL_RenderCopy(g_renderer, g_bullet_texture, nullptr, nullptr);
                    SDL_RenderCopy(g_renderer, g_score_kr, &g_score_kr_rect, &tmp_r);
                    SDL_RenderCopy(g_renderer, g_scoreN_kr, &g_scoreN_kr_rect, &tmp_r2);
                    SDL_RenderCopy(g_renderer, g_count, &g_count_rect, &tmp_r3);

                    SDL_RenderPresent(g_renderer); // 렌더러를 화면에 보여줍니다.
                    SDL_FreeSurface(count_surface);
                    SDL_DestroyTexture(g_count);
                    system("cls");

                    this_thread::sleep_for(chrono::seconds(1));
                    count_down--;
                }

                TTF_CloseFont(count);

                if (SDL_GetTicks() - g_start_time_ms >= 3000)
                {
                    g_current_game_phase = PHASE_ENDING;

                    //stage가 끝나고 점수를 00000으로 초기화, 비행기와 미사일도 원래 위치로
                    printscore(0);
                    g_destination_rect = { 360, 500, 60, 50 };
                    for (int i = 0; i < 30; i++) {
                        g_bullet_destination_rect[i].y = -100;
                    }
                }
            }
            break;

        default:
            break;
        }
    }
}

void Stage1::Clear()
{
    SDL_DestroyTexture(g_bg_texture);
    SDL_DestroyTexture(g_airplane_texture);
    SDL_DestroyTexture(g_bullet_texture);
    Mix_FreeMusic(g_bg_mus);
    Mix_FreeChunk(g_missile_sound);
    SDL_DestroyTexture(g_score_kr);
    SDL_DestroyTexture(g_scoreN_kr);
}