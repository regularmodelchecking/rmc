let
q :: {'O','W','C'}

copy(i) = q[i] = q'[i]
copy_other(i) = all1 j ~(j=i) => copy(j)

a1(i) = 
     q[i] = 'O' & q'[i] = 'W' &
     (all1 j j > i => q[j]= 'O') &
     (i = 0 | (all1 j i = j+1 => (q[j]='W'|q[j]='C')))

a2(i) = q[i] = 'W' & q'[i] = 'C' & (all1 j j < i => q[j] = 'O') 

a3(i) = q[i] = 'C' & q'[i] = 'O'

seq(i) = q[i] = 'W' & q'[i] = 'O' & (all1 j j < i => q[j] = 'O') 


a(i) = 
(copy_other(i) & a1(i)) |
(copy_other(i) & a2(i)) |
(copy_other(i) & a3(i))
#| (copy_other(i) & seq(i))

initial = all1 i q[i] = 'O'    
idle = all1 i copy(i)

safety = alw(ex1 i a(i)|idle()) & initial() & ~alw(~(ex1 i ex1 j (~(i = j) & q[i] = 'C' & q[j] = 'C')))
liveness =
 (alw((ex1 i a(i)) | idle())) & initial() &
 (all1 i alw ev (a(i) | ~guard(a(i)))) &
 ~(alw all1 i (q[i] = 'W' => ev q[i] = 'C'))

in
(
 safety()
)
