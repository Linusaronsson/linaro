BYTECODE(nop)
BYTECODE(pop)
BYTECODE(dup)

/* Arithmetic */
BYTECODE(incr)
BYTECODE(decr)
BYTECODE(add)
BYTECODE(sub)
BYTECODE(mod)
BYTECODE(mul)
BYTECODE(div)
BYTECODE(exp)
BYTECODE(neg)

/* Boolean */
BYTECODE(neq)
BYTECODE(eq)
BYTECODE(lt)
BYTECODE(lte)
BYTECODE(gt)
BYTECODE(gte)

/* Logical */
BYTECODE(NOT)

/* Conversions (converts TOS) */
BYTECODE(to_bool)

/* Jumping */
BYTECODE(jmp)
BYTECODE(jmp_true)
BYTECODE(jmp_false)

/* rvalues */
BYTECODE(constant)
BYTECODE(new_obj)
BYTECODE(new_array)
BYTECODE(TRUE)
BYTECODE(FALSE)
BYTECODE(null)

/* Global variable access */
BYTECODE(gload)
BYTECODE(gstore)

/* Local variable access */
BYTECODE(load)
BYTECODE(store)

/* Captured variable access */
BYTECODE(cload)
BYTECODE(cstore)

/* Array access */
BYTECODE(aload)
BYTECODE(astore)

/* Built-in functions */
BYTECODE(print)

/* Return from sub routine */
BYTECODE(ret)

/* Functions calls */
BYTECODE(call)      // Calls argument (index into constant pool)
BYTECODE(call_tos)  // Calls top of operand stack

/* Creates a closure for some function in the const pool */
BYTECODE(closure)

/* Halt execution */
BYTECODE(halt)