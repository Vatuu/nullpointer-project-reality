#include "debug_font.h"

const u8 debug_font_width = 6;
const u8 debug_font_height = 8;
const u8 debug_font_char_per_line = 12;

static const u8 debug_font[] __attribute__((aligned(8))) = {
  0x00,0x46,0xC0,0x21,0x92,0x0C,0x10,0x80,0x00,0x00,0x00,0x00, // .........#...##.##........#....##..#..#.....##.....#....#.......................................
  0x00,0xE6,0xCA,0x39,0x95,0x0C,0x20,0x42,0x84,0x00,0x00,0x01, // ........###..##.##..#.#...###..##..#.#.#....##....#......#....#.#....#.........................#
  0x00,0xE4,0x9F,0x40,0x25,0x08,0x20,0x43,0x84,0x00,0x00,0x02, // ........###..#..#..#####.#........#..#.#....#.....#......#....###....#........................#.
  0x00,0x40,0x0A,0x30,0x42,0x00,0x20,0x47,0xDF,0x01,0xF0,0x04, // .........#..........#.#...##.....#....#...........#......#...#####.#####.......#####.........#..
  0x00,0x40,0x0A,0x08,0x85,0x40,0x20,0x43,0x84,0x00,0x00,0x08, // .........#..........#.#.....#...#....#.#.#........#......#....###....#......................#...
  0x00,0x00,0x1F,0x71,0x34,0x80,0x20,0x42,0x84,0x30,0x03,0x10, // ...................#####.###...#..##.#..#.........#......#....#.#....#....##..........##...#....
  0x00,0x40,0x0A,0x11,0x33,0x40,0x10,0x80,0x00,0x30,0x03,0x00, // .........#..........#.#....#...#..##..##.#.........#....#.................##..........##........
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00, // ..........................................................................#.....................
  0x38,0x43,0x8E,0x09,0xF1,0x9F,0x38,0xE0,0x00,0x08,0x02,0x0E, // ..###....#....###...###.....#..#####...##..#####..###...###.................#.........#.....###.
  0x44,0xC4,0x51,0x19,0x02,0x01,0x45,0x10,0x00,0x10,0x01,0x11, // .#...#..##...#...#.#...#...##..#......#........#.#...#.#...#...............#...........#...#...#
  0x4C,0x40,0x41,0x29,0x04,0x02,0x45,0x13,0x0C,0x21,0xF0,0x81, // .#..##...#.......#.....#..#.#..#.....#........#..#...#.#...#..##....##....#....#####....#......#
  0x54,0x41,0x8E,0x49,0xE7,0x84,0x38,0xF3,0x0C,0x40,0x00,0x46, // .#.#.#...#.....##...###..#..#..####..####....#....###...####..##....##...#...............#...##.
  0x64,0x42,0x01,0x7C,0x14,0x48,0x44,0x10,0x00,0x20,0x00,0x84, // .##..#...#....#........#.#####.....#.#...#..#....#...#.....#..............#.............#....#..
  0x44,0x44,0x11,0x09,0x14,0x48,0x44,0x23,0x0C,0x11,0xF1,0x00, // .#...#...#...#.....#...#....#..#...#.#...#..#....#...#....#...##....##.....#...#####...#........
  0x38,0xE7,0xCE,0x08,0xE3,0x88,0x38,0xC3,0x0C,0x08,0x02,0x04, // ..###...###..#####..###.....#...###...###...#.....###...##....##....##......#.........#......#..
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00, // ....................................................................#...........................
  0x38,0xE7,0x8E,0x79,0xF7,0xCE,0x44,0xE0,0x51,0x41,0x14,0x4E, // ..###...###..####...###..####..#####.#####..###..#...#..###......#.#...#.#.....#...#.#...#..###.
  0x45,0x14,0x51,0x45,0x04,0x11,0x44,0x40,0x52,0x41,0xB6,0x51, // .#...#.#...#.#...#.#...#.#...#.#.....#.....#...#.#...#...#.......#.#..#..#.....##.##.##..#.#...#
  0x5D,0x14,0x50,0x45,0x04,0x10,0x44,0x40,0x54,0x41,0x55,0x51, // .#.###.#...#.#...#.#.....#...#.#.....#.....#.....#...#...#.......#.#.#...#.....#.#.#.#.#.#.#...#
  0x55,0x17,0x90,0x45,0xE7,0x97,0x7C,0x40,0x58,0x41,0x14,0xD1, // .#.#.#.#...#.####..#.....#...#.####..####..#.###.#####...#.......#.##....#.....#...#.#..##.#...#
  0x5D,0xF4,0x50,0x45,0x04,0x11,0x44,0x44,0x54,0x41,0x14,0x51, // .#.###.#####.#...#.#.....#...#.#.....#.....#...#.#...#...#...#...#.#.#...#.....#...#.#...#.#...#
  0x41,0x14,0x51,0x45,0x04,0x11,0x44,0x44,0x52,0x41,0x14,0x51, // .#.....#...#.#...#.#...#.#...#.#.....#.....#...#.#...#...#...#...#.#..#..#.....#...#.#...#.#...#
  0x39,0x17,0x8E,0x79,0xF4,0x0F,0x44,0xE3,0x91,0x7D,0x14,0x4E, // ..###..#...#.####...###..####..#####.#......####.#...#..###...###..#...#.#####.#...#.#...#..###.
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ................................................................................................
  0x78,0xE7,0x8E,0x7D,0x14,0x51,0x45,0x17,0x8E,0x00,0xE1,0x00, // .####...###..####...###..#####.#...#.#...#.#...#.#...#.#...#.####...###.........###....#........
  0x45,0x14,0x51,0x11,0x14,0x51,0x45,0x10,0x88,0x40,0x22,0x80, // .#...#.#...#.#...#.#...#...#...#...#.#...#.#...#.#...#.#...#....#...#....#........#...#.#.......
  0x45,0x14,0x50,0x11,0x14,0x55,0x29,0x11,0x08,0x20,0x24,0x40, // .#...#.#...#.#...#.#.......#...#...#.#...#.#.#.#..#.#..#...#...#....#.....#.......#..#...#......
  0x79,0x17,0x8E,0x11,0x14,0x55,0x10,0xA2,0x08,0x10,0x20,0x00, // .####..#...#.####...###....#...#...#.#...#.#.#.#...#....#.#...#.....#......#......#.............
  0x41,0x54,0x81,0x11,0x14,0x55,0x28,0x44,0x08,0x08,0x20,0x00, // .#.....#.#.#.#..#......#...#...#...#.#...#.#.#.#..#.#....#...#......#.......#.....#.............
  0x41,0x24,0x51,0x11,0x12,0x95,0x44,0x44,0x08,0x04,0x20,0x00, // .#.....#..#..#...#.#...#...#...#...#..#.#..#.#.#.#...#...#...#......#........#....#.............
  0x40,0xD4,0x4E,0x10,0xE1,0x0A,0x44,0x47,0x8E,0x00,0xE0,0x00, // .#......##.#.#...#..###....#....###....#....#.#..#...#...#...####...###.........###.............
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F, // ..........................................................................................######
  0x30,0x04,0x00,0x04,0x01,0x80,0x40,0x40,0x90,0x10,0x00,0x00, // ..##.........#...............#.........##........#.......#......#..#.......#....................
  0x30,0x04,0x00,0x04,0x02,0x00,0x40,0x00,0x10,0x10,0x00,0x00, // ..##.........#...............#........#..........#.................#.......#....................
  0x10,0xE7,0x8E,0x3C,0xE2,0x0F,0x70,0x41,0x92,0x11,0xA7,0x0E, // ...#....###..####...###...####..###...#.....####.###.....#.....##..#..#....#...##.#..###....###.
  0x00,0x14,0x51,0x45,0x17,0x91,0x48,0x40,0x94,0x11,0x54,0x91, // ...........#.#...#.#...#.#...#.#...#.####..#...#.#..#....#......#..#.#.....#...#.#.#.#..#..#...#
  0x00,0xF4,0x50,0x45,0xE2,0x11,0x48,0x40,0x98,0x11,0x54,0x91, // ........####.#...#.#.....#...#.####...#....#...#.#..#....#......#..##......#...#.#.#.#..#..#...#
  0x01,0x14,0x51,0x45,0x02,0x0F,0x48,0x40,0x94,0x11,0x14,0x91, // .......#...#.#...#.#...#.#...#.#......#.....####.#..#....#......#..#.#.....#...#...#.#..#..#...#
  0x00,0xF7,0x8E,0x3C,0xE2,0x01,0x48,0x64,0x92,0x19,0x14,0x8E, // ........####.####...###...####..###...#........#.#..#....##..#..#..#..#....##..#...#.#..#...###.
  0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x03,0x00,0x00,0x00,0x00, // ............................................###...............##................................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x10,0xC2,0x84, // .....................................................................##....#....##....#.#....#..
  0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x08,0x10,0x25,0x0E, // ..........................#.........................................#......#......#..#.#....###.
  0x78,0xF5,0x8E,0x79,0x24,0x51,0x49,0x27,0x88,0x10,0x20,0x1B, // .####...####.#.##...###..####..#..#..#...#.#...#.#..#..#..#..####...#......#......#........##.##
  0x45,0x12,0x50,0x21,0x24,0x51,0x49,0x20,0x98,0x00,0x30,0x11, // .#...#.#...#..#..#.#......#....#..#..#...#.#...#.#..#..#..#.....#..##.............##.......#...#
  0x45,0x12,0x0E,0x21,0x24,0x55,0x31,0x23,0x08,0x10,0x20,0x11, // .#...#.#...#..#.....###...#....#..#..#...#.#.#.#..##...#..#...##....#......#......#........#...#
  0x45,0x12,0x01,0x29,0x62,0x9F,0x48,0xE4,0x08,0x10,0x20,0x1F, // .#...#.#...#..#........#..#.#..#.##...#.#..#####.#..#...###..#......#......#......#........#####
  0x78,0xF7,0x0E,0x10,0xA1,0x0A,0x48,0x47,0x86,0x10,0xC0,0x00, // .####...####.###....###....#....#.#....#....#.#..#..#....#...####....##....#....##..............
  0x40,0x10,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00  // .#.........#...........................................##.......................................
};

void debug_draw_char(u32 x, u32 y, char c, int scale) {
    u32 line_length = debug_font_char_per_line * debug_font_width;
    u8 index = (u8)c - 32;
    u32 offset = ((index % 8) * debug_font_width) + ((line_length * debug_font_height) *  (index / 8));
    u32 fbX = x; u32 fbY = y;

    for(int i = 0; i < debug_font_height; i++) {
        u32 offsetY = i * line_length;
        for(int j = 0; j < debug_font_height; j++) {
            u8 pixel = debug_font[offset + (j + offsetY)];
            u16* fbPixel = (u16*)FB_ADDRESS + (fbY * 320) + fbX;
            for(int mask = 80; mask != 0; mask >>= 1) {
                *fbPixel = pixel & mask ? 0xFFFF : 0x0001;
            }
            fbX++;
        }
        fbX = x;
        fbY++;
    }
}