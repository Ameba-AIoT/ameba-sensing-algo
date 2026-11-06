/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* WiFi radar algorithm public API. Typical flow (see example radar_detect):
 *   wifi_radar_set_cfg() -> wifi_radar_algo_init() -> radar enable (wifi_radar_config)
 *   -> per assembled report: wifi_radar_rpt_hdl() then wifi_radar_get_result()
 *   -> radar disable -> wifi_radar_algo_deinit()
 * All APIs must be called from the same thread that calls wifi_radar_rpt_hdl() (no locking inside). */

#ifndef __WIFI_RADAR_API_H__
#define __WIFI_RADAR_API_H__

#include "basic_types.h"

#define RADAR_SUSTAINED_TIME_MAX	60	/* s */

struct wifi_radar_cfg {
	u8 sustained_time;	/* s, 0~RADAR_SUSTAINED_TIME_MAX: hold time from target lost until dis_trk is cleared;
				 * 0 = clear as soon as the track is lost. General scene only, ignored by other scenes. */
};

struct wifi_radar_result {
	float dis_trk;		/* tracked target distance (m), 0 when no track */
	float rssi;		/* near target peak rssi (dBm), -100 when no near target in this window */
	bool is_human;		/* NN human judgement, always true without CONFIG_TFLITE_MICRO_EN */
};

/* Register the radar proc report segment callback (RTW_EVENT_RADAR_PROC_RPT), NULL to unregister.
 * Implemented by the wifi driver. */
void wifi_radar_register_report_cb(void (*cb)(u8 *evt_info));

/* Init algorithm resources (NN, scene peripherals). Call once before radar enable.
 * Returns RTK_FAIL if any part failed; radar still runs with the failed part disabled. */
s32 wifi_radar_algo_init(void);
void wifi_radar_algo_deinit(void);

/* Feed one assembled radar report (struct rtw_event_radar_proc_rpt_info + data) with its AGC gains. */
void wifi_radar_rpt_hdl(u8 *radar_buffer, float aagc_gain, float *dagc_gain_array);

/* Set algorithm config, may be called at any time, takes effect from the next window.
 * Returns RTK_FAIL (config unchanged) on NULL cfg or out-of-range value. */
s32 wifi_radar_set_cfg(const struct wifi_radar_cfg *cfg);

/* Get the latest window result (one per window, ~240ms at trig_period 15ms). Newer window overwrites
 * an unread one. Returns RTK_SUCCESS with *res filled once per window, RTK_FAIL if no new result. */
s32 wifi_radar_get_result(struct wifi_radar_result *res);

#endif /* __WIFI_RADAR_API_H__ */
