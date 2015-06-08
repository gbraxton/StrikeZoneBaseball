/*
 * pitches.c
 *
 * Created: 5/31/2013 1:51:12 AM
 *  Author: George
 */ 

#include "pitches.h"

const unsigned char* str_pitches[] = {"fastball        ", "changeup        ", "slider          ", "curveball       "};
const unsigned char pitch_path_modifier[] = { 0, 0, 1, 2 };
const unsigned char pitch_speeds[] = {10, 20, 20, 30};
const unsigned char pitches_difficulty[] = { 0, 1, 2, 3 };
const unsigned char num_pitches = 4;