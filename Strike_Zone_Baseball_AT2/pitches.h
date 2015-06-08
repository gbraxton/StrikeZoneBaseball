/*
 * pitches.h
 *
 * Created: 5/31/2013 1:50:58 AM
 *  Author: George
 */ 


#ifndef PITCHES_H_
#define PITCHES_H_

enum Pitches{fastball, changeup, slider, curveball};

extern const unsigned char pitch_path_modifier[];
extern const unsigned char pitch_speeds[];
extern const unsigned char* str_pitches[];
extern const unsigned char pitches_difficulty[];
extern const unsigned char num_pitches;

#endif /* PITCHES_H_ */