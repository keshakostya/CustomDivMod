# -*- coding: utf-8 -*-

def RevIntNuton(N,step):
    while N % 2 == 0: N >>= 1
    if N == 1: return 1
    Nb = N.bit_length() - 1
    x = 3
    p = 4 << Nb
    if x*N > p: x = 2
    t = 1
    for i in range(step): 
        vv = 2*t + 3
        shift = N.bit_length() - vv
        if shift > 0: R = (N >> shift) + 1
        else: R = (N << (-shift))
        x *= 2**(1 + vv + t) - x*R
        t = 2*t
        x >>= x.bit_length() - t - 1
        prod = (x + 1)* R
        p = 1<<(vv+t)
        while prod < p:
            x += 1
            prod += R
            
    if (1 << step) <= Nb:
        prod = (x + 1)* N
        p = 1 << (x.bit_length() + Nb)
        while prod < p:
            prod += N
            x += 1 
    return x

def DivRest(A, B):
    small = 4
    r = A
    for q in range(small):
        if r < B: return (q,r)
        r = r - B
        
    n = A.bit_length()
    m = B.bit_length()
    k = n-m
    if A >= (B << k): k += 1
    
    step = 1
    p = 3
    while p <= k:
        step += 1
        p = 2 * p - 1
        
    revB = RevIntNuton(B,step)
    q = A * revB
    q >>= (q.bit_length() - k)
    r = A - B*q
    j = 0
    while r>=B:
        j+=1
        q+=1
        r-=B
    
    while (r<0):
        q-=1
        r+=B
    return (q,r)
