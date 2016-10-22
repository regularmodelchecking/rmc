let

pc :: {1, 2, 3, 4, 5, 6}

copy(i) = pc[i] = pc'[i] & (flag[i] <=> flag'[i])
copy_other(i) = (all1 j ~(j=i) => copy(j))
copy_flag(i) = (flag'[i] <=> flag[i])

a1(i) = pc[i] = '1' & pc'[i] = '2' & ~flag'[i]
a2(i) = (pc[i] = '2' & (ex1 j (j < i & flag[j])) & pc'[i] = '1' & copy_flag(i)) |
        (pc[i] = '2' & ~(ex1 j (j < i & flag[j])) & pc'[i] = '3' &  copy_flag(i))
a3(i) = pc[i] = '3' & pc'[i] = '4' & flag'[i]
a4(i) = (pc[i] = '4' & (ex1 j (j < i & flag[j])) & pc'[i] = '1' & copy_flag(i)) |
        (pc[i] = '4' & ~(ex1 j (j < i & flag[j])) & pc'[i] = '5' & copy_flag(i))
a5(i) = pc[i] = '5' & (all1 j (j > i => ~flag[j])) & pc'[i] = '6' & copy_flag(i)
a6(i) = pc[i] = '6' & pc'[i] = '1' & ~flag'[i]

seq1(i) = pc[i] = '2' & pc'[i] = '4' & ~(ex1 j (j < i & flag[j])) & flag'[i]
seq2(i) = pc[i] = '2' & pc'[i] = '5' & ~(ex1 j (j < i & flag[j])) & flag'[i]

a(i) = 
(copy_other(i) & a1(i)) |
(copy_other(i) & a2(i)) |
(copy_other(i) & a3(i)) |
(copy_other(i) & a4(i)) |
(copy_other(i) & a5(i)) |
(copy_other(i) & a6(i))
#(copy_other(i) & seq1(i)) |
#(copy_other(i) & seq2(i)) 

initial = all1 i (pc[i] = '1' & ~flag[i])
idle = all1 i copy(i)
sys = alw (idle()|ex1 i a(i)) & initial()

safety = sys() & ~alw(~(ex1 i ex1 j (~(i = j) & pc[i] = '6' & pc[j] = '6')))

liveness = sys() &
(all1 i alw ev (a(i) | ~guard(a(i)))) &
~(alw all1 i (pc[i] = '1' => ev pc[i] = '6'))

in
(
 safety()
)
