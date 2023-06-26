.data
    user_prompt_1 : .asciiz "Please enter the input string:\n"       #allocating space for the display message 
    user_prompt_2 : .asciiz "The output is:\n"

.text       
    #The main function 
    main:

        # user prompt 1 
        li      $v0, 4                  # syscall to print the string 
        la      $a0, user_prompt_1      # starting address of the string to be stored at $a0 for syscall
        syscall                         

        #allocating heap 
        li      $v0 , 9
        li      $a0 , 1000              #allocating heap memory, the address of heap is stored at $v0
        syscall
        move    $t1 , $v0               # $t1 = address of the input string 

        #taking the input from user
        li		$v0, 8		            # $v0 = 8 i.e preparing for syscall to get user text 
        move    $a0, $t1                # The string is stored at the address pointed by register $a0
        li		$a1, 1000               # $a1 = 20
        syscall                         # syscall to take the input 


        # user prompt 2
        li      $v0, 4                  # syscall to print the string 
        la      $a0, user_prompt_2      # starting address of the string to be stored at $a0 for syscall
        syscall     
        

        # To display the input 
        li		$v0, 4		            # $v0 = 0 i.e. syscall to display string message
        move	$a0, $t1		        # loading the address of the input string in a0
        syscall                         # displaying message
        


        # End of main function 
        li $v0 , 10
        syscall