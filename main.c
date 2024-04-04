#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gba.h"
#include "states.h"

// image:
#include "images/background.h"
#include "images/redCar.h"
#include "images/blackCar.h"
#include "images/sree.h"
#include "images/exam.h"
#include "images/crying1.h"
#include "images/crying2.h"
#include "images/crying3.h"
#include "images/freshmanhill.h"
#include "images/endcry.h"
#include "images/culc.h"
#include "images/endhappy.h"


static int frame_counter = 0;


void getTimeStringWE(char *output, int time_seconds, int hasEnding, char *endingText) { // with ending
    if (time_seconds == 0) {
        strcpy(output, "YOU MISSED THE EXAM AND FAILED!");
        return;
    }
    int minutes = time_seconds / 60;
    int seconds = time_seconds % 60;
    char formatted_string[50];

    if (seconds < 10) {
        if (hasEnding) {
            snprintf(output, sizeof(formatted_string) , "YOU %s WITH %d:0%d TIME LEFT!", endingText, minutes, seconds);
        } else {
            snprintf(output, sizeof(formatted_string) , "%d:0%d", minutes, seconds);
        }
    } else {
        if (hasEnding) {
            snprintf(output, sizeof(formatted_string) , "YOU %s WITH %d:%d TIME LEFT!", endingText, minutes, seconds);
        } else {
            snprintf(output, sizeof(formatted_string) , "%d:%d", minutes, seconds);
        }
    }
}

void getTimeString(char *output, int time_seconds) {
    getTimeStringWE(output, time_seconds, 0, "");
}

void setWinningText(char *output, int time_seconds) {
    getTimeStringWE(output, time_seconds, 1, "PASSED");
}

void setLosingText(char *output, int time_seconds) {
    getTimeStringWE(output, time_seconds, 1, "FAILED");
}

int detectCollision(struct pos *player, struct pos *object) {
    if (player->col + player->width >= object->col &&
        player->col <= object->col + object->width &&
        player->row + player->height >= object->row &&
        player->row <= object->row + object->height) {

        return 1;
    }

    return 0;
}

int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //
    REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

  // Load initial application state
    enum gba_state state = RESET;

    struct pos *black_car_data;
    struct pos *red_car_data;
    struct pos *player_data;
    struct pos *final_exam;
    struct time *time_remaining;


    while (1) {
        currentButtons = BUTTONS; // Load the current state of the buttons

        waitForVBlank();
        switch (state) {
            case SETUP_START_SCREEN:
                drawFullScreenImageDMA(gatechfreshmanhill);
                drawCenteredString(100, 0, 240, 50, "SREE IS LATE TO HIS CS 2110 FINAL!", BLACK);
                drawCenteredString(110, 0, 240, 50, "GET HIM THERE SAFELY!!", BLACK);
                drawCenteredString(130, 110, 30, 50, "PRESS START TO START GAME", BLACK);

                state = WAIT_TO_START;
                stateInfo.gamestate = WAIT_TO_START;
                break;
            case WAIT_TO_START:
                // undrawImageDMA(0, 20, CRYING1_WIDTH, CRYING1_HEIGHT - 20, )
                if (frame_counter == 0 ||
                    frame_counter == 40) {
                    frame_counter = 0;
                    // drawRectDMA(20, 190, CRYING1_WIDTH, CRYING1_HEIGHT - 20, GREEN);
                    drawImageDMA(0, 190, CRYING1_WIDTH, CRYING1_HEIGHT, crying1);
                } else if (frame_counter == 10 || 
                    frame_counter == 30) {
                    // drawRectDMA(20, 190, CRYING1_WIDTH, CRYING1_HEIGHT, GREEN);
                    drawImageDMA(0, 190, CRYING1_WIDTH, CRYING1_HEIGHT, crying2);
                } else if (frame_counter == 20) {
                    // drawRectDMA(20, 190, CRYING1_WIDTH, CRYING1_HEIGHT, GREEN);
                    drawImageDMA(0, 190, CRYING1_WIDTH, CRYING1_HEIGHT, crying3);
                }
                
                if (KEY_DOWN(BUTTON_START, currentButtons)) {
                    state = SETUP_GAME;
                    stateInfo.gamestate = SETUP_GAME;
                }

                if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
                    state = RESET;
                    stateInfo.gamestate = RESET;
                }

                frame_counter++;

                break;
            case SETUP_GAME:
                drawFullScreenImageDMA(background);

                black_car_data = &stateInfo.cars[0];
                red_car_data = &stateInfo.cars[1];
                player_data = &stateInfo.player;
                final_exam = &stateInfo.final_exam;
                time_remaining = &stateInfo.time_remaining;

                player_data->row = PLAYER_ROW;
                player_data->col = PLAYER_COL;
                player_data->width = SREE_WIDTH;
                player_data->height = SREE_HEIGHT;


                black_car_data->row = BLACK_CAR_ROW;
                black_car_data->col = BLACK_CAR_COL;
                black_car_data->width = BLACKCAR_WIDTH;
                black_car_data->height = BLACKCAR_HEIGHT;

                red_car_data->row = RED_CAR_ROW;
                red_car_data->col = RED_CAR_COL;
                red_car_data->width = REDCAR_WIDTH;
                red_car_data->height = REDCAR_HEIGHT;

                final_exam->row = randint(30, 130);
                final_exam->col = randint(140, 200);
                final_exam->width = EXAM_WIDTH;
                final_exam->height = EXAM_HEIGHT;

                time_remaining->pos.row = TIME_ROW;
                time_remaining->pos.col = TIME_COL;
                time_remaining->initial_time = TIME_LIMIT;
                time_remaining->time_seconds = TIME_LIMIT;
                time_remaining->pos.width = TIME_WIDTH;
                time_remaining->pos.height = TIME_HEIGHT;

                frame_counter = 0;


                state = PLAY;
                stateInfo.gamestate = PLAY;
                break;


            case PLAY:

                if (time_remaining->time_seconds == 0) {
                    state = SETUP_LOSE;
                    stateInfo.gamestate = SETUP_LOSE;
                }

                if (frame_counter == 60) {
                    frame_counter = 0;
                    time_remaining->time_seconds--;
                }
                frame_counter++;

                black_car_data = &stateInfo.cars[0];
                red_car_data = &stateInfo.cars[1];
                player_data = &stateInfo.player;
                final_exam = &stateInfo.final_exam;
                time_remaining = &stateInfo.time_remaining;


                if (detectCollision(player_data, black_car_data) || detectCollision(player_data, red_car_data)) {
                    state = SETUP_LOSE;
                    stateInfo.gamestate = SETUP_LOSE;
                }

                if (detectCollision(player_data, final_exam)) {
                    state = SETUP_WIN;
                    stateInfo.gamestate = SETUP_WIN;
                }

                black_car_data->row -= 1;
                red_car_data->row += 2;

                if (black_car_data->row < -25) {
                    black_car_data->row = 160;
                    black_car_data->col = BLACK_CAR_COL;
                }

                if (red_car_data->row > 177) {
                    red_car_data->row = 0;
                    red_car_data->col = RED_CAR_COL;
                }

                char win_string[10];
                char lose_string[15];

                snprintf(win_string, sizeof(win_string), "Wins: %d", stateInfo.win_count);
                snprintf(lose_string, sizeof(lose_string), "Losses: %d", stateInfo.loss_count);

                drawString(140, 192, win_string, WHITE);
                drawString(150, 180, lose_string, WHITE);



                char time_string[5];
                getTimeString(time_string, time_remaining->time_seconds);

                undrawImageDMA(time_remaining->pos.row, time_remaining->pos.col, 30, 10 , background);
                drawString(time_remaining->pos.row, time_remaining->pos.col, time_string, WHITE);

                drawImageDMA(stateInfo.cars[0].row, stateInfo.cars[0].col, BLACKCAR_WIDTH, BLACKCAR_HEIGHT, blackcar);
                undrawImageDMA(black_car_data->row + BLACKCAR_HEIGHT + 1, black_car_data->col, BLACKCAR_WIDTH, 1, background);

                drawImageDMA(stateInfo.cars[1].row, stateInfo.cars[1].col, REDCAR_WIDTH, REDCAR_HEIGHT, redcar);
                undrawImageDMA(red_car_data->row - 2, red_car_data->col, REDCAR_WIDTH, 2, background);


                if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
                    state = RESET;
                    stateInfo.gamestate = RESET;
                }

                if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
                    if (player_data->col  < 230) {
                        undrawImageDMA(player_data->row, player_data->col, 1, 10, background);
                        player_data->col++;
                    }
                }

                if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
                    if (player_data->col > 0) {
                        undrawImageDMA(player_data->row, player_data->col + 9, 2, 10, background);
                        player_data->col--;
                    }
                }

                if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
                    if (player_data->row < 150) {
                        undrawImageDMA(player_data->row, player_data->col, 10, 1, background);
                        player_data->row++;
                    }
                }

                if (KEY_DOWN(BUTTON_UP, currentButtons)) {
                    if (player_data->row > 0) {
                        undrawImageDMA(player_data->row + 9, player_data->col, 10, 2, background);
                        player_data->row--;
                    }
                }
                drawImageDMA(player_data->row, player_data->col, SREE_WIDTH, SREE_HEIGHT, sree);


                drawImageDMA(final_exam->row, final_exam->col, EXAM_WIDTH, EXAM_HEIGHT, exam);

                break;
            case SETUP_WIN:
                stateInfo.win_count++;
                drawFullScreenImageDMA(gatechculc);

                drawString(30, 63, "HIT SELECT TO RESET", WHITE);
                drawString(40, 74, "HIT A TO RESTART", WHITE);

                drawImageDMA(55, 95, ENDHAPPY_WIDTH, ENDHAPPY_HEIGHT, endhappy);

                char winningText[50];

                setWinningText(winningText, stateInfo.time_remaining.time_seconds);
                drawString(130, 35, winningText, WHITE);

                char personal_best_text[30];


                if (stateInfo.personal_best_time == TIME_LIMIT) {
                    stateInfo.personal_best_time = time_remaining->initial_time - time_remaining->time_seconds;
                    snprintf(personal_best_text, sizeof(personal_best_text) , "PERSONAL BEST: %d SECONDS", stateInfo.personal_best_time);
                    drawString(140, 40, "YOU HAVE A NEW PERSONAL BEST!", WHITE);
                    drawString(150, 50, personal_best_text, WHITE);
                } else if (time_remaining->initial_time - time_remaining->time_seconds < stateInfo.personal_best_time) {
                    snprintf(personal_best_text, sizeof(personal_best_text) , "OLD PERSONAL BEST: %d SECONDS", stateInfo.personal_best_time);                    
                    drawString(140, 40, "YOU BEAT YOUR PERSONAL BEST!", WHITE);
                    drawString(150, 37, personal_best_text, WHITE);
                    stateInfo.personal_best_time = time_remaining->initial_time - time_remaining->time_seconds;
                } else {
                    snprintf(personal_best_text, sizeof(personal_best_text) , "PERSONAL BEST: %d SECONDS", stateInfo.personal_best_time);
                    drawString(150, 50, personal_best_text, WHITE);
                }

                state = END;
                stateInfo.gamestate = END;
                break;

            case SETUP_LOSE:
                stateInfo.loss_count++;
                fillScreenDMA(BLACK);
                
                drawImageDMA(55, 95, ENDCRY_WIDTH, ENDCRY_HEIGHT, endcry);

                char losingText[50];

                setLosingText(losingText, stateInfo.time_remaining.time_seconds);

                drawString(120, 33, losingText, WHITE);
                drawString(30, 63, "HIT SELECT TO RESET", WHITE);
                drawString(40, 74, "HIT A TO RESTART", WHITE);

                state = END;
                stateInfo.gamestate = END;
                
                
                break;

            case END:

                if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
                    state = RESET;
                    stateInfo.gamestate = RESET;   
                }

                if (KEY_DOWN(BUTTON_A, currentButtons)) {
                    state = SETUP_START_SCREEN;
                    stateInfo.gamestate = SETUP_START_SCREEN;
                }

                break;
            case RESET:

                stateInfo.win_count = 0;
                stateInfo.loss_count = 0;
                stateInfo.personal_best_time = TIME_LIMIT;
                
                
                state = SETUP_START_SCREEN;
                stateInfo.gamestate = SETUP_START_SCREEN;
        }

        previousButtons = currentButtons; // Store the current state of the buttons
    }

    UNUSED(previousButtons);

    return 0;
}
