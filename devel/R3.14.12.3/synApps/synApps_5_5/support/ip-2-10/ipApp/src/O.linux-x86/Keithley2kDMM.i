# 1 "../Keithley2kDMM.st"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../Keithley2kDMM.st"
program Keithley2kDMM ("P=13LAB:, Dmm=DMM1, channels=10, model=2000")
# 37 "../Keithley2kDMM.st"
option +r;




%% #include "stdlib.h"
%% #include "string.h"

int i;
int scan_chan;
long previous_mode;
long previous_chan;

long ch_mode[22];
assign ch_mode to
           {"{P}{Dmm}ch1_mode_sel.VAL",
            "{P}{Dmm}ch2_mode_sel.VAL",
            "{P}{Dmm}ch3_mode_sel.VAL",
            "{P}{Dmm}ch4_mode_sel.VAL",
            "{P}{Dmm}ch5_mode_sel.VAL",
            "{P}{Dmm}ch6_mode_sel.VAL",
            "{P}{Dmm}ch7_mode_sel.VAL",
            "{P}{Dmm}ch8_mode_sel.VAL",
            "{P}{Dmm}ch9_mode_sel.VAL",
            "{P}{Dmm}ch10_mode_sel.VAL",
            "{P}{Dmm}ch11_mode_sel.VAL",
            "{P}{Dmm}ch12_mode_sel.VAL",
            "{P}{Dmm}ch13_mode_sel.VAL",
            "{P}{Dmm}ch14_mode_sel.VAL",
            "{P}{Dmm}ch15_mode_sel.VAL",
            "{P}{Dmm}ch16_mode_sel.VAL",
            "{P}{Dmm}ch17_mode_sel.VAL",
            "{P}{Dmm}ch18_mode_sel.VAL",
            "{P}{Dmm}ch19_mode_sel.VAL",
            "{P}{Dmm}ch20_mode_sel.VAL",
            "{P}{Dmm}ch21_mode_sel.VAL",
            "{P}{Dmm}ch22_mode_sel.VAL"};
monitor ch_mode;
evflag ch_mode_mon;
sync ch_mode ch_mode_mon;

long ch_on_off[22];
assign ch_on_off to
           {"{P}{Dmm}Ch1_on_off.VAL",
            "{P}{Dmm}Ch2_on_off.VAL",
            "{P}{Dmm}Ch3_on_off.VAL",
            "{P}{Dmm}Ch4_on_off.VAL",
            "{P}{Dmm}Ch5_on_off.VAL",
            "{P}{Dmm}Ch6_on_off.VAL",
            "{P}{Dmm}Ch7_on_off.VAL",
            "{P}{Dmm}Ch8_on_off.VAL",
            "{P}{Dmm}Ch9_on_off.VAL",
            "{P}{Dmm}Ch10_on_off.VAL",
            "{P}{Dmm}Ch11_on_off.VAL",
            "{P}{Dmm}Ch12_on_off.VAL",
            "{P}{Dmm}Ch13_on_off.VAL",
            "{P}{Dmm}Ch14_on_off.VAL",
            "{P}{Dmm}Ch15_on_off.VAL",
            "{P}{Dmm}Ch16_on_off.VAL",
            "{P}{Dmm}Ch17_on_off.VAL",
            "{P}{Dmm}Ch18_on_off.VAL",
            "{P}{Dmm}Ch19_on_off.VAL",
            "{P}{Dmm}Ch20_on_off.VAL",
            "{P}{Dmm}Ch21_on_off.VAL",
            "{P}{Dmm}Ch22_on_off.VAL"};
monitor ch_on_off;

long ch_raw[22];
assign ch_raw to
           {"{P}{Dmm}Ch1_raw.PROC",
            "{P}{Dmm}Ch2_raw.PROC",
            "{P}{Dmm}Ch3_raw.PROC",
            "{P}{Dmm}Ch4_raw.PROC",
            "{P}{Dmm}Ch5_raw.PROC",
            "{P}{Dmm}Ch6_raw.PROC",
            "{P}{Dmm}Ch7_raw.PROC",
            "{P}{Dmm}Ch8_raw.PROC",
            "{P}{Dmm}Ch9_raw.PROC",
            "{P}{Dmm}Ch10_raw.PROC",
            "{P}{Dmm}Ch11_raw.PROC",
            "{P}{Dmm}Ch12_raw.PROC",
            "{P}{Dmm}Ch13_raw.PROC",
            "{P}{Dmm}Ch14_raw.PROC",
            "{P}{Dmm}Ch15_raw.PROC",
            "{P}{Dmm}Ch16_raw.PROC",
            "{P}{Dmm}Ch17_raw.PROC",
            "{P}{Dmm}Ch18_raw.PROC",
            "{P}{Dmm}Ch19_raw.PROC",
            "{P}{Dmm}Ch20_raw.PROC",
            "{P}{Dmm}Ch21_raw.PROC",
            "{P}{Dmm}Ch22_raw.PROC"};



long dmm_modes[15];
assign dmm_modes to
           {"{P}{Dmm}conf_dcv.PROC",
            "{P}{Dmm}conf_acv.PROC",
            "{P}{Dmm}conf_ohm2w.PROC",
            "{P}{Dmm}conf_ohm4w.PROC",
            "{P}{Dmm}conf_freq.PROC",
            "{P}{Dmm}conf_tempj.PROC",
            "{P}{Dmm}conf_tempk.PROC",
            "{P}{Dmm}conf_tempn.PROC",
            "{P}{Dmm}conf_tempt.PROC",
            "{P}{Dmm}conf_tempe.PROC",
            "{P}{Dmm}conf_tempr.PROC",
            "{P}{Dmm}conf_temps.PROC",
            "{P}{Dmm}conf_tempb.PROC",
            "{P}{Dmm}conf_dci.PROC",
            "{P}{Dmm}conf_aci.PROC"};

long ch_mode_sel;
assign ch_mode_sel to "{P}{Dmm}ch_mode_sel.VAL";
monitor ch_mode_sel;
evflag ch_mode_sel_mon;
sync ch_mode_sel ch_mode_sel_mon;


long done_read;
assign done_read to "{P}{Dmm}done_read.VAL";
monitor done_read;

long dmm_chan;
assign dmm_chan to "{P}{Dmm}dmm_chan.VAL";
monitor dmm_chan;
evflag dmm_chan_mon;
sync dmm_chan dmm_chan_mon;

long dmm_chan2;
assign dmm_chan2 to "{P}{Dmm}dmm_chan2.VAL";
monitor dmm_chan2;
evflag dmm_chan2_mon;
sync dmm_chan2 dmm_chan2_mon;

long dmm_ch_range;
assign dmm_ch_range to "{P}{Dmm}dmm_ch_range.VAL";
monitor dmm_ch_range;

long dmm_delay_read;
assign dmm_delay_read to "{P}{Dmm}dmm_delay_read.PROC";

long dmm_read;
assign dmm_read to "{P}{Dmm}dmm_read.PROC";

long Dmm_raw;
assign Dmm_raw to "{P}{Dmm}Dmm_raw.PROC";

long read_complete;
assign read_complete to "{P}{Dmm}read_complete.VAL";
monitor read_complete;
evflag read_complete_mon;
sync read_complete read_complete_mon;

long single_multi;
assign single_multi to "{P}{Dmm}single_multi.VAL";
monitor single_multi;

long init_dmm;
assign init_dmm to "{P}{Dmm}init_dmm.VAL";
monitor init_dmm;
evflag init_dmm_mon;
sync init_dmm init_dmm_mon;

long do_init;
assign do_init to "{P}{Dmm}init_string.PROC";

string ch_close; assign ch_close to "{P}{Dmm}ch_close.VAL";
string model; assign model to "{P}{Dmm}model.VAL";
long channels; assign channels to "{P}{Dmm}channels.VAL";
string dmm_units; assign dmm_units to "{P}{Dmm}units.VAL";
string ch_units; assign ch_units to "";

char *unit_strings[15];
char *P;
char *Dmm;
string pvname;
string close_format;
long mode;
long chan;

ss mode_change {
    state init {
        when() {


            efSet(ch_mode_mon);
            efSet(ch_mode_sel_mon);
            efSet(dmm_chan_mon);
            unit_strings[0] = "VDC";
            unit_strings[1] = "VAC";
            unit_strings[2] = "OHM";
            unit_strings[3] = "OHM";
            unit_strings[4] = "HZ";
            unit_strings[5] = "DegC";
            unit_strings[6] = "DegC";
            unit_strings[7] = "DegC";
            unit_strings[8] = "DegC";
            unit_strings[9] = "DegC";
            unit_strings[10] = "DegC";
            unit_strings[11] = "DegC";
            unit_strings[12] = "DegC";
            unit_strings[13] = "ADC";
            unit_strings[14] = "AAC";

            P = macValueGet("P");
            Dmm = macValueGet("Dmm");
            strcpy(model, macValueGet("model"));
            pvPut(model);
            channels = atoi(macValueGet("channels"));
            pvPut(channels);
            if (strcmp(model, "2000") == 0) {
               strcpy(close_format, "rout:clos (@%d)");
            } else {
               strcpy(close_format, "rout:clos (@1%2.2d)");
            }
        } state monitor_mode_changes
    }
    state monitor_mode_changes {
        when(efTestAndClear(ch_mode_mon)) {
            for (i=0; i<channels; i++) {






                mode = ch_mode[i];



                if ((strcmp(model, "2700") == 0) && (i > 19)) {
                    mode = mode + 13;
                }
                sprintf(ch_units, "%s", unit_strings[mode]);
                sprintf(pvname, "%s%sch%d_units.VAL", P, Dmm, i+1);
                pvAssign(ch_units, pvname);
                pvPut(ch_units);
            }
        } state monitor_mode_changes

        when(efTestAndClear(ch_mode_sel_mon)) {
            sprintf(dmm_units, "%s", unit_strings[ch_mode_sel]);
            pvPut(dmm_units);
            pvPut(dmm_modes[ch_mode_sel]);
            previous_mode = ch_mode_sel;

            if (ch_mode_sel == 3) {
                sprintf(ch_close, "rout:open:all");
                pvPut(ch_close);
                previous_chan = -1;
            }
        } state monitor_mode_changes

        when(efTestAndClear(dmm_chan_mon)) {
            sprintf(ch_close, close_format, dmm_chan+1);
            pvPut(ch_close);
            previous_chan = dmm_chan;
        } state monitor_mode_changes

        when(efTestAndClear(dmm_chan2_mon) && (channels > 10)) {
            sprintf(ch_close, close_format, dmm_chan2+11);
            pvPut(ch_close);
            previous_chan = dmm_chan2+10;
        } state monitor_mode_changes

        when(efTestAndClear(init_dmm_mon)) {
            if (init_dmm == 1) {
                scan_chan = 0;
                previous_mode = -1;
                previous_chan = -1;
                init_dmm = 0;
                pvPut(init_dmm);
                pvPut(do_init);
            }
        } state monitor_mode_changes
    }
}

ss read_meter {
    state init {
        when() {
            scan_chan = 0;
            previous_mode = -1;
            previous_chan = -1;
            efClear(read_complete_mon);
            pvPut(do_init);
        } state wait_read
    }
    state wait_read {
        when(done_read == 1) {
        } state read_channel
    }
    state read_channel {
        when (single_multi == 0) {
            if (ch_mode_sel != previous_mode) {
                sprintf(dmm_units, "%s", unit_strings[ch_mode_sel]);
                pvPut(dmm_units);
                pvPut(dmm_modes[ch_mode_sel]);
            }

            chan = dmm_chan;
            if (dmm_ch_range == 1) chan = dmm_chan2+10;
            if (chan != previous_chan) {
                sprintf(ch_close, close_format, chan+1);
                pvPut(ch_close);
                previous_chan = chan;
            }
            previous_mode = ch_mode_sel;
            read_complete=0;
            pvPut(read_complete);
            efClear(read_complete_mon);
            pvPut(dmm_read);
        } state single_read_response

       when((single_multi != 0) && (ch_on_off[scan_chan] == 0)) {
            mode = ch_mode[scan_chan];



            if ((strcmp(model, "2700") == 0) && (scan_chan > 19)) {
                mode = mode + 13;
            }
            if (mode != previous_mode) {
                pvPut(dmm_modes[mode]);
                previous_mode = mode;
            }
            if (scan_chan != previous_chan) {
                sprintf(ch_close, close_format, scan_chan+1);
                pvPut(ch_close);
                previous_chan = scan_chan;
            }
            read_complete=0;
            pvPut(read_complete);
            efClear(read_complete_mon);



            if (mode == 1)
                pvPut(dmm_delay_read);
            else
                pvPut(dmm_read);
        } state multi_read_response

        when((single_multi != 0) && (ch_on_off[scan_chan] != 0)) {
        } state next_channel
    }
    state single_read_response {
        when(efTestAndClear(read_complete_mon) && read_complete) {
            pvPut(Dmm_raw);
            done_read = 0;
            pvPut(done_read);
        } state wait_read
    }
    state multi_read_response {
        when(efTestAndClear(read_complete_mon) && read_complete) {
            pvPut(ch_raw[scan_chan]);
        } state next_channel
    }
    state next_channel {
        when (scan_chan < (channels-1)) {
            scan_chan++;
        } state read_channel
        when (scan_chan >= (channels-1)) {
            scan_chan = 0;
            done_read = 0;
            pvPut(done_read);
        } state wait_read
    }
}
