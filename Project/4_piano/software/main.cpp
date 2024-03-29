#include "cmidi/ff_bigbr.h"
#include "cmidi/makaimura.h"
#include "cmidi/rockman_dr_wily.h"
#include "cmidi/ugoku.h"
#include "firmware.hpp"

static const uint32_t FREQ_TABLE[] = {
    0, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15,
    16, 17, 18, 19, 21, 22, 23, 25, 26, 28, 29, 31,
    33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,
    65, 69, 73, 78, 82, 87, 93, 98, 104, 110, 117, 124,
    131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
    523, 554, 587, 622, 659, 699, 740, 784, 831, 880, 932, 988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2218, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
    4186, 4435, 4699, 4978, 5274, 5587, 5920, 6272, 6645, 7040, 7459, 7902,
    8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};

void play(int ch, int note);
void stop(int ch);
void multi_ch_piano();
void single_ch_piano();
void music(const uint16_t* m, uint32_t len);

void init() {
  set_irq_mask(0);
  sampling.set_hz(44'100);
  stop(0);
  stop(1);
  stop(2);
  stop(3);
  serial.baud(9600);
  serial.print(
      "  _  _         _   __  __ _  \n"
      " | \\| |_____ _| |_|  \\/  (_)__ ___ _ _  \n"
      " | .` / -_) \\ /  _| |\\/| | / _/ _ \\ ' \\ \n"
      " |_|\\_\\___/_\\_\\\\__|_|  |_|_\\__\\___/_||_|\n"
      "        32bit RISC-V microcontroller\n"
      "        https://github.com/NextMicon\n\n");
}

void loop() {
  serial.print(
      "Select:\n"
      " [1] Sownd Test\n"
      " [2] Piano\n"
      " [3] Music: Rockman\n"
      " [4] Music: Ugoku\n"
      " [5] Music: Makaimura\n"
      " [6] Music: FF\n");
  switch(serial.receive()) {
    case '1': {
      serial.print("=== Sownd Test ===\n");
      play(0, 48);
      mix.set_vol(0, 2);
      delay_ms(500);
      play(1, 52);
      mix.set_vol(1, 2);
      delay_ms(500);
      play(2, 55);
      mix.set_vol(2, 2);
      delay_ms(1500);
      stop(0);
      stop(1);
      stop(2);
      stop(3);
      serial.print("=== End ===\n");
    } break;
    case '2': {
      serial.print("=== Piano ===\n");
      single_ch_piano();
      serial.print("=== End ===\n");
    } break;
    case '3': {
      serial.print("=== Music: Rockman ===\n");
      serial.print("  /\\_/\\ \n"
                   "6/ '-' )__ \n"
                   "(    >____|| o o \n"
                   " )  /\\ \\ \n"
                   "(__)  \\_> \n");
      music(rockman_dr_wily_music, rockman_dr_wily_len);
      serial.print("=== End ===\n");
    } break;
    case '4': {
      serial.print("=== Music: Ugoku ===\n");
      serial.print(" .. (  '-')\n");
      music(ugoku_music, ugoku_len);
      serial.print("=== End ===\n");
    } break;
    case '5': {
      serial.print("=== Music: Makaimura ===\n");
      music(makaimura_music, makaimura_len);
      serial.print("=== End ===\n");
    } break;
    case '6': {
      serial.print("=== Music: FF ===\n");
      music(ff_bigbr_music, ff_bigbr_len);
      serial.print("=== End ===\n");
    } break;
    default: {
    } break;
  }
}

extern "C" uint32_t* irq(uint32_t* regs, uint32_t irqs) {
  serial.print("I N T E R R U P T !!!\n");
  return regs;
}

void play(int ch, int note) {
  switch(ch) {
    case 0: {
      ch0.freq(FREQ_TABLE[note]);
    } break;
    case 1: {
      ch1.freq(FREQ_TABLE[note]);
    } break;
    case 2: {
      ch2.freq(FREQ_TABLE[note]);
    } break;
    case 3: {
      ch3.freq(FREQ_TABLE[note]);
    } break;
    default:
      break;
  }
}

void stop(int ch) {
  switch(ch) {
    case 0: {
      ch0.stop();
    } break;
    case 1: {
      ch1.stop();
    } break;
    case 2: {
      ch2.stop();
    } break;
    case 3: {
      ch3.stop();
    } break;
    default:
      break;
  }
}

void single_ch_piano() {
  static const char* KEY_B = " \e[7m \e[0mw\e[7m \e[0me\e[7m \e[0m\e[7m \e[0mt\e[7m \e[0my\e[7m \e[0mu\e[7m \e[0m\e[7m \e[0mo\e[7m \e[0mp\e[7m \e[0m";
  static const char* KEY_W = " \e[7ma s df g h jk l ;\e[0m";
  static const char* ERASE = "\r                  \r";
  static const char* KEYS = "awsedftgyhujkolp;";
  const uint32_t N_KEY = 17;
  const uint32_t OCT_MIN = 1;
  const uint32_t OCT_MAX = 8;

  uint32_t ch = 0;
  uint32_t oct = 4;
  uint32_t note = 0;

  // --------------------------------------------------------------------------------

  mix.set_vol(0,5);

  serial.print(KEY_B).print("\n").print(KEY_W).print("\n");

  for(char cmd;;) {

    cmd = serial.receive();

    // Exit
    if(cmd == '\n') break;

    // Change Octave
    if(OCT_MIN <= cmd - '0' && cmd - '0' <= OCT_MAX) {
      oct = cmd - '0';
      serial.print(ERASE).print(oct);
    }

    // Stop note
    if(cmd == ' ') {
      stop(ch);
      serial.print(ERASE).print(oct);
    }

    // Play note
    for(int i = 0; i < N_KEY; ++i) {
      if(cmd == KEYS[i]) {
        int note = oct * 12 + i;
        play(ch, note);
        serial.print(ERASE).print(oct);
        for(int j = 0; j <= i; ++j) {
          serial.print(' ');
        }
      }
    }
  }
exit:
  stop(0);
  stop(1);
  stop(2);
  stop(3);
}

void multi_ch_piano() {
  const int N_CH = 4;
  static const char* keys = "zsxdcvgbhnjm,l.;/";
  static const char* keyboard_black = "\e[7m:\e[0m \e[7m \e[0m \e[7m \e[0m\e[7m \e[0m \e[7m \e[0m \e[7m \e[0m \e[7m \e[0m";
  static const char* keyboard_white = "\e[7m:           \e[0m";
  static const char* keyboard_black_key = "\e[7m:\e[0mS\e[7m \e[0mD\e[7m  \e[0mG\e[7m \e[0mH\e[7m \e[0mJ\e[7m \e[0m";
  static const char* keyboard_white_key = "\e[7mZ X CV B N M\e[0m";
  static const char* erase = "\r           :           :           :           :           :           :           :           :           |\r";
  static const char* tab = "           ";
  static const char* ch_name[N_CH] = {"triangle", "square1", "square2", "square3"};

  const uint32_t OCT_MIN = 1;
  const uint32_t OCT_MAX = 8;

  uint32_t ch = 0;
  uint32_t octave = 3;
  uint32_t notes[N_CH] = {0};

  for(char cmd;;) {

    // Show Keyboard
    serial.print(tab);
    for(uint32_t i = OCT_MIN; i <= OCT_MAX; ++i) serial.print(i).print(i == octave ? "-----------" : "           ");
    serial.print("|\n");

    serial.print(tab);
    for(uint32_t i = OCT_MIN; i <= OCT_MAX; ++i) serial.print(keyboard_black);
    serial.print("|\n");

    serial.print(tab);
    for(uint32_t i = OCT_MIN; i <= OCT_MAX; ++i) serial.print(keyboard_white);
    serial.print("|\n");

    for(int i = 0; i < N_CH; ++i) {
      serial.print(erase);
      serial.print(i == ch ? "> " : "  ").print(ch_name[i]).print("\r");
      if(notes[i] != 0) serial.print("\e[").print(notes[i] - 1).print("C*");
      serial.print("\n");
    }
    serial.print("\e[8F");

    // --------------------------------------------------------------------------------
    // Input Command

    cmd = serial.receive();

    // Exit
    if(cmd == '\n') break;
    // Change Octave
    if(OCT_MIN <= cmd - '0' && cmd - '0' <= OCT_MAX) octave = cmd - '0';
    // Change Channel
    if(cmd == 'q') ch = 0;
    if(cmd == 'w') ch = 1;
    if(cmd == 'e') ch = 2;
    if(cmd == 'r') ch = 3;
    if(cmd == 't') ch = 4;
    // Stop note
    if(cmd == ' ') {
      stop(ch);
      notes[ch] = 0;
    }
    // Play sownd
    for(int i = 0; i < 17; ++i) {
      if(cmd == keys[i]) {
        int note = octave * 12 + i;
        notes[ch] = note;
        play(ch, note);
      }
    }
  }
  stop(0);
  stop(1);
  stop(2);
  stop(3);
  serial.print("\n\n\n\n\n\n\n\n");
}

// Compressed MIDI
// Data Format
//          15 - 0:Delay 1:Sound
//   IF Delay
//     14 ~  0 - Delay Time [ms]
//   IF Sound
//     14 ~ 13 - Chip Sellect
//     12 ~ 11 - Channel
//     10 ~  4 - Note Number
//      3 ~  0 - Velocity
void cmidi(uint16_t data) {
  int type = data & (0b1 << 15);
  if(type) {
    int channel = (data >> 11) & 0b11;
    int note_number = (data >> 4) & 0b1111'111;
    int velocity = data & 0b1111;
    play(channel, velocity ? note_number : 0);
  } else {
    delay_ms(data);
  }
  return;
}

void music(const uint16_t* m, uint32_t len) {
  mix.set_vol(0, 4);
  mix.set_vol(1, 3);
  mix.set_vol(2, 3);
  mix.set_vol(3, 3);
  for(int i = 0; i < len; ++i) {
    uint16_t data = m[i];
    cmidi(data);
  }
}

void hoge(){}
