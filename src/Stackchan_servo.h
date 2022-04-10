#ifndef _STACKCHAN_SERVO_H_
#define _STACKCHAN_SERVO_H_

// コンパイル時にServoEasing.hppをIncludeしてくださいという警告が出ますが、hppにすると二重定義のリンクエラーが出ます。
// その対処でStackchan_servo.hはh, Stackchan_servo.cppはhppをincludeしています。
#include <ServoEasing.h>

enum Motion {
    nomove,    // 動かない
    greet,     // 挨拶
    laugh,     // 笑う
    nod,       // うなづく
    refuse,    // 首を横に振る（拒絶）
    test = 99,      // テスト用
};



typedef struct ServoParam {
    int servo_pin;                    // サーボのピン番号
    int8_t start_degree;              // 初期角度
    int8_t offset;                    // オフセット（90°からの+-）
    int8_t degree;                    // 角度
    uint32_t millis_for_move;         // 移動時間(msec)
} servo_param_s;


typedef struct  StackchanServo{
    servo_param_s x;
    servo_param_s y;
} stackchan_servo_initial_param_s;

class StackchanSERVO {
    private:
        stackchan_servo_initial_param_s _init_param;
        ServoEasing _servo_x;
        ServoEasing _servo_y;
        void attachServos();
    public:
        StackchanSERVO();
        ~StackchanSERVO();
        void begin(stackchan_servo_initial_param_s init_params);
        void begin(int servo_pin_x, int8_t start_degree_x, int8_t offset_x,
                   int servo_pin_y, int8_t start_degree_y, int8_t offset_y );
        void moveX(int x, uint32_t millis_for_move = 0);
        void moveY(int y, uint32_t millis_for_move = 0);
        void moveXY(int x, int y, uint32_t millis_for_move);
        void moveX(servo_param_s servo_param_x);
        void moveY(servo_param_s servo_param_y);
        void moveXY(servo_param_s servo_param_x, servo_param_s servo_param_y);
        void motion(Motion motion_no);
};

#endif // _STACKCHAN_SERVO_H_