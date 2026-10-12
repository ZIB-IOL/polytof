# GF(2^3) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^3+t+1.
# q0..q2: a; q3..q5: b; q6..q8: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 6 Toffoli, 16 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8
.i q0 q1 q2 q3 q4 q5 q6 q7 q8
.o q0 q1 q2 q3 q4 q5 q6 q7 q8
BEGIN
tof q2 q5 q8
tof q0 q3 q7
cnot q1 q2
cnot q4 q5
cnot q8 q7
tof q2 q5 q8
cnot q1 q0
cnot q4 q3
cnot q8 q6
tof q0 q3 q8
cnot q0 q2
cnot q3 q5
tof q2 q5 q6
cnot q1 q2
cnot q4 q5
cnot q8 q7
tof q2 q5 q8
cnot q0 q2
cnot q3 q5
cnot q1 q0
cnot q4 q3
cnot q8 q6
END
