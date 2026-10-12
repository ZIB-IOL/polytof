# GF(2^2) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^2+t+1.
# q0..q1: a; q2..q3: b; q4..q5: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 3 Toffoli, 6 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5
.i q0 q1 q2 q3 q4 q5
.o q0 q1 q2 q3 q4 q5
BEGIN
cnot q5 q4
tof q0 q2 q5
cnot q0 q1
cnot q2 q3
cnot q5 q4
tof q1 q3 q5
cnot q0 q1
cnot q2 q3
tof q1 q3 q4
END
