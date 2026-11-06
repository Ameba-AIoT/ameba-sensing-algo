# Example Description

This example shows how to run radar detect.

# HW Configuration

None

# SW configuration

1. In `component/soc/usrcfg/amebaxxx/ameba_wificfg.c`.
	```C
	wifi_user_config.ips_enable = 0;
	wifi_user_config.lps_enable = 0;
	```
2. Use menuconfig and select `CONFIG WIFI`->`Enable WIFI RADAR`.

3. Radar scene: default is `General scene`. For other scenes (e.g. M fridge) add `CONFIG_RADAR_M_FRIDGE_SCENE=y` to `prj.conf`.

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images [cmd: `./ameba.py build -a radar_detect`].
   * `Download` images to board by Ameba Image Tool.

5. For `DUT_IC`.
	1. reset `DUT_IC`

# API usage

The algorithm API is declared in `component/sensing_algo/include/wifi_radar_api.h`:

```C
struct wifi_radar_cfg cfg = {.sustained_time = 10};   /* s, 0~60, General scene only */
struct wifi_radar_result res;

wifi_radar_set_cfg(&cfg);       /* once, may be called again at any time */
wifi_radar_algo_init();         /* before radar enable */
/* for each assembled radar report */
wifi_radar_rpt_hdl(buf, aagc_gain, dagc_gain);
if (wifi_radar_get_result(&res) == RTK_SUCCESS) {
	/* one result per window (~240ms at trig_period 15ms): res.dis_trk (m), res.is_human, res.rssi (dBm) */
}
wifi_radar_algo_deinit();       /* after radar disable */
```

* `sustained_time`: after the target is lost, `dis_trk` keeps the kalman prediction for this time, then is cleared to 0.
* `dis_trk` is 0 when no track; `rssi` is -100 dBm when no near target in the window.
* All APIs must be called from the thread that calls `wifi_radar_rpt_hdl`.

# Expect result

In General scene, one result line is printed per window:

	```
	[radar] dis_trk = 1.234 m, is_human = 1, rssi = -45.678 dBm
	```

`AT+RADDBG=radar_result_debug,1` adds a verbose result log (nn_rate, near baseline power) from the radar library.

Raw report dump (`example_radar_detect_show`, disabled by default): the `wifi_radar_thread` may run and handle radar report after manually starting the radar, and will show radar header and raw data in trace tool or other uart tools at regular intervals.

	```
	--------------Radar Rpt Type 0-------------------
	[RADAR] radar header info:
	# rpt_type              = 3
	# rpt_seg_start         = 0
	# rpt_seg_end           = 0
	# bw_idx                = 1[0-70M;1-40M;2-20M]
	# chirp_width           = 3
	# chirp_num             = 14
	# frame_num             = 32
	# frame_interval        = 15
	# fft_strt_idx          = 0
	# fft_num_sub           = 44
	# channel               = 7
	# doppler_sample_num    = 128
	# isolation             = 40
	# range_leakage_dBx10   = 17
	# doppler_t2f_strt_idx  = -64 / 43 / 43
	# doppler_t2f_end_idx   = 63 / 42 / 42
	# doppler_t2f_step      = 0 / 0 / 0
	[RADAR] radar raw data: len = 11264 [rpt_len=11320]

	[0000]0019 001b 0019 0017 0013 000f 000b 0009
	[0008]000b 000f 0011 0012 0012 0011 0010 0011
	[RADAR] raw data done!
	```

# Note

It is currently used for internal radar detect.

# Supported IC

RTL8720F
