# GF(2^12) multiplication; Table 3.
# Contract: (a,b,c) -> (a,b,c+a*b), for arbitrary a,b,c; no ancillas.
# Polynomial: t^12+t^3+1.
# q0..q11: a; q12..q23: b; q24..q35: c.
# Within each register, increasing wire index means increasing power of t.
# Gates: 42 Toffoli, 308 CNOT, 0 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26 q27 q28 q29 q30 q31 q32 q33 q34 q35
.i q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26 q27 q28 q29 q30 q31 q32 q33 q34 q35
.o q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19 q20 q21 q22 q23 q24 q25 q26 q27 q28 q29 q30 q31 q32 q33 q34 q35
BEGIN
cnot q34 q24
cnot q34 q29
tof q9 q21 q34
cnot q2 q4
cnot q14 q16
cnot q27 q24
cnot q27 q25
cnot q27 q30
cnot q27 q31
tof q4 q16 q27
cnot q8 q1
cnot q20 q13
cnot q28 q27
cnot q28 q30
cnot q28 q33
tof q1 q13 q28
cnot q9 q4
cnot q21 q16
cnot q34 q27
cnot q34 q29
tof q4 q16 q34
cnot q2 q1
cnot q14 q13
cnot q7 q1
cnot q19 q13
cnot q11 q1
cnot q23 q13
cnot q29 q34
tof q1 q13 q29
cnot q1 q4
cnot q13 q16
cnot q29 q34
tof q4 q16 q29
cnot q9 q1
cnot q21 q13
cnot q34 q27
tof q1 q13 q34
cnot q1 q4
cnot q13 q16
cnot q7 q4
cnot q19 q16
cnot q8 q4
cnot q20 q16
cnot q27 q24
cnot q27 q28
tof q4 q16 q27
cnot q0 q3
cnot q12 q15
cnot q34 q26
cnot q34 q30
cnot q34 q31
cnot q34 q35
tof q3 q15 q34
cnot q3 q4
cnot q15 q16
cnot q26 q27
cnot q26 q35
tof q4 q16 q26
cnot q2 q10
cnot q14 q22
cnot q5 q10
cnot q17 q22
cnot q8 q10
cnot q20 q22
cnot q34 q27
tof q10 q22 q34
cnot q10 q3
cnot q22 q15
cnot q26 q34
tof q3 q15 q26
cnot q10 q4
cnot q22 q16
cnot q26 q27
tof q4 q16 q26
cnot q0 q6
cnot q12 q18
cnot q34 q25
cnot q34 q26
cnot q34 q30
cnot q34 q32
cnot q34 q35
tof q6 q18 q34
cnot q2 q1
cnot q14 q13
cnot q9 q1
cnot q21 q13
cnot q10 q1
cnot q22 q13
cnot q34 q28
tof q1 q13 q34
cnot q1 q6
cnot q13 q18
cnot q25 q30
tof q6 q18 q25
cnot q6 q10
cnot q18 q22
cnot q7 q10
cnot q19 q22
cnot q11 q10
cnot q23 q22
cnot q34 q25
tof q10 q22 q34
cnot q1 q10
cnot q13 q22
cnot q28 q25
tof q10 q22 q28
cnot q0 q1
cnot q12 q13
cnot q3 q1
cnot q15 q13
cnot q11 q1
cnot q23 q13
cnot q27 q29
cnot q27 q30
cnot q27 q33
cnot q27 q34
cnot q27 q35
tof q1 q13 q27
cnot q8 q3
cnot q20 q15
cnot q9 q3
cnot q21 q15
cnot q25 q27
cnot q25 q31
cnot q25 q35
tof q3 q15 q25
cnot q1 q0
cnot q13 q12
cnot q5 q0
cnot q17 q12
cnot q6 q0
cnot q18 q12
cnot q27 q25
tof q0 q12 q27
cnot q1 q3
cnot q13 q15
cnot q28 q31
cnot q28 q32
cnot q28 q33
cnot q28 q35
tof q3 q15 q28
cnot q3 q0
cnot q15 q12
cnot q27 q28
tof q0 q12 q27
cnot q1 q0
cnot q13 q12
cnot q25 q27
tof q0 q12 q25
cnot q1 q6
cnot q13 q18
cnot q8 q6
cnot q20 q18
cnot q27 q30
cnot q27 q35
tof q6 q18 q27
cnot q2 q1
cnot q14 q13
cnot q9 q1
cnot q21 q13
cnot q28 q34
cnot q28 q35
tof q1 q13 q28
cnot q6 q1
cnot q18 q13
cnot q34 q24
cnot q34 q27
cnot q34 q29
cnot q34 q30
tof q1 q13 q34
cnot q0 q10
cnot q12 q22
cnot q3 q10
cnot q15 q22
cnot q7 q10
cnot q19 q22
cnot q27 q28
cnot q27 q34
tof q10 q22 q27
cnot q10 q6
cnot q22 q18
cnot q28 q34
tof q6 q18 q28
cnot q1 q6
cnot q13 q18
cnot q27 q34
tof q6 q18 q27
cnot q5 q0
cnot q17 q12
cnot q8 q0
cnot q20 q12
cnot q26 q24
cnot q26 q32
cnot q26 q33
cnot q26 q35
tof q0 q12 q26
cnot q5 q6
cnot q17 q18
cnot q11 q6
cnot q23 q18
cnot q28 q25
cnot q28 q29
cnot q28 q31
cnot q28 q32
cnot q28 q34
cnot q28 q35
tof q6 q18 q28
cnot q1 q3
cnot q13 q15
cnot q4 q3
cnot q16 q15
cnot q8 q3
cnot q20 q15
cnot q27 q26
cnot q27 q28
cnot q27 q30
cnot q27 q31
tof q3 q15 q27
cnot q3 q6
cnot q15 q18
cnot q27 q28
tof q6 q18 q27
cnot q5 q1
cnot q17 q13
cnot q7 q1
cnot q19 q13
cnot q9 q1
cnot q21 q13
cnot q10 q1
cnot q22 q13
cnot q28 q26
cnot q28 q32
cnot q28 q34
tof q1 q13 q28
cnot q0 q1
cnot q12 q13
cnot q26 q28
tof q1 q13 q26
cnot q2 q4
cnot q14 q16
cnot q5 q4
cnot q17 q16
cnot q9 q4
cnot q21 q16
cnot q11 q4
cnot q23 q16
cnot q34 q27
cnot q34 q29
cnot q34 q30
cnot q34 q33
tof q4 q16 q34
cnot q0 q4
cnot q12 q16
cnot q28 q34
tof q4 q16 q28
cnot q4 q1
cnot q16 q13
cnot q34 q26
tof q1 q13 q34
cnot q0 q1
cnot q12 q13
cnot q6 q1
cnot q18 q13
cnot q8 q1
cnot q20 q13
cnot q9 q1
cnot q21 q13
cnot q11 q1
cnot q23 q13
cnot q28 q24
cnot q28 q32
cnot q28 q35
tof q1 q13 q28
cnot q3 q1
cnot q15 q13
cnot q6 q1
cnot q18 q13
cnot q27 q28
tof q1 q13 q27
cnot q6 q1
cnot q18 q13
cnot q27 q29
cnot q27 q30
cnot q27 q32
cnot q27 q33
cnot q27 q34
tof q1 q13 q27
cnot q6 q0
cnot q18 q12
cnot q10 q1
cnot q22 q13
cnot q2 q4
cnot q14 q16
cnot q0 q3
cnot q12 q15
cnot q0 q10
cnot q12 q22
cnot q1 q10
cnot q13 q22
cnot q2 q3
cnot q14 q15
cnot q3 q4
cnot q15 q16
cnot q4 q6
cnot q16 q18
cnot q5 q6
cnot q17 q18
cnot q7 q10
cnot q19 q22
cnot q8 q6
cnot q20 q18
cnot q9 q6
cnot q21 q18
cnot q11 q4
cnot q23 q16
cnot q10 q4
cnot q22 q16
cnot q6 q3
cnot q18 q15
cnot q4 q1
cnot q16 q13
cnot q4 q0
cnot q16 q12
cnot q11 q4
cnot q23 q16
cnot q10 q0
cnot q22 q12
cnot q8 q0
cnot q20 q12
cnot q7 q4
cnot q19 q16
cnot q5 q0
cnot q17 q12
cnot q27 q32
cnot q27 q24
cnot q28 q27
cnot q34 q27
cnot q25 q24
cnot q27 q24
cnot q27 q26
cnot q34 q35
cnot q28 q35
cnot q27 q33
cnot q28 q31
cnot q25 q26
cnot q26 q33
cnot q26 q31
cnot q25 q30
cnot q26 q29
cnot q26 q28
END
