#ifndef GUI_H
#define GUI_H

#define HISTORY_LEN 3
#define TIMEOUT 5 // 15 seconds
#define BTN1_PIN 0
#define BTN2_PIN 21

enum people{
  NOONE,
  HUSBAND,
  WIFE,
};

enum ops{
  ADD,
  MINUS,
};

enum houseworks{
  SOMETHING,
  COOK,
  LUNDARY,
  CLEAN,
  TRASH,
};

typedef struct whoDid{
  enum people who;
  enum ops did;
  enum houseworks housework;
}WhoDid;

extern int pageID;
extern unsigned short wife;
extern unsigned short husband;
extern float battery;
extern float battery_percent;
extern uint8_t time_count;


void mainGUI(void);
void nextPage(void);
void appendNewHistory(void);
void clearData(void);
// const char* getPersonName(enum people);
// const char* getOpsName(enum ops);
// const char* getHouseworkName(enum houseworks);

#endif