#ifndef STATES_H_   /* Include guard */
#define STATES_H_

enum gba_state {
  SETUP_START_SCREEN,
  WAIT_TO_START,
  SETUP_GAME,
  PLAY,
  SETUP_WIN,
  SETUP_LOSE,
  END,
  RESET
};

struct pos {
    int row;
    int col;
    int width;
    int height;
};

struct time {
    int time_seconds;
    int initial_time;
    struct pos pos;
};

struct state {
    enum gba_state gamestate;
    struct pos cars[2]; // 0 - black car, 1 - red car
    struct pos player;
    struct pos final_exam;
    struct time time_remaining;
    int win_count;
    int loss_count;
    int personal_best_time; // to complete
} stateInfo;

#define PLAYER_ROW 80
#define PLAYER_COL 25
#define BLACK_CAR_ROW 30
#define BLACK_CAR_COL 95
#define RED_CAR_ROW 30
#define RED_CAR_COL 122
#define TIME_ROW 0
#define TIME_COL 215
#define TIME_LIMIT 60
#define TIME_WIDTH 30
#define TIME_HEIGHT 30


#endif // STATES_H_