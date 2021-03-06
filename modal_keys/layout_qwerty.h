#if !defined(__LAYOUT_QWERTY_H_)
#define __LAYOUT_QWERTY_H_

#include "keys.h"

const KeySpec qwertyKeymap[] = {
/* A               =>  aA */         (KeySpec) { 0, _A, RShift, _A },
/* B               =>  bB */         (KeySpec) { 0, _B, RShift, _B },
/* C               =>  cC */         (KeySpec) { 0, _C, RShift, _C },
/* D               =>  dD */         (KeySpec) { 0, _D, RShift, _D },
/* E               =>  eE */         (KeySpec) { 0, _E, RShift, _E },
/* F               =>  fF */         (KeySpec) { 0, _F, RShift, _F },
/* G               =>  gG */         (KeySpec) { 0, _G, RShift, _G },
/* H               =>  hH */         (KeySpec) { 0, _H, LShift, _H },
/* I               =>  iI */         (KeySpec) { 0, _I, LShift, _I },
/* J               =>  jJ */         (KeySpec) { 0, _J, LShift, _J },
/* K               =>  kK */         (KeySpec) { 0, _K, LShift, _K },
/* L               =>  lL */         (KeySpec) { 0, _L, LShift, _L },
/* M               =>  mM */         (KeySpec) { 0, _M, LShift, _M },
/* N               =>  nN */         (KeySpec) { 0, _N, LShift, _N },
/* O               =>  oO */         (KeySpec) { 0, _O, LShift, _O },
/* P               =>  pP */         (KeySpec) { 0, _P, LShift, _P },
/* Q               =>  qQ */         (KeySpec) { 0, _Q, RShift, _Q },
/* R               =>  rR */         (KeySpec) { 0, _R, RShift, _R },
/* S               =>  sS */         (KeySpec) { 0, _S, RShift, _S },
/* T               =>  tT */         (KeySpec) { 0, _T, RShift, _T },
/* U               =>  uU */         (KeySpec) { 0, _U, LShift, _U },
/* V               =>  vV */         (KeySpec) { 0, _V, RShift, _V },
/* W               =>  wW */         (KeySpec) { 0, _W, RShift, _W },
/* X               =>  xX */         (KeySpec) { 0, _X, RShift, _X },
/* Y               =>  yY */         (KeySpec) { 0, _Y, LShift, _Y },
/* Z               =>  zZ */         (KeySpec) { 0, _Z, RShift, _Z },
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
/* Dash            =>  -_ */         (KeySpec) { 0, _Dash, LShift, _Dash },
/* Equals          =>  =+ */         (KeySpec) { 0, _Equals, LShift, _Equals },
/* LeftBracket     =>  [{ */         (KeySpec) { 0, _LeftBracket, LShift, _LeftBracket },
/* RightBracket    =>  ]} */         (KeySpec) { 0, _RightBracket, LShift, _RightBracket },
/* Backslash       =>  \| */         (KeySpec) { 0, _Backslash, LShift, _Backslash},
/* International2  =>  ... */        (KeySpec) { 0, _International2, LShift, _International2 },
/* Semicolon       =>  ;: */         (KeySpec) { 0, _Semicolon, LShift, _Semicolon },
/* Apostrophe      =>  '" */         (KeySpec) { 0, _Apostrophe, LShift, _Apostrophe },
/* Backtick        =>  `~ */         (KeySpec) { 0, _Backtick, RShift, _Backtick },
/* Comma           =>  ,< */         (KeySpec) { 0, _Comma, LShift, _Comma },
/* Fullstop        =>  .> */         (KeySpec) { 0, _Fullstop, LShift, _Fullstop },
/* ForwardSlash    =>  /? */         (KeySpec) { 0, _ForwardSlash, LShift, _ForwardSlash },
/* CapsLock        =>  Capslock */   (KeySpec) { 0, _CapsLock, LShift, _CapsLock }
};

#endif // __LAYOUT_QWERTY_H_
