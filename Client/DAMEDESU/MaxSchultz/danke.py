# -*- coding: utf-8 -*-
from z3 import *

p, q = Bools(["p", "q"])
x = Int("x")

s = Solver()

s.add(q == True, p != q)
s.add(x * x - x == 2)

r = s.check()
if r == sat:
	m = s.model()
else:
	print(r)

solution_p = is_true(m[p])
solution_q = is_true(m[q])
solution_x = m[x].as_long()

print(solution_p, solution_q, solution_x)

