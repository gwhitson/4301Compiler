; YOUR NAME(S)       Mon Oct 19 17:06:19 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage0no029

_start:                                 
        Exit    {0}                     

SECTION .data                           
I2      dd      1                       ; big
I3      dd      2                       ; bigger_yet
B1      dd      0                       ; no
I0      dd      0                       ; small
I1      dd      -1                      ; smalleryet
B0      dd      -1                      ; yes

SECTION .bss                            
I4      resd    1                       ; some
