# GF(2^6) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^6+t+1.
# q0..q5: a; q6..q11: b; q12..q17: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 15 Toffoli, 70 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17
BEGIN
cnot q15 q12
cnot q15 q17
tof q3 q9 q15
cnot q0 q2
cnot q6 q8
cnot q14 q15
tof q2 q8 q14
cnot q3 q2
cnot q9 q8
cnot q14 q15
tof q2 q8 q14
cnot q0 q2
cnot q6 q8
cnot q15 q12
cnot q15 q16
tof q2 q8 q15
cnot q15 q13
tof q5 q11 q15
cnot q4 q3
cnot q10 q9
cnot q14 q13
tof q3 q9 q14
cnot q1 q2
cnot q7 q8
cnot q14 q15
cnot q14 q17
tof q2 q8 q14
cnot q2 q3
cnot q8 q9
cnot q15 q17
tof q3 q9 q15
cnot q0 q3
cnot q6 q9
tof q3 q9 q12
cnot q5 q3
cnot q11 q9
cnot q15 q12
cnot q15 q17
tof q3 q9 q15
cnot q1 q3
cnot q7 q9
cnot q17 q12
cnot q17 q13
tof q3 q9 q17
cnot q0 q4
cnot q6 q10
cnot q15 q12
cnot q15 q16
cnot q15 q17
tof q4 q10 q15
cnot q4 q3
cnot q10 q9
cnot q17 q15
tof q3 q9 q17
cnot q0 q3
cnot q6 q9
cnot q1 q3
cnot q7 q9
cnot q14 q13
cnot q14 q17
tof q3 q9 q14
cnot q1 q3
cnot q7 q9
cnot q2 q3
cnot q8 q9
cnot q14 q13
cnot q14 q16
cnot q14 q17
tof q3 q9 q14
cnot q1 q2
cnot q7 q8
cnot q0 q4
cnot q6 q10
cnot q0 q3
cnot q6 q9
cnot q1 q3
cnot q7 q9
cnot q5 q3
cnot q11 q9
cnot q3 q2
cnot q9 q8
cnot q15 q12
cnot q17 q13
cnot q17 q15
cnot q14 q15
END
