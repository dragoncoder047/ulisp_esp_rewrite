
#define nil             NULL
#define car(x)          ((x)->car)
#define cdr(x)          ((x)->cdr)

#define first(x)        car(x)
#define rest(x)         cdr(x)
#define cadr(x)         first(rest(x))
#define second(x)       cadr(x)
#define cddr(x)         rest(rest(x))
#define third(x)        first(cddr(x))
#define fourth(x)       first(rest(cddr(x)))

#define push(x, y)      ((y) = cons((x), (y)))
#define pop(y)          ((y) = cdr(y))

#define protect(y)      push((y), GCStack)
#define unprotect()     pop(GCStack)

#define integerp(x)     isinstance(x, &integer_type)
#define floatp(x)       isinstance(x, &float_type)
#define symbolp(x)      isinstance(x, &symbol_type)
#define stringp(x)      isinstance(x, &string_type)
#define characterp(x)   isinstance(x, &character_type)
#define arrayp(x)       isinstance(x, &fastarray_type)
#define streamp(x)      isinstance(x, &stream_type)

#define mark(x)         (car(x) = (object*)(((uintptr_t)(car(x))) | MARKBIT))
#define unmark(x)       (car(x) = (object*)(((uintptr_t)(car(x))) & ~MARKBIT))
#define unmarked(x)     (car(x) & (object*)(uintptr_t)(~MARKBIT))
#define marked(x)       ((((uintptr_t)(car(x))) & MARKBIT) > 0)
#define MARKBIT         1
