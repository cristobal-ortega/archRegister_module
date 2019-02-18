# archRegister_module
Expose architectural register to the OS through module.  

Useful when the register is only accessible with privileged instructions.  


/proc/arch_register is created when the module is loaded.  

Output is a representation of the current state of the register.
Representation is at a bit level. Consider the modification of the REG_SIZE definition to fit it to your register size.  
Accepted input is a string that represents bits to be written into the register. This module considers a big-endian architecture.  

