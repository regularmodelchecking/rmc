let

pc :: {1, 2, 3, 4, 5, 6}
flag :: {0, 1, 2}

p_set(i) = all1 j (p'[j] <=> j = i)
flagzero_at_p = all1 k (p[k] => flag[k] = '0')

copy(i) = pc[i] = pc'[i] & flag[i] = flag'[i] & (p[i] <=> p'[i])
copy_other(i) = (all1 j ~(j=i) => copy(j))
copy_p = (all1 k (p[k] <=> p'[k]))
copy_flag(i) = flag[i] = flag'[i]

a1(i) = pc[i] = '1' & flag'[i] = '1' & pc'[i] = '2' & copy_p()
a2(i) = (pc[i] = '2' & ~p[i] & copy_flag(i) & flagzero_at_p() & pc'[i] = '3') |
        (pc[i] = '2' & p[i] & pc'[i] = '4' & copy_flag(i) & copy_p())
a3(i) = pc[i] = '3' & p_set(i) & pc'[i] = '4' & copy_flag(i)
a4(i) = pc[i] = '4' & flag'[i] = '2' & pc'[i] = '5' & copy_p()
a5(i) = (pc[i] = '5' & (ex1 j (~(j = i) & flag[j] = '2')) & pc'[i] = '1' & copy_flag(i) & copy_p()) |
        (pc[i] = '5' & ~(ex1 j (~(j = i) & flag[j] = '2')) & pc'[i] = '6' & copy_flag(i) & copy_p())
a6(i) = pc[i] = '6' & flag'[i] = '0' & pc'[i] = '1' & copy_p()

a(i) = 
(copy_other(i) & a1(i)) |
(copy_other(i) & a2(i)) |
(copy_other(i) & a3(i)) |
(copy_other(i) & a4(i)) |
(copy_other(i) & a5(i)) |
(copy_other(i) & a6(i))

initial = all1 i (pc[i] = '1' & flag[i] = '0' & ~p[i])
idle = all1 i copy(i)
sys = alw (idle()|ex1 i a(i)) & initial()

safety = sys() & ~alw(~(ex1 i ex1 j (~(i = j) & pc[i] = '6' & pc[j] = '6')))

liveness =
sys() &
(all1 i alw ev ((a(i) | ~guard(a(i))))) &
~(alw all1 i (ev pc[i] = '6'))

in
(
 safety()
)
