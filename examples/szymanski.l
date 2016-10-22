# Run this with --acceleration=tc

let
pc :: {'1','2','3','4','5','6','7'}

copy(i) = pc[i] = pc'[i] & (w[i] <=> w'[i]) & (s[i] <=> s'[i])
copy_w(i) = w[i] <=> w'[i]
copy_s(i) = s[i] <=> s'[i]
copy_other(i) = (all1 j ~(j=i) => copy(j))

a1(i) = pc[i] = '1' & (all1 j (~(j = i) => ~s[j])) & pc'[i] = '2' & copy_w(i) & copy_s(i)
a2(i) = pc[i] = '2' & pc'[i] = '3' & w'[i] & s'[i]
a3(i) = (pc[i] = '3' & (ex1 j (~(j = i) & ~(pc[j] = '1') & ~w[j])) & pc'[i] = '4' & ~s'[i] & copy_w(i)) |
        (pc[i] = '3' & ~(ex1 j (~(j = i) & ~(pc[j] = '1') & ~w[j])) & pc'[i] = '5' & ~w'[i] & copy_s(i))
a4(i) = pc[i] = '4' & (ex1 j (~(j = i) & s[j] & ~w[j])) & pc'[i] = '5' & ~w'[i] & s'[i]
a5(i) = pc[i] = '5' & (all1 j (~(j = i) => ~w[j])) & pc'[i] = '6' & copy_w(i) & copy_s(i)
a6(i) = pc[i] = '6' & (all1 j (j < i => ~s[j])) & pc'[i] = '7' & copy_w(i) & copy_s(i)
a7(i) = pc[i] = '7' & pc'[i] = '1' & ~s'[i] & copy_w(i)

#a(i) = (copy_other(i) & (a1(i) | a2(i) | a3(i) | a4(i) | a5(i) | a6(i) | a7(i)))
a(i) = 
(copy_other(i) & a1(i)) |
(copy_other(i) & a2(i)) |
(copy_other(i) & a3(i)) |
(copy_other(i) & a4(i)) |
(copy_other(i) & a5(i)) |
(copy_other(i) & a6(i)) |
(copy_other(i) & a7(i))

initial = all1 i (pc[i] = '1' & ~s[i] & ~w[i])
idle = all1 i copy(i)
sys = alw (idle()|ex1 i a(i)) & initial()

safety = sys() & ~alw(~(ex1 i ex1 j (~(i = j) & pc[i] = '7' & pc[j] = '7')))

example = sys() & ev ex1 i pc[i] = '7'

liveness = sys() &
(all1 i alw ev (a(i) | ~guard(a(i)))) &
~(alw all1 i (ev pc[i] = '7'))

in
(
safety()
)

