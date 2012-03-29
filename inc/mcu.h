#ifndef _MCU_H_
#define _MCU_H_

// See Chapter 6.9 for why this is important
// Clock is prescaled by 2^{-a}
#define SET_CLKPR(a) \
    do{ \
        CLKPR=1<<CLKPCE; \
        CLKPR=a; \
    }while(0)


#endif /* _MCU_H_ */
