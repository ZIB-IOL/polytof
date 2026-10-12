# 20-qubit XOR AES S-box. Manually optimized circuit; Table 4.
# Contract: (x,y,0^4) -> (x,y+S(x),0^4), for arbitrary bytes x,y.
# Input: q0..q7 = x MSB first; q12..q19 = y MSB first; q8..q11 = 0.
# Output: first eight .o wires = x MSB first; next eight = y+S(x) MSB first.
# Wires not listed in .o return to zero; .o includes free output relabelling.
# Toffoli depth: 24 (X and CNOT have zero weight).
# Gates: 52 Toffoli, 184 CNOT, 43 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19
.i q0 q1 q2 q3 q4 q5 q6 q7 q12 q13 q14 q15 q16 q17 q18 q19
.o q0 q6 q7 q5 q4 q1 q3 q2 q16 q13 q12 q15 q18 q17 q19 q14
BEGIN
X q11
cnot q2 q1
cnot q4 q2
cnot q5 q4
cnot q6 q5
cnot q11 q9
cnot q10 q11
cnot q8 q9
cnot q9 q10
cnot q4 q9
cnot q0 q5
cnot q1 q7
cnot q4 q1
X q4
cnot q3 q1
cnot q5 q2
cnot q7 q0
cnot q5 q3
cnot q7 q5
cnot q6 q7
tof q3 q4 q8
X q1
X q4
tof q5 q1 q11
X q0
X q1
X q2
tof q0 q2 q10
X q0
X q2
X q6
tof q7 q6 q9
X q8
cnot q6 q2
cnot q1 q6
cnot q7 q0
cnot q6 q8
cnot q8 q11
cnot q5 q7
cnot q3 q5
X q3
cnot q4 q1
cnot q2 q4
cnot q11 q9
cnot q9 q10
cnot q10 q8
cnot q0 q3
tof q3 q4 q11
X q3
X q4
tof q7 q6 q10
X q1
tof q5 q1 q8
X q1
tof q0 q2 q9
X q2
cnot q10 q11
cnot q11 q9
cnot q8 q10
cnot q3 q7
cnot q4 q6
tof q7 q6 q10
X q6
cnot q1 q6
cnot q2 q6
cnot q0 q6
cnot q3 q0
cnot q7 q3
cnot q5 q1
cnot q5 q7
cnot q4 q2
cnot q0 q2
tof q9 q8 q11
X q9
tof q11 q9 q10
X q9
tof q10 q8 q9
cnot q9 q8
cnot q8 q11
tof q10 q11 q9
tof q8 q19 q10
tof q9 q11 q19
tof q8 q19 q10
cnot q8 q19
cnot q1 q5
cnot q9 q10
cnot q11 q10
tof q9 q11 q8
tof q5 q10 q17
cnot q1 q3
cnot q4 q3
cnot q5 q3
cnot q2 q0
cnot q10 q9
cnot q11 q10
cnot q13 q12
cnot q18 q19
cnot q14 q17
cnot q13 q17
cnot q13 q16
cnot q13 q15
cnot q6 q2
cnot q8 q10
cnot q7 q3
cnot q3 q2
cnot q0 q5
cnot q19 q16
cnot q19 q15
cnot q9 q8
cnot q2 q0
cnot q4 q0
tof q3 q11 q19
tof q5 q10 q13
tof q2 q8 q14
tof q0 q9 q18
cnot q9 q8
cnot q0 q5
cnot q9 q10
cnot q13 q18
cnot q18 q14
cnot q14 q17
cnot q7 q6
cnot q2 q0
cnot q12 q17
cnot q12 q19
tof q4 q11 q13
tof q5 q10 q14
tof q0 q8 q18
tof q6 q9 q12
cnot q7 q2
cnot q16 q18
cnot q4 q0
cnot q11 q10
cnot q8 q11
cnot q2 q6
cnot q2 q1
cnot q12 q16
cnot q13 q14
cnot q13 q18
cnot q14 q15
cnot q14 q16
cnot q8 q9
tof q1 q10 q14
tof q0 q11 q13
tof q6 q8 q12
tof q2 q9 q16
cnot q3 q6
cnot q1 q3
cnot q13 q18
cnot q5 q4
cnot q16 q12
cnot q16 q19
cnot q19 q14
cnot q10 q8
cnot q11 q8
cnot q12 q14
cnot q10 q9
cnot q11 q9
cnot q6 q2
cnot q1 q2
tof q4 q10 q13
tof q3 q8 q19
tof q6 q11 q12
tof q2 q9 q16
X q14
X q18
cnot q6 q0
cnot q4 q1
cnot q5 q4
cnot q11 q8
cnot q11 q9
cnot q13 q12
X q13
cnot q14 q15
cnot q13 q15
cnot q12 q16
cnot q8 q10
cnot q10 q11
cnot q7 q1
cnot q3 q5
cnot q18 q12
cnot q14 q18
cnot q2 q3
cnot q6 q2
cnot q16 q17
cnot q18 q19
cnot q19 q17
cnot q13 q19
cnot q7 q3
tof q8 q11 q10
tof q2 q9 q12
cnot q12 q14
cnot q12 q18
cnot q11 q9
cnot q8 q9
cnot q10 q14
tof q10 q14 q9
tof q8 q11 q14
tof q10 q14 q9
tof q9 q11 q8
cnot q10 q11
cnot q8 q10
tof q9 q10 q8
X q8
tof q11 q8 q9
X q8
tof q8 q10 q11
X q3
X q10
cnot q0 q6
cnot q4 q6
cnot q1 q7
cnot q1 q2
cnot q7 q5
cnot q5 q0
cnot q0 q3
cnot q6 q3
cnot q2 q3
tof q7 q3 q9
X q6
cnot q4 q3
cnot q5 q7
cnot q10 q9
cnot q11 q8
cnot q9 q11
tof q0 q6 q8
X q2
X q6
tof q1 q2 q10
X q2
tof q7 q3 q9
X q3
X q4
X q5
tof q5 q4 q11
X q0
cnot q6 q4
cnot q9 q10
cnot q8 q9
X q8
cnot q0 q5
cnot q4 q8
cnot q4 q2
cnot q11 q8
cnot q10 q11
cnot q3 q10
cnot q5 q1
cnot q1 q7
cnot q7 q0
cnot q2 q3
cnot q3 q6
tof q7 q3 q8
X q3
tof q0 q6 q9
X q0
X q6
tof q1 q2 q11
X q2
tof q5 q4 q10
X q4
X q14
X q18
cnot q8 q9
cnot q10 q8
cnot q9 q11
cnot q11 q8
cnot q1 q5
cnot q5 q2
cnot q4 q2
cnot q1 q6
cnot q3 q7
cnot q7 q5
cnot q3 q1
cnot q0 q1
cnot q1 q4
cnot q4 q6
cnot q7 q0
cnot q6 q7
cnot q2 q6
END
