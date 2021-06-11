
void state_transition_0_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_0;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}

void state_transition_1_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_1;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}

void state_transition_2_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_2;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}

void state_transition_3_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_3;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}

void state_transition_4_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_4;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}

void state_transition_5_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_5;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}


void state_transition_6_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_6;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}

void state_transition_7_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_7;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}

void state_transition_8_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

    static rng_state_t state = INIT_STATE_8;
    while (1) {
#pragma HLS pipeline II=9
        rng_state_pkt_t v;
        rng_state_t oldstate = state;
        state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
        v.data = oldstate;
        output.write(v);
    }
}
