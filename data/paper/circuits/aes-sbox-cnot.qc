# 20-qubit XOR AES S-box. Manually optimized circuit; Table 4.
# Contract: (x,y,0^4) -> (x,y+S(x),0^4), for arbitrary bytes x,y.
# Input: q0..q7 = x MSB first; q12..q19 = y MSB first; q8..q11 = 0.
# Output: first eight .o wires = x MSB first; next eight = y+S(x) MSB first.
# Wires not listed in .o return to zero; .o includes free output relabelling.
# Toffoli depth: 47 (X and CNOT have zero weight).
# Gates: 52 Toffoli, 147 CNOT, 35 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19
.i q0 q1 q2 q3 q4 q5 q6 q7 q12 q13 q14 q15 q16 q17 q18 q19
.o q0 q1 q2 q3 q4 q5 q6 q7 q12 q13 q14 q15 q16 q17 q18 q19
BEGIN
X q1
X q6
X q8
cnot q4 q9
cnot q2 q1
cnot q7 q1
cnot q10 q11
X q10
tof q6 q1 q10
X q1
cnot q2 q4
cnot q5 q4
cnot q6 q4
X q6
cnot q0 q6
tof q6 q4 q9
cnot q0 q1
cnot q0 q4
cnot q10 q9
tof q1 q4 q10
X q2
cnot q3 q5
cnot q0 q2
cnot q10 q11
tof q5 q2 q10
X q2
X q3
cnot q0 q3
cnot q1 q4
cnot q7 q4
tof q3 q4 q8
X q3
X q4
cnot q2 q4
cnot q5 q4
cnot q4 q9
cnot q3 q5
cnot q10 q8
tof q5 q4 q10
cnot q1 q3
cnot q8 q11
cnot q2 q8
tof q3 q7 q8
cnot q1 q6
cnot q5 q6
cnot q0 q4
cnot q1 q4
cnot q9 q11
tof q6 q4 q9
cnot q3 q6
cnot q7 q4
cnot q9 q8
tof q6 q4 q9
cnot q10 q9
cnot q8 q11
tof q9 q11 q10
X q9
X q10
tof q10 q9 q8
X q8
X q9
X q10
tof q8 q11 q9
cnot q9 q11
cnot q11 q10
tof q8 q10 q9
tof q11 q19 q8
tof q9 q10 q19
tof q11 q19 q8
cnot q11 q19
tof q9 q10 q11
cnot q7 q3
cnot q8 q9
cnot q10 q9
tof q3 q9 q17
cnot q16 q14
cnot q16 q19
tof q7 q9 q16
cnot q10 q11
cnot q16 q12
cnot q16 q17
tof q4 q11 q16
cnot q1 q0
cnot q10 q8
cnot q16 q14
cnot q16 q15
tof q0 q8 q16
cnot q4 q6
cnot q16 q13
cnot q16 q17
cnot q16 q19
tof q6 q11 q16
cnot q3 q6
cnot q9 q11
cnot q16 q17
tof q6 q11 q16
cnot q7 q4
cnot q12 q17
tof q4 q11 q12
cnot q0 q5
cnot q4 q5
cnot q8 q11
tof q5 q11 q14
cnot q5 q6
cnot q19 q18
tof q6 q8 q19
cnot q16 q19
tof q2 q10 q16
cnot q0 q4
cnot q18 q17
tof q4 q11 q18
cnot q2 q5
cnot q10 q11
cnot q16 q14
tof q5 q11 q16
cnot q3 q5
cnot q9 q11
cnot q17 q14
tof q5 q11 q17
cnot q1 q5
cnot q7 q5
cnot q11 q9
cnot q16 q12
cnot q16 q13
cnot q16 q17
tof q5 q9 q16
cnot q4 q5
cnot q18 q16
tof q5 q10 q18
cnot q5 q4
cnot q7 q4
cnot q16 q14
cnot q16 q15
tof q4 q11 q16
cnot q1 q6
cnot q4 q6
cnot q8 q11
cnot q19 q17
tof q6 q11 q19
cnot q0 q4
cnot q18 q12
cnot q18 q16
tof q4 q11 q18
X q13
X q19
cnot q1 q0
cnot q7 q3
cnot q5 q4
cnot q10 q8
cnot q11 q9
cnot q18 q12
cnot q18 q15
cnot q16 q19
cnot q3 q5
cnot q4 q6
cnot q2 q5
cnot q1 q5
cnot q2 q6
cnot q19 q18
cnot q18 q17
cnot q17 q14
X q17
cnot q12 q17
tof q9 q10 q11
cnot q11 q19
tof q11 q19 q8
tof q9 q10 q19
tof q11 q19 q8
tof q8 q10 q9
cnot q11 q10
cnot q9 q11
tof q8 q11 q9
X q8
X q9
X q10
tof q10 q9 q8
X q9
X q10
tof q9 q11 q10
cnot q8 q11
cnot q10 q9
tof q6 q4 q9
cnot q9 q8
cnot q7 q4
cnot q3 q6
tof q6 q4 q9
cnot q9 q11
cnot q1 q4
cnot q0 q4
cnot q5 q6
cnot q1 q6
tof q3 q7 q8
cnot q2 q8
cnot q8 q11
cnot q1 q3
tof q5 q4 q10
X q3
cnot q10 q8
cnot q3 q5
cnot q4 q9
cnot q5 q4
X q5
cnot q2 q4
tof q3 q4 q8
X q2
X q3
cnot q7 q4
cnot q1 q4
cnot q0 q3
tof q5 q2 q10
X q2
cnot q10 q11
cnot q0 q2
cnot q3 q5
tof q1 q4 q10
cnot q10 q9
cnot q0 q4
cnot q0 q1
tof q6 q4 q9
X q1
X q6
cnot q0 q6
cnot q6 q4
cnot q5 q4
cnot q2 q4
tof q6 q1 q10
X q1
X q6
X q8
X q10
cnot q10 q11
cnot q7 q1
cnot q2 q1
cnot q4 q9
END
