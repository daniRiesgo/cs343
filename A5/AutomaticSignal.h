#define AUTOMATIC_SIGNAL uCondition cond;
#define WAITUNTIL( pred, before, after ) before; while( !pred ) cond.wait(); after; cond.signal();
#define RETURN( ... ) cond.signal(); return __VA_ARGS__;
