; YOUR NAME(S)       Mon Oct 19 17:06:18 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage0no018

_start:                                 
        Exit    {0}                     

SECTION .data                           
I1      dd      5                       ; five
I0      dd      0                       ; zero

SECTION .bss                            
I2      resd    1                       ; aaaaaaaaaaaaaaa
I3      resd    1                       ; bbbbbbbbbbbbbbb
I4      resd    1                       ; ccccccccccccccc
B0      resd    1                       ; d
I5      resd    1                       ; ddddddddddddddd
I6      resd    1                       ; eeeeeeeeeeeeeee
I7      resd    1                       ; fffffffffffffff
I8      resd    1                       ; ggggggggggggggg
I9      resd    1                       ; hhhhhhhhhhhhhhh
I10     resd    1                       ; iiiiiiiiiiiiiii
I11     resd    1                       ; jjjjjjjjjjjjjjj
I12     resd    1                       ; kkkkkkkkkkkkkkk
I13     resd    1                       ; lllllllllllllll
I14     resd    1                       ; mmmmmmmmmmmmmmm
I15     resd    1                       ; nnnnnnnnnnnnnnn
I16     resd    1                       ; ooooooooooooooo
I17     resd    1                       ; ppppppppppppppp
I18     resd    1                       ; qqqqqqqqqqqqqqq
I19     resd    1                       ; rrrrrrrrrrrrrrr
I20     resd    1                       ; sssssssssssssss
I21     resd    1                       ; ttttttttttttttt
I22     resd    1                       ; uuuuuuuuuuuuuuu
I23     resd    1                       ; vvvvvvvvvvvvvvv
I24     resd    1                       ; wwwwwwwwwwwwwww
I25     resd    1                       ; xxxxxxxxxxxxxxx
I26     resd    1                       ; yyyyyyyyyyyyyyy
I27     resd    1                       ; zzzzzzzzzzzzzzz
