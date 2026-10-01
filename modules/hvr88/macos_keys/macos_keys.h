// Copyright 2026 HVR88
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// Apple Fn + basic key, same idea as C(kc) / SCMD(kc).
// Uses unused keycode space between QK_LIGHTING_MAX and QK_QUANTUM.
#define QK_AFN     0x7900
#define QK_AFN_MAX 0x79FF

#define AFN(kc) (QK_AFN | ((kc) & 0xFF))

#define IS_QK_AFN(kc)          ((kc) >= QK_AFN && (kc) <= QK_AFN_MAX)
#define QK_AFN_GET_KEYCODE(kc) ((kc) & 0xFF)
