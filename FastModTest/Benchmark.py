from custom_divmod_module import custom_divmod
from fast_divmod import DivRest
from timeit import default_timer as timer

TEST_CASE = (3**4000000 + 1, 3**4000000)

def bench_time(f):
    time_arr = []
    for _ in range(100):
        s = timer()
        f(*TEST_CASE)
        e = timer()
        time_arr.append(e-s)
        
    return time_arr

if __name__ == '__main__':

    time_divmod = bench_time(divmod)
    time_fastmod = bench_time(DivRest)
    time_custom_divmod = bench_time(custom_divmod)

    avg_time_divmod = sum(time_divmod)/100
    avg_time_fastmod = sum(time_fastmod)/100    
    avg_time_custom_divmod = sum(time_custom_divmod)/100
    
    print("Default divmod avg time - ", avg_time_divmod)
    print("fastmod avg time - ", avg_time_fastmod)
    print("custom divmod avg time - ", avg_time_custom_divmod)