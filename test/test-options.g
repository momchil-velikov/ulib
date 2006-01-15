/* XG grammar to generate random sequence of options for test-cmdline.  */

%start option-list ;

option-list :
        option
    |   option-list option
    ;

option :
        a-option
    |   b-option
    |   c-option
    |   d-option
    |   e-option
    |   f-option
    |   g-option
    |   h-option
    |   i-option
    |   j-option
    |   k-option
    |   l-option
    |   m-option
    ;

a-option : -a ;

b-option : --b-long ;

c-option : -c | --c-long ;

d-option : -d | --d-long ;

e-option : -e | --e-long ;

f-option : -f f-arg ;

f-arg : | f-arg-value-1 | f-arg-value-2 ;

g-option : --g-long g-arg ;

g-arg : | g-arg-value-1 | g-arg-value-2 ;

h-option : -h h-arg | --h-long h-arg | --h-long '=' h-arg ;

h-arg : | h-arg-value-1 | h-arg-value-2 ;

i-option : -i i-arg | --i-long i-arg | --i-long '=' i-arg ;

i-arg : | i-arg-value-1 | i-arg-value-2 ;

j-option : -j j-arg ;

j-arg : j-arg-value-1 | j-arg-value-2 ;

k-option : --k-long k-arg | --k-long '=' k-arg ;

k-arg : k-arg-value-1 | k-arg-value-2 ;

l-option : -l l-arg | --l-long l-arg | --l-long '=' l-arg ;

l-arg : l-arg-value-1 | l-arg-value-2 ;

m-option : -m m-arg | --m-long m-arg | --m-long '=' m-arg ;

m-arg : m-arg-value-1 | m-arg-value-2 ;

/* arch-tag: ead5a689-98c1-4f2a-a809-965daf94af81 */
