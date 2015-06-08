[PLOTREC]

style curve 1 name LAUNCH 
style curve 2 name UNDER CLD
style curve 3 name OVER CLD
style curve 4 name ENGAGE
style curve 5 name LASE
style curve 6 name BO

style plottype hstack
style legend on
style title      \c1 ABL\n21 Missle Raid\nLauches 0 to 29 Seconds (U)
style xlabel     Time (sec)
style ylabel     MISSLE NUMBER
style xprecision  0
style yprecision  0
style curve 1 linecolor lightblue
style curve 2 linecolor orange
style curve 3 linecolor green
style curve 4 linecolor blue
style curve 5 linecolor red
style curve 6 linecolor yellow

style curve 2 pattern 50%
style curve 3 pattern vstripe
style curve 4 pattern hatch
style curve 5 pattern diagonal2
style curve 6 pattern weave

style attachlegend on
style ynozoom on
style width 500
style height 450
style majxgrid on
style majxtick 11

style order c y y y y y y

!NOTE first column are "categories"  --
!they are actually read and stored as STRINGS

2      21     35     12     20     8     4
8      15     22     14     22     7     9
13      16     31     16     21     6     6
18      10     27     18     14     5     10
21       8     19     19     20     6     2
19       3     39     17     15     7     7
12      22     27     15     17     8     0
14      15     13     13     12     7     7
NULL  0 0 0 0 0 0
1       3     18     11     19     7     7
3      0     33     12     25     5     5
4     12     12     14     27     9     7
5     16     10     19     14     5     5
6     12     19     17     17     3     3
7     29     31     20     17     7     10
9     13     37     10     12     4     8
10     10     25      7     23     3     3
11     18     22      0     22     8     8
15     10     26     13     10     0     6
16      6     19     14     15     4     4
17     17     21     15     14     3     9
20      2     22     19     12     8     4
[ENDREC]
