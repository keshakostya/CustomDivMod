from custom_divmod_module import custom_divmod
import random


def main():
    nums = [(random.randint(2**10000, 2**100000),random.randint(2**50, 2**9999)) for _ in range(100)]
    
    for x,y in nums:
        assert divmod(x,y) == custom_divmod(x,y), f'Failed on {x}, {y}'
        
    print('OK')


if __name__ == '__main__':
    main()