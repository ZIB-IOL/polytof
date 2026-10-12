# GF(2^10) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^10+t^3+1.
# q0..q9: a; q10..q19: b; q20..q29: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 33 Toffoli, 260 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26 q27 q28 q29
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26 q27 q28 q29
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26 q27 q28 q29
BEGIN
cnot q3 q2
cnot q13 q12
cnot q8 q2
cnot q18 q12
cnot q26 q20
cnot q26 q24
tof q2 q12 q26
cnot q26 q29
tof q7 q17 q26
cnot q0 q2
cnot q10 q12
cnot q27 q21
cnot q27 q28
tof q2 q12 q27
cnot q3 q2
cnot q13 q12
cnot q7 q2
cnot q17 q12
cnot q26 q20
cnot q26 q28
tof q2 q12 q26
cnot q8 q7
cnot q18 q17
cnot q9 q7
cnot q19 q17
cnot q27 q21
cnot q27 q29
tof q7 q17 q27
cnot q7 q2
cnot q17 q12
cnot q26 q27
tof q2 q12 q26
cnot q8 q1
cnot q18 q11
cnot q27 q20
tof q1 q11 q27
cnot q1 q2
cnot q11 q12
cnot q3 q2
cnot q13 q12
cnot q4 q2
cnot q14 q12
cnot q26 q20
cnot q26 q22
tof q2 q12 q26
cnot q3 q0
cnot q13 q10
cnot q9 q0
cnot q19 q10
cnot q23 q22
cnot q23 q26
tof q0 q10 q23
cnot q0 q2
cnot q10 q12
cnot q26 q23
tof q2 q12 q26
cnot q3 q2
cnot q13 q12
cnot q8 q2
cnot q18 q12
cnot q26 q24
cnot q26 q25
tof q2 q12 q26
cnot q1 q2
cnot q11 q12
cnot q27 q26
tof q2 q12 q27
cnot q7 q0
cnot q17 q10
cnot q9 q0
cnot q19 q10
cnot q23 q26
cnot q23 q28
cnot q23 q29
tof q0 q10 q23
cnot q0 q1
cnot q10 q11
cnot q23 q27
tof q1 q11 q23
cnot q1 q6
cnot q11 q16
cnot q8 q6
cnot q18 q16
cnot q27 q22
cnot q27 q28
tof q6 q16 q27
cnot q2 q6
cnot q12 q16
cnot q3 q6
cnot q13 q16
cnot q4 q6
cnot q14 q16
cnot q23 q20
cnot q23 q21
tof q6 q16 q23
cnot q6 q8
cnot q16 q18
cnot q7 q8
cnot q17 q18
cnot q27 q20
cnot q27 q24
tof q8 q18 q27
cnot q8 q2
cnot q18 q12
cnot q27 q26
tof q2 q12 q27
cnot q1 q2
cnot q11 q12
cnot q23 q20
cnot q23 q21
cnot q23 q27
tof q2 q12 q23
cnot q3 q1
cnot q13 q11
cnot q6 q1
cnot q16 q11
cnot q23 q21
tof q1 q11 q23
cnot q5 q2
cnot q15 q12
cnot q6 q2
cnot q16 q12
cnot q9 q2
cnot q19 q12
cnot q23 q24
cnot q23 q29
tof q2 q12 q23
cnot q7 q2
cnot q17 q12
cnot q8 q2
cnot q18 q12
cnot q28 q22
cnot q28 q25
cnot q28 q26
tof q2 q12 q28
cnot q4 q2
cnot q14 q12
cnot q9 q2
cnot q19 q12
cnot q27 q20
cnot q27 q21
cnot q27 q23
cnot q27 q25
tof q2 q12 q27
cnot q0 q8
cnot q10 q18
cnot q28 q26
cnot q28 q27
cnot q28 q29
tof q8 q18 q28
cnot q2 q0
cnot q12 q10
cnot q5 q0
cnot q15 q10
cnot q29 q20
cnot q29 q28
tof q0 q10 q29
cnot q3 q8
cnot q13 q18
cnot q9 q8
cnot q19 q18
cnot q23 q20
cnot q23 q22
cnot q23 q24
cnot q23 q25
tof q8 q18 q23
cnot q8 q2
cnot q18 q12
cnot q27 q23
tof q2 q12 q27
cnot q1 q8
cnot q11 q18
cnot q5 q8
cnot q15 q18
cnot q6 q8
cnot q16 q18
cnot q26 q25
cnot q26 q27
cnot q26 q29
tof q8 q18 q26
cnot q2 q7
cnot q12 q17
cnot q25 q20
cnot q25 q21
cnot q25 q22
cnot q25 q29
tof q7 q17 q25
cnot q3 q1
cnot q13 q11
cnot q7 q1
cnot q17 q11
cnot q9 q1
cnot q19 q11
cnot q27 q24
cnot q27 q25
cnot q27 q28
cnot q27 q29
tof q1 q11 q27
cnot q1 q6
cnot q11 q16
cnot q29 q22
cnot q29 q23
tof q6 q16 q29
cnot q2 q1
cnot q12 q11
cnot q4 q1
cnot q14 q11
cnot q5 q1
cnot q15 q11
cnot q28 q22
tof q1 q11 q28
cnot q2 q1
cnot q12 q11
cnot q3 q1
cnot q13 q11
cnot q8 q1
cnot q18 q11
cnot q9 q1
cnot q19 q11
cnot q29 q20
cnot q29 q22
cnot q29 q23
cnot q29 q24
cnot q29 q26
tof q1 q11 q29
cnot q4 q1
cnot q14 q11
cnot q2 q7
cnot q12 q17
cnot q6 q8
cnot q16 q18
cnot q0 q1
cnot q10 q11
cnot q0 q6
cnot q10 q16
cnot q1 q2
cnot q11 q12
cnot q6 q2
cnot q16 q12
cnot q2 q6
cnot q12 q16
cnot q6 q8
cnot q16 q18
cnot q7 q8
cnot q17 q18
cnot q9 q7
cnot q19 q17
cnot q8 q7
cnot q18 q17
cnot q8 q6
cnot q18 q16
cnot q7 q6
cnot q17 q16
cnot q3 q2
cnot q13 q12
cnot q6 q1
cnot q16 q11
cnot q2 q1
cnot q12 q11
cnot q6 q0
cnot q16 q10
cnot q9 q1
cnot q19 q11
cnot q8 q0
cnot q18 q10
cnot q7 q2
cnot q17 q12
cnot q5 q2
cnot q15 q12
cnot q29 q25
cnot q25 q23
cnot q26 q21
cnot q28 q20
cnot q29 q22
cnot q23 q20
cnot q23 q21
cnot q26 q24
cnot q27 q23
cnot q28 q23
cnot q27 q24
cnot q25 q28
cnot q28 q25
cnot q29 q25
cnot q29 q26
cnot q27 q28
cnot q28 q27
cnot q28 q29
cnot q26 q28
cnot q26 q27
cnot q25 q29
cnot q23 q25
cnot q23 q27
cnot q23 q26
END
