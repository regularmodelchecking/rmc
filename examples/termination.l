let

t :: {'B','W','N'}

w_set = ex1 k k=0 & w'[k]
w_clear = ex1 k k=0 & ~w'[k]
w_true = ex1 k k=0 & w[k]
w_false = ex1 k k=0 & ~w[k]
copy_w = ex1 k k=0 & (w'[k] <=> w[k])
copy_q(i) = q[i] <=> q'[i]
copy_t(i) = t[i] = t'[i]
copy(i) = copy_q(i) & copy_t(i)
copy_other(i) = all1 j (~(j=i) => copy(j))
copy_other2(i,j) = all1 k (~(k=i | k=j) => copy(k))
one_token = ex1 k all1 l (t[i] = 'N' <=> k = l)
move_token(i,j) = (t[i] = 'B' <=> t'[j] = 'B') & (t[i] = 'W' <=> t'[j] = 'W')
adjacent(i,j) = (j = i+1 | (j = 0 & i = $))

pass(i,j) = ~(i=0) & ~(t[i] = 'N') &
(~q[i] => t'[j] = 'B') & (q[i] => move_token(i, j)) &  t'[i] = 'N' &
 copy_q(i) & copy_q(j) & copy_w() 

start(i,j) = i = 0 & q[i] & (t[i] = 'B' | w_false()) &
t'[i] = 'N' & t'[j] = 'W' & w_set() & 
copy_q(i) & copy_q(j) 

check1(i) = q'[i] & copy_t(i) & copy_w() 

check2(i,j) = ~q[i] & copy(i) &
(j = 0 => w_clear()) & ~q'[j] &
(~(j = 0) => copy_w()) & copy_t(j)

a(i) = 
(ex1 j adjacent(i,j) & copy_other2(i,j) & start(i,j)) |
(ex1 j adjacent(i,j) & copy_other2(i,j) & pass(i,j) & one_token()) |
(ex1 j adjacent(i,j) & copy_other2(i,j) & check2(i,j)) |
(copy_other(i) & check1(i))

initial = (all1 i (i = 0 => t[i] = 'B') & (~(i = 0) => t[i]='N') & ~q[i]) & w_false()
idle = (all1 i copy(i)) & copy_w()
sys = alw(idle() | ex1 i a(i)) & initial()

safety =
sys() & ~alw((ex1 i i = 0 & t[i] = 'W' & w_true()) => all1 i q[i])

example =
sys() & ev ex1 i q[i] 

liveness =
(alw((ex1 i a(i)) | idle())) & initial() &
(all1 i alw ev (a(i) | ~guard(a(i)))) &
~(alw all1 i (pc[i] = '1' => ev pc[i] = '7'))

in
(
 safety()
)
