#include "state.h"

#ifndef TEST
#define TEST

void all_tests();
bool test_indiv_setup();
bool test_global_setup();
bool test_add_rm_old_drum_hits();
void assert_true(bool test);
State copy_state(State st);
DrumHit copy_drum_hits(DrumHit hit);
Drum copy_drum(Drum drum);
HitQueue copy_hit_queue(HitQueue queue);
bool compare_states(State orig_state, State test_state);
bool compare_hit_queues(HitQueue orig_hits, HitQueue test_hits);
bool compare_drums(Drum orig_drum, Drum test_drum);
bool compare_drum_hits(DrumHit orig_hit, DrumHit test_hit);

#endif
