# GF(2^5) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^5+t^2+1.
# q0..q4: a; q5..q9: b; q10..q14: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 13 Toffoli, 42 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14
BEGIN
cnot q12 q11
tof q3 q8 q12
cnot q12 q13
tof q4 q9 q12
cnot q2 q4
cnot q7 q9
tof q4 q9 q11
cnot q11 q10
tof q2 q7 q11
cnot q11 q14
tof q1 q6 q11
cnot q1 q4
cnot q6 q9
cnot q12 q11
cnot q12 q13
tof q4 q9 q12
cnot q11 q10
cnot q11 q13
tof q0 q5 q11
cnot q0 q4
cnot q5 q9
cnot q3 q4
cnot q8 q9
cnot q12 q13
tof q4 q9 q12
cnot q2 q4
cnot q7 q9
cnot q12 q14
tof q4 q9 q12
cnot q1 q0
cnot q6 q5
cnot q11 q13
tof q0 q5 q11
cnot q0 q4
cnot q5 q9
cnot q12 q10
cnot q12 q13
tof q4 q9 q12
cnot q1 q0
cnot q6 q5
cnot q2 q0
cnot q7 q5
tof q0 q5 q10
cnot q3 q0
cnot q8 q5
cnot q14 q13
tof q0 q5 q14
cnot q3 q4
cnot q8 q9
cnot q3 q0
cnot q8 q5
cnot q2 q0
cnot q7 q5
cnot q14 q13
cnot q11 q14
END
