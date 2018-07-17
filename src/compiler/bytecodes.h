BYTECODE(nop)
BYTECODE(pop)											
BYTECODE(dup)											
                                                
/* Arithmetic */								
BYTECODE(add)                                         
BYTECODE(sub)										    
BYTECODE(mod)								        
BYTECODE(mul)											
BYTECODE(div)									    
BYTECODE(exp)									   
                                                
/* Boolean */								   
BYTECODE(neq)											
BYTECODE(eq)											
BYTECODE(lt)											
BYTECODE(lte)											
BYTECODE(gt)											
BYTECODE(gte)											

/* Logical */                                   
BYTECODE(OR)						
BYTECODE(AND)										    
BYTECODE(NOT)								    
                                                
/* Conversions (converts TOS) */								
BYTECODE(to_bool) 	        
                                                
/* Jumping */
BYTECODE(jmp)
BYTECODE(jmp_true)									
BYTECODE(jmp_false)									
                                                
BYTECODE(neg)										
                                                
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

/* Built-in functions */                             
BYTECODE(print)		

/* Return from sub routine */
BYTECODE(ret)		

/* Functions calls */
BYTECODE(call) //Calls argument (index into constant pool) 									
BYTECODE(call_tos) //Calls top of operand stack						    
BYTECODE(call_method) //Calls method belonging to class descriptor. (Instance of the class must be on TOS)  

/* Halt execution */
BYTECODE(halt)	