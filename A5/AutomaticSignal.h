#define AUTOMATIC_SIGNAL \
{\
    uCondition cond;
}
#define WAITUNTIL( pred, before, after ) \
{\
    before;\
    while( !pred ) cond.wait();\
    after;\
}

#define RETURN( expr... ) \
{\
    cond.signalAll();\
    return __VA_ARGS__;\
}
