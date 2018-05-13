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
                                                
BYTECODE(print)										
BYTECODE(ret)											
BYTECODE(call)										
BYTECODE(call_tos)								    
BYTECODE(call_method)							    
BYTECODE(halt)	