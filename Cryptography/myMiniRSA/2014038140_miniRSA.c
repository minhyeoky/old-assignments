/*
* @file    rsa.c
* @author  이민혁 /2014038140
* @date    2018/11/22
* @brief   mini RSA implementation code
* @details 세부 설명
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "2014038140_miniRSA.h"

#define TEST_PRIME FALSE
#define TEST_MODPOW FALSE
uint p, q, e, d, n;

/*i
* @brief     모듈러 덧셈 연산을 하는 함수.
* @param     uint a     : 피연산자1.
* @param     uint b     : 피연산자2.
* @param     byte op    : +, - 연산자.
* @param     uint n      : 모듈러 값.
* @return    uint result : 피연산자의 덧셈에 대한 모듈러 연산 값. (a op b) mod n
* @todo      모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/
uint modulo(uint a, uint n){
    // modulo by minus....
    uint rest = a;
    while(rest >= n){
        rest -= n;
    }
    return rest;
}
uint divide(uint a, uint n){
    // divide by minus....
    uint quotient = 0;
    while(a >= n){
        a -= n;
        quotient++;
    }
    return quotient;
}
uint modAdd(uint a, uint b, byte op, uint n) {
    // (a+b)modn=(amodn+bmodn)modn
    uint ret;
    a = modulo(a,n);
    b = modulo(b,n);
    if (op == '+') {
        // ret = modulo(a+b,n); <--- overflow.
        if(a >= n - b){ // a + b >= n -> overflow posibility.
            ret = a + b - n;
            // = (a+b)mod n (overflow를 피하기 위해- a+b는 2^32를 넘을 수 있음)
        }else{
            ret = a + b;
        }
    }else if(op == '-'){
        if ( a > b ){
            ret = modulo(a-b,n);
        }else{
            ret = modulo(b-a,n);
        }
    }else{
        printf("modAdd - wrong operation!\n");
    }
    return ret;
}
//op는 캐릭터 타입
// op가 +, overflow 처리,
// 나누기와 모듈러만 비트연산, 나머지는 하고싶은대로 하세여

/*
* @brief      모듈러 곱셈 연산을 하는 함수.
* @param      uint x       : 피연산자1.
* @param      uint y       : 피연산자2.
* @param      uint n       : 모듈러 값.
* @return     uint result  : 피연산자의 곱셈에 대한 모듈러 연산 값. (a x b) mod n
* @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/
uint modMul(uint x, uint y, uint n) {
    // xymodn = (xmodn*ymodn)modn
    uint result = 0;
    while(y > 0){ // 곱 할 y가 남아 있다면,
        if(y & 0x01){ // 곱해야 한다면.
            result = modAdd(result,x,'+',n);
        }
        x = modAdd(x,x,'+',n);
        y = y >> 1;
    }

    return modulo(result,n);
}

/*
* @brief      모듈러 거듭제곱 연산을 하는 함수.
* @param      uint base   : 피연산자1.
* @param      uint exp    : 피연산자2.
* @param      uint n      : 모듈러 값.
* @return     uint result : 피연산자의 연산에 대한 모듈러 연산 값. (base ^ exp) mod n
* @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
'square and multiply' 알고리즘을 사용하여 작성한다.
*/
uint modPow(uint base, uint exp, uint n) {
    /*
    Modular Exponentiation Algorithm using square and multiply.
    http://www.collectedwebs.com/math/modular/exponentiation/ (계산 과정 참고)
    */
    if(TEST_MODPOW){
        printf("modPow\n");
    }
    uint equation = 1;
    bool start = FALSE;
    for(int i=0;i<32;i++){
        if(TEST_MODPOW){
            printf("equation:%u\n",equation);
        }
        if ( (exp << i) & 0x80000000) {
            /*
                앞 비트부터
                첫 1을 만나면 start, equation = base.
                1을 만나면 square & multiply.
                0을 만나면 square.
            */
            if(!start) {
                if(TEST_MODPOW){
                    printf("equation started\n");
                }
                equation = base;
                start = TRUE;
            }
            else{
                if(TEST_MODPOW){
                    printf("square and multiplying..\n");
                }
                equation = modMul(equation,equation,n);
                equation = modMul(equation, base, n);
            }
        }
        else{
            if(TEST_MODPOW){
                printf("square..or not started\n");
            }
            if(start) equation = modMul(equation,equation,n);
        }
    }
    return equation;
}

/*
* @brief      입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
* @param      uint testNum   : 임의 생성된 홀수.
* @param      uint repeat    : 판단함수의 반복횟수.
* @return     uint result    : 판단 결과에 따른 TRUE, FALSE 값.
* @todo       Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다.
*/
bool isPrime(uint testNum, uint repeat) {
    /*
    밀러라빈 알고리즘.
    n이 합성수 일 때, 밀러 라빈에서 n이 소수 일 것이라고 판별 할 확률은 4^-k.
    99.99% 를 장담하려면, 4^-4 = 0.00390625, 100 - 0.00390625 = 99.99609375%
    4회 이상 반복하면 된다.
    */
    if(modulo(testNum,2) == 0) return FALSE;

    uint d = testNum - 1;
    uint s = 0;
    while(modulo(d,2)==0){ // testNum - 1 = 2^s * d
        d = divide(d,2);
        s += 1;
    }
    if(TEST_PRIME){
        printf("---IsPrime---\n");
        printf("Num:%d,Num-1=%d=2^s*d,s:%d,d:%d,n:%d\n"
        ,testNum,testNum-1, s, d, testNum);
    }
    while(repeat > 0){
        uint a = modulo((int)(WELLRNG512a() * 1000),testNum) + 1; // 1<=a<=testNum-1
        bool check = FALSE;
        if(modPow(a,d,testNum) == 1) {
            check = TRUE;// 1, 소수 일 수 있다.
        }
        else{
            uint exp_2r = 1;
            for(int r=0; r<s; r++){ // 0<=r<=s-1
                if(TEST_PRIME){printf("r:%d,exp_2r:%dmodPow(a,exp_2r*d,testNum):%d,\n",r,exp_2r,modPow(a,exp_2r*d,testNum));}
                if(modPow(a,exp_2r*d,testNum) == testNum-1){ // -1, 소수 일 수 있다.
                    check = TRUE;
                    break;
                }
                exp_2r *= 2;
            }
        }
        if(TEST_PRIME){
            printf("--repeating..--\n");
            printf("a : %d, repeat : %d, check : %d \n",a, repeat, check);
        }
        if(check == FALSE) return FALSE;
        repeat--;
    }
    return TRUE;
}

/*
* @brief       모듈러 역 값을 계산하는 함수.
* @param       uint a      : 피연산자1.
* @param       uint m      : 모듈러 값.
* @return      uint result : 피연산자의 모듈러 역수 값.
* @todo        확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
*/
uint modInv(uint a, uint m) {
    /*
    확장 유클리드 알고리즘.
    a : e,
    m : phi_n
    return : e^-1
    */
    uint m_ = m;
    int x_1 = 1;
    int x_2 = 0;
    while(m != 1){
        uint q = divide(a,m);
        int temp = x_2;
        x_2 = x_1 -x_2 * q;
        x_1 = temp;
        temp = m;
        m = modulo(a,m);
        a = temp;
    }
    if(x_2 < 0) x_2 += m_;
    return x_2;
}

/*
* @brief     RSA 키를 생성하는 함수.
* @param     uint *p   : 소수 p.
* @param     uint *q   : 소수 q.
* @param     uint *e   : 공개키 값.
* @param     uint *d   : 개인키 값.
* @param     uint *n   : 모듈러 n 값.
* @return    void
* @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
uint GCD(uint a, uint b) {
    uint prev_a;
    while(b != 0) {
        prev_a = a;
        a = b;
        while(prev_a >= b) prev_a -= b;
        b = prev_a;
    }
    return a;
}
void miniRSAKeygen(uint *p, uint *q, uint *e, uint *d, uint *n) {
    /*
    p, q = 소수.
    e는 phi(n)과 서로소인 임의의 정수.
    d는 phi(n)에 대한 e의 역원.
    n = pq. 단, 2^31<=n<2^32
    */
    while(TRUE){
        *p = WELLRNG512a() * 100000;
        *q = WELLRNG512a() * 100000;
        if(*p >= 46341 && *p < 65536 && *q >= 46341 && *q < 65536){
            if(isPrime(*p,4) && isPrime(*q,4)) break;
        }
    }
    uint phi_n = (*p-1)*(*q-1);
    while(TRUE){
        *e = WELLRNG512a()*1000000;
        *e = modulo(*e,phi_n);
        if(GCD(phi_n,*e) == 1) break;
    }
    *d = modInv(*e,phi_n);
    *n = *p * *q;
    printf(":::miniRSAKeygen Survey:::\ne:%u,d:%u,\ned(mod(phi_n))=%u\n::::::::::::::::::::::::::\n",
                                        e,d,modMul(*e,*d,phi_n));

}

/*
* @brief     RSA 암복호화를 진행하는 함수.
* @param     uint data   : 키 값.
* @param     uint key    : 키 값.
* @param     uint n      : 모듈러 n 값.
* @return    uint result : 암복호화에 결과값
* @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
uint miniRSA(uint data, uint key, uint n) {
    /*
    1. data : M, Key : e
    2. data : C, Key : d
    C = M^e (mod n)
    M = C^d (mod n)
    return : C or M.
    */
    return modPow(data, key, n);
}



int main(int argc, char* argv[]) {
    byte plain_text[4] = {0x12, 0x34, 0x56, 0x78}; // 4byte = 32bit.
    uint plain_data, encrpyted_data, decrpyted_data; //?
    uint seed = time(NULL); // RandomGenerator seed -> time

    memcpy(&plain_data, plain_text, 4); // copy 4byte to plain_data.

    // 난수 생성기 시드값 설정
    seed = time(NULL); // set seed.
    InitWELLRNG512a(&seed); // WELL function initializer for RNG

    // RSA 키 생성
    miniRSAKeygen(&p, &q, &e, &d, &n); // p * q = n, (e,n), (d,n).
    printf("0. Key generation is Success!\n ");
    printf("p : %u\n q : %u\n e : %u\n d : %u\n N : %u\n\n", p, q, e, d, n);

    // RSA 암호화 테스트
    encrpyted_data = miniRSA(plain_data, e, n); // encryption.
    printf("1. plain text : %u\n", plain_data);
    printf("2. encrypted plain text : %u\n\n", encrpyted_data);

    // RSA 복호화 테스트
    decrpyted_data = miniRSA(encrpyted_data, d, n); //decryption.
    printf("3. cipher text : %u\n", encrpyted_data);
    printf("4. Decrypted plain text : %u\n\n", decrpyted_data);

    // 결과 출력
    printf("RSA Decryption: %s\n", (decrpyted_data == plain_data) ? "SUCCESS!" : "FAILURE!");

    return 0;
}
