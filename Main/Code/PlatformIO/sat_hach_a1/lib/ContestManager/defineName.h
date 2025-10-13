#pragma once

#define START_CONTEST 0xA0
#define STOP_CONTEST 0xA1
// Contest IDs
#define CONTEST_1_ID 0x41
#define CONTEST_2_ID 0x42
#define CONTEST_3_ID 0x43
#define CONTEST_4_ID 0x44

// Contest Names
#define CONTEST_1_NAME "Đi qua hình số 8"
#define CONTEST_2_NAME "Đi qua vạch đường thẳng"
#define CONTEST_3_NAME "Đi qua đường có vạch cản"
#define CONTEST_4_NAME "Đi qua đường gồ ghề"

// Error IDs
#define ERROR_OVERRIDE_START 0xE1 // đè vạch xuất phát
#define ERROR_OVERRIDE_LINE 0xE2  // đè vạch chướng ngại vật
#define ERROR_FOOT_AT_GROUND 0xE3 // chân chạm đất
#define ERROR_RUNOUT_TIME 0xE4    // quá thời gian quy định
#define ERROR_GO_WRONG_WAY 0xE5   // đi sai đường
#define ERROR_FALL_OF_BIKE 0xE6   // đổ xe
#define ERROR_GO_OUT_LINE 0xE7    // đi ra ngoài vạch
#define ERROR_STOP_ENGINE 0xE8    // chết máy giữa đường thi
#define ERROR_NO_HELMET 0xE9      // Không đội mũ bảo hiểm

//