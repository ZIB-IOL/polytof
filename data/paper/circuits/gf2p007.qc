# GF(2^7) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^7+t+1.
# q0..q6: a; q7..q13: b; q14..q20: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 22 Toffoli, 98 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20
BEGIN
cnot q15 q18
cnot q15 q19
tof q1 q8 q15
cnot q18 q14
cnot q18 q17
tof q0 q7 q18
cnot q19 q18
tof q4 q11 q19
cnot q18 q16
cnot q18 q20
tof q2 q9 q18
cnot q18 q15
cnot q18 q17
tof q6 q13 q18
cnot q15 q20
tof q5 q12 q15
cnot q0 q1
cnot q7 q8
cnot q18 q17
tof q1 q8 q18
cnot q6 q4
cnot q13 q11
tof q4 q11 q19
cnot q4 q1
cnot q11 q8
tof q1 q8 q16
cnot q2 q4
cnot q9 q11
cnot q5 q4
cnot q12 q11
cnot q18 q14
cnot q18 q16
tof q4 q11 q18
cnot q6 q5
cnot q13 q12
cnot q15 q19
tof q5 q12 q15
cnot q4 q1
cnot q11 q8
cnot q19 q17
tof q1 q8 q19
cnot q0 q5
cnot q7 q12
cnot q3 q5
cnot q10 q12
cnot q19 q16
tof q5 q12 q19
cnot q5 q4
cnot q12 q11
cnot q18 q16
cnot q18 q19
tof q4 q11 q18
cnot q5 q1
cnot q12 q8
cnot q18 q16
tof q1 q8 q18
cnot q2 q5
cnot q9 q12
cnot q19 q15
cnot q19 q20
tof q5 q12 q19
cnot q3 q1
cnot q10 q8
cnot q4 q1
cnot q11 q8
cnot q15 q14
cnot q15 q17
tof q1 q8 q15
cnot q5 q1
cnot q12 q8
cnot q19 q15
tof q1 q8 q19
cnot q0 q1
cnot q7 q8
cnot q4 q1
cnot q11 q8
cnot q18 q17
cnot q18 q20
tof q1 q8 q18
cnot q3 q4
cnot q10 q11
cnot q6 q4
cnot q13 q11
tof q4 q11 q18
cnot q2 q0
cnot q9 q7
cnot q18 q15
cnot q18 q17
tof q0 q7 q18
cnot q1 q4
cnot q8 q11
cnot q18 q14
cnot q18 q15
cnot q18 q16
tof q4 q11 q18
cnot q0 q5
cnot q7 q12
cnot q0 q4
cnot q7 q11
cnot q1 q4
cnot q8 q11
cnot q3 q5
cnot q10 q12
cnot q6 q5
cnot q13 q12
cnot q6 q4
cnot q13 q11
cnot q5 q1
cnot q12 q8
cnot q3 q1
cnot q10 q8
cnot q2 q0
cnot q9 q7
cnot q18 q15
cnot q18 q17
cnot q19 q16
cnot q19 q17
cnot q19 q18
cnot q18 q20
cnot q15 q20
END
