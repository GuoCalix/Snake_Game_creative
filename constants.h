#ifndef CONSTANTS_H
#define CONSTANTS_H

const int TILE_SIZE = 40;
// 根据六边形网格大小定义 HEX_X 和 HEX_Y
const int HEX_X = 40; // 示例值
const int HEX_Y = 40; // 示例值

enum GameObjectsData {
    GD_Type
};

enum GameObjectTypes {
    GO_Food,
    GO_Wall
};

enum MoveMode {
    FOUR_DIRECTIONS,  // 上下左右
    SIX_DIRECTIONS    // 上下左右斜角
};

enum GameMode{
    INFINITE_MODE,  // 无限模式
    CHALLENGE_MODE  // 挑战闯关模式
};

#endif // CONSTANTS_H
