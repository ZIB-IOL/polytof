# GF(2^4) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^4+t+1.
# q0..q3: a; q4..q7: b; q8..q11: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 9 Toffoli, 26 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11
BEGIN
tof q2 q6 q9
cnot q9 q10
tof q3 q7 q9
cnot q2 q1
cnot q6 q5
cnot q9 q8
tof q1 q5 q9
cnot q1 q3
cnot q5 q7
tof q3 q7 q8
cnot q8 q11
tof q0 q4 q8
cnot q0 q3
cnot q4 q7
cnot q9 q8
tof q3 q7 q9
cnot q2 q3
cnot q6 q7
cnot q11 q10
tof q3 q7 q11
cnot q1 q3
cnot q5 q7
cnot q9 q10
tof q3 q7 q9
cnot q0 q3
cnot q4 q7
cnot q8 q9
tof q3 q7 q8
cnot q2 q3
cnot q6 q7
cnot q2 q1
cnot q6 q5
cnot q11 q10
cnot q9 q11
cnot q8 q9
END
