# GF(2^9) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^9+t+1.
# q0..q8: a; q9..q17: b; q18..q26: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 30 Toffoli, 185 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26
BEGIN
cnot q8 q5
cnot q17 q14
cnot q19 q21
cnot q19 q25
tof q5 q14 q19
cnot q3 q0
cnot q12 q9
cnot q8 q0
cnot q17 q9
cnot q22 q18
cnot q22 q21
cnot q22 q23
tof q0 q9 q22
cnot q2 q0
cnot q11 q9
cnot q18 q20
cnot q18 q23
tof q0 q9 q18
cnot q0 q5
cnot q9 q14
cnot q18 q26
tof q5 q14 q18
cnot q8 q0
cnot q17 q9
cnot q19 q18
cnot q19 q20
tof q0 q9 q19
cnot q3 q4
cnot q12 q13
cnot q8 q4
cnot q17 q13
cnot q18 q22
cnot q18 q23
tof q4 q13 q18
cnot q2 q6
cnot q11 q15
cnot q3 q6
cnot q12 q15
cnot q18 q19
tof q6 q15 q18
cnot q0 q4
cnot q9 q13
cnot q19 q20
cnot q19 q25
cnot q19 q26
tof q4 q13 q19
cnot q6 q4
cnot q15 q13
tof q4 q13 q18
cnot q2 q0
cnot q11 q9
cnot q23 q19
cnot q23 q21
cnot q23 q22
cnot q23 q24
cnot q23 q26
tof q0 q9 q23
cnot q3 q4
cnot q12 q13
cnot q23 q21
cnot q23 q25
tof q4 q13 q23
cnot q0 q5
cnot q9 q14
cnot q7 q5
cnot q16 q14
cnot q23 q22
tof q5 q14 q23
cnot q4 q5
cnot q13 q14
cnot q18 q21
cnot q18 q22
tof q5 q14 q18
cnot q0 q5
cnot q9 q14
cnot q8 q5
cnot q17 q14
cnot q22 q18
tof q5 q14 q22
cnot q5 q4
cnot q14 q13
cnot q22 q23
tof q4 q13 q22
cnot q1 q5
cnot q10 q14
cnot q24 q18
cnot q24 q23
cnot q24 q26
tof q5 q14 q24
cnot q5 q0
cnot q14 q9
cnot q22 q25
cnot q22 q26
tof q0 q9 q22
cnot q0 q6
cnot q9 q15
cnot q7 q6
cnot q16 q15
cnot q18 q21
cnot q18 q24
cnot q18 q26
tof q6 q15 q18
cnot q6 q5
cnot q15 q14
cnot q18 q22
tof q5 q14 q18
cnot q0 q5
cnot q9 q14
cnot q22 q24
tof q5 q14 q22
cnot q3 q5
cnot q12 q14
cnot q7 q5
cnot q16 q14
cnot q19 q25
tof q5 q14 q19
cnot q0 q4
cnot q9 q13
cnot q7 q4
cnot q16 q13
cnot q24 q18
cnot q24 q20
cnot q24 q23
tof q4 q13 q24
cnot q4 q0
cnot q13 q9
cnot q5 q0
cnot q14 q9
cnot q8 q0
cnot q17 q9
tof q0 q9 q19
cnot q6 q0
cnot q15 q9
cnot q8 q0
cnot q17 q9
cnot q23 q24
cnot q23 q25
tof q0 q9 q23
cnot q1 q5
cnot q10 q14
cnot q2 q5
cnot q11 q14
cnot q6 q5
cnot q15 q14
cnot q19 q21
cnot q19 q22
tof q5 q14 q19
cnot q0 q5
cnot q9 q14
cnot q23 q19
tof q5 q14 q23
cnot q2 q0
cnot q11 q9
cnot q7 q0
cnot q16 q9
cnot q8 q0
cnot q17 q9
tof q0 q9 q24
cnot q6 q5
cnot q15 q14
cnot q7 q5
cnot q16 q14
cnot q19 q24
cnot q19 q25
cnot q19 q26
tof q5 q14 q19
cnot q0 q5
cnot q9 q14
cnot q4 q5
cnot q13 q14
cnot q24 q19
tof q5 q14 q24
cnot q0 q4
cnot q9 q13
cnot q19 q22
cnot q19 q25
tof q4 q13 q19
cnot q5 q4
cnot q14 q13
cnot q1 q5
cnot q10 q14
cnot q6 q0
cnot q15 q9
cnot q0 q6
cnot q9 q15
cnot q3 q6
cnot q12 q15
cnot q5 q6
cnot q14 q15
cnot q8 q4
cnot q17 q13
cnot q7 q4
cnot q16 q13
cnot q6 q5
cnot q15 q14
cnot q1 q0
cnot q10 q9
cnot q4 q0
cnot q13 q9
cnot q22 q18
cnot q23 q20
cnot q24 q22
cnot q22 q19
cnot q22 q20
cnot q22 q21
cnot q24 q22
cnot q24 q25
cnot q23 q26
cnot q22 q23
cnot q23 q25
cnot q18 q20
cnot q18 q19
cnot q19 q26
cnot q18 q24
cnot q19 q22
END
