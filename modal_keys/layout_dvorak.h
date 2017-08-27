#if !defined(__LAYOUT_DVORAK_H_)
#define __LAYOUT_DVORAK_H_

#include "keys.h"

const KeySpec dvorakKeymap[] = {
/* A               =>  aA */         (KeySpec) { 0, _A, RShift, _A },
/* B               =>  xX */         (KeySpec) { 0, _X, RShift, _X },
/* C               =>  jJ */         (KeySpec) { 0, _J, RShift, _J },
/* D               =>  eE */         (KeySpec) { 0, _E, RShift, _E },
/* E               =>  .> */         (KeySpec) { 0, _Fullstop, RShift, _Fullstop },
/* F               =>  uU */         (KeySpec) { 0, _U, RShift, _U },
/* G               =>  iI */         (KeySpec) { 0, _I, RShift, _I },
/* H               =>  dD */         (KeySpec) { 0, _D, LShift, _D },
/* I               =>  cC */         (KeySpec) { 0, _C, LShift, _C },
/* J               =>  hH */         (KeySpec) { 0, _H, LShift, _H },
/* K               =>  tT */         (KeySpec) { 0, _T, LShift, _T },
/* L               =>  nN */         (KeySpec) { 0, _N, LShift, _N },
/* M               =>  mM */         (KeySpec) { 0, _M, LShift, _M },
/* N               =>  bB */         (KeySpec) { 0, _B, LShift, _B },
/* O               =>  rR */         (KeySpec) { 0, _R, LShift, _R },
/* P               =>  lL */         (KeySpec) { 0, _L, LShift, _L },
/* Q               =>  '" */         (KeySpec) { 0, _Apostrophe, RShift, _Apostrophe },
/* R               =>  pP */         (KeySpec) { 0, _P, RShift, _P },
/* S               =>  oO */         (KeySpec) { 0, _O, RShift, _O },
/* T               =>  yY */         (KeySpec) { 0, _Y, RShift, _Y },
/* U               =>  gG */         (KeySpec) { 0, _G, LShift, _G },
/* V               =>  kK */         (KeySpec) { 0, _K, LShift, _K },
/* W               =>  ,< */         (KeySpec) { 0, _Comma, RShift, _Comma },
/* X               =>  qQ */         (KeySpec) { 0, _Q, RShift, _Q },
/* Y               =>  fF */         (KeySpec) { 0, _F, LShift, _F },
/* Z               =>  ;: */         (KeySpec) { 0, _Semicolon, RShift, _Semicolon },
/* 1               =>  1! */         (KeySpec) { 0, _1, RShift, _1 },
/* 2               =>  2@ */         (KeySpec) { 0, _2, RShift, _2 },
/* 3               =>  3# */         (KeySpec) { 0, _3, RShift, _3 },
/* 4               =>  4$ */         (KeySpec) { 0, _4, RShift, _4 },
/* 5               =>  5% */         (KeySpec) { 0, _5, RShift, _5 },
/* 6               =>  6^ */         (KeySpec) { 0, _6, RShift, _6 },
/* 7               =>  7& */         (KeySpec) { 0, _7, LShift, _7 },
/* 8               =>  8* */         (KeySpec) { 0, _8, LShift, _8 },
/* 9               =>  9( */         (KeySpec) { 0, _9, LShift, _9 },
/* 0               =>  0) */         (KeySpec) { 0, _0, LShift, _0 },
/* Enter           =>  Enter */      (KeySpec) { 0, _Enter, LShift, _Enter },
/* Escape          =>  Escape */     (KeySpec) { 0, _Escape, RShift, _Escape },
/* Backspace       =>  Backspace */  (KeySpec) { 0, _Backspace, LShift, _Backspace },
/* Tab             =>  Tab */        (KeySpec) { 0, _Tab, RShift, _Tab },
/* Space           =>  Space */      (KeySpec) { 0, _Space, LShift, _Space },
/* Dash            =>  [{ */         (KeySpec) { 0, _LeftBracket, LShift, _LeftBracket },
/* Equals          =>  ]} */         (KeySpec) { 0, _RightBracket, LShift, _RightBracket },
/* LeftBracket     =>  /? */         (KeySpec) { 0, _ForwardSlash, LShift, _ForwardSlash },
/* RightBracket    =>  =+ */         (KeySpec) { 0, _Equals, LShift, _Equals },
/* Backslash       =>  \| */         (KeySpec) { 0, _Backslash, LShift, _Backslash},
/* International2  =>  ... */        (KeySpec) { 0, _International2, LShift, _International2 },
/* Semicolon       =>  sS */         (KeySpec) { 0, _S, LShift, _S },
/* Apostrophe      =>  -_ */         (KeySpec) { 0, _Dash, LShift, _Dash },
/* Backtick        =>  `~ */         (KeySpec) { 0, _Backtick, RShift, _Backtick },
/* Comma           =>  wW */         (KeySpec) { 0, _W, LShift, _W },
/* Fullstop        =>  vV */         (KeySpec) { 0, _V, LShift, _V },
/* ForwardSlash    =>  zZ */         (KeySpec) { 0, _Z, LShift, _Z },
/* CapsLock        =>  Capslock */   (KeySpec) { 0, _CapsLock, LShift, _CapsLock }
};

#endif // __LAYOUT_DVORAK_H_