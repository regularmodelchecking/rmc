let
initial = all1 i (t[i] <=> i=0)
idle = all1 i (t[i] <=> t'[i])
a(i) = ex1 j all1 k (~(k = i | k = j) => (t[k] <=> t'[k])) & (j = i+1 & t[i] & ~t'[i] & ~t[j] & t'[j])
sys = alw (idle()|ex1 i a(i)) & initial()
safety = sys() & ~alw~ex1 i ex1 j ~(i = j) & t[i] & t[j]

liveness = sys() &
 (all1 i alw ev (a(i) | ~guard(a(i)))) &
 ~(alw all1 i (ex1 j j < i & t[j]) => ev t[i])
example = sys() & ev ex1 i t[i] & ~(i=0)
in
(
 safety()
)
