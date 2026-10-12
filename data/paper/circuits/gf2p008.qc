# GF(2^8) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^8+t^4+t^3+t+1.
# q0..q7: a; q8..q15: b; q16..q23: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 24 Toffoli, 137 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23
BEGIN
cnot q0 q4
cnot q8 q12
cnot q20 q22
cnot q20 q23
tof q4 q12 q20
cnot q1 q4
cnot q9 q12
cnot q18 q21
cnot q18 q22
tof q4 q12 q18
cnot q0 q4
cnot q8 q12
cnot q6 q4
cnot q14 q12
cnot q17 q23
tof q4 q12 q17
cnot q0 q4
cnot q8 q12
cnot q2 q4
cnot q10 q12
cnot q18 q19
tof q4 q12 q18
cnot q7 q0
cnot q15 q8
cnot q17 q16
tof q0 q8 q17
cnot q4 q0
cnot q12 q8
cnot q18 q17
tof q0 q8 q18
cnot q3 q0
cnot q11 q8
cnot q20 q17
cnot q20 q21
tof q0 q8 q20
cnot q5 q0
cnot q13 q8
cnot q18 q20
cnot q18 q23
tof q0 q8 q18
cnot q7 q5
cnot q15 q13
cnot q18 q19
cnot q18 q22
tof q5 q13 q18
cnot q7 q0
cnot q15 q8
cnot q20 q18
tof q0 q8 q20
cnot q1 q4
cnot q9 q12
cnot q3 q4
cnot q11 q12
cnot q20 q16
cnot q20 q22
tof q4 q12 q20
cnot q19 q16
cnot q19 q18
cnot q19 q21
tof q2 q10 q19
cnot q4 q2
cnot q12 q10
cnot q20 q19
tof q2 q10 q20
cnot q6 q4
cnot q14 q12
cnot q18 q16
tof q4 q12 q18
cnot q1 q5
cnot q9 q13
cnot q7 q5
cnot q15 q13
cnot q20 q18
cnot q20 q23
tof q5 q13 q20
cnot q5 q4
cnot q13 q12
cnot q20 q18
tof q4 q12 q20
cnot q3 q0
cnot q11 q8
cnot q7 q0
cnot q15 q8
cnot q23 q21
cnot q23 q22
tof q0 q8 q23
cnot q5 q0
cnot q13 q8
cnot q17 q22
tof q0 q8 q17
cnot q5 q2
cnot q13 q10
cnot q7 q2
cnot q15 q10
cnot q20 q23
tof q2 q10 q20
cnot q2 q0
cnot q10 q8
cnot q20 q17
tof q0 q8 q20
cnot q4 q2
cnot q12 q10
cnot q18 q20
tof q2 q10 q18
cnot q0 q4
cnot q8 q12
cnot q5 q4
cnot q13 q12
cnot q18 q23
tof q4 q12 q18
cnot q1 q0
cnot q9 q8
cnot q6 q0
cnot q14 q8
cnot q7 q0
cnot q15 q8
cnot q20 q21
cnot q20 q23
tof q0 q8 q20
cnot q0 q2
cnot q8 q10
cnot q3 q2
cnot q11 q10
cnot q5 q2
cnot q13 q10
cnot q6 q2
cnot q14 q10
cnot q17 q16
cnot q17 q18
cnot q17 q19
cnot q17 q23
tof q2 q10 q17
cnot q5 q4
cnot q13 q12
cnot q1 q5
cnot q9 q13
cnot q0 q2
cnot q8 q10
cnot q1 q2
cnot q9 q10
cnot q2 q0
cnot q10 q8
cnot q7 q2
cnot q15 q10
cnot q6 q0
cnot q14 q8
cnot q5 q2
cnot q13 q10
cnot q3 q2
cnot q11 q10
cnot q19 q18
cnot q18 q16
cnot q23 q21
cnot q19 q20
cnot q20 q19
cnot q23 q22
cnot q20 q21
cnot q20 q22
cnot q17 q22
cnot q17 q20
cnot q18 q19
END
