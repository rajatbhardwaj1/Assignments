.data
        user_prompt_1 : .asciiz "Enter the value of x: "
        user_prompt_2 : .asciiz "Enter the value of n: "
        result_is    :  .asciiz "The result is: "


.text
main:

                # Prompting user to enter the value of x:
                li      $v0, 4                  # syscall to print the string 
                la      $a0, user_prompt_1      
                syscall                         # printing "enter value of x"


                # Taking the input x
                li      $v0, 5		        # syscall for integer input
                syscall
                move    $s0, $v0                # $s0 = x


                # Prompting user to enter the value of n:
                li      $v0, 4                  # syscall to print the string 
                la      $a0, user_prompt_2      
                syscall                 


                # Taking the input n
                li      $v0, 5		        # syscall of integer input 
                syscall
                move    $s1, $v0                # $s1 = n
                


                # The main function begins here
                # $s0 will have the value for current x  
                # $s1 will have the valud for current n
                # $s2 will have the value of result


                la      $ra , printres          # before the start of function we load the address of the printing function 
                                                # where we should return after executing all the recursive calls 
                # calling FastPow(x , n) -> FastPow($s0 , $s1) 
FastPow:        
                beq     $s1, $zero, n_is_0
                addi    $sp ,$sp , -8           # making space for n and x in stack pointer 
                sw      $ra , 4($sp)            # return address is stored at the address $sp + 4
                sw      $s1 , 0($sp)            # n is stored at the address $sp
                div     $s1 , $s1 , 2                 # n = n / 2 , arguments for recursive call 
                jal     FastPow                 # Calling FastPow(x, n/2)
                
                # We will recieve output at register #s2 , i.e. $s2 = FastPow(x , n/2) 

                mul     $s2 , $s2 , $s2         # $s2 = $s2*$s2 i.e. result = pow(x , n/2) * pow(x , n/2) 
                lw      $s1 , 0($sp)            # extracting n from the stack
                lw      $ra , 4($sp)            # extracting return address from the stack
                addi    $sp , $sp , 8
                andi    $t1 , $s1 , 1           # check if n is odd
                bne     $t1 , $zero , mul_x     # if n is odd we multiply the return value with x 
                li      $t1 , 1                 # if n is even we just set $t1 = 1 
continue:       mul     $s2 , $s2 , $t1         # result = pow(x , n/2) * pow(x , n/2) *(n is odd ? x:1)
                jr      $ra                     
                j       printres


mul_x  :        move    $t1 , $s0               # moving x in t1 to be multiplied with the answer
                j       continue

n_is_0:         li      $s2, 1                  # base case, if n == 0 we just return 0 in $s2 
                jr      $ra 



printres:            
                li      $v0, 4                  # syscall to print the string 
                la      $a0,   result_is          
                syscall                         # printing  Yes at index 
                move    $a0 , $s2               # moving result to $a0 for syscall to print the int
                li      $v0,  1                 # syscall to print integer 
                syscall
                

end_function:
                # End of main function 
                li $v0 , 10
                syscall