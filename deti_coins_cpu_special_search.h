// typedef union{
//   u32_t coin_as_ints[13]; // 13 4-byte integers
//   char coin_as_chars[52]; // 52 = 13*4 bytes
// }coin_t;

// u32_t next_value_to_try_ascii(u32_t v){
//     v=v+1;
//     if((v & 0x000000FF) < 0x0000007F) return v;
//     v+=0x000000A1;
//     if((v & 0x0000FF00) < 0x00007F00) return v;
//     v+=0x0000A100;
//     if((v & 0x00FF0000) < 0x007F0000) return v;
//     v+=0x00A10000;
//     if((v & 0xFF000000) < 0x7F000000) return v;
//     return 0x20202020;
// }