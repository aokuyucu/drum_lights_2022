// drum_config.h

#ifndef _DRUM_CONFIG_H
#define _DRUM_DRUM_CONFIG_H

enum drumID {
  black_pawn,
  white_pawn
};

//Define timing for flashes, all in milliseconds
uint8_t ShortFlash_OnMS = 100;
uint8_t ShortFlash_OffMS = 25;
uint8_t MediumFlash_OnMS = 300;
uint8_t MediumFlash_OffMS = 25;
uint8_t LongFlash_OnMS = 500;
uint8_t LongFlash_OffMS = 50;

#endif  // _DRUM_DRUM_CONFIG_H
