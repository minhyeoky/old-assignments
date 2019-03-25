/*  ======================================================================== *

                                    주 의 사 항


    1. 구현은 다양한 방식으로 이뤄질 수 있음
    2. AES128(...) 함수의 호출과 리턴이 여러번 반복되더라도 메모리 누수가 생기지 않게 함
    3. AddRoundKey 함수를 구현할 때에도 파라미터 rKey는 사전에 선언된 지역 배열을 가리키도록 해야 함
       (정확한 구현을 위해서는 포인터 개념의 이해가 필요함)
    4. 배열의 인덱스 계산시 아래에 정의된 KEY_SIZE, ROUNDKEY_SIZE, state_SIZE를 이용해야 함
       (상수 그대로 사용하면 안됨. 예로, 4, 16는 안되고 KEY_SIZE/4, state_SIZE로 사용해야 함)

 *  ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include "AES128.h"
#include <string.h>

#define KEY_SIZE 32 //16, 24, 32 - 라운드 10, 12, 14
#define ROUNDKEY_SIZE 240 //176, 208, 240
#define state_SIZE 16 //평문의 크기는 항상 같음


static const BYTE sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const BYTE rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
static const BYTE Rcon[ROUNDKEY_SIZE/16][4] = {
    {0x01,0x00,0x00,0x00},
    {0x02,0,0,0},
    {0x04,0,0,0},
    {0x08,0,0,0},
    {0x10,0,0,0},
    {0x20,0,0,0},
    {0x40,0,0,0},
    {0x80,0,0,0},
    {0x1B,0,0,0},
    {0x36,0,0,0}//rKey 10
};
/* 기타 필요한 함수 추가 선언 및 정의 */
BYTE multyplyGF(int x, BYTE s){
    if(x == 2){
        if((s & 0x80) == 0x80) return ((s<<1) ^ 0x1B);
        else return s<<1;
    }else if(x == 3){
        return s ^ multyplyGF(2,s);
    }else if(x == 9){
        return multyplyGF(2,multyplyGF(2,multyplyGF(2,s))) ^ s;
    }else if(x == 11){
        return multyplyGF(2,(multyplyGF(2,multyplyGF(2,s)) ^ s)) ^s;
    }else if(x == 13){
        return multyplyGF(2,multyplyGF(2,multyplyGF(2,s) ^ s)) ^ s;
    }else if(x == 14){
        return multyplyGF(2,multyplyGF(2,multyplyGF(2,s) ^ s) ^ s);
    }else return s;
}
/*  <키스케줄링 함수>
 *
 *  key         키스케줄링을 수행할 16바이트 키
 *  roundKey    키스케줄링의 결과인 176바이트 라운드키가 담길 공간
 */


void expandKey(BYTE *key, BYTE *w){
    BYTE temp[KEY_SIZE/4]; //w, B0~B3, 4, 6, 8
    printf("@@");
    int i;
    for(i=0;i<(state_SIZE/4);i++){ //rows.(4)
        for(int j=0;j<(KEY_SIZE/4);j++){ //colums.(4,6,8)
            w[i+(KEY_SIZE/4)*j] = key[(KEY_SIZE/4)*i+j]; //rKey[0]
        }
    }
    for(i=(KEY_SIZE/4);i<(state_SIZE/4)*(ROUNDKEY_SIZE/16);i++){ //rKey[1]~rKey[10,12,14]
            int w_ = i-1;
            for(int j=0;j<(KEY_SIZE/4);j++){
                temp[j] = w[(w_-w_%(KEY_SIZE/4))*(KEY_SIZE/4) + w_%(KEY_SIZE/4) + j*(KEY_SIZE/4)]; //W-1, B0~B3,5,7
            }
        if(i % (KEY_SIZE/4) == 0){ //g

            BYTE temp2;
            temp2 = temp[0];
            for(int j=0;j<(KEY_SIZE/4);j++){ // RotWord,
                if(j==(KEY_SIZE/4)-1) temp[j] = temp2;
                else {temp[j] = temp[j+1];}
            }

            for(int j=0;j<(KEY_SIZE/4);j++){ //SubWord
                temp[j] = sbox[temp[j]];
            }

            temp[0] = temp[0] ^ Rcon[i/(KEY_SIZE/4)-1][0]; //Rcon
        }
        w_ = i - 4; //W[i-4]
        for(int j=0;j<(KEY_SIZE/4);j++){
            w[(i-i%(KEY_SIZE/4))*(KEY_SIZE/4) + i%(KEY_SIZE/4) + j*(KEY_SIZE/4)] = w[(w_-w_%(KEY_SIZE/4))*(KEY_SIZE/4) + w_%(KEY_SIZE/4)+j*(KEY_SIZE/4)] ^ temp[j];
        }
//        w[(i-i%4)*4 +i%4 + 0] = w[(w_-w_%4)*4 +w_%4 + 0] ^ temp[0]; // W[i]
//        w[(i-i%4)*4 +i%4 + 4] = w[(w_-w_%4)*4 +w_%4 + 4] ^ temp[1];
//        w[(i-i%4)*4 +i%4 + 8] = w[(w_-w_%4)*4 +w_%4 + 8] ^ temp[2];
//        w[(i-i%4)*4 +i%4 + 12] = w[(w_-w_%4)*4 +w_%4 + 12] ^ temp[3];

    }
    /* 추가 구현 */

}


/*  <SubBytes 함수>
 *
 *  state   SubBytes 수행할 16바이트 state. 수행 결과는 해당 배열에 바로 반영
 *  mode    SubBytes 수행 모드
 */
 BYTE* subBytes(BYTE *state, int mode){

    /* 필요하다 생각하면 추가 선언 */

    switch(mode){

        case ENC:

            for(int i=0;i<state_SIZE/4;i++){
                for(int j=0;j<state_SIZE/4;j++){
                    state[j+i*state_SIZE/4] = sbox[state[j+i*state_SIZE/4]];
                }
            }


            break;

        case DEC:

            for(int i=0;i<state_SIZE/4;i++){
                for(int j=0;j<state_SIZE/4;j++){
                    state[j+i*state_SIZE/4] = rsbox[state[j+i*state_SIZE/4]];
                }
            }


            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }

    return state;
}


/*  <ShiftRows 함수>
 *
 *  state   ShiftRows 수행할 16바이트 state. 수행 결과는 해당 배열에 바로 반영
 *  mode    ShiftRows 수행 모드
 */

BYTE* shiftRows(BYTE *state, int mode){

    /* 필요하다 생각하면 추가 선언 */
    BYTE temp;
    switch(mode){

        case ENC:

            temp = state[state_SIZE/4];
            for(int i=0;i<state_SIZE/4-1;i++){
                state[state_SIZE/4+i] = state[state_SIZE/4+i+1];
            }
            state[state_SIZE/2-1] = temp;

            temp = state[state_SIZE/2];
            state[state_SIZE/2] = state[state_SIZE/2+2];
            state[state_SIZE/2+2] = temp;

            temp = state[state_SIZE/2+1];
            state[state_SIZE/2+1] = state[state_SIZE/2+3];
            state[state_SIZE/2+3] = temp; //row 2 shifi 2


            temp = state[state_SIZE-4];
            state[state_SIZE-4] = state[state_SIZE-1];
            state[state_SIZE-1] = state[state_SIZE-2];
            state[state_SIZE-2] = state[state_SIZE-3];
            state[state_SIZE-3] = temp; //row 3 shift 3

            break;

        case DEC:

            temp = state[state_SIZE/4];
            state[state_SIZE/4] = state[state_SIZE/4+3];
            state[state_SIZE/4+3] = state[state_SIZE/4+2];
            state[state_SIZE/4+2] = state[state_SIZE/4+1];
            state[state_SIZE/4+1] = temp; //row 1 shift 1

            temp = state[state_SIZE/2];
            state[state_SIZE/2] = state[state_SIZE/2+2];
            state[state_SIZE/2+2] = temp;
            temp = state[state_SIZE/2+1];
            state[state_SIZE/2+1] = state[state_SIZE/2+3];
            state[state_SIZE/2+3] = temp; //row 2 shift 2

            temp = state[state_SIZE-4];
            state[state_SIZE-4] = state[state_SIZE-3];
            state[state_SIZE-3] = state[state_SIZE-2];
            state[state_SIZE-2] = state[state_SIZE-1];
            state[state_SIZE-1] = temp; //row 3 shift 3

            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }

    return state;
}


/*  <MixColumns 함수>
 *
 *  state   MixColumns을 수행할 16바이트 state. 수행 결과는 해당 배열에 바로 반영
 *  mode    MixColumns의 수행 모드
 */

//미리 만들어 놓은 테이블을 사용할 수 있음.
BYTE* mixColumns(BYTE *state, int mode){

    /* 필요하다 생각하면 추가 선언 */
    BYTE temp[state_SIZE];
    memcpy(temp, state, sizeof(BYTE)*state_SIZE);
    switch(mode){
        case ENC:
            for(int i=0;i<state_SIZE/4;i++){
                state[0+i] = multyplyGF(2,temp[0+i]) ^ multyplyGF(3,temp[4+i]) ^
                            multyplyGF(1,temp[8+i]) ^ multyplyGF(1,temp[12+i]);
                state[4+i] = multyplyGF(1,temp[0+i]) ^ multyplyGF(2,temp[4+i]) ^
                            multyplyGF(3,temp[8+i]) ^ multyplyGF(1,temp[12+i]);
                state[8+i] = multyplyGF(1,temp[0+i]) ^ multyplyGF(1,temp[4+i]) ^
                            multyplyGF(2,temp[8+i]) ^ multyplyGF(3,temp[12+i]);
                state[12+i] = multyplyGF(3,temp[0+i]) ^ multyplyGF(1,temp[4+i]) ^
                            multyplyGF(1,temp[8+i]) ^ multyplyGF(2,temp[12+i]);
            }
            /* 추가 구현 */

            break;

        case DEC:
                for(int i=0;i<state_SIZE/4;i++){
                state[0+i] = multyplyGF(14,temp[0+i]) ^ multyplyGF(11,temp[4+i]) ^
                            multyplyGF(13,temp[8+i]) ^ multyplyGF(9,temp[12+i]);
                state[4+i] = multyplyGF(9,temp[0+i]) ^ multyplyGF(14,temp[4+i]) ^
                            multyplyGF(11,temp[8+i]) ^ multyplyGF(13,temp[12+i]);
                state[8+i] = multyplyGF(13,temp[0+i]) ^ multyplyGF(9,temp[4+i]) ^
                            multyplyGF(14,temp[8+i]) ^ multyplyGF(11,temp[12+i]);
                state[12+i] = multyplyGF(11,temp[0+i]) ^ multyplyGF(13,temp[4+i]) ^
                            multyplyGF(9,temp[8+i]) ^ multyplyGF(14,temp[12+i]);
                }
            break;

        default:
            fprintf(stderr, "Invalid mode!\n");
            exit(1);
    }

    return state;
}


/*  <AddRoundKey 함수>
 *
 *  state   AddRoundKey를 수행할 16바이트 state. 수행 결과는 해당 배열에 반영
 *  rKey    AddRoundKey를 수행할 16바이트 라운드키
 */
BYTE* addRoundKey(BYTE *state, BYTE *rKey){

    /* 추가 구현 */
    for(int i=0;i<state_SIZE/4;i++){
        for(int j=0;j<state_SIZE/4;j++){
            state[j+i*4] ^= rKey[j+i*4];
        }
    }
    return state;
}


/*  <128비트 AES 암복호화 함수>
 *
 *  mode가 ENC일 경우 평문을 암호화하고, DEC일 경우 암호문을 복호화하는 함수
 *
 *  [ENC 모드]
 *  input   평문 바이트 배열
 *  result  결과(암호문)이 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘어옴
 *  key     128비트 암호키 (16바이트)
 *
 *  [DEC 모드]
 *  input   암호문 바이트 배열
 *  result  결과(평문)가 담길 바이트 배열. 호출하는 사용자가 사전에 메모리를 할당하여 파라미터로 넘어옴
 *  key     128비트 암호키 (16바이트)
 */

void AES128(BYTE *input, BYTE *result, BYTE *key, int mode){

    BYTE state[state_SIZE]; /* state = input */
    BYTE out[state_SIZE]; /* out = result */
    BYTE w[ROUNDKEY_SIZE]; /* expandKEY */

    /* state = input */
    for(int i=0;i<state_SIZE/4;i++){
        for(int j=0;j<state_SIZE/4;j++){
            state[i*4+j] = input[i+4*j];
        }
    }
    expandKey(key, w);
    for(int k =0;k<12;k++){
    for(int i=0;i<state_SIZE/4;i++){
        for(int j=0;j<state_SIZE/4;j++){
            printf("%x",w+k*16+i+j*4);
        }
        printf("\n");
    }}

    printf("%d",mode);
    if(mode == ENC){
    printf("@@");


        addRoundKey(state, w); //ROUND 0
    printf("@@");


        for(int i=1;i<(ROUNDKEY_SIZE/16-1);i++){
            subBytes(state,ENC);
            shiftRows(state,ENC);
            mixColumns(state,ENC);
            addRoundKey(state,w + i*16);
        }
    printf("@@");

        subBytes(state,ENC);
        shiftRows(state,ENC);
        addRoundKey(state,w + ROUNDKEY_SIZE-16);
    printf("!!!!@");


        /* 추가 작업이 필요하다 생각하면 추가 구현 */
        for(int i=0;i<state_SIZE/4;i++){
            for(int j=0;j<state_SIZE/4;j++){
                out[i*4+j] = state[i+4*j];
            }
        }
    printf("@@");
        for(int i=0;i<state_SIZE/4;i++){
            for(int j=0;j<state_SIZE/4;j++){
                printf("%x", state[i*4+j]);
            }
            printf("\n");
        }
        memcpy(result, out, sizeof(BYTE)*state_SIZE);
    printf("@@");

    }else if(mode == DEC){

        addRoundKey(state, w+ROUNDKEY_SIZE-16); //w[Nr*Nb, (Nr+1)*Nb-1]

        for(int i=1;i<(ROUNDKEY_SIZE/16-1);i++){
            shiftRows(state,DEC);
            subBytes(state,DEC);
            addRoundKey(state,w + ROUNDKEY_SIZE-KEY_SIZE*(i+1)); //w[round*Nb, (round+1)*Nb-1]
            mixColumns(state,DEC);
        }
        subBytes(state,DEC);
        shiftRows(state,DEC);
        addRoundKey(state,w);

        /* 추가 작업이 필요하다 생각하면 추가 구현 */
        for(int i=0;i<state_SIZE/4;i++){
            for(int j=0;j<state_SIZE/4;j++){
                out[i*4+j] = state[i+4*j];
            }
        }
        memcpy(result, out, sizeof(BYTE)*state_SIZE);

    }else{
        fprintf(stderr, "Invalid mode@@@@@@@@@!\n");
        exit(1);
    }
}

