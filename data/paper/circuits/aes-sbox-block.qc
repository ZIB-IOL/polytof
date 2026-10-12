# 20-qubit XOR AES S-box. Block-structured circuit; Table 4.
# Contract: (x,y,0^4) -> (x,y+S(x),0^4), for arbitrary bytes x,y.
# Input: q0..q7 = x MSB first; q12..q19 = y MSB first; q8..q11 = 0.
# Output: first eight .o wires = x MSB first; next eight = y+S(x) MSB first.
# Wires not listed in .o return to zero; .o includes free output relabelling.
# Toffoli depth: 44 (X and CNOT have zero weight).
# Gates: 52 Toffoli, 186 CNOT, 30 X.
.v q0 q1 q2 q3 q4 q5 q6 q7 q8 q9 q10 q11 q12 q13 q14 q15 q16 q17 q18 q19
.i q0 q1 q2 q3 q4 q5 q6 q7 q12 q13 q14 q15 q16 q17 q18 q19
.o q0 q1 q2 q3 q4 q5 q6 q7 q12 q13 q14 q15 q16 q17 q18 q19
BEGIN
cnot q5 q4
cnot q2 q1
cnot q7 q1
cnot q3 q1
cnot q6 q0
cnot q0 q2
cnot q5 q3
cnot q6 q5
cnot q2 q6
cnot q1 q7
cnot q6 q4
cnot q7 q2
cnot q18 q15
cnot q19 q16
cnot q13 q14
cnot q14 q18
cnot q13 q12
cnot q15 q19
cnot q12 q14
cnot q19 q13
cnot q18 q19
cnot q14 q17
cnot q17 q18
cnot q16 q15
cnot q19 q16
cnot q13 q17
cnot q16 q14
cnot q15 q12
X q4
X q6
cnot q0 q5
X q10
tof q0 q4 q9
cnot q9 q10
X q4
tof q3 q6 q9
cnot q4 q2
X q3
cnot q9 q8
cnot q5 q3
cnot q2 q6
tof q3 q6 q8
X q2
cnot q8 q11
cnot q1 q3
tof q5 q2 q9
X q3
cnot q7 q6
cnot q9 q8
tof q3 q6 q9
cnot q7 q11
X q3
cnot q0 q3
cnot q4 q6
cnot q11 q10
X q2
tof q3 q6 q11
cnot q9 q10
cnot q2 q6
cnot q5 q3
tof q3 q6 q9
cnot q11 q10
cnot q0 q5
cnot q7 q6
cnot q1 q3
cnot q9 q8
X q7
cnot q4 q8
tof q1 q7 q8
tof q3 q6 q9
cnot q8 q11
cnot q4 q6
X q7
X q3
cnot q0 q3
cnot q9 q8
cnot q4 q2
X q6
tof q9 q11 q10
tof q10 q9 q8
cnot q10 q8
tof q8 q11 q9
cnot q9 q11
cnot q11 q10
tof q8 q10 q9
tof q11 q18 q8
tof q9 q10 q18
tof q11 q18 q8
cnot q11 q18
tof q9 q10 q11
cnot q11 q18
cnot q10 q9
cnot q9 q8
tof q4 q11 q16
cnot q16 q19
cnot q11 q9
cnot q4 q2
tof q6 q10 q16
cnot q2 q6
cnot q9 q10
cnot q16 q17
tof q6 q10 q17
tof q2 q9 q16
cnot q17 q18
cnot q7 q6
cnot q8 q10
cnot q16 q17
cnot q18 q19
tof q6 q10 q16
cnot q11 q10
cnot q4 q6
cnot q16 q19
tof q6 q10 q18
cnot q9 q10
cnot q2 q6
cnot q18 q19
tof q6 q10 q16
cnot q4 q2
cnot q8 q10
cnot q7 q6
cnot q16 q17
tof q7 q8 q17
tof q6 q10 q16
cnot q17 q18
cnot q11 q10
cnot q4 q6
cnot q11 q9
cnot q16 q17
cnot q1 q7
cnot q5 q2
cnot q0 q4
cnot q3 q6
tof q4 q11 q12
cnot q12 q15
cnot q11 q9
cnot q4 q2
tof q6 q10 q12
cnot q2 q6
cnot q9 q10
cnot q12 q13
tof q6 q10 q13
tof q2 q9 q12
cnot q13 q14
cnot q7 q6
cnot q8 q10
cnot q12 q13
cnot q14 q15
tof q6 q10 q12
cnot q11 q10
cnot q4 q6
cnot q12 q15
tof q6 q10 q14
cnot q9 q10
cnot q2 q6
cnot q14 q15
tof q6 q10 q12
cnot q4 q2
cnot q8 q10
cnot q7 q6
cnot q12 q13
tof q7 q8 q13
tof q6 q10 q12
cnot q13 q14
cnot q11 q10
cnot q4 q6
cnot q11 q9
cnot q12 q13
cnot q1 q7
cnot q5 q2
cnot q0 q4
cnot q3 q6
cnot q9 q8
cnot q10 q9
cnot q11 q18
tof q9 q10 q11
cnot q11 q18
tof q11 q18 q8
tof q9 q10 q18
tof q11 q18 q8
tof q8 q10 q9
cnot q11 q10
cnot q9 q11
tof q8 q11 q9
cnot q10 q8
tof q10 q9 q8
tof q9 q11 q10
X q6
cnot q4 q2
cnot q9 q8
cnot q0 q3
X q3
X q7
cnot q4 q6
cnot q8 q11
tof q3 q6 q9
tof q1 q7 q8
cnot q4 q8
X q7
cnot q9 q8
cnot q1 q3
cnot q7 q6
cnot q0 q5
cnot q11 q10
tof q3 q6 q9
cnot q5 q3
cnot q2 q6
cnot q9 q10
tof q3 q6 q11
X q2
cnot q11 q10
cnot q4 q6
cnot q0 q3
X q3
cnot q7 q11
tof q3 q6 q9
cnot q9 q8
cnot q7 q6
X q3
tof q5 q2 q9
cnot q1 q3
cnot q8 q11
X q2
tof q3 q6 q8
cnot q2 q6
cnot q5 q3
cnot q9 q8
X q3
cnot q4 q2
tof q3 q6 q9
X q4
cnot q9 q10
tof q0 q4 q9
X q10
cnot q0 q5
X q6
X q4
X q13
X q12
X q14
X q19
cnot q7 q2
cnot q6 q4
cnot q1 q7
cnot q2 q6
cnot q6 q5
cnot q5 q3
cnot q0 q2
cnot q6 q0
cnot q3 q1
cnot q7 q1
cnot q2 q1
cnot q5 q4
cnot q15 q12
cnot q16 q14
cnot q13 q17
cnot q19 q16
cnot q16 q15
cnot q17 q18
cnot q14 q17
cnot q18 q19
cnot q19 q13
cnot q12 q14
cnot q15 q19
cnot q13 q12
cnot q14 q18
cnot q13 q14
cnot q19 q16
cnot q18 q15
END
