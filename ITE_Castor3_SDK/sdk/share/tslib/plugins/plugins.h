﻿#define TSLIB_DECLARE_MODULE(name) \
	TSAPI struct tslib_module_info *name##_mod_init(struct tsdev *dev, const char *params)

TSLIB_DECLARE_MODULE(linear);
TSLIB_DECLARE_MODULE(dejitter);
TSLIB_DECLARE_MODULE(linear_h2200);
TSLIB_DECLARE_MODULE(variance);
TSLIB_DECLARE_MODULE(pthres);

TSLIB_DECLARE_MODULE(ucb1x00);
TSLIB_DECLARE_MODULE(corgi);
TSLIB_DECLARE_MODULE(collie);
TSLIB_DECLARE_MODULE(h3600);
TSLIB_DECLARE_MODULE(mk712);
TSLIB_DECLARE_MODULE(arctic2);
TSLIB_DECLARE_MODULE(tatung);
TSLIB_DECLARE_MODULE(input);
TSLIB_DECLARE_MODULE(cy8mrln_palmpre);

TSLIB_DECLARE_MODULE(ad7879);
TSLIB_DECLARE_MODULE(alltouch);
TSLIB_DECLARE_MODULE(ft5216);
TSLIB_DECLARE_MODULE(ft5306);
TSLIB_DECLARE_MODULE(ft5316_ucm10d);
TSLIB_DECLARE_MODULE(ft5316);
TSLIB_DECLARE_MODULE(ft5406);
TSLIB_DECLARE_MODULE(ft6336);
TSLIB_DECLARE_MODULE(ft6436);
TSLIB_DECLARE_MODULE(gt911);
TSLIB_DECLARE_MODULE(gt5688);
TSLIB_DECLARE_MODULE(icn8826);
TSLIB_DECLARE_MODULE(icnt8852);
TSLIB_DECLARE_MODULE(it7260);
TSLIB_DECLARE_MODULE(msg22s);
TSLIB_DECLARE_MODULE(sis9252);
TSLIB_DECLARE_MODULE(st1633);
TSLIB_DECLARE_MODULE(zet6221);
TSLIB_DECLARE_MODULE(zet6231);
TSLIB_DECLARE_MODULE(zet7235);
TSLIB_DECLARE_MODULE(zt2083_inv);
TSLIB_DECLARE_MODULE(zt2083);
